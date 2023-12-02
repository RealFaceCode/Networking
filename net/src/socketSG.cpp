#include "socket.h"

namespace NetSock
{
void NetSocket::setIP(const IPv4 &ip)
    {
        mIPv4 = ip;
        mSockIPType = NetSocketIPType::IPv4;
    }

    void NetSocket::setIP(const IPv6 &ip)
    {
        mIPv6 = ip;
        mSockIPType = NetSocketIPType::IPv6;
    }

    void NetSocket::setPort(const Port &port)
    {
        mPort = port;
    }

    void NetSocket::setProtocol(NetSocketProtocol sProt)
    {
        mSockProt = sProt;
    }

    void NetSocket::setType(NetSocketType sType)
    {
        mSockType = sType;
    }

    IPv4 NetSocket::getIPv4() const
    {
        return mIPv4;
    }

    IPv6 NetSocket::getIPv6() const
    {
        return mIPv6;
    }

    std::string NetSocket::getIP() const
    {
        if(mIPv4)
            return mIPv4.ip().data();
        else if(mIPv6)
            return mIPv6.ip().data();
        else
            return std::string();
    }

    Port NetSocket::getPort() const
    {
        return mPort;
    }

    NetSocketProtocol NetSocket::getSockProt() const
    {
        return mSockProt;
    }

    NetSocketType NetSocket::getSockType() const
    {
        return mSockType;
    }

    void NetSocket::setHandle(NET_SOCKET handle)
    {
        mSocket = handle;
    }

    NET_SOCKET NetSocket::getHandle() const
    {
        return mSocket;
    }
}