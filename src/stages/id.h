#ifndef ID_H
#define ID_H

/**
 * Top-level function that executes a clock cycle in the ID stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg an output pointer to a dbuf
 * @return void pointer
 */
void *id_run(void *arg);

#endif
