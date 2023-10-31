#ifndef MSG_TYPE_DEFINE_H
#define MSG_TYPE_DEFINE_H

#include <vector>
#include <initializer_list>
#include <cstdint>

enum class MsgOrderType : uint8_t
{
    I8, I16, I32, I64,
    UI8, UI16, UI32, UI64,
    F32, F64, STRING,
};

class MsgOrder
{
private:
    std::vector<MsgOrderType> mMsgOrderValues;
    
public:
    MsgOrder();
    explicit MsgOrder(const std::initializer_list<MsgOrderType>& list);
    ~MsgOrder();

    void addToOrder(MsgOrderType type);
    void addOrder(const std::initializer_list<MsgOrderType>& list);

    std::vector<MsgOrderType> getMsgOrder() const;
};

#endif // MSG_TYPE_DEFINE