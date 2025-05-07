#include "mem.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../isa.h"
#include "../util/mem_util.h"
#include "../vm.h"

void *mem_run(void *arg) {
    (void)arg;

    ebuf_t ebuf = get_ebuf();

    if (ebuf.nop) {
        pthread_barrier_wait(&vm.pipeline_cycle_barrier);
        send_bubble_to_wb();
        return NULL;
    }

    mbuf_t *mbuf = init_mbuf(ebuf);

    if (ebuf.opcode == OP_LD || ebuf.opcode == OP_LDR) {
        mbuf->result = mem_get((uint16_t)ebuf.result);
    } else if (ebuf.opcode == OP_ST || ebuf.opcode == OP_STR) {
        mem_set((uint16_t)ebuf.result, ebuf.reg);
    } else if (ebuf.opcode == OP_LDI || ebuf.opcode == OP_STI) {
        mbuf->result = mem_get((uint16_t)ebuf.result);

        pthread_mutex_lock(&vm.ebuf_mutex);
        vm.ebuf.indirect_counter =
            (uint16_t)((vm.ebuf.indirect_counter + 1) % 2);

        // if first indirect
        if (vm.ebuf.indirect_counter) {
            vm.ebuf.result = mbuf->result;
            stall_pipeline();
        } else if (ebuf.opcode == OP_STI) {
            mem_set((uint16_t)ebuf.result, ebuf.reg);
        }
        pthread_mutex_unlock(&vm.ebuf_mutex);
    }

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);
    update_mbuf(mbuf);
    return NULL;
}
