#include "ex.h"

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
        // TODO: compare nzp bits, override pc on match
    } else if (instruction_needs_add(dbuf.opcode)) {
        next_ebuf.result = dbuf.operand1 + dbuf.operand2;
    } else if (ex_instruction_is_jsr(dbuf)) {
        send_bubble_to_id();

        uint16_t new_pc = (uint16_t)(dbuf.operand1 + dbuf.operand2);
        set_pc(new_pc); // TODO: what if this runs before IF increments PC
    } else if (ex_instruction_is_jsrr(dbuf)) {
        send_bubble_to_id();

        uint16_t new_pc = (uint16_t)dbuf.reg;
        set_pc(new_pc);
    } else if (dbuf.opcode == OP_AND) {
        next_ebuf.result = dbuf.operand1 & dbuf.operand2;
    } else if (dbuf.opcode == OP_NOT) {
        next_ebuf.result = ~dbuf.operand1;
    } else if (dbuf.opcode == OP_JMP) {
        send_bubble_to_id();

        uint16_t new_pc = (uint16_t)dbuf.reg;
        set_pc(new_pc);
    } else if (dbuf.opcode == OP_TRAP) {
        // TODO: fetch starting address of trap handler using the trap vector
        // table, then set pc to starting address of trap handler
    }

    ex_teardown(next_ebuf);
    return NULL;
}

static void ex_teardown(ebuf_t next_ebuf) {
    pthread_barrier_wait(&vm.pipeline_cycle_barrier);

    // handle bubbles
    pthread_mutex_lock(&vm.mem_nop_mutex);
    next_ebuf.nop = vm.mem_nop;
    vm.mem_nop = 0;
    pthread_mutex_unlock(&vm.mem_nop_mutex);

    update_ebuf(&next_ebuf);

    pthread_exit(0);
}
