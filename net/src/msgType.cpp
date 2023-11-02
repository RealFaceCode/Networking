#include "msgType.h"

MsgType UserDefType(unsigned int type)
{
    return static_cast<MsgType>(static_cast<int>(MsgType::LAST) + 1 + type);
}
