#include "vm.h"

int main(void) {
    vm_init(0);
    vm_run();
    vm_teardown();

    return 0;
}
