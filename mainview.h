#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <QQuickWidget>
#include <QGeoCoordinate>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QVariant>
#include <QMetaObject>
#include <QApplication>

class MainView : public QWidget
{
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = nullptr);
    ~MainView();

public slots:
    void updateLocation(const QGeoCoordinate &coordinate);
    void updateNavigationInfo(double latitude,
                              double longitude,
                              int speed,
                              double bearing);
    void showDrowsinessAlert();

    // 도로 네트워크 데이터 설정
    void setRoadData(const QVariantList &roadData);

private:
    QQuickWidget   *quickWidget;
    QQuickItem     *qmlMap;
    QGeoCoordinate  currentLocation;

    void setupMap();
};

#endif // MAINVIEW_H
