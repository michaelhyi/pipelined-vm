#include "test_if.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "../../src/stages/if.h"
#include "../../src/vm.h"
#include "../driver.h"

void test_if_run() {
    pthread_t tid;
    int err;

    vm.pc = 0x3000;
    vm.mem[0x3000] = 0x0000;
    vm.mem[0x3001] = 0x0001;
    vm.mem[0x3002] = 0x0002;
    vm.mem[0x3003] = 0x0003;
    vm.mem[0x3004] = 0x0004;

    for (int i = 0; i < 5; i++) {
        err = pthread_create(&tid, NULL, if_run, NULL);
        if (err) {
            fprintf(stderr,
                    "test_if_run failed: pthread_create(&tid, NULL, if_run, "
                    "NULL). err: %d\n",
                    err);
            errno = err;
            return;
        }

        pthread_mutex_lock(&vm.fbuf_mutex);
        sleep(1);
        vm.fbuf.read = 1;
        pthread_cond_signal(&vm.fbuf_read_cond);
        pthread_mutex_unlock(&vm.fbuf_mutex);

        err = pthread_join(tid, NULL);
        if (err) {
            fprintf(stderr,
                    "test_if_run failed: pthread_join(tid, NULL). err: %d\n",
                    err);
            errno = err;
            return;
        }

        int expected_fbuf_ready = 1;
        int actual_fbuf_ready = vm.fbuf.ready;
        if (expected_fbuf_ready != actual_fbuf_ready) {
            fprintf(stderr,
                    "test_if_run failed: if_run(NULL). expected_fbuf_ready: "
                    "%d, actual_fbuf_ready: %d\n",
                    expected_fbuf_ready, actual_fbuf_ready);
        } else {
            passed_tests++;
        }

        int expected_fbuf_read = 0;
        int actual_fbuf_read = vm.fbuf.read;
        if (expected_fbuf_read != actual_fbuf_read) {
            fprintf(stderr,
                    "test_if_run failed: if_run(NULL). expected_fbuf_read: "
                    "%d, actual_fbuf_read: %d\n",
                    expected_fbuf_read, actual_fbuf_read);
        } else {
            passed_tests++;
        }

        int expected_fbuf_pc = 0x3000 + i;
        int actual_fbuf_pc = vm.fbuf.pc;
        if (expected_fbuf_pc != actual_fbuf_pc) {
            fprintf(stderr,
                    "test_if_run failed: if_run(NULL). expected_fbuf_pc: "
                    "0x%X, actual_fbuf_pc: 0x%X\n",
                    expected_fbuf_pc, actual_fbuf_pc);
        } else {
            passed_tests++;
        }

        int expected_fbuf_ir = 0x0000 + i;
        int actual_fbuf_ir = vm.fbuf.ir;
        if (expected_fbuf_ir != actual_fbuf_ir) {
            fprintf(stderr,
                    "test_if_run failed: if_run(NULL). expected_fbuf_ir: "
                    "0x%X, actual_fbuf_ir: 0x%X\n",
                    expected_fbuf_ir, actual_fbuf_ir);
        } else {
            passed_tests++;
        }

        int expected_vm_pc = 0x3000 + i + 1;
        int actual_vm_pc = vm.pc;
        if (expected_vm_pc != actual_vm_pc) {
            fprintf(stderr,
                    "test_if_run failed: if_run(NULL). expected_vm_pc: 0x%X, "
                    "actual_vm_pc: 0x%X\n",
                    expected_vm_pc, actual_vm_pc);
        } else {
            passed_tests++;
        }
    }
}
