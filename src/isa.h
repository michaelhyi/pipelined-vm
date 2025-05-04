#ifndef ISA_H
#define ISA_H

#define OP_BR 0x0       // Branch
#define OP_ADD 0x1      // Add
#define OP_LD 0x2       // Load
#define OP_ST 0x3       // Store
#define OP_JSR 0x4      // JSR
#define OP_JSRR 0x4     // JSRR
#define OP_AND 0x5      // And
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

#endif
