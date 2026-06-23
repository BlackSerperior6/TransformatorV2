#include "mainwindow.h"
#include "connectionedit.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentConListIndex(-1), ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    connectionCounter = ui->ConnectionsList->count();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_CreateConnection_clicked()
{
    ConnectionEdit* editWindow = new ConnectionEdit(nullptr, nullptr, connectionCounter);

    editWindow->setModal(true);
    editWindow->exec();

    if (editWindow->addedAConnection)
    {
        connectionCounter = editWindow->updatedConnectionCounter;
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(QString("Подключение: %1").arg(connectionCounter));
        PortsConnection* connection = editWindow->portsConnection;
        connection->setParent(ui->ConnectionsList);
        ui->ConnectionsList->addItem(item);
        ui->ConnectionsList->setItemWidget(item, connection);

        connect(connection->firstPort, &AbstractPortWrapper::errorOccurred, this, &MainWindow::WriteErrorsToLog);
        connect(connection->secondPort, &AbstractPortWrapper::errorOccurred, this, &MainWindow::WriteErrorsToLog);
        connect(connection, &PortsConnection::errorOccured, this, &MainWindow::WriteErrorsToLog);
        connect(connection->secondPort, &AbstractPortWrapper::dataReceived, connection, &PortsConnection::onDataPassing);

        connection->secondPort->Start();
        connection->firstPort->Start();
    }
}

void MainWindow::on_RemoveConnection_clicked()
{
    if (currentConListIndex == -1)
        return;

    delete ui->ConnectionsList->takeItem(currentConListIndex);

    ui->ConnectionsList->setCurrentRow(-1);
}

void MainWindow::on_ClearErrorLogs_clicked()
{
    ui->ErrorLogs->clear();
}

void MainWindow::on_ConnectionsList_itemDoubleClicked(QListWidgetItem *item)
{
    PortsConnection *connection =  (PortsConnection*) ui->ConnectionsList->itemWidget(item);

    ConnectionEdit *win = new ConnectionEdit(nullptr, connection, connectionCounter);

    win->setModal(true);
    win->exec();

    if (win->addedAConnection)
    {
        connect(connection->firstPort, &AbstractPortWrapper::errorOccurred, this, &MainWindow::WriteErrorsToLog);
        connect(connection->secondPort, &AbstractPortWrapper::errorOccurred, this, &MainWindow::WriteErrorsToLog);
        connect(connection, &PortsConnection::errorOccured, this, &MainWindow::WriteErrorsToLog);
        connect(connection->secondPort, &AbstractPortWrapper::dataReceived, connection, &PortsConnection::onDataPassing);

        connection->secondPort->Start();
        connection->firstPort->Start();
    }
}

void MainWindow::WriteErrorsToLog(quint32 connectionId, const QString &message)
{
    QString finalErrorMessage = QString("Ошибка в подключении с id %1: %2").arg(connectionId).arg(message);

    new QListWidgetItem(finalErrorMessage, ui->ErrorLogs);
}

void MainWindow::on_ConnectionsList_currentRowChanged(int currentRow)
{
    currentConListIndex = currentRow;
}
