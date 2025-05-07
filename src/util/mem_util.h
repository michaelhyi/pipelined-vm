#ifndef MEM_UTIL_H
#define MEM_UTIL_H

#include "../vm.h"

/**
 * Returns the VM's current `ebuf` in a thread-safe manner.
 */
ebuf_t get_ebuf(void);

/**
 * Sends a NOP bubble to the `WB` stage.
 */
void send_bubble_to_wb(void);

/**
 * Returns an initialized `mbuf` based on `ebuf`.
 *
 * @param ebuf the VM's current `ebuf`
 * @returns a pointer to a new dynamically allocated mbuf
 */
mbuf_t *init_mbuf(ebuf_t ebuf);

/**
 * Fetches data from the VM's memory in a thread-safe manner.
 *
 * @param addr memory address to fetch from
 * @returns the value stored at that memory address
 */
int16_t mem_get(uint16_t addr);

/**
 * Sets data in the VM's memory in a thread-safe manner.
 *
 * @param addr memory address to set
 * @param data data to set in `mem[addr]`
 */
void mem_set(uint16_t addr, int16_t data);

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
