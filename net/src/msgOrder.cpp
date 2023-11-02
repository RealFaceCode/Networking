#include "msgOrder.h"

MsgOrder::MsgOrder() = default;

MsgOrder::MsgOrder(const std::initializer_list<MsgOrderType>& list)
{
    mMsgOrderValues.insert(mMsgOrderValues.end(), list.begin(), list.end());
}

MsgOrder::~MsgOrder() = default;

void MsgOrder::addToOrder(MsgOrderType type)
{
    mMsgOrderValues.emplace_back(type);
}

void MsgOrder::addOrder(const std::initializer_list<MsgOrderType>& list)
{
    mMsgOrderValues.insert(mMsgOrderValues.end(), list.begin(), list.end());
}

std::vector<MsgOrderType> MsgOrder::getMsgOrder() const
{
    return mMsgOrderValues;
}
