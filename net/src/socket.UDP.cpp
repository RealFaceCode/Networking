#include "socket.h"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <WS2tcpip.h>
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif

namespace NetSock
{
    bool NetSocket::connectUDP()
    {
    #if _WIN32
        addrinfo* result = nullptr;
        addrinfo const* ptr = nullptr;
        addrinfo hints;

        ZeroMemory(&hints, sizeof(hints));

        if(mIPv4)
            hints.ai_family = AF_INET;
        else if(mIPv6)
            hints.ai_family = AF_INET6;
        else 
            return false;

        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

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

        SOCKADDR_IN clientAddr;
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = ::htons(mPort.toInteger());
        clientAddr.sin_addr.s_addr = ::inet_addr(getIP().c_str());

        std::string message;
        int bytesSent = ::sendto(mSocket, message.c_str(), static_cast<int>(message.size()), 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
        if (bytesSent != SOCKET_ERROR)
            return true;

        int timeoutSeconds = 10;
        auto start = std::chrono::steady_clock::now();
        while (true)
        {
            auto end = std::chrono::steady_clock::now();

            if (auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start).count(); 
                elapsed >= timeoutSeconds)
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            if (bytesSent = ::sendto(mSocket, message.c_str(), static_cast<int>(message.size()), 0, (SOCKADDR*)&clientAddr, sizeof(clientAddr)),
                bytesSent != SOCKET_ERROR)
                return true;
        }

        ::freeaddrinfo(result);
    #elif __unix__
    #else
    #endif

        return false;
    }

    NetSocket NetSocket::acceptUDP() const
    {
        NetSocket inSock;
    #if _WIN32

        SOCKADDR_IN clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        ZeroMemory(&clientAddr, clientAddrSize);
        std::string buffer;
        buffer.reserve(MAX_MSG_LEN);
        buffer.resize(MAX_MSG_LEN);

        if (int bytesReceived = ::recvfrom(mSocket, &buffer.at(0), MAX_MSG_LEN, 0, (SOCKADDR*)&clientAddr, &clientAddrSize); 
            bytesReceived == SOCKET_ERROR)
            return NetSocket();

        inSock.mSocket = mSocket;
        inSock.mPort = mPort;
        inSock.mSockProt = mSockProt;
        inSock.mSockType = NetSocketType::CLIENT;
        inSock.mBlock = mBlock;

        if(char ip_str[INET_ADDRSTRLEN]; ::inet_ntop(AF_INET, &clientAddr.sin_addr, ip_str, INET_ADDRSTRLEN) != nullptr)
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

        inSock.setPort(Port(clientAddr.sin_port));
    #elif __unix__
    #else
    #endif

        return inSock;
    }

    bool NetSocket::sendUDP(const NetMSG& msg) const
    {
        NetMsgHeader const& header = msg.getConstHeader();
        NetMsgOrder const& order = msg.getConstMsgOrder();
    #ifdef _WIN32
        if(!sendHeaderUDP(header))
            return false;
    
        if(!sendOrderUDP(order))
            return false;

        std::vector<NetPackage> packages = msg.toPackages();
        if(auto result = sendPackagesUDP(packages); result != header.mMsgSize)
            return false;
    #elif __unix__
    #else
    #endif
        return true;
    }

    NetMSG NetSocket::recvUDP() const
    {
        NetMSG msg;
        NetMsgHeader header;
        NetMsgOrder order;
    
    #if _WIN32
        header = recvHeaderUDP();
        order = recvOrderUDP(header.mMsgOrderCount);
    
        for(auto packages = recvPackagesUDP(header.mMsgSize); 
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