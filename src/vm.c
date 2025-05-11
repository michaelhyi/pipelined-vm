#include "vm.h"

#include <pthread.h>
#include <stdint.h>
#include <string.h>

#include "stages/ex.h"
#include "stages/id.h"
#include "stages/if.h"
#include "stages/mem.h"
#include "stages/wb.h"

vm_t vm;

void vm_init(void) {
    memset(&vm, 0, sizeof(vm_t));

    vm.pc = 0x3000;
    vm.psr = (int16_t)(1 << 15);

    vm.running = 1;

    vm.fbuf.nop = 1;
    vm.dbuf.nop = 1;
    vm.ebuf.nop = 1;
    vm.mbuf.nop = 1;

    pthread_mutex_init(&vm.running_mutex, NULL);

    pthread_mutex_init(&vm.mem_mutex, NULL);
    pthread_mutex_init(&vm.register_file_mutex, NULL);
    pthread_mutex_init(&vm.pc_mutex, NULL);
    pthread_mutex_init(&vm.cc_mutex, NULL);
    pthread_mutex_init(&vm.psr_mutex, NULL);

    pthread_mutex_init(&vm.fbuf_mutex, NULL);
    pthread_mutex_init(&vm.dbuf_mutex, NULL);
    pthread_mutex_init(&vm.ebuf_mutex, NULL);
    pthread_mutex_init(&vm.mbuf_mutex, NULL);

    pthread_mutex_init(&vm.pc_override_mutex, NULL);
    pthread_mutex_init(&vm.pc_override_signal_mutex, NULL);

    pthread_mutex_init(&vm.id_nop_mutex, NULL);
    pthread_mutex_init(&vm.ex_nop_mutex, NULL);
    pthread_mutex_init(&vm.mem_nop_mutex, NULL);
    pthread_mutex_init(&vm.wb_nop_mutex, NULL);

    pthread_mutex_init(&vm.id_stay_mutex, NULL);
    pthread_mutex_init(&vm.ex_stay_mutex, NULL);
    pthread_mutex_init(&vm.mem_stay_mutex, NULL);
    pthread_mutex_init(&vm.wb_stay_mutex, NULL);

    pthread_barrier_init(&vm.pipeline_cycle_barrier, NULL, NUM_PIPELINE_STAGES);
}

void vm_run(void) {
    while (vm.running) {
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

        // TODO: handle errno
    }
}

void vm_teardown(void) {
    pthread_mutex_destroy(&vm.running_mutex);

    pthread_mutex_destroy(&vm.mem_mutex);
    pthread_mutex_destroy(&vm.register_file_mutex);
    pthread_mutex_destroy(&vm.pc_mutex);
    pthread_mutex_destroy(&vm.cc_mutex);
    pthread_mutex_destroy(&vm.psr_mutex);

    pthread_mutex_destroy(&vm.fbuf_mutex);
    pthread_mutex_destroy(&vm.dbuf_mutex);
    pthread_mutex_destroy(&vm.ebuf_mutex);
    pthread_mutex_destroy(&vm.mbuf_mutex);

    pthread_mutex_destroy(&vm.pc_override_mutex);
    pthread_mutex_destroy(&vm.pc_override_signal_mutex);

    pthread_mutex_destroy(&vm.id_nop_mutex);
    pthread_mutex_destroy(&vm.ex_nop_mutex);
    pthread_mutex_destroy(&vm.mem_nop_mutex);
    pthread_mutex_destroy(&vm.wb_nop_mutex);

    pthread_mutex_destroy(&vm.id_stay_mutex);
    pthread_mutex_destroy(&vm.ex_stay_mutex);
    pthread_mutex_destroy(&vm.mem_stay_mutex);
    pthread_mutex_destroy(&vm.wb_stay_mutex);

    pthread_barrier_destroy(&vm.pipeline_cycle_barrier);
}
