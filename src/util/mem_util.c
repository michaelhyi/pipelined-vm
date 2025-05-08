#include "mem_util.h"

#include <stdlib.h>
#include <string.h>

#include "../vm.h"

/**
 * Sets `mbuf` unconditionally.
 *
 * @param mbuf pointer to mbuf to set
 */
static void set_mbuf(mbuf_t *mbuf);

ebuf_t get_ebuf(void) {
    ebuf_t ebuf;

    pthread_mutex_lock(&vm.ebuf_mutex);
    ebuf = vm.ebuf;
    pthread_mutex_unlock(&vm.ebuf_mutex);

    return ebuf;
}

void send_bubble_to_wb(void) {
    pthread_mutex_lock(&vm.mbuf_mutex);
    vm.mbuf.nop = 1;
    pthread_mutex_unlock(&vm.mbuf_mutex);
}

mbuf_t *init_mbuf(ebuf_t ebuf) {
    mbuf_t *mbuf = malloc(sizeof(mbuf_t));

    mbuf->nop = ebuf.nop;
    mbuf->pc = ebuf.pc;
    mbuf->opcode = ebuf.opcode;
    mbuf->result = ebuf.result;
    mbuf->reg = ebuf.reg;

    return mbuf;
}

int16_t mem_get(uint16_t addr) {
    int16_t data;

    pthread_mutex_lock(&vm.mem_mutex);
    data = vm.mem[addr];
    pthread_mutex_unlock(&vm.mem_mutex);

    return data;
}

void mem_set(uint16_t addr, int16_t data) {
    pthread_mutex_lock(&vm.mem_mutex);
    vm.mem[addr] = data;
    pthread_mutex_unlock(&vm.mem_mutex);
}

void stall_pipeline(void) {
    pthread_mutex_lock(&vm.fbuf_stay_mutex);
    vm.fbuf_stay = 1;
    pthread_mutex_unlock(&vm.fbuf_stay_mutex);

    pthread_mutex_lock(&vm.dbuf_stay_mutex);
    vm.dbuf_stay = 1;
    pthread_mutex_unlock(&vm.dbuf_stay_mutex);

    pthread_mutex_lock(&vm.ebuf_stay_mutex);
    vm.ebuf_stay = 1;
    pthread_mutex_unlock(&vm.ebuf_stay_mutex);

    pthread_mutex_lock(&vm.mbuf_stay_mutex);
    vm.mbuf_stay = 1;
    pthread_mutex_unlock(&vm.mbuf_stay_mutex);
}

void update_mbuf(mbuf_t *mbuf) {
    pthread_mutex_lock(&vm.mbuf_stay_mutex);
    if (!vm.mbuf_stay) {
        set_mbuf(mbuf);
    } else {
        vm.mbuf_stay = 0;
    }
    pthread_mutex_unlock(&vm.mbuf_stay_mutex);
}

static void set_mbuf(mbuf_t *mbuf) {
    pthread_mutex_lock(&vm.mbuf_mutex);
    memcpy(&vm.mbuf, mbuf, sizeof(mbuf_t));
    pthread_mutex_unlock(&vm.mbuf_mutex);
}
