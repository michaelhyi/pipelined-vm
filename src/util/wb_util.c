#include "wb_util.h"

#include "../isa.h"

void set_cc(mbuf_t mbuf) {
    if (mbuf.opcode != OP_LEA) {
        pthread_mutex_lock(&vm.cc_mutex);

        if (mbuf.result < 0) {
            vm.cc.data = 4;
        } else if (mbuf.result == 0) {
            vm.cc.data = 2;
        } else {
            vm.cc.data = 1;
        }

        vm.cc.busy_counter--;

        pthread_mutex_unlock(&vm.cc_mutex);
    }
}
