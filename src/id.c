#include "id.h"

#include <stddef.h>

#include "util.h"
#include "vm.h"

// TODO: for all stages, handle the case where buffers are empty. (ie first time
// running the pipeline)
void *id_exec_cycle(void *arg) {
    if (!vm.dbuf.ready) {
        return NULL;
    }

    dbuf_t *dbuf = (dbuf_t *)arg;

    dbuf->ready = 1;
    dbuf->pc = vm.fbuf.pc;
    dbuf->opcode = ((uint16_t)vm.fbuf.ir) >> 12 & 0xF;

    if (dbuf->opcode == OP_ADD || dbuf->opcode == OP_AND) {
        dbuf->reg = bit_range(vm.fbuf.ir, 9, 11);
        dbuf->operand1 = vm.reg[bit_range(vm.fbuf.ir, 6, 8)];

        if ((((uint16_t)vm.fbuf.ir >> 5) & 0x1) == 0) {
            dbuf->operand2 = vm.reg[bit_range(vm.fbuf.ir, 0, 2)];
        } else {
            dbuf->operand2 = sign_extend(bit_range(vm.fbuf.ir, 0, 4), 5);
        }
    } else if (dbuf->opcode == OP_NOT) {
        dbuf->reg = ((uint16_t)vm.fbuf.ir >> 9) & 0x7;
        dbuf->operand1 = vm.reg[((uint16_t)vm.fbuf.ir >> 6) & 0x7];
    } else if (dbuf->opcode == OP_BR) {
        dbuf->nzp = ((uint16_t)vm.fbuf.ir >> 9) & 0x7;
        dbuf->operand1 = sign_extend(vm.fbuf.ir & 0x1FF, 9);
    } else if (dbuf->opcode == OP_JMP ||
               (dbuf->opcode == OP_JSR && !((dbuf->opcode >> 11) & 1))) {
        dbuf->operand1 = vm.reg[((uint16_t)vm.fbuf.ir >> 9) & 0x7];
    } else if (dbuf->opcode == OP_JSR) {
        dbuf->operand1 = sign_extend(vm.fbuf.ir & 0x7FF, 11);
    } else if (dbuf->opcode == OP_LD || dbuf->opcode == OP_LDI ||
               dbuf->opcode == OP_LEA || dbuf->opcode == OP_ST ||
               dbuf->opcode == OP_STI) {
        dbuf->reg = ((uint16_t)vm.fbuf.ir >> 9) & 0x7;
        dbuf->operand1 = sign_extend(vm.fbuf.ir & 0x1FF, 9);
    } else if (dbuf->opcode == OP_LDR || dbuf->opcode == OP_STR) {
        dbuf->reg = ((uint16_t)vm.fbuf.ir >> 9) & 0x7;
        dbuf->operand1 = vm.reg[((uint16_t)vm.fbuf.ir >> 6) & 0x7];
        dbuf->operand2 = sign_extend(vm.fbuf.ir & 0x3F, 6);
    } else if (dbuf->opcode == OP_TRAP) {
        dbuf->operand1 = vm.fbuf.ir & 0xFF;
    }

    switch (dbuf->opcode) {
    case 0x4: // jsr & jsrr
        if ((((uint16_t)vm.fbuf.ir >> 11) & 0x1) == 0) {
            dbuf->operand1 = ((uint16_t)vm.fbuf.ir >> 6) & 0x7;
        } else {
            dbuf->operand1 = sign_extend(vm.fbuf.ir & 0x7FF, 11);
        }
        return NULL;

    case 0xC: // jmp
        dbuf->operand1 = ((uint16_t)vm.fbuf.ir >> 6) & 0x7;
        return NULL;
    }

    return NULL;
}
