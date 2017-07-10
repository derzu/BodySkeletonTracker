#include "SocketClient.h"


//#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include<arpa/inet.h> //inet_addr
#include <stdio.h>

SocketClient::SocketClient()
{
    //ctor
}

SocketClient::SocketClient(int porta, std::string ipServer) : Socket(porta)
{
    this->ipServer = ipServer;
}


SocketClient::~SocketClient()
{
    //dtor
}


void SocketClient::conecta() {
    bzero((char *) &server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ipServer.c_str());
    server.sin_port = htons( porta );

    if (connect(socketFd,(struct sockaddr *) &server, sizeof(struct sockaddr_in)) < 0)
        printf("ERROR connecting");
}
