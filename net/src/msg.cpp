
#include "msg.h"
#include "core.h"

#include <bit>

std::unordered_map<MsgType, MsgOrder> NetMSG::mOrders;

NetMSG::NetMSG() = default;

NetMSG::NetMSG(MsgType msgType)
{
    mHeader.mMsgType = msgType;
}

NetMSG::~NetMSG() = default;

void NetMSG::add(const char *msg, std::size_t msgLen)
{
    mMsgBuffer.insert(mMsgBuffer.end(), msg, msg + msgLen);
    mHeader.mMsgSize = mMsgBuffer.size();
}

void NetMSG::add(const std::uint8_t *msg, std::size_t msgLen)
{
    mMsgBuffer.insert(mMsgBuffer.end(), msg, msg + msgLen);
}

void NetMSG::add(std::string_view msg)
{
    mMsgBuffer.insert(mMsgBuffer.end(), msg.data(), msg.data() + msg.size());
}

void NetMSG::add(const std::vector<std::uint8_t> &msg)
{
    mMsgBuffer.insert(mMsgBuffer.end(), msg.data(), msg.data() + msg.size());
}

template<typename Type>
void NetMSG::add(Type msg)
{
    Type value;
    if(!::IsBigEndian())
        value = ::ReverseByteOrder(msg);
    uint8_t* addValue = std::bit_cast<uint8_t*>(&value);
    mMsgBuffer.insert(mMsgBuffer.end(), addValue, addValue + sizeof(Type));
}

template void NetMSG::add<int8_t>(int8_t);
template void NetMSG::add<int16_t>(int16_t);
template void NetMSG::add<int32_t>(int32_t);
template void NetMSG::add<int64_t>(int64_t);
template void NetMSG::add<uint8_t>(uint8_t);
template void NetMSG::add<uint16_t>(uint16_t);
template void NetMSG::add<uint32_t>(uint32_t);
template void NetMSG::add<uint64_t>(uint64_t);
template void NetMSG::add<float>(float);
template void NetMSG::add<double>(double);

void NetMSG::setType(MsgType msgType)
{
    mHeader.mMsgType = msgType;
}

std::string NetMSG::getAsString() const
{
    std::string msg;
    msg.insert(msg.begin(), mMsgBuffer.data(), mMsgBuffer.data() + mMsgBuffer.size());
    return msg;
}

std::vector<std::uint8_t> NetMSG::getAsVec() const
{
    return mMsgBuffer;
}

void NetMSG::AddOrder(MsgType type, const MsgOrder &order)
{
    for(auto const& [key, value] : mOrders)
    {
        if(key == type)
        {
            return;
        }
    }

    mOrders.try_emplace(type, order);
}

void NetMSG::AddOrder(MsgType type, const std::initializer_list<MsgOrderType> &list)
{
    for(auto const& [key, value] : mOrders)
    {
        if(key == type)
        {
            return;
        }
    }
    
    mOrders.try_emplace(type, list);
}

MsgOrder NetMSG::GetOrder(MsgType type)
{
    for(auto const& [key, value] : mOrders)
    {
        if(key == type)
        {
            return value;
        }
    }

    return MsgOrder();
}
