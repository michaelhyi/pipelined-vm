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
    int16_t n = 0;
    int16_t expected = 0;
    int16_t actual = bit_range(n, 15, 12);
    int expected_errno = EINVAL;
    int actual_errno = errno;
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0, 15, 12). expected %d, "
                "actual: %d\n",
                expected, actual);
    } else {
        passed_tests++;
    }
    if (expected_errno != actual_errno) {
        fprintf(
            stderr,
            "test_bit_range failed: bit_range(0, 15, 12). expected errno: %d, "
            "actual errno: %d\n",
            expected_errno, actual_errno);
    } else {
        passed_tests++;
    }
    errno = 0;

    actual = bit_range(n, 0, 16);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0, 0, 16). expected %d, "
                "actual: %d\n",
                expected, actual);
    } else {
        passed_tests++;
    }
    if (expected_errno != actual_errno) {
        fprintf(
            stderr,
            "test_bit_range: bit_range(0, 0, 16). expected %d, actual: %d\n",
            expected_errno, actual_errno);
    } else {
        passed_tests++;
    }
    errno = 0;

    n = (int16_t)0xF000;
    expected = (int16_t)0xF;
    actual = bit_range(n, 12, 15);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_bit_range failed: bit_range(0xF000, 12, 15). expected %x, "
            "actual: %x\n",
            expected, actual);
    } else {
        passed_tests++;
    }

    n = (int16_t)0x0F00;
    expected = (int16_t)0xF;
    actual = bit_range(n, 8, 11);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0x0F00, 8, 11) expected %x, "
                "got %x\n",
                expected, actual);
    } else {
        passed_tests++;
    }

    n = (int16_t)0x000F;
    expected = (int16_t)0xF;
    actual = bit_range(n, 0, 3);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0x000F, 0, 3) expected %x, "
                "got %x\n",
                expected, actual);
    } else {
        passed_tests++;
    }

    n = (int16_t)0x00F8;
    expected = (int16_t)0xF;
    actual = bit_range(n, 3, 6);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0x00F8, 3, 6) expected %x, "
                "got %x\n",
                expected, actual);
    } else {
        passed_tests++;
    }

    n = (int16_t)0x0010;
    expected = 1;
    actual = bit_range(n, 4, 4);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range failed: bit_range(0x0010, 4, 4) expected %x, "
                "got %x\n",
                expected, actual);
    } else {
        passed_tests++;
    }
}

static void test_sign_extend() {
    int16_t n = 0;
    int16_t expected = 0;
    int16_t actual = sign_extend(n, 16);
    int16_t expected_errno = EINVAL;
    int actual_errno = errno;
    if (expected != actual) {
        fprintf(stderr,
                "test_sign_extend failed: sign_extend(0, 16). expected %x, "
                "actual: %x\n",
                expected, actual);
    } else {
        passed_tests++;
    }
    if (expected_errno != actual_errno) {
        fprintf(
            stderr,
            "test_sign_extend failed: sign_extend(0, 16). expected errno: %d, "
            "actual errno: %d\n",
            expected_errno, actual_errno);
    } else {
        passed_tests++;
    }
    errno = 0;

    n = (int16_t)0x0FF2;
    expected = (int16_t)0xFFF2;
    actual = sign_extend(n, 12);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_sign_extend failed: sign_extend(0x0FF2, 12). expected %d, "
            "actual: %d\n",
            expected, actual);
    } else {
        passed_tests++;
    }

    n = (int16_t)0x00F2; // 11110010 => -14 if sign-extended from 8 bits
    expected = -14;
    actual = sign_extend(n, 8);
    if (expected != actual) {
        fprintf(stderr,
                "test_sign_extend failed: sign_extend(0x00F2, 8). expected %x, "
                "actual: %x\n",
                expected, actual);
    } else {
        passed_tests++;
    }

    n = (int16_t)0x003E; // 00111110 => still 62 if sign-extended from 8 bits
    expected = 62;
    actual = sign_extend(n, 8);
    if (expected != actual) {
        fprintf(stderr,
                "test_sign_extend failed: sign_extend(0x003E, 8). expected %x, "
                "actual: %x\n",
                expected, actual);
    } else {
        passed_tests++;
    }
}
