#ifndef MSG_TYPE_H
#define MSG_TYPE_H

namespace NetSock
{
    enum class NetMsgType : unsigned int
    {
        NONE = 0,
        CONNECT_CLIENT_SIDE = 1,
        CONNECT_SERVER_SIDE = 2,
        DISCONECT = 3,
        PING = 4,
        LAST = PING,
    };

    NetMsgType UserDefType(unsigned int type);
}

#endif // MSG_TYPE_H