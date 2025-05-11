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

#include "vm.h"

void loader_run(void) {
    while (1) {
        char addr_str[4], data_str[4];

        printf("enter mem addr: 0x");
        assert(scanf("%s", addr_str) == 1);
        uint16_t addr = (uint16_t)strtol(addr_str, NULL, 16);

        if (!addr) {
            break;
        }

        printf("set mem[0x%hx]=0x", addr);
        assert(scanf("%s", data_str) == 1);
        int16_t data = (int16_t)strtol(data_str, NULL, 16);

        vm.mem[addr] = data;
    }
}
