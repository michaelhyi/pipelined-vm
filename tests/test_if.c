#include "test_if.h"

#include <pthread.h>
#include <stdio.h>

#include "../src/if.h"
#include "../src/vm.h"

void test_if_exec_cycle(int *passed_tests, int *total_tests) {
  *total_tests += (5 * 5);

  vm.mem[0x3000] = 0x0000;
  vm.mem[0x3001] = 0x0001;
  vm.mem[0x3002] = 0x0002;
  vm.mem[0x3003] = 0x0003;
  vm.mem[0x3004] = 0x0004;

  for (int i = 0; i < 5; i++) {
    pthread_t tid;
    fbuf_t fbuf;

    if (pthread_create(&tid, NULL, if_exec_cycle, (void *)&fbuf)) {
      fprintf(stderr, "failed to create thread %ld\n", tid);
    }
    (*passed_tests)++;

    if (pthread_join(tid, NULL)) {
      fprintf(stderr, "failed to join thread %ld\n", tid);
    }
    (*passed_tests)++;

    if (fbuf.pc != 0x3000 + i) {
      fprintf(stderr, "expected fbuf pc: %x, actual fbuf pc: %x\n", 0x3000 + i,
              fbuf.pc);
    }
    (*passed_tests)++;

    if (fbuf.instruction != 0x0000 + i) {
      fprintf(stderr,
              "expected fbuf instruction: %x, actual fbuf instruction: %x\n",
              0x0000 + i, fbuf.instruction);
    }
    (*passed_tests)++;

    if (vm.pc != 0x3000 + i + 1) {
      fprintf(stderr, "expected vm pc: %x, actual vm pc: %x\n", 0x3000 + i + 1,
              fbuf.pc);
    }
    (*passed_tests)++;
  }
}
