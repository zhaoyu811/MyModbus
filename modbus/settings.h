#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <windows.h>

namespace Ui {
class settings;
}

class settings : public QMainWindow
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = 0);
    ~settings();

    DCB dcb;
    QString portName;

    void UpdateSerialPort(void);

private slots:
    void on_pB_Aplly_clicked();

private:
    Ui::settings *ui;

};

#endif // SETTINGS_H
