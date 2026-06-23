#include "abstractportwrapper.h"

AbstractPortWrapper::AbstractPortWrapper(QObject *parent, qint32 conId, PortType portType, AbstractPortWrapper* target) :
    QObject(parent), targetPort(target), connectionId(conId), _portType(portType)
{
    disconnect(this, &AbstractPortWrapper::dataSend, nullptr, nullptr);

    if (targetPort == nullptr)
        return;

    connect(this, &AbstractPortWrapper::dataSend,
            targetPort, &AbstractPortWrapper::Accept);
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

void AbstractPortWrapper::SendToTargetPort(const QByteArray &data)
{
    if (targetPort == nullptr || data.isEmpty())
        return;

    emit dataSend(data);
}
