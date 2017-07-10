#include "Socket.h"

#include <stdio.h>

Socket::Socket()
{
    //ctor
}

Socket::Socket(int porta)
{
    this->porta = porta;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0)
        printf("ERROR opening socket\n");
}

Socket::~Socket()
{
    //dtor
}

int Socket::enviar(char message[], int tam) {
    return send(socketFd, message, tam, 0);

}
int Socket::receber(char * message) {
    return recv(socketFd, message, TMAX, 0);
}
