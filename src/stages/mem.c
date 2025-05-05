#include "mem.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "../isa.h"
#include "../vm.h"

void *mem_run(void *arg) {
    (void)arg;

    pthread_mutex_lock(&vm.ebuf_mutex);
    ebuf_t ebuf = vm.ebuf;
    vm.ebuf.read = 1;
    pthread_cond_signal(&vm.ebuf_read_cond);
    pthread_mutex_unlock(&vm.ebuf_mutex);

    if (!ebuf.ready) {
        return NULL;
    }

    ebuf.read = 1;

    mbuf_t mbuf;
    mbuf.ready = 1;
    mbuf.read = 0;
    mbuf.pc = ebuf.pc;
    mbuf.opcode = ebuf.opcode;
    mbuf.result = ebuf.result;
    mbuf.reg = ebuf.reg;

    if (ebuf.opcode == OP_LD || ebuf.opcode == OP_LDR) {
        pthread_mutex_lock(&vm.mem_mutex);
        mbuf.result = vm.mem[(uint16_t)ebuf.result];
        pthread_mutex_unlock(&vm.mem_mutex);
    } else if (ebuf.opcode == OP_ST || ebuf.opcode == OP_STR) {
        pthread_mutex_lock(&vm.mem_mutex);
        vm.mem[(uint16_t)ebuf.result] = ebuf.reg;
        pthread_mutex_unlock(&vm.mem_mutex);
    } else if (ebuf.opcode == OP_LDI || ebuf.opcode == OP_STI) {
        // TODO
    }

    pthread_mutex_lock(&vm.mbuf_mutex);
    while (!vm.mbuf.read) {
        pthread_cond_wait(&vm.mbuf_read_cond, &vm.mbuf_mutex);
    }
    vm.mbuf = mbuf;
    pthread_mutex_unlock(&vm.mbuf_mutex);

    return NULL;
}
