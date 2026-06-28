#include "updclientwrapper.h"

UpdClientWrapper::UpdClientWrapper(QMap<QString, quint16>& listOfServers, quint16 localPort, QObject *parent, qint32 conId,
                                   AbstractPortWrapper* target)
    : AbstractPortWrapper(parent, conId, PortType::TcpPort, target), _localPort(localPort)
{
    for (QMap<QString, quint16>::iterator it = listOfServers.begin();
            it != listOfServers.end(); ++it)
    {
        QHostAddress addr(it.key());

        if (!addr.isNull())
            _listOfServers.insert(it.key(), it.value());
        else
        {
            QString error = QString("Invalid IP addres: %1").arg(it.key());
            emit errorOccurred(connectionId, error);
        }
    }
}

UpdClientWrapper::UpdClientWrapper(const QJsonObject& obj, QObject* parent, qint32 conId, AbstractPortWrapper* target, bool& isSucceeded) :
    AbstractPortWrapper(parent, conId, PortType::TcpPort, target)
{
    isSucceeded = FromJson(obj);
}

UpdClientWrapper::~UpdClientWrapper()
{
    Stop();
}

void UpdClientWrapper::Start()
{
    if (socket->state() == QAbstractSocket::BoundState)
    {
        errorOccurred(connectionId, "UPD socket is already bound!");
        return;
    }
}
