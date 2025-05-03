#include "driver.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/vm.h"
#include "stages/test_if.h"
#include "util/test_bitops.h"
#include "util/test_id_util.h"

int passed_tests;

/**
 * Helper function that initializes the VM and test parameters.
 */
static void test_setup(void);

/**
 * Helper function that tears down tests.
 */
static void test_teardown(void);

int main(void) {
    test_setup();

    errno = 0;
    test_if_run();
    if (errno) {
        fprintf(stderr, "test_if_run failed: errno: %d\n", errno);
        test_teardown();
    }

    errno = 0;
    test_bitops();
    if (errno) {
        fprintf(stderr, "test_bitops failed: errno: %d\n", errno);
        test_teardown();
    }

    errno = 0;
    test_id_util();
    if (errno) {
        fprintf(stderr, "test_id_util failed: errno: %d\n", errno);
        test_teardown();
    }

    test_teardown();
}

static void test_setup() {
    system_init();
    passed_tests = 0;
}

static void test_teardown() {
    printf("\n");
    printf("\n");
    printf("\n");
    printf("test results: %d/%d passed\n", passed_tests, TOTAL_TESTS);
    exit(passed_tests != TOTAL_TESTS);
}
