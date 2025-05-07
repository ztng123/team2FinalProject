#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    bool translatorLoaded = false;

    for (const QString &locale : uiLanguages) {
        const QString baseName = "carproject_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            translatorLoaded = true;
            break;
        }
    }

    if (!translatorLoaded) {
        qDebug() << "No translator loaded";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
