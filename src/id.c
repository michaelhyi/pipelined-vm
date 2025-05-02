#include "id.h"

#include <stddef.h>

#include "util.h"
#include "vm.h"

// TODO: for all stages, handle the case where buffers are empty. (ie first time
// running the pipeline)
void *id_exec_cycle(void *arg) {
    dbuf_t *dbuf = (dbuf_t *)arg;

    dbuf->pc = vm.fbuf.pc;
    dbuf->opcode = ((uint16_t)vm.fbuf.instruction) >> 12 & 0xF;

    switch (dbuf->opcode) {
    case 0x0: // br
        dbuf->nzp = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->pcoffset9 = sign_extend(vm.fbuf.instruction & 0x1FF, 9);
        return NULL;

    case 0x1: // add
        dbuf->dr = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->sr1 = ((uint16_t)vm.fbuf.instruction >> 6) & 0x7;

        if ((((uint16_t)vm.fbuf.instruction >> 5) & 0x1) == 0) {
            dbuf->sr2 = vm.fbuf.instruction & 0x7;
        } else {
            dbuf->imm5 = sign_extend(vm.fbuf.instruction & 0x1F, 5);
        }
        return NULL;

    case 0x2: // ld
        dbuf->dr = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->pcoffset9 = sign_extend(vm.fbuf.instruction & 0x1FF, 9);
        return NULL;

    case 0x3: // st
        dbuf->sr1 = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->pcoffset9 = sign_extend(vm.fbuf.instruction & 0x1FF, 9);
        return NULL;

    case 0x4: // jsr & jsrr
        if ((((uint16_t)vm.fbuf.instruction >> 11) & 0x1) == 0) {
            dbuf->base_r = ((uint16_t)vm.fbuf.instruction >> 6) & 0x7;
        } else {
            dbuf->pcoffset11 = sign_extend(vm.fbuf.instruction & 0x7FF, 11);
        }
        return NULL;

    case 0x5: // and
        dbuf->dr = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->sr1 = ((uint16_t)vm.fbuf.instruction >> 6) & 0x7;

        if ((((uint16_t)vm.fbuf.instruction >> 5) & 0x1) == 0) {
            dbuf->sr2 = vm.fbuf.instruction & 0x7;
        } else {
            dbuf->imm5 = sign_extend(vm.fbuf.instruction & 0x1F, 5);
        }
        return NULL;

    case 0x6: // ldr
        dbuf->dr = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->base_r = ((uint16_t)vm.fbuf.instruction >> 6) & 0x7;
        dbuf->offset6 = sign_extend(vm.fbuf.instruction & 0x3F, 6);
        return NULL;

    case 0x7: // str
        dbuf->sr1 = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->base_r = ((uint16_t)vm.fbuf.instruction >> 6) & 0x7;
        dbuf->offset6 = sign_extend(vm.fbuf.instruction & 0x3F, 6);
        return NULL;

    case 0x9: // not
        dbuf->dr = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->sr1 = ((uint16_t)vm.fbuf.instruction >> 6) & 0x7;
        return NULL;

    case 0xA: // ldi
        dbuf->dr = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->pcoffset9 = sign_extend(vm.fbuf.instruction & 0x1FF, 9);
        return NULL;

    case 0xB: // sti
        dbuf->sr1 = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->pcoffset9 = sign_extend(vm.fbuf.instruction & 0x1FF, 9);
        return NULL;

    case 0xC: // jmp
        dbuf->base_r = ((uint16_t)vm.fbuf.instruction >> 6) & 0x7;
        return NULL;

    case 0xE: // lea
        dbuf->dr = ((uint16_t)vm.fbuf.instruction >> 9) & 0x7;
        dbuf->pcoffset9 = sign_extend(vm.fbuf.instruction & 0x1FF, 9);
        return NULL;

    case 0xF: // trap
        dbuf->trapvect8 = vm.fbuf.instruction & 0xFF;
        return NULL;
    }

    return NULL;
}
