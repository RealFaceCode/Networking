#include "port.h"

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

bool Port::operator==(const Port &ip) const = default;

Port::operator bool() const
{
    return !mPort.empty();
}