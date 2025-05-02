#ifndef IF_H
#define IF_H

#include <stdint.h>

typedef struct fbuf_t {
    int16_t pc;
    int16_t instruction;
} fbuf_t;

void *if_exec_cycle(void *arg);

#endif
