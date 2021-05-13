#include "settings.h"
#include "ui_settings.h"
#include <qDebug>
#include <string.h>
#include <QSettings>

settings::settings(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::settings)
{
    qDebug()<<"UartUI::UartUI(QDialog *parent)";
    ui->setupUi(this);
    this->setWindowTitle(QString("串口设置"));
    UpdateSerialPort();
}

settings::~settings()
{
    delete ui;
}

void settings::UpdateSerialPort(void)
{
    QSettings *setting = new QSettings("setting.ini",QSettings::IniFormat);
    //1.动态获取可用串口
    QStringList serialPortList;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        serialPortList.append(info.portName());
    }
    ui->cB_SerialPort->clear();
    ui->cB_SerialPort->addItems(serialPortList);

    QString tmp = setting->value("uart/serialport").toString();
    if(tmp.isEmpty())
    {
        //没有这个配置，设找到的第一个串口
        if(serialPortList.length()>1)
        {
            setting->setValue("uart/serialport", serialPortList.at(0));
            ui->cB_SerialPort->setCurrentText(serialPortList.at(0));
        }
        else
        {
            setting->setValue("uart/serialport", tr("COM1"));
        }
    }
    else
    {
        //存在这个串口，把这个串口设为首位
        ui->cB_SerialPort->setCurrentText(tmp);
    }
    //2. 波特率
    QStringList baudRateList;
    foreach(qint32 baudRate, QSerialPortInfo::standardBaudRates())
    {
        baudRateList.append(QString::number(baudRate));
    }
    baudRateList.append(QString::number(1000000));
    ui->cB_Baudrate->clear();
    ui->cB_Baudrate->addItems(baudRateList);

    tmp = setting->value("uart/baudrate").toString();
    if(tmp.isEmpty())
    {
        //没有这个配置项，够造配置
        ui->cB_Baudrate->setCurrentText(QString::number(1000000));
        setting->setValue("uart/baudrate", "1000000");
    }
    else
    {
        ui->cB_Baudrate->setCurrentText(tmp);
    }

    //3. 数据位
    ui->cB_DataBits->clear();
    ui->cB_DataBits->addItem("5");
    ui->cB_DataBits->addItem("6");
    ui->cB_DataBits->addItem("7");
    ui->cB_DataBits->addItem("8");

    tmp = setting->value("uart/databits").toString();
    if(tmp.isEmpty())
    {
        //没有这个配置项，设为默认 8
        ui->cB_DataBits->setCurrentText("8");
        setting->setValue("uart/databits", "8");
    }
    else
    {
        ui->cB_DataBits->setCurrentText(tmp);
    }

    //4. 停止位
    ui->cB_StopBits->clear();
    ui->cB_StopBits->addItem("1");
    ui->cB_StopBits->addItem("1.5");
    ui->cB_StopBits->addItem("2");

    tmp = setting->value("uart/stopbits").toString();
    if(tmp.isEmpty())
    {
        //没有这个配置项，设为默认1
        ui->cB_StopBits->setCurrentText("1");
        setting->setValue("uart/stopbits", "1");
    }
    else
    {
        ui->cB_StopBits->setCurrentText(tmp);
    }

    //5. 校验位
    ui->cB_Parity->clear();
    ui->cB_Parity->addItem("None");
    ui->cB_Parity->addItem("Odd");
    ui->cB_Parity->addItem("Even");

    tmp = setting->value("uart/parity").toString();
    if(tmp.isEmpty())
    {
        ui->cB_Parity->setCurrentText("None");
        setting->setValue("uart/parity", "None");
    }
    else
    {
        ui->cB_Parity->setCurrentText(tmp);
    }

    portName = ui->cB_SerialPort->currentText();
    dcb.BaudRate = ui->cB_Baudrate->currentText().toULong();
    dcb.ByteSize = (unsigned char)ui->cB_DataBits->currentText().toUShort();
    dcb.StopBits = (unsigned char)ui->cB_StopBits->currentIndex();
    dcb.Parity = (unsigned char)ui->cB_Parity->currentIndex();

    delete setting;
}

void settings::on_pB_Aplly_clicked()
{
    QSettings *setting = new QSettings("setting.ini", QSettings::IniFormat);

    setting->setValue("uart/serialport", ui->cB_SerialPort->currentText());
    setting->setValue("uart/baudrate", ui->cB_Baudrate->currentText());
    setting->setValue("uart/databits", ui->cB_DataBits->currentText());
    setting->setValue("uart/stopbits", ui->cB_StopBits->currentText());
    setting->setValue("uart/parity", ui->cB_Parity->currentText());

    portName = ui->cB_SerialPort->currentText();
    dcb.BaudRate = ui->cB_Baudrate->currentText().toULong();
    dcb.ByteSize = (unsigned char)ui->cB_DataBits->currentText().toUShort();
    dcb.StopBits = (unsigned char)ui->cB_StopBits->currentIndex();
    dcb.Parity = (unsigned char)ui->cB_Parity->currentIndex();

    delete setting;
    this->close();
}
