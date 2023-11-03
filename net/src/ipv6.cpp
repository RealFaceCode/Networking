#include "ipv6.h"


#include <format>
#include <algorithm>

IPv6::IPv6() = default;

IPv6::IPv6(std::string_view ip)
: mIP(ip)
{}

IPv6::IPv6(uint16_t sec1, uint16_t sec2, uint16_t sec3, uint16_t sec4, uint16_t sec5, uint16_t sec6, uint16_t sec7, uint16_t sec8)
{
    mIP = std::format("{:X}:{:X}:{:X}:{:X}:{:X}:{:X}:{:X}:{:X}",
    sec1, sec2, sec3, sec4, sec5, sec6, sec7, sec8);
}

IPv6::~IPv6() = default;

std::string_view IPv6::ip() const
{
    return mIP;
}

bool IPv6::operator==(const IPv6 &ip) const = default;

IPv6::operator bool() const
{
    return std::count(mIP.begin(), mIP.end(), ':') == 7;
}