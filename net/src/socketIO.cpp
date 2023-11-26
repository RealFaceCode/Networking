#include "socket.h"
#include <iostream>
#include <memory>

#ifdef _WIN32
#include <WS2tcpip.h>
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif

bool NetSocket::send(const NetMSG &msg) const
{
    if(mSockType == NetSocketType::SERVER || mSockType == NetSocketType::NONE)
        return false;

    switch(mSockProt)
    {
        using enum NetSocketProtocol;
        case NONE:
            return false;
        case UDP:
            return sendUDP(msg);
        case TCP:
            return sendTCP(msg);
        default:
            return false;
    }
}

NetMSG NetSocket::recv() const
{

    if(mSockType ==  NetSocketType::NONE)
        return NetMSG();

    switch(mSockProt)
    {
        using enum NetSocketProtocol;
        case NONE:
            return NetMSG();
        case UDP:
            return recvUDP();
        case TCP:
            return recvTCP();
        default:
            return NetMSG();
    }
}

bool NetSocket::sendHeader(const NetMsgHeader &header) const
{
    NetMsgHeader sendHeader;
    sendHeader.mMsgSize = ::htonll(header.mMsgSize);
    sendHeader.mMsgOrderCount = ::htonll(header.mMsgOrderCount);
    sendHeader.mMsgType = static_cast<NetMsgType>(::htonl(static_cast<unsigned int>(header.mMsgType)));

#ifdef _WIN32
    if(int result = ::send(mSocket, (const char*)(&sendHeader), sizeof(NetMsgHeader), 0); 
        result <= 0 || result != sizeof(NetMsgHeader))
        return false;
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif
    return true;
}

bool NetSocket::sendOrder(const NetMsgOrder &order) const
{
    auto const& vec = order.constRef();

    if(vec.empty())
        return true;
    
#ifdef _WIN32

    if(int result = ::send(mSocket, (const char*)(vec.data()), static_cast<int>(vec.size()), 0); 
        result <= 0 || result != vec.size())
        return false;
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif
    
    return true;
}

std::size_t NetSocket::sendPackages(const std::vector<NetPackage> &packages) const
{
    std::size_t len = 0;

#ifdef _WIN32
    for(const auto& packet : packages)
    {
        int result = ::send(mSocket, packet.data(), static_cast<int>(packet.size()), 0);
        if(result == 0 || result != packet.size())
        {
            len += result;
            break;
        }

        len += result;
    }
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif
    return len;
}

NetMsgHeader NetSocket::recvHeader() const
{
NetMsgHeader msgHeader;

#ifdef _WIN32
    if (int result = ::recv(mSocket, std::bit_cast<char*>(&msgHeader), sizeof(NetMsgHeader), 0);
            result != sizeof(NetMsgHeader))
        return NetMsgHeader(); 
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif
    
    msgHeader.mMsgSize = ::ntohll(msgHeader.mMsgSize);
    msgHeader.mMsgOrderCount = ::ntohll(msgHeader.mMsgOrderCount);
    msgHeader.mMsgType = static_cast<NetMsgType>(::ntohl(static_cast<unsigned int>(msgHeader.mMsgType)));

    return msgHeader;
}

NetMsgOrder NetSocket::recvOrder(std::size_t size) const
{
    NetMsgOrder order;
    order.ref().reserve(size);
    order.ref().resize(size);
    
#ifdef _WIN32
    if (int result = ::recv(mSocket, (char*)&order.ref().at(0), static_cast<int>(size), 0);
            result != size)
        return NetMsgOrder(); 
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif
    return order;
}

std::vector<NetPackage> NetSocket::recvPackages(std::size_t maxMsgLen) const
{
    std::size_t recvBytes = 0;
    std::vector<NetPackage> packages;
#ifdef _WIN32
    while(recvBytes < maxMsgLen)
    {
        std::string recvBuf;
        recvBuf.reserve(::MAX_MSG_LEN);
        recvBuf.resize(::MAX_MSG_LEN);

        int result = ::recv(mSocket, &recvBuf.at(0), ::MAX_MSG_LEN, 0);
        if(result <= 0)
            return packages;
        else 
        {
            packages.emplace_back(recvBuf);
            recvBytes += result;
        }
    }
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif
    return packages;
}
