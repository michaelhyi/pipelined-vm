#include "if.h"

#include <stddef.h>

#include "vm.h"

void *if_exec_cycle(void *arg) {
  fbuf_t *fbuf = (fbuf_t *)arg;

  fbuf->pc = vm.pc;
  fbuf->instruction = vm.mem[vm.pc++];

  return NULL;
}
