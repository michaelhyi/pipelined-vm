#ifndef EX_H
#define EX_H

#include <stdint.h>

typedef struct ebuf_t {
    int16_t ready;

    int16_t pc;
    int16_t opcode;
    int16_t result;
    int16_t reg;
} ebuf_t;

/**
 * Top-level function that executes a clock cycle in the EX stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg an output pointer to an ebuf
 * @return void pointer
 */
void *ex_run(void *arg);

#endif
