#include "if.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include "../vm.h"

void *if_run(void *arg) {
    (void)arg;

    fbuf_t fbuf;
    fbuf.nop = 0;

    pthread_mutex_lock(&vm.pc_mutex);
    pthread_mutex_lock(&vm.mem_mutex);
    fbuf.ir = vm.mem[vm.pc];
    pthread_mutex_unlock(&vm.mem_mutex);
    vm.pc++;
    fbuf.pc = vm.pc;
    pthread_mutex_unlock(&vm.pc_mutex);

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);

    pthread_mutex_lock(&vm.fbuf_nop_mutex);
    fbuf.nop = vm.fbuf_nop;
    vm.fbuf_nop = 0;
    pthread_mutex_unlock(&vm.fbuf_nop_mutex);

    pthread_mutex_lock(&vm.fbuf_stay_mutex);
    if (!vm.fbuf_stay) {
        pthread_mutex_lock(&vm.fbuf_mutex);
        vm.fbuf = fbuf;
        pthread_mutex_unlock(&vm.fbuf_mutex);
    } else {
        vm.fbuf_stay = 0;

        pthread_mutex_lock(&vm.pc_mutex);
        vm.pc--;
        pthread_mutex_unlock(&vm.pc_mutex);
    }

    pthread_mutex_unlock(&vm.fbuf_stay_mutex);

    return NULL;
}
