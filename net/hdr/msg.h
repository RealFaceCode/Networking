#ifndef MSG_H
#define MSG_H

#include <vector>
#include <cstdint>
#include <string_view>
#include <string>
#include <unordered_map>

#include "msgHeader.h"
#include "msgOrder.h"
#include "package.h"

class NetMSG
{
private:
    NetMsgHeader mHeader = {};
    NetMsgOrder mOrder = {};

    std::vector<std::uint8_t> mMsgBuffer;
    static std::unordered_map<NetMsgType, NetMsgOrder> mOrders;

public:
    NetMSG();
    explicit NetMSG(NetMsgType msgType);
    ~NetMSG();

    void add(const char* msg, std::size_t msgLen);
    void add(const uint8_t* msg, std::size_t msgLen);
    void addStr(std::string_view msg);
    void addVec(const std::vector<std::uint8_t>& msg);

    template<typename Type> void add(Type msg);
    template<typename Type> Type get();

    std::string getStr();

    void setType(NetMsgType msgType);
    void setHeader(const NetMsgHeader& header);
    void setOrder(const NetMsgOrder& order);

    std::string getAsString() const;
    std::vector<std::uint8_t> getAsVec() const;

    NetMsgHeader& getHeader();
    const NetMsgHeader& getConstHeader() const;
    NetMsgOrder& getMsgOrder();
    const NetMsgOrder& getConstMsgOrder() const;

    std::size_t getMsgSize() const;
    std::size_t getOrderCount() const;
    NetMsgType getMsgType() const;

    std::vector<NetPackage> toPackages() const;

    explicit operator bool() const;

    static void AddOrder(NetMsgType type, const NetMsgOrder& order);
    static void AddOrder(NetMsgType type, const std::initializer_list<NetMsgOrderType>& list);
    static NetMsgOrder GetOrder(NetMsgType type);
};

#endif // MSG_H