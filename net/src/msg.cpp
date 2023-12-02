
#include "msg.h"
#include "core.h"

#include <bit>

namespace NetSock
{
    std::unordered_map<NetMsgType, NetMsgOrder> NetMSG::mOrders;

    NetMSG::NetMSG() = default;

    NetMSG::NetMSG(NetMsgType msgType)
    {
        mHeader.mMsgType = msgType;
        mOrder = GetOrder(msgType);
        mHeader.mMsgOrderCount = mOrder.getMsgOrder().size();
    }

    NetMSG::~NetMSG() = default;

    void NetMSG::add(const char *msg, std::size_t msgLen)
    {
        mHeader.mMsgSize += msgLen;
        mMsgBuffer.insert(mMsgBuffer.end(), msg, msg + msgLen);
        mHeader.mMsgSize = mMsgBuffer.size();
    }

    void NetMSG::add(const uint8_t *msg, std::size_t msgLen)
    {
        mHeader.mMsgSize += msgLen;
        mMsgBuffer.insert(mMsgBuffer.end(), msg, msg + msgLen);
    }

    void NetMSG::addStr(std::string_view msg)
    {
        auto size = msg.size();
        mHeader.mMsgSize += size;
        add<std::size_t>(size);
        mMsgBuffer.insert(mMsgBuffer.end(), msg.data(), msg.data() + msg.size());
    }

    void NetMSG::addVec(const std::vector<std::uint8_t> &msg)
    {
        mMsgBuffer.insert(mMsgBuffer.end(), msg.data(), msg.data() + msg.size());
    }

    template<typename Type>
    void NetMSG::add(Type msg)
    {
        std::size_t size = sizeof(Type);
        mHeader.mMsgSize += size;
        Type value = swapEndianness(msg);
        uint8_t* addValue = std::bit_cast<uint8_t*>(&value);
        mMsgBuffer.reserve(size);
        mMsgBuffer.insert(mMsgBuffer.end(), addValue, addValue + size);
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

    template <typename Type>
    Type NetMSG::get()
    {
        if(mMsgBuffer.empty())
            return Type(0);

        std::size_t size = sizeof(Type);
        Type value = 0;

        auto begin = mMsgBuffer.begin();
        auto end = mMsgBuffer.begin() + size;

        ::memcpy(&value, begin.base(), size);
        mMsgBuffer.erase(begin, end);

        return swapEndianness(value);
    }

    template int8_t NetMSG::get<int8_t>();
    template int16_t NetMSG::get<int16_t>();
    template int32_t NetMSG::get<int32_t>();
    template int64_t NetMSG::get<int64_t>();
    template uint8_t NetMSG::get<uint8_t>();
    template uint16_t NetMSG::get<uint16_t>();
    template uint32_t NetMSG::get<uint32_t>();
    template uint64_t NetMSG::get<uint64_t>();
    template float NetMSG::get<float>();
    template double NetMSG::get<double>();

    std::string NetMSG::getStr()
    {
        std::size_t len = get<std::size_t>();

        std::string str;
        str.reserve(len);
        str.resize(len);

        auto begin = mMsgBuffer.begin();
        auto end = mMsgBuffer.begin() + len;

        std::copy(begin, end, str.begin());
        mMsgBuffer.erase(begin, end);

        return str;
    }

    void NetMSG::setType(NetMsgType msgType)
    {
        mHeader.mMsgType = msgType;
    }

    void NetMSG::setHeader(const NetMsgHeader &header)
    {
        mHeader = header;
    }

    void NetMSG::setOrder(const NetMsgOrder &order)
    {
        mOrder = order;
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

    NetMsgHeader& NetMSG::getHeader()
    {
        return mHeader;
    }

    const NetMsgHeader& NetMSG::getConstHeader() const
    {
        return mHeader;
    }

    NetMsgOrder &NetMSG::getMsgOrder()
    {
        return mOrder;
    }

    const NetMsgOrder &NetMSG::getConstMsgOrder() const
    {
        return mOrder;
    }

    std::size_t NetMSG::getMsgSize() const
    {
        return mHeader.mMsgSize;
    }

    std::size_t NetMSG::getOrderCount() const
    {
        return mHeader.mMsgOrderCount;
    }

    NetMsgType NetMSG::getMsgType() const
    {
        return mHeader.mMsgType;
    }

    std::vector<NetPackage> NetMSG::toPackages() const
    {
        std::vector<NetPackage> packages;
        std::string tmpBuf;
        tmpBuf.reserve(mMsgBuffer.size());
        tmpBuf.resize(mMsgBuffer.size());

        auto begin = mMsgBuffer.begin();
        auto end = mMsgBuffer.end();
        std::copy(begin, end, tmpBuf.begin());

        while(!tmpBuf.empty())
        {
            packages.emplace_back(tmpBuf.substr(0, MAX_MSG_LEN));
            tmpBuf.erase(0, MAX_MSG_LEN);
        }

        return packages;
    }

    NetMSG::operator bool() const
    {
        return mHeader.mMsgType != NetMsgType::NONE;
    }

    void NetMSG::AddOrder(NetMsgType type, const NetMsgOrder &order)
    {
        for(auto const& [key, value] : mOrders)
            if(key == type)
                return;

        mOrders.try_emplace(type, order);
    }

    void NetMSG::AddOrder(NetMsgType type, const std::initializer_list<NetMsgOrderType> &list)
    {
        for(auto const& [key, value] : mOrders)
            if(key == type)
                return;

        mOrders.try_emplace(type, list);
    }

    NetMsgOrder NetMSG::GetOrder(NetMsgType type)
    {
        for(auto const& [key, value] : mOrders)
            if(key == type)
                return value;

        return NetMsgOrder();
    }
}