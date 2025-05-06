#include "vm.h"

#include <stdio.h>

#include "isa.h"
#include "stages/ex.h"
#include "stages/id.h"
#include "stages/if.h"
#include "stages/mem.h"
#include "stages/wb.h"

#include "util/bitops.h"

vm_t vm;

void vm_init() {
    vm.pc = 0x3000;
    vm.psr = (int16_t)(1 << 15);

    vm.fbuf.nop = 1;
    vm.dbuf.nop = 1;
    vm.ebuf.nop = 1;
    vm.mbuf.nop = 1;

    vm.fbuf_nop = 0;

    pthread_mutex_init(&vm.mem_mutex, NULL);
    pthread_mutex_init(&vm.reg_mutex, NULL);
    pthread_mutex_init(&vm.pc_mutex, NULL);
    pthread_mutex_init(&vm.cc_mutex, NULL);
    pthread_mutex_init(&vm.psr_mutex, NULL);

    pthread_mutex_init(&vm.fbuf_mutex, NULL);
    pthread_mutex_init(&vm.dbuf_mutex, NULL);
    pthread_mutex_init(&vm.ebuf_mutex, NULL);
    pthread_mutex_init(&vm.mbuf_mutex, NULL);

    pthread_mutex_init(&vm.fbuf_nop_mutex, NULL);

    pthread_mutex_init(&vm.fbuf_stay_mutex, NULL);
    pthread_mutex_init(&vm.dbuf_stay_mutex, NULL);
    pthread_mutex_init(&vm.ebuf_stay_mutex, NULL);
    pthread_mutex_init(&vm.mbuf_stay_mutex, NULL);

    pthread_barrier_init(&vm.pipeline_cycle_barrier, NULL, NUM_PIPELINE_STAGES);
}

void vm_run() {
    vm.reg[0] = 0x2110;
    // mem[0x2200]
    vm.mem[0x3000] = OP_STI << 12;
    vm.mem[0x3001] = 0x2200;

    for (int i = 0; i < 10; i++) {
        printf("cycle: %d\n", i + 1);
        pthread_t if_tid;
        pthread_t id_tid;
        pthread_t ex_tid;
        pthread_t mem_tid;
        pthread_t wb_tid;

        pthread_create(&if_tid, NULL, if_run, NULL);
        pthread_create(&id_tid, NULL, id_run, NULL);
        pthread_create(&ex_tid, NULL, ex_run, NULL);
        pthread_create(&mem_tid, NULL, mem_run, NULL);
        pthread_create(&wb_tid, NULL, wb_run, NULL);

        pthread_join(if_tid, NULL);
        pthread_join(id_tid, NULL);
        pthread_join(ex_tid, NULL);
        pthread_join(mem_tid, NULL);
        pthread_join(wb_tid, NULL);
    }
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

    pthread_mutex_destroy(&vm.fbuf_nop_mutex);

    pthread_mutex_destroy(&vm.fbuf_stay_mutex);
    pthread_mutex_destroy(&vm.dbuf_stay_mutex);
    pthread_mutex_destroy(&vm.ebuf_stay_mutex);
    pthread_mutex_destroy(&vm.mbuf_stay_mutex);

    pthread_barrier_destroy(&vm.pipeline_cycle_barrier);
}
