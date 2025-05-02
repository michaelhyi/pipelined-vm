#include "util.h"

int32_t sign_extend(int32_t n, uint32_t bit_length) {
    // negative
    if ((n >> (bit_length - 1)) & 1) {
        return n | (int32_t)(((uint32_t)~0) << bit_length);
    }

    return n & ((1 << bit_length) - 1);
}
