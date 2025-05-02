#ifndef ID_H
#define ID_H

#include <stdint.h>

typedef struct dbuf_t {
  int16_t pc;
  int opcode;

  int dr;
  int sr1;
  int sr2;
  int base_r;

  int nzp;

  int imm5;
  int offset6;
  int pcoffset9;
  int pcoffset11;

  int trapvect8;
} dbuf_t;

void *id_exec_cycle(void *arg);

#endif
