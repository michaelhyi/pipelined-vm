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
        // TODO: implement rti, undo check for access violation
        send_bubble_to_id();
        send_bubble_to_ex();

        uint16_t r6 = (uint16_t)get_register_data(6);
        set_saved_usp(r6);
        set_register_data(6, (int16_t)get_saved_ssp());

        set_register_data(6, get_register_data(6) - 1);
        set_mem((uint16_t)get_register_data(6), (int16_t)get_pc());

        set_register_data(6, get_register_data(6) - 1);
        set_mem((uint16_t)get_register_data(6), get_psr());

        set_psr((int16_t)(0 << 15));
        set_pc((uint16_t)get_mem((uint16_t)dbuf.operand1));
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
