#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/vm.h"
#include "stages/test_id.h"
#include "stages/test_if.h"
#include "util/test_bitops.h"
#include "util/test_id_util.h"

// TODO: descriptive test cases per output line
// TODO: no need to test for thread failure

#define TOTAL_TESTS 56

int main(void) {
    system_init();

    int *passed_tests = malloc(sizeof(int));
    if (!passed_tests) {
        fprintf(stderr, "failed to initialize tests\n");
        return 1;
    }

    errno = 0;
    test_if_run(passed_tests);
    if (errno) {
        fprintf(stderr, "error while running test_if_exec_cycle; errno: %d\n",
                errno);
        // TODO: total tests wont be fully counted here
        printf("test results: %d/%d passed\n", *passed_tests, TOTAL_TESTS);

        free(passed_tests);
        return 1;
    }

    errno = 0;
    test_id_util(passed_tests);
    if (errno) {
        fprintf(stderr, "test_id_util failed: errno: %d\n", errno);
        // TODO: total tests wont be fully counted here
        printf("test results: %d/%d passed\n", *passed_tests, TOTAL_TESTS);

        free(passed_tests);
        return 1;
    }

    errno = 0;
    // test_id_exec_cycle(passed_tests, total_tests);
    if (errno) {
        fprintf(stderr, "error while running test_id_exec_cycle; errno: %d\n",
                errno);
        printf("test results: %d/%d passed\n", *passed_tests, TOTAL_TESTS);

        free(passed_tests);
        return 1;
    }

    errno = 0;
    test_bitops(passed_tests);
    if (errno) {
        // TODO: this runs every time
        fprintf(stderr, "error while running test_util; errno: %d\n", errno);
        printf("test results: %d/%d passed\n", *passed_tests, TOTAL_TESTS);
        free(passed_tests);
        return 1;
    }

    printf("test results: %d/%d passed\n", *passed_tests, TOTAL_TESTS);
    if (*passed_tests != TOTAL_TESTS) {
        free(passed_tests);
        return 1;
    }

    free(passed_tests);
    return 0;
}
