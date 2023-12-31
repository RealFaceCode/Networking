#ifndef SOCKET_H
#define SOCKET_H

#include "core.h"
#include "msg.h"
#include "ipv4.h"
#include "ipv6.h"
#include "port.h"
#include "package.h"

#include <vector>
#include <string_view>

namespace NetSock
{
#ifdef _WIN32
    using NET_SOCKET = unsigned long long;
#elif __unix__
    using NET_SOCKET = int;
#else
#error NOT SUPPORTED YET!
#endif

    enum class NetSocketProtocol
    {
        NONE, TCP, UDP
    };

    enum class NetSocketType
    {
        NONE, CLIENT, SERVER
    };

    enum class NetSocketIPType
    {
        NONE, IPv4, IPv6
    };

    enum class NetSocketConnectionInfo
    {
        FAILED = 0,
        SUCCESS = 1,
        TIMEOUT = 2
    };

    class NetSocket
    {
    private:
        NET_SOCKET mSocket = 0;
        IPv4 mIPv4 = {};
        IPv6 mIPv6 = {};
        Port mPort = {};
        NetSocketProtocol mSockProt = NetSocketProtocol::NONE;
        NetSocketType mSockType = NetSocketType::NONE;
        NetSocketIPType mSockIPType = NetSocketIPType::NONE;
        bool mBlock = false;

    public:
        NetSocket();
        NetSocket(const IPv4& ip, const Port& port, NetSocketProtocol sProt, NetSocketType sType, bool block = false);
        NetSocket(const IPv6& ip, const Port& port, NetSocketProtocol sProt, NetSocketType sType, bool block = false);
        ~NetSocket();

        void setIP(const IPv4& ip);
        void setIP(const IPv6& ip);
        void setPort(const Port& port);
        void setProtocol(NetSocketProtocol sProt);
        void setType(NetSocketType sType);

        IPv4 getIPv4() const;
        IPv6 getIPv6() const;
        std::string getIP() const;
        Port getPort() const;
        NetSocketProtocol getSockProt() const;
        NetSocketType getSockType() const;

        bool block(bool blocking) const;

        bool connect();
        bool listen();
        NetSocket accept() const;
        bool disconnect(const NetMSG& msg = NetMSG(NetMsgType::DISCONECT)) const;

        bool send(const NetMSG& msg) const;
        NetMSG recv() const;

        bool closeSend() const;
        bool closeRecv() const;

        NET_SOCKET getHandle() const;

        explicit operator bool() const;

    private:
        void setHandle(NET_SOCKET handle);
        //TCP IO
        bool sendHeaderTCP(const NetMsgHeader& header) const;
        bool sendOrderTCP(const NetMsgOrder& order) const;
        std::size_t sendPackagesTCP(const std::vector<NetPackage>& packages) const;
        NetMsgHeader recvHeaderTCP() const;
        NetMsgOrder recvOrderTCP(std::size_t size) const;
        std::vector<NetPackage> recvPackagesTCP(std::size_t maxMsgLen) const;
        bool sendTCP(const NetMSG& msg) const;
        NetMSG recvTCP() const;    
        //UDP IO
        bool sendHeaderUDP(const NetMsgHeader& header) const;
        bool sendOrderUDP(const NetMsgOrder& order) const;
        std::size_t sendPackagesUDP(const std::vector<NetPackage>& packages) const;
        NetMsgHeader recvHeaderUDP() const;
        NetMsgOrder recvOrderUDP(std::size_t size) const;
        std::vector<NetPackage> recvPackagesUDP(std::size_t maxMsgLen) const;
        bool sendUDP(const NetMSG& msg) const;
        NetMSG recvUDP() const;

        std::pair<NetSocketConnectionInfo, std::string> waitForConnection(long time = 10) const;
        NetSocket acceptTCP() const;
        NetSocket acceptUDP() const;
        bool connectTCP() ;
        bool connectUDP();
    };
}

#endif // SOCKET_H