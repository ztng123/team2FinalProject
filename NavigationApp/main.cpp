#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "drowsinessdetector.h"
#include <Qfile>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 졸음 감지기 인스턴스 생성
    DrowsinessDetector drowsinessDetector;

    QQmlApplicationEngine engine;

    // 졸음 감지기를 QML에 노출시킴
    engine.rootContext()->setContextProperty("drowsinessDetector", &drowsinessDetector);

    // QML 메인 파일 로드
    // 리소스 시스템 대신 직접 파일 경로 사용
    engine.load(QUrl::fromLocalFile("C:/Users/g_gyuuuuuu/Desktop/NavigationApp/Main.qml"));


    if (QFile::exists("C:\\Users\\g_gyuuuuuu\\Desktop\\NavigationApp\\build\\Desktop_Qt_6_9_0_MinGW_64_bit-Debug\\location.png"))
    {
        QFile::remove("C:\\Users\\g_gyuuuuuu\\Desktop\\NavigationApp\\build\\Desktop_Qt_6_9_0_MinGW_64_bit-Debug\\location.png");
    }

    QFile::copy("C:\\Users\\g_gyuuuuuu\\Desktop\\NavigationApp\\location.png", "C:\\Users\\g_gyuuuuuu\\Desktop\\NavigationApp\\build\\Desktop_Qt_6_9_0_MinGW_64_bit-Debug\\location.png");
    return app.exec();

}


