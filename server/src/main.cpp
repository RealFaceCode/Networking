#include <iostream>

#include "msg.h"
#include "socket.h"
#include "core.h"

int main()
{
    if(!NetSock::Init())
        return -1;

    NetSock::NetSocket server(NetSock::IPv4("0", true),
                            NetSock::Port(12345),
                            NetSock::NetSocketProtocol::UDP,
                            NetSock::NetSocketType::SERVER,
                            true);
    if(!server.listen())
    {
        std::cout << "Failed to listen!" << std::endl;
        ::system("pause");
        return -1;
    }

    std::vector<NetSock::NetSocket> connections;

    while(true)
    {
        if(auto newConnection = server.accept(); newConnection)
        {
            std::cout << "new connection: " << newConnection.getIP() << ":" << newConnection.getPort().port() << std::endl;
            connections.emplace_back(newConnection);
        }

        for(auto const& connection : connections)
        {
            auto msg = connection.recv();
            if(msg)
            {
                std::cout << "new message: " << msg.getStr() << std::endl;
                ::system("pause");
                return 1;
            }
        }
    }

    ::system("pause");
    return 0;
}