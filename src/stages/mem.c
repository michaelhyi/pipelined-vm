#include "mem.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../vm.h"

void *mem_run(void *arg) {
    if (!arg) {
        fprintf(stderr, "mem_run failed: arg must be non-null\n");
        errno = EINVAL;
        return NULL;
    }

    ebuf_t ebuf = vm.ebuf;

    if (!ebuf.ready) {
        return NULL;
    }

    mbuf_t *mbuf = (mbuf_t *)arg;
    memcpy(mbuf, &ebuf, sizeof(mbuf_t));

    if (ebuf.opcode == OP_LD || ebuf.opcode == OP_LDR) {
        mbuf->result = vm.mem[ebuf.result];
    } else if (ebuf.opcode == OP_LDI) {
        // TODO: send bubbles
    } else if (ebuf.opcode == OP_ST || ebuf.opcode == OP_STR) {
        vm.mem[ebuf.result] = ebuf.reg;
    }

    return NULL;
}
