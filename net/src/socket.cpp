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
    NetSocket::NetSocket() = default;

    NetSocket::NetSocket(const IPv4 &ip, const Port &port, NetSocketProtocol sProt, NetSocketType sType, bool block)
    : mIPv4(ip), mPort(port), mSockProt(sProt), mSockType(sType), mSockIPType(NetSocketIPType::IPv4), mBlock(block)
    {
        if(!ip)
            mIPv4 = IPv4("0");
    }

    NetSocket::NetSocket(const IPv6 &ip, const Port &port, NetSocketProtocol sProt, NetSocketType sType, bool block)
    : mIPv6(ip), mPort(port), mSockProt(sProt), mSockType(sType), mSockIPType(NetSocketIPType::IPv6), mBlock(block)
    {
        if(!ip)
            mIPv6 = IPv6("0");
    }

    NetSocket::~NetSocket() = default;

    bool NetSocket::block(bool blocking) const
    {
        unsigned long mode = 0;
        if(blocking)
            mode = 1;

    #ifdef _WIN32
        if (::ioctlsocket(mSocket, FIONBIO, &mode) == SOCKET_ERROR)
            return false;
    #elif __unix__
    #else
    #error NOT SUPPORTED YET!
    #endif
        return true;
    }

    bool NetSocket::connect()
    {
        if(mSockType == NetSocketType::SERVER || mSockType == NetSocketType::NONE)
            return false;

        switch (mSockProt)
        {
        using enum NetSock::NetSocketProtocol;
        case NONE:
            return false;
        case TCP:
            return connectTCP();
        case UDP:
            return connectUDP();
        default:
            return false;
        }
    }

    bool NetSocket::listen()
    {
        if(mSockType == NetSocketType::CLIENT || mSockType == NetSocketType::NONE)
            return false;

    #if _WIN32
        addrinfo* result = nullptr;
        addrinfo hints;

        ZeroMemory(&hints, sizeof (hints));
        hints.ai_flags = AI_PASSIVE;

        if(mIPv4)
            hints.ai_family = AF_INET;
        else if(mIPv6)
            hints.ai_family = AF_INET6;
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

        if (::getaddrinfo(nullptr, mPort.port().data(), &hints, &result) != 0)
            return false;

        mSocket = ::socket(result->ai_family, result->ai_socktype, result->ai_protocol);
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

        if (::bind( mSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
        {
            ::freeaddrinfo(result);
            ::closesocket(mSocket);
            return false;
        }

        ::freeaddrinfo(result);

        if(mSockProt == NetSocketProtocol::TCP && ::listen(mSocket, SOMAXCONN ) == SOCKET_ERROR)
        {
            ::closesocket(mSocket);
            return false;
        }
    #elif __unix__
    #else
    #endif
        return true;
    }

    NetSocket NetSocket::accept() const
    {
        if(mSockType == NetSocketType::CLIENT || mSockType == NetSocketType::NONE)
            return NetSocket();

        switch (mSockProt)
        {
        using enum NetSock::NetSocketProtocol;
        case NONE:
            return NetSocket();
        case TCP:
            return acceptTCP();
        case UDP:
            return acceptUDP();
        default:
            return NetSocket();
        }
    }

    bool NetSocket::disconnect(const NetMSG& msg) const
    {
        if(msg.getMsgType() != NetMsgType::DISCONECT)
            return false;

        if(mSockType != NetSocketType::SERVER && !send(msg))
            return false;

    #ifdef _WIN32
        if (auto iResult = ::shutdown(mSocket, SD_SEND | SD_RECEIVE);
            iResult != SOCKET_ERROR)
            return false;
        ::closesocket(mSocket);
    #elif __unix__
    #else
    #endif

        return true;
    }

    bool NetSocket::closeSend() const
    {
        return SOCKET_ERROR == ::shutdown(mSocket, SD_SEND);
    }

    bool NetSocket::closeRecv() const
    {
        return SOCKET_ERROR == ::shutdown(mSocket, SD_RECEIVE);
    }

    NetSocket::operator bool() const
    {
        return (mIPv4 || mIPv6) && mPort && mSockProt != NetSocketProtocol::NONE && mSockType != NetSocketType::NONE;
    }

    std::pair<NetSocketConnectionInfo, std::string> NetSocket::waitForConnection(long time) const
    {
        using enum NetSocketConnectionInfo;
    #ifdef _WIN32
        if (int error = ::WSAGetLastError(); error == WSAEWOULDBLOCK) {
                struct pollfd fd;
                int pollResult;

                fd.fd = mSocket;
                fd.events = POLLOUT;

                pollResult = ::WSAPoll(&fd, 1, 1000 * time);

                if (pollResult > 0)
                {
                    if (fd.revents & POLLOUT) 
                        return {SUCCESS, ""};
                    else 
                        return {FAILED, ""};
                }
                else if (pollResult == 0)
                    return {TIMEOUT, ""};
                else 
                {
                    LPSTR errorMessage = nullptr;
                    ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                            nullptr,
                            error,
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            (LPSTR)&errorMessage,
                            0,
                            nullptr);

                    return {FAILED, errorMessage ? errorMessage : "FAILED TO GET ERR MSG"};
                }
            } 
            else 
            {
                LPSTR errorMessage = nullptr;
                ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        nullptr,
                        error,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        (LPSTR)&errorMessage,
                        0,
                        nullptr);

                return {FAILED, errorMessage ? errorMessage : "FAILED TO GET ERR MSG"};
            }
    #elif __unix__
    #else
    #error NOT SUPPORTED YET!
    #endif

        return {FAILED, ""};
    }
}