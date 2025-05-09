#ifndef IF_H
#define IF_H

/**
 * Top-level function that executes a clock cycle in the IF stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg unused
 * @returns NULL
 */
void *if_run(void *arg);

#endif
