#include "id_util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../isa.h"
#include "../vm_util.h"
#include "bitops.h"

/**
 * Sets `dbuf` unconditionally.
 *
 * @param dbuf pointer to dbuf to set
 */
static void set_dbuf(dbuf_t *dbuf);

/**
 * Handles a busy register by sending a STAY signal to the `IF` and `ID` stages
 * of the pipeline and sending a bubble to the `EX` stage.
 *
 * @param reg_num the register number to handle
 * @param dbuf dbuf to modify when sending a bubble
 * @note sets errno to be EINVAL if reg_num > 7 or if the register is not busy
 */
static void handle_busy_register(uint16_t reg_num, dbuf_t *dbuf);

fbuf_t get_fbuf(void) {
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

void send_bubble_to_id(void) {
    pthread_mutex_lock(&vm.fbuf_nop_mutex);
    vm.fbuf_nop = 1;
    pthread_mutex_unlock(&vm.fbuf_nop_mutex);
}

void send_bubble_to_ex(void) {
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
    int opcode = get_opcode(fbuf.ir);

    if (fbuf.nop || !dbuf || (opcode != OP_ADD && opcode != OP_AND)) {
        errno = EINVAL;
        return;
    }

    dbuf->reg = bit_range(fbuf.ir, 9, 11);

    int sr1 = bit_range(fbuf.ir, 6, 8);

    handle_busy_register((uint16_t)sr1, dbuf);
    if (!errno) {
        return;
    }
    errno = 0;

    dbuf->operand1 = get_register_data((uint16_t)sr1);

    if (!bit_range(fbuf.ir, 5, 5)) {
        int sr2 = bit_range(fbuf.ir, 0, 2);

        handle_busy_register((uint16_t)sr2, dbuf);
        if (!errno) {
            return;
        }
        errno = 0;

        dbuf->operand2 = get_register_data((uint16_t)sr2);
    } else {
        dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 4), 5);
    }
}

void decode_ld_ldi_lea(fbuf_t fbuf, dbuf_t *dbuf) {
    int opcode = get_opcode(fbuf.ir);

    if (fbuf.nop || !dbuf ||
        (opcode != OP_LD && opcode != OP_LDI && opcode != OP_LEA)) {
        errno = EINVAL;
        return;
    }

    dbuf->reg = bit_range(fbuf.ir, 9, 11);
    dbuf->operand1 = (int16_t)fbuf.pc;
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
}

void decode_st_sti(fbuf_t fbuf, dbuf_t *dbuf) {
    int opcode = get_opcode(fbuf.ir);

    if (fbuf.nop || !dbuf || (opcode != OP_ST && opcode != OP_STI)) {
        errno = EINVAL;
        return;
    }

    int sr = bit_range(fbuf.ir, 9, 11);

    handle_busy_register((uint16_t)sr, dbuf);
    if (!errno) {
        return;
    }
    errno = 0;

    dbuf->reg = get_register_data((uint16_t)sr);
    dbuf->operand1 = (int16_t)fbuf.pc;
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 8), 9);
}

void decode_jsr(fbuf_t fbuf, dbuf_t *dbuf) {
    int opcode = get_opcode(fbuf.ir);

    if (fbuf.nop || !dbuf || opcode != OP_JSR) {
        errno = EINVAL;
        return;
    }

    dbuf->operand1 = (int16_t)fbuf.pc;
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 10), 11);
    dbuf->bit11 = 1;
}

void decode_jmp_jsrr(fbuf_t fbuf, dbuf_t *dbuf) {
    int opcode = get_opcode(fbuf.ir);

    if (fbuf.nop || !dbuf || (opcode != OP_JMP && opcode != OP_JSRR)) {
        errno = EINVAL;
        return;
    }

    int base_r = bit_range(fbuf.ir, 6, 8);

    handle_busy_register((uint16_t)base_r, dbuf);
    if (!errno) {
        return;
    }
    errno = 0;

    dbuf->reg = get_register_data((uint16_t)base_r);
    dbuf->bit11 = 0;
}

void decode_ldr(fbuf_t fbuf, dbuf_t *dbuf) {
    int opcode = get_opcode(fbuf.ir);

    if (fbuf.nop || !dbuf || opcode != OP_LDR) {
        errno = EINVAL;
        return;
    }

    int base_r = bit_range(fbuf.ir, 6, 8);

    dbuf->reg = bit_range(fbuf.ir, 9, 11);

    handle_busy_register((uint16_t)base_r, dbuf);
    if (!errno) {
        return;
    }
    errno = 0;

    dbuf->operand1 = get_register_data((uint16_t)base_r);
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 5), 6);
}

void decode_str(fbuf_t fbuf, dbuf_t *dbuf) {
    int opcode = get_opcode(fbuf.ir);

    if (fbuf.nop || !dbuf || opcode != OP_STR) {
        errno = EINVAL;
        return;
    }

    int sr = bit_range(fbuf.ir, 9, 11);
    int base_r = bit_range(fbuf.ir, 6, 8);

    handle_busy_register((uint16_t)sr, dbuf);
    if (!errno) {
        return;
    }
    errno = 0;

    dbuf->reg = get_register_data((uint16_t)sr);

    handle_busy_register((uint16_t)base_r, dbuf);
    if (!errno) {
        return;
    }
    errno = 0;

    dbuf->operand1 = get_register_data((uint16_t)base_r);
    dbuf->operand2 = sign_extend(bit_range(fbuf.ir, 0, 5), 6);
}

void decode_not(fbuf_t fbuf, dbuf_t *dbuf) {
    int opcode = get_opcode(fbuf.ir);

    if (fbuf.nop || !dbuf || opcode != OP_NOT) {
        errno = EINVAL;
        return;
    }

    int sr = bit_range(fbuf.ir, 6, 8);

    dbuf->reg = bit_range(fbuf.ir, 9, 11);

    handle_busy_register((uint16_t)sr, dbuf);
    if (!errno) {
        return;
    }
    errno = 0;

    dbuf->operand1 = get_register_data((uint16_t)sr);
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
    pthread_mutex_lock(&vm.register_file_mutex);
    vm.register_file[register_num].busy_counter++;
    pthread_mutex_unlock(&vm.register_file_mutex);
}

void update_dbuf(dbuf_t *dbuf) {
    pthread_mutex_lock(&vm.ex_stay_mutex);

    if (!vm.ex_stay) {
        set_dbuf(dbuf);
    } else {
        vm.ex_stay = 0;
    }

    pthread_mutex_unlock(&vm.ex_stay_mutex);
}

static void set_dbuf(dbuf_t *dbuf) {
    pthread_mutex_lock(&vm.dbuf_mutex);
    memcpy(&vm.dbuf, dbuf, sizeof(dbuf_t));
    pthread_mutex_unlock(&vm.dbuf_mutex);
}

static void handle_busy_register(uint16_t reg_num, dbuf_t *dbuf) {
    pthread_mutex_lock(&vm.register_file_mutex);
    if (reg_num > 7 || !vm.register_file[reg_num].busy_counter) {
        errno = EINVAL;
        pthread_mutex_unlock(&vm.register_file_mutex);
        return;
    }
    pthread_mutex_unlock(&vm.register_file_mutex);

    send_stay_to_id();
    // send bubble to EX
    dbuf->nop = 1;
}
