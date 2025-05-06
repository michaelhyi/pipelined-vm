#ifndef VM_H
#define VM_H

#include <pthread.h>
#include <stdint.h>

#define ADDRESS_SPACE (1 << 16)
#define NUM_REGISTERS 8
#define NUM_PIPELINE_STAGES 5

typedef struct fbuf_t {
    uint16_t ready;

    uint16_t pc;
    int16_t ir;
} fbuf_t;

typedef struct dbuf_t {
    uint16_t ready;

    uint16_t pc;
    int16_t opcode;

    int16_t reg;
    int16_t operand1;
    int16_t operand2;
    int16_t cc;
} dbuf_t;

typedef struct ebuf_t {
    uint16_t ready;

    uint16_t pc;
    int16_t opcode;
    int16_t result;
    int16_t reg;
} ebuf_t;

typedef struct mbuf_t {
    uint16_t ready;

    uint16_t pc;
    int16_t opcode;
    int16_t result;
    int16_t reg;
} mbuf_t;

typedef struct vm_t {
    int16_t mem[ADDRESS_SPACE];
    int16_t reg[NUM_REGISTERS];
    uint16_t pc;
    int16_t cc;
    int16_t psr;

    fbuf_t fbuf;
    dbuf_t dbuf;
    ebuf_t ebuf;
    mbuf_t mbuf;

    pthread_mutex_t mem_mutex;
    pthread_mutex_t reg_mutex;
    pthread_mutex_t pc_mutex;
    pthread_mutex_t cc_mutex;
    pthread_mutex_t psr_mutex;

    pthread_mutex_t fbuf_mutex;
    pthread_mutex_t dbuf_mutex;
    pthread_mutex_t ebuf_mutex;
    pthread_mutex_t mbuf_mutex;

    pthread_barrier_t pipeline_cycle_barrier;
} vm_t;

extern vm_t vm;

/**
 * Initializes the virtual machine.
 */
void vm_init(void);

/**
 * Runs the virtual machine.
 */
void vm_run(void);

/**
 * Tears down the virtual machine.
 */
void vm_teardown(void);

#endif
