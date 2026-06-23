#ifndef ABSTRACTPORTWRAPPER_H
#define ABSTRACTPORTWRAPPER_H

#include "porttype.h"
#include <QObject>

class AbstractPortWrapper : public QObject
{
    Q_OBJECT

public:
    AbstractPortWrapper(QObject *parent, qint32 conId, PortType portType, AbstractPortWrapper* target = nullptr);

    virtual ~AbstractPortWrapper();

    virtual void Start() = 0;

    virtual void Stop() = 0;

    PortType GetPortType();

protected:
    void SendToTargetPort(const QByteArray& data);

    AbstractPortWrapper* targetPort;
    qint32 connectionId;

private:
    PortType _portType;

protected slots:
    virtual void Accept(const QByteArray& data);

signals:
    void dataSend(const QByteArray &data);
    void dataReceived(const QByteArray &data);
    void errorOccurred(qint32 connectionId, const QString &error);
};

#endif // ABSTRACTPORTWRAPPER_H
