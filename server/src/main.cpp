#include <iostream>

#include "msg.h"
#include "socket.h"
#include "core.h"

int main()
{
    if(!::init())
        return -1;

    NetSocket server(IPv4("0", true), Port(12345), NetSocketProtocol::TCP, NetSocketType::SERVER, true);
    if(!server.listen())
    {
        std::cout << "Failed to listen!" << std::endl;
        ::system("pause");
        return -1;
    }

    std::vector<NetSocket> connections;

    while(true)
    {
        auto [newConnection, connectionMsg] = server.accept();
        if(newConnection)
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