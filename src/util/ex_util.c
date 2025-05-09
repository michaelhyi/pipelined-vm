#include "ex_util.h"

#include <stdlib.h>
#include <string.h>

#include "../isa.h"
#include "../vm_util.h"

ebuf_t init_ebuf(dbuf_t dbuf) {
    ebuf_t ebuf;

    ebuf.nop = dbuf.nop;
    ebuf.pc = dbuf.pc;
    ebuf.opcode = dbuf.opcode;
    ebuf.reg = dbuf.reg;
    ebuf.indirect_counter = 0;

    return ebuf;
}

inline int instruction_needs_add(int16_t opcode) {
    return opcode == OP_ADD || opcode == OP_LD || opcode == OP_ST ||
           opcode == OP_LDR || opcode == OP_STR || opcode == OP_LDI ||
           opcode == OP_STI || opcode == OP_LEA;
}

inline int ex_instruction_is_jsr(dbuf_t dbuf) {
    return dbuf.opcode == OP_JSR && dbuf.bit11;
}

inline int ex_instruction_is_jsrr(dbuf_t dbuf) {
    return dbuf.opcode == OP_JSRR && !dbuf.bit11;
}

void update_ebuf(ebuf_t *ebuf) {
    pthread_mutex_lock(&vm.mem_stay_mutex);

    if (!vm.mem_stay) {
        set_ebuf(*ebuf);
    } else {
        vm.mem_stay = 0;
    }

    pthread_mutex_unlock(&vm.mem_stay_mutex);
}
