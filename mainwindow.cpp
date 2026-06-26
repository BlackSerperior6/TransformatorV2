#include "mainwindow.h"
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
        PortsConnection* connection = editWindow->portsConnection;
        AddConnectionToList(connection);
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

void MainWindow::on_SaveButton_clicked()
{
    if (ui->ConnectionsList->count() == 0)
    {
        QMessageBox::warning(this, "Внимание!", "Нету активных подключений!");
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Файл для сохранения. ВНИМАНИЕ! Предыдущие содрежимое файла будет перезаписано!"),
        QDir::homePath(),
        tr(".json")
    );

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Ошибка!", "Не удалось открыть файл!");
        return;
    }

    QJsonObject root;

    root["version"] = CurrentVersion;
    root["connectionCounter"] = (qint32) connectionCounter;

    QJsonArray array;

    for (int i = 0; i < ui->ConnectionsList->count(); ++i)
    {
         PortsConnection *connection =  (PortsConnection*) ui->ConnectionsList->itemWidget(ui->ConnectionsList->item(i));
         array.append(connection->ToJson());
    }

    root["connections"] = array;

    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Compact));
    file.close();
}

void MainWindow::on_LoadButton_clicked()
{
    QList<PortsConnection*> loadedConnections;

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Файл для загрузки. ВНИМАНИЕ! Все текущие сохранения будут переписаны теми, которые сохранены в файле!"),
        QDir::homePath(),
        tr(".json")
    );

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Ошибка!", "Не удалось открыть файл!");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    try
    {
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (doc.isNull())
        {
            QMessageBox::critical(this, "Ошибка!", "JSON файл имеет не верный формат!");
            return;
        }

        QJsonObject root = doc.object();

        if (!root.contains("connections") || !root.contains("version") || !root.contains("connectionCounter"))
        {
            QMessageBox::critical(this, "Ошибка!", "JSON файл имеет не верный формат!");
            return;
        }

        qint32 version = root["version"].toInt();

        if (version != CurrentVersion)
        {
            QMessageBox::critical(this, "Ошибка!", "JSON файл не подходит для данной версии программы!");
            return;
        }

        QJsonArray connectionsArray = root["connections"].toArray();

        bool isSucceeded = true;

        for (const QJsonValue& value : connectionsArray)
        {
            QJsonObject obj = value.toObject();

            PortsConnection* connection = new PortsConnection(obj, isSucceeded);

            if (!isSucceeded)
            {
                connection->deleteLater();
                break;
            }
            else
                loadedConnections.append(connection);
        }

        if (!isSucceeded)
        {
            QMessageBox::critical(this, "Ошибка!", "JSON файл имеет не верный формат!");

            for (auto &i : loadedConnections)
                i->deleteLater();

            return;
        }

        ui->ConnectionsList->clear();

        for (auto &i : loadedConnections)
            AddConnectionToList(i);

        connectionCounter = root["connectionCounter"].toInt();
    }
    catch (...)
    {
        QMessageBox::critical(this, "Ошибка!", "JSON файл имеет не верный формат!");

        for (auto &i : loadedConnections)
            i->deleteLater();
    }
}

void MainWindow::AddConnectionToList(PortsConnection* connection)
{
    QListWidgetItem *item = new QListWidgetItem;
    item->setText(QString("Подключение: %1").arg(connection->GetConnectionId()));
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
