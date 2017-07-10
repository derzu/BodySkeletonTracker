#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <Socket.h>

class SocketServer : public Socket
{
    public:
        SocketServer();
        SocketServer(int porta);
        virtual ~SocketServer();
        int aceita();
        int init();
        int receber(char * message);
    private:
        int newsockfd;
        struct sockaddr cli_addr;
};

#endif // SOCKETSERVER_H
