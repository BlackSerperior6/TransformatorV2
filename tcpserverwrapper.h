#ifndef TCPSERVERWRAPPER_H
#define TCPSERVERWRAPPER_H

#include "abstractportwrapper.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QSet>
#include <QList>

class TcpServerWrapper : public AbstractPortWrapper
{
public:
    TcpServerWrapper(quint16 networkPort, QSet<QString> allowedIps, QObject *parent, qint32 conId, AbstractPortWrapper* target = nullptr);

    ~TcpServerWrapper();

    void Start() override;

    void Stop() override;

    quint16 GetNetworkPort();

    QSet<QString> GetAllowedIps();

protected slots:
    void Accept(const QByteArray &data) override;

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();
    void onTcpSocketError(QAbstractSocket::SocketError socketError);

private:
    void SetAllowedIps(QSet<QString>& allowedIps);

    bool IsIpWhitelisted(const QHostAddress &ip) const;

    quint16 _networkPort;
    QSet<QString> _allowedIps;
    QList<QTcpSocket*> connectedClients;

    QTcpServer* server;
};

#endif // TCPSERVERWRAPPER_H
