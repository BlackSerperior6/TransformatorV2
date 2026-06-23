#ifndef PORTSCONNECTION_H
#define PORTSCONNECTION_H

#include "abstractportwrapper.h"
#include <QObject>
#include <QWidget>
#include <QFile>
#include <QTextStream>

class PortsConnection : public QWidget
{
public:
    PortsConnection(AbstractPortWrapper* portOne, AbstractPortWrapper* portTwo);

    AbstractPortWrapper* firstPort;
    AbstractPortWrapper* secondPort;

private:
    QString filePath;
    qint32 connectionId;

public slots:
    void onDataPassing(const QByteArray& data);

signals:
    void errorOccured(quint32 connectionId, const QString& errorMessage);
};

#endif // PORTSCONNECTION_H
