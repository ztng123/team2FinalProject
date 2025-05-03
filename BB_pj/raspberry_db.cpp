#include <iostream>
#include <mysql.h>
#include <string>
using namespace std;

int main() {
    MYSQL* conn;
    MYSQL* conn_result;
    
    conn = mysql_init(NULL);
    if (conn == NULL) {
        cerr << "mysql_init fail" << endl;
        return 1;
    }

    conn_result = mysql_real_connect(conn,
        "192.168.129.77",  // 라즈베리파이 IP
        "root",            // DB 계정
        "1234",            // 비밀번호
        "bungbung_db",     // 데이터베이스 이름
        3306, NULL, 0);

    if (conn_result == NULL) {
        cerr << "DB 연결 실패: " << mysql_error(conn) << endl;
        return 1;
    }

    cout << "DB 연결 성공" << endl;

    // 감지된 졸음 정보 (예시값) -> 이후에 감지된 데이터 값 업데이트해서 바로 넣을 수 있도록 수정
    string carNumber = "33가 1234";
    string location = "서울시 금천구";
    float closedTime = 3.0;
    int warningLevel = 2;

    // INSERT 쿼리 작성
    char query[1024];
    sprintf_s(query, sizeof(query),
        "INSERT INTO driver_drown_logs (car_number, location, detection_time, closed_time, warning_level) "
        "VALUES ('%s', '%s', NOW(), %.2f, %d)",
        carNumber.c_str(), location.c_str(), closedTime, warningLevel);

    // 쿼리 실행
    if (mysql_query(conn, query)) {
        cerr << "쿼리 실행 실패: " << mysql_error(conn) << endl;
    }
    else {
        cout << "졸음 감지 정보 저장 완료!" << endl;
    }

    mysql_close(conn);
    return 0;
}
