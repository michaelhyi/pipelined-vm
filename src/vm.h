#ifndef VM_H
#define VM_H

#include <stdint.h>

#include "./stages/ex.h"
#include "./stages/id.h"
#include "./stages/if.h"
#include "./stages/mem.h"

#define ADDRESS_SPACE (1 << 16)
#define NUM_REGISTERS (8)

// TODO: add mutex locks
typedef struct vm_t {
    int16_t mem[ADDRESS_SPACE];
    int16_t reg[NUM_REGISTERS];
    int16_t pc;
    int16_t cc;
    int16_t psr;

    fbuf_t fbuf;
    dbuf_t dbuf;
    ebuf_t ebuf;
    mbuf_t mbuf;
} vm_t;

extern vm_t vm;

/**
 * Initializes the virtual machine.
 */
void system_init(void);

#endif
