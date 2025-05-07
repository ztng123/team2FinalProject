#ifndef LOCATIONSERVICE_H
#define LOCATIONSERVICE_H

#include <QObject>
#include <QGeoPositionInfoSource>
#include <QGeoCoordinate>
#include <QTimer>

class LocationService : public QObject
{
    Q_OBJECT

public:
    explicit LocationService(QObject *parent = nullptr);
    ~LocationService();

signals:
    void locationUpdated(const QGeoCoordinate &coordinate);

private slots:
    void onPositionUpdated(const QGeoPositionInfo &info);
    void simulateLocationUpdate(); // 테스트용
    void handlePositionError(QGeoPositionInfoSource::Error error);

private:
    QGeoPositionInfoSource *positionSource = nullptr;
    QTimer *simulationTimer = nullptr; // 테스트용
    QGeoCoordinate lastPosition;
};

#endif // LOCATIONSERVICE_H
