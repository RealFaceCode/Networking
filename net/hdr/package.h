#ifndef NET_PACKET_H
#define NET_PACKET_H

#include <array>
#include "core.h"

namespace NetSock
{
    class NetPackage
    {
    private:
        std::array<char, MAX_MSG_LEN> mData;
        std::size_t mSize;

    public:
        NetPackage();
        explicit NetPackage(std::string_view data);
        ~NetPackage();

        const char* data() const;
        std::size_t size() const;
    };
}

#endif // NET_PACKET_H