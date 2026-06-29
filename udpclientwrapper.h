#ifndef UPDCLIENTWRAPPER_H
#define UPDCLIENTWRAPPER_H

#include "abstractportwrapper.h"
#include <QUdpSocket>

class UdpClientWrapper : public AbstractPortWrapper
{
public:
    UdpClientWrapper(QMap<QString, quint16>& listOfServers, quint16 localPort, QObject *parent, qint32 conId, AbstractPortWrapper* target = nullptr);

    UdpClientWrapper(QMap<QString, quint16>& listOfServers, QObject *parent, qint32 conId, AbstractPortWrapper* target = nullptr);

    UdpClientWrapper(const QJsonObject& obj, QObject* parent, qint32 conId, AbstractPortWrapper* target, bool& isSucceeded);

    ~UdpClientWrapper();

    void Start() override;

    void Stop() override;

    QJsonObject ToJson() const override;

    bool FromJson(const QJsonObject& obj) override;

    QString GetTypeName() const override;

    quint16 GetLocalPort() const;

    QMap<QString, quint16> GetListOfServers() const;

protected slots:
    void Accept(const QByteArray &data) override;

private slots:
    void onUpdClientError(QAbstractSocket::SocketError socketError);

private:
    quint16 _localPort;

    bool shouldSetNetworkPortAutomatically;

    QMap<QString, quint16> _listOfServers;

    QUdpSocket* socket;
};

#endif // UPDCLIENTWRAPPER_H
