#ifndef IF_UTIL_H
#define IF_UTIL_H

#include <stdint.h>

#include "../vm.h"

/**
 * Updates `fbuf` with the new fbuf from the `IF` stage if its STAY signal is
 * not high.
 *
 * @param fbuf pointer to the new fbuf from the `IF` stage
 */
void update_fbuf(fbuf_t *fbuf);

#endif
