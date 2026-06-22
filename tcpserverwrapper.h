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

    void Start() override;

    void Stop() override;

protected slots:
    void Accept(const QByteArray &data) override;

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

signals:
    void messageReceived(const QString &message, const QHostAddress &senderIp);

private:

    void SetAllowedIps(QSet<QString>& allowedIps);

    bool IsIpWhitelisted(const QHostAddress &ip) const;

    quint16 _networkPort;
    QSet<QString> _allowedIps;
    QList<QTcpSocket*> connectedClients;

    QTcpServer* server;
};

#endif // TCPSERVERWRAPPER_H
