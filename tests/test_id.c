#include "test_id.h"
#include <pthread.h>
#include <stdio.h>

#include "../src/id.h"
#include "../src/vm.h"

// void test_id_exec_cycle(int *passed_tests, int *total_tests) {
//     test_id_add_register_exec_cycle(passed_tests, total_tests);
//     test_id_add_immediate_exec_cycle(passed_tests, total_tests);
//     test_id_and_register_exec_cycle(passed_tests, total_tests);
//     test_id_and_immediate_exec_cycle(passed_tests, total_tests);
//     test_id_not_exec_cycle(passed_tests, total_tests);
//     test_id_br_exec_cycle(passed_tests, total_tests);
//     test_id_jmp_exec_cycle(passed_tests, total_tests);
//     test_id_jsr_exec_cycle(passed_tests, total_tests);
//     test_id_jsrr_exec_cycle(passed_tests, total_tests);
//     test_id_ld_exec_cycle(passed_tests, total_tests);
//     test_id_ldi_exec_cycle(passed_tests, total_tests);
//     test_id_ldr_exec_cycle(passed_tests, total_tests);
//     test_id_lea_exec_cycle(passed_tests, total_tests);
//     test_id_st_exec_cycle(passed_tests, total_tests);
//     test_id_sti_exec_cycle(passed_tests, total_tests);
//     test_id_str_exec_cycle(passed_tests, total_tests);
//     test_id_trap_exec_cycle(passed_tests, total_tests);
// }

// // ADD (register mode)
// void test_id_add_register_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 7;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = 0x1401; // ADD R2 = R0 + R1
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_ADD) {
//         fprintf(stderr, "expected OP_ADD, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.dr != 2) {
//         fprintf(stderr, "expected DR=2, got %d\n", dbuf.dr);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.sr1 != 0) {
//         fprintf(stderr, "expected SR1=0, got %d\n", dbuf.sr1);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.sr2 != 1) {
//         fprintf(stderr, "expected SR2=1, got %d\n", dbuf.sr2);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pc != 0x3001) {
//         fprintf(stderr, "expected PC=0x3001, got %x\n", dbuf.pc);
//         return;
//     }
//     (*passed_tests)++;
// }

// // ADD (immediate mode)
// void test_id_add_immediate_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 7;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = 0x143D; // ADD R2 = R0 + imm5(-3)
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_ADD) {
//         fprintf(stderr, "expected OP_ADD, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.dr != 2) {
//         fprintf(stderr, "expected DR=2, got %d\n", dbuf.dr);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.sr1 != 0) {
//         fprintf(stderr, "expected SR1=0, got %d\n", dbuf.sr1);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.imm5 != -3) {
//         fprintf(stderr, "expected IMM5=-3, got %d\n", dbuf.imm5);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pc != 0x3001) {
//         fprintf(stderr, "expected PC=0x3001, got %x\n", dbuf.pc);
//         return;
//     }
//     (*passed_tests)++;
// }

// // AND (register mode)
// void test_id_and_register_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 7;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = 0x5401; // AND R2 = R0 & R1
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_AND) {
//         fprintf(stderr, "expected OP_AND, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.dr != 2) {
//         fprintf(stderr, "expected DR=2, got %d\n", dbuf.dr);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.sr1 != 0) {
//         fprintf(stderr, "expected SR1=0, got %d\n", dbuf.sr1);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.sr2 != 1) {
//         fprintf(stderr, "expected SR2=1, got %d\n", dbuf.sr2);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pc != 0x3001) {
//         fprintf(stderr, "expected PC=0x3001, got %x\n", dbuf.pc);
//         return;
//     }
//     (*passed_tests)++;
// }

// // AND (immediate mode)
// void test_id_and_immediate_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 7;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = 0x543D; // AND R2 = R0 & imm5(-3)
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_AND) {
//         fprintf(stderr, "expected OP_AND, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.dr != 2) {
//         fprintf(stderr, "expected DR=2, got %d\n", dbuf.dr);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.sr1 != 0) {
//         fprintf(stderr, "expected SR1=0, got %d\n", dbuf.sr1);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.imm5 != -3) {
//         fprintf(stderr, "expected IMM5=-3, got %d\n", dbuf.imm5);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pc != 0x3001) {
//         fprintf(stderr, "expected PC=0x3001, got %x\n", dbuf.pc);
//         return;
//     }
//     (*passed_tests)++;
// }

// // NOT
// void test_id_not_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 6;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = (int16_t)0x973F; // NOT R3 = NOT R4
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_NOT) {
//         fprintf(stderr, "expected OP_NOT, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.dr != 3) {
//         fprintf(stderr, "expected DR=3, got %d\n", dbuf.dr);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.sr1 != 4) {
//         fprintf(stderr, "expected SR1=4, got %d\n", dbuf.sr1);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pc != 0x3001) {
//         fprintf(stderr, "expected PC=0x3001, got %x\n", dbuf.pc);
//         return;
//     }
//     (*passed_tests)++;
// }

// // BR
// void test_id_br_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 5;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = 0xD01; // BRnz, PCoffset9=1
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_BR) {
//         fprintf(stderr, "expected OP_BR, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.nzp != 0x6) {
//         fprintf(stderr, "expected NZP=110, got %d\n", dbuf.nzp);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pcoffset9 != -255) {
//         fprintf(stderr, "expected offset9=-255, got %d\n", dbuf.pcoffset9);
//         return;
//     }
//     (*passed_tests)++;
// }

// // JMP / RET
// void test_id_jmp_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 4;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = (int16_t)0xC1C0; // JMP base_r=1
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_JMP) {
//         fprintf(stderr, "expected OP_JMP, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.base_r != 7) {
//         fprintf(stderr, "expected base_r=7, got %d\n", dbuf.base_r);
//         return;
//     }
//     (*passed_tests)++;
// }

// // JSR (imm)
// void test_id_jsr_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 4;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = 0x481F; // JSR, PCoffset11=31
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_JSR) {
//         fprintf(stderr, "expected OP_JSR, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pcoffset11 != 31) {
//         fprintf(stderr, "expected offset11=31, got %d\n", dbuf.pcoffset11);
//         return;
//     }
//     (*passed_tests)++;
// }

// // JSRR
// void test_id_jsrr_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 4;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = 0x4180; // JSRR base_r=1
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_JSR) {
//         fprintf(stderr, "expected OP_JSRR, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.base_r != 6) {
//         fprintf(stderr, "expected base_r=6, got %d\n", dbuf.base_r);
//         return;
//     }
//     (*passed_tests)++;
// }

// // LD
// void test_id_ld_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 5;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = 0x2A00; // LD DR=5, PCoffset9=0
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_LD) {
//         fprintf(stderr, "expected OP_LD, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.dr != 5) {
//         fprintf(stderr, "expected DR=5, got %d\n", dbuf.dr);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pcoffset9 != 0) {
//         fprintf(stderr, "expected offset9=0, got %d\n", dbuf.pcoffset9);
//         return;
//     }
//     (*passed_tests)++;
// }

// // LDI
// void test_id_ldi_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 5;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = (int16_t)0xA61E; // LDI DR=3, PCoffset9=30
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_LDI) {
//         fprintf(stderr, "expected OP_LDI, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.dr != 3) {
//         fprintf(stderr, "expected DR=3, got %d\n", dbuf.dr);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pcoffset9 != 30) {
//         fprintf(stderr, "expected offset9=30, got %d\n", dbuf.pcoffset9);
//         return;
//     }
//     (*passed_tests)++;
// }

// // LDR
// void test_id_ldr_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 6;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = 0x6801; // LDR DR=4, base_r=0, offset6=1
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_LDR) {
//         fprintf(stderr, "expected OP_LDR, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.dr != 4) {
//         fprintf(stderr, "expected DR=4, got %d\n", dbuf.dr);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.base_r != 0) {
//         fprintf(stderr, "expected base_r=0, got %d\n", dbuf.base_r);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.offset6 != 1) {
//         fprintf(stderr, "expected offset6=1, got %d\n", dbuf.offset6);
//         return;
//     }
//     (*passed_tests)++;
// }

// // LEA
// void test_id_lea_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 5;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = (int16_t)0xE003; // LEA DR=0, PCoffset9=3
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_LEA) {
//         fprintf(stderr, "expected OP_LEA, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.dr != 0) {
//         fprintf(stderr, "expected DR=0, got %d\n", dbuf.dr);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pcoffset9 != 3) {
//         fprintf(stderr, "expected offset9=3, got %d\n", dbuf.pcoffset9);
//         return;
//     }
//     (*passed_tests)++;
// }

// // ST
// void test_id_st_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 5;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = (int16_t)0x340F; // ST SR=2, PCoffset9=15
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_ST) {
//         fprintf(stderr, "expected OP_ST, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.sr1 != 2) {
//         fprintf(stderr, "expected SR=2, got %d\n", dbuf.sr1);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pcoffset9 != 15) {
//         fprintf(stderr, "expected offset9=15, got %d\n", dbuf.pcoffset9);
//         return;
//     }
//     (*passed_tests)++;
// }

// // STI
// void test_id_sti_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 5;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = (int16_t)0xB46A; // STI SR=2, PCoffset9=106
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_STI) {
//         fprintf(stderr, "expected OP_STI, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.sr1 != 2) {
//         fprintf(stderr, "expected SR=2, got %d\n", dbuf.sr1);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.pcoffset9 != 106) {
//         fprintf(stderr, "expected offset9=106, got %d\n", dbuf.pcoffset9);
//         return;
//     }
//     (*passed_tests)++;
// }

// // STR
// void test_id_str_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 6;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = (int16_t)0x7900; // STR SR=4, base_r=4, offset6=0
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_STR) {
//         fprintf(stderr, "expected OP_STR, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.sr1 != 4) {
//         fprintf(stderr, "expected SR=4, got %d\n", dbuf.sr1);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.base_r != 4) {
//         fprintf(stderr, "expected base_r=4, got %d\n", dbuf.base_r);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.offset6 != 0) {
//         fprintf(stderr, "expected offset6=0, got %d\n", dbuf.offset6);
//         return;
//     }
//     (*passed_tests)++;
// }

// // TRAP
// void test_id_trap_exec_cycle(int *passed_tests, int *total_tests) {
//     *total_tests += 4;
//     vm.fbuf.pc = 0x3001;
//     vm.fbuf.ir = (int16_t)0xF025; // TRAP vect8=0x25
//     pthread_t tid;
//     dbuf_t dbuf;

//     pthread_create(&tid, NULL, id_exec_cycle, &dbuf);
//     (*passed_tests)++;
//     pthread_join(tid, NULL);
//     (*passed_tests)++;
//     if (dbuf.opcode != OP_TRAP) {
//         fprintf(stderr, "expected OP_TRAP, got %d\n", dbuf.opcode);
//         return;
//     }
//     (*passed_tests)++;
//     if (dbuf.trapvect8 != 0x25) {
//         fprintf(stderr, "expected trapvect8=0x25, got %d\n", dbuf.trapvect8);
//         return;
//     }
//     (*passed_tests)++;
// }
