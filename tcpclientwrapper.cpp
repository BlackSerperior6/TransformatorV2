#include "tcpclientwrapper.h"

TcpClientWrapper::TcpClientWrapper(QMap<QString, quint16>& listOfServers, QObject *parent, qint32 conId, AbstractPortWrapper* target)
    : AbstractPortWrapper(parent, conId, PortType::TcpPort, target)
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

TcpClientWrapper::TcpClientWrapper(const QJsonObject& obj, QObject* parent, qint32 conId, AbstractPortWrapper* target, bool& isSucceeded) :
    AbstractPortWrapper(parent, conId, PortType::TcpPort, target)
{
    isSucceeded = FromJson(obj);
}

TcpClientWrapper::~TcpClientWrapper()
{
    Stop();
}

void TcpClientWrapper::Start()
{
    for (QMap<QString, quint16>::iterator it = _listOfServers.begin();
            it != _listOfServers.end(); ++it)
    {
        QTcpSocket* socket = new QTcpSocket(this);

        socket->connectToHost(it.key(), it.value());

        connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
                    this, &TcpClientWrapper::onTcpClientError);

        hostConnections.append(socket);
    }
}

void TcpClientWrapper::Stop()
{
    for (auto& connection : hostConnections)
        connection->disconnectFromHost();

    hostConnections.clear();
}

QJsonObject TcpClientWrapper::ToJson() const
{
    QJsonObject obj = AbstractPortWrapper::ToJson();

    QJsonObject serversListObj;

    for (auto it = _listOfServers.begin(); it != _listOfServers.end(); ++it)
        serversListObj[it.key()] = it.value();

    obj["serverList"] = serversListObj;

    return obj;
}

bool TcpClientWrapper::FromJson(const QJsonObject &obj)
{
    if (!obj.contains("serverList"))
        return false;

    QJsonObject serversListObj = obj["serverList"].toObject();

    for (auto it = serversListObj.begin(); it != serversListObj.end(); ++it)
        _listOfServers[it.key()] = it.value().toInt();

    return true;
}

QString TcpClientWrapper::GetTypeName() const
{
    return "TcpClientWrapper";
}

QMap<QString, quint16> TcpClientWrapper::GetListOfServers() const
{
    return _listOfServers;
}

void TcpClientWrapper::Accept(const QByteArray &data)
{
    AbstractPortWrapper::Accept(data);

    for (auto& connection : hostConnections)
    {
        qint64 writtenData = connection->write(data);

        if (writtenData < 0)
        {
            QString errorMsg = connection->errorString();
            emit errorOccurred(connectionId, QString("Failed to write data to a server: %1").arg(errorMsg));
        }
        else
        {
            AbstractPortWrapper::Accept(data);
            connection->flush();
        }
    }
}

void TcpClientWrapper::onTcpClientError(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *serverConnection = qobject_cast<QTcpSocket*>(sender());

    if (!serverConnection)
        return;

    QString errorMsg = serverConnection->errorString();

    emit errorOccurred(connectionId, errorMsg);
}
