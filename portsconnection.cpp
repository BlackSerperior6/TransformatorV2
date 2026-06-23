#include "portsconnection.h"

PortsConnection::PortsConnection(AbstractPortWrapper* portOne, AbstractPortWrapper* portTwo, quint32 conId) :
    firstPort(portOne), secondPort(portTwo), connectionId(conId)
{}

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
