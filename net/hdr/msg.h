#ifndef MSG_H
#define MSG_H

#include <vector>
#include <cstdint>
#include <string_view>
#include <string>
#include <unordered_map>

#include "msgHeader.h"

class NetMSG
{
private:
    MsgHeader mHeader = {};
    std::vector<std::uint8_t> mMsgBuffer;
    static std::unordered_map<MsgType, MsgOrder> mOrders;

public:
    NetMSG();
    explicit NetMSG(MsgType msgType);
    ~NetMSG();

    void add(const char* msg, std::size_t msgLen);
    void add(const std::uint8_t* msg, std::size_t msgLen);
    void add(std::string_view msg);
    void add(const std::vector<std::uint8_t>& msg);

    template<typename Type> void add(Type msg);

    void setType(MsgType msgType);

    std::string getAsString() const;
    std::vector<std::uint8_t> getAsVec() const;

    static void AddOrder(MsgType type, const MsgOrder& order);
    static void AddOrder(MsgType type, const std::initializer_list<MsgOrderType>& list);
    static MsgOrder GetOrder(MsgType type);
};

#endif // MSG_H