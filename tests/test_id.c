#include "test_id.h"

#include <pthread.h>
#include <stdio.h>

#include "../src/id.h"
#include "../src/vm.h"

const int32_t INSTRUCTIONS[] = {
    0b0001010000000001, 0b0001010000111101, 0b0101010000000001,
    0b0101010000111101, 0b1001011100111111, 0b0000110100000001,
    0b1100000111000000, 0b0100100000011111, 0b0100000110000000,
    0b0010101000000000, 0b1010011000011110, 0b0110100000000001,
    0b1110000000000011, 0b0011010000001111, 0b1011010001101010,
    0b0111100100000000, 0b1111000000100101};

// TODO: need to finish
void test_id_exec_cycle(int *passed_tests, int *total_tests) {
  const int num_instructions = sizeof(INSTRUCTIONS) / sizeof(int);

  for (int i = 0; i < num_instructions; i++) {
    pthread_t tid;
    dbuf_t dbuf;

    vm.fbuf.pc = 0x3001;
    vm.fbuf.instruction = INSTRUCTIONS[i];

    if (pthread_create(&tid, NULL, id_exec_cycle, (void *)&dbuf)) {
      fprintf(stderr, "failed to create thread %ld\n", tid);
      return;
    }
    (*passed_tests)++;

    if (pthread_join(tid, NULL)) {
      fprintf(stderr, "failed to join thread %ld\n", tid);
      return;
    }
    (*passed_tests)++;

    if (dbuf.pc != 0x3001) {
      fprintf(stderr, "expected dbuf pc: %x, actual dbuf pc: %x\n", 0x3001,
              dbuf.pc);
      return;
    }
    (*passed_tests)++;
  }
}
