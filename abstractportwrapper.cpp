#include "abstractportwrapper.h"
#include "serialportwrapper.h"
#include "tcpclientwrapper.h"
#include "tcpserverwrapper.h"

AbstractPortWrapper::AbstractPortWrapper(QObject *parent, qint32 conId, PortType portType, AbstractPortWrapper* target) :
    QObject(parent), _portType(portType), targetPort(target), connectionId(conId)
{
    if (targetPort == nullptr)
        return;

    connect(this, &AbstractPortWrapper::dataSend,
            targetPort, &AbstractPortWrapper::Accept);
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

QJsonObject AbstractPortWrapper::ToJson() const
{
    QJsonObject obj;
    obj["type"] = GetTypeName();
    obj["portType"] = static_cast<int>(_portType);
    obj["connectionId"] = connectionId;
    return obj;
}

bool AbstractPortWrapper::FromJson(const QJsonObject& obj)
{
    if (!obj.contains("portType"))
        return false;

    _portType = static_cast<PortType>(obj["portType"].toInt());

    if (!obj.contains("connectionId"))
        return false;

    connectionId = obj["connectionId"].toInt();

    return true;
}

void AbstractPortWrapper::SendToTargetPort(const QByteArray &data)
{
    if (targetPort == nullptr || data.isEmpty())
        return;

    emit dataSend(data);
}

AbstractPortWrapper *AbstractPortWrapper::CreateFromJson(const QJsonObject &obj, QObject *parent, qint32 conId, AbstractPortWrapper *target)
{
    if (!obj.contains("type"))
        return nullptr;

    QString type = obj["type"].toString();

    if (type == "SerialPortWrapper")
        return new SerialPortWrapper(obj, parent, conId, target);
    else if (type == "TcpClientWrapper")
        return new TcpClientWrapper(obj, parent, conId, target);
    else if (type == "TcpServerWrapper")
        return new TcpServerWrapper(obj, parent, conId, target);

    return nullptr;
}
