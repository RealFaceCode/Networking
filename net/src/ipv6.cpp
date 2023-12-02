#include "ipv6.h"

#ifdef _WIN32
#include <format>
#else
#include <sstream>
#endif
#include <algorithm>

namespace NetSock
{
    IPv6::IPv6() = default;
    
    IPv6::IPv6(std::string_view ip)
    : mIP(ip)
    {}
    
    IPv6::IPv6(uint16_t sec1, uint16_t sec2, uint16_t sec3, uint16_t sec4, uint16_t sec5, uint16_t sec6, uint16_t sec7, uint16_t sec8)
    {
    #ifdef _WIN32
        mIP = std::format("{:X}:{:X}:{:X}:{:X}:{:X}:{:X}:{:X}:{:X}",
        sec1, sec2, sec3, sec4, sec5, sec6, sec7, sec8);
    #else
        std::stringstream stream;
        stream << std::hex << sec1 << ":"
        << std::hex << sec2 << ":"
        << std::hex << sec3 << ":"
        << std::hex << sec4 << ":"
        << std::hex << sec5 << ":"
        << std::hex << sec6 << ":"
        << std::hex << sec7 << ":"
        << std::hex << sec8;
        mIP = stream.str();
    #endif
    }
    
    IPv6::IPv6(std::string_view ip, bool force)
    : mIP(ip), mForced(force)
    {}
    
    IPv6::~IPv6() = default;
    
    std::string_view IPv6::ip() const
    {
        return mIP;
    }
    
    bool IPv6::operator==(const IPv6 &ip) const = default;
    
    IPv6::operator bool() const
    {
        return std::count(mIP.begin(), mIP.end(), ':') == 7 || mForced;
    }
}