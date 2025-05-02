#ifndef MEM_H
#define MEM_H

#include <stdint.h>

typedef struct mbuf_t {
    int16_t ready;

    int16_t pc;
    int16_t opcode;
    int16_t result;
    int16_t reg;
} mbuf_t;

/**
 * Top-level function that executes a clock cycle in the MEM stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg an output pointer to a mbuf
 * @return void pointer
 */
void *mem_run(void *arg);

#endif
