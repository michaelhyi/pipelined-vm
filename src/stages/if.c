#include "if.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include "../vm.h"

void *if_run(void *arg) {
    (void)arg;

    fbuf_t fbuf;
    fbuf.ready = 1;

    pthread_mutex_lock(&vm.pc_mutex);
    fbuf.pc = vm.pc;
    vm.pc++;
    pthread_mutex_unlock(&vm.pc_mutex);

    pthread_mutex_lock(&vm.mem_mutex);
    fbuf.ir = vm.mem[fbuf.pc];
    pthread_mutex_unlock(&vm.mem_mutex);

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);
    pthread_mutex_lock(&vm.fbuf_mutex);
    vm.fbuf = fbuf;
    pthread_mutex_unlock(&vm.fbuf_mutex);

    return NULL;
}
