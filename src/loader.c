/**
 * All operations interfacing with the vm can be assumed to be thread-safe, even
 * without mutex locks, as there are no threads concurrently running before and
 * after `vm_run` is called.
 */

#include "loader.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

void loader_run(char *file_path) {
    FILE *f_ptr;

    f_ptr = fopen(file_path, "r");
    assert(f_ptr);

    char *line;
    size_t cap = 0;
    size_t line_num = 0;

    while (getline(&line, &cap, f_ptr) != -1) {
        line_num++;

        if (line_num < 4) {
            continue;
        }

        if (!strcmp(line, "\n")) {
            break;
        }

        uint16_t orig = (uint16_t)strtol(line, NULL, 16);
        assert(getline(&line, &cap, f_ptr) != -1);
        size_t lines_remaining = (size_t)atoi(line);

        for (size_t i = 0; i < lines_remaining; i++) {
            assert(getline(&line, &cap, f_ptr) != -1);

            if (!strncmp(line, "????", 4)) {
                continue;
            }

            int16_t val = (int16_t)strtol(line, NULL, 16);
            vm.mem[orig + i] = val;
        }
    }

    fclose(f_ptr);
    if (line) {
        free(line);
    }
}
