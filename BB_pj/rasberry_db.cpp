#include <iostream>
#include <mysql.h>
#include <ctime>  // 시간

using namespace std;

int main()
{
    MYSQL* conn;
    MYSQL* conn_result;
    unsigned int timeout_sec = 1;

    conn = mysql_init(NULL);
    mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
    conn_result = mysql_real_connect(conn, 
        "192.168.129.77", // 라즈베리파이 IP
        "root", "1234", 
        "bungbung_db", 
        3306, NULL, 0); // 포트와 기본 옵션

    if (conn_result == NULL)
    {
        cout << "DB Connection Fail" << endl;
        return -1;
    }

    cout << "DB Connection Success" << endl;

    // 1️. 저장할 샘플 데이터 -> 이후에 값을 받아오면 자동으로 저장되어 다시 내보낼 수 있는 방향으로 수정해야할 듯.
    const char* car_number = "23가 1234";
    const char* location = "광주 자차로 9길";
    float closed_time = 3.2f;
    int warning_level = 2;

    // 2️. 현재 시간 얻기 (YYYY-MM-DD HH:MM:SS)
    time_t now = time(0);
    struct tm* t = localtime(&now);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

    // 3️. INSERT 쿼리 구성
    char query[1024];
    sprintf_s(query, sizeof(query),
        "INSERT INTO driver_drown_logs (car_number, location, detection_time, closed_time, warning_level) "
        "VALUES ('%s', '%s', '%s', %.2f, %d)",
        car_number, location, time_str, closed_time, warning_level);

    // 4️. 쿼리 실행
    if (mysql_query(conn, query)) {
        cerr << "INSERT Fail: " << mysql_error(conn) << endl;
    }
    else {
        cout << "졸음운전 데이터 DB 저장 성공!" << endl;
    }

    mysql_close(conn);
    return 0;
}
