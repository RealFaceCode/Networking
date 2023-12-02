#include <iostream>

#include "msg.h"
#include "socket.h"
#include "core.h"

int main()
{
    if(!NetSock::Init())
        return -1;

    NetSock::NetMSG::AddOrder(NetSock::UserDefType(0), {NetSock::NetMsgOrderType::STRING});

    NetSock::NetSocket client(NetSock::IPv4("127.0.0.1"),
                            NetSock::Port(12345),
                            NetSock::NetSocketProtocol::UDP,
                            NetSock::NetSocketType::CLIENT,
                            true);
    if(!client.connect())
    {
        std::cout << "Failed to connect!" << std::endl;
        ::system("pause");
        return -1;
    }

    std::string mStr(4000, 'X');
    NetSock::NetMSG msgSend(NetSock::UserDefType(0));
    msgSend.addStr(mStr);

    while(!client.send(msgSend))
        std::cout << "Failed to send msg" << std::endl;

    ::system("pause");
    return 0;
}