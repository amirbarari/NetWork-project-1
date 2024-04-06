#include "clients.h"
//#include <QObject>

Clients::Clients(QTcpSocket* client_Socket)
{
   clientsocket = client_Socket;
   QObject::connect(thread, &QTimer::timeout, [](){

   });
   thread->start(0);
}
