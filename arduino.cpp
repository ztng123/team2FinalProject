#include <wiringSerial.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <mariadb/mysql.h>
#include <ctime>
#include <cmath>
#include <dirent.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

#include <filesystem> // C++17 이상
namespace fs = std::filesystem;

std::string findSerialPort()
{
    for (const auto &entry : fs::directory_iterator("/dev"))
    {
        std::string name = entry.path().string();
        if (name.find("ttyACM") != std::string::npos || name.find("ttyUSB") != std::string::npos)
        {
            int test_port = serialOpen(name.c_str(), 9600);
            if (test_port >= 0)
            {
                serialClose(test_port);
                return name;
            }
        }
    }
    return "";
}

// 제동거리 삭제
// float computeBraking(float speed_kmh) {
//     float v = speed_kmh / 3.6;
//     float g = 9.81;
//     float distance = (v * v) / (2 * 0.75 * g);
//     return distance * 100.0f + 5.0f; // m → cm
// }

float getSpeedFromDistances(float d1, float d2, float delta_time_sec = 0.2f)
{
    float delta_m = (d1 - d2) / 100.0f;
    float speed_mps = delta_m / delta_time_sec;
    float speed_kph = std::fabs(speed_mps * 3.6);
    return speed_kph;
}

void insertToDatabase(MYSQL *conn, float distance_cm, float braking_cm, bool emergency, float speed_kph)
{
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO pedestrian_detection_log (detection_time, distance_meters, braking_distance, emergency_brake, speed_kmph) "
             "VALUES (NOW(), %.2f, %.2f, %d, %.2f)",
             distance_cm / 100.0f,
             braking_cm / 100.0f,
             emergency ? 1 : 0,
             speed_kph * 10);

    if (mysql_query(conn, query))
    {
        std::cerr << "❌ MySQL 오류: " << mysql_error(conn) << std::endl;
    }
    else
    {
        std::cout << "🚀 MySQL Insert Success" << std::endl;
    }
}

void checkDrowsinessWarning(MYSQL *conn, int serial_port, std::string &last_detected_time)
{
    const char *query =
        "SELECT id, warning_level FROM driver_drown_logs "
        "ORDER BY id DESC LIMIT 1";

    if (mysql_query(conn, query))
    {
        cerr << "❌ 쿼리 실패: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (!result)
    {
        cerr << "❌ 결과 수신 실패" << endl;
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0] && row[1])
    {
        string detection_time = row[0];
        int warning_level = atoi(row[1]);
        if (warning_level == 2 && detection_time != last_detected_time)
        {
            cout << "🚨 졸음 감지됨: " << detection_time << " → 아두이노로 경고 전송" << endl;
            serialPuts(serial_port, "D\n");
            serialFlush(serial_port);
            last_detected_time = detection_time;
        }

        mysql_free_result(result);
    }
}

float getSingleDistance(int serial_port)
{
    serialPuts(serial_port, "R\n");
    serialFlush(serial_port);
    usleep(10000);

    char buf[64];
    int idx = 0;
    const int timeout_ms = 1000;
    int waited = 0;

    while (waited < timeout_ms)
    {
        if (serialDataAvail(serial_port))
        {
            char c = serialGetchar(serial_port);
            if (c == '\n')
                break;
            if (idx < sizeof(buf) - 1)
                buf[idx++] = c;
        }
        else
        {
            usleep(10000);
            waited += 10;
        }
    }

    buf[idx] = '\0';

    if (idx == 0)
    {
        std::cerr << "❌ 수신 실패: 버퍼 비어 있음" << std::endl;
        return 0.0f;
    }

    float distance = atof(buf);
    std::cout << "📥 받은 거리: " << distance << " cm" << std::endl;
    return distance;
}

int main()
{
    std::string portName = findSerialPort();
    int serial_port = serialOpen(portName.c_str(), 9600);
    int speed = 40;
    // int serial_port = serialOpen("/dev/ttyACM2", 9600);
    if (serial_port < 0)
    {
        std::cerr << "❌ Serial Port 열기 실패!" << std::endl;
        return -1;
    }

    MYSQL *conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, "localhost", "root", "1234", "bungbung_db", 0, NULL, 0))
    {
        std::cerr << "❌ MySQL 연결 실패: " << mysql_error(conn) << std::endl;
        return -1;
    }

    std::string last_detected_time;
    bool sentStop = false;

    while (true)
    {
        // float d1 = getSingleDistance(serial_port);
        // usleep(200000); // 0.2초 대기
        // float d2 = getSingleDistance(serial_port);
        // float speed = getSpeedFromDistances(d1, d2);

        // if (d1 > 700 || speed > 200) {
        //     std::cerr << "🚫 비정상 거리/속도 무시됨" << std::endl;
        //     continue;
        // }

        float distance = getSingleDistance(serial_port);
        // float braking = computeBraking(speed);
        // bool emergency = braking > d1;
        checkDrowsinessWarning(conn, serial_port, last_detected_time);
        if (distance < 5.0f || distance > 700.0f)
        {
            std::cerr << "❌ 비정상 거리 수신: " << distance << " cm → 무시됨" << std::endl;
            continue; // 이 루프 건너뜀
        }
        bool emergency = distance < 100.0f;

        std::cout << "거리: " << distance;

        if (emergency)
        {
            while (serialDataAvail(serial_port))
            {
                serialGetchar(serial_port); // 버려서 클리어
            }
            if (!sentStop)
            {
                std::cout << "⚠️ 제동 거리 이내 접근! 모터 정지 신호 전송" << std::endl;
                sentStop = true;
            }
            serialPuts(serial_port, "S\n");
            serialFlush(serial_port);
        }
        else if (!emergency && !sentStop)
        {
            serialPuts(serial_port, "G");
            std::cout << "✅ 안전한 상태" << std::endl;
        }

        insertToDatabase(conn, distance, distance, emergency, speed);

        usleep(5000); // 루프 간격
    }

    serialClose(serial_port);
    mysql_close(conn);
    return 0;
}
