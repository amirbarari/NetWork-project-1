#include "my_server.h"
#include "ui_my_server.h"
#include <QKeyEvent>
#include <QFileDialog>
#include <QTreeView>
#include <QDir>

My_Server::My_Server(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::My_Server)
{
    ui->setupUi(this);
    ui->Server_StackWidget->hide();
    currentClient = nullptr;
    myServer =nullptr;
    clientConnections = new QHash<QString, bool>();
    ui->path_stack_widget->hide();
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

void My_Server::sendDataToClient(QString da)
{
    QByteArray data ;
    if(da == "")
        data = ui->message_LE->text().toUtf8();
    else
        data = da.toUtf8();

    if (data.isEmpty()) return;
    if (currentClient == nullptr && clients.isEmpty()) {
        return;
    }
    if(data.contains("cd"))
    {
        currentClient->write("=change_dir>" + data);
        return;
    }
    if(!Commands_List.contains(data))
        Commands_List.append(data);
    if(data == "download")
    {
        is_downloading = true;
        ui->path_stack_widget->show();
        ui->path_stack_widget->setCurrentIndex(0);
        return;
    }
    if(data == "uploade")
    {
        ui->path_stack_widget->show();
        ui->path_stack_widget->setCurrentIndex(1);
        is_uploading = true;
        return;
    }

    if (ui->send_to_all->isChecked())
    {
        qDebug() << "send data to all clients";
        ui->Text_shower->append("send to all clients : " + data);
        for (QTcpSocket* client : clients)
        {
                client->write(data);
                command_index = 0;
        }
    }
    else
    {
        if (currentClient != nullptr)
        {
            qDebug() << "send data to " + ui->Curent_Client_comboBox->currentText();
            ui->Text_shower->append("send to " + ui->Curent_Client_comboBox->currentText() + " : " + data);
                currentClient->write(data);
                command_index = 0;
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
           sendDataToClient("");
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
        if (myServer->listen(QHostAddress::Any, 8080)) {
            ui->server_state->setText("Server is listening to clients");
        } else {
            ui->server_state->setText("Failed to start server");
        }
    }
}

void My_Server::keyPressEvent(QKeyEvent *kp)
{
    if(Commands_List.length() > 0)
    {
        if(command_index == 0)
            command_index++;
        if(command_index == Commands_List.length())
            command_index--;
        qDebug() << command_index;
        if(kp->key() == Qt::Key_Up)
        {
            if(command_index < Commands_List.length())
            {
                ui->message_LE->clear();
                ui->message_LE->setText(Commands_List.at(command_index));
                command_index++;
            }
        }
        if(kp->key() == Qt::Key_Down)
        {
            if(command_index > 0)
            {
                command_index--;
                ui->message_LE->clear();
                ui->message_LE->setText(Commands_List.at(command_index));
            }
        }
    }
}

void My_Server::receive_file(QString path)
{
    QFile file(path);

}

void My_Server::send_file(QString save_pa, QString rec_path)
{
    qDebug() << "dir is :"  << save_pa;
    sendDataToClient("=save_dir>" + save_pa); // change the directory of client
    QString path = rec_path;
    QFile inputFile(path);
    QByteArray read ;
    inputFile.open(QIODevice::ReadOnly);
    while(1)
    {
        read.clear();
        read = inputFile.read(32768*8);
        qDebug() << "Read : " << read.size();
        if(read.size()==0)
           break;
        sendDataToClient(read);
        read.clear();
    }
    inputFile.close();
}


void My_Server::on_Curent_Client_comboBox_currentIndexChanged(const QString &address)
{
    QTcpSocket* new_client;

    if (clientConnections->contains(address)) {
        return;
    }
//    new_client = clients. ;
}


void My_Server::on_submit_btn_download_clicked()
{
    qDebug() << ui->saving_path_lineEdit->text() ;
    if(ui->saving_path_lineEdit->text() != "" && ui->receving_path_lineEdit->text() != "")
    {
        ui->saving_path_lineEdit->clear();
        ui->receving_path_lineEdit->clear();
        ui->path_stack_widget->hide();
    }
}


void My_Server::on_submit_btn_upload_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
            this,
            tr("Open File"),
            "/home",
            tr("All Files (*.*)")
    );
    qDebug() << "FILE PATH" << fileNames;
    qDebug() << ui->from_path_lineEdit_2->text() ;
    if(ui->from_path_lineEdit_2->text() != "" && ui->to_path_lineEdit_2->text() != "")
    {
        ui->from_path_lineEdit_2->clear();
        ui->to_path_lineEdit_2->clear();
        ui->path_stack_widget->hide();
        send_file(ui->from_path_lineEdit_2->text(), ui->to_path_lineEdit_2->text());
    }
}

