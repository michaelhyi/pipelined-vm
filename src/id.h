#ifndef ID_H
#define ID_H

#include <stdint.h>

#define OP_BR 0x0       // Branch
#define OP_ADD 0x1      // Add (reg or imm)
#define OP_LD 0x2       // Load
#define OP_ST 0x3       // Store
#define OP_JSR 0x4      // JSR / JSRR
#define OP_AND 0x5      // And (reg or imm)
#define OP_LDR 0x6      // Load Base+offset
#define OP_STR 0x7      // Store Base+offset
#define OP_RTI 0x8      // (unused in user‐mode)
#define OP_NOT 0x9      // Not
#define OP_LDI 0xA      // Load Indirect
#define OP_STI 0xB      // Store Indirect
#define OP_JMP 0xC      // Jump (including RET)
#define OP_RESERVED 0xD // Reserved
#define OP_LEA 0xE      // Load Effective Address
#define OP_TRAP 0xF     // TRAP

typedef struct dbuf_t {
    int16_t pc;
    int opcode;

    int dr;
    int sr1;
    int sr2;
    int base_r;

    int nzp;

    int imm5;
    int offset6;
    int pcoffset9;
    int pcoffset11;

    int trapvect8;
} dbuf_t;

void *id_exec_cycle(void *arg);

#endif
