#ifndef MEM_UTIL_H
#define MEM_UTIL_H

#include "../vm.h"

/**
 * Returns an initialized `mbuf` based on `ebuf`.
 *
 * @param ebuf the VM's current `ebuf`
 * @returns a pointer to a new dynamically allocated mbuf
 */
mbuf_t *init_mbuf(ebuf_t ebuf);

/**
 * Stalls all 5 pipline stages. Used for `LDI` and `STI` instructions.
 */
void stall_pipeline(void);

/**
 * Updates `mbuf` with the new mbuf, if a STAY signal is not high.
 *
 * @param mbuf pointer to the new mbuf from the `MEM` stage
 */
void update_mbuf(mbuf_t *mbuf);

#endif
