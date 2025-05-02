#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

/**
 * Returns the bits within the specified range of a number.
 *
 * @param n number to mask
 * @param lower inclusive lower bound of range
 * @param upper inclusive upper bound of range
 * @return n[upper:lower]
 */
int16_t bit_range(int16_t n, uint16_t lower, uint16_t upper);

/**
 * Sign extends a 2's complement number.
 *
 * @param n number to sign extend
 * @param bit_length length of n in bits
 * @return sign extended value of n
 */
int16_t sign_extend(int16_t n, uint16_t bit_length);

#endif
