#include "port.h"
#include <limits>

Port::Port() = default;

Port::Port(std::string_view port)
: mPort(port)
{}

Port::Port(uint16_t port)
: mPort(std::to_string(port))
{}

Port::~Port() = default;

std::string_view Port::port() const
{
    return mPort;
}

uint16_t Port::toInteger() const
{
    if (unsigned long num = std::stoul(mPort); num <= std::numeric_limits<unsigned short>::max())
        return static_cast<unsigned short>(num);
    
    return 0;
}

bool Port::operator==(const Port &ip) const = default;

Port::operator bool() const
{
    return !mPort.empty();
}