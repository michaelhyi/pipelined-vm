#include "test_util.h"

#include <errno.h>
#include <stdio.h>

#include "../src/util.h"

static void test_bit_range(int *passed_tests, int *total_tests);
static void test_sign_extend(int *passed_tests, int *total_tests);

void test_util(int *passed_tests, int *total_tests) {
    if (!passed_tests || !total_tests) {
        fprintf(
            stderr,
            "test_util: passed_tests and total_tests both must be non-null\n");
        errno = EINVAL;
        return;
    }

    errno = 0;
    test_bit_range(passed_tests, total_tests);

    errno = 0;
    test_sign_extend(passed_tests, total_tests);
}

static void test_bit_range(int *passed_tests, int *total_tests) {
    if (!passed_tests || !total_tests) {
        fprintf(stderr, "test_bit_range: passed_tests and total_tests both "
                        "must be non-null\n");
        errno = EINVAL;
        return;
    }

    *total_tests += 9;

    int16_t n = 0;
    int16_t expected = 0;
    int16_t actual = bit_range(n, 15, 12);
    int16_t expected_errno = EINVAL;
    int actual_errno = errno;
    if (expected != actual) {
        fprintf(
            stderr,
            "test_bit_range: bit_range(0, 15, 12). expected 0, actual: %d\n",
            actual);
    } else {
        (*passed_tests)++;
    }
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_bit_range: bit_range(0, 15, 12). expected errno: 22, "
                "actual errno: %d\n",
                actual_errno);
    } else {
        (*passed_tests)++;
    }
    errno = 0;

    actual = bit_range(n, 0, 16);
    expected_errno = EINVAL;
    actual_errno = errno;
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range: bit_range(0, 0, 16). expected 0, actual: %d\n",
                actual);
    } else {
        (*passed_tests)++;
    }
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_bit_range: bit_range(0, 0, 16). expected 0, actual: %d\n",
                actual);
    } else {
        (*passed_tests)++;
    }
    errno = 0;

    n = (int16_t)0xF000;
    expected = (int16_t)0xF;
    actual = bit_range(n, 12, 15);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range: bit_range(0xF000, 12, 15). expected 0xF, "
                "actual: %d\n",
                actual);
    } else {
        (*passed_tests)++;
    }

    n = (int16_t)0x0F00;
    expected = (int16_t)0xF;
    actual = bit_range(n, 8, 11);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_bit_range: bit_range(0x0F00, 8, 11) expected 0xF, got %d\n",
            actual);
    } else {
        (*passed_tests)++;
    }

    n = (int16_t)0x000F;
    expected = (int16_t)0xF;
    actual = bit_range(n, 0, 3);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_bit_range: bit_range(0x000F, 0, 3) expected 0xF, got %d\n",
            actual);
    } else {
        (*passed_tests)++;
    }

    n = (int16_t)0x00F8;
    expected = (int16_t)0xF;
    actual = bit_range(n, 3, 6);
    if (expected != actual) {
        fprintf(
            stderr,
            "test_bit_range: bit_range(0x00F8, 3, 6) expected 0b1111, got %d\n",
            actual);
    } else {
        (*passed_tests)++;
    }

    n = (int16_t)0x0010;
    expected = 1;
    actual = bit_range(n, 4, 4);
    if (expected != actual) {
        fprintf(stderr,
                "test_bit_range: bit_range(0x0010, 4, 4) expected 1, got %d\n",
                actual);
    } else {
        (*passed_tests)++;
    }
}

static void test_sign_extend(int *passed_tests, int *total_tests) {
    if (!passed_tests || !total_tests) {
        fprintf(stderr, "test_sign_extend: passed_tests and total_tests both "
                        "must be non-null\n");
        errno = EINVAL;
        return;
    }

    *total_tests += 5;

    int16_t n = 0;
    int16_t expected = 0;
    int16_t actual = sign_extend(n, 16);
    int16_t expected_errno = EINVAL;
    int actual_errno = errno;
    if (expected != actual) {
        fprintf(
            stderr,
            "test_sign_extend: sign_extend(0, 16). expected 0, actual: %d\n",
            actual);
    } else {
        (*passed_tests)++;
    }
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_sign_extend: sign_extend(0, 16). expected errno: 22, "
                "actual errno: %d\n",
                actual_errno);
    } else {
        (*passed_tests)++;
    }
    errno = 0;

    n = (int16_t)0x0FF2;
    expected = (int16_t)0xFFF2;
    actual = sign_extend(n, 12);
    if (expected != actual) {
        fprintf(stderr,
                "test_sign_extend: sign_extend(0x0FF2, 12). expected 0xFFF2, "
                "actual: %d\n",
                actual);
    } else {
        (*passed_tests)++;
    }

    n = (int16_t)0x00F2; // 11110010 => -14 if sign-extended from 8 bits
    expected = -14;
    actual = sign_extend(n, 8);
    if (expected != actual) {
        fprintf(stderr,
                "test_sign_extend: sign_extend(0x00F2, 8). expected -14, "
                "actual: %d\n",
                actual);
    } else {
        (*passed_tests)++;
    }

    n = (int16_t)0x003E; // 00111110 => still 62 if sign-extended from 8 bits
    expected = 62;
    actual = sign_extend(n, 8);
    if (expected != actual) {
        fprintf(stderr,
                "test_sign_extend: sign_extend(0x003E, 8). expected 62, "
                "actual: %d\n",
                actual);
    } else {
        (*passed_tests)++;
    }
}
