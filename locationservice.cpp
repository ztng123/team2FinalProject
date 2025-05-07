#include <cstdlib>
#include "locationservice.h"
#include <QDebug>

LocationService::LocationService(QObject *parent) :
    QObject(parent),
    // 제공된 좌표로 초기화
    lastPosition(37.547249, 126.942316)
{
    // 고정된 위치 설정
    qDebug() << "초기 위치 설정:"
             << "위도: " << lastPosition.latitude()
             << "경도: " << lastPosition.longitude();

    // 위치 고정
    emit locationUpdated(lastPosition);
}

LocationService::~LocationService()
{
    // 필요한 경우 리소스 정리
    if (simulationTimer) {
        simulationTimer->stop();
        delete simulationTimer;
    }
}

void LocationService::onPositionUpdated(const QGeoPositionInfo &info)
{
    QGeoCoordinate coord = info.coordinate();

    if (coord.isValid()) {
        lastPosition = coord;
        qDebug() << "Position Updated:"
                 << "Latitude:" << coord.latitude()
                 << "Longitude:" << coord.longitude();
        emit locationUpdated(coord);
    }
}

void LocationService::handlePositionError(QGeoPositionInfoSource::Error error)
{
    qWarning() << "Position source error:" << error;

    // 오류 발생 시 고정된 위치 사용
    emit locationUpdated(lastPosition);
}

void LocationService::simulateLocationUpdate()
{
    // 고정된 위치 유지
    emit locationUpdated(lastPosition);
}
