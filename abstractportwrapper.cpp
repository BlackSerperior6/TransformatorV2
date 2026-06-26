#include "abstractportwrapper.h"
#include "serialportwrapper.h"
#include "tcpclientwrapper.h"
#include "tcpserverwrapper.h"

AbstractPortWrapper::AbstractPortWrapper(QObject *parent, qint32 conId, PortType portType, AbstractPortWrapper* target) :
    QObject(parent), _portType(portType), connectionId(conId), _targetPort(target)
{
    if (_targetPort == nullptr)
        return;

    connect(this, &AbstractPortWrapper::dataSend,
            _targetPort, &AbstractPortWrapper::Accept);
}

AbstractPortWrapper::~AbstractPortWrapper()
{

}

void AbstractPortWrapper::Accept(const QByteArray &data)
{
    if (data.isEmpty())
        return;

    emit dataReceived(data);
}

PortType AbstractPortWrapper::GetPortType()
{
    return _portType;
}

void AbstractPortWrapper::SetTargetPort(AbstractPortWrapper* targetPort)
{
    _targetPort = targetPort;
}

QJsonObject AbstractPortWrapper::ToJson() const
{
    QJsonObject obj;
    obj["type"] = GetTypeName();
    return obj;
}

void AbstractPortWrapper::SendToTargetPort(const QByteArray &data)
{
    if (_targetPort == nullptr || data.isEmpty())
        return;

    emit dataSend(data);
}

AbstractPortWrapper *AbstractPortWrapper::CreateFromJson(const QJsonObject &obj, QObject *parent, qint32 conId, AbstractPortWrapper *target)
{
    if (!obj.contains("type"))
        return nullptr;

    QString type = obj["type"].toString();

    AbstractPortWrapper* result;
    bool isSucceeded;

    if (type == "SerialPortWrapper")
        result = new SerialPortWrapper(obj, parent, conId, target, isSucceeded);
    else if (type == "TcpClientWrapper")
        result = new TcpClientWrapper(obj, parent, conId, target, isSucceeded);
    else if (type == "TcpServerWrapper")
        result = new TcpServerWrapper(obj, parent, conId, target, isSucceeded);

    if (!isSucceeded)
        return nullptr;

    return result;
}
