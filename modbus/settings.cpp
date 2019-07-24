#include "settings.h"
#include "ui_settings.h"
#include <qDebug>
#include <string.h>

settings::settings(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("设置串口"));
    QStringList serialPortList;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        serialPortList.append(info.portName());
    }
    ui->cB_SerialPort->addItems(serialPortList);
    serial.name = ui->cB_SerialPort->currentText();
}

settings::~settings()
{
    delete ui;
}

void settings::on_pB_Aplly_clicked()
{
    serial.name = ui->cB_SerialPort->currentText();
    serial.baudRate = ui->cB_Baudrate->currentText().toInt();
    serial.dataBits = ui->cB_DataBits->currentText().toInt();
    serial.stopBits = ui->cB_StopBits->currentText().toInt();

    if(ui->cB_Parity->currentText() == "No")
    {
        serial.parity = 'N';
    }
    else if(ui->cB_Parity->currentText() == "Even")
    {
        serial.parity = 'E';
    }
    else if(ui->cB_Baudrate->currentText() == "Odd")
    {
        serial.parity = 'O';
    }
    else
    {
        serial.parity = 'N';
    }

    this->hide();
}
