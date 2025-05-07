#include "id_util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../isa.h"
#include "bitops.h"

/**
 * Sets `dbuf` unconditionally.
 *
 * @param dbuf pointer to dbuf to set
 */
static void set_dbuf(dbuf_t *dbuf);

fbuf_t get_fbuf() {
    fbuf_t fbuf;

    pthread_mutex_lock(&vm.fbuf_mutex);
    fbuf = vm.fbuf;
    pthread_mutex_unlock(&vm.fbuf_mutex);

    return fbuf;
}

dbuf_t *init_dbuf(fbuf_t fbuf) {
    dbuf_t *dbuf = malloc(sizeof(dbuf_t));

    dbuf->nop = fbuf.nop;
    dbuf->pc = fbuf.pc;
    dbuf->opcode = get_opcode(fbuf.ir);

    return dbuf;
}

void send_bubble_to_id() {
    pthread_mutex_lock(&vm.fbuf_nop_mutex);
    vm.fbuf_nop = 1;
    pthread_mutex_unlock(&vm.fbuf_nop_mutex);
}

void send_bubble_to_ex() {
    pthread_mutex_lock(&vm.dbuf_mutex);
    vm.dbuf.nop = 1;
    pthread_mutex_unlock(&vm.dbuf_mutex);
}

int16_t get_opcode(int16_t ir) { return bit_range(ir, 12, 15); }

int id_instruction_is_jsr(int16_t ir) {
    return get_opcode(ir) == OP_JSR && bit_range(ir, 11, 11);
}

int id_instruction_is_jsrr(int16_t ir) {
    return get_opcode(ir) == OP_JSRR && !bit_range(ir, 11, 11);
}

void decode_br(fbuf_t fbuf, dbuf_t *dbuf) {
    if (fbuf.nop || !dbuf) {
        fprintf(stderr, "decode_br failed: fbuf must not be a nop and dbuf "
                        "must be non-null\n");
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
    dbuf->operand1 = (int16_t)fbuf.pc;
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
}

void decode_add_and(fbuf_t fbuf, dbuf_t *dbuf) {
    if (fbuf.nop || !dbuf) {
        fprintf(stderr,
                "decode_add_and failed: fbuf must not be a nop and dbuf "
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

    pthread_mutex_lock(&vm.reg_mutex);
    // if register is busy
    if (!vm.register_file[bit_range(fbuf.ir, 6, 8)].busy_counter) {
        // tell stage to wait
        pthread_mutex_lock(&vm.fbuf_stay_mutex);
        vm.fbuf_stay = 1;
        pthread_mutex_unlock(&vm.fbuf_stay_mutex);

        // send bubble forward
        pthread_mutex_lock(&vm.dbuf_mutex);
        vm.dbuf.nop = 1;
        pthread_mutex_unlock(&vm.dbuf_mutex);

        pthread_mutex_unlock(&vm.reg_mutex);
        return;
    }

    dbuf->operand1 = vm.register_file[bit_range(fbuf.ir, 6, 8)].data;
    pthread_mutex_unlock(&vm.reg_mutex);

    if (bit_range(fbuf.ir, 5, 5) == 0) {
        pthread_mutex_lock(&vm.reg_mutex);
        dbuf->operand2 = vm.register_file[bit_range(fbuf.ir, 0, 2)].data;
        pthread_mutex_unlock(&vm.reg_mutex);
    } else {
        dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 4), 5);
    }
}

void decode_ld_ldi_lea(fbuf_t fbuf, dbuf_t *dbuf) {
    if (fbuf.nop || !dbuf) {
        fprintf(stderr,
                "decode_ld_ldi_lea failed: fbuf must not be a nop and dbuf "
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
    dbuf->operand1 = (int16_t)fbuf.pc;
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
}

void decode_st_sti(fbuf_t fbuf, dbuf_t *dbuf) {
    if (fbuf.nop || !dbuf) {
        fprintf(stderr, "decode_st_sti failed: fbuf must not be a nop and dbuf "
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

    pthread_mutex_lock(&vm.reg_mutex);
    dbuf->reg = vm.register_file[bit_range(fbuf.ir, 9, 11)].data;
    pthread_mutex_unlock(&vm.reg_mutex);

    dbuf->operand1 = (int16_t)fbuf.pc;
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
}

void decode_jsr(fbuf_t fbuf, dbuf_t *dbuf) {
    if (fbuf.nop || !dbuf) {
        fprintf(stderr,
                "decode_jsr failed: fbuf must not be a nop and dbuf must "
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

    dbuf->operand1 = (int16_t)fbuf.pc;
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 10), 11);
    dbuf->bit11 = 1;
}

void decode_jmp_jsrr(fbuf_t fbuf, dbuf_t *dbuf) {
    if (fbuf.nop || !dbuf) {
        fprintf(stderr,
                "decode_jmp_jsrr failed: fbuf must not be a nop and dbuf "
                "must be non-null\n");
        errno = EINVAL;
        return;
    }

    int opcode = get_opcode(fbuf.ir);
    if (opcode != OP_JMP && opcode != OP_JSRR) {
        fprintf(stderr, "decode_jmp_jsrr failed: fbuf.ir must have opcode "
                        "OP_JMP or OP_JSRR\n");
        errno = EINVAL;
        return;
    }

    pthread_mutex_lock(&vm.reg_mutex);
    dbuf->reg = vm.register_file[bit_range(fbuf.ir, 6, 8)].data;
    pthread_mutex_unlock(&vm.reg_mutex);

    dbuf->bit11 = 0;
}

void decode_ldr(fbuf_t fbuf, dbuf_t *dbuf) {
    if (fbuf.nop || !dbuf) {
        fprintf(stderr,
                "decode_ldr failed: fbuf must not be a nop and dbuf must "
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

    pthread_mutex_lock(&vm.reg_mutex);
    dbuf->operand1 = vm.register_file[bit_range(fbuf.ir, 6, 8)].data;
    pthread_mutex_unlock(&vm.reg_mutex);

    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 5), 6);
}

void decode_str(fbuf_t fbuf, dbuf_t *dbuf) {
    if (fbuf.nop || !dbuf) {
        fprintf(stderr,
                "decode_str failed: fbuf must not be a nop and dbuf must "
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

    pthread_mutex_lock(&vm.reg_mutex);
    dbuf->reg = vm.register_file[bit_range(fbuf.ir, 9, 11)].data;
    dbuf->operand1 = vm.register_file[bit_range(fbuf.ir, 6, 8)].data;
    pthread_mutex_unlock(&vm.reg_mutex);

    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 5), 6);
}

void decode_not(fbuf_t fbuf, dbuf_t *dbuf) {
    if (fbuf.nop || !dbuf) {
        fprintf(stderr,
                "decode_not failed: fbuf must not be a nop and dbuf must "
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

    pthread_mutex_lock(&vm.reg_mutex);
    dbuf->operand1 = vm.register_file[bit_range(fbuf.ir, 6, 8)].data;
    pthread_mutex_unlock(&vm.reg_mutex);
}

void decode_trap(fbuf_t fbuf, dbuf_t *dbuf) {
    if (fbuf.nop || !dbuf) {
        fprintf(stderr,
                "decode_trap failed: fbuf must not be a nop and dbuf must "
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

void increment_busy_counter(uint16_t register_num) {
    pthread_mutex_lock(&vm.reg_mutex);
    vm.register_file[register_num].busy_counter++;
    pthread_mutex_unlock(&vm.reg_mutex);
}

void update_dbuf(dbuf_t *dbuf) {
    pthread_mutex_lock(&vm.dbuf_stay_mutex);

    if (!vm.dbuf_stay) {
        set_dbuf(dbuf);
    } else {
        vm.dbuf_stay = 0;
    }

    pthread_mutex_unlock(&vm.dbuf_stay_mutex);
}

static void set_dbuf(dbuf_t *dbuf) {
    pthread_mutex_lock(&vm.dbuf_mutex);
    memcpy(&vm.dbuf, dbuf, sizeof(dbuf_t));
    pthread_mutex_unlock(&vm.dbuf_mutex);
}
