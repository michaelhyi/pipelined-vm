#include "vm.h"

vm_t vm;

void system_init() { vm.pc = 0x3000; }
