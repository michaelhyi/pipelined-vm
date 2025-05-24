#include "vm.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 3 || strcmp(argv[1], "-l")) {
        fprintf(
            stderr,
            "2 additional arguments must be passed: -l <asm obj file path>\n");
        return 1;
    }

    char *obj_file_path = argv[2];

    vm_init(0, obj_file_path);
    vm_run(0); // TODO: make test bit a global var
    vm_teardown(0);

    return 0;
}
