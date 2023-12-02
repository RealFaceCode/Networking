#include "package.h"

namespace NetSock
{
    NetPackage::NetPackage() = default;

    NetPackage::NetPackage(std::string_view data)
    {
        if(data.size() > MAX_MSG_LEN)
            return;

        mSize = data.size();
        std::ranges::copy(data.begin(), data.end(), mData.begin());
    }

    NetPackage::~NetPackage() = default;

    const char *NetPackage::data() const
    {
        return mData.data();
    }

    std::size_t NetPackage::size() const
    {
        return mSize;
    }
}