#ifndef IPV4_H
#define IPV4_H

#include <string>
#include <string_view>
#include <cstdint>

namespace NetSock
{
    class IPv4
    {
    private:
        std::string mIP;
        bool mForced = false;

    public:
        IPv4();
        explicit IPv4(std::string_view ip);
        IPv4(uint8_t sec1, uint8_t sec2, uint8_t sec3, uint8_t sec4);
        IPv4(std::string_view ip, bool force);
        ~IPv4();

        std::string_view ip() const;

        bool operator==(const IPv4& ip) const;
        explicit operator bool() const;
    };
}

#endif // IPV4_H