#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

class EyeDetector {
public:
    EyeDetector(const std::string& modelPath);
    bool detectClosedEyes(const cv::Mat& frame, cv::Mat& outFrame);  // 0 : 눈 감음, 1 : 눈뜸

private:
    dlib::frontal_face_detector detector;
    dlib::shape_predictor shapePredictor;
    cv::Ptr<cv::Tracker> tracker;
    dlib::rectangle dlibFace;
    bool detectNewFace = true;

    const double EAR_THRESHOLD = 0.22;

    double euclidean(const dlib::point& p1, const dlib::point& p2);
    double computeEAR(const dlib::full_object_detection& shape, int eye_start);
};
