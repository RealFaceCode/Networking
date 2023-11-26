#include <iostream>

#include "msg.h"
#include "socket.h"
#include "core.h"

int main()
{
    if(!::init())
        return -1;

    NetMSG::AddOrder(::UserDefType(0), {NetMsgOrderType::STRING});

    NetSocket client(IPv4("127.0.0.1"), Port(12345), NetSocketProtocol::TCP, NetSocketType::CLIENT, true);
    auto [msg, success] = client.connect();
    if(!success)
    {
        std::cout << "Failed to connect!" << std::endl;
        ::system("pause");
        return -1;
    }

    std::string mStr(4000, 'X');
    NetMSG msgSend(::UserDefType(0));
    msgSend.addStr(mStr);

    while(!client.send(msgSend))
        std::cout << "Failed to send msg" << std::endl;

    ::system("pause");
    return 0;
}
