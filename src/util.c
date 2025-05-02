#include "util.h"

#include <errno.h>
#include <stdio.h>

int16_t bit_range(int16_t n, uint16_t lower, uint16_t upper) {
    if (lower > upper || upper >= 16) {
        fprintf(stderr,
                "bit_range: lower must be <= upper, and upper must be < 16\n");
        errno = EINVAL;
        return 0;
    }

    uint16_t range = (uint16_t)(upper - lower + 1);
    uint16_t bit_mask = (uint16_t)((~(((uint16_t)~0) << range)) << lower);

    return (int16_t)((n & bit_mask) >> lower);
}

int16_t sign_extend(int16_t n, uint16_t bit_length) {
    if (bit_length >= 16) {
        fprintf(stderr, "sign_extend: bit_length must be < 16\n");
        errno = EINVAL;
        return 0;
    }

    uint16_t msb = (n >> (bit_length - 1)) & 1;

    // positive
    if (!msb) {
        return n;
    }

    // negative
    return n | (int16_t)(((uint16_t)~0) << bit_length);
}
