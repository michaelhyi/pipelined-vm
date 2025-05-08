#ifndef WB_UTIL_H
#define WB_UTIL_H

#include "../vm.h"

/**
 * Sets the condition code register based on `mbuf`.
 *
 * @param mbuf mbuf currently in use by the `WB` stage
 */
void set_cc(mbuf_t mbuf);

#endif
