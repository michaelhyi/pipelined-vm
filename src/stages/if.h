#ifndef IF_H
#define IF_H

#include <stdint.h>

typedef struct fbuf_t {
    int16_t ready;

    int16_t pc;
    int16_t ir;
} fbuf_t;

/**
 * Top-level function that executes a clock cycle in the IF stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg an output pointer to an fbuf
 * @return void pointer
 */
void *if_run(void *arg);

#endif
