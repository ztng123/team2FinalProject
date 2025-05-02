#pragma once

#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

class EyeDetector {
public:
    EyeDetector(const std::string& modelPath);
    bool detectClosedEyes(const cv::Mat& frame, cv::Mat& outFrame);  // 0: 눈 감음, 1: 눈뜸

private:
    dlib::frontal_face_detector detector;
    dlib::shape_predictor shapePredictor;

    // 트래커 제거 → 얼굴 재검출을 항상 수행
    const double EAR_THRESHOLD = 0.22;

    double euclidean(const dlib::point& p1, const dlib::point& p2);
    double computeEAR(const dlib::full_object_detection& shape, int eye_start);
};
