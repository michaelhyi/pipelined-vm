#include "id.h"

#include <errno.h>
#include <pthread.h>

#include "../isa.h"
#include "../util/bitops.h"
#include "../util/id_util.h"
#include "../util/vm_util.h"
#include "../vm.h"

/**
 * Tears down the `id` stage.
 *
 * @param next_dbuf the next dbuf that the vm will use
 */
static void id_teardown(dbuf_t next_dbuf);

void *id_run(void *arg) {
    (void)arg;

    fbuf_t fbuf = get_fbuf();

    if (fbuf.nop) {
        send_bubble_to_ex();
        id_teardown((dbuf_t){.nop = 1});
        return NULL;
    }

    dbuf_t next_dbuf = init_next_dbuf(fbuf);

    if (next_dbuf.opcode == OP_BR) {
        decode_br(fbuf, &next_dbuf);

        pthread_mutex_lock(&vm.cc_mutex);
        if (vm.cc.busy_counter) {
            send_stay_to_id();
            send_bubble_to_ex();
            pthread_mutex_unlock(&vm.cc_mutex);
            id_teardown((dbuf_t){.nop = 1});
        }
        pthread_mutex_unlock(&vm.cc_mutex);

    } else if (next_dbuf.opcode == OP_ADD || next_dbuf.opcode == OP_AND) {
        decode_add_and(fbuf, &next_dbuf);

        // TODO: make thread-safe
        if (!vm.ex_nop) {
            increment_busy_counter((uint16_t)next_dbuf.reg);
            increment_cc_busy_counter();
        }

    } else if (next_dbuf.opcode == OP_LD || next_dbuf.opcode == OP_LDI ||
               next_dbuf.opcode == OP_LEA) {
        decode_ld_ldi_lea(fbuf, &next_dbuf);
        if (!vm.ex_nop) {
            increment_busy_counter((uint16_t)next_dbuf.reg);

            if (next_dbuf.opcode != OP_LEA) {
                increment_cc_busy_counter();
            }
        }
    } else if (next_dbuf.opcode == OP_ST || next_dbuf.opcode == OP_STI) {
        decode_st_sti(fbuf, &next_dbuf);
    } else if (id_instruction_is_jsr(fbuf.ir)) {
        decode_jsr(fbuf, &next_dbuf);
        send_bubble_to_id();
        if (!vm.ex_nop)
            increment_busy_counter((uint16_t)7);
    } else if (id_instruction_is_jsrr(fbuf.ir) || next_dbuf.opcode == OP_JMP) {
        decode_jmp_jsrr(fbuf, &next_dbuf);
        send_bubble_to_id();
        if (!vm.ex_nop)
            increment_busy_counter((uint16_t)7);
    } else if (next_dbuf.opcode == OP_LDR) {
        decode_ldr(fbuf, &next_dbuf);
        if (!vm.ex_nop) {
            increment_busy_counter((uint16_t)next_dbuf.reg);
            increment_cc_busy_counter();
        }
    } else if (next_dbuf.opcode == OP_STR) {
        decode_str(fbuf, &next_dbuf);
    } else if (next_dbuf.opcode == OP_NOT) {
        decode_not(fbuf, &next_dbuf);
        if (!vm.ex_nop) {
            increment_busy_counter((uint16_t)next_dbuf.reg);
            increment_cc_busy_counter();
        }
    } else if (next_dbuf.opcode == OP_TRAP) {
        decode_trap(fbuf, &next_dbuf);
    }

    if (errno) {
        id_teardown(next_dbuf);
    }

    id_teardown(next_dbuf);
    return NULL;
}

static void id_teardown(dbuf_t next_dbuf) {
    pthread_barrier_wait(&vm.pipeline_cycle_barrier);

    if (errno) {
        int errno_cpy = errno;
        errno = 0;
        pthread_exit((void *)(intptr_t)errno_cpy);
    }

    // handle bubbles
    pthread_mutex_lock(&vm.ex_nop_mutex);
    next_dbuf.nop = vm.ex_nop;
    vm.ex_nop = 0;
    pthread_mutex_unlock(&vm.ex_nop_mutex);

    update_dbuf(next_dbuf);

    if (errno) {
        int errno_cpy = errno;
        errno = 0;
        pthread_exit((void *)(intptr_t)errno_cpy);
    }

    pthread_exit((void *)(intptr_t)0);
}
