#ifndef VM_H
#define VM_H

#include <stdint.h>

#define ADDRESS_SPACE (1 << 16)
#define NUM_REGISTERS 8

typedef struct fbuf_t {
    int16_t ready;

    int16_t pc;
    int16_t ir;
} fbuf_t;

typedef struct dbuf_t {
    int16_t ready;

    int16_t pc;
    int16_t opcode;

    int16_t reg;
    int16_t operand1;
    int16_t operand2;

    int16_t cc;
} dbuf_t;

typedef struct ebuf_t {
    int16_t ready;

    int16_t pc;
    int16_t opcode;
    int16_t result;
    int16_t reg;
} ebuf_t;

typedef struct mbuf_t {
    int16_t ready;

    int16_t pc;
    int16_t opcode;
    int16_t result;
    int16_t reg;
} mbuf_t;

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
