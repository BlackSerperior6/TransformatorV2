#ifndef TCPSERVERWRAPPER_H
#define TCPSERVERWRAPPER_H

#include "abstractportwrapper.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QSet>
#include <QList>
#include <QJsonArray>

class TcpServerWrapper : public AbstractPortWrapper
{
    Q_OBJECT

public:
    TcpServerWrapper(quint16 networkPort, QSet<QString> allowedIps, QObject *parent, qint32 conId, AbstractPortWrapper* target = nullptr);

    TcpServerWrapper(QSet<QString> allowedIps, QObject *parent, qint32 conId, AbstractPortWrapper* target = nullptr);

    TcpServerWrapper(const QJsonObject& obj, QObject* parent, qint32 conId, AbstractPortWrapper* target, bool& isSucceeded);

    ~TcpServerWrapper();

    void Start() override;

    void Stop() override;

    QJsonObject ToJson() const override;

    bool FromJson(const QJsonObject& obj) override;

    QString GetTypeName() const override;

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
    bool shouldSetNetworkPortAutomatically;
    QSet<QString> _allowedIps;
    QList<QTcpSocket*> connectedClients;

    QTcpServer* server;
};

#endif // TCPSERVERWRAPPER_H
