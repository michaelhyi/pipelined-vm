#include "ex_util.h"

#include <stdlib.h>
#include <string.h>

#include "../isa.h"

/**
 * Sets `ebuf` unconditionally.
 *
 * @param ebuf pointer to ebuf to set
 */
static void set_ebuf(ebuf_t *ebuf);

dbuf_t get_dbuf(void) {
    dbuf_t dbuf;

    pthread_mutex_lock(&vm.dbuf_mutex);
    dbuf = vm.dbuf;
    pthread_mutex_unlock(&vm.dbuf_mutex);

    return dbuf;
}

void send_bubble_to_mem(void) {
    pthread_mutex_lock(&vm.ebuf_mutex);
    vm.ebuf.nop = 1;
    pthread_mutex_unlock(&vm.ebuf_mutex);
}

ebuf_t *init_ebuf(dbuf_t dbuf) {
    ebuf_t *ebuf = malloc(sizeof(ebuf_t));

    ebuf->nop = dbuf.nop;
    ebuf->pc = dbuf.pc;
    ebuf->opcode = dbuf.opcode;
    ebuf->reg = dbuf.reg;
    ebuf->indirect_counter = 0;

    return ebuf;
}

int instruction_needs_add(int16_t opcode) {
    return opcode == OP_ADD || opcode == OP_LD || opcode == OP_ST ||
           opcode == OP_LDR || opcode == OP_STR || opcode == OP_LDI ||
           opcode == OP_STI || opcode == OP_LEA;
}

int ex_instruction_is_jsr(dbuf_t dbuf) {
    return dbuf.opcode == OP_JSR && dbuf.bit11;
}

int ex_instruction_is_jsrr(dbuf_t dbuf) {
    return dbuf.opcode == OP_JSRR && !dbuf.bit11;
}

void flush_id(void) {
    pthread_mutex_lock(&vm.fbuf_nop_mutex);
    vm.fbuf_nop = 1;
    pthread_mutex_unlock(&vm.fbuf_nop_mutex);
}

void override_pc(uint16_t new_pc) {
    pthread_mutex_lock(&vm.pc_mutex);
    vm.pc = new_pc;
    pthread_mutex_unlock(&vm.pc_mutex);
}

void update_ebuf(ebuf_t *ebuf) {
    pthread_mutex_lock(&vm.mem_stay_mutex);

    if (!vm.mem_stay) {
        set_ebuf(ebuf);
    } else {
        vm.mem_stay = 0;
    }

    pthread_mutex_unlock(&vm.mem_stay_mutex);
}

static void set_ebuf(ebuf_t *ebuf) {
    pthread_mutex_lock(&vm.ebuf_mutex);
    memcpy(&vm.ebuf, ebuf, sizeof(ebuf_t));
    pthread_mutex_unlock(&vm.ebuf_mutex);
}
