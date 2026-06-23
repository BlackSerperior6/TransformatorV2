#ifndef SERIALPORTWRAPPER_H
#define SERIALPORTWRAPPER_H

#include "abstractportwrapper.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class SerialPortWrapper : public AbstractPortWrapper
{
    Q_OBJECT

public:
    SerialPortWrapper(const QString &portName, QObject *parent, int conId, AbstractPortWrapper* target, qint32 baudRate = QSerialPort::Baud115200);

    ~SerialPortWrapper();

    void Start() override;

    void Stop() override;

    QString GetPortName();

protected slots:
    void Accept(const QByteArray &data) override;

private slots:
    void OnReadReady();

private:
    QSerialPort *serialPort;

    QString _portName;
    qint32 _baudRate;
};

#endif // SERIALPORTWRAPPER_H
