#ifndef MEM_H
#define MEM_H

/**
 * Top-level function that executes a clock cycle in the MEM stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg unused
 * @return NULL
 */
void *mem_run(void *arg);

#endif
