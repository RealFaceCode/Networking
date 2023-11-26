#ifndef MSG_TYPE_DEFINE_H
#define MSG_TYPE_DEFINE_H

#include <vector>
#include <initializer_list>
#include <cstdint>

enum class NetMsgOrderType : uint8_t
{
    I8, I16, I32, I64,
    UI8, UI16, UI32, UI64,
    F32, F64, STRING,
};

class NetMsgOrder
{
private:
    std::vector<NetMsgOrderType> mMsgOrderValues;
    
public:
    NetMsgOrder();
    explicit NetMsgOrder(const std::initializer_list<NetMsgOrderType>& list);
    ~NetMsgOrder();

    void addToOrder(NetMsgOrderType type);
    void addOrder(const std::initializer_list<NetMsgOrderType>& list);

    std::vector<NetMsgOrderType>& ref();
    const std::vector<NetMsgOrderType>& constRef() const;
    std::vector<NetMsgOrderType> getMsgOrder() const;
};

#endif // MSG_TYPE_DEFINE