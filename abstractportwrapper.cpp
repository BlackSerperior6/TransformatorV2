#include "abstractportwrapper.h"

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

void AbstractPortWrapper::SendToTargetPort(const QByteArray &data)
{
    if (targetPort == nullptr || data.isEmpty())
        return;

    emit dataSend(data);
}
