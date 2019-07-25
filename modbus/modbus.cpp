#include "modbus.h"
#include <QDebug>

/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}

Modbus::Modbus()
{
    qDebug()<<"Modbus::Modbus()";
}

Modbus::Modbus(const QString &name)
{
    qDebug()<<"Modbus::Modbus(const QString &name)";

    WCHAR wszClassName[5];
    memset(wszClassName, 0, sizeof(wszClassName));
    MultiByteToWideChar(CP_ACP, 0, name.toStdString().data(), strlen(name.toStdString().data()) + 1, wszClassName,sizeof(wszClassName) / sizeof(wszClassName[0]));

    uart = CreateFile(wszClassName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if(uart==INVALID_HANDLE_VALUE)
    {
        DWORD dwError = GetLastError();
        qDebug()<<"打开串口失败:" << dwError;
    }
    else
    {
#if 0
        COMMTIMEOUTS结构的定义为： typedef struct _COMMTIMEOUTS {
            DWORD ReadIntervalTimeout; //读间隔超时
            DWORD ReadTotalTimeoutMultiplier; //读时间系数
            DWORD ReadTotalTimeoutConstant; //读时间常量
            DWORD WriteTotalTimeoutMultiplier; // 写时间系数
            DWORD WriteTotalTimeoutConstant; //写时间常量
        } COMMTIMEOUTS,*LPCOMMTIMEOUTS;

        COMMTIMEOUTS结构的成员都以毫秒为单位。总超时的计算公式是：
        总超时＝时间系数×要求读/写的字符数＋时间常量
        例如，要读入10个字符，那么读操作的总超时的计算公式为：
        读总超时＝ReadTotalTimeoutMultiplier×10＋ReadTotalTimeoutConstant
        可以看出：间隔超时和总超时的设置是不相关的，这可以方便通信程序灵活地设置各种超时。
#endif

        qDebug()<<"打开串口成功";
        SetupComm(uart, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024
        COMMTIMEOUTS TimeOuts;
        //设定读超时
        TimeOuts.ReadIntervalTimeout = 10;
        TimeOuts.ReadTotalTimeoutMultiplier = 5;
        TimeOuts.ReadTotalTimeoutConstant = 5;
        //设定写超时
        TimeOuts.WriteTotalTimeoutMultiplier = 50;
        TimeOuts.WriteTotalTimeoutConstant = 2;
        SetCommTimeouts(uart, &TimeOuts); //设置超时
        DCB dcb;
        GetCommState(uart, &dcb);
        dcb.BaudRate = 9600; //波特率为9600
        dcb.ByteSize = 8; //每个字节有8位
        dcb.Parity = NOPARITY; //无奇偶校验位
        dcb.StopBits = ONESTOPBIT; //1个停止位
        SetCommState(uart, &dcb);
    }
}

Modbus::~Modbus()
{
    qDebug()<<"Modbus::~Modbus()";
    if(uart!=NULL)
        CloseHandle(uart);
}

void Modbus::ModbusSetSerialPort(const QString &name)
{
    WCHAR wszClassName[20];

    memset(wszClassName, 0, sizeof(wszClassName));
    QString serialport = QString("\\\\.\\%1").arg(name);
    qDebug()<<serialport;
    MultiByteToWideChar(CP_ACP, 0, serialport.toStdString().data(), strlen(serialport.toStdString().data()) + 1, wszClassName,sizeof(wszClassName) / sizeof(wszClassName[0]));

    uart = CreateFile(wszClassName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if(uart==INVALID_HANDLE_VALUE)
    {
        DWORD dwError = GetLastError();
        qDebug()<<"打开串口失败:" << dwError;
    }
    else
    {
        qDebug()<<"打开串口成功";
        SetupComm(uart, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024
        COMMTIMEOUTS TimeOuts;
        //设定读超时
        TimeOuts.ReadIntervalTimeout = 20;
        TimeOuts.ReadTotalTimeoutMultiplier = 5;
        TimeOuts.ReadTotalTimeoutConstant = 300;
        //设定写超时
        TimeOuts.WriteTotalTimeoutMultiplier = 5;
        TimeOuts.WriteTotalTimeoutConstant = 300;
        SetCommTimeouts(uart, &TimeOuts); //设置超时
        DCB dcb;
        GetCommState(uart, &dcb);
        dcb.BaudRate = 9600; //波特率为9600
        dcb.ByteSize = 8; //每个字节有8位
        dcb.Parity = NOPARITY; //无奇偶校验位
        dcb.StopBits = ONESTOPBIT; //1个停止位
        SetCommState(uart, &dcb);
    }
}

int Modbus::ModbusRead(int slaveid, int reg, int num, unsigned short *dest)
{
    //1. 构造发送数据
    unsigned char buf[8];
    buf[0] = slaveid;
    buf[1] = 0x04;
    buf[2] = reg>>8;
    buf[3] = reg;
    buf[4] = num>>8;
    buf[5] = num;

    unsigned short ret = crc16(buf, 6);
    buf[6] = ret>>8;
    buf[7] = ret;

    //2. 将数据发送出去
    DWORD bytesWritten;
    PurgeComm(uart, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);   //清空缓存
    WriteFile(uart, buf, 8, &bytesWritten, NULL);
    if(bytesWritten != 8)
    {
        qDebug()<<"写入错误"<<bytesWritten;
        return -1;
    }
    else
    {
        //3. 读取返回数据
        unsigned char retbuf[128];
        DWORD bytesRead;
        ReadFile(uart, retbuf, (num*2)+5, &bytesRead, NULL);
        if(bytesRead != ((num*2)+5))
        {
            qDebug()<<"读到数据长度错误"<<bytesRead<<"应该读到数据长度"<<((num*2)+5);
            return -1;
        }
        else
        {
            unsigned short ret = crc16(retbuf, bytesRead);

            if(ret!=0)
            {
                qDebug()<<"数据校验错误";
                return -1;
            }
            else
            {
                for(int i=0; i<num; i++)
                {
                    unsigned char tmp;
                    tmp = *(retbuf+(i*2)+3);
                    *(retbuf+(i*2)+3) = *(retbuf+(i*2)+4);
                    *(retbuf+(i*2)+4) = tmp;
                }
                memcpy(dest, retbuf+3, num*2);

                return 0;
            }
        }
    }
}

int Modbus::ModbusWrite(int slaveid, int reg, int val)
{
    unsigned char buf[8];
    buf[0] = slaveid;
    buf[1] = 0x06;
    buf[2] = reg>>8;
    buf[3] = reg;
    buf[4] = val>>8;
    buf[5] = val;

    unsigned short ret = crc16(buf, 6);
    buf[6] = ret>>8;
    buf[7] = ret;

    DWORD bytesWritten;
    PurgeComm(uart, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    WriteFile(uart, buf, 8, &bytesWritten, NULL);
    if(bytesWritten != 8)
    {
        qDebug()<<"写入错误"<<bytesWritten;
        return -1;
    }
    else
    {
        unsigned char retbuf[8];
        DWORD bytesRead;
        ReadFile(uart, retbuf, 8, &bytesRead, NULL);
        if(bytesRead != 8)
        {
            qDebug()<<"读到数据长度错误"<<bytesRead;
            return -1;
        }
        else
        {
            unsigned short ret = crc16(retbuf, 8);

            if(ret!=0)
            {
                qDebug()<<"数据校验错误";
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
}

int Modbus::ModbusWriteRegisters(int slaveid, int reg, int num, unsigned short *data)
{
    unsigned char *buf = new unsigned char[num*2+9];

    buf[0] = slaveid;
    buf[1] = 0x10;
    buf[2] = reg>>8;
    buf[3] = reg;
    buf[4] = num>>8;
    buf[5] = num;
    buf[6] = num*2;

    for(int i=0; i<num; i++)
    {
        buf[7+(i*2)] = data[i]>>8;
        buf[8+(i*2)] = data[i];
    }

    unsigned short ret = crc16(buf, (num*2+7));
    buf[num*2+7] = ret>>8;
    buf[num*2+8] = ret;

    DWORD bytesWritten;
    PurgeComm(uart, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
    WriteFile(uart, buf, num*2+9, &bytesWritten, NULL);
    if(bytesWritten != (num*2+9))
    {
        qDebug()<<"写入错误"<<bytesWritten;
        return -1;
    }
    else
    {
        unsigned char *retbuf = new unsigned char[8];
        DWORD bytesRead;
        ReadFile(uart, retbuf, 8, &bytesRead, NULL);
        if(bytesRead != 8)
        {
            qDebug()<<"读到数据长度错误"<<bytesRead;
            return -1;
        }
        else
        {
            unsigned short ret = crc16(retbuf, 8);

            if(ret!=0)
            {
                qDebug()<<"数据校验错误";
                return -1;
            }
            else
            {
                return 0;
            }
        }
        delete[] retbuf;
    }
    delete[] buf;
}
