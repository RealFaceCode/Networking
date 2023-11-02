#ifndef MSG_TYPE_H
#define MSG_TYPE_H

enum class MsgType : unsigned int
{
    NONE = 0,
    LAST = NONE,
};

MsgType UserDefType(unsigned int type);

#endif // MSG_TYPE_H