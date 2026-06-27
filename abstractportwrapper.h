#ifndef ABSTRACTPORTWRAPPER_H
#define ABSTRACTPORTWRAPPER_H

#include "porttype.h"
#include <QObject>
#include <QDebug>
#include <QJsonObject>

class AbstractPortWrapper : public QObject
{
    Q_OBJECT

public:
    AbstractPortWrapper(QObject *parent, qint32 conId, PortType portType, AbstractPortWrapper* target = nullptr);

    virtual ~AbstractPortWrapper();

    virtual void Start() = 0;

    virtual void Stop() = 0;

    virtual QString GetTypeName() const = 0;

    virtual bool FromJson(const QJsonObject& obj) = 0;

    virtual QJsonObject ToJson() const;

    PortType GetPortType();

    static AbstractPortWrapper* CreateFromJson(const QJsonObject& obj,
                                                   QObject* parent,
                                                   qint32 conId,
                                                   AbstractPortWrapper* target = nullptr);

private:
    PortType _portType;

protected:
    void SendToTargetPort(const QByteArray& data);

    qint32 connectionId;
    AbstractPortWrapper* _targetPort;

protected slots:
    virtual void Accept(const QByteArray& data);

signals:
    void dataSend(const QByteArray &data);
    void dataReceived(const QByteArray &data);
    void errorOccurred(qint32 connectionId, const QString &error);
};

#endif // ABSTRACTPORTWRAPPER_H
