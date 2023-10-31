#ifndef SOCKET_H
#define SOCKET_H

#include "core.h"

class NetSocket
{
    private:
    NET_SOCKET socket; 

public:
    NetSocket();

    void connect();
    void disconnect();
    
    void send();
    void recv();

    
private:
    void setup();
};

#endif // SOCKET_H