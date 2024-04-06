#include "my_server.h"
#include "ui_my_server.h"
#include <QKeyEvent>

My_Server::My_Server(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::My_Server)
{
    ui->setupUi(this);
    ui->Server_StackWidget->hide();
    currentClient = nullptr;
    myServer =nullptr;
    clientConnections = new QHash<QString, bool>();
}

My_Server::~My_Server()
{
    delete ui;
}

void My_Server::on_Create_New_Server_BTN_clicked()
{
    ui->Server_StackWidget->show();
    if (myServer != nullptr) {
        myServer->close();
        delete myServer;
        clientConnections->clear();
    }

    myServer = new QTcpServer();

    connect(myServer, SIGNAL(newConnection()), this, SLOT(handle_new_connection()));
    ui->server_state->clear();
    ui->spinBox->clear();
    ui->Server_StackWidget->show();
    ui->Server_StackWidget->setCurrentIndex(0);
    ui->Text_shower->clear();
}

void My_Server::handle_new_connection()
{
    qDebug() << "handle new connection";
    QTcpSocket* socket = myServer->nextPendingConnection();

    QString clientAddress = socket->peerAddress().toString();
    if (clientConnections->contains(clientAddress)) {
        return;
    }
    currentClient = socket;
    clientConnections->insert(clientAddress, true);
    ui->Curent_Client_comboBox->addItem(clientAddress);
    clients.append(socket);

    connect(socket, SIGNAL(readyRead()), this, SLOT(ReadyReadFromClient()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(Disconnected_FromServer()));

    ui->Text_shower->append("New connection from: " + clientAddress);
    ui->Server_StackWidget->setCurrentIndex(1);
}

void My_Server::ReadyReadFromClient()
{
     qDebug() << "read from clints";
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == nullptr) return;

    QString clientAddress = socket->peerAddress().toString();
    QByteArray data = socket->readAll();
    ui->Text_shower->append(clientAddress + " : " + data);
}

void My_Server::Disconnected_FromServer()
{
     qDebug() << "disconnect from server";
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    if (socket == nullptr) return;

    QString clientAddress = socket->peerAddress().toString();
    clientConnections->remove(clientAddress);
    int index = ui->Curent_Client_comboBox->findText(clientAddress);
    if(index != -1)
        ui->Curent_Client_comboBox->removeItem(index);

    clients.removeOne(socket);
    socket->deleteLater();

    ui->Text_shower->append("Disconnected from: " + clientAddress);
}

void My_Server::sendDataToClient()
{
    QByteArray data = ui->message_LE->text().toUtf8();
    if (data.isEmpty()) return;
    if (currentClient == nullptr && clients.isEmpty()) {
        return;
    }

    if (ui->send_to_all->isChecked())
    {
        qDebug() << "send data to all clients";
        ui->Text_shower->append("send to all clients : " + data);
        for (QTcpSocket* client : clients)
        {
                client->write(data);
        }
    }
    else
    {
        if (currentClient != nullptr)
        {
            qDebug() << "send data to " + ui->Curent_Client_comboBox->currentText();
            ui->Text_shower->append("send to " + ui->Curent_Client_comboBox->currentText() + " : " + data);
                currentClient->write(data);
        }
    }
}

void My_Server::on_Send_BTN_clicked()
{
     qDebug() << "send btn clicked ";
    QString message = ui->message_LE->text();
    if (message.isEmpty()) return;

    currentClient = nullptr;
    QString current_clinet_ip = ui->Curent_Client_comboBox->currentText();
    for(auto c : clients)
    {
       QString clientAddress = c->peerAddress().toString();
       if(clientAddress == current_clinet_ip)
       {
           currentClient = c;
           sendDataToClient();
           break;
       }
    }


    ui->message_LE->clear();
    ui->message_LE->setFocus();
}

void My_Server::on_go_to_clients_clicked()
{
    ui->Server_StackWidget->setCurrentIndex(1);
}

void My_Server::on_listen_btn_clicked()
{
    if (myServer->isListening()) {
        myServer->close();
        ui->server_state->setText("Server stopped");
    } else {
        if (myServer->listen(QHostAddress::Any, ui->spinBox->value())) {
            ui->server_state->setText("Server is listening to clients");
        } else {
            ui->server_state->setText("Failed to start server");
        }
    }
}

