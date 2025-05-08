#include "wb.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../isa.h"
#include "../util/wb_util.h"
#include "../vm.h"
#include "../vm_util.h"

void *wb_run(void *arg) {
    (void)arg;

    pthread_mutex_lock(&vm.mbuf_mutex);
    mbuf_t mbuf = vm.mbuf;
    pthread_mutex_unlock(&vm.mbuf_mutex);

    if (mbuf.nop) {
        pthread_barrier_wait(&vm.pipeline_cycle_barrier);
        return NULL;
    }

    if (mbuf.opcode == OP_ADD || mbuf.opcode == OP_LD ||
        mbuf.opcode == OP_AND || mbuf.opcode == OP_LDR ||
        mbuf.opcode == OP_NOT || mbuf.opcode == OP_LDI ||
        mbuf.opcode == OP_LEA) {
        set_register((uint16_t)mbuf.reg, mbuf.result);
        set_cc(mbuf);
    } else if (mbuf.opcode == OP_JSR || mbuf.opcode == OP_JSRR) {
        set_register(7, (int16_t)mbuf.pc);
    } else if (mbuf.opcode == OP_TRAP) {
        // TODO: for now, it is assumed that all traps passed in are HALT
        // statements. this will, obviously, change in the near future. this is
        // just for testing purposes
        pthread_mutex_lock(&vm.running_mutex);
        vm.running = 0;
        pthread_mutex_unlock(&vm.running_mutex);
    }

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);
    return NULL;
}
