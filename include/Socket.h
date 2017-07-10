#ifndef SOCKET_H
#define SOCKET_H

//#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>

#define TMAX 2000

class Socket
{
    public:
        Socket();
        Socket(int porta);
        virtual ~Socket();
        int enviar(char message[], int tam);
        int receber(char * message);
    protected:
        int socketFd;
        int porta;
        struct sockaddr_in server;
        char * message[TMAX];

};

#endif // SOCKET_H
