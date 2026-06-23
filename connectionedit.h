#ifndef CONNECTIONEDIT_H
#define CONNECTIONEDIT_H

#include <QDialog>
#include <QPlainTextEdit>
#include "porttype.h"
#include "portsconnection.h"
#include "serialportwrapper.h"
#include "tcpserverwrapper.h"
#include "tcpclientwrapper.h"

namespace Ui {
class ConnectionEdit;
}

class ConnectionEdit : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionEdit(QWidget *parent = nullptr,
                            PortsConnection* connection = nullptr,
                            quint32 connectionCounter = 0);
    ~ConnectionEdit();

    PortsConnection* portsConnection;

    quint32 updatedConnectionCounter;

    bool addedAConnection;

private:
    Ui::ConnectionEdit *ui;

    PortsConnection* CreateConnection();

    QList<QString> ParseIpInput(QPlainTextEdit* plainText);

    QMap<QString, quint16> ParseIpInputWithPort(QPlainTextEdit* plainText);
};

#endif // CONNECTIONEDIT_H
