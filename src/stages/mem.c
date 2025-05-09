#include "mem.h"

#include <errno.h>
#include <string.h>

#include "../isa.h"
#include "../util/mem_util.h"
#include "../util/vm_util.h"
#include "../vm.h"

// TODO: refactor each of these comments, need for wb?
/**
 * A function that runs at the end of every `MEM` stage to tear it down.
 *
 * @param next_mbuf the next mbuf that the VM will use
 */
static void mem_teardown(mbuf_t next_mbuf);

void *mem_run(void *arg) {
    (void)arg;

    ebuf_t ebuf = get_ebuf(); // TODO: next_ebuf

    if (ebuf.nop) {
        send_bubble_to_wb();
        pthread_barrier_wait(&vm.pipeline_cycle_barrier);
        mem_teardown((mbuf_t){.nop = 1});
        return NULL;
    }

    mbuf_t next_mbuf = init_next_mbuf(ebuf);

    if (ebuf.opcode == OP_LD || ebuf.opcode == OP_LDR) {
        next_mbuf.result = get_mem((uint16_t)ebuf.result);
    } else if (ebuf.opcode == OP_ST || ebuf.opcode == OP_STR) {
        set_mem((uint16_t)ebuf.result, ebuf.reg);
    } else if (ebuf.opcode == OP_LDI || ebuf.opcode == OP_STI) {
        next_mbuf.result = get_mem((uint16_t)ebuf.result);

        pthread_mutex_lock(&vm.ebuf_mutex);
        vm.ebuf.indirect_counter =
            (uint16_t)((vm.ebuf.indirect_counter + 1) % 2);

        // if first indirect
        if (vm.ebuf.indirect_counter) {
            vm.ebuf.result = next_mbuf.result;
            stall_pipeline();
        } else if (ebuf.opcode == OP_STI) {
            set_mem((uint16_t)ebuf.result, ebuf.reg);
        }
        pthread_mutex_unlock(&vm.ebuf_mutex);
    }

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);
    mem_teardown(next_mbuf);
    return NULL;
}

static void mem_teardown(mbuf_t next_mbuf) {
    // receive any bubbles
    pthread_mutex_lock(&vm.wb_nop_mutex);
    next_mbuf.nop = vm.wb_nop;
    vm.wb_nop = 0;
    pthread_mutex_unlock(&vm.wb_nop_mutex);

    update_mbuf(&next_mbuf);

    pthread_exit(0);
}
