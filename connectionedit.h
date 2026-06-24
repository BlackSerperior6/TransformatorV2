#ifndef CONNECTIONEDIT_H
#define CONNECTIONEDIT_H

#include <QDialog>
#include <QFileDialog>
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
                            qint32 connectionCounter = 0);
    ~ConnectionEdit();

    PortsConnection* portsConnection;

    QString logFilePath;

    qint32 updatedConnectionCounter;

    bool addedAConnection;

private slots:
    void on_ConTypeSelectionFirst_currentIndexChanged(int index);

    void on_ConTypeSelectionSecond_currentIndexChanged(int index);

    void on_SaveConnectionButton_clicked();

    void on_viewButton_clicked();

private:
    Ui::ConnectionEdit *ui;

    PortsConnection* CreateConnection();

    QSet<QString> ParseIpInput(QPlainTextEdit* plainText);

    QMap<QString, quint16> ParseIpInputWithPort(QPlainTextEdit* plainText);
};

#endif // CONNECTIONEDIT_H
