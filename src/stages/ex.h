#ifndef EX_H
#define EX_H

/**
 * Top-level function that executes a clock cycle in the EX stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg an output pointer to an ebuf
 * @return void pointer
 */
void *ex_run(void *arg);

#endif
