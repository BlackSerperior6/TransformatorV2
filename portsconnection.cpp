#include "portsconnection.h"

PortsConnection::PortsConnection(quint32 conId, QString logFilePath) : filePath(logFilePath), connectionId(conId)
{}

PortsConnection::PortsConnection(const QJsonObject &obj, bool &isSucceeded)
{
    isSucceeded = FromJson(obj);
}

void PortsConnection::SetPorts(AbstractPortWrapper *first, AbstractPortWrapper *second)
{
    firstPort = first;
    secondPort = second;
}

void PortsConnection::Stop()
{
    firstPort->Stop();
    secondPort->Stop();
}

qint32 PortsConnection::GetConnectionId()
{
    return connectionId;
}

QString PortsConnection::GetFilePath()
{
    return filePath;
}

QJsonObject PortsConnection::ToJson() const
{
    QJsonObject obj;
    obj["connectionId"] = connectionId;
    obj["filePath"] = filePath;

    obj["firstPort"] = firstPort->ToJson();
    obj["secondPort"] = secondPort->ToJson();

    return obj;
}

bool PortsConnection::FromJson(const QJsonObject &obj)
{
    if (!obj.contains("connectionId"))
        return false;

    connectionId = obj["connectionId"].toInt();

    if (!obj.contains("filePath"))
        return false;

    filePath = obj["filePath"].toString();

    if (!obj.contains("secondPort"))
        return false;

    QJsonObject secondPortObj = obj["secondPort"].toObject();

    secondPort = AbstractPortWrapper::CreateFromJson(secondPortObj, this, connectionId, nullptr);

    if (!obj.contains("firstPort"))
        return false;

    QJsonObject firstPortObj = obj["firstPort"].toObject();

    firstPort = AbstractPortWrapper::CreateFromJson(firstPortObj, this, connectionId, secondPort);

    return true;
}

void PortsConnection::onDataPassing(const QByteArray& data)
{
    if (filePath.isEmpty())
        return;

    QString dataAsText = QString::fromUtf8(data);

    QFile file(filePath);

    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        emit errorOccured(connectionId, QString("Failed to open file with path: ").arg(filePath));
        return;
    }

    QTextStream out(&file);
    out << "\n" + dataAsText;
    file.close();
}
