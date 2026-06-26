#include "serialportwrapper.h"

SerialPortWrapper::SerialPortWrapper(const QString &portName, QObject *parent, int conId, AbstractPortWrapper* target, qint32 baudRate) :
    AbstractPortWrapper(parent, conId, PortType::SerialPort, target), serialPort(new QSerialPort(this)), _portName(portName), _baudRate(baudRate)
{}

SerialPortWrapper::SerialPortWrapper(const QJsonObject& obj, QObject* parent, qint32 conId, AbstractPortWrapper* target, bool& isSucceeded) :
    AbstractPortWrapper(parent, conId, PortType::SerialPort, target)
{
    isSucceeded = FromJson(obj);

    if (isSucceeded)
        serialPort = new QSerialPort(this);
}

SerialPortWrapper::~SerialPortWrapper()
{
    Stop();
}

void SerialPortWrapper::Start()
{
    if (serialPort->isOpen())
        Stop();

    serialPort->setPortName(_portName);
    serialPort->setBaudRate(_baudRate);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort->open(QIODevice::ReadWrite))
    {
        QString error = QString("Failed to open port %1: %2")
                        .arg(_portName)
                        .arg(serialPort->errorString());

        emit errorOccurred(connectionId, error);
    }

    if (targetPort == nullptr)
        return;

    connect(serialPort, &QSerialPort::readyRead,
                this, &SerialPortWrapper::OnReadReady);
}

void SerialPortWrapper::OnReadReady() //With target port
{
    if (targetPort == nullptr)
    {
        QString error = QString("Target port is null, yet OnReadReady event is subscribed!");
        emit errorOccurred(connectionId, error);
        return;
    }

    QByteArray data = serialPort->readAll();

    SendToTargetPort(data);
}

void SerialPortWrapper::Accept(const QByteArray &data) //Without target port
{
    if (!serialPort->isOpen())
    {
        emit errorOccurred(connectionId, "Port is not open");
        return;
    }

    qint64 bytesWritten = serialPort->write(data);

    if (bytesWritten == -1)
    {
        emit errorOccurred(connectionId, QString("Write failed: %1")
                          .arg(serialPort->errorString()));
    }
    else
    {
        serialPort->flush();
        AbstractPortWrapper::Accept(data);
    }
}

void SerialPortWrapper::Stop()
{
    if (!serialPort->isOpen())
        return;

    serialPort->close();
}

QJsonObject SerialPortWrapper::ToJson() const
{
    QJsonObject obj = AbstractPortWrapper::ToJson();
    obj["portName"] = _portName;
    obj["baudRate"] = _baudRate;
    return obj;
}

bool SerialPortWrapper::FromJson(const QJsonObject &obj)
{
    if (!AbstractPortWrapper::FromJson(obj))
        return false;

    if (!obj.contains("portName"))
        return false;

    _portName = obj["portName"].toString();

    if (!obj.contains("baudRate"))
        return false;

    _baudRate = obj["baudRate"].toInt(QSerialPort::Baud115200);
    return true;
}

QString SerialPortWrapper::GetTypeName() const
{
    return "SerialPortWrapper";
}

QString SerialPortWrapper::GetPortName()
{
    return serialPort->portName();
}

quint32 SerialPortWrapper::GetBaudRate()
{
    return _baudRate;
}
