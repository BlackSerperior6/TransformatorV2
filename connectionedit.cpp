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
    }

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
        ui->IPEdit1->setVisible(false);
        ui->IPLabel1->setVisible(false);
        ui->NetLabel1->setVisible(false);
        ui->NETPortEdit1->setVisible(false);

        ui->COMEdit1->setVisible(true);
        ui->ComLabel1->setVisible(true);

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

    if (portTwoType == PortType::SerialPort)
    {
        ui->IPEdit2->setVisible(false);
        ui->IPLabel2->setVisible(false);

        ui->COMEdit2->setVisible(true);
        ui->ComLabel2->setVisible(true);

        if (isConnectionPresent)
        {
            SerialPortWrapper* portTwo = (SerialPortWrapper*) portsConnection->secondPort;
            ui->COMEdit1->setText(portTwo->GetPortName());
        }
    }
    else if (portTwoType == PortType::TcpPort)
    {
        ui->COMEdit2->setVisible(false);
        ui->ComLabel2->setVisible(false);

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

            ui->IPEdit2->setVisible(true);
            ui->IPLabel2->setVisible(true);
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

    portsConnection = CreateConnection();

    close();
}

PortsConnection* ConnectionEdit::CreateConnection()
{
    AbstractPortWrapper* firstPort;
    AbstractPortWrapper* secondport;
    PortsConnection* connection = new PortsConnection(firstPort, secondport, updatedConnectionCounter);

    switch (ui->ConTypeSelectionSecond->currentIndex())
    {
        case 0: // Must make a COM port
            secondport = new SerialPortWrapper(ui->COMEdit2->text(), connection, updatedConnectionCounter, nullptr);
            break;
        case 1: // Must make a TCP client
            QMap<QString, quint16> parsedIps = ParseIpInputWithPort(ui->IPEdit2);
            secondport = new TcpClientWrapper(parsedIps, connection, updatedConnectionCounter, nullptr);
            break;
    }

    switch (ui->ConTypeSelectionFirst->currentIndex())
    {
        case 0: // Must make a COM port
            firstPort = new SerialPortWrapper(ui->COMEdit1->text(), connection, updatedConnectionCounter, secondport);
            break;
        case 1: // Must make a TCP server
            QSet<QString> parsedIps;
            firstPort = new TcpServerWrapper(ui->NETPortEdit1->text().toUShort(), parsedIps, connection, updatedConnectionCounter, secondport);
            break;
    }

    return connection;
}
