#include "mem_util.h"

#include "../vm.h"
#include "vm_util.h"

mbuf_t init_next_mbuf(ebuf_t ebuf) {
    mbuf_t mbuf;

    mbuf.nop = ebuf.nop;
    mbuf.pc = ebuf.pc;
    mbuf.opcode = ebuf.opcode;
    mbuf.result = ebuf.result;
    mbuf.reg = ebuf.reg;

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
        set_mbuf(*mbuf);
    } else {
        vm.wb_stay = 0;
    }
    pthread_mutex_unlock(&vm.wb_stay_mutex);
}
