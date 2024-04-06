#ifndef MY_CLIENT_H
#define MY_CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class My_Client; }
QT_END_NAMESPACE

class My_Client : public QMainWindow
{
    Q_OBJECT

public slots:
    void ConnectToServer();
    void on_Create_New_Client_BTN_clicked();
    void on_ConnectToServer_BTN_clicked();
    void Write_Data(QString out);
    void Read_Data();
    void Disconnected_FromServer();
    void Check_connection_To_Server();
    void on_Send_BTN_clicked();
    void on_pushButton_clicked();

public:
    My_Client(QWidget *parent = nullptr);
    ~My_Client();
    void Create_New_Page_For_Client();

private slots:
    void on_send_BTN_clicked();
    void on_Ip_Address_textChanged(const QString &arg1);
    void procces_the_command(QString a);
    void on_disconnect_to_server_clicked();

private:
    Ui::My_Client *ui;
    QTcpSocket* client_Socket;
    bool connection_state = 1;
};
#endif // MY_CLIENT_H
