#include "vm.h"

vm_t vm;

void vm_init() {
    vm.pc = 0x3000;
    vm.psr = (int16_t)(1 << 15);

    vm.fbuf.ready = 0;
    vm.dbuf.ready = 0;
    vm.ebuf.ready = 0;
    vm.mbuf.ready = 0;

    vm.fbuf.read = 0;
    vm.dbuf.read = 0;
    vm.ebuf.read = 0;
    vm.mbuf.read = 0;

    pthread_mutex_init(&vm.mem_mutex, NULL);
    pthread_mutex_init(&vm.reg_mutex, NULL);
    pthread_mutex_init(&vm.pc_mutex, NULL);
    pthread_mutex_init(&vm.cc_mutex, NULL);
    pthread_mutex_init(&vm.psr_mutex, NULL);

    pthread_mutex_init(&vm.fbuf_mutex, NULL);
    pthread_mutex_init(&vm.dbuf_mutex, NULL);
    pthread_mutex_init(&vm.ebuf_mutex, NULL);
    pthread_mutex_init(&vm.mbuf_mutex, NULL);

    pthread_cond_init(&vm.fbuf_read_cond, NULL);
}

void vm_teardown() {
    pthread_mutex_destroy(&vm.mem_mutex);
    pthread_mutex_destroy(&vm.reg_mutex);
    pthread_mutex_destroy(&vm.pc_mutex);
    pthread_mutex_destroy(&vm.cc_mutex);
    pthread_mutex_destroy(&vm.psr_mutex);

    pthread_mutex_destroy(&vm.fbuf_mutex);
    pthread_mutex_destroy(&vm.dbuf_mutex);
    pthread_mutex_destroy(&vm.ebuf_mutex);
    pthread_mutex_destroy(&vm.mbuf_mutex);

    pthread_cond_destroy(&vm.fbuf_read_cond);
}