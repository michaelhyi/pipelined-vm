#include "id.h"

#include <stddef.h>

#include "util.h"
#include "vm.h"

void *id_exec_cycle(void *arg) {
    if (!vm.dbuf.ready) {
        return NULL;
    }

    dbuf_t *dbuf = (dbuf_t *)arg;
    fbuf_t fbuf = vm.fbuf;

    dbuf->ready = 1;
    dbuf->pc = fbuf.pc;
    dbuf->opcode = bit_range(fbuf.ir, 12, 15);

    if (dbuf->opcode == OP_ADD || dbuf->opcode == OP_AND) {
        dbuf->reg = bit_range(fbuf.ir, 9, 11);
        dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];

        if (bit_range(fbuf.ir, 5, 5) == 0) {
            dbuf->operand2 = vm.reg[bit_range(fbuf.ir, 0, 2)];
        } else {
            dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 4), 5);
        }
    } else if (dbuf->opcode == OP_NOT) {
        dbuf->reg = bit_range(fbuf.ir, 9, 11);
        dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];
    } else if (dbuf->opcode == OP_BR) {
        dbuf->nzp = bit_range(fbuf.ir, 9, 11);
        dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
    } else if (dbuf->opcode == OP_JMP ||
               (dbuf->opcode == OP_JSR && !bit_range(fbuf.ir, 11, 11))) {
        dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];
    } else if (dbuf->opcode == OP_JSR) {
        dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 10), 11);
    } else if (dbuf->opcode == OP_LD || dbuf->opcode == OP_LDI ||
               dbuf->opcode == OP_LEA || dbuf->opcode == OP_ST ||
               dbuf->opcode == OP_STI) {
        dbuf->reg = bit_range(fbuf.ir, 9, 11);
        dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
    } else if (dbuf->opcode == OP_LDR || dbuf->opcode == OP_STR) {
        dbuf->reg = bit_range(fbuf.ir, 9, 11);
        dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];
        dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 5), 6);
    } else if (dbuf->opcode == OP_TRAP) {
        dbuf->operand1 = bit_range(fbuf.ir, 0, 7);
    }

    return NULL;
}
