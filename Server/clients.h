#ifndef CLIENTS_H
#define CLIENTS_H

#include <QTcpSocket>
#include <QTimer>


class Clients
{
public:
    Clients(QTcpSocket* client_Socket);
private:
    QTcpSocket* clientsocket;
    QTimer* thread;
};

#endif // CLIENTS_H
