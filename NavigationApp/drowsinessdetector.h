#ifndef DROWSINESSDETECTOR_H
#define DROWSINESSDETECTOR_H

#include <QObject>
#include <QTimer>

class DrowsinessDetector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool drowsinessDetected READ isDrowsinessDetected NOTIFY drowsinessStatusChanged)

public:
    explicit DrowsinessDetector(QObject *parent = nullptr);
    bool isDrowsinessDetected() const { return m_drowsinessDetected; }

public slots:
    void startMonitoring();
    void stopMonitoring();
    void simulateDrowsiness(); // 테스트용 메서드

signals:
    void drowsinessStatusChanged();
    void drowsinessDetected();

private slots:
    void checkDrowsiness();

private:
    bool m_drowsinessDetected;
    QTimer *m_timer;

    // 실제 구현에서는 카메라 또는 센서 관련 코드가 추가될 수 있음
};

#endif // DROWSINESSDETECTOR_H
