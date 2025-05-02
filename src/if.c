#include "if.h"

#include <stddef.h>

#include "vm.h"

void *if_exec_cycle(void *arg) {
    fbuf_t *fbuf = (fbuf_t *)arg;

    fbuf->ready = 1;
    fbuf->pc = vm.pc;
    fbuf->ir = vm.mem[vm.pc++];

    return NULL;
}
