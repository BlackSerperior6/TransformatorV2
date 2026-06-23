#include "tcpclientwrapper.h"

TcpClientWrapper::TcpClientWrapper(QMap<QString, quint16>& listOfServers, QObject *parent, qint32 conId, AbstractPortWrapper* target)
    : AbstractPortWrapper(parent, conId, target)
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

void TcpClientWrapper::Accept(const QByteArray &data)
{
    AbstractPortWrapper::Accept(data);

    for (auto& connection : hostConnections)
    {
        qint64 writtenData = connection->write(data);

        if (writtenData < 0)
            emit errorOccurred(connectionId, "Failed to write data to a server");

        connection->flush();
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
