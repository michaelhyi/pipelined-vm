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
    pthread_mutex_unlock(&vm.ebuf_mutex);

    mbuf_t mbuf;
    mbuf.nop = ebuf.nop;

    if (ebuf.nop) {
        pthread_barrier_wait(&vm.pipeline_cycle_barrier);
        pthread_mutex_lock(&vm.mbuf_mutex);
        vm.mbuf = mbuf;
        pthread_mutex_unlock(&vm.mbuf_mutex);

        return NULL;
    }

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
    } else if (ebuf.opcode == OP_LDI) {
        pthread_mutex_lock(&vm.mem_mutex);
        mbuf.result = vm.mem[(uint16_t)ebuf.result];
        pthread_mutex_unlock(&vm.mem_mutex);

        pthread_mutex_lock(&vm.ebuf_mutex);
        vm.ebuf.indirect_counter =
            (uint16_t)((vm.ebuf.indirect_counter + 1) % 2);

        if (vm.ebuf.indirect_counter) {
            vm.ebuf.result = mbuf.result;

            pthread_mutex_lock(&vm.fbuf_stay_mutex);
            vm.fbuf_stay = 1;
            pthread_mutex_unlock(&vm.fbuf_stay_mutex);

            pthread_mutex_lock(&vm.dbuf_stay_mutex);
            vm.dbuf_stay = 1;
            pthread_mutex_unlock(&vm.dbuf_stay_mutex);

            pthread_mutex_lock(&vm.ebuf_stay_mutex);
            vm.ebuf_stay = 1;
            pthread_mutex_unlock(&vm.ebuf_stay_mutex);

            pthread_mutex_lock(&vm.mbuf_stay_mutex);
            vm.mbuf_stay = 1;
            pthread_mutex_unlock(&vm.mbuf_stay_mutex);
        }
        pthread_mutex_unlock(&vm.ebuf_mutex);
    } else if (ebuf.opcode == OP_STI) {
        pthread_mutex_lock(&vm.mem_mutex);
        mbuf.result = vm.mem[(uint16_t)ebuf.result];
        pthread_mutex_unlock(&vm.mem_mutex);

        pthread_mutex_lock(&vm.ebuf_mutex);
        vm.ebuf.indirect_counter =
            (uint16_t)((vm.ebuf.indirect_counter + 1) % 2);

        if (vm.ebuf.indirect_counter) {
            vm.ebuf.result = mbuf.result;

            pthread_mutex_lock(&vm.fbuf_stay_mutex);
            vm.fbuf_stay = 1;
            pthread_mutex_unlock(&vm.fbuf_stay_mutex);

            pthread_mutex_lock(&vm.dbuf_stay_mutex);
            vm.dbuf_stay = 1;
            pthread_mutex_unlock(&vm.dbuf_stay_mutex);

            pthread_mutex_lock(&vm.ebuf_stay_mutex);
            vm.ebuf_stay = 1;
            pthread_mutex_unlock(&vm.ebuf_stay_mutex);

            pthread_mutex_lock(&vm.mbuf_stay_mutex);
            vm.mbuf_stay = 1;
            pthread_mutex_unlock(&vm.mbuf_stay_mutex);
        } else {
            pthread_mutex_lock(&vm.mem_mutex);
            vm.mem[(uint16_t)ebuf.result] = ebuf.reg;
            pthread_mutex_unlock(&vm.mem_mutex);
        }
        pthread_mutex_unlock(&vm.ebuf_mutex);
    }

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);

    pthread_mutex_lock(&vm.mbuf_stay_mutex);
    if (!vm.mbuf_stay) {
        pthread_mutex_lock(&vm.mbuf_mutex);
        vm.mbuf = mbuf;
        pthread_mutex_unlock(&vm.mbuf_mutex);
    }

    vm.mbuf_stay = 0;
    pthread_mutex_unlock(&vm.mbuf_stay_mutex);

    return NULL;
}
