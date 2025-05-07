#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainview.h"
#include "locationservice.h"
#include "drowsinessdetector.h"
#include "alertdialog.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QGeoCoordinate>
#include <QTimer>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QVariantList>
#include <QVariantMap>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextToSpeech>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tts(nullptr)
{
    ui->setupUi(this);

    // 맵 뷰 생성 - QML 기반 지도 표시
    mapView = new MainView(this);
    setCentralWidget(mapView);

    // 위치 서비스 및 졸음 감지기 초기화
    locationService = new LocationService(this);
    drowsinessDetector = new DrowsinessDetector(this);

    // TTS 엔진 초기화
    tts = new QTextToSpeech(this);
    // 한국어 설정 (가능한 경우)
    QVector<QLocale> locales = tts->availableLocales();
    for (const QLocale &locale : locales) {
        if (locale.language() == QLocale::Korean) {
            tts->setLocale(locale);
            break;
        }
    }

    // 도로 네트워크 로드
    loadRoadNetwork();

    // CSV 데이터 로드
    loadCsvData();

    // 연결 설정
    setupConnections();

    // 타이머 설정
    speedUpdateTimer = new QTimer(this);
    connect(speedUpdateTimer, &QTimer::timeout, this, &MainWindow::updateSpeed);
    speedUpdateTimer->start(1000); // 1초마다 업데이트

    // 윈도우 크기 설정
    resize(800, 600);
    setWindowTitle("차량 네비게이션 시스템");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 도로 네트워크 로드 함수
void MainWindow::loadRoadNetwork() {
    // 도로 네트워크 초기화
    roadNetwork.clear();

    // CSV 데이터에서 좌표를 기반으로 도로 정의 (아직 CSV 데이터가 없으므로 기본 도로 추가)
    double baseLongitude = 127.103565; // 기준 경도

    // 북쪽으로 이어지는 세로 도로 정의
    roadNetwork.append({
        QGeoCoordinate(37.334000, baseLongitude - 0.0001), // 시작점 (폭 넓게)
        QGeoCoordinate(37.342000, baseLongitude + 0.0001)  // 끝점 (폭 넓게)
    });

    // 동서 방향 도로 추가 (교차로)
    roadNetwork.append({
        QGeoCoordinate(37.338000, baseLongitude - 0.003), // 시작점 (서쪽)
        QGeoCoordinate(37.338000, baseLongitude + 0.003)  // 끝점 (동쪽)
    });

    // 도로 데이터를 QML로 전달하기 위한 처리
    QVariantList roadData;
    for (const auto &segment : roadNetwork) {
        QVariantMap segmentMap;
        segmentMap["startLat"] = segment.start.latitude();
        segmentMap["startLon"] = segment.start.longitude();
        segmentMap["endLat"] = segment.end.latitude();
        segmentMap["endLon"] = segment.end.longitude();
        roadData.append(segmentMap);
    }

    // 맵뷰에 도로 데이터 전달
    if (mapView) {
        mapView->setRoadData(roadData);
    }

    qDebug() << "도로 네트워크 로드 완료: " << roadNetwork.size() << "개의 도로 세그먼트";
}

// CSV 파일에서 데이터 로드
void MainWindow::loadCsvData()
{
    // 데이터 저장용 벡터 초기화 - 먼저 비워서 안전하게 처리
    routeCoordinates.clear();
    speedData.clear();
    emergencyData.clear();

    // 파일 경로 설정 - 이미지 폴더 경로 추가
    QString csvPath = "C:/Users/g_gyuuuuuu/Documents/carproject/images/Updated_Speed_Data__With_Emergency_Cases_.csv";
    QFile file(csvPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "CSV 파일을 열 수 없습니다: " << csvPath;

        // 다른 가능한 경로들 시도
        QStringList possiblePaths;
        possiblePaths << QCoreApplication::applicationDirPath() + "/images/Updated_Speed_Data__With_Emergency_Cases_.csv"
                      << "./images/Updated_Speed_Data__With_Emergency_Cases_.csv"
                      << "../images/Updated_Speed_Data__With_Emergency_Cases_.csv"
                      << "../../images/Updated_Speed_Data__With_Emergency_Cases_.csv";

        bool fileFound = false;
        for (const QString &path : possiblePaths) {
            file.setFileName(path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                fileFound = true;
                qDebug() << "CSV 파일을 찾았습니다: " << path;
                break;
            }
        }

        if (!fileFound) {
            qWarning() << "CSV 파일을 찾을 수 없습니다. 기본 경로를 생성합니다.";
            createDefaultRoute();
            return;
        }
    } else {
        qDebug() << "CSV 파일을 찾았습니다: " << csvPath;
    }

    QTextStream in(&file);
    if (!in.atEnd()) {
        in.readLine(); // 헤더 스킵
    }

    // 임시 좌표 데이터
    double baseLatitude = 37.334942;
    double baseLongitude = 127.103565;
    double latIncrement = 0.0002; // 각 포인트마다 위도 증가량

    int rowIndex = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        if (fields.size() < 5) {
            qWarning() << "CSV 행 형식이 올바르지 않습니다:" << line;
            continue; // 형식이 맞지 않는 행 건너뛰기
        }

        // CSV 데이터 파싱 (안전하게 처리)
        int id = fields.size() > 0 ? fields[0].toInt() : 0;
        double speed = fields.size() > 5 ? fields[5].toDouble() : 50.0; // 기본값 설정
        bool isEmergency = fields.size() > 4 ? (fields[4].trimmed() == "1") : false;

        // 속도가 0이면 기본값으로 설정
        if (speed <= 0) {
            speed = 50.0 + (rowIndex % 10) * 5.0; // 50~95 사이의 값
        }

        // 위치 데이터 생성 (도로를 따라 이동하도록)
        double lat = baseLatitude + (rowIndex * latIncrement);
        double lon = baseLongitude;

        // 약간의 오차 추가 (맵 매칭 테스트용)
        if (rowIndex % 3 == 0) lon += 0.0001;
        if (rowIndex % 5 == 0) lon -= 0.0001;

        // 데이터 저장
        routeCoordinates.append(QGeoCoordinate(lat, lon));
        speedData.append(static_cast<int>(speed));
        emergencyData.append(isEmergency);

        rowIndex++;
    }

    file.close();

    currentIndex = 0;
    qDebug() << "CSV 파일에서" << routeCoordinates.size() << "개의 위치 데이터를 로드했습니다.";

    // 데이터가 없는 경우 기본 데이터 생성
    if (routeCoordinates.isEmpty()) {
        qDebug() << "유효한 데이터가 없어 기본 경로를 생성합니다.";
        createDefaultRoute();
    }
}

// 기본 경로 생성 (별도 함수로 분리)
void MainWindow::createDefaultRoute()
{
    qDebug() << "기본 경로를 생성합니다.";

    // 시작점 (기본 좌표)
    QGeoCoordinate baseCoord(37.334942, 127.103565);

    // 초기화
    routeCoordinates.clear();
    speedData.clear();
    emergencyData.clear();

    // 북쪽으로 이동하는 경로 생성 (도로 위를 따라가도록)
    for (int i = 0; i < 20; i++) {
        // 위도 약간씩 증가 (북쪽으로 이동)
        double lat = baseCoord.latitude() + (i * 0.0005);
        double lon = baseCoord.longitude();

        // 약간의 오차 추가 (맵 매칭 테스트용)
        if (i % 3 == 0) lon += 0.0001;
        if (i % 5 == 0) lon -= 0.0001;

        // 속도 변화 (40~80km/h 사이)
        int speed = 40 + (i % 5) * 10;

        // 비상 상황 (10번째 포인트에서만)
        bool emergency = (i == 10);

        routeCoordinates.append(QGeoCoordinate(lat, lon));
        speedData.append(speed);
        emergencyData.append(emergency);
    }

    currentIndex = 0;
    qDebug() << "기본 경로 생성 완료: " << routeCoordinates.size() << "개의 위치 데이터";
}

// 연결 설정 함수
void MainWindow::setupConnections()
{
    // 위치 업데이트 연결
    if (locationService && mapView) {
        connect(locationService, &LocationService::locationUpdated,
                mapView, &MainView::updateLocation);
    }

    // 졸음 감지 연결
    if (drowsinessDetector) {
        connect(drowsinessDetector, &DrowsinessDetector::drowsinessDetected,
                this, &MainWindow::handleDrowsinessDetected);
    }
}

// 특정 위치가 졸음운전 다발구역인지 확인하는 함수
bool MainWindow::isDrowsyZoneLocation(int routeIndex, QString &zoneDescription)
{
    // 경로 인덱스를 기준으로 특정 구간을 졸음운전 다발구역으로 지정
    // 예: 경로의 20~25% 구간과 60~65% 구간을 다발구역으로 지정

    int totalPoints = routeCoordinates.size();
    if (totalPoints == 0) return false;

    // 다발구역 범위 계산 (경로의 비율로 계산)
    int zone1Start = static_cast<int>(totalPoints * 0.2);  // 20% 위치
    int zone1End = static_cast<int>(totalPoints * 0.25);   // 25% 위치
    int zone2Start = static_cast<int>(totalPoints * 0.6);  // 60% 위치
    int zone2End = static_cast<int>(totalPoints * 0.65);   // 65% 위치

    // 첫 번째 다발구역
    if (routeIndex >= zone1Start && routeIndex <= zone1End) {
        zoneDescription = "졸음운전 다발구역 A (국도 42번 구간)";
        return true;
    }

    // 두 번째 다발구역
    if (routeIndex >= zone2Start && routeIndex <= zone2End) {
        zoneDescription = "졸음운전 다발구역 B (고속도로 진입 구간)";
        return true;
    }

    return false;
}

// 졸음 다발구역 경고창 표시
void MainWindow::showDrowsyZoneAlert(const QString &zoneDescription)
{
    // 다른 경고창이 이미 표시되고 있는지 확인
    QList<QDialog*> activeDialogs = findChildren<QDialog*>();
    for (QDialog* dialog : activeDialogs) {
        if (dialog->isVisible() &&
            (dialog->windowTitle() == "졸음운전 경고" || dialog->windowTitle() == "졸음운전 다발구역 경고")) {
            // 이미 다른 경고창이 표시 중이면 새 경고창을 표시하지 않음
            qDebug() << "다른 경고창이 이미 표시 중이므로 졸음운전 다발구역 경고를 표시하지 않습니다.";
            return;
        }
    }

    // 다발구역 전용 경고창 생성
    QDialog* zoneAlert = new QDialog(this);
    zoneAlert->setWindowTitle("졸음운전 다발구역 경고");
    zoneAlert->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    zoneAlert->setAttribute(Qt::WA_TranslucentBackground);
    zoneAlert->setAttribute(Qt::WA_DeleteOnClose);

    // 레이아웃 설정
    QVBoxLayout* layout = new QVBoxLayout(zoneAlert);

    // 배경 패널 (사진처럼 보이게 하는 컨테이너)
    QWidget* backgroundPanel = new QWidget(zoneAlert);
    backgroundPanel->setObjectName("backgroundPanel");
    backgroundPanel->setStyleSheet(
        "QWidget#backgroundPanel {"
        "background-color: rgba(255, 140, 0, 220);"  // 다발구역은 주황색 배경
        "border: 2px solid #ff3b30;"  // 빨간색 테두리
        "border-radius: 15px;"
        "}"
        );
    QVBoxLayout* panelLayout = new QVBoxLayout(backgroundPanel);

    // 경고 아이콘
    QLabel* iconLabel = new QLabel;
    QPixmap warningIcon(":/images/warning.png");
    if (!warningIcon.isNull()) {
        iconLabel->setPixmap(warningIcon.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        iconLabel->setAlignment(Qt::AlignCenter);
    }

    // 경고 메시지
    QLabel* messageLabel = new QLabel(QString("⚠️ %1 ⚠️\n\n이 구간에서는 속도를 줄이고 주의 운전하세요.").arg(zoneDescription));
    messageLabel->setStyleSheet(
        "color: white;"
        "font-size: 14pt;"
        "font-weight: bold;"
        "background-color: transparent;"  // 투명 배경
        "padding: 10px;"
        );
    messageLabel->setAlignment(Qt::AlignCenter);

    // 확인 버튼
    QPushButton* confirmButton = new QPushButton("확인");
    confirmButton->setStyleSheet(
        "background-color: #e53935;"
        "color: white;"
        "border-radius: 8px;"
        "padding: 8px 16px;"
        "font-weight: bold;"
        "font-size: 12pt;"
        );
    connect(confirmButton, &QPushButton::clicked, zoneAlert, &QDialog::accept);

    // 패널에 위젯 추가
    panelLayout->addWidget(iconLabel);
    panelLayout->addWidget(messageLabel);
    panelLayout->addWidget(confirmButton, 0, Qt::AlignCenter);
    panelLayout->setContentsMargins(20, 20, 20, 20);

    // 메인 레이아웃에 패널 추가
    layout->addWidget(backgroundPanel);

    // 자동 닫힘 타이머 설정
    QTimer::singleShot(7000, zoneAlert, &QDialog::accept); // 7초 후 자동 닫힘

    // 경고창 표시 (비모달 방식)
    zoneAlert->show();

    qDebug() << "졸음운전 다발구역 경고 표시:" << zoneDescription;
}

// TTS 실행 함수
void MainWindow::speakDrowsyZoneWarning(const QString &text)
{
    if (!tts) return;

    // 음성 출력
    QString warningText = "주의하세요. " + text + " 입니다. 안전 운전하세요.";
    tts->say(warningText);

    qDebug() << "TTS 실행:" << warningText;
}

// 졸음운전 감지 처리 함수 - 감속 기능 추가
void MainWindow::handleDrowsinessDetected()
{
    // 다른 경고창이 이미 표시되고 있는지 확인
    QList<QDialog*> activeDialogs = findChildren<QDialog*>();
    for (QDialog* dialog : activeDialogs) {
        if (dialog->isVisible() &&
            (dialog->windowTitle() == "졸음운전 경고" || dialog->windowTitle() == "졸음운전 다발구역 경고")) {
            // 이미 다른 경고창이 표시 중이면 새 경고창을 표시하지 않음
            qDebug() << "다른 경고창이 이미 표시 중이므로 졸음운전 경고를 표시하지 않습니다.";
            return;
        }
    }

    // 속도 10% 감소 로직 추가
    if (currentIndex < speedData.size()) {
        int currentSpeed = speedData[currentIndex];
        int reducedSpeed = static_cast<int>(currentSpeed * 0.9); // 10% 감속

        // 현재 및 향후 몇 개의 위치 포인트에 대해 속도 감소 적용
        const int slowDownPoints = 5; // 감속 상태를 유지할 포인트 수
        for (int i = currentIndex; i < qMin(currentIndex + slowDownPoints, speedData.size()); i++) {
            speedData[i] = reducedSpeed;
        }

        // 현재 표시된 속도 즉시 갱신
        statusBar()->showMessage(QString("안전 감속 중: %1 km/h").arg(reducedSpeed));

        // 맵 뷰에도 감속된 속도 정보 전달
        if (mapView && currentIndex < routeCoordinates.size()) {
            QGeoCoordinate currentPosition = routeCoordinates[currentIndex];
            mapView->updateNavigationInfo(
                currentPosition.latitude(),
                currentPosition.longitude(),
                reducedSpeed,
                0.0  // 항상 북쪽 방향
                );
        }
    }

    // AlertDialog 생성 및 설정
    AlertDialog* alertDialog = new AlertDialog(this);
    if (alertDialog) {
        alertDialog->setWindowTitle("졸음운전 경고");
        alertDialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        alertDialog->setAttribute(Qt::WA_TranslucentBackground);
        alertDialog->setAttribute(Qt::WA_DeleteOnClose);

        // 모달리스 방식으로 다이얼로그 표시
        alertDialog->show();
    }
}

// 속도 및 위치 업데이트
void MainWindow::updateSpeed()
{
    // 데이터가 비어있는지 확인
    if (routeCoordinates.isEmpty()) {
        qWarning() << "위치 데이터가 없습니다!";
        return;
    }

    // 인덱스 범위 검증
    if (currentIndex >= routeCoordinates.size()) {
        qDebug() << "경로의 끝에 도달했습니다. 처음으로 돌아갑니다.";
        currentIndex = 0;
    }

    // 현재 위치 및 속도 정보 가져오기
    QGeoCoordinate currentPosition = routeCoordinates[currentIndex];

    // 속도 데이터가 있는지 확인
    int speed = (currentIndex < speedData.size()) ? speedData[currentIndex] : 50;

    // 비상 상황 데이터가 있는지 확인
    bool isEmergency = (currentIndex < emergencyData.size()) ? emergencyData[currentIndex] : false;

    // 졸음운전 다발구역 확인 (추가)
    QString zoneDescription;
    bool inDrowsyZone = isDrowsyZoneLocation(currentIndex, zoneDescription);

    // 다발구역 진입 시 경고 표시 및 TTS 실행 (추가)
    static bool wasInDrowsyZone = false;
    if (inDrowsyZone && !wasInDrowsyZone) {
        // 새로운 다발구역 진입 시
        showDrowsyZoneAlert(zoneDescription);

        // TTS 실행
        speakDrowsyZoneWarning(zoneDescription);
    }
    wasInDrowsyZone = inDrowsyZone;

    // 맵 뷰 업데이트 (null 체크 추가)
    if (mapView) {
        mapView->updateNavigationInfo(
            currentPosition.latitude(),
            currentPosition.longitude(),
            speed,
            0.0  // 항상 북쪽 방향
            );
    }

    // 상태바에 속도 표시 (다발구역 정보 추가)
    if (inDrowsyZone) {
        statusBar()->showMessage(QString("현재 속도: %1 km/h - %2").arg(speed).arg(zoneDescription));
    } else {
        statusBar()->showMessage(QString("현재 속도: %1 km/h").arg(speed));
    }

    // 졸음 감지 시뮬레이션 (비상 상황인 경우)
    if (isEmergency && drowsinessDetector) {
        static QElapsedTimer lastAlert;
        if (!lastAlert.isValid() || lastAlert.elapsed() > 10000) { // 10초 간격으로 제한
            drowsinessDetector->simulateDetection();
            lastAlert.start();
        }
    }

    // 다음 인덱스로 이동
    currentIndex++;
}
