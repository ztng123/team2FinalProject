#include <opencv2/opencv.hpp>
#include "eye.h"
#include <iostream>

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "❌ 카메라 열기 실패" << std::endl;
        return -1;
    }
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    EyeDetector eyeDetector("C:\\Users\\there\\Desktop\\BB_pj\\x64\\Release\\shape_predictor_68_face_landmarks.dat");

    int closedCount = 0;
    const int thresholdFrames = 90;  // 약 3초 (30fps 기준)

    while (true) {
        cv::Mat frame, output;
        cap >> frame;
        if (frame.empty()) continue;

        bool isEyeOpen = eyeDetector.detectClosedEyes(frame, output);

        std::string label;
        cv::Scalar color;

        if (!isEyeOpen) {
            closedCount++;
        }
        else {
            closedCount = 0;  // 눈을 떴으면 카운트 초기화
        }

        if (closedCount >= thresholdFrames) {
            label = "sleep!";
            color = cv::Scalar(0, 0, 255);  // 빨간색
        }
        else {
            label = isEyeOpen ? "eye:open" : "eye:close";
            color = isEyeOpen ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 255, 255);  // 초록 / 노랑
        }

        // 텍스트 출력
        cv::putText(output, label, cv::Point(30, 60),
            cv::FONT_HERSHEY_SIMPLEX, 1.0, color, 2);

        cv::imshow("Eye Detection Test", output);

        char key = cv::waitKey(5);
        if (key == 'q') break;
    }

    cap.release();
    cv::destroyAllWindows();
#ifdef __APPLE__
    cv::waitKey(10);
#endif
    return 0;
}
