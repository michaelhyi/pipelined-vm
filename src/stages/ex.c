#include "ex.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include "../isa.h"
#include "../vm.h"

void *ex_run(void *arg) {
    (void)arg;

    pthread_mutex_lock(&vm.dbuf_mutex);
    dbuf_t dbuf = vm.dbuf;
    pthread_mutex_unlock(&vm.dbuf_mutex);

    ebuf_t ebuf;
    ebuf.nop = dbuf.nop;

    if (dbuf.nop) {
        pthread_barrier_wait(&vm.pipeline_cycle_barrier);
        pthread_mutex_lock(&vm.ebuf_mutex);
        vm.ebuf = ebuf;
        pthread_mutex_unlock(&vm.ebuf_mutex);

        return NULL;
    }

    ebuf.pc = dbuf.pc;
    ebuf.opcode = dbuf.opcode;
    ebuf.reg = dbuf.reg;

    if (dbuf.opcode == OP_BR) {
        // TODO
    } else if (dbuf.opcode == OP_ADD || dbuf.opcode == OP_LD ||
               dbuf.opcode == OP_ST || dbuf.opcode == OP_LDR ||
               dbuf.opcode == OP_STR || dbuf.opcode == OP_LDI ||
               dbuf.opcode == OP_STI || dbuf.opcode == OP_LEA) {
        ebuf.result = dbuf.operand1 + dbuf.operand2;
    } else if (dbuf.opcode == OP_JSR || dbuf.opcode == OP_JSRR) {
        // TODO
    } else if (dbuf.opcode == OP_AND) {
        ebuf.result = dbuf.operand1 & dbuf.operand2;
    } else if (dbuf.opcode == OP_NOT) {
        ebuf.result = ~dbuf.operand1;
    } else if (dbuf.opcode == OP_JMP) {
        // TODO
    } else if (dbuf.opcode == OP_TRAP) {
        // TODO
    }

    pthread_barrier_wait(&vm.pipeline_cycle_barrier);
    pthread_mutex_lock(&vm.ebuf_mutex);
    vm.ebuf = ebuf;
    pthread_mutex_unlock(&vm.ebuf_mutex);

    return NULL;
}
