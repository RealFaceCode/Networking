#ifndef MSG_HEADER_H
#define MSG_HEADER_H

#include "msgType.h"
#include "msgOrder.h"

struct MsgHeader
{
    std::size_t mMsgSize = 0;
    MsgType mMsgType = MsgType::NONE;
    MsgOrder mOrder = {};
};

#endif // MSG_HEADER_H