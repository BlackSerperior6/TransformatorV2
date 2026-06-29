#include "updclientwrapper.h"

UpdClientWrapper::UpdClientWrapper(QMap<QString, quint16>& listOfServers, quint16 localPort, QObject *parent, qint32 conId,
                                   AbstractPortWrapper* target)
    : AbstractPortWrapper(parent, conId, PortType::TcpPort, target), _localPort(localPort), shouldSetNetworkPortAutomatically(false)
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

UpdClientWrapper::UpdClientWrapper(QMap<QString, quint16> &listOfServers, QObject *parent, qint32 conId, AbstractPortWrapper *target) :
    AbstractPortWrapper(parent, conId, PortType::TcpPort, target), shouldSetNetworkPortAutomatically(true)
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

    if (!shouldSetNetworkPortAutomatically)
    {
        if (!socket->bind(QHostAddress::AnyIPv4, _localPort, QUdpSocket::ShareAddress))
        {
           emit errorOccurred(connectionId, QString("Error on biding UPD client: %1").arg(socket->errorString()));
           return;
        }
    }
    else
    {
        if (!socket->bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ShareAddress))
        {
           emit errorOccurred(connectionId, QString("Error on biding UPD client: %1").arg(socket->errorString()));
           return;
        }
    }
}

void UpdClientWrapper::Stop()
{
    if (socket->state() != QAbstractSocket::BoundState)
        return;

    socket->close();
}

QJsonObject UpdClientWrapper::ToJson() const
{
    QJsonObject obj = AbstractPortWrapper::ToJson();

    obj["localPort"] = _localPort;
    obj["shouldSetNetworkPortAutomatically"] = shouldSetNetworkPortAutomatically;

    QJsonObject serversListObj;

    for (auto it = _listOfServers.begin(); it != _listOfServers.end(); ++it)
        serversListObj[it.key()] = it.value();

    obj["serverList"] = serversListObj;

    return obj;
}

bool UpdClientWrapper::FromJson(const QJsonObject &obj)
{
    if (!obj.contains("localPort"))
        return false;

    _localPort = obj["localPort"].toInt();

    if (!obj.contains("shouldSetNetworkPortAutomatically"))
        return false;

    shouldSetNetworkPortAutomatically = obj["shouldSetNetworkPortAutomatically"].toBool();

    if (!obj.contains("serverList"))
        return false;

    QJsonObject serversListObj = obj["serverList"].toObject();

    for (auto it = serversListObj.begin(); it != serversListObj.end(); ++it)
        _listOfServers[it.key()] = it.value().toInt();

    return true;
}

QString UpdClientWrapper::GetTypeName() const
{
    return "UpdClientWrapper";
}

quint16 UpdClientWrapper::GetLocalPort() const
{
    return _localPort;
}

QMap<QString, quint16> UpdClientWrapper::GetListOfServers() const
{
    return _listOfServers;
}

void UpdClientWrapper::Accept(const QByteArray &data)
{
    if (socket->state() != QAbstractSocket::BoundState)
    {
        emit errorOccurred(connectionId, "Socket is unbound yet attempts to send data!");
        return;
    }

    for (auto it = _listOfServers.begin(); it != _listOfServers.end(); ++it)
    {
        qint64 bytesSent = socket->writeDatagram(data, QHostAddress(it.key()), it.value());

        if (bytesSent != -1)
            continue;

        emit errorOccurred(connectionId, QString("Failed to send data: %1").arg(socket->errorString()));
        continue;
    }
}

void UpdClientWrapper::onUpdClientError(QAbstractSocket::SocketError socketError)
{
    QString errorMsg = socket->errorString();

    emit errorOccurred(connectionId, errorMsg);
}
