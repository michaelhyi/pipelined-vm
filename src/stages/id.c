#include "id.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include "../isa.h"
#include "../util/bitops.h"
#include "../util/id_util.h"
#include "../vm.h"

void *id_run(void *arg) {
    // TODO: test for this case
    if (!arg) {
        fprintf(stderr, "id_run failed: arg must be non-null\n");
        pthread_exit((void *)(intptr_t)EINVAL);
    }

    fbuf_t fbuf = vm.fbuf;
    if (!fbuf.ready) {
        return NULL;
    }

    dbuf_t *dbuf = (dbuf_t *)arg;
    dbuf->ready = 1;
    dbuf->pc = fbuf.pc;
    dbuf->opcode = get_opcode(fbuf.ir);

    if (dbuf->opcode == OP_ADD || dbuf->opcode == OP_AND) {
        decode_add_and(fbuf, dbuf);
    } else if (dbuf->opcode == OP_NOT) {
        decode_not(fbuf, dbuf);
    } else if (dbuf->opcode == OP_BR) {
        decode_br(fbuf, dbuf);
    } else if (dbuf->opcode == OP_JMP || is_jsrr(fbuf.ir)) {
        decode_jmp_jsrr(fbuf, dbuf);
    } else if (is_jsr(fbuf.ir)) {
        decode_jsr(fbuf, dbuf);
    } else if (dbuf->opcode == OP_LD || dbuf->opcode == OP_LDI ||
               dbuf->opcode == OP_LEA) {
        decode_ld_ldi_lea(fbuf, dbuf);
    } else if (dbuf->opcode == OP_ST || dbuf->opcode == OP_STI) {
        decode_st_sti(fbuf, dbuf);
    } else if (dbuf->opcode == OP_LDR) {
        decode_ldr(fbuf, dbuf);
    } else if (dbuf->opcode == OP_STR) {
        decode_str(fbuf, dbuf);
    } else if (dbuf->opcode == OP_TRAP) {
        decode_trap(fbuf, dbuf);
    }

    if (errno) {
        pthread_exit((void *)(intptr_t)errno);
    }

    return NULL;
}
