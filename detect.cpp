#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <onnxruntime_cxx_api.h>
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;

// EAR 계산 함수
double computeEAR(const std::vector<cv::Point>& eye) {
    double A = norm(eye[1] - eye[5]);
    double B = norm(eye[2] - eye[4]);
    double C = norm(eye[0] - eye[3]);
    return (A + B) / (2.0 * C);
}

// 눈 영역 crop (중앙 정사각형 확장)
cv::Mat cropEye(const cv::Mat& img, const std::vector<cv::Point>& eye) {
    cv::Rect bbox = cv::boundingRect(eye);
    cv::Point center(bbox.x + bbox.width / 2, bbox.y + bbox.height / 2);
    int size = static_cast<int>(std::max(bbox.width, bbox.height) * 1.5);
    cv::Rect square(center.x - size / 2, center.y - size / 2, size, size);
    square &= Rect(0, 0, img.cols, img.rows);
    return img(square).clone();
}

// 이미지 전처리: OpenCV → ONNX 입력 (float[1,3,96,96])
std::vector<float> preprocess(const cv::Mat& img) {
    cv::Mat resized, float_img, normalized;
    cv::resize(img, resized, Size(96, 96));
    resized.convertTo(float_img, CV_32F, 1.0 / 255.0);
    normalized = (float_img - 0.5) / 0.5;

    std::vector<float> input_tensor_values(3 * 96 * 96);
    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < 96; ++i) {
            for (int j = 0; j < 96; ++j) {
                input_tensor_values[c * 96 * 96 + i * 96 + j] = normalized.at<Vec3f>(i, j)[c];
            }
        }
    }
    return input_tensor_values;
}

// 추론 함수
int predictEye(const cv::Mat& eye_img, Ort::Session& session, Ort::AllocatorWithDefaultOptions& allocator) {
    std::vector<float> input_tensor = preprocess(eye_img);
    std::vector<int64_t> input_shape = {1, 3, 96, 96};
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

    Ort::Value input_tensor_ort = Ort::Value::CreateTensor<float>(
        memory_info,
        input_tensor.data(),
        input_tensor.size(),
        input_shape.data(),
        input_shape.size()
    );

    const char* input_names[] = {"input"};
    const char* output_names[] = {"output"};

    auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_names, &input_tensor_ort, 1, output_names, 1);
    float* output = output_tensors.front().GetTensorMutableData<float>();
    return output[1] > output[0] ? 1 : 0;  // label: 1=open, 0=closed
}

int main() {
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor predictor;
    dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;


    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "eye_model");
    Ort::SessionOptions session_options;
    Ort::Session session(env, "eye_model.onnx", session_options);
    Ort::AllocatorWithDefaultOptions allocator;


    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "❌ 카메라 열기 실패" << endl;
        return -1;
    }

    while (true) {
        Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        dlib::cv_image<dlib::bgr_pixel> dlib_img(frame);
        std::vector<dlib::rectangle> faces = detector(dlib_img);

        for (const auto& face : faces) {
            dlib::full_object_detection shape = predictor(dlib_img, face);

            std::vector<cv::Point> left_eye, right_eye;
            for (int i = 36; i <= 41; ++i)
                left_eye.push_back(Point(shape.part(i).x(), shape.part(i).y()));
            for (int i = 42; i <= 47; ++i)
                right_eye.push_back(Point(shape.part(i).x(), shape.part(i).y()));

            double l_ear = computeEAR(left_eye);
            double r_ear = computeEAR(right_eye);
            double avg_ear = (l_ear + r_ear) / 2.0;
            

            cv::Mat left_patch = cropEye(frame, left_eye);
            cv::Mat right_patch = cropEye(frame, right_eye);

            string result = "";
            int l_pred = predictEye(left_patch, session, allocator);
            int r_pred = predictEye(right_patch, session, allocator);

            double ear_threshold = 0.25;

            if (r_pred == 0 && r_ear > ear_threshold) {
                r_pred = 1;
            }
            else if (r_pred == 1 && r_ear < 0.15) {
                r_pred = 0;
            }

            if (l_pred == 0 && l_ear > ear_threshold) {
                l_pred = 1;
            }
            else if (l_pred == 1 && l_ear< 0.15) {
                l_pred = 0;
            }

            result += (l_pred == 1 ? "open" : "close");
            result += (r_pred == 1 ? "open" : "close");
            std::cout << result << std::endl;

            // 출력
            cv::putText(frame, result, Point(face.left(), face.top() - 10), FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0, 255, 0), 3);
        }

        imshow("Drowsiness Detection", frame);
        if (waitKey(1) == 'q') break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}

