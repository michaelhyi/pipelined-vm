#include "ex.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include "../vm.h"

void *ex_run(void *arg) {
    if (!arg) {
        fprintf(stderr, "ex_run failed: arg must be non-null\n");
        pthread_exit((void *)(intptr_t)EINVAL);
    }

    dbuf_t dbuf = vm.dbuf;

    if (!dbuf.ready) {
        return NULL;
    }

    ebuf_t *ebuf = (ebuf_t *)arg;
    ebuf->ready = 1;
    ebuf->pc = dbuf.pc;
    ebuf->opcode = dbuf.opcode;
    ebuf->reg = dbuf.reg;

    if (ebuf->opcode == OP_ADD || ebuf->opcode == OP_LDR ||
        ebuf->opcode == OP_STR) {
        ebuf->result = dbuf.operand1 + dbuf.operand2;
    } else if (ebuf->opcode == OP_AND) {
        ebuf->result = dbuf.operand1 & dbuf.operand2;
    } else if (ebuf->opcode == OP_NOT) {
        ebuf->result = ~dbuf.operand1;
    } else if (ebuf->opcode == OP_BR) {
        // TODO: handle branch prediction
    } else if (ebuf->opcode == OP_JMP) {
        // TODO: flush younger stages in pipeline, override PC
        // TODO: handle JSR(R)
    } else if (ebuf->opcode == OP_LD || ebuf->opcode == OP_LDI ||
               ebuf->opcode == OP_LEA || ebuf->opcode == OP_ST ||
               ebuf->opcode == OP_STI) {
        ebuf->result = dbuf.pc + dbuf.operand1;
    } else if (ebuf->opcode == OP_TRAP) {
        ebuf->result = dbuf.operand1;
    }

    return NULL;
}
