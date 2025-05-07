#include "if.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include "../util/if_util.h"

void *if_run(void *arg) {
    (void)arg;

    fbuf_t fbuf;
    fbuf.ir = get_instruction_and_increment_pc();
    save_pc(&fbuf);

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);

    // receive any bubbles
    pthread_mutex_lock(&vm.fbuf_nop_mutex);
    fbuf.nop = vm.fbuf_nop;
    vm.fbuf_nop = 0;
    pthread_mutex_unlock(&vm.fbuf_nop_mutex);

    update_fbuf(&fbuf);
    return NULL;
}
