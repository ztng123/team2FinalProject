#include "mainview.h"
#include <QVBoxLayout>
#include <QQmlContext>
#include <QQmlEngine>
#include <QMetaObject>
#include <QVariant>
#include <QDebug>
#include <QUrl>
#include <QApplication>

MainView::MainView(QWidget *parent)
    : QWidget(parent),
    currentLocation(37.334942, 127.103565)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    quickWidget = new QQuickWidget(this);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    layout->addWidget(quickWidget);

    setupMap();
}

MainView::~MainView() = default;

void MainView::setupMap()
{
    // QML 엔진 설정
    QQmlEngine *engine = quickWidget->engine();
    QQmlContext *context = engine->rootContext();

    // 초기값 설정 (QML 로드 전)
    context->setContextProperty("initialLatitude", 37.334942);
    context->setContextProperty("initialLongitude", 127.103565);
    context->setContextProperty("initialZoomLevel", 19.5);

    // 빈 도로 네트워크 데이터 초기 설정
    context->setContextProperty("roadNetworkData", QVariantList());

    // QML 파일 로드 (resources에서)
    quickWidget->setSource(QUrl("qrc:/qml/map.qml"));

    // 로드 성공 여부 확인
    if (!quickWidget->rootObject()) {
        qWarning() << "❌ QML 로드 실패!";

        // 대체 경로 시도
        QString qmlPath = QApplication::applicationDirPath() + "/qml/map.qml";
        quickWidget->setSource(QUrl::fromLocalFile(qmlPath));

        if (!quickWidget->rootObject()) {
            qWarning() << "❌ 대체 경로로도 QML 로드 실패!";
            return;
        }
    }

    qmlMap = quickWidget->rootObject();
    qDebug() << "✅ QML 로드 성공!";

    // 마커 위치 초기화
    updateNavigationInfo(
        currentLocation.latitude(),
        currentLocation.longitude(),
        0,
        0.0
        );
}

void MainView::updateLocation(const QGeoCoordinate &coord)
{
    if (!qmlMap) {
        qWarning() << "❌ qmlMap is null!";
        return;
    }

    currentLocation = coord;

    // QML 함수 호출
    bool invoked = QMetaObject::invokeMethod(qmlMap, "updateMarkerPosition",
                                             Q_ARG(QVariant, coord.latitude()),
                                             Q_ARG(QVariant, coord.longitude()),
                                             Q_ARG(QVariant, 0),
                                             Q_ARG(QVariant, 0));

    if (!invoked) {
        qWarning() << "❌ updateMarkerPosition invoke failed";
    }
}

void MainView::updateNavigationInfo(double lat, double lon, int speed, double bearing)
{
    if (!qmlMap) {
        qWarning() << "❌ qmlMap is null!";
        return;
    }

    // QML 함수 호출
    bool invoked = QMetaObject::invokeMethod(qmlMap, "updateMarkerPosition",
                                             Q_ARG(QVariant, lat),
                                             Q_ARG(QVariant, lon),
                                             Q_ARG(QVariant, speed),
                                             Q_ARG(QVariant, bearing));

    if (!invoked) {
        qWarning() << "❌ updateMarkerPosition 함수 호출 실패!";
    } else {
        // 성공적으로 호출되면 로그 출력
        qDebug() << "지도 위치 업데이트 성공: " << lat << ", " << lon;
    }
}

void MainView::showDrowsinessAlert()
{
    if (!qmlMap) {
        qWarning() << "❌ qmlMap is null!";
        return;
    }

    // 경고창 표시 함수 호출
    bool invoked = QMetaObject::invokeMethod(qmlMap, "showDrowsinessAlert");

    if (!invoked) {
        qWarning() << "❌ showDrowsinessAlert invoke failed";
    }
}

void MainView::setRoadData(const QVariantList &roadData)
{
    if (!quickWidget || !quickWidget->rootContext()) {
        qWarning() << "❌ QML 컨텍스트가 없습니다!";
        return;
    }

    // QML 컨텍스트에 도로 데이터 설정
    quickWidget->engine()->rootContext()->setContextProperty("roadNetworkData", roadData);
    qDebug() << "도로 네트워크 데이터 설정 완료: " << roadData.size() << "개의 도로 세그먼트";
}
