#include "ex.h"

#include <stdio.h>
#include <stdlib.h>

#include "../isa.h"
#include "../util/ex_util.h"
#include "../vm.h"

void *ex_run(void *arg) {
    (void)arg;

    dbuf_t dbuf = get_dbuf();

    if (dbuf.nop) {
        pthread_barrier_wait(&vm.pipeline_cycle_barrier);
        send_bubble_to_mem();
        return NULL;
    }

    ebuf_t *ebuf = init_ebuf(dbuf);

    if (dbuf.opcode == OP_BR) {
        // TODO: compare nzp bits, override pc on match
    } else if (instruction_needs_add(dbuf.opcode)) {
        ebuf->result = dbuf.operand1 + dbuf.operand2;
    } else if (ex_instruction_is_jsr(dbuf)) {
        flush_id();

        uint16_t new_pc = (uint16_t)(dbuf.operand1 + dbuf.operand2);
        override_pc(new_pc);
    } else if (ex_instruction_is_jsrr(dbuf)) {
        flush_id();

        uint16_t new_pc = (uint16_t)dbuf.reg;
        override_pc(new_pc);
    } else if (dbuf.opcode == OP_AND) {
        ebuf->result = dbuf.operand1 & dbuf.operand2;
    } else if (dbuf.opcode == OP_NOT) {
        ebuf->result = ~dbuf.operand1;
    } else if (dbuf.opcode == OP_JMP) {
        flush_id();

        uint16_t new_pc = (uint16_t)dbuf.reg;
        override_pc(new_pc);
    } else if (dbuf.opcode == OP_TRAP) {
        // TODO: fetch starting address of trap handler using the trap vector
        // table, then set pc to starting address of trap handler
    }

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);
    update_ebuf(ebuf);
    free(ebuf);
    return NULL;
}
