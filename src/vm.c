#include "vm.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "loader.h"
#include "pipeline_table.h"
#include "stages/ex.h"
#include "stages/id.h"
#include "stages/if.h"
#include "stages/mem.h"
#include "stages/wb.h"
#include "util/vm_util.h"

vm_t vm;
size_t clock_cycle_counter;

void vm_init(int test) {
    memset(&vm, 0, sizeof(vm_t));
    clock_cycle_counter = 0;

    vm.pc = 0x3000;
    vm.psr = (int16_t)(1 << 15);

    vm.running = 1;

    vm.fbuf.nop = 1;
    vm.dbuf.nop = 1;
    vm.ebuf.nop = 1;
    vm.mbuf.nop = 1;

    init_mutexes_and_barriers();

    if (!test) {
        loader_run();
    }

    pipeline_table_init();
}

void vm_run(void) {
    while (vm.running) {
        pipeline_table_add_row();

        pthread_t if_tid;
        pthread_t id_tid;
        pthread_t ex_tid;
        pthread_t mem_tid;
        pthread_t wb_tid;

        void *if_err, *id_err, *ex_err, *mem_err, *wb_err;

        pthread_create(&if_tid, NULL, if_run, NULL);
        pthread_create(&id_tid, NULL, id_run, NULL);
        pthread_create(&ex_tid, NULL, ex_run, NULL);
        pthread_create(&mem_tid, NULL, mem_run, NULL);
        pthread_create(&wb_tid, NULL, wb_run, NULL);

        pthread_join(if_tid, &if_err);
        pthread_join(id_tid, &id_err);
        pthread_join(ex_tid, &ex_err);
        pthread_join(mem_tid, &mem_err);
        pthread_join(wb_tid, &wb_err);

        assert(!((intptr_t)if_err));
        assert(!((intptr_t)id_err));
        assert(!((intptr_t)ex_err));
        assert(!((intptr_t)mem_err));
        assert(!((intptr_t)wb_err));

        clock_cycle_counter++;
    }
}

void vm_teardown(int test) {
    destroy_mutexes_and_barriers();

    printf("vm ran %ld clock cycles\n", clock_cycle_counter);
    pipeline_table_print();
    pipeline_table_teardown();

    if (!test) {
        memory_viewer_run();
    }
}
