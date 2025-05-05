#include "vm.h"

int main(void) {
    vm_init();
    vm_run();
    vm_teardown();

    return 0;
}
