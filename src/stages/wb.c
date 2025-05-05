#include "wb.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../isa.h"
#include "../util/wb_util.h"
#include "../vm.h"

void *wb_run(void *arg) {
    (void)arg;

    pthread_mutex_lock(&vm.mbuf_mutex);
    mbuf_t mbuf = vm.mbuf;
    pthread_mutex_unlock(&vm.mbuf_mutex);

    if (!mbuf.ready) {
        pthread_barrier_wait(&vm.pipeline_cycle_barrier);
        return NULL;
    }

    if (mbuf.opcode == OP_ADD || mbuf.opcode == OP_LD ||
        mbuf.opcode == OP_AND || mbuf.opcode == OP_LDR ||
        mbuf.opcode == OP_NOT || mbuf.opcode == OP_LDI ||
        mbuf.opcode == OP_LEA) {
        pthread_mutex_lock(&vm.reg_mutex);
        vm.reg[mbuf.reg] = mbuf.result;
        pthread_mutex_unlock(&vm.reg_mutex);

        set_cc(mbuf);
    }

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);

    return NULL;
}
