#include "core.h"
#include <bit>
#include <algorithm>
#include <cstdint>

#ifdef _WIN32
#include <winsock2.h>
#elif __unix__
#else
#error NOT SUPPORTED YET!
#endif

namespace NetSock
{
    bool IsBigEndian()
    {
        return (std::endian::native == std::endian::big);
    }

    bool IsLittleEndian()
    {
        return (std::endian::native == std::endian::little);
    }

    template <typename Type>
    inline Type ReverseByteOrder(Type type)
    {
        const std::size_t size = sizeof(Type);
        Type tmp = type;

        uint8_t* ptr = std::bit_cast<uint8_t*>(&tmp);

        std::reverse(ptr, ptr + size);
        return tmp;
    }

    template int8_t ReverseByteOrder<int8_t>(int8_t type);
    template int16_t ReverseByteOrder<int16_t>(int16_t type);
    template int32_t ReverseByteOrder<int32_t>(int32_t type);
    template int64_t ReverseByteOrder<int64_t>(int64_t type);
    template uint8_t ReverseByteOrder<uint8_t>(uint8_t type);
    template uint16_t ReverseByteOrder<uint16_t>(uint16_t type);
    template uint32_t ReverseByteOrder<uint32_t>(uint32_t type);
    template uint64_t ReverseByteOrder<uint64_t>(uint64_t type);
    template float ReverseByteOrder<float>(float type);
    template double ReverseByteOrder<double>(double type);

    bool Init()
    {
    #ifdef _WIN32
        if (WSADATA wsaData; ::WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
            return false;
    #elif __unix__
    #error NOT SUPPORTED YET!
    #else
    #error NOT SUPPORTED YET!
    #endif

        return true;
    }
}