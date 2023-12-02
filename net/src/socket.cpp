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

    std::pair<NetMSG, bool> NetSocket::connect(const NetMSG& msg)
    {
        if(mSockType == NetSocketType::SERVER || mSockType == NetSocketType::NONE)
            return {NetMSG(), false};

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
            return {NetMSG(), false};

        switch(mSockProt)
        {
            using enum NetSocketProtocol;
            case NONE:
                return {NetMSG(), false};
            case UDP:
                hints.ai_socktype = SOCK_DGRAM;
                hints.ai_protocol = IPPROTO_UDP;
                break;
            case TCP:
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;
                break;
            default:
                return {NetMSG(), false};
        }

        if (const char* addr = mIPv4 ? mIPv4.ip().data() : mIPv6.ip().data(); 
            ::getaddrinfo(addr, mPort.port().data(), &hints, &result) != 0)
            return {NetMSG(), false};

        //TODO: loop all addr
        ptr = result;

        mSocket = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (mSocket == INVALID_SOCKET) 
        {
            ::freeaddrinfo(result);
            return {NetMSG(), false};
        }

        if(!block(mBlock))
        {
            ::freeaddrinfo(result);
            return {NetMSG(), false};
        }

        if(mSockProt == NetSocketProtocol::TCP && ::connect(mSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR
        && waitForConnection().first != NetSocketConnectionInfo::SUCCESS)
        {
            std::cout << ::strerror(errno) << std::endl;
            ::closesocket(mSocket);
            ::freeaddrinfo(result);
            mSocket = INVALID_SOCKET;
            return {NetMSG(), false};
        }

        ::freeaddrinfo(result);
    #elif __unix__
    #else
    #endif

        if(msg.getMsgType() == NetMsgType::CONNECT_CLIENT_SIDE && !mBlock)
        {
            send(msg);
            return {recv(), true};
        }
        else 
            return {NetMSG(), true};
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

    std::pair<NetSocket, NetMSG> NetSocket::accept(const NetMSG& msg) const
    {
        if(mSockType == NetSocketType::CLIENT || mSockType == NetSocketType::NONE)
            return {NetSocket(), NetMSG()};

        NetSocket inSock;

    #if _WIN32

        if(mSockProt == NetSocketProtocol::TCP)
        {
            auto clientSocket = INVALID_SOCKET;
            sockaddr_in clientAddress;
            int clientAddressSize = sizeof(clientAddress);

            clientSocket = ::accept(mSocket, (sockaddr*)&clientAddress, &clientAddressSize);
            if (clientSocket == INVALID_SOCKET)
            {
                //::closesocket(mSocket);
                return {NetSocket(), NetMSG()};
            }

            inSock.mSocket = clientSocket;
            inSock.mPort = mPort;
            inSock.mSockProt = mSockProt;
            inSock.mSockType = NetSocketType::CLIENT;

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
        }
    #elif __unix__
    #else
    #endif

        if(msg.getMsgType() == NetMsgType::CONNECT_SERVER_SIDE && !mBlock)
        {
            inSock.send(msg);
            return {inSock, inSock.recv()};
        }
        else
            return {inSock, NetMSG()};
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

    NET_SOCKET NetSocket::getHandle() const
    {
        return mSocket;
    }

    NetSocket::operator bool() const
    {
        return (mIPv4 || mIPv6) && mPort && mSockProt != NetSocketProtocol::NONE && mSockType != NetSocketType::NONE;
    }

    void NetSocket::setHandle(NET_SOCKET handle)
    {
        mSocket = handle;
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