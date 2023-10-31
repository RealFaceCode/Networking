#include "core.h"
#include <iostream>
#include <cstdint>
#include "msg.h"
#include "msgOrder.h"

int main()
{
    if(::IsBigEndian())
        std::cout << "Big" << std::endl;
    else
        std::cout << "Little" << std::endl;
    
    uint16_t i = 0x00ff;
    uint16_t ir = ::ReverseByteOrder(i);

    std::cout << "Original Value: " << std::hex << i << std::endl;
    std::cout << "New Value: " << std::hex << ir << std::endl;

    NetMSG msg;

    msg.add(i);

    MsgOrder order({MsgOrderType::F32, MsgOrderType::I32, MsgOrderType::STRING});
    NetMSG::AddOrder(::UserDefType(0), order);
    auto nOrder = NetMSG::GetOrder(::UserDefType(0));
    return 0;
}  