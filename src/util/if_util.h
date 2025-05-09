#ifndef IF_UTIL_H
#define IF_UTIL_H

#include <stdint.h>

#include "../vm.h"

/**
 * Returns the next instruction and increments the PC.
 */
int16_t get_instruction_and_increment_pc(void);

/**
 * Saves PC* in `fbuf`.
 *
 * @param fbuf buffer to save PC* in
 */
void save_pc(fbuf_t *fbuf);

/**
 * Updates `fbuf` with the new fbuf from the `IF` stage if its STAY signal is
 * not high.
 *
 * @param fbuf pointer to the new fbuf from the `IF` stage
 */
void update_fbuf(fbuf_t *fbuf);

#endif
