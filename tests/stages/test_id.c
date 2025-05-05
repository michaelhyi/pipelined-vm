#include "test_id.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "../../src/stages/id.h"
#include "../../src/vm.h"
#include "../driver.h"

/**
 * Runs unit tests on id_run using a BR instruction.
 */
static void test_id_br(void);

/**
 * Runs unit tests on id_run using an ADD instruction.
 */
static void test_id_add(void);

/**
 * Runs unit tests on id_run using a LD instruction.
 */
static void test_id_ld(void);

/**
 * Runs unit tests on id_run using a ST instruction.
 */
static void test_id_st(void);

/**
 * Runs unit tests on id_run using a JSR instruction.
 */
static void test_id_jsr(void);

/**
 * Runs unit tests on id_run using a JSRR instruction.
 */
static void test_id_jsrr(void);

/**
 * Runs unit tests on id_run using an AND instruction.
 */
static void test_id_and(void);

/**
 * Runs unit tests on id_run using a LDR instruction.
 */
static void test_id_ldr(void);

/**
 * Runs unit tests on id_run using a STR instruction.
 */
static void test_id_str(void);

/**
 * Runs unit tests on id_run using a NOT instruction.
 */
static void test_id_not(void);

/**
 * Runs unit tests on id_run using an LDI instruction.
 */
static void test_id_ldi(void);

/**
 * Runs unit tests on id_run using a STI instruction.
 */
static void test_id_sti(void);

/**
 * Runs unit tests on id_run using a JMP instruction.
 */
static void test_id_jmp(void);

/**
 * Runs unit tests on id_run using a LEA instruction.
 */
static void test_id_lea(void);

/**
 * Runs unit tests on id_run using a TRAP instruction.
 */
static void test_id_trap(void);

void test_id_run() { test_id_br(); }

static void test_id_br() {
    pthread_t tid;
    int err;
    int expected_fbuf_read, expected_dbuf_ready, expected_dbuf_read;
    int expected_dbuf_pc, expected_dbuf_opcode, expected_dbuf_reg;
    int expected_dbuf_operand1, expected_dbuf_operand2, expected_dbuf_cc;
    int actual_fbuf_read, actual_dbuf_ready, actual_dbuf_read;
    int actual_dbuf_pc, actual_dbuf_opcode, actual_dbuf_reg;
    int actual_dbuf_operand1, actual_dbuf_operand2, actual_dbuf_cc;

    // ready bit = 0
    vm.fbuf = (fbuf_t){.ready = 0, .read = 0};
    expected_fbuf_read = 1;
    err = pthread_create(&tid, NULL, id_run, NULL);
    if (err) {
        fprintf(stderr,
                "test_id_br failed: pthread_create(&tid, NULL, id_run, "
                "NULL). err: %d\n",
                err);
        errno = err;
        return;
    }

    err = pthread_join(tid, NULL);
    if (err) {
        fprintf(stderr,
                "test_id_br failed: pthread_join(tid, NULL). "
                "err: %d\n",
                err);
        errno = err;
        return;
    }

    actual_fbuf_read = vm.fbuf.read;
    if (expected_fbuf_read != actual_fbuf_read) {
        fprintf(stderr,
                "test_decode_br failed: expected_fbuf_read: %d, "
                "actual_fbuf_read: %d\n",
                expected_fbuf_read, actual_fbuf_read);
    } else {
        passed_tests++;
    }
}
