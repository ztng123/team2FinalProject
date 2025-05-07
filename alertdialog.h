#ifndef ALERTDIALOG_H
#define ALERTDIALOG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class AlertDialog;
}

class AlertDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlertDialog(QWidget *parent = nullptr);
    ~AlertDialog();

private slots:
    void onConfirmClicked();

private:
    Ui::AlertDialog *ui;
    QTimer *autoCloseTimer;
};

#endif // ALERTDIALOG_H
