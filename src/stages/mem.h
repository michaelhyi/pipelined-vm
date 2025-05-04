#ifndef MEM_H
#define MEM_H

/**
 * Top-level function that executes a clock cycle in the MEM stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg an output pointer to a mbuf
 * @return void pointer
 */
void *mem_run(void *arg);

#endif
