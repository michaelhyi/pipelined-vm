#ifndef IF_H
#define IF_H

/**
 * Top-level function that executes a clock cycle in the IF stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg an output pointer to an fbuf
 * @return void pointer
 */
void *if_run(void *arg);

#endif
