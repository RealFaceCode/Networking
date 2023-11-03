#ifndef PORT_H
#define PORT_H

#include <string>
#include <string_view>
#include <cstdint>

class Port
{
private:
    std::string mPort;

public:
    Port();
    explicit Port(std::string_view port);
    explicit Port(uint16_t port);
    ~Port();

    std::string_view port() const;

    bool operator==(const Port& port) const;
    explicit operator bool() const;
};

#endif // PORT_H