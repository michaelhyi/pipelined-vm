#include "ex.h"

#include <errno.h>

#include "../isa.h"
#include "../util/ex_util.h"
#include "../util/vm_util.h"
#include "../vm.h"

/**
 * Tears down the `ex` stage.
 *
 * @param next_ebuf the next ebuf that the vm will use
 */
static void ex_teardown(ebuf_t next_ebuf);

void *ex_run(void *arg) {
    (void)arg;

    dbuf_t dbuf = get_dbuf();

    if (dbuf.nop) {
        send_bubble_to_mem();
        ex_teardown((ebuf_t){.nop = 1});
        return NULL;
    }

    ebuf_t next_ebuf = init_next_ebuf(dbuf);

    if (dbuf.opcode == OP_BR) {
        int16_t cc = get_cc_data();

        // TODO: abstract into functions
        if (cc_match(cc, dbuf.cc)) {
            uint16_t new_pc = (uint16_t)(dbuf.operand1 + dbuf.operand2);
            set_pc_override(new_pc);

            send_bubble_to_id();
            // TODO: if we squash an instruction in decode that incremented the
            // busy counter, we need to decrement it here
            send_bubble_to_ex();
        }
    } else if (instruction_needs_add(dbuf.opcode)) {
        next_ebuf.result = dbuf.operand1 + dbuf.operand2;
    } else if (ex_instruction_is_jsr(dbuf)) {
        send_bubble_to_id();

        uint16_t new_pc = (uint16_t)(dbuf.operand1 + dbuf.operand2);
        set_pc_override(new_pc);
    } else if (ex_instruction_is_jsrr(dbuf)) {
        send_bubble_to_id();

        uint16_t new_pc = (uint16_t)dbuf.reg;
        set_pc_override(new_pc);
    } else if (dbuf.opcode == OP_AND) {
        next_ebuf.result = dbuf.operand1 & dbuf.operand2;
    } else if (dbuf.opcode == OP_NOT) {
        next_ebuf.result = ~dbuf.operand1;
    } else if (dbuf.opcode == OP_JMP) {
        send_bubble_to_id();

        uint16_t new_pc = (uint16_t)dbuf.reg;
        set_pc_override(new_pc);
    } else if (dbuf.opcode == OP_TRAP) {
        // TODO: fetch starting address of trap handler using the trap vector
        // table, then set pc to starting address of trap handler
        // Some tests are failing because there is no trap handler
    }

    if (errno) {
        ex_teardown(next_ebuf);
    }

    ex_teardown(next_ebuf);
    return NULL;
}

static void ex_teardown(ebuf_t next_ebuf) {
    pthread_barrier_wait(&vm.pipeline_cycle_barrier);

    if (errno) {
        int errno_cpy = errno;
        errno = 0;
        pthread_exit((void *)(intptr_t)errno_cpy);
    }

    // handle bubbles
    pthread_mutex_lock(&vm.mem_nop_mutex);
    next_ebuf.nop = vm.mem_nop;
    vm.mem_nop = 0;
    pthread_mutex_unlock(&vm.mem_nop_mutex);

    update_ebuf(&next_ebuf);

    if (errno) {
        int errno_cpy = errno;
        errno = 0;
        pthread_exit((void *)(intptr_t)errno_cpy);
    }

    pthread_exit((void *)(intptr_t)0);
}
