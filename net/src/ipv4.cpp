#include "ipv4.h"

#include <format>
#include <algorithm>

IPv4::IPv4() = default;

IPv4::IPv4(std::string_view ip)
: mIP(ip)
{}

IPv4::IPv4(uint8_t sec1, uint8_t sec2, uint8_t sec3, uint8_t sec4)
{
    mIP = std::format("{}.{}.{}.{}",
    sec1, sec2, sec3, sec4);
}

IPv4::~IPv4() = default;

std::string_view IPv4::ip() const
{
    return mIP;
}

bool IPv4::operator==(const IPv4 &ip) const = default;

IPv4::operator bool() const
{
    return std::count(mIP.begin(), mIP.end(), '.') == 3;
}