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

void stall_pipeline(void) {
    pthread_mutex_lock(&vm.id_stay_mutex);
    vm.id_stay = 1;
    pthread_mutex_unlock(&vm.id_stay_mutex);

    pthread_mutex_lock(&vm.ex_stay_mutex);
    vm.ex_stay = 1;
    pthread_mutex_unlock(&vm.ex_stay_mutex);

    pthread_mutex_lock(&vm.mem_stay_mutex);
    vm.mem_stay = 1;
    pthread_mutex_unlock(&vm.mem_stay_mutex);

    pthread_mutex_lock(&vm.wb_stay_mutex);
    vm.wb_stay = 1;
    pthread_mutex_unlock(&vm.wb_stay_mutex);
}

void update_mbuf(mbuf_t *mbuf) {
    pthread_mutex_lock(&vm.wb_stay_mutex);
    if (!vm.wb_stay) {
        set_mbuf(mbuf);
    } else {
        vm.wb_stay = 0;
    }
    pthread_mutex_unlock(&vm.wb_stay_mutex);
}

static void set_mbuf(mbuf_t *mbuf) {
    pthread_mutex_lock(&vm.mbuf_mutex);
    memcpy(&vm.mbuf, mbuf, sizeof(mbuf_t));
    pthread_mutex_unlock(&vm.mbuf_mutex);
}
