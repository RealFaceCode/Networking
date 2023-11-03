#ifndef IPV6_H
#define IPV6_H

#include <string>
#include <string_view>
#include <cstdint>

class IPv6
{
private:
    std::string mIP;
    
public:
    IPv6();
    explicit IPv6(std::string_view ip);
    IPv6(uint16_t sec1, uint16_t sec2, uint16_t sec3, uint16_t sec4, uint16_t sec5, uint16_t sec6, uint16_t sec7, uint16_t sec8);
    ~IPv6();

    std::string_view ip() const;

    bool operator==(const IPv6& ip) const;
    explicit operator bool() const;
};

#endif // IPV6_H