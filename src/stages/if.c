#include "if.h"

#include <errno.h>
#include <pthread.h>

#include "../util/if_util.h"

/**
 * Tears down the `if` stage.
 *
 * @param next_fbuf the next fbuf that the vm will use
 */
static void if_teardown(fbuf_t next_fbuf);

void *if_run(void *arg) {
    (void)arg;

    fbuf_t next_fbuf;
    next_fbuf.ir = get_instruction_and_increment_pc();
    save_pc(&next_fbuf);

    if_teardown(next_fbuf);
    return NULL;
}

static void if_teardown(fbuf_t next_fbuf) {
    pthread_barrier_wait(&vm.pipeline_cycle_barrier);

    // handle bubbles
    pthread_mutex_lock(&vm.id_nop_mutex);
    next_fbuf.nop = vm.id_nop;
    vm.id_nop = 0;
    pthread_mutex_unlock(&vm.id_nop_mutex);

    update_fbuf(&next_fbuf);

    pthread_exit(0);
}
