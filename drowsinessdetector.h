#ifndef DROWSINESSDETECTOR_H
#define DROWSINESSDETECTOR_H
#include <QObject>

class DrowsinessDetector : public QObject
{
    Q_OBJECT
public:
    explicit DrowsinessDetector(QObject *parent = nullptr);
    ~DrowsinessDetector();

    // 테스트용: 졸음 감지 시뮬레이션
    void simulateDetection();

signals:
    void drowsinessDetected();

private:
         // 실제 구현에서는 여기에 카메라 또는 센서 연결 코드가 들어갈 수 있음
};

#endif // DROWSINESSDETECTOR_H
