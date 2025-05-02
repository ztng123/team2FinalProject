#include "eye.h"
#include <cmath>

EyeDetector::EyeDetector(const std::string& modelPath) {
    detector = dlib::get_frontal_face_detector();
    dlib::deserialize(modelPath) >> shapePredictor;
}

double EyeDetector::euclidean(const dlib::point& p1, const dlib::point& p2) {
    return std::sqrt(std::pow(p1.x() - p2.x(), 2) + std::pow(p1.y() - p2.y(), 2));
}

double EyeDetector::computeEAR(const dlib::full_object_detection& shape, int eye_start) {
    double A = euclidean(shape.part(eye_start + 1), shape.part(eye_start + 5));
    double B = euclidean(shape.part(eye_start + 2), shape.part(eye_start + 4));
    double C = euclidean(shape.part(eye_start + 0), shape.part(eye_start + 3));
    return (A + B) / (2.0 * C);
}

bool EyeDetector::detectClosedEyes(const cv::Mat& frame, cv::Mat& outFrame) {
    if (frame.empty()) return false;

    outFrame = frame.clone();

    // 얼굴 검출
    cv::Mat rgb;
    cv::cvtColor(frame, rgb, cv::COLOR_BGR2RGB);
    dlib::cv_image<dlib::rgb_pixel> dlibImg(rgb);

    std::vector<dlib::rectangle> faces = detector(dlibImg);
    if (faces.empty()) {
        return false;
    }

    // 첫 번째 얼굴 선택
    dlib::rectangle face = faces[0];
    dlib::full_object_detection shape = shapePredictor(dlibImg, face);

    double leftEAR = computeEAR(shape, 36);
    double rightEAR = computeEAR(shape, 42);
    double avgEAR = (leftEAR + rightEAR) / 2.0;

    return avgEAR >= EAR_THRESHOLD;
}
