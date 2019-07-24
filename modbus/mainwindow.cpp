#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QIcon icon(":./serialport.svg");
    this->setWindowIcon(icon);
    ui->setupUi(this);
    this->setWindowTitle(QString("Modbus调试"));
    ui->centralWidget->setLayout(ui->hL_MainLayout);
    s = new settings(this);
    this->setTabOrder(ui->lE_Slave_W, ui->lE_Registor_W);
    this->setTabOrder(ui->lE_Registor_W, ui->lE_Data_W);
    this->setTabOrder(ui->lE_Data_W, ui->pB_Send_W);
    this->setTabOrder(ui->pB_Send_W, ui->lE_Slave_R);
    this->setTabOrder(ui->lE_Slave_R, ui->lE_Registor_R);
    this->setTabOrder(ui->lE_Registor_R, ui->lE_Data_R);
    this->setTabOrder(ui->lE_Data_R, ui->pB_Send_R);

    connect(ui->action, &QAction::triggered, this, &MainWindow::SettingsUI_Show);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SettingsUI_Show(void)
{
    s->show();
}

void MainWindow::on_pB_Send_W_clicked()
{
    qint64 start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    modbus = new Modbus();
    modbus->ModbusSetSerialPort(s->serial.name);

    if(modbus->ModbusWrite(ui->lE_Slave_W->text().toInt(), ui->lE_Registor_W->text().toInt(), ui->lE_Data_W->text().toInt())==0)
    {
        qint64 end = QDateTime::currentDateTime().toMSecsSinceEpoch();
        QMessageBox::information(this, tr("写入成功"), tr("写入成功,耗时%1ms").arg(end-start));
        qDebug()<<"写入成功";
    }
    else
    {
        qint64 end = QDateTime::currentDateTime().toMSecsSinceEpoch();
        QMessageBox::information(this, tr("写入失败"), tr("写入失败,耗时%1ms").arg(end-start));
        qDebug()<<"写入失败";
    }
    delete modbus;
}

void MainWindow::on_pB_Send_R_clicked()
{
    unsigned short dest[256];

    qint64 start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    modbus = new Modbus();
    modbus->ModbusSetSerialPort(s->serial.name);
    if(modbus->ModbusRead(ui->lE_Slave_R->text().toInt(), ui->lE_Registor_R->text().toInt(), ui->lE_Data_R->text().toInt(), dest)==0)
    {
        qint64 end = QDateTime::currentDateTime().toMSecsSinceEpoch();
        QString message = "数据:";
        for(int i=0; i<ui->lE_Data_R->text().toInt(); i++)
        {
            message += tr("%1 ").arg(dest[i]);
            qDebug()<< dest[i];
        }
        QMessageBox::information(this, tr("读取成功"), tr("%1,耗时%2ms").arg(message).arg(end-start));
        qDebug()<<"读取成功";
    }
    else
    {
        qint64 end = QDateTime::currentDateTime().toMSecsSinceEpoch();
        QMessageBox::information(this, tr("读取失败"),tr("读取失败,耗时%1ms").arg(end-start));
        qDebug()<<"读取失败";
    }
    delete modbus;
}
