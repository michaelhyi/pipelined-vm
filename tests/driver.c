#include <stdio.h>
#include <stdlib.h>

#include "../src/vm.h"
#include "test_if.h"

int main(void) {
  system_init();

  int *passed_tests = malloc(sizeof(int));
  if (!passed_tests) {
    fprintf(stderr, "failed to initialize tests\n");
    return 1;
  }

  int *total_tests = malloc(sizeof(int));
  if (!total_tests) {
    free(passed_tests);
    fprintf(stderr, "failed to initialize tests\n");
    return 1;
  }

  test_if_exec_cycle(passed_tests, total_tests);

  printf("test results: %d/%d passed\n", *passed_tests, *total_tests);
  free(passed_tests);
  free(total_tests);

  return 0;
}
