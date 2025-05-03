#include "test_id_util.h"

#include <errno.h>
#include <stdio.h>

#include "../../src/stages/id.h"
#include "../../src/util/bitops.h"
#include "../../src/util/id_util.h"
#include "../../src/vm.h"
#include "../driver.h"

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

static void test_is_jsr() {
    int16_t ir;
    int expected, actual;

    // opcode != JSR
    ir = 0x0000;
    expected = 0;
    actual = is_jsr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsr failed: is_jsr(0x%04X): expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // JSRR (bit11 = 0)
    ir = 0x4000;
    expected = 0;
    actual = is_jsr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsr failed: is_jsr(0x%04X): expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // JSR with bit11 = 1
    ir = (OP_JSR << 12) | (1 << 11);
    expected = 1;
    actual = is_jsr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsr failed: is_jsr(0x%04X): expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // some other opcode with bit11 = 1
    ir = (OP_ADD << 12) | (1 << 11);
    expected = 0;
    actual = is_jsr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsr failed: is_jsr(0x%04X): expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }
}

static void test_is_jsrr() {
    int16_t ir;
    int expected, actual;

    // opcode != JSR
    ir = 0x0000;
    expected = 0;
    actual = is_jsrr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsrr failed: is_jsrr(0x%04X): expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // JSRR (bit11 = 0)
    ir = (OP_JSR << 12) | (0 << 11) | 0x045;
    expected = 1;
    actual = is_jsrr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsrr failed: is_jsrr(0x%04X): expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // JSR immediate (bit11 = 1)
    ir = (OP_JSR << 12) | (1 << 11);
    expected = 0;
    actual = is_jsrr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsrr failed: is_jsrr(0x%04X): expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }

    // other opcode with bit11 = 0
    ir = (OP_AND << 12) | (0 << 11);
    expected = 0;
    actual = is_jsrr(ir);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_is_jsrr failed: is_jsrr(0x%04X): expected: %d, actual: %d\n",
            ir, expected, actual);
    } else {
        passed_tests++;
    }
}

static void test_decode_add_and() {
    fbuf_t fbad, fg;
    dbuf_t dg;
    int expected_errno, actual_errno;

    // error case: !ready
    fbad = (fbuf_t){.ready = 0};
    decode_add_and(fbad, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_add_and failed: decode_add_and(fbuf.ready=0, "
                "dbuf=NULL): expected_errno: %d, actual_errno: %d\n",
                expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid ADD (register)
    errno = 0;
    fg = (fbuf_t){.ready = 1,
                  .ir = (OP_ADD << 12) | (3 << 9) | (2 << 6) | (0 << 5) |
                        (1 << 0)};
    vm.reg[2] = 0x10;
    vm.reg[1] = 0x20;
    dg = (dbuf_t){0};
    decode_add_and(fg, &dg);
    if (dg.reg != 3 || dg.operand1 != 0x10 || dg.operand2 != 0x20) {
        fprintf(stderr,
                "test_decode_add_and failed: decode_add_and(ir=0x%04X): "
                "expected: reg=3, operand1=0x10, operand2=0x20; actual: "
                "reg=%d, operand1=0x%X, operand2=0x%X\n",
                fg.ir, dg.reg, dg.operand1, dg.operand2);
    } else {
        passed_tests++;
    }
}

static void test_decode_not() {
    fbuf_t fbad, fg;
    dbuf_t dg;
    int expected_errno, actual_errno;

    // error case: !ready
    fbad = (fbuf_t){.ready = 0};
    decode_not(fbad, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_not failed: decode_not(fbuf.ready=0, dbuf=NULL): "
                "expected_errno: %d, actual_errno: %d\n",
                expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid NOT
    errno = 0;
    fg = (fbuf_t){.ready = 1, .ir = (OP_NOT << 12) | (4 << 9) | (5 << 6)};
    vm.reg[5] = 0x33;
    dg = (dbuf_t){0};
    decode_not(fg, &dg);
    if (dg.reg != 4 || dg.operand1 != 0x33) {
        fprintf(stderr,
                "test_decode_not failed: decode_not(ir=0x%04X): expected: "
                "reg=4, operand1=0x33; actual: reg=%d, operand1=0x%X\n",
                fg.ir, dg.reg, dg.operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_br() {
    fbuf_t fbad, fg;
    dbuf_t dg;
    int expected_errno, actual_errno;

    // error case: !ready
    fbad = (fbuf_t){.ready = 0};
    decode_br(fbad, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_br failed: decode_br(fbuf.ready=0, dbuf=NULL): "
                "expected_errno: %d, actual_errno: %d\n",
                expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid BR: cc=5, offset=-4 (0x1FC)
    errno = 0;
    fg = (fbuf_t){.ready = 1, .ir = (OP_BR << 12) | (5 << 9) | 0x1FC};
    dg = (dbuf_t){0};
    decode_br(fg, &dg);
    if (dg.cc != 5 || dg.operand1 != (int16_t)-4) {
        fprintf(stderr,
                "test_decode_br failed: decode_br(ir=0x%04X): expected: cc=5, "
                "operand1=-4; actual: cc=%d, operand1=%d\n",
                fg.ir, dg.cc, dg.operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_jmp_jsrr() {
    fbuf_t fbad, fg;
    dbuf_t dg;
    int expected_errno, actual_errno;

    // error case: !ready
    fbad = (fbuf_t){.ready = 0};
    decode_jmp_jsrr(fbad, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_jmp_jsrr failed: decode_jmp_jsrr(fbuf.ready=0, "
                "dbuf=NULL): expected_errno: %d, actual_errno: %d\n",
                expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid JMP/JSRR
    errno = 0;
    fg = (fbuf_t){.ready = 1, .ir = (OP_JMP << 12) | (6 << 6)};
    vm.reg[6] = 0x77;
    dg = (dbuf_t){0};
    decode_jmp_jsrr(fg, &dg);
    if (dg.operand1 != 0x77) {
        fprintf(stderr,
                "test_decode_jmp_jsrr failed: decode_jmp_jsrr(ir=0x%04X): "
                "expected: operand1=0x77; actual: operand1=0x%X\n",
                fg.ir, dg.operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_jsr() {
    fbuf_t fbad, fg;
    dbuf_t dg;
    int expected_errno, actual_errno;

    // error case: !ready
    fbad = (fbuf_t){.ready = 0};
    decode_jsr(fbad, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_jsr failed: decode_jsr(fbuf.ready=0, dbuf=NULL): "
                "expected_errno: %d, actual_errno: %d\n",
                expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid JSR: offset = -2 (0x7FE)
    errno = 0;
    fg = (fbuf_t){.ready = 1, .ir = (OP_JSR << 12) | (1 << 11) | 0x7FE};
    dg = (dbuf_t){0};
    decode_jsr(fg, &dg);
    if (dg.operand1 != (int16_t)-2) {
        fprintf(stderr,
                "test_decode_jsr failed: decode_jsr(ir=0x%04X): expected: "
                "operand1=-2; actual: operand1=%d\n",
                fg.ir, dg.operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_ld_ldi_lea() {
    fbuf_t fbad, fg;
    dbuf_t dg;
    int expected_errno, actual_errno;

    // error case: !ready
    fbad = (fbuf_t){.ready = 0};
    decode_ld_ldi_lea(fbad, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(
            stderr,
            "test_decode_ld_ldi_lea failed: decode_ld_ldi_lea(fbuf.ready=0, "
            "dbuf=NULL): expected_errno: %d, actual_errno: %d\n",
            expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid LD/LDI/LEA
    errno = 0;
    fg = (fbuf_t){.ready = 1, .ir = (OP_LD << 12) | (7 << 9) | 0x0A};
    dg = (dbuf_t){0};
    decode_ld_ldi_lea(fg, &dg);
    if (dg.reg != 7 || dg.operand1 != 0x0A) {
        fprintf(
            stderr,
            "test_decode_ld_ldi_lea failed: decode_ld_ldi_lea(ir=0x%04X): "
            "expected: reg=7, operand1=0x0A; actual: reg=%d, operand1=0x%X\n",
            fg.ir, dg.reg, dg.operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_st_sti() {
    fbuf_t fbad, fg;
    dbuf_t dg;
    int expected_errno, actual_errno;

    // error case: !ready
    fbad = (fbuf_t){.ready = 0};
    decode_st_sti(fbad, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_st_sti failed: decode_st_sti(fbuf.ready=0, "
                "dbuf=NULL): expected_errno: %d, actual_errno: %d\n",
                expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid ST/STI
    errno = 0;
    fg = (fbuf_t){.ready = 1, .ir = (OP_ST << 12) | (7 << 9) | 0x0B};
    vm.reg[7] = 0x56;
    dg = (dbuf_t){0};
    decode_st_sti(fg, &dg);
    if (dg.reg != 0x56 || dg.operand1 != 0x0B) {
        fprintf(
            stderr,
            "test_decode_st_sti failed: decode_st_sti(ir=0x%04X): expected: "
            "reg=0x56, operand1=0x0B; actual: reg=0x%X, operand1=0x%X\n",
            fg.ir, dg.reg, dg.operand1);
    } else {
        passed_tests++;
    }
}

static void test_decode_ldr() {
    fbuf_t fbad, fg;
    dbuf_t dg;
    int expected_errno, actual_errno;

    // error case: !ready
    fbad = (fbuf_t){.ready = 0};
    decode_ldr(fbad, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_ldr failed: decode_ldr(fbuf.ready=0, dbuf=NULL): "
                "expected_errno: %d, actual_errno: %d\n",
                expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid LDR
    errno = 0;
    fg =
        (fbuf_t){.ready = 1, .ir = (OP_LDR << 12) | (9 << 9) | (1 << 6) | 0x03};
    vm.reg[1] = 0x9A;
    dg = (dbuf_t){0};
    decode_ldr(fg, &dg);
    if (dg.reg != 9 || dg.operand1 != 0x9A || dg.operand2 != 3) {
        fprintf(stderr,
                "test_decode_ldr failed: decode_ldr(ir=0x%04X): expected: "
                "reg=9, operand1=0x9A, operand2=3; actual: reg=%d, "
                "operand1=0x%X, operand2=%d\n",
                fg.ir, dg.reg, dg.operand1, dg.operand2);
    } else {
        passed_tests++;
    }
}

static void test_decode_str() {
    fbuf_t fbad, fg;
    dbuf_t dg;
    int expected_errno, actual_errno;

    // error case: !ready
    fbad = (fbuf_t){.ready = 0};
    decode_str(fbad, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_str failed: decode_str(fbuf.ready=0, dbuf=NULL): "
                "expected_errno: %d, actual_errno: %d\n",
                expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid STR
    errno = 0;
    fg =
        (fbuf_t){.ready = 1, .ir = (OP_STR << 12) | (2 << 9) | (3 << 6) | 0x02};
    vm.reg[2] = 0xAB;
    vm.reg[3] = 0xCD;
    dg = (dbuf_t){0};
    decode_str(fg, &dg);
    if (dg.reg != 0xCD || dg.operand1 != 0xAB || dg.operand2 != 2) {
        fprintf(stderr,
                "test_decode_str failed: decode_str(ir=0x%04X): expected: "
                "reg=0xCD, operand1=0xAB, operand2=2; actual: reg=0x%X, "
                "operand1=0x%X, operand2=%d\n",
                fg.ir, dg.reg, dg.operand1, dg.operand2);
    } else {
        passed_tests++;
    }
}

static void test_decode_trap() {
    fbuf_t fbad, fg;
    dbuf_t dg;
    int expected_errno, actual_errno;

    // error case: !ready
    fbad = (fbuf_t){.ready = 0};
    decode_trap(fbad, NULL);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_decode_trap failed: decode_trap(fbuf.ready=0, "
                "dbuf=NULL): expected_errno: %d, actual_errno: %d\n",
                expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid TRAP
    errno = 0;
    fg = (fbuf_t){.ready = 1, .ir = (OP_TRAP << 12) | 0x7F};
    dg = (dbuf_t){0};
    decode_trap(fg, &dg);
    if (dg.operand1 != 0x7F) {
        fprintf(stderr,
                "test_decode_trap failed: decode_trap(ir=0x%04X): expected: "
                "operand1=0x7F; actual: operand1=0x%X\n",
                fg.ir, dg.operand1);
    } else {
        passed_tests++;
    }
}
