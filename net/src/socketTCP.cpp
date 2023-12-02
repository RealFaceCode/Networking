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
    bool NetSocket::connectTCP()
    {
    #if _WIN32

        addrinfo* result = nullptr;
        addrinfo const* ptr = nullptr;
        addrinfo hints;

        ZeroMemory(&hints, sizeof(hints));

        if(mIPv4)
        {
            hints.ai_family = AF_INET;
        }
        else if(mIPv6)
        {
            hints.ai_family = AF_INET6;
        }
        else 
            return false;

        switch(mSockProt)
        {
            using enum NetSocketProtocol;
            case NONE:
                return false;
            case UDP:
                hints.ai_socktype = SOCK_DGRAM;
                hints.ai_protocol = IPPROTO_UDP;
                break;
            case TCP:
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;
                break;
            default:
                return false;
        }

        if (const char* addr = mIPv4 ? mIPv4.ip().data() : mIPv6.ip().data(); 
            ::getaddrinfo(addr, mPort.port().data(), &hints, &result) != 0)
            return false;

        //TODO: loop all addr
        ptr = result;

        mSocket = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (mSocket == INVALID_SOCKET) 
        {
            ::freeaddrinfo(result);
            return false;
        }

        if(!block(mBlock))
        {
            ::freeaddrinfo(result);
            return false;
        }

        if(mSockProt == NetSocketProtocol::TCP && ::connect(mSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR
        && waitForConnection().first != NetSocketConnectionInfo::SUCCESS)
        {
            std::cout << ::strerror(errno) << std::endl;
            ::closesocket(mSocket);
            ::freeaddrinfo(result);
            mSocket = INVALID_SOCKET;
            return false;
        }

        ::freeaddrinfo(result);
    #elif __unix__
    #else
    #endif
    
        return true;
    }

        NetSocket NetSocket::acceptTCP() const
    {
        NetSocket inSock;
    #if _WIN32
        
        auto clientSocket = INVALID_SOCKET;
        SOCKADDR_IN clientAddress;
        int clientAddressSize = sizeof(clientAddress);

        clientSocket = ::accept(mSocket, (sockaddr*)&clientAddress, &clientAddressSize);
        if (clientSocket == INVALID_SOCKET)
        {
            //::closesocket(mSocket);
            return NetSocket();
        }

        inSock.mSocket = clientSocket;
        inSock.mPort = mPort;
        inSock.mSockProt = mSockProt;
        inSock.mSockType = NetSocketType::CLIENT;
        inSock.block(mBlock);

        if(char ip_str[INET_ADDRSTRLEN]; ::inet_ntop(AF_INET, &clientAddress.sin_addr, ip_str, INET_ADDRSTRLEN) != nullptr)
        {
            std::string addr = ip_str;
            if(addr.find(".") != std::string::npos)
            {
                inSock.mIPv4 = IPv4(addr);
                inSock.mSockIPType = NetSocketIPType::IPv4;
            }
            else if(addr.find(":") != std::string::npos)
            {
                inSock.mIPv6 = IPv6(addr);
                inSock.mSockIPType = NetSocketIPType::IPv6;
            }
        }

        inSock.setPort(Port(clientAddress.sin_port));
    #elif __unix__
    #else
    #endif

        return inSock;
    }

    bool NetSocket::sendTCP(const NetMSG& msg) const
    {
        NetMsgHeader const& header = msg.getConstHeader();
        NetMsgOrder const& order = msg.getConstMsgOrder();
    #ifdef _WIN32
        if(!sendHeaderTCP(header))
            return false;
    
        if(!sendOrderTCP(order))
            return false;

        std::vector<NetPackage> packages = msg.toPackages();
        if(auto result = sendPackagesTCP(packages); result != header.mMsgSize)
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
        header = recvHeaderTCP();
        order = recvOrderTCP(header.mMsgOrderCount);
    
        for(auto packages = recvPackagesTCP(header.mMsgSize); 
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