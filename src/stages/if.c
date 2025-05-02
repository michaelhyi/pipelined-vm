#include "if.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include "../vm.h"

void *if_run(void *arg) {
    if (!arg) {
        fprintf(stderr, "if_run failed: arg must be non-null\n");
        pthread_exit((void *)(intptr_t)EINVAL);
    }

    fbuf_t *fbuf = (fbuf_t *)arg;

    fbuf->ready = 1;
    fbuf->pc = vm.pc;
    fbuf->ir = vm.mem[vm.pc++];

    return NULL;
}
