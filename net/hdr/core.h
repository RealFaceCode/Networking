#ifndef CORE_H
#define CORE_H
    #include <vector>
    #include <cstdint>
    #include <memory>
    #include <bit>
    #include <cstdint>
    #include <type_traits>

namespace NetSock
{
    constexpr unsigned int MAX_MSG_LEN = 512;

    bool IsBigEndian();
    bool IsLittleEndian();

    template<typename Type> Type ReverseByteOrder(Type type);

    template<typename extrType, class arrType = std::vector<uint8_t>>
    extrType ExtractBytesToType(const arrType& arr, std::size_t posFirstByte)
    {
        extrType rVal = extrType();
        ::memcpy(&rVal, &arr[0 + posFirstByte], sizeof(extrType));
        return rVal;
    }

    template <typename T>
    constexpr T swapEndianness(T value) {
        if constexpr (std::endian::native == std::endian::little) {
            if constexpr (std::is_integral_v<T>) {
                if constexpr (sizeof(T) == 1)
                    return value;
                else if constexpr (sizeof(T) == 2)
                    return static_cast<T>((value >> 8) | (value << 8));
                else if constexpr (sizeof(T) == 4)
                {
                    return static_cast<T>((value >> 24) |
                                        ((value << 8) & 0x00FF0000) |
                                        ((value >> 8) & 0x0000FF00) |
                                        (value << 24));
                }
                else if constexpr (sizeof(T) == 8)
                {
                    return static_cast<T>((value >> 56) |
                                        ((value << 40) & 0x00FF'0000'0000'0000ULL) |
                                        ((value << 24) & 0x0000'FF00'0000'0000ULL) |
                                        ((value << 8)  & 0x0000'00FF'0000'0000ULL) |
                                        ((value >> 8)  & 0x0000'0000'FF00'0000ULL) |
                                        ((value >> 24) & 0x0000'0000'00FF'0000ULL) |
                                        ((value >> 40) & 0x0000'0000'0000'FF00ULL) |
                                        (value << 56));
                }
                else
                    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "Unsupported size for endianness swap");
            } 
            else if constexpr (std::is_floating_point_v<T>)
            {
                union
                {
                    T value;
                    std::uint64_t intValue;
                } u;

                u.value = value;
                u.intValue = swapEndianness(u.intValue);
                return u.value;
            }
            else
                static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "Unsupported type for endianness swap");
        } 
        else
            return value;
    }

    bool Init();
}

#endif // CORE_H