#include "udpserverwrapper.h"

UdpServerWrapper::UdpServerWrapper(quint16 networkPort, QSet<QString> allowedIps, QObject *parent, qint32 conId, AbstractPortWrapper* target)
    : AbstractPortWrapper(parent, conId, PortType::UdpPort, target), _networkPort(networkPort), shouldSetNetworkPortAutomatically(false),
      socket(new QUdpSocket(this))
{
    SetAllowedIps(allowedIps);

    if (_targetPort == nullptr)
        emit errorOccurred(connectionId, QString("UPD server must have a target port!"));
}

UdpServerWrapper::UdpServerWrapper(QSet<QString> allowedIps, QObject *parent, qint32 conId, AbstractPortWrapper *target)
    : AbstractPortWrapper(parent, conId, PortType::UdpPort, target),
      shouldSetNetworkPortAutomatically(true), socket(new QUdpSocket(this))
{
    SetAllowedIps(allowedIps);

    if (_targetPort == nullptr)
        emit errorOccurred(connectionId, QString("TCP server must have a target port!"));
}

UdpServerWrapper::UdpServerWrapper(const QJsonObject& obj, QObject* parent, qint32 conId, AbstractPortWrapper* target, bool& isSucceeded) :
    AbstractPortWrapper(parent, conId, PortType::UdpPort, target)
{
    isSucceeded = FromJson(obj);

    if (isSucceeded)
        socket = new QUdpSocket(this);
}

UdpServerWrapper::~UdpServerWrapper()
{
    Stop();
}

void UdpServerWrapper::Start()
{
    if (socket->state() == QAbstractSocket::BoundState)
    {
        emit errorOccurred(connectionId, "UPD server is already bound!");
        return;
    }

    if (!shouldSetNetworkPortAutomatically)
    {
        if (!socket->bind(QHostAddress::AnyIPv4, _networkPort, QUdpSocket::ShareAddress))
        {
           emit errorOccurred(connectionId, QString("Error on starting listening server: %1").arg(socket->errorString()));
           return;
        }
    }
    else
    {
        if (!socket->bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ShareAddress))
        {
           emit errorOccurred(connectionId, QString("Error on starting listening server: %1").arg(socket->errorString()));
           return;
        }
    }

    connect(socket, &QUdpSocket::readyRead,
            this, &UdpServerWrapper::onReadyRead);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QUdpSocket::error),
            this, &UdpServerWrapper::onUpdSocketError);
}

void UdpServerWrapper::Stop()
{
    if (socket->state() != QAbstractSocket::BoundState)
        return;

    socket->close();
}

QJsonObject UdpServerWrapper::ToJson() const
{
    QJsonObject obj = AbstractPortWrapper::ToJson();

    obj["networkPort"] = _networkPort;
    obj["shouldSetNetworkPortAutomatically"] = shouldSetNetworkPortAutomatically;

    QJsonArray allowedIpsListObject;

    for (const auto &i : _allowedIps)
        allowedIpsListObject.append(i);

    obj["allowedIps"] = allowedIpsListObject;

    return obj;
}

bool UdpServerWrapper::FromJson(const QJsonObject &obj)
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

    return true;
}

QString UdpServerWrapper::GetTypeName() const
{
    return "UpdServerWrapper";
}

quint16 UdpServerWrapper::GetNetworkPort() const
{
    return _networkPort;
}

QSet<QString> UdpServerWrapper::GetAllowedIps() const
{
    return _allowedIps;
}

void UdpServerWrapper::Accept(const QByteArray &data)
{
    emit errorOccurred(connectionId, QString("Accept slot triggered in a server wrapper!"));
}

void UdpServerWrapper::onReadyRead()
{
    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress senderIp;

        qint64 datagramSize = socket->pendingDatagramSize();
        if (datagramSize <= 0)
            return;

        datagram.resize(static_cast<int>(datagramSize));

        qint64 bytesRead = socket->readDatagram(datagram.data(),
                                                      datagram.size(),
                                                      &senderIp);

        if (bytesRead == -1)
        {
            emit errorOccurred(connectionId, QString("Failed to read datagram: %1").arg(socket->errorString()));
            return;
        }

        if (!IsIpWhitelisted(senderIp))
            continue;

        datagram.resize(static_cast<int>(bytesRead));

        SendToTargetPort(datagram);
    }
}

void UdpServerWrapper::onUpdSocketError(QAbstractSocket::SocketError socketError)
{
    QString errorMsg = socket->errorString();

    emit errorOccurred(connectionId, errorMsg);
}

void UdpServerWrapper::SetAllowedIps(QSet<QString> &allowedIps)
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

bool UdpServerWrapper::IsIpWhitelisted(const QHostAddress &ip) const
{
    if (_allowedIps.isEmpty())
        return true;

    QString ipString = ip.toString();

    qDebug() << "Checked ip string: " + ipString;

    return _allowedIps.contains(ipString);
}


