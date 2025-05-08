#include "wb_util.h"

#include "../isa.h"

void write_to_register(mbuf_t mbuf) {
    pthread_mutex_lock(&vm.register_file_mutex);
    vm.register_file[mbuf.reg].data = mbuf.result;
    vm.register_file[mbuf.reg].busy_counter--;
    pthread_mutex_unlock(&vm.register_file_mutex);
}

void set_cc(mbuf_t mbuf) {
    if (mbuf.opcode != OP_LEA) {
        pthread_mutex_lock(&vm.cc_mutex);

        if (mbuf.result < 0) {
            vm.cc = 4;
        } else if (mbuf.result == 0) {
            vm.cc = 2;
        } else {
            vm.cc = 1;
        }

        pthread_mutex_unlock(&vm.cc_mutex);
    }
}

void save_pc_in_r7(mbuf_t mbuf) {
    pthread_mutex_lock(&vm.register_file_mutex);
    vm.register_file[7].data = (int16_t)mbuf.pc;
    vm.register_file[7].busy_counter--;
    pthread_mutex_unlock(&vm.register_file_mutex);
}
