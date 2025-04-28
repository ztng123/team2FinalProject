#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/shape_predictor.h>
#include <dlib/opencv.h>

int main() {
    // 얼굴 검출기, 랜드마크 예측기 초기화
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor predictor;
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;

    // 웹캠 열기
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "웹캠을 열 수 없습니다." << std::endl;
        return -1;
    }

    // 윈도우 설정
    cv::namedWindow("frame", cv::WINDOW_NORMAL);
    cv::resizeWindow("frame", 800, 600);

    while (true) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        // BGR -> RGB 변환
        cv::Mat img_rgb;
        cv::cvtColor(frame, img_rgb, cv::COLOR_BGR2RGB);

        // 이미지 리사이즈
        double r = 400.0 / img_rgb.cols;
        cv::Size dim(400, static_cast<int>(img_rgb.rows * r));
        cv::Mat resized;
        cv::resize(img_rgb, resized, dim);

        // Dlib 이미지 포맷 변환
        dlib::cv_image<dlib::rgb_pixel> dlib_img(resized);

        // 얼굴 검출
        std::vector<dlib::rectangle> rects = detector(dlib_img);

        for (auto& rect : rects) {
            dlib::full_object_detection shape = predictor(dlib_img, rect);

            // 랜드마크 찍기
            for (int j = 0; j < 68; ++j) {
                int x = shape.part(j).x();
                int y = shape.part(j).y();
                cv::circle(resized, cv::Point(x, y), 2, cv::Scalar(255, 0, 0), -1);
            }

            // 얼굴 박스 그리기
            cv::rectangle(resized, 
                cv::Point(rect.left(), rect.top()), 
                cv::Point(rect.right(), rect.bottom()), 
                cv::Scalar(0, 255, 0), 2);
        }

        // 다시 RGB -> BGR 변환
        cv::Mat resized_bgr;
        cv::cvtColor(resized, resized_bgr, cv::COLOR_RGB2BGR);

        // 800x600 사이즈로 키워서 보여주기
        cv::Mat bigger;
        cv::resize(resized_bgr, bigger, cv::Size(800, 600));
        cv::imshow("frame", bigger);

        // 'q' 키 누르면 종료
        if (cv::waitKey(1) == 'q') break;
    }

    // 리소스 해제
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
