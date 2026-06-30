#ifndef PORTSCONNECTION_H
#define PORTSCONNECTION_H

#include "abstractportwrapper.h"
#include <QObject>
#include <QWidget>
#include <QFile>
#include <QTextStream>

class PortsConnection : public QWidget
{
    Q_OBJECT

public:
    PortsConnection(quint32 conId, QString logFilePath);

    PortsConnection(const QJsonObject& obj, bool& isSucceeded);

    void SetPorts(AbstractPortWrapper* first, AbstractPortWrapper* second);

    void Stop();

    qint32 GetConnectionId();

    QString GetFilePath();

    QJsonObject ToJson() const;

    bool FromJson(const QJsonObject& obj);

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
