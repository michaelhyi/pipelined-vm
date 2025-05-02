#include "test_id_util.h"

#include <errno.h>
#include <stdio.h>

#include "../../src/stages/id.h"
#include "../../src/stages/id_util.h"
#include "../../src/util.h"
#include "../../src/vm.h"

/**
 * Runs unit tests on the is_jsr function from "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_is_jsr(int *passed_tests);

/**
 * Runs unit tests on the is_jsrr function from "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_is_jsrr(int *passed_tests);

/**
 * Runs unit tests on the decode_add_and function from
 * "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_decode_add_and(int *passed_tests);

/**
 * Runs unit tests on the decode_not function from "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_decode_not(int *passed_tests);

/**
 * Runs unit tests on the decode_br function from "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_decode_br(int *passed_tests);

/**
 * Runs unit tests on the decode_jmp_jsrr function from
 * "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_decode_jmp_jsrr(int *passed_tests);

/**
 * Runs unit tests on the decode_jsr function from "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_decode_jsr(int *passed_tests);

/**
 * Runs unit tests on the decode_ld_ldi_lea function from
 * "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_decode_ld_ldi_lea(int *passed_tests);

/**
 * Runs unit tests on the decode_st_sti function from "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_decode_st_sti(int *passed_tests);

/**
 * Runs unit tests on the decode_ldr function from "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_decode_ldr(int *passed_tests);

/**
 * Runs unit tests on the decode_str function from "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_decode_str(int *passed_tests);

/**
 * Runs unit tests on the decode_trap function from "../src/stages/id_util.h".
 *
 * @param passed_tests pointer to a counter of passed tests
 */
static void test_decode_trap(int *passed_tests);

void test_id_util(int *passed_tests) {
    if (!passed_tests) {
        fprintf(stderr, "test_id_util failed: passed_tests must be non-null\n");
        errno = EINVAL;
        return;
    }

    test_is_jsr(passed_tests);
    test_is_jsrr(passed_tests);
    test_decode_add_and(passed_tests);
    test_decode_not(passed_tests);
    test_decode_br(passed_tests);
    test_decode_jmp_jsrr(passed_tests);
    test_decode_jsr(passed_tests);
    test_decode_ld_ldi_lea(passed_tests);
    test_decode_st_sti(passed_tests);
    test_decode_ldr(passed_tests);
    test_decode_str(passed_tests);
    test_decode_trap(passed_tests);
}

static void test_is_jsr(int *passed_tests) {
    // opcode != JSR
    if (is_jsr(0x0000) != 0) {
        fprintf(stderr, "test_is_jsr: expected 0 for non-JSR opcode\n");
    } else {
        (*passed_tests)++;
    }

    // JSRR (bit11 = 0)
    int16_t ir = (OP_JSR << 12) | (0 << 11);
    if (is_jsr(ir) != 0) {
        fprintf(stderr, "test_is_jsr: expected 0 for JSRR encoding\n");
    } else {
        (*passed_tests)++;
    }

    // JSR with bit11 = 1
    ir = (OP_JSR << 12) | (1 << 11) | 0x123;
    if (is_jsr(ir) != 1) {
        fprintf(stderr, "test_is_jsr: expected 1 for JSR immediate\n");
    } else {
        (*passed_tests)++;
    }

    // some other opcode with bit11 = 1
    ir = (OP_ADD << 12) | (1 << 11);
    if (is_jsr(ir) != 0) {
        fprintf(stderr, "test_is_jsr: expected 0 for non-JSR with bit11=1\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_is_jsrr(int *passed_tests) {
    // opcode != JSR
    if (is_jsrr(0x0000) != 0) {
        fprintf(stderr, "test_is_jsrr: expected 0 for non-JSR opcode\n");
    } else {
        (*passed_tests)++;
    }

    // JSRR (bit11 = 0)
    int16_t ir = (OP_JSR << 12) | (0 << 11) | 0x045;
    if (is_jsrr(ir) != 1) {
        fprintf(stderr, "test_is_jsrr: expected 1 for JSRR encoding\n");
    } else {
        (*passed_tests)++;
    }

    // JSR immediate (bit11 = 1)
    ir = (OP_JSR << 12) | (1 << 11);
    if (is_jsrr(ir) != 0) {
        fprintf(stderr, "test_is_jsrr: expected 0 for JSR immediate\n");
    } else {
        (*passed_tests)++;
    }

    // other opcode with bit11 = 0
    ir = (OP_AND << 12) | (0 << 11);
    if (is_jsrr(ir) != 0) {
        fprintf(stderr, "test_is_jsrr: expected 0 for non-JSR with bit11=0\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_decode_add_and(int *passed_tests) {
    // error case: !ready
    fbuf_t fbad = {.ready = 0};
    decode_add_and(fbad, NULL);
    if (errno != EINVAL) {
        fprintf(
            stderr,
            "test_decode_add_and: expected errno=EINVAL for bad args, got %d\n",
            errno);
    } else {
        (*passed_tests)++;
    }

    // valid ADD (register)
    errno = 0;
    fbuf_t fg = {.ready = 1,
                 .ir = (OP_ADD << 12) | (3 << 9) | (2 << 6) | (0 << 5) |
                       (1 << 0)};
    vm.reg[2] = 0x10;
    vm.reg[1] = 0x20;
    dbuf_t dg = {0};
    decode_add_and(fg, &dg);
    if (dg.reg != 3 || dg.operand1 != 0x10 || dg.operand2 != 0x20) {
        fprintf(stderr, "test_decode_add_and: wrong reg/op for register ADD\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_decode_not(int *passed_tests) {
    // error case: !ready
    fbuf_t fbad = {.ready = 0};
    decode_not(fbad, NULL);
    if (errno != EINVAL) {
        fprintf(stderr,
                "test_decode_not: expected errno=EINVAL for bad args, got %d\n",
                errno);
    } else {
        (*passed_tests)++;
    }

    // valid NOT
    errno = 0;
    fbuf_t fg = {.ready = 1,
                 .ir = (int16_t)((OP_NOT << 12) | (4 << 9) | (5 << 6))};
    vm.reg[5] = 0x33;
    dbuf_t dg = {0};
    decode_not(fg, &dg);
    if (dg.reg != 4 || dg.operand1 != 0x33) {
        fprintf(stderr, "test_decode_not: wrong reg/op for NOT\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_decode_br(int *passed_tests) {
    // error case: !ready
    fbuf_t fbad = {.ready = 0};
    decode_br(fbad, NULL);
    if (errno != EINVAL) {
        fprintf(stderr,
                "test_decode_br: expected errno=EINVAL for bad args, got %d\n",
                errno);
    } else {
        (*passed_tests)++;
    }

    // valid BR: cc=5, offset=-4 (0x1FC)
    errno = 0;
    fbuf_t fg = {.ready = 1, .ir = (OP_BR << 12) | (5 << 9) | 0x1FC};
    dbuf_t dg = {0};
    decode_br(fg, &dg);
    if (dg.cc != 5 || dg.operand1 != (int16_t)-4) {
        fprintf(stderr, "test_decode_br: wrong cc/op for BR\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_decode_jmp_jsrr(int *passed_tests) {
    // error case: !ready
    fbuf_t fbad = {.ready = 0};
    decode_jmp_jsrr(fbad, NULL);
    if (errno != EINVAL) {
        fprintf(stderr,
                "test_decode_jmp_jsrr: expected errno=EINVAL for bad args, got "
                "%d\n",
                errno);
    } else {
        (*passed_tests)++;
    }

    // valid JMP/JSRR
    errno = 0;
    fbuf_t fg = {.ready = 1, .ir = (int16_t)((OP_JMP << 12) | (6 << 6))};
    vm.reg[6] = 0x77;
    dbuf_t dg = {0};
    decode_jmp_jsrr(fg, &dg);
    if (dg.operand1 != 0x77) {
        fprintf(stderr, "test_decode_jmp_jsrr: wrong operand for JMP/JSRR\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_decode_jsr(int *passed_tests) {
    // error case: !ready
    fbuf_t fbad = {.ready = 0};
    decode_jsr(fbad, NULL);
    if (errno != EINVAL) {
        fprintf(stderr,
                "test_decode_jsr: expected errno=EINVAL for bad args, got %d\n",
                errno);
    } else {
        (*passed_tests)++;
    }

    // valid JSR: offset = -2 (0x7FE)
    errno = 0;
    fbuf_t fg = {.ready = 1, .ir = (OP_JSR << 12) | (1 << 11) | 0x7FE};
    dbuf_t dg = {0};
    decode_jsr(fg, &dg);
    if (dg.operand1 != (int16_t)-2) {
        fprintf(stderr, "test_decode_jsr: wrong operand for JSR\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_decode_ld_ldi_lea(int *passed_tests) {
    // error case: !ready
    fbuf_t fbad = {.ready = 0};
    decode_ld_ldi_lea(fbad, NULL);
    if (errno != EINVAL) {
        fprintf(stderr,
                "test_decode_ld_ldi_lea: expected errno=EINVAL for bad args, "
                "got %d\n",
                errno);
    } else {
        (*passed_tests)++;
    }

    // valid LD/LDI/LEA
    errno = 0;
    fbuf_t fg = {.ready = 1, .ir = (OP_LD << 12) | (7 << 9) | 0x0A};
    dbuf_t dg = {0};
    decode_ld_ldi_lea(fg, &dg);
    if (dg.reg != 7 || dg.operand1 != 0x0A) {
        fprintf(stderr,
                "test_decode_ld_ldi_lea: wrong reg/op for LD/LDI/LEA\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_decode_st_sti(int *passed_tests) {
    // error case: !ready
    fbuf_t fbad = {.ready = 0};
    decode_st_sti(fbad, NULL);
    if (errno != EINVAL) {
        fprintf(
            stderr,
            "test_decode_st_sti: expected errno=EINVAL for bad args, got %d\n",
            errno);
    } else {
        (*passed_tests)++;
    }

    // valid ST/STI
    errno = 0;
    fbuf_t fg = {.ready = 1, .ir = (OP_ST << 12) | (7 << 9) | 0x0B};
    vm.reg[7] = 0x56;
    dbuf_t dg = {0};
    decode_st_sti(fg, &dg);
    if (dg.reg != 0x56 || dg.operand1 != 0x0B) {
        fprintf(stderr, "test_decode_st_sti: wrong reg/op for ST/STI\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_decode_ldr(int *passed_tests) {
    // error case: !ready
    fbuf_t fbad = {.ready = 0};
    decode_ldr(fbad, NULL);
    if (errno != EINVAL) {
        fprintf(stderr,
                "test_decode_ldr: expected errno=EINVAL for bad args, got %d\n",
                errno);
    } else {
        (*passed_tests)++;
    }

    // valid LDR
    errno = 0;
    fbuf_t fg = {.ready = 1, .ir = (OP_LDR << 12) | (9 << 9) | (1 << 6) | 0x03};
    vm.reg[1] = 0x9A;
    dbuf_t dg = {0};
    decode_ldr(fg, &dg);
    if (dg.reg != 9 || dg.operand1 != 0x9A || dg.operand2 != 3) {
        fprintf(stderr, "test_decode_ldr: wrong fields for LDR\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_decode_str(int *passed_tests) {
    // error case: !ready
    fbuf_t fbad = {.ready = 0};
    decode_str(fbad, NULL);
    if (errno != EINVAL) {
        fprintf(stderr,
                "test_decode_str: expected errno=EINVAL for bad args, got %d\n",
                errno);
    } else {
        (*passed_tests)++;
    }

    // valid STR
    errno = 0;
    fbuf_t fg = {.ready = 1, .ir = (OP_STR << 12) | (2 << 9) | (3 << 6) | 0x02};
    vm.reg[2] = 0xAB;
    vm.reg[3] = 0xCD;
    dbuf_t dg = {0};
    decode_str(fg, &dg);
    if (dg.reg != 0xCD || dg.operand1 != 0xAB || dg.operand2 != 2) {
        fprintf(stderr, "test_decode_str: wrong fields for STR\n");
    } else {
        (*passed_tests)++;
    }
}

static void test_decode_trap(int *passed_tests) {
    // error case: !ready
    fbuf_t fbad = {.ready = 0};
    decode_trap(fbad, NULL);
    if (errno != EINVAL) {
        fprintf(
            stderr,
            "test_decode_trap: expected errno=EINVAL for bad args, got %d\n",
            errno);
    } else {
        (*passed_tests)++;
    }

    // valid TRAP
    errno = 0;
    fbuf_t fg = {.ready = 1, .ir = (int16_t)((OP_TRAP << 12) | 0x7F)};
    dbuf_t dg = {0};
    decode_trap(fg, &dg);
    if (dg.operand1 != 0x7F) {
        fprintf(stderr, "test_decode_trap: wrong operand for TRAP\n");
    } else {
        (*passed_tests)++;
    }
}
