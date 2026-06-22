#include "tcpserverwrapper.h"

TcpServerWrapper::TcpServerWrapper(quint16 networkPort, QSet<QString> allowedIps, QObject *parent, qint32 conId, AbstractPortWrapper* target)
    : AbstractPortWrapper(parent, conId, target), _networkPort(networkPort), server(new QTcpServer(this))
{
    SetAllowedIps(allowedIps);

    connect(server, &QTcpServer::newConnection,
                this, &TcpServerWrapper::onNewConnection);

    if (targetPort == nullptr)
        emit errorOccurred(connectionId, QString("TCP server must have a target port!"));
}

void TcpServerWrapper::Start()
{
    if (server->isListening())
    {
        emit errorOccurred(connectionId, QString("Server is already listening"));
        return;
    }

    if (!server->listen(QHostAddress::Any, _networkPort))
    {
       emit errorOccurred(connectionId, QString("Error on starting listening server: %1").arg(server->errorString()));
       return;
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

    if (ipString == "::1")
        ipString = "127.0.0.1";

    if (ipString.startsWith("[") && ipString.contains("]:")) {
        ipString = ipString.mid(1, ipString.indexOf("]:") - 1);
    }

    return _allowedIps.contains(ipString);
}
