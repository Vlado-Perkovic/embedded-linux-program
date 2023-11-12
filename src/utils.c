#include <utils.h>

bool utils_is_system_little_endian()
{
    uint8_t num = 1;
    return (*(char *)&num) == 1; // Least significant byte will be 1 for little-endian
}

int32_t utils_to_big_endian(int32_t value)
{
    if (utils_is_system_little_endian())
    {
        return bswap_32(value);
    }
    else
    {
        return value; // No conversion needed for big-endian systems
    }
}