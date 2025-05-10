/**
 * All operations interfacing with the VM can be assumed to be thread-safe, even
 * without mutex locks, as there are no threads concurrently running before and
 * after `vm_run` is called.
 */

#include "test_vm.h"

#include <assert.h>
#include <errno.h>

#include "../src/isa.h"
#include "../src/util/bitops.h"
#include "../src/vm.h"

/**
 * Tests the `br` instruction, but the case that it is not taken.
 */
static void test_branch_not_taken(void);

/**
 * Tests the `br` instruction, but the case that it is taken.
 */
static void test_branch_taken(void);

/**
 * Tests the ADD instruction in register mode.
 */
static void test_add_reg_mode(void);

/**
 * Tests the ADD instruction in immediate mode.
 */
static void test_add_imm_mode(void);

/**
 * Tests the LD instruction.
 */
static void test_ld(void);

/**
 * Tests the ST instruction.
 */
static void test_st(void);

/**
 * Tests the JSR instruction.
 */
static void test_jsr(void);

/**
 * Tests the JSRR instruction.
 */
static void test_jsrr(void);

/**
 * Tests the AND instruction in register mode.
 */
static void test_and_reg_mode(void);

/**
 * Tests the AND instruction in immediate mode.
 */
static void test_and_imm_mode(void);

/**
 * Tests the LDR instruction.
 */
static void test_ldr(void);

/**
 * Tests the STR instruction.
 */
static void test_str(void);

/**
 * Tests the NOT instruction.
 */
static void test_not(void);

/**
 * Tests the LDI instruction.
 */
static void test_ldi(void);

/**
 * Tests the STI instruction.
 */
static void test_sti(void);

/**
 * Tests the JMP instruction.
 */
static void test_jmp(void);

/**
 * Tests the LEA instruction.
 */
static void test_lea(void);

/**
 * Tests the handling of RAW hazards with the ADD instruction.
 */
static void test_add_raw_hazard(void);

/**
 * Tests the handling of RAW hazards with the ST instruction.
 */
static void test_st_raw_hazard(void);

/**
 * Tests the handling of RAW hazards with the JSRR instruction.
 */
static void test_jsrr_raw_hazard(void);

/**
 * Tests the handling of RAW hazards with the AND instruction.
 */
static void test_and_raw_hazard(void);

/**
 * Tests the handling of RAW hazards with the LDR instruction.
 */
static void test_ldr_raw_hazard(void);

/**
 * Tests the handling of RAW hazards with the STR instruction.
 */
static void test_str_raw_hazard(void);

/**
 * Tests the handling of RAW hazards with the NOT instruction.
 */
static void test_not_raw_hazard(void);

/**
 * Tests the handling of RAW hazards with the STI instruction.
 */
static void test_sti_raw_hazard(void);

/**
 * Tests the handling of RAW hazards with the JMP instruction.
 */
static void test_jmp_raw_hazard(void);

// TODO: assert cc in all tests

void test_vm(void) {
    // TODO: test rti, trap
    test_branch_not_taken();
    test_branch_taken();
    test_add_reg_mode();
    test_add_imm_mode();
    test_ld();
    test_st();
    test_jsr();
    test_jsrr();
    test_and_reg_mode();
    test_and_imm_mode();
    test_ldr();
    test_str();
    test_not();
    test_ldi();
    test_sti();
    test_jmp();
    test_lea();

    test_add_raw_hazard();
    test_st_raw_hazard();
    test_jsrr_raw_hazard();
    test_and_raw_hazard();
    test_ldr_raw_hazard();
    test_str_raw_hazard();
    test_not_raw_hazard();
    test_sti_raw_hazard();
    test_jmp_raw_hazard();
}

static void test_branch_not_taken(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    vm.mem[0x3000] = (int16_t)((OP_BR << 12) | (1 << 9) | 1); // brp 1
    vm.mem[0x3001] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3002] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt

    vm_run();

    // then
    int16_t expected_r2 = 0;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    int16_t expected_cc = 0; // 000
    int16_t actual_cc = vm.cc.data;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_branch_taken(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    // TODO: need CC busy bit
    vm.mem[0x3000] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1);  // add r2, r0, r1
    vm.mem[0x3001] = (int16_t)((OP_BR << 12) | (1 << 9) | 1); // brp 1
    vm.mem[0x3002] = (int16_t)((OP_ADD << 12) | (2 << 9) | (2 << 6) | (1 << 5) |
                               bit_range(-8, 0, 4));    // add r2, r2, -8
    vm.mem[0x3003] = (int16_t)((OP_TRAP << 12) | 0x25); // halt

    vm_run();

    // then
    int16_t expected_r2 = 6;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    int16_t expected_cc = 1; // 001
    int16_t actual_cc = vm.cc.data;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_add_reg_mode(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    vm.mem[0x3000] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt

    vm_run();

    // then
    int16_t expected_r2 = 6;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    int16_t expected_cc = 1; // 001
    int16_t actual_cc = vm.cc.data;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_add_imm_mode(void) {
    // setup
    vm_init();

    // when
    vm.register_file[6].data = (int16_t)0xF000;
    vm.mem[0x3000] = (int16_t)((OP_ADD << 12) | (6 << 9) | (6 << 6) | (1 << 5) |
                               (bit_range(-1, 0, 4)));  // add r6, r6, -1
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25); // halt

    vm_run();

    // then
    int16_t expected_r6 = (int16_t)0xEFFF;
    int16_t actual_r6 = vm.register_file[6].data;
    assert(expected_r6 == actual_r6);

    int16_t expected_cc = 4; // 100
    int16_t actual_cc = vm.cc.data;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_ld(void) {
    // setup
    vm_init();

    // when
    vm.mem[0x3000] = (int16_t)((OP_LD << 12) | (0 << 9) | 1); // ld r0, 1
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);       // halt
    vm.mem[0x3002] = 0x2110;

    vm_run();

    // then
    int16_t expected_r0 = 0x2110;
    int16_t actual_r0 = vm.register_file[0].data;
    assert(expected_r0 == actual_r0);

    int16_t expected_cc = 1; // 001
    int16_t actual_cc = vm.cc.data;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_st(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 0x2110;
    vm.mem[0x3000] = (int16_t)((OP_ST << 12) | (0 << 9) | 1); // st r0, 1
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);       // halt

    vm_run();

    // then
    int16_t expected_mem = 0x2110;
    int16_t actual_mem = vm.mem[0x3002];
    assert(expected_mem == actual_mem);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_jsr(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    vm.mem[0x3000] = (int16_t)((OP_JSR << 12) | (1 << 11) | 1); // jsr 1
    vm.mem[0x3001] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3002] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt

    vm_run();

    // then
    int16_t expected_r2 = 0;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    int16_t expected_r7 = 0x3001;
    int16_t actual_r7 = vm.register_file[7].data;
    assert(expected_r7 == actual_r7);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_jsrr(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    vm.register_file[3].data = 0x3002;
    vm.mem[0x3000] = (int16_t)((OP_JSRR << 12) | (3 << 6)); // jsrr r3
    vm.mem[0x3001] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3002] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt

    vm_run();

    // then
    int16_t expected_r2 = 0;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    int16_t expected_r7 = 0x3001;
    int16_t actual_r7 = vm.register_file[7].data;
    assert(expected_r7 == actual_r7);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_and_reg_mode(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 3;
    vm.register_file[1].data = 6;
    vm.mem[0x3000] =
        (int16_t)((OP_AND << 12) | (2 << 9) | (0 << 6) | 1); // and r2, r0, r1
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt

    vm_run();

    // then
    int16_t expected_r2 = 2; // 011 & 110 = 010
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    int16_t expected_cc = 1; // 001
    int16_t actual_cc = vm.cc.data;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_and_imm_mode(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = (int16_t)0xF000;
    vm.mem[0x3000] = (int16_t)((OP_AND << 12) | (0 << 9) | (0 << 6) | (1 << 5) |
                               0);                      // and r6, r6, 0
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25); // halt

    vm_run();

    // then
    int16_t expected_r0 = 0;
    int16_t actual_r0 = vm.register_file[0].data;
    assert(expected_r0 == actual_r0);

    int16_t expected_cc = 2; // 010
    int16_t actual_cc = vm.cc.data;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_ldr(void) {
    // setup
    vm_init();

    // when
    vm.register_file[5].data = (int16_t)0xEFFB;
    vm.mem[0x3000] =
        (int16_t)((OP_LDR << 12) | (0 << 9) | (5 << 6) | 4); // ldr r0, r5, 4
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt
    vm.mem[0xEFFF] = 5;

    vm_run();

    // then
    int16_t expected_r0 = 5;
    int16_t actual_r0 = vm.register_file[0].data;
    assert(expected_r0 == actual_r0);

    int16_t expected_cc = 1; // 001
    int16_t actual_cc = vm.cc.data;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_str(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = (int16_t)0x2110;
    vm.register_file[5].data = (int16_t)0xEFFB;
    vm.mem[0x3000] =
        (int16_t)((OP_STR << 12) | (0 << 9) | (5 << 6) | 3); // str r0, r5, 3
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt

    vm_run();

    // then
    int16_t expected_mem = 0x2110;
    int16_t actual_mem = vm.mem[0xEFFE];
    assert(expected_mem == actual_mem);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_not(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.mem[0x3000] =
        (int16_t)((OP_NOT << 12) | (0 << 9) | (0 << 6)); // not r0, r0
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);  // halt

    vm_run();

    // then
    int16_t expected_r0 = -3;
    int16_t actual_r0 = vm.register_file[0].data;
    assert(expected_r0 == actual_r0);

    int16_t expected_cc = 4; // 100
    int16_t actual_cc = vm.cc.data;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_ldi(void) {
    // setup
    vm_init();

    // when
    vm.mem[0x3000] = (int16_t)((OP_LDI << 12) | (0 << 9) | 1); // ldi r0, 1
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);        // halt
    vm.mem[0x3002] = 0x3210;
    vm.mem[0x3210] = 0x4210;

    vm_run();

    // then
    int16_t expected_r0 = 0x4210;
    int16_t actual_r0 = vm.register_file[0].data;
    assert(expected_r0 == actual_r0);

    int16_t expected_cc = 1; // 001
    int16_t actual_cc = vm.cc.data;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_sti(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 0x4210;
    vm.mem[0x3000] = (int16_t)((OP_STI << 12) | (0 << 9) | 1); // sti r0, 1
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);        // halt
    vm.mem[0x3002] = 0x3210;

    vm_run();

    // then
    int16_t expected_mem = 0x4210;
    int16_t actual_mem = vm.mem[0x3210];
    assert(expected_mem == actual_mem);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_jmp(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    vm.register_file[7].data = 0x3002;
    vm.mem[0x3000] = (int16_t)((OP_JMP << 12) | (7 << 6)); // jmp r7
    vm.mem[0x3001] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3002] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt

    vm_run();

    // then
    int16_t expected_r2 = 0;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_lea(void) {
    // setup
    vm_init();

    // when
    vm.mem[0x3000] = (int16_t)((OP_LEA << 12) | (0 << 9) | 1); // lea r0, 1
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);        // halt

    vm_run();

    // then
    int16_t expected_r0 = 0x3002;
    int16_t actual_r0 = vm.register_file[0].data;
    assert(expected_r0 == actual_r0);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_add_raw_hazard(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    vm.mem[0x3000] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3001] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (2 << 6) | 2); // add r2, r2, r2
    vm.mem[0x3002] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt

    vm_run();

    // then
    int16_t expected_r2 = 12;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_st_raw_hazard(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    vm.mem[0x3000] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3001] = (int16_t)((OP_ADD << 12) | (6 << 9) | (6 << 6) | (1 << 5) |
                               (bit_range(-1, 0, 4)));        // add r6, r6, -1
    vm.mem[0x3002] = (int16_t)((OP_ST << 12) | (2 << 9) | 1); // st r2, 1
    vm.mem[0x3003] = (int16_t)((OP_TRAP << 12) | 0x25);       // halt

    vm_run();

    // then
    int16_t expected_mem = 6;
    int16_t actual_mem = vm.mem[0x3004];
    assert(expected_mem == actual_mem);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_jsrr_raw_hazard(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 0x3000;
    vm.register_file[1].data = 5;
    vm.mem[0x3000] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3001] = (int16_t)((OP_ADD << 12) | (6 << 9) | (6 << 6) | (1 << 5) |
                               (bit_range(-1, 0, 4)));        // add r6, r6, -1
    vm.mem[0x3002] = (int16_t)((OP_ST << 12) | (7 << 9) | 5); // st r7, 5
    vm.mem[0x3003] =
        (int16_t)((OP_JSRR << 12) | (0 << 11) | (2 << 6)); // jsrr r2;
    vm.mem[0x3004] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (2 << 6) | 2); // add r2, r2, r2
    vm.mem[0x3005] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt

    vm_run();

    // then
    int16_t expected_r2 = 0x3005;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_and_raw_hazard(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    vm.mem[0x3000] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3001] = (int16_t)((OP_ADD << 12) | (6 << 9) | (6 << 6) | (1 << 5) |
                               (bit_range(-1, 0, 4)));        // add r6, r6, -1
    vm.mem[0x3002] = (int16_t)((OP_ST << 12) | (7 << 9) | 5); // st r7, 5
    vm.mem[0x3003] = (int16_t)((OP_AND << 12) | (2 << 9) | (2 << 6) | (1 << 5) |
                               (3));                    // and r2, r2, 3
    vm.mem[0x3004] = (int16_t)((OP_TRAP << 12) | 0x25); // halt

    vm_run();

    // then
    int16_t expected_r2 = 2;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_ldr_raw_hazard(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 0x3000;
    vm.register_file[1].data = 3;
    vm.mem[0x3000] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3001] = (int16_t)((OP_ADD << 12) | (6 << 9) | (6 << 6) | (1 << 5) |
                               (bit_range(-1, 0, 4))); // add r6, r6, -1
    vm.mem[0x3002] =
        (int16_t)((OP_LDR << 12) | (2 << 9) | (2 << 6) | 1); // ldr r2, r2, 1
    vm.mem[0x3003] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt
    vm.mem[0x3004] = 0x2110;

    vm_run();

    // then
    int16_t expected_r2 = 0x2110;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_str_raw_hazard(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 0x3000;
    vm.register_file[1].data = 2;
    vm.mem[0x3000] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3001] =
        (int16_t)((OP_STR << 12) | (2 << 9) | (2 << 6) | 1); // str r2, r2, 1
    vm.mem[0x3002] = (int16_t)((OP_TRAP << 12) | 0x25);      // halt

    vm_run();

    // then
    int16_t expected_mem = 0x3002;
    int16_t actual_mem = vm.mem[0x3003];
    assert(expected_mem == actual_mem);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_not_raw_hazard(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    vm.mem[0x3000] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3001] = (int16_t)((OP_ADD << 12) | (6 << 9) | (6 << 6) | (1 << 5) |
                               (bit_range(-1, 0, 4))); // add r6, r6, -1
    vm.mem[0x3002] =
        (int16_t)((OP_NOT << 12) | (2 << 9) | (2 << 6)); // not r2, r2
    vm.mem[0x3003] = (int16_t)((OP_TRAP << 12) | 0x25);  // halt

    vm_run();

    // then
    int16_t expected_r2 = -7;
    int16_t actual_r2 = vm.register_file[2].data;
    assert(expected_r2 == actual_r2);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_sti_raw_hazard(void) {
    // setup
    vm_init();

    // when
    vm.register_file[0].data = 2;
    vm.register_file[1].data = 4;
    vm.mem[0x3000] =
        (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1); // add r2, r0, r1
    vm.mem[0x3001] = (int16_t)((OP_ADD << 12) | (6 << 9) | (6 << 6) | (1 << 5) |
                               (bit_range(-1, 0, 4))); // add r6, r6, -1
    vm.mem[0x3002] = (int16_t)((OP_ADD << 12) | (6 << 9) | (6 << 6) | (1 << 5) |
                               (bit_range(-1, 0, 4)));         // add r6, r6, -1
    vm.mem[0x3003] = (int16_t)((OP_STI << 12) | (2 << 9) | 1); // sti r2, 1
    vm.mem[0x3004] = (int16_t)((OP_TRAP << 12) | 0x25);        // halt
    vm.mem[0x3005] = 0x3100;

    vm_run();

    // then
    int16_t expected_mem = 6;
    int16_t actual_mem = vm.mem[0x3100];
    assert(expected_mem == actual_mem);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_jmp_raw_hazard(void) {
    // setup
    vm_init();

    // when
    vm.mem[0x3000] = (int16_t)((OP_JSR << 12) | (1 << 11) | 1); // jsr 1
    vm.mem[0x3001] = (int16_t)((OP_TRAP << 12) | 0x25);         // halt
    vm.mem[0x3002] = (int16_t)((OP_JMP << 12) | (7 << 6));      // jmp r7

    vm_run();

    // then
    int16_t expected_r7 = 0x3001;
    int16_t actual_r7 = vm.register_file[7].data;
    assert(expected_r7 == actual_r7);

    // teardown
    vm_teardown();

    assert(!errno);
}
