#ifndef WB_H
#define WB_H

/**
 * Top-level function that executes a clock cycle in the WB stage of the
 * pipelined processor. Runs within a thread.
 *
 * @param arg unused
 * @return NULL
 */
void *wb_run(void *arg);

#endif
