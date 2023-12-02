#ifndef MSG_HEADER_H
#define MSG_HEADER_H

#include "msgType.h"
#include <string>
#include <vector>
#include <cstdint>

namespace NetSock
{
    class NetMsgHeader
    {
    public:
        std::size_t mMsgSize = 0;
        std::size_t mMsgOrderCount = 0;
        NetMsgType mMsgType = NetMsgType::NONE;

        std::vector<uint8_t> toVec() const;
    };
}
#endif // MSG_HEADER_H