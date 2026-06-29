#include "connectionedit.h"
#include "ui_connectionedit.h"

ConnectionEdit::ConnectionEdit(QWidget *parent,
                               PortsConnection* connection,
                               qint32 connectionCounter) :
    QDialog(parent), portsConnection(connection), updatedConnectionCounter(connectionCounter), addedAConnection(false), ui(new Ui::ConnectionEdit)
{
    ui->setupUi(this);

    PortType portOneType;
    PortType portTwoType;

    bool isConnectionPresent = portsConnection != nullptr;

    updatedConnectionCounter = connectionCounter;

    if (!isConnectionPresent)
    {
        portOneType = PortType::SerialPort;
        portTwoType = PortType::SerialPort;
    }
    else
    {
        portOneType = portsConnection->firstPort->GetPortType();
        portTwoType = portsConnection->secondPort->GetPortType();

        logFilePath = portsConnection->GetFilePath();
    }

    ui->logFileEdit->setText(logFilePath);

    if (portOneType == PortType::SerialPort)
    {
        ui->IPEdit1->setVisible(false);
        ui->IPLabel1->setVisible(false);
        ui->NetLabel1->setVisible(false);
        ui->NETPortEdit1->setVisible(false);

        ui->COMEdit1->setVisible(true);
        ui->ComLabel1->setVisible(true);

        if (isConnectionPresent)
        {
            SerialPortWrapper* portOne = (SerialPortWrapper*) portsConnection->firstPort;
            ui->COMEdit1->setText(portOne->GetPortName());
        }

    }
    else if (portOneType == PortType::TcpPort)
    {
        ui->COMEdit1->setVisible(false);
        ui->ComLabel1->setVisible(false);

        ui->IPEdit1->setVisible(true);
        ui->IPLabel1->setVisible(true);
        ui->NetLabel1->setVisible(true);
        ui->NETPortEdit1->setVisible(true);

        ui->ConTypeSelectionFirst->setCurrentIndex(1);

        if (isConnectionPresent)
        {
            TcpServerWrapper* portOne = (TcpServerWrapper*) portsConnection->firstPort;

            QStringList lines;

            ui->NETPortEdit1->setText(QString::number(portOne->GetNetworkPort()));

            for (const auto& item : portOne->GetAllowedIps())
                 lines.append(item);

            ui->IPEdit1->setPlainText(lines.join('\n'));
        }
    }
    else if (portOneType == PortType::UpdPort)
    {
        ui->COMEdit1->setVisible(false);
        ui->ComLabel1->setVisible(false);

        ui->IPEdit1->setVisible(true);
        ui->IPLabel1->setVisible(true);
        ui->NetLabel1->setVisible(true);
        ui->NETPortEdit1->setVisible(true);

        ui->ConTypeSelectionFirst->setCurrentIndex(2);

        if (isConnectionPresent)
        {
            UpdServerWrapper* portOne = (UpdServerWrapper*) portsConnection->firstPort;

            QStringList lines;

            ui->NETPortEdit1->setText(QString::number(portOne->GetNetworkPort()));

            for (const auto& item : portOne->GetAllowedIps())
                 lines.append(item);

            ui->IPEdit1->setPlainText(lines.join('\n'));

            ui->NETPortEdit2->setText(QString::number(portOne->GetNetworkPort()));
        }

    }

    if (portTwoType == PortType::SerialPort)
    {
        ui->IPEdit2->setVisible(false);
        ui->IPLabel2->setVisible(false);
        ui->NetLabel2->setVisible(false);
        ui->NETPortEdit2->setVisible(false);

        ui->COMEdit2->setVisible(true);
        ui->ComLabel2->setVisible(true);

        if (isConnectionPresent)
        {
            SerialPortWrapper* portTwo = (SerialPortWrapper*) portsConnection->secondPort;
            ui->COMEdit2->setText(portTwo->GetPortName());
        }
    }
    else if (portTwoType == PortType::TcpPort)
    {
        ui->COMEdit2->setVisible(false);
        ui->ComLabel2->setVisible(false);
        ui->NetLabel2->setVisible(false);
        ui->NETPortEdit2->setVisible(false);

        ui->IPEdit2->setVisible(true);
        ui->IPLabel2->setVisible(true);

        ui->ConTypeSelectionSecond->setCurrentIndex(1);

        if (isConnectionPresent)
        {
            TcpClientWrapper* portTwo = (TcpClientWrapper*) portsConnection->secondPort;

            QMap<QString, quint16> listOfServers = portTwo->GetListOfServers();

            QStringList lines;

            for (QMap<QString, quint16>::iterator it = listOfServers.begin();
                    it != listOfServers.end(); ++it)
            {
                QString line = QString("%1:%2").arg(it.key()).arg(it.value());
                lines.append(line);
            }

            ui->IPEdit2->setPlainText(lines.join('\n'));
        }
    }
    else if (portTwoType == PortType::UpdPort)
    {
        ui->COMEdit2->setVisible(false);
        ui->ComLabel2->setVisible(false);

        ui->IPEdit2->setVisible(true);
        ui->IPLabel2->setVisible(true);
        ui->NetLabel2->setVisible(true);
        ui->NETPortEdit2->setVisible(true);

        ui->ConTypeSelectionSecond->setCurrentIndex(2);

        if (isConnectionPresent)
        {
            UpdClientWrapper* portTwo = (UpdClientWrapper*) portsConnection->secondPort;

            QMap<QString, quint16> listOfServers = portTwo->GetListOfServers();

            QStringList lines;

            for (QMap<QString, quint16>::iterator it = listOfServers.begin();
                    it != listOfServers.end(); ++it)
            {
                QString line = QString("%1:%2").arg(it.key()).arg(it.value());
                lines.append(line);
            }

            ui->IPEdit2->setPlainText(lines.join('\n'));

            ui->NETPortEdit2->setText(QString::number(portTwo->GetLocalPort()));
        }
    }
}

ConnectionEdit::~ConnectionEdit()
{
    delete ui;
}

void ConnectionEdit::on_ConTypeSelectionFirst_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0: // COM port selected
            ui->IPEdit1->setVisible(false);
            ui->IPLabel1->setVisible(false);
            ui->NetLabel1->setVisible(false);
            ui->NETPortEdit1->setVisible(false);

            ui->COMEdit1->setVisible(true);
            ui->ComLabel1->setVisible(true);
            break;

        case 1: // TCP port selected
        case 2: //UPD port selected
            ui->COMEdit1->setVisible(false);
            ui->ComLabel1->setVisible(false);

            ui->IPEdit1->setVisible(true);
            ui->IPLabel1->setVisible(true);
            ui->NetLabel1->setVisible(true);
            ui->NETPortEdit1->setVisible(true);
            break;
    }
}

void ConnectionEdit::on_ConTypeSelectionSecond_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0: // COM port selected
            ui->IPEdit2->setVisible(false);
            ui->IPLabel2->setVisible(false);

            ui->COMEdit2->setVisible(true);
            ui->ComLabel2->setVisible(true);
            break;

        case 1: // TCP port selected
            ui->COMEdit2->setVisible(false);
            ui->ComLabel2->setVisible(false);
            ui->NetLabel2->setVisible(false);
            ui->NETPortEdit2->setVisible(false);

            ui->IPEdit2->setVisible(true);
            ui->IPLabel2->setVisible(true);
            break;

        case 2: //UPD Port Selected
            ui->COMEdit2->setVisible(false);
            ui->ComLabel2->setVisible(false);

            ui->IPEdit2->setVisible(true);
            ui->IPLabel2->setVisible(true);
            ui->NetLabel2->setVisible(true);
            ui->NETPortEdit2->setVisible(true);
            break;
    }
}

void ConnectionEdit::on_SaveConnectionButton_clicked()
{
    if (portsConnection != nullptr)
        delete portsConnection;
    else
        updatedConnectionCounter++;

    addedAConnection = true;

    logFilePath = ui->logFileEdit->text();

    portsConnection = CreateConnection();

    close();
}

PortsConnection* ConnectionEdit::CreateConnection()
{
    AbstractPortWrapper* firstPort;
    AbstractPortWrapper* secondport;
    PortsConnection* connection = new PortsConnection(updatedConnectionCounter, logFilePath);

    QMap<QString, quint16> parsedIps;
    switch (ui->ConTypeSelectionSecond->currentIndex())
    {
        case 0: // Must make a COM port
            secondport = new SerialPortWrapper(ui->COMEdit2->text(), connection, updatedConnectionCounter, nullptr);
            break;
        case 1: // Must make a TCP client
            parsedIps = ParseIpInputWithPort(ui->IPEdit2);
            secondport = new TcpClientWrapper(parsedIps, connection, updatedConnectionCounter, nullptr);
            break;
        case 2: //Must make a UPD client
            parsedIps = ParseIpInputWithPort(ui->IPEdit2);
            bool isPortValid = false;
            quint16 port = ui->NETPortEdit1->text().toUShort(&isPortValid);

            if (!isPortValid)
                secondport = new UpdClientWrapper(parsedIps, connection, updatedConnectionCounter, nullptr);
            else
                secondport = new UpdClientWrapper(parsedIps, port, connection, updatedConnectionCounter, nullptr);
            break;
    }

    bool isPortValid;
    quint16 port;
    QSet<QString> parsedServersIps;

    switch (ui->ConTypeSelectionFirst->currentIndex())
    {
        case 0: // Must make a COM port
            firstPort = new SerialPortWrapper(ui->COMEdit1->text(), connection, updatedConnectionCounter, secondport);
            break;
        case 1: // Must make a TCP server
            isPortValid = false;
            port = ui->NETPortEdit1->text().toUShort(&isPortValid);
            parsedServersIps = ParseIpInput(ui->IPEdit1);

            if (!isPortValid)
                firstPort = new TcpServerWrapper(parsedServersIps, connection, updatedConnectionCounter, secondport);
            else
                firstPort = new TcpServerWrapper(port, parsedServersIps, connection, updatedConnectionCounter, secondport);
            break;
        case 2: // Must make a UPD server
            isPortValid = false;
            port = ui->NETPortEdit1->text().toUShort(&isPortValid);
            parsedServersIps = ParseIpInput(ui->IPEdit1);

            if (!isPortValid)
                firstPort = new UpdServerWrapper(parsedServersIps, connection, updatedConnectionCounter, secondport);
            else
                firstPort = new UpdServerWrapper(port, parsedServersIps, connection, updatedConnectionCounter, secondport);
            break;
    }

    connection->SetPorts(firstPort, secondport);

    return connection;
}

QSet<QString> ConnectionEdit::ParseIpInput(QPlainTextEdit *plainText)
{
    QSet<QString> result;

    QString text = plainText->toPlainText();

    QStringList lines = text.split('\n');

    for (const QString& line : lines)
    {
        QString pureLine = line.trimmed();

        result.insert(pureLine);
    }

    return result;
}

QMap<QString, quint16> ConnectionEdit::ParseIpInputWithPort(QPlainTextEdit *plainText)
{
    QMap<QString, quint16> result;

    QString text = plainText->toPlainText();

    QStringList lines = text.split('\n');

    for (const QString& line : lines)
    {
        QString pureLine = line.trimmed();

        QStringList parts = line.split(':');

        if (parts.count() != 2)
            continue;

        bool canPortBeParsed = false;

        quint16 parsedPort = parts[1].toUShort(&canPortBeParsed);

        if (!canPortBeParsed)
            continue;

        result.insert(parts[0], parsedPort);
    }

    return result;
}

void ConnectionEdit::on_viewButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Файл логов."),
        QDir::homePath(),
        tr("All Files (*.*)")
    );

    ui->logFileEdit->setText(filePath);
}
