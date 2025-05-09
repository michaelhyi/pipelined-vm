#include "id.h"

#include <errno.h>
#include <pthread.h>

#include "../isa.h"
#include "../util/bitops.h"
#include "../util/id_util.h"
#include "../vm.h"
#include "../vm_util.h"

/**
 * A function that runs at the end of every `ID` stage to tear it down.
 *
 * @param next_dbuf the next dbuf that the VM will use
 */
static void id_teardown(dbuf_t next_dbuf);

void *id_run(void *arg) {
    (void)arg;

    fbuf_t fbuf = get_fbuf();

    if (fbuf.nop) {
        send_bubble_to_ex(); // TODO: bug on this line.
        pthread_barrier_wait(&vm.pipeline_cycle_barrier);
        id_teardown((dbuf_t){.nop = 1});
        return NULL;
    }

    dbuf_t dbuf = init_dbuf(fbuf);

    if (dbuf.opcode == OP_BR) {
        decode_br(fbuf, &dbuf);
    } else if (dbuf.opcode == OP_ADD || dbuf.opcode == OP_AND) {
        decode_add_and(fbuf, &dbuf);

        // TODO: make thread-safe
        if (!vm.ex_nop)
            increment_busy_counter((uint16_t)dbuf.reg);
    } else if (dbuf.opcode == OP_LD || dbuf.opcode == OP_LDI ||
               dbuf.opcode == OP_LEA) {
        decode_ld_ldi_lea(fbuf, &dbuf);
        if (!vm.ex_nop)
            increment_busy_counter((uint16_t)dbuf.reg);
    } else if (dbuf.opcode == OP_ST || dbuf.opcode == OP_STI) {
        decode_st_sti(fbuf, &dbuf);
    } else if (id_instruction_is_jsr(fbuf.ir)) {
        decode_jsr(fbuf, &dbuf);
        send_bubble_to_id();
        if (!vm.ex_nop)
            increment_busy_counter((uint16_t)7);
    } else if (id_instruction_is_jsrr(fbuf.ir) || dbuf.opcode == OP_JMP) {
        decode_jmp_jsrr(fbuf, &dbuf);
        send_bubble_to_id();
        if (!vm.ex_nop)
            increment_busy_counter((uint16_t)7);
    } else if (dbuf.opcode == OP_LDR) {
        decode_ldr(fbuf, &dbuf);
        if (!vm.ex_nop)
            increment_busy_counter((uint16_t)dbuf.reg);
    } else if (dbuf.opcode == OP_STR) {
        decode_str(fbuf, &dbuf);
    } else if (dbuf.opcode == OP_NOT) {
        decode_not(fbuf, &dbuf);
        if (!vm.ex_nop)
            increment_busy_counter((uint16_t)dbuf.reg);
    } else if (dbuf.opcode == OP_TRAP) {
        decode_trap(fbuf, &dbuf);
    }

    if (errno) {
        // TODO: set errno = 0;
        pthread_exit((void *)(intptr_t)errno);
    }

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);
    id_teardown(dbuf);
    return NULL;
}

static void id_teardown(dbuf_t next_dbuf) {
    // receive any bubbles
    pthread_mutex_lock(&vm.ex_nop_mutex);
    next_dbuf.nop = vm.ex_nop;
    vm.ex_nop = 0;
    pthread_mutex_unlock(&vm.ex_nop_mutex);

    update_dbuf(next_dbuf);

    pthread_exit(0);
}
