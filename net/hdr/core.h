#ifndef CORE_H
#define CORE_H

#ifdef _WIN32
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using NET_SOCKET = SOCKET;

#elif __unix__
using NET_SOCKET = int;
#error NOT SUPPORTED YET!
#else
#error NOT SUPPORTED YET!
#endif

constexpr unsigned int MAX_MSG_LEN = 512;

bool IsBigEndian();
bool IsLittleEndian();

template<typename Type> Type ReverseByteOrder(Type type);

#endif // CORE_H