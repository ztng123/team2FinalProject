#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGeoCoordinate>
#include <QTimer>
#include <QVector>
#include <QTextToSpeech>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainView;
class LocationService;
class DrowsinessDetector;

// 도로 세그먼트 구조체
struct RoadSegment {
    QGeoCoordinate start;
    QGeoCoordinate end;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateSpeed();
    void handleDrowsinessDetected();
    void showDrowsyZoneAlert(const QString &zoneDescription);
    void speakDrowsyZoneWarning(const QString &text);

private:
    Ui::MainWindow *ui;
    MainView *mapView;
    LocationService *locationService;
    DrowsinessDetector *drowsinessDetector;
    QTimer *speedUpdateTimer;
    QTextToSpeech *tts;

    // 경로 및 속도 데이터
    QVector<QGeoCoordinate> routeCoordinates;
    QVector<int> speedData;
    QVector<bool> emergencyData;
    int currentIndex;

    // 도로 네트워크
    QVector<RoadSegment> roadNetwork;

    // 함수들
    void loadRoadNetwork();
    void loadCsvData();
    void createDefaultRoute();
    void setupConnections();

    // 졸음운전 다발구역 관련
    bool isDrowsyZoneLocation(int routeIndex, QString &zoneDescription);
};
#endif // MAINWINDOW_H
