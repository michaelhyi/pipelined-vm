/**
 * All operations interfacing with the vm can be assumed to be thread-safe, even
 * without mutex locks, as there are no threads concurrently running before and
 * after `vm_run` is called.
 */

#include "loader.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "vm.h"

void loader_run(void) {
    while (1) {
        uint16_t addr;
        int16_t data;

        printf("enter mem addr: 0x");
        assert(scanf("%hx", &addr) == 1);

        if (!addr) {
            break;
        }

        printf("set mem[0x%hx]=0x", addr);
        assert(scanf("%hi", &data) == 1);

        vm.mem[addr] = data;
    }
}
