#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <byteswap.h>

/**
 * @brief function that checks if the system is little endian
 *
 * @return true if system is little endian
 * @return false if system is big endian
 */
bool utils_is_system_little_endian();

/**
 * @brief function that swaps the order of the bytes if the system is little endian
 *
 * @param value the value to swap the order of the bytes
 * @return int32_t value with the bytes reversed
 */
int32_t utils_to_big_endian(int32_t value);

#endif // _UTILS_H