#include "alertdialog.h"
#include "ui_alertdialog.h"
#include <QPixmap>
#include <QGraphicsDropShadowEffect>

AlertDialog::AlertDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlertDialog)
{
    ui->setupUi(this);

    // 버튼 클릭 연결
    connect(ui->confirmButton, &QPushButton::clicked, this, &AlertDialog::onConfirmClicked);

    // 배경을 어둡게 설정
    setStyleSheet("QDialog {"
                  "background-color: rgba(40, 40, 40, 220);"
                  "border-radius: 15px;"
                  "}"
                  "QLabel#messageLabel {"
                  "color: white;"
                  "font-size: 14pt;"
                  "font-weight: bold;"
                  "background-color: rgba(255, 82, 82, 180);"
                  "border-radius: 10px;"
                  "padding: 15px;"
                  "}"
                  "QPushButton#confirmButton {"
                  "background-color: #e53935;"
                  "color: white;"
                  "border-radius: 8px;"
                  "padding: 8px 16px;"
                  "font-weight: bold;"
                  "font-size: 12pt;"
                  "}"
                  "QPushButton#confirmButton:hover {"
                  "background-color: #f44336;"
                  "}"
                  );

    // 경고 아이콘 설정
    QPixmap warningIcon(":/images/warning.png");
    if (!warningIcon.isNull()) {
        ui->iconLabel->setPixmap(warningIcon.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // 5초 후 자동 닫힘 타이머 설정
    autoCloseTimer = new QTimer(this);
    autoCloseTimer->setSingleShot(true);
    connect(autoCloseTimer, &QTimer::timeout, this, &AlertDialog::accept);
    autoCloseTimer->start(5000); // 5초 후 닫힘
}

AlertDialog::~AlertDialog()
{
    delete ui;
}

void AlertDialog::onConfirmClicked()
{
    accept(); // 확인 버튼 누르면 다이얼로그 닫기
}
