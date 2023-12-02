#include "msgOrder.h"

namespace NetSock
{
    NetMsgOrder::NetMsgOrder() = default;

    NetMsgOrder::NetMsgOrder(const std::initializer_list<NetMsgOrderType>& list)
    {
        mMsgOrderValues.insert(mMsgOrderValues.end(), list.begin(), list.end());
    }

    NetMsgOrder::~NetMsgOrder() = default;

    void NetMsgOrder::addToOrder(NetMsgOrderType type)
    {
        mMsgOrderValues.emplace_back(type);
    }

    void NetMsgOrder::addOrder(const std::initializer_list<NetMsgOrderType>& list)
    {
        mMsgOrderValues.insert(mMsgOrderValues.end(), list.begin(), list.end());
    }

    std::vector<NetMsgOrderType> &NetMsgOrder::ref()
    {
        return mMsgOrderValues;
    }

    const std::vector<NetMsgOrderType> &NetMsgOrder::constRef() const
    {
        return mMsgOrderValues;
    }

    std::vector<NetMsgOrderType> NetMsgOrder::getMsgOrder() const
    {
        return mMsgOrderValues;
    }
}