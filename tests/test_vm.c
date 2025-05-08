#include "test_vm.h"

#include <assert.h>
#include <errno.h>

#include "../src/isa.h"
#include "../src/util/bitops.h"
#include "../src/vm.h"

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

void test_vm() {
    // TODO: test br, rti, trap
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
}

static void test_add_reg_mode() {
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
    int16_t actual_cc = vm.cc;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_add_imm_mode() {
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
    int16_t actual_cc = vm.cc;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_ld() {
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
    int16_t actual_cc = 1;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_st() {
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

static void test_jsr() {
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

static void test_jsrr() {
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

static void test_and_reg_mode() {
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
    int16_t actual_cc = vm.cc;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_and_imm_mode() {
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
    int16_t actual_cc = vm.cc;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_ldr() {
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
    int16_t actual_cc = vm.cc;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_str() {
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

static void test_not() {
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
    int16_t actual_cc = vm.cc;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_ldi() {
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
    int16_t actual_cc = 1;
    assert(expected_cc == actual_cc);

    // teardown
    vm_teardown();

    assert(!errno);
}

static void test_sti() {
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

static void test_jmp() {
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

static void test_lea() {
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
