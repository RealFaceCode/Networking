#include "socket.h"
#include <iostream>
#include <memory>

#ifdef _WIN32
#include <WS2tcpip.h>
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif

namespace NetSock
{
    bool NetSocket::sendTCP(const NetMSG& msg) const
    {
        NetMsgHeader const& header = msg.getConstHeader();
        NetMsgOrder const& order = msg.getConstMsgOrder();
    #ifdef _WIN32
        if(!sendHeader(header))
            return false;
    
        if(!sendOrder(order))
            return false;
        std::vector<NetPackage> packages = msg.toPackages();
        auto result = sendPackages(packages);
        if(result != header.mMsgSize)
            return false;
    #elif __unix__
    #else
    #endif
        return true;
    }
    
    NetMSG NetSocket::recvTCP() const
    {
        NetMSG msg;
        NetMsgHeader header;
        NetMsgOrder order;
    
    #if _WIN32
        header = recvHeader();
        order = recvOrder(header.mMsgOrderCount);
    
        for(auto packages = recvPackages(header.mMsgSize); 
            auto const& package : packages)
            msg.add(package.data(), package.size());
    
        msg.setHeader(header);
        msg.setOrder(order);
        
    #elif __unix__
    #else
    #endif
        return msg;
    }
}