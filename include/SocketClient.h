#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <string>
#include <Socket.h>

class SocketClient : public Socket
{
    public:
        SocketClient();
        SocketClient(int porta, std::string ipServer);
        virtual ~SocketClient();
        void conecta();
    protected:
        std::string ipServer;
    private:
};

#endif // SOCKETCLIENT_H
