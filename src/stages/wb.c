#include "wb.h"

#include <errno.h>
#include <string.h>

#include "../isa.h"
#include "../util/vm_util.h"
#include "../util/wb_util.h"
#include "../vm.h"

/**
 * Tears down the `wb` stage.
 */
static void wb_teardown(void);

void *wb_run(void *arg) {
    (void)arg;

    mbuf_t mbuf = get_mbuf();

    if (mbuf.nop) {
        wb_teardown();
        return NULL;
    }

    if (mbuf.opcode == OP_ADD || mbuf.opcode == OP_LD ||
        mbuf.opcode == OP_AND || mbuf.opcode == OP_LDR ||
        mbuf.opcode == OP_NOT || mbuf.opcode == OP_LDI ||
        mbuf.opcode == OP_LEA) {
        set_register_data((uint16_t)mbuf.reg, mbuf.result);
        set_cc(mbuf);
    } else if (mbuf.opcode == OP_JSR || mbuf.opcode == OP_JSRR) {
        set_register_data(7, (int16_t)mbuf.pc);
    } else if (mbuf.opcode == OP_TRAP) {
        // TODO: for now, it is assumed that all traps passed in are HALT
        // statements. this will, obviously, change in the near future. this is
        // just for testing purposes
        pthread_mutex_lock(&vm.running_mutex);
        vm.running = 0;
        pthread_mutex_unlock(&vm.running_mutex);
    }

    if (errno) {
        wb_teardown();
    }

    wb_teardown();
    return NULL;
}

static void wb_teardown(void) {
    pthread_barrier_wait(&vm.pipeline_cycle_barrier);

    if (errno) {
        int errno_cpy = errno;
        errno = 0;
        pthread_exit((void *)(intptr_t)errno_cpy);
    }

    pthread_exit((void *)(intptr_t)0);
}
