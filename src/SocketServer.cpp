#include "SocketServer.h"

#include <stdio.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_addr
#include <iostream>

SocketServer::SocketServer()
{
    //ctor
}
SocketServer::SocketServer(int porta) : Socket(porta)
{
    //ctor
}

SocketServer::~SocketServer()
{
    //dtor
}


int SocketServer::init() {
     bzero((char *) &server, sizeof(struct sockaddr));
     server.sin_family = AF_INET;
     server.sin_addr.s_addr = INADDR_ANY;
     server.sin_port = htons(porta);
     if (bind(socketFd, (struct sockaddr*) &server, sizeof(struct sockaddr)) < 0)
              printf("ERROR on binding\n");
     printf("Aguardando conexao na porta %d\n", porta);
     listen(socketFd,5);
}


int SocketServer::aceita() {
     socklen_t tam = 0;
     newsockfd = accept(socketFd,  (struct sockaddr *) &cli_addr,  &tam);
     if (newsockfd < 0)
          printf("ERROR on accept\n");
     std::cout << " tam: " << tam << std::endl;
     return 0;
}

int SocketServer::receber(char * message) {
    return recv(newsockfd, message, TMAX, 0);
}
