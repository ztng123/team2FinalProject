#include "drowsinessdetector.h"
#include <QDebug>

DrowsinessDetector::DrowsinessDetector(QObject *parent) :
    QObject(parent)
{
    // 초기화 작업 (필요한 경우)
    qDebug() << "DrowsinessDetector 생성됨";
}

DrowsinessDetector::~DrowsinessDetector()
{
    // 리소스 정리 (필요한 경우)
    qDebug() << "DrowsinessDetector 소멸됨";
}

void DrowsinessDetector::simulateDetection()
{
    // 테스트용: 졸음 감지 시뮬레이션
    qDebug() << "졸음 감지 시뮬레이션";

    // 졸음 감지 신호 발생
    emit drowsinessDetected();
}
