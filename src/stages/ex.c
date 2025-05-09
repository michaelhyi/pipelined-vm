#include "ex.h"

#include "../isa.h"
#include "../util/ex_util.h"
#include "../util/vm_util.h"
#include "../vm.h"

/**
 * A function that runs at the end of every `EX` stage to tear it down.
 *
 * @param next_ebuf the next ebuf that the VM will use
 */
static void ex_teardown(ebuf_t next_ebuf);

void *ex_run(void *arg) {
    (void)arg;

    dbuf_t dbuf = get_dbuf();

    if (dbuf.nop) {
        send_bubble_to_mem();
        pthread_barrier_wait(&vm.pipeline_cycle_barrier);
        ex_teardown((ebuf_t){.nop = 1});
        return NULL;
    }

    ebuf_t ebuf = init_ebuf(dbuf);

    if (dbuf.opcode == OP_BR) {
        // TODO: compare nzp bits, override pc on match
    } else if (instruction_needs_add(dbuf.opcode)) {
        ebuf.result = dbuf.operand1 + dbuf.operand2;
    } else if (ex_instruction_is_jsr(dbuf)) {
        send_bubble_to_id();

        uint16_t new_pc = (uint16_t)(dbuf.operand1 + dbuf.operand2);
        set_pc(new_pc); // TODO: what if this runs before IF increments PC
    } else if (ex_instruction_is_jsrr(dbuf)) {
        send_bubble_to_id();

        uint16_t new_pc = (uint16_t)dbuf.reg;
        set_pc(new_pc);
    } else if (dbuf.opcode == OP_AND) {
        ebuf.result = dbuf.operand1 & dbuf.operand2;
    } else if (dbuf.opcode == OP_NOT) {
        ebuf.result = ~dbuf.operand1;
    } else if (dbuf.opcode == OP_JMP) {
        send_bubble_to_id();

        uint16_t new_pc = (uint16_t)dbuf.reg;
        set_pc(new_pc);
    } else if (dbuf.opcode == OP_TRAP) {
        // TODO: fetch starting address of trap handler using the trap vector
        // table, then set pc to starting address of trap handler
    }

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);
    ex_teardown(ebuf);
    return NULL;
}

static void ex_teardown(ebuf_t next_ebuf) {
    // receive any bubbles
    pthread_mutex_lock(&vm.mem_nop_mutex);
    next_ebuf.nop = vm.mem_nop;
    vm.mem_nop = 0;
    pthread_mutex_unlock(&vm.mem_nop_mutex);

    update_ebuf(&next_ebuf);

    pthread_exit(0);
}
