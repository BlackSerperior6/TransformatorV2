#include "connectionedit.h"
#include "ui_connectionedit.h"

ConnectionEdit::ConnectionEdit(QWidget *parent,
                               PortsConnection* connection,
                               quint32 connectionCounter) :
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
        ui->NetLabel2->setVisible(false);
        ui->NETPortEdit2->setVisible(false);

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
        ui->NetLabel2->setVisible(true);
        ui->NETPortEdit2->setVisible(true);

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
