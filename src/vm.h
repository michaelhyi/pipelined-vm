#ifndef VM_H
#define VM_H

#include <stdint.h>

#include "ex.h"
#include "id.h"
#include "if.h"

#define ADDRESS_SPACE (1 << 16)
#define NUM_GENERAL_PURPOSE_REGISTERS (8)

// TODO: add mutex locks
typedef struct vm_t {
    int16_t mem[ADDRESS_SPACE];
    int16_t reg[NUM_GENERAL_PURPOSE_REGISTERS];
    int16_t pc;
    int16_t ir;
    int16_t cc;
    int16_t psr;

    fbuf_t fbuf;
    dbuf_t dbuf;
    ebuf_t ebuf;
} vm_t;

extern vm_t vm;

void system_init(void);

#endif
