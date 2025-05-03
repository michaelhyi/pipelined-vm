#include "id_util.h"

#include <errno.h>
#include <stdio.h>

#include "../stages/id.h"
#include "../vm.h"
#include "bitops.h"

int16_t get_opcode(int16_t ir) { return bit_range(ir, 12, 15); }

int is_jsr(int16_t ir) {
    return get_opcode(ir) == OP_JSR && bit_range(ir, 11, 11);
}

int is_jsrr(int16_t ir) {
    return get_opcode(ir) == OP_JSR && !bit_range(ir, 11, 11);
}

void decode_add_and(fbuf_t fbuf, dbuf_t *dbuf) {
    if (!fbuf.ready || !dbuf) {
        fprintf(stderr, "decode_add_and failed: fbuf must be ready and dbuf "
                        "must be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_ADD && opcode != OP_AND) {
        fprintf(stderr, "decode_add_and failed: fbuf.ir must have opcode "
                        "OP_ADD or OP_AND\n");
        errno = EINVAL;
        return;
    }

    dbuf->reg = bit_range(fbuf.ir, 9, 11);
    dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];

    if (bit_range(fbuf.ir, 5, 5) == 0) {
        dbuf->operand2 = vm.reg[bit_range(fbuf.ir, 0, 2)];
    } else {
        dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 4), 5);
    }
}

void decode_not(fbuf_t fbuf, dbuf_t *dbuf) {
    if (!fbuf.ready || !dbuf) {
        fprintf(stderr, "decode_not failed: fbuf must be ready and dbuf must "
                        "be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_NOT) {
        fprintf(stderr, "decode_not failed: fbuf.ir must have opcode OP_NOT\n");
        errno = EINVAL;
        return;
    }

    dbuf->reg = bit_range(fbuf.ir, 9, 11);
    dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];
}

void decode_br(fbuf_t fbuf, dbuf_t *dbuf) {
    if (!fbuf.ready || !dbuf) {
        fprintf(
            stderr,
            "decode_br failed: fbuf must be ready and dbuf must be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_BR) {
        fprintf(stderr, "decode_br failed: fbuf.ir must have opcode OP_BR\n");
        errno = EINVAL;
        return;
    }

    dbuf->cc = bit_range(fbuf.ir, 9, 11);
    dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
}

void decode_jmp_jsrr(fbuf_t fbuf, dbuf_t *dbuf) {
    if (!fbuf.ready || !dbuf) {
        fprintf(stderr, "decode_jmp_jsrr failed: fbuf must be ready and dbuf "
                        "must be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_JMP && opcode != OP_JSR) {
        fprintf(stderr, "decode_jmp_jsrr failed: fbuf.ir must have opcode "
                        "OP_JMP or OP_JSR\n");
        errno = EINVAL;
        return;
    }

    dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];
}

void decode_jsr(fbuf_t fbuf, dbuf_t *dbuf) {
    if (!fbuf.ready || !dbuf) {
        fprintf(stderr, "decode_jsr failed: fbuf must be ready and dbuf must "
                        "be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_JSR) {
        fprintf(stderr, "decode_jsr failed: fbuf.ir must have opcode OP_JSR\n");
        errno = EINVAL;
        return;
    }

    dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 10), 11);
}

void decode_ld_ldi_lea(fbuf_t fbuf, dbuf_t *dbuf) {
    if (!fbuf.ready || !dbuf) {
        fprintf(stderr, "decode_ld_ldi_lea failed: fbuf must be ready and dbuf "
                        "must be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_LD && opcode != OP_LDI && opcode != OP_LEA) {
        fprintf(stderr, "decode_ld_ldi_lea failed: fbuf.ir must have opcode "
                        "OP_LD, OP_LDI, or OP_LEA\n");
        errno = EINVAL;
        return;
    }

    dbuf->reg = bit_range(fbuf.ir, 9, 11);
    dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
}

void decode_st_sti(fbuf_t fbuf, dbuf_t *dbuf) {
    if (!fbuf.ready || !dbuf) {
        fprintf(stderr, "decode_st_sti failed: fbuf must be ready and dbuf "
                        "must be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_ST && opcode != OP_STI) {
        fprintf(
            stderr,
            "decode_st_sti failed: fbuf.ir must have opcode OP_ST or OP_STI\n");
        errno = EINVAL;
        return;
    }

    dbuf->reg = vm.reg[bit_range(fbuf.ir, 9, 11)];
    dbuf->operand1 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
}

void decode_ldr(fbuf_t fbuf, dbuf_t *dbuf) {
    if (!fbuf.ready || !dbuf) {
        fprintf(stderr, "decode_ldr failed: fbuf must be ready and dbuf must "
                        "be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_LDR) {
        fprintf(stderr, "decode_ldr failed: fbuf.ir must have opcode OP_LDR\n");
        errno = EINVAL;
        return;
    }

    dbuf->reg = bit_range(fbuf.ir, 9, 11);
    dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 5), 6);
}

void decode_str(fbuf_t fbuf, dbuf_t *dbuf) {
    if (!fbuf.ready || !dbuf) {
        fprintf(stderr, "decode_str failed: fbuf must be ready and dbuf must "
                        "be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_STR) {
        fprintf(stderr, "decode_str failed: fbuf.ir must have opcode OP_STR\n");
        errno = EINVAL;
        return;
    }

    dbuf->reg = vm.reg[bit_range(fbuf.ir, 9, 11)];
    dbuf->operand1 = vm.reg[bit_range(fbuf.ir, 6, 8)];
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 5), 6);
}

void decode_trap(fbuf_t fbuf, dbuf_t *dbuf) {
    if (!fbuf.ready || !dbuf) {
        fprintf(stderr, "decode_trap failed: fbuf must be ready and dbuf must "
                        "be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_TRAP) {
        fprintf(stderr,
                "decode_trap failed: fbuf.ir must have opcode OP_TRAP\n");
        errno = EINVAL;
        return;
    }

    dbuf->operand1 = bit_range(fbuf.ir, 0, 7);
}
