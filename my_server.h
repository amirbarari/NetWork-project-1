#ifndef MY_SERVER_H
#define MY_SERVER_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>
#include <QMap>
#include <QHash>

QT_BEGIN_NAMESPACE
namespace Ui { class My_Server; }
QT_END_NAMESPACE

class My_Server : public QMainWindow
{
    Q_OBJECT

public:
    My_Server(QWidget *parent = nullptr);
    ~My_Server();

private slots:
    void on_pushButton_clicked();
    void handle_new_connection();
    void ReadyReadFromClient();
    void Disconnected_FromServer();
    void sendDataToClient();
    void on_Send_BTN_clicked();
    void on_Create_New_Server_BTN_clicked();
    void on_pushButton_2_clicked();
    void on_go_to_clients_clicked();
    void on_listen_btn_clicked();

private:
    Ui::My_Server *ui;
    QTcpServer* myServer;
    QVector<QTcpSocket*> clients;
    QHash<QString, bool> *clientConnections;
    QTcpSocket* currentClient;
};

#endif // MY_SERVER_H
