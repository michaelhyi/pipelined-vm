#include "test_bitops.h"

#include <errno.h>
#include <stdio.h>

#include "../../src/util/bitops.h"
#include "../driver.h"

/**
 * Runs unit tests on bit_range function from "../../src/util/bitops.h".
 */
static void test_bit_range(void);

/**
 * Runs unit tests on sign_extend function from "../../src/util/bitops.h".
 */
static void test_sign_extend(void);

void test_bitops() {
    errno = 0;
    test_bit_range();

    errno = 0;
    test_sign_extend();
}

static void test_bit_range() {
    int16_t n, expected, actual;
    uint16_t lower, upper;
    int expected_errno, actual_errno;

    // invalid: lower > upper
    errno = 0;
    n = 0;
    lower = 15;
    upper = 12;
    expected = 0;
    expected_errno = EINVAL;
    actual = bit_range(n, lower, upper);
    actual_errno = errno;
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(%d, %d, %d): expected: %d, "
                "actual: %d\n",
                n, lower, upper, expected, actual);
    } else {
        passed_tests++;
    }
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(%d, %d, %d): expected_errno: "
                "%d, actual_errno: %d\n",
                n, lower, upper, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // invalid: upper out of range
    errno = 0;
    n = 0;
    lower = 0;
    upper = 16;
    expected = 0;
    expected_errno = EINVAL;
    actual = bit_range(n, lower, upper);
    actual_errno = errno;
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(%d, %d, %d): expected: %d, "
                "actual: %d\n",
                n, lower, upper, expected, actual);
    } else {
        passed_tests++;
    }
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(%d, %d, %d): expected_errno: "
                "%d, actual_errno: %d\n",
                n, lower, upper, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid: bit_length 12–15 of 0xF000
    errno = 0;
    n = (int16_t)0xF000;
    lower = 12;
    upper = 15;
    expected = 0xF;
    actual = bit_range(n, lower, upper);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0x%04X, %d, %d): expected: "
                "0x%X, actual: 0x%X\n",
                n, lower, upper, expected, actual);
    } else {
        passed_tests++;
    }

    // valid: bit_length 8–11 of 0x0F00
    errno = 0;
    n = (int16_t)0x0F00;
    lower = 8;
    upper = 11;
    expected = 0xF;
    actual = bit_range(n, lower, upper);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0x%04X, %d, %d): expected: "
                "0x%X, actual: 0x%X\n",
                n, lower, upper, expected, actual);
    } else {
        passed_tests++;
    }

    // valid: bit_length 0–3 of 0x000F
    errno = 0;
    n = (int16_t)0x000F;
    lower = 0;
    upper = 3;
    expected = 0xF;
    actual = bit_range(n, lower, upper);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0x%04X, %d, %d): expected: "
                "0x%X, actual: 0x%X\n",
                n, lower, upper, expected, actual);
    } else {
        passed_tests++;
    }

    // valid: bit_length 3–6 of 0x00F8
    errno = 0;
    n = (int16_t)0x00F8;
    lower = 3;
    upper = 6;
    expected = 0xF;
    actual = bit_range(n, lower, upper);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0x%04X, %d, %d): expected: "
                "0x%X, actual: 0x%X\n",
                n, lower, upper, expected, actual);
    } else {
        passed_tests++;
    }

    // valid: bit 4 of 0x0010
    errno = 0;
    n = (int16_t)0x0010;
    lower = 4;
    upper = 4;
    expected = 1;
    actual = bit_range(n, lower, upper);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0x%04X, %d, %d): expected: "
                "%d, actual: %d\n",
                n, lower, upper, expected, actual);
    } else {
        passed_tests++;
    }
}

static void test_sign_extend() {
    int16_t n, expected, actual;
    uint16_t bit_length;
    int expected_errno, actual_errno;

    // invalid: bit_length >= 16
    errno = 0;
    n = 0;
    bit_length = 16;
    expected = 0;
    expected_errno = EINVAL;
    actual = sign_extend(n, bit_length);
    actual_errno = errno;
    if (expected != actual) {
        fprintf(stderr,
                "test_sign_extend failed: sign_extend(%d, %d): expected: %d, "
                "actual: %d\n",
                n, bit_length, expected, actual);
    } else {
        passed_tests++;
    }
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_sign_extend failed: sign_extend(%d, %d): expected_errno: "
                "%d, actual_errno: %d\n",
                n, bit_length, expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    // valid: sign-extend 12 bit_length of 0x0FF2
    errno = 0;
    n = (int16_t)0x0FF2;
    bit_length = 12;
    expected = (int16_t)0xFFF2;
    actual = sign_extend(n, bit_length);
    if (expected != actual) {
        fprintf(stderr,
                "test_sign_extend failed: sign_extend(0x%04X, %d): expected: "
                "0x%04X, actual: 0x%04X\n",
                n, bit_length, expected, actual);
    } else {
        passed_tests++;
    }

    // valid: sign-extend 8 bit_length of 0x00F2 => -14
    errno = 0;
    n = (int16_t)0x00F2;
    bit_length = 8;
    expected = -14;
    actual = sign_extend(n, bit_length);
    if (expected != actual) {
        fprintf(stderr,
                "test_sign_extend failed: sign_extend(0x%04X, %d): expected: "
                "%d, actual: %d\n",
                n, bit_length, expected, actual);
    } else {
        passed_tests++;
    }

    // valid: sign-extend 8 bit_length of 0x003E => 62
    errno = 0;
    n = (int16_t)0x003E;
    bit_length = 8;
    expected = 62;
    actual = sign_extend(n, bit_length);
    if (expected != actual) {
        fprintf(stderr,
                "test_sign_extend failed: sign_extend(0x%04X, %d): expected: "
                "%d, actual: %d\n",
                n, bit_length, expected, actual);
    } else {
        passed_tests++;
    }
}
