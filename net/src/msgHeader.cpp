#include "msgHeader.h"
#include <array>

std::vector<uint8_t> NetMsgHeader::toVec() const
{
    std::array<uint8_t, sizeof(std::size_t)> len = std::bit_cast<std::array<uint8_t, sizeof(std::size_t)>>(mMsgSize);
    std::array<uint8_t, sizeof(unsigned int)> type = std::bit_cast<std::array<uint8_t, sizeof(unsigned int)>>(mMsgType);
    std::array<uint8_t, sizeof(std::size_t)> count = std::bit_cast<std::array<uint8_t, sizeof(std::size_t)>>(mMsgOrderCount);

    std::vector<uint8_t> out;
    out.insert(out.end(), len.begin(), len.end());
    out.insert(out.end(), count.begin(), count.end());
    out.insert(out.end(), type.begin(), type.end());

    return out;
}
