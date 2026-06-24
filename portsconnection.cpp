#include "portsconnection.h"

PortsConnection::PortsConnection(quint32 conId, QString logFilePath) : filePath(logFilePath), connectionId(conId)
{}

void PortsConnection::SetPorts(AbstractPortWrapper *first, AbstractPortWrapper *second)
{
    firstPort = first;
    secondPort = second;
}

QString PortsConnection::GetFilePath()
{
    return filePath;
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
