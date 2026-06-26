#include "tcpserverwrapper.h"

TcpServerWrapper::TcpServerWrapper(quint16 networkPort, QSet<QString> allowedIps, QObject *parent, qint32 conId, AbstractPortWrapper* target)
    : AbstractPortWrapper(parent, conId, PortType::TcpPort, target), _networkPort(networkPort), shouldSetNetworkPortAutomatically(false),
      server(new QTcpServer(this))

{
    SetAllowedIps(allowedIps);

    if (_targetPort == nullptr)
        emit errorOccurred(connectionId, QString("TCP server must have a target port!"));
}

TcpServerWrapper::TcpServerWrapper(QSet<QString> allowedIps, QObject *parent, qint32 conId, AbstractPortWrapper *target)
    : AbstractPortWrapper(parent, conId, PortType::TcpPort, target),
      shouldSetNetworkPortAutomatically(true), server(new QTcpServer(this))
{
    SetAllowedIps(allowedIps);

    if (_targetPort == nullptr)
        emit errorOccurred(connectionId, QString("TCP server must have a target port!"));
}

TcpServerWrapper::TcpServerWrapper(const QJsonObject& obj, QObject* parent, qint32 conId, AbstractPortWrapper* target, bool& isSucceeded) :
    AbstractPortWrapper(parent, conId, PortType::TcpPort, target)
{
    isSucceeded = FromJson(obj);

    if (isSucceeded)
        server = new QTcpServer(this);
}

TcpServerWrapper::~TcpServerWrapper()
{
    Stop();
}

void TcpServerWrapper::Start()
{
    if (server->isListening())
    {
        emit errorOccurred(connectionId, QString("Server is already listening"));
        return;
    }

    connect(server, &QTcpServer::newConnection,
                this, &TcpServerWrapper::onNewConnection);

    if (!shouldSetNetworkPortAutomatically)
    {
        if (!server->listen(QHostAddress::AnyIPv4, _networkPort))
        {
           emit errorOccurred(connectionId, QString("Error on starting listening server: %1").arg(server->errorString()));
           return;
        }
    }
    else
    {
        if (!server->listen(QHostAddress::Any))
        {
           emit errorOccurred(connectionId, QString("Error on starting listening server: %1").arg(server->errorString()));
           return;
        }
    }
}

void TcpServerWrapper::Stop()
{
    for (QTcpSocket *client : connectedClients)
    {
        client->close();
        client->deleteLater();
    }

    connectedClients.clear();

    if (server->isListening())
        server->close();
}

QJsonObject TcpServerWrapper::ToJson() const
{
    QJsonObject obj = AbstractPortWrapper::ToJson();

    obj["networkPort"] = _networkPort;
    obj["shouldSetNetworkPortAutomatically"] = shouldSetNetworkPortAutomatically;

    QJsonArray allowedIpsListObject;

    for (const auto &i : _allowedIps)
        allowedIpsListObject.append(i);

    obj["allowedIps"] = allowedIpsListObject;
}

bool TcpServerWrapper::FromJson(const QJsonObject &obj)
{
    if (!obj.contains("networkPort"))
        return false;

    _networkPort = obj["networkPort"].toInt();

    if (!obj.contains("shouldSetNetworkPortAutomatically"))
        return false;

    shouldSetNetworkPortAutomatically = obj["shouldSetNetworkPortAutomatically"].toBool();

    if (!obj.contains("allowedIps"))
        return false;

    QJsonArray arrayOfIps = obj["allowedIps"].toArray();

    for (const auto &i : arrayOfIps)
        _allowedIps.insert(i.toString());
}

QString TcpServerWrapper::GetTypeName() const
{
    return "TcpServerWrapper";
}

quint16 TcpServerWrapper::GetNetworkPort()
{
    return server->serverPort();
}

QSet<QString> TcpServerWrapper::GetAllowedIps()
{
    return _allowedIps;
}

void TcpServerWrapper::onReadyRead()
{
    qDebug() << "Read ready triggered!";

    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());

    if (!client)
        return;

    QByteArray data = client->readAll();

    SendToTargetPort(data);
}

void TcpServerWrapper::onNewConnection()
{
    qDebug() << "New Connection Triggered";

    while (server->hasPendingConnections())
    {
        QTcpSocket *client = server->nextPendingConnection();
        QHostAddress clientIp = client->peerAddress();

        if (!IsIpWhitelisted(clientIp))
        {
            qDebug() << "New Connection Triggered: ip found to be not whitelisted!";

            client->close();
            client->deleteLater();
            continue;
        }

        connectedClients.append(client);

        connect(client, &QTcpSocket::readyRead,
                this, &TcpServerWrapper::onReadyRead);
        connect(client, &QTcpSocket::disconnected,
                this, &TcpServerWrapper::onClientDisconnected);
        connect(client, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
                this, &TcpServerWrapper::onTcpSocketError);
    }
}

void TcpServerWrapper::onClientDisconnected()
{
    qDebug() << "Client disconnected triggered!";

    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());

    if (!client)
        return;

    QHostAddress clientIp = client->peerAddress();

    connectedClients.removeAll(client);
    client->deleteLater();
}

void TcpServerWrapper::onTcpSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "TCP Socket Error triggered!";

    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());

    if (!client)
        return;

    QString errorMsg = client->errorString();

    emit errorOccurred(connectionId, errorMsg);
}


void TcpServerWrapper::SetAllowedIps(QSet<QString>& allowedIps)
{
    _allowedIps.clear();

    for (const QString &ip : allowedIps)
    {
        QHostAddress addr(ip);

        if (!addr.isNull())
            _allowedIps.insert(ip);
        else
        {
            QString error = QString("Invalid IP addres: %1").arg(ip);
            emit errorOccurred(connectionId, error);
        }
    }
}

bool TcpServerWrapper::IsIpWhitelisted(const QHostAddress &ip) const
{
    if (_allowedIps.isEmpty())
        return true;

    QString ipString = ip.toString();

    qDebug() << "Checked ip string: " + ipString;

    return _allowedIps.contains(ipString);
}

void TcpServerWrapper::Accept(const QByteArray &data)
{
    emit errorOccurred(connectionId, QString("Accept slot triggered in a server wrapper!"));
}
