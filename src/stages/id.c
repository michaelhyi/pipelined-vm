#include "id.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../isa.h"
#include "../util/bitops.h"
#include "../util/id_util.h"
#include "../vm.h"

void *id_run(void *arg) {
    (void)arg;

    fbuf_t fbuf = get_fbuf();

    if (fbuf.nop) {
        pthread_barrier_wait(&vm.pipeline_cycle_barrier);
        send_bubble_to_ex();
        return NULL;
    }

    dbuf_t *dbuf = init_dbuf(fbuf);

    if (dbuf->opcode == OP_BR) {
        decode_br(fbuf, dbuf);
    } else if (dbuf->opcode == OP_ADD || dbuf->opcode == OP_AND) {
        decode_add_and(fbuf, dbuf);
        increment_busy_counter((uint16_t)dbuf->reg);
    } else if (dbuf->opcode == OP_LD || dbuf->opcode == OP_LDI ||
               dbuf->opcode == OP_LEA) {
        decode_ld_ldi_lea(fbuf, dbuf);
        increment_busy_counter((uint16_t)dbuf->reg);
    } else if (dbuf->opcode == OP_ST || dbuf->opcode == OP_STI) {
        decode_st_sti(fbuf, dbuf);
    } else if (id_instruction_is_jsr(fbuf.ir)) {
        decode_jsr(fbuf, dbuf);
        send_bubble_to_id();
        increment_busy_counter((uint16_t)7);
    } else if (id_instruction_is_jsrr(fbuf.ir) || dbuf->opcode == OP_JMP) {
        decode_jmp_jsrr(fbuf, dbuf);
        send_bubble_to_id();
        increment_busy_counter((uint16_t)7);
    } else if (dbuf->opcode == OP_LDR) {
        decode_ldr(fbuf, dbuf);
        increment_busy_counter((uint16_t)dbuf->reg);
    } else if (dbuf->opcode == OP_STR) {
        decode_str(fbuf, dbuf);
    } else if (dbuf->opcode == OP_NOT) {
        decode_not(fbuf, dbuf);
        increment_busy_counter((uint16_t)dbuf->reg);
    } else if (dbuf->opcode == OP_TRAP) {
        decode_trap(fbuf, dbuf);
    }

    if (errno) {
        pthread_exit((void *)(intptr_t)errno);
    }

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);
    update_dbuf(dbuf);
    free(dbuf);
    return NULL;
}
