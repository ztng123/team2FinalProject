// drowsiness_db_realtime.cpp

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <mysql.h>

using namespace std;
using namespace cv;

// EAR 계산 함수
double computeEAR(const vector<Point>& eye) {
    double A = norm(eye[1] - eye[5]);
    double B = norm(eye[2] - eye[4]);
    double C = norm(eye[0] - eye[3]);
    return (A + B) / (2.0 * C);
}

int main() {
    // MySQL 연결
    MYSQL* conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, 
        "192.168.129.77", 
        "root", 
        "1234", 
        "bungbung_db", 
        3306, NULL, 0)) {
        cerr << "DB 연결 실패: " << mysql_error(conn) << endl;
        return -1;
    }
    cout << "DB 연결 완료" << endl;

    // Dlib 설정
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor predictor;
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;

    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "웹캠 열기 실패" << endl;
        return -1;
    }

    const double EAR_THRESHOLD = 0.25;
    const int FRAME_THRESHOLD = 90; // 3초
    int frameCount = 0;

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        dlib::cv_image<unsigned char> dlibImg(gray);
        vector<dlib::rectangle> faces = detector(dlibImg);

        for (auto& face : faces) {
            dlib::full_object_detection shape = predictor(dlibImg, face);

            vector<Point> leftEye, rightEye;
            for (int i = 36; i <= 41; ++i) leftEye.push_back(Point(shape.part(i).x(), shape.part(i).y()));
            for (int i = 42; i <= 47; ++i) rightEye.push_back(Point(shape.part(i).x(), shape.part(i).y()));

            double ear = (computeEAR(leftEye) + computeEAR(rightEye)) / 2.0;

            if (ear < EAR_THRESHOLD) frameCount++;
            else frameCount = 0;

            // 졸음 감지
            if (frameCount >= FRAME_THRESHOLD) {
                cout << "졸음 운전 감지됨!" << endl;

                string carNumber = "33가 1234";
                string location = "서울시 금천구";
                float closedTime = 3.0;
                int warningLevel = 2;

                char query[1024];
                sprintf(query,
                    "INSERT INTO driver_drown_logs (car_number, location, detection_time, closed_time, warning_level) "
                    "VALUES ('%s', '%s', NOW(), %.2f, %d)",
                    carNumber.c_str(), location.c_str(), closedTime, warningLevel);

                if (mysql_query(conn, query)) {
                    cerr << "쿼리 실패: " << mysql_error(conn) << endl;
                }
                else {
                    cout << "DB 저장 완료\n";
                }
                frameCount = 0; // 중복 감지 방지
            }
        }

        imshow("Drowsiness Detection", frame);
        if (waitKey(1) == 27) break; // ESC 키
    }

    cap.release();
    destroyAllWindows();
    mysql_close(conn);
    return 0;
}
