#include "msgType.h"

namespace NetSock
{
    NetMsgType UserDefType(unsigned int type)
    {
        return static_cast<NetMsgType>(static_cast<unsigned int>(NetMsgType::LAST) + 1 + type);
    }
}