#include "test_id_util.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../../src/stages/id.h"
#include "../../src/util/bitops.h"
#include "../../src/util/id_util.h"
#include "../../src/vm.h"
#include "../driver.h"

/**
 * Runs unit tests on get_opcode function from "../../src/util/id_util.h".
 */
static void test_get_opcode(void);

/**
 * Runs unit tests on is_jsr function from "../../src/util/id_util.h".
 */
static void test_is_jsr(void);

/**
 * Runs unit tests on is_jsrr function from "../../src/util/id_util.h".
 */
static void test_is_jsrr(void);

/**
 * Runs unit tests on decode_add_and function from
 * "../../src/util/id_util.h".
 */
static void test_decode_add_and(void);

/**
 * Runs unit tests on decode_not function from "../../src/util/id_util.h".
 */
static void test_decode_not(void);

/**
 * Runs unit tests on decode_br function from "../../src/util/id_util.h".
 */
static void test_decode_br(void);

/**
 * Runs unit tests on decode_jmp_jsrr function from
 * "../../src/util/id_util.h".
 */
static void test_decode_jmp_jsrr(void);

/**
 * Runs unit tests on decode_jsr function from "../../src/util/id_util.h".
 */
static void test_decode_jsr(void);

/**
 * Runs unit tests on decode_ld_ldi_lea function from
 * "../../src/util/id_util.h".
 */
static void test_decode_ld_ldi_lea(void);

/**
 * Runs unit tests on decode_st_sti function from
 * "../../src/util/id_util.h".
 */
static void test_decode_st_sti(void);

/**
 * Runs unit tests on decode_ldr function from "../../src/util/id_util.h".
 */
static void test_decode_ldr(void);

/**
 * Runs unit tests on decode_str function from "../../src/util/id_util.h".
 */
static void test_decode_str(void);

/**
 * Runs unit tests on decode_trap function from "../../src/util/id_util.h".
 */
static void test_decode_trap(void);

void test_id_util() {
    test_get_opcode();
    test_is_jsr();
    test_is_jsrr();
    test_decode_add_and();
    test_decode_not();
    test_decode_br();
    test_decode_jmp_jsrr();
    test_decode_jsr();
    test_decode_ld_ldi_lea();
    test_decode_st_sti();
    test_decode_ldr();
    test_decode_str();
    test_decode_trap();
}

static void test_get_opcode(void) {
    int16_t ir;
    int expected, actual;

    errno = 0;
    ir = (int16_t)0x0000;
    expected = OP_BR;
    actual = get_opcode(ir);
    if (expected != actual) {
        fprintf(stderr,
                "test_get_opcode failed: get_opcode(0x%04X). expected: 0x%04X, "
                "actual: 0x%04X\n",
                ir, expected, actual);
    } else {
        passed_tests++;
    }

    errno = 0;
    ir = (int16_t)0x1000;
    expected = OP_ADD;
    actual = get_opcode(ir);
    if (expected != actual) {
        fprintf(stderr,
                "test_get_opcode failed: get_opcode(0x%04X). expected: 0x%04X, "
                "actual: 0x%04X\n",
                ir, expected, actual);
    } else {
        passed_tests++;
    }

    errno = 0;
    ir = (int16_t)0x2000;
    expected = OP_LD;
    actual = get_opcode(ir);
    if (expected != actual) {
        fprintf(stderr,
                "test_get_opcode failed: get_opcode(0x%04X). expected: 0x%04X, "
                "actual: 0x%04X\n",
                ir, expected, actual);
    } else {
        passed_tests++;
    }

    errno = 0;
    ir = (int16_t)0xB000;
    expected = OP_STI;
    actual = get_opcode(ir);
    if (expected != actual) {
        fprintf(stderr,
                "test_get_opcode failed: get_opcode(0x%04X). expected: 0x%04X, "
                "actual: 0x%04X\n",
                ir, expected, actual);
    } else {
        passed_tests++;
    }
}

static void test_is_jsr() {
    int16_t ir;
    int expected, actual;

    // opcode != JSR
    errno = 0;
    ir = 0x0000;
    expected = 0;
    actual = is_jsr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsr failed: is_jsr(0x%04X). expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // JSRR (bit11 = 0)
    errno = 0;
    ir = 0x4000;
    expected = 0;
    actual = is_jsr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsr failed: is_jsr(0x%04X). expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // JSR with bit11 = 1
    errno = 0;
    ir = (OP_JSR << 12) | (1 << 11);
    expected = 1;
    actual = is_jsr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsr failed: is_jsr(0x%04X). expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // some other opcode with bit11 = 1
    errno = 0;
    ir = (OP_ADD << 12) | (1 << 11);
    expected = 0;
    actual = is_jsr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsr failed: is_jsr(0x%04X). expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }
}

static void test_is_jsrr() {
    int16_t ir;
    int expected, actual;

    // opcode != JSR
    errno = 0;
    ir = 0x0000;
    expected = 0;
    actual = is_jsrr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsrr failed: is_jsrr(0x%04X). expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // JSRR (bit11 = 0)
    errno = 0;
    ir = (OP_JSR << 12) | (0 << 11) | 0x045;
    expected = 1;
    actual = is_jsrr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsrr failed: is_jsrr(0x%04X). expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // JSR immediate (bit11 = 1)
    errno = 0;
    ir = (OP_JSR << 12) | (1 << 11);
    expected = 0;
    actual = is_jsrr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsrr failed: is_jsrr(0x%04X). expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // other opcode with bit11 = 0
    errno = 0;
    ir = (OP_AND << 12) | (0 << 11);
    expected = 0;
    actual = is_jsrr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsrr failed: is_jsrr(0x%04X). expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }
}

static void test_decode_add_and() {
    fbuf_t fbuf;
    dbuf_t dbuf;
    int expected_errno, actual_errno;
    int16_t expected_reg, expected_operand1, expected_operand2;
    int16_t actual_reg, actual_operand1, actual_operand2;

    // error case: !fbuf.ready
    errno = 0;
    fbuf.ready = 0;
    decode_add_and(fbuf, &dbuf);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_add_and failed: decode_add_and({.ready=%d}, "
                "&dbuf). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // error case: dbuf = NULL
    errno = 0;
    fbuf.ready = 1;
    decode_add_and(fbuf, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_add_and failed: decode_add_and({.ready=%d}, "
                "NULL). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // add r2, r0, r1
    errno = 0;
    fbuf = (fbuf_t){.ready = 1,
                    .ir = (int16_t)((OP_ADD << 12) | (2 << 9) | (0 << 6) | 1)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[0] = 0x10;
    vm.reg[1] = 0x20;
    expected_reg = 2;
    expected_operand1 = 0x10;
    expected_operand2 = 0x20;

    decode_add_and(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;
    actual_operand2 = dbuf.operand2;

    if (expected_reg != actual_reg) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). "
                "expected_reg: r%d, actual_reg: r%d\n",
                fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "0x%04X, actual_operand1: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    if (expected_operand2 != actual_operand2) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). expected_operand2: "
                "0x%04X, actual_operand2: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand2, actual_operand2);
    } else {
        passed_tests++;
    }

    // add r6, r6, -1
    errno = 0;
    fbuf = (fbuf_t){.ready = 1,
                    .ir = (int16_t)((OP_ADD << 12) | (6 << 9) | (6 << 6) |
                                    (1 << 5) | bit_range(-1, 0, 4))};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[6] = (int16_t)0xF000;
    expected_reg = 6;
    expected_operand1 = (int16_t)0xF000;
    expected_operand2 = -1;

    decode_add_and(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;
    actual_operand2 = dbuf.operand2;

    if (expected_reg != actual_reg) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). "
                "expected_reg: r%d, actual_reg: r%d\n",
                fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "0x%04X, actual_operand1: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    if (expected_operand2 != actual_operand2) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). expected_operand2: "
                "%d, actual_operand2: %d\n",
                fbuf.ready, fbuf.ir, expected_operand2, actual_operand2);
    } else {
        passed_tests++;
    }

    // and r0, r1, r2
    errno = 0;
    fbuf = (fbuf_t){
        .ready = 1,
        .ir = (int16_t)((OP_AND << 12) | (0 << 9) | (1 << 6) | (0 << 5) | 2)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[1] = 0x1234;
    vm.reg[2] = 0x0FA2;
    expected_reg = 0;
    expected_operand1 = 0x1234;
    expected_operand2 = 0x0FA2;

    decode_add_and(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;
    actual_operand2 = dbuf.operand2;

    if (expected_reg != actual_reg) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). "
                "expected_reg: r%d, actual_reg: r%d\n",
                fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "0x%04X, actual_operand1: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    if (expected_operand2 != actual_operand2) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). expected_operand2: "
                "0x%04X, actual_operand2: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand2, actual_operand2);
    } else {
        passed_tests++;
    }

    // and r4, r4, 0
    errno = 0;
    fbuf = (fbuf_t){
        .ready = 1,
        .ir = (int16_t)((OP_AND << 12) | (4 << 9) | (4 << 6) | (1 << 5) | 0)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[4] = (int16_t)0xAAAA;
    expected_reg = 4;
    expected_operand1 = (int16_t)0xAAAA;
    expected_operand2 = 0;

    decode_add_and(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;
    actual_operand2 = dbuf.operand2;

    if (expected_reg != actual_reg) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). "
                "expected_reg: r%d, actual_reg: r%d\n",
                fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "0x%04X, actual_operand1: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    if (expected_operand2 != actual_operand2) {
        fprintf(stderr,
                "test_decode_add_and failed: "
                "decode_add_and({.ready=%d,.ir=0x%04X}). expected_operand2: "
                "%d, actual_operand2: %d\n",
                fbuf.ready, fbuf.ir, expected_operand2, actual_operand2);
    } else {
        passed_tests++;
    }
}

static void test_decode_not() {
    fbuf_t fbuf;
    dbuf_t dbuf;
    int expected_errno, actual_errno;
    int16_t expected_reg, expected_operand1;
    int16_t actual_reg, actual_operand1;

    // error case: !fbuf.ready
    errno = 0;
    fbuf.ready = 0;
    decode_not(fbuf, &dbuf);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_not failed: decode_not({.ready=%d}, "
                "&dbuf). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // error case: dbuf = NULL
    errno = 0;
    fbuf.ready = 1;
    decode_not(fbuf, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_not failed: decode_not({.ready=%d}, "
                "NULL). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // not r1, r2
    errno = 0;
    fbuf = (fbuf_t){.ready = 1,
                    .ir = (int16_t)((OP_NOT << 12) | (1 << 9) | (2 << 6))};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[2] = 0x20;
    expected_reg = 1;
    expected_operand1 = 0x20;

    decode_not(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;

    if (expected_reg != actual_reg) {
        fprintf(stderr,
                "test_decode_not failed: "
                "decode_not({.ready=%d,.ir=0x%04X}). "
                "expected_reg: r%d, actual_reg: r%d\n",
                fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_not failed: "
                "decode_not({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "0x%04X, actual_operand1: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_br() {
    fbuf_t fbuf;
    dbuf_t dbuf;
    int expected_errno, actual_errno;
    int16_t expected_cc, expected_operand1;
    int16_t actual_cc, actual_operand1;

    // error case: !fbuf.ready
    errno = 0;
    fbuf.ready = 0;
    decode_br(fbuf, &dbuf);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_br failed: decode_br({.ready=%d}, "
                "&dbuf). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // error case: dbuf = NULL
    errno = 0;
    fbuf.ready = 1;
    decode_br(fbuf, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_br failed: decode_br({.ready=%d}, "
                "NULL). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // brnzp 24
    errno = 0;
    fbuf = (fbuf_t){.ready = 1, .ir = (int16_t)((OP_BR << 12) | (7 << 9) | 24)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    expected_cc = 7;
    expected_operand1 = 24;

    decode_br(fbuf, &dbuf);

    actual_cc = dbuf.cc;
    actual_operand1 = dbuf.operand1;

    if (expected_cc != actual_cc) {
        fprintf(stderr,
                "test_decode_br failed: "
                "decode_br({.ready=%d,.ir=0x%04X}). "
                "expected_cc: %d, actual_cc: %d\n",
                fbuf.ready, fbuf.ir, expected_cc, actual_cc);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_br failed: "
                "decode_br({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "%d, actual_operand1: %d\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    // brnp 4
    errno = 0;
    fbuf = (fbuf_t){.ready = 1, .ir = (int16_t)((OP_BR << 12) | (5 << 9) | 4)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    expected_cc = 5;
    expected_operand1 = 4;

    decode_br(fbuf, &dbuf);

    actual_cc = dbuf.cc;
    actual_operand1 = dbuf.operand1;

    if (expected_cc != actual_cc) {
        fprintf(stderr,
                "test_decode_br failed: "
                "decode_br({.ready=%d,.ir=0x%04X}). "
                "expected_cc: %d, actual_cc: %d\n",
                fbuf.ready, fbuf.ir, expected_cc, actual_cc);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_br failed: "
                "decode_br({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "%d, actual_operand1: %d\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    // brn 1
    errno = 0;
    fbuf = (fbuf_t){.ready = 1, .ir = (int16_t)((OP_BR << 12) | (4 << 9) | 1)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    expected_cc = 4;
    expected_operand1 = 1;

    decode_br(fbuf, &dbuf);

    actual_cc = dbuf.cc;
    actual_operand1 = dbuf.operand1;

    if (expected_cc != actual_cc) {
        fprintf(stderr,
                "test_decode_br failed: "
                "decode_br({.ready=%d,.ir=0x%04X}). "
                "expected_cc: %d, actual_cc: %d\n",
                fbuf.ready, fbuf.ir, expected_cc, actual_cc);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_br failed: "
                "decode_br({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "%d, actual_operand1: %d\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_jmp_jsrr() {
    fbuf_t fbuf;
    dbuf_t dbuf;
    int expected_errno, actual_errno;
    int16_t expected_operand1, actual_operand1;

    // error case: !fbuf.ready
    errno = 0;
    fbuf.ready = 0;
    decode_jmp_jsrr(fbuf, &dbuf);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_jmp_jsrr failed: decode_jmp_jsrr({.ready=%d}, "
                "&dbuf). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // error case: dbuf = NULL
    errno = 0;
    fbuf.ready = 1;
    decode_jmp_jsrr(fbuf, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_jmp_jsrr failed: decode_jmp_jsrr({.ready=%d}, "
                "NULL). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // jmp r7 (ret)
    errno = 0;
    fbuf = (fbuf_t){.ready = 1, .ir = (int16_t)((OP_JMP << 12) | (7 << 6))};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[7] = 0x3001;
    expected_operand1 = 0x3001;

    decode_jmp_jsrr(fbuf, &dbuf);

    actual_operand1 = dbuf.operand1;

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_jmp_jsrr failed: "
                "decode_jmp_jsrr({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "0x%04X, actual_operand1: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    // jsrr r4
    errno = 0;
    fbuf = (fbuf_t){.ready = 1, .ir = (int16_t)((OP_JSR << 12) | (4 << 6))};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[4] = 0x300A;
    expected_operand1 = 0x300A;

    decode_jmp_jsrr(fbuf, &dbuf);

    actual_operand1 = dbuf.operand1;

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_jmp_jsrr failed: "
                "decode_jmp_jsrr({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "0x%04X, actual_operand1: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_jsr() {
    fbuf_t fbuf;
    dbuf_t dbuf;
    int expected_errno, actual_errno;
    int16_t expected_operand1, actual_operand1;

    // error case: !fbuf.ready
    errno = 0;
    fbuf.ready = 0;
    decode_jsr(fbuf, &dbuf);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_jsr failed: decode_jsr({.ready=%d}, "
                "&dbuf). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // error case: dbuf = NULL
    errno = 0;
    fbuf.ready = 1;
    decode_jsr(fbuf, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_jsr failed: decode_jsr({.ready=%d}, "
                "NULL). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // jsr -12
    errno = 0;
    fbuf = (fbuf_t){
        .ready = 1,
        .ir = (int16_t)((OP_JSR << 12) | (1 << 11) | bit_range(-12, 0, 11))};
    memset(&dbuf, 0, sizeof(dbuf_t));
    expected_operand1 = -12;

    decode_jsr(fbuf, &dbuf);

    actual_operand1 = dbuf.operand1;

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_jsr failed: "
                "decode_jsr({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "%d, actual_operand1: %d\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_ld_ldi_lea() {
    fbuf_t fbuf;
    dbuf_t dbuf;
    int expected_errno, actual_errno;
    int16_t expected_reg, expected_operand1, actual_reg, actual_operand1;

    // error case: !fbuf.ready
    errno = 0;
    fbuf.ready = 0;
    decode_ld_ldi_lea(fbuf, &dbuf);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_ld_ldi_lea failed: decode_ld_ldi_lea({.ready=%d}, "
                "&dbuf). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // error case: dbuf = NULL
    errno = 0;
    fbuf.ready = 1;
    decode_ld_ldi_lea(fbuf, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_ld_ldi_lea failed: decode_ld_ldi_lea({.ready=%d}, "
                "NULL). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // ld r0, 54
    errno = 0;
    fbuf = (fbuf_t){
        .ready = 1,
        .ir = (int16_t)((OP_LD << 12) | (0 << 9) | bit_range(-54, 0, 8))};
    memset(&dbuf, 0, sizeof(dbuf_t));
    expected_reg = 0;
    expected_operand1 = -54;

    decode_ld_ldi_lea(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;

    if (expected_reg != actual_reg) {
        fprintf(stderr,
                "test_decode_ld_ldi_lea failed: "
                "decode_ld_ldi_lea({.ready=%d,.ir=0x%04X}). expected_reg: "
                "r%d, actual_reg: r%d\n",
                fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_ld_ldi_lea failed: "
                "decode_ld_ldi_lea({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "%d, actual_operand1: %d\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    // ldi r3, 2
    errno = 0;
    fbuf = (fbuf_t){.ready = 1, .ir = (int16_t)((OP_LDI << 12) | (3 << 9) | 2)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    expected_reg = 3;
    expected_operand1 = 2;

    decode_ld_ldi_lea(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;

    if (expected_reg != actual_reg) {
        fprintf(stderr,
                "test_decode_ld_ldi_lea failed: "
                "decode_ld_ldi_lea({.ready=%d,.ir=0x%04X}). expected_reg: "
                "r%d, actual_reg: r%d\n",
                fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_ld_ldi_lea failed: "
                "decode_ld_ldi_lea({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "%d, actual_operand1: %d\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    // lea r4, 10
    errno = 0;
    fbuf =
        (fbuf_t){.ready = 1, .ir = (int16_t)((OP_LEA << 12) | (4 << 9) | 10)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    expected_reg = 4;
    expected_operand1 = 10;

    decode_ld_ldi_lea(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;

    if (expected_reg != actual_reg) {
        fprintf(stderr,
                "test_decode_ld_ldi_lea failed: "
                "decode_ld_ldi_lea({.ready=%d,.ir=0x%04X}). expected_reg: "
                "r%d, actual_reg: r%d\n",
                fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_ld_ldi_lea failed: "
                "decode_ld_ldi_lea({.ready=%d,.ir=0x%04X}). expected_operand1: "
                "%d, actual_operand1: %d\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_st_sti() {
    fbuf_t fbuf;
    dbuf_t dbuf;
    int expected_errno, actual_errno;
    int16_t expected_reg, expected_operand1, actual_reg, actual_operand1;

    // error case: !fbuf.ready
    errno = 0;
    fbuf.ready = 0;
    decode_st_sti(fbuf, &dbuf);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_st_sti failed: decode_st_sti({.ready=%d}, "
                "&dbuf). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // error case: dbuf = NULL
    errno = 0;
    fbuf.ready = 1;
    decode_st_sti(fbuf, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_st_sti failed: decode_st_sti({.ready=%d}, "
                "NULL). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // st r0, 54
    errno = 0;
    fbuf = (fbuf_t){.ready = 1, .ir = (int16_t)((OP_ST << 12) | (0 << 9) | 54)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[0] = 0x10;
    expected_reg = 0x10;
    expected_operand1 = 54;

    decode_st_sti(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;

    if (expected_reg != actual_reg) {
        fprintf(
            stderr,
            "test_decode_st_sti failed: decode_st_sti({.ready=%d, .ir=0x%04X}, "
            "&dbuf). expected_reg: 0x%04X, actual_reg: 0x%04X\n",
            fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(
            stderr,
            "test_decode_st_sti failed: decode_st_sti({.ready=%d, .ir=0x%04X}, "
            "&dbuf). expected_operand1: %d, actual_operand1: %d\n",
            fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    // sti r2, 5
    errno = 0;
    fbuf = (fbuf_t){.ready = 1, .ir = (int16_t)((OP_STI << 12) | (2 << 9) | 5)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[2] = 0x5000;
    expected_reg = 0x5000;
    expected_operand1 = 5;

    decode_st_sti(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;

    if (expected_reg != actual_reg) {
        fprintf(
            stderr,
            "test_decode_st_sti failed: decode_st_sti({.ready=%d, .ir=0x%04X}, "
            "&dbuf). expected_reg: 0x%04X, actual_reg: 0x%04X\n",
            fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(
            stderr,
            "test_decode_st_sti failed: decode_st_sti({.ready=%d, .ir=0x%04X}, "
            "&dbuf). expected_operand1: %d, actual_operand1: %d\n",
            fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_ldr() {
    fbuf_t fbuf;
    dbuf_t dbuf;
    int expected_errno, actual_errno;
    int16_t expected_reg, expected_operand1, expected_operand2;
    int16_t actual_reg, actual_operand1, actual_operand2;

    // error case: !fbuf.ready
    errno = 0;
    fbuf.ready = 0;
    decode_ldr(fbuf, &dbuf);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_ldr failed: decode_ldr({.ready=%d}, "
                "&dbuf). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // error case: dbuf = NULL
    errno = 0;
    fbuf.ready = 1;
    decode_ldr(fbuf, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_ldr failed: decode_ldr({.ready=%d}, "
                "NULL). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // ldr r0, r5, -2
    errno = 0;
    fbuf = (fbuf_t){.ready = 1,
                    .ir = (int16_t)((OP_LDR << 12) | (0 << 9) | (5 << 6) |
                                    bit_range(-2, 0, 5))};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[5] = (int16_t)0xEFFE;
    expected_reg = 0;
    expected_operand1 = (int16_t)0xEFFE;
    expected_operand2 = -2;

    decode_ldr(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;
    actual_operand2 = dbuf.operand2;

    if (expected_reg != actual_reg) {
        fprintf(stderr,
                "test_decode_ldr failed: decode_ldr({.ready=%d, .ir=0x%04X}, "
                "&dbuf). expected_reg: r%d, actual_reg: r%d\n",
                fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_ldr failed: decode_ldr({.ready=%d, .ir=0x%04X}, "
                "&dbuf). expected_operand1: 0x%04X, actual_operand1: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    if (expected_operand2 != actual_operand2) {
        fprintf(stderr,
                "test_decode_ldr failed: decode_ldr({.ready=%d, .ir=0x%04X}, "
                "&dbuf). expected_operand2: %d, actual_operand2: %d\n",
                fbuf.ready, fbuf.ir, expected_operand2, actual_operand2);
    } else {
        passed_tests++;
    }
}

static void test_decode_str() {
    fbuf_t fbuf;
    dbuf_t dbuf;
    int expected_errno, actual_errno;
    int16_t expected_reg, expected_operand1, expected_operand2;
    int16_t actual_reg, actual_operand1, actual_operand2;

    // error case: !fbuf.ready
    errno = 0;
    fbuf.ready = 0;
    decode_str(fbuf, &dbuf);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_str failed: decode_str({.ready=%d}, "
                "&dbuf). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // error case: dbuf = NULL
    errno = 0;
    fbuf.ready = 1;
    decode_str(fbuf, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_str failed: decode_str({.ready=%d}, "
                "NULL). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // str r0, r5, 3
    errno = 0;
    fbuf = (fbuf_t){.ready = 1,
                    .ir = (int16_t)((OP_STR << 12) | (0 << 9) | (5 << 6) | 3)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    vm.reg[0] = (int16_t)0x19;
    vm.reg[5] = (int16_t)0xEFFE;
    expected_reg = (int16_t)0x19;
    expected_operand1 = (int16_t)0xEFFE;
    expected_operand2 = 3;

    decode_str(fbuf, &dbuf);

    actual_reg = dbuf.reg;
    actual_operand1 = dbuf.operand1;
    actual_operand2 = dbuf.operand2;

    if (expected_reg != actual_reg) {
        fprintf(stderr,
                "test_decode_str failed: decode_str({.ready=%d, .ir=0x%04X}, "
                "&dbuf). expected_reg: 0x%04X, actual_reg: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_reg, actual_reg);
    } else {
        passed_tests++;
    }

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_str failed: decode_str({.ready=%d, .ir=0x%04X}, "
                "&dbuf). expected_operand1: 0x%04X, actual_operand1: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }

    if (expected_operand2 != actual_operand2) {
        fprintf(stderr,
                "test_decode_str failed: decode_str({.ready=%d, .ir=0x%04X}, "
                "&dbuf). expected_operand2: %d, actual_operand2: %d\n",
                fbuf.ready, fbuf.ir, expected_operand2, actual_operand2);
    } else {
        passed_tests++;
    }
}

static void test_decode_trap() {
    fbuf_t fbuf;
    dbuf_t dbuf;
    int expected_errno, actual_errno;
    int16_t expected_operand1, actual_operand1;

    // error case: !fbuf.ready
    errno = 0;
    fbuf.ready = 0;
    decode_trap(fbuf, &dbuf);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_trap failed: decode_trap({.ready=%d}, "
                "&dbuf). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // error case: dbuf = NULL
    errno = 0;
    fbuf.ready = 1;
    decode_trap(fbuf, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_trap failed: decode_trap({.ready=%d}, "
                "NULL). expected_errno: %d, actual_errno: %d\n",
                fbuf.ready, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // trap 0x25
    errno = 0;
    fbuf = (fbuf_t){.ready = 1, .ir = (int16_t)((OP_TRAP << 12) | 0x25)};
    memset(&dbuf, 0, sizeof(dbuf_t));
    expected_operand1 = 0x25;

    decode_trap(fbuf, &dbuf);

    actual_operand1 = dbuf.operand1;

    if (expected_operand1 != actual_operand1) {
        fprintf(stderr,
                "test_decode_trap failed: decode_trap({.ready=%d, .ir=0x%04X}, "
                "&dbuf). expected_operand1: 0x%04X, actual_operand1: 0x%04X\n",
                fbuf.ready, fbuf.ir, expected_operand1, actual_operand1);
    } else {
        passed_tests++;
    }
}
