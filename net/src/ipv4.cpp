#include "ipv4.h"

#ifdef _WIN32
#include <format>
#else
#include <sstream>
#endif
#include <algorithm>

namespace NetSock
{
    IPv4::IPv4() = default;

    IPv4::IPv4(std::string_view ip)
    : mIP(ip)
    {}

    IPv4::IPv4(uint8_t sec1, uint8_t sec2, uint8_t sec3, uint8_t sec4)
    {
    #ifdef _WIN32
        mIP = std::format("{}.{}.{}.{}",
        sec1, sec2, sec3, sec4);
    #else
        mIP.append(std::to_string(sec1)).append(".")
        .append(std::to_string(sec2)).append(".")
        .append(std::to_string(sec3)).append(".")
        .append(std::to_string(sec4));
    #endif
    }

    IPv4::IPv4(std::string_view ip, bool force)
    : mIP(ip), mForced(force)
    {}

    IPv4::~IPv4() = default;

    std::string_view IPv4::ip() const
    {
        return mIP;
    }

    bool IPv4::operator==(const IPv4 &ip) const = default;

    IPv4::operator bool() const
    {
        return std::count(mIP.begin(), mIP.end(), '.') == 3 || mForced;
    }
}