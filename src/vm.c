#include "vm.h"

vm_t vm;

void system_init() {
    vm.pc = 0x3000;
    vm.fbuf.ready = 0;
    vm.dbuf.ready = 0;
}