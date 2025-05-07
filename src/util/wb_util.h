#ifndef WB_UTIL_H
#define WB_UTIL_H

#include "../vm.h"

/**
 * Writes `mbuf.result` to the register specified by `mbuf.reg`.
 */
void write_to_register(mbuf_t mbuf);

/**
 * Sets the condition code register based on `mbuf`.
 *
 * @param mbuf mbuf currently in use by the `WB` stage
 */
void set_cc(mbuf_t mbuf);

/**
 * Saves PC* in R7.
 */
void save_pc_in_r7(mbuf_t mbuf);

#endif
