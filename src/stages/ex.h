#ifndef EX_H
#define EX_H

/**
 * Top-level function that executes a clock cycle in the EX stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg unused
 * @return NULL
 */
void *ex_run(void *arg);

#endif
