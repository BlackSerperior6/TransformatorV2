#ifndef UPDSERVERWRAPPER_H
#define UPDSERVERWRAPPER_H

#include "abstractportwrapper.h"
#include <QHostAddress>
#include <QUdpSocket>
#include <QJsonArray>

class UpdServerWrapper : public AbstractPortWrapper
{
public:
    UpdServerWrapper(quint16 networkPort, QSet<QString> allowedIps, QObject *parent, qint32 conId, AbstractPortWrapper* target = nullptr);

    UpdServerWrapper(QSet<QString> allowedIps, QObject *parent, qint32 conId, AbstractPortWrapper* target = nullptr);

    UpdServerWrapper(const QJsonObject& obj, QObject* parent, qint32 conId, AbstractPortWrapper* target, bool& isSucceeded);

    ~UpdServerWrapper();

    void Start() override;

    void Stop() override;

    QJsonObject ToJson() const override;

    bool FromJson(const QJsonObject& obj) override;

    QString GetTypeName() const override;

    quint16 GetNetworkPort() const;

    QSet<QString> GetAllowedIps() const;

protected slots:
    void Accept(const QByteArray &data) override;

private slots:
    void onReadyRead();
    void onUpdSocketError(QAbstractSocket::SocketError socketError);

private:
    void SetAllowedIps(QSet<QString>& allowedIps);

    bool IsIpWhitelisted(const QHostAddress &ip) const;

    quint16 _networkPort;
    bool shouldSetNetworkPortAutomatically;
    QSet<QString> _allowedIps;

    QUdpSocket* socket;
};

#endif // UPDSERVERWRAPPER_H
