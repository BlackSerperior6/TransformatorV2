#ifndef TCPCLIENTWRAPPER_H
#define TCPCLIENTWRAPPER_H

#include <QTcpSocket>
#include <QHostAddress>
#include "abstractportwrapper.h"

class TcpClientWrapper : public AbstractPortWrapper
{
public:
    TcpClientWrapper(QMap<QString, quint16>& listOfServers, QObject *parent, qint32 conId, AbstractPortWrapper* target = nullptr);

    ~TcpClientWrapper();

    void Start() override;

    void Stop() override;

    QMap<QString, quint16> GetListOfServers();

protected slots:
    void Accept(const QByteArray &data) override;

private slots:
    void onTcpClientError(QAbstractSocket::SocketError socketError);

private:
    QList<QTcpSocket*> hostConnections;

    QMap<QString, quint16> _listOfServers;
};

#endif // TCPCLIENTWRAPPER_H
