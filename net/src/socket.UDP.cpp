#include "socket.h"
#include <iostream>
#include <memory>

#ifdef _WIN32
#include <WS2tcpip.h>
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif

bool NetSocket::sendUDP(const NetMSG& msg) const
{
    return false;

    std::size_t msgLen = 0;
    std::size_t lenSendMsg = 0;
    NetMsgHeader const& header = msg.getConstHeader();

#ifdef _WIN32 // TODO: rewrite for udp?
    sockaddr_in addr;
    if(mIPv4)
        addr.sin_family = AF_INET;
    else if(mIPv6)
        addr.sin_family = AF_INET6;
    addr.sin_addr.s_addr = ::inet_addr(getIP().data());
    addr.sin_port = ::htons(mPort.toInteger());
    int lenAddr = sizeof(addr);

    auto buf = header.toVec();
    msgLen = ::sendto(mSocket, (char*)buf.data(), sizeof(NetMsgHeader), 0, (sockaddr*)&addr, lenAddr);
    if(msgLen <= 0 || msgLen != sizeof(NetMsgHeader))
        return false;

    auto order = msg.getConstMsgOrder().constRef().data();
    if(order)
        msgLen = ::sendto(mSocket, (char*)buf.data(), static_cast<int>(header.mMsgOrderCount), 0, (sockaddr*)&addr, lenAddr);
    if(msgLen <= 0 && order )
        return false;

    std::string msgString = msg.getAsString();
    while (lenSendMsg < header.mMsgSize)
    {
        auto buffer = msgString.substr(0, ::MAX_MSG_LEN);
        msgString.erase(0, ::MAX_MSG_LEN);
        lenSendMsg += ::sendto(mSocket, (char*)buf.data(), static_cast<int>(buffer.size()), 0, (sockaddr*)&addr, lenAddr);
    }
#elif __unix__
#else
#endif
    return true;
}

NetMSG NetSocket::recvUDP() const
{
    int msgLen = 0;
    std::size_t lenRecvMsg = 0;
    NetMSG msg;
    NetMsgHeader header;
    NetMsgOrder order;

    return NetMSG();

#if _WIN32 //TODO: rewrite for udp?
    std::unique_ptr<char, decltype(&::free)> bufHead(new char[sizeof(NetMsgHeader)], &::free);
    if(!bufHead)
        return NetMSG();

    sockaddr_in addr;
    int lenAddr = sizeof(addr);


    msgLen = ::recvfrom(mSocket, bufHead.get(), sizeof(NetMsgHeader), 0, (sockaddr*)&addr, &lenAddr);
    if(msgLen <= 0 || msgLen != sizeof(NetMsgHeader))
        return NetMSG();

    ::memcpy(&header, bufHead.get(), sizeof(NetMsgHeader));

    if(header.mMsgOrderCount > 0)
    {
        order.ref().reserve(header.mMsgOrderCount);
        order.ref().resize(header.mMsgOrderCount);

        std::unique_ptr<char, decltype(&::free)> buf(new char[sizeof(NetMsgHeader)], &::free);
        if(!buf)
            return NetMSG();

        msgLen = ::recvfrom(mSocket, buf.get(), static_cast<int>(header.mMsgOrderCount), 0, (sockaddr*)&addr, &lenAddr);
        if(msgLen <= 0)
        {
            return NetMSG();
        }

        ::memcpy(order.ref().data(), buf.get(), header.mMsgOrderCount);
        msg.getMsgOrder() = order;
    }
    
    while (lenRecvMsg < header.mMsgSize)
    {
        std::string recvBuffer;
        recvBuffer.reserve(::MAX_MSG_LEN);
        recvBuffer.resize(::MAX_MSG_LEN);

        auto len = ::recvfrom(mSocket, recvBuffer.data(), ::MAX_MSG_LEN, 0, (sockaddr*)&addr, &lenAddr);
        if(len < 0)
            return msg;

        recvBuffer.resize(len);
        msg.addStr(recvBuffer);
        lenRecvMsg += len;
    }
    
    msg.setHeader(header);
#elif __unix__
#else
#endif
    return msg;
}