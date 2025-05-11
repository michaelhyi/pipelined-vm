#include "if.h"

#include <errno.h>
#include <pthread.h>

#include "../util/if_util.h"
#include "../util/vm_util.h"
#include "../vm.h"

/**
 * Tears down the `if` stage.
 *
 * @param next_fbuf the next fbuf that the vm will use
 */
static void if_teardown(fbuf_t next_fbuf);

void *if_run(void *arg) {
    (void)arg;

    fbuf_t next_fbuf;

    uint16_t pc = get_pc();
    next_fbuf.ir = get_mem(pc);
    next_fbuf.pc = pc + 1;

    set_pc(pc + 1);

    if (errno) {
        if_teardown(next_fbuf);
    }

    if_teardown(next_fbuf);
    return NULL;
}

static void if_teardown(fbuf_t next_fbuf) {
    pthread_barrier_wait(&vm.pipeline_cycle_barrier);

    if (errno) {
        int errno_cpy = errno;
        errno = 0;
        pthread_exit((void *)(intptr_t)errno_cpy);
    }

    // handle bubbles
    pthread_mutex_lock(&vm.id_nop_mutex);
    next_fbuf.nop = vm.id_nop;
    vm.id_nop = 0;
    pthread_mutex_unlock(&vm.id_nop_mutex);

    update_fbuf(&next_fbuf);

    if (get_pc_override_signal()) {
        set_pc(get_pc_override());
    }

    if (errno) {
        int errno_cpy = errno;
        errno = 0;
        pthread_exit((void *)(intptr_t)errno_cpy);
    }

    pthread_exit((void *)(intptr_t)0);
}
