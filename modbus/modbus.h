#ifndef MODBUS_H
#define MODBUS_H

#include <windows.h>
#include <QString>

class Modbus
{
public:
    Modbus();
    Modbus(const QString &name);
    ~Modbus();

    void ModbusSetSerialPort(const QString &name);
    int ModbusRead(int slaveid, int reg, int num, unsigned short *dest);
    int ModbusWrite(int slaveid, int reg, int val);
private:
    HANDLE uart = NULL;
};

#endif // MODBUS_H
