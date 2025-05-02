#include "id.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#include "../util.h"
#include "../vm.h"

/**
 * Returns whether the instruction performs JSR.
 *
 * @param ir instruction to decode
 * @return 1 if the instruction performs JSR, 0 otherwise
 */
static int is_jsr(int16_t ir);

/**
 * Returns whether the instruction performs JSRR.
 *
 * @param ir instruction to decode
 * @return 1 if the instruction performs JSRR, 0 otherwise
 */
static int is_jsrr(int16_t ir);

void *id_run(void *arg) {
    // TODO: need to set shared err, since errno is thread-local
    if (!arg) {
        fprintf(stderr, "id_run failed: arg must be non-null\n");
        errno = EINVAL;
        return NULL;
    }

    fbuf_t fbuf = vm.fbuf;
    if (!fbuf.ready) {
        return NULL;
    }

    dbuf_t *dbuf = (dbuf_t *)arg;
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
        dbuf->cc = bit_range(fbuf.ir, 9, 11);
        dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
    } else if (dbuf->opcode == OP_JMP || is_jsrr(fbuf.ir)) {
        dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];
    } else if (is_jsr(fbuf.ir)) {
        dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 10), 11);
    } else if (dbuf->opcode == OP_LD || dbuf->opcode == OP_LDI ||
               dbuf->opcode == OP_LEA) {
        dbuf->reg = bit_range(fbuf.ir, 9, 11);
        dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
    } else if (dbuf->opcode == OP_ST || dbuf->opcode == OP_STI) {
        dbuf->reg = vm.reg[bit_range(fbuf.ir, 9, 11)];
        dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
    } else if (dbuf->opcode == OP_LDR) {
        dbuf->reg = bit_range(fbuf.ir, 9, 11);
        dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];
        dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 5), 6);
    } else if (dbuf->opcode == OP_STR) {
        dbuf->reg = vm.reg[bit_range(fbuf.ir, 9, 11)];
        dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];
        dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 5), 6);
    } else if (dbuf->opcode == OP_TRAP) {

        dbuf->operand1 = bit_range(fbuf.ir, 0, 7);
    }

    return NULL;
}

static int is_jsr(int16_t ir) {
    int opcode = bit_range(ir, 12, 15);
    return opcode == OP_JSR && bit_range(ir, 11, 11);
}

static int is_jsrr(int16_t ir) {
    int opcode = bit_range(ir, 12, 15);
    return opcode == OP_JSR && !bit_range(ir, 11, 11);
}
