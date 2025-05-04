#include "test_if.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include "../../src/stages/if.h"
#include "../../src/vm.h"
#include "../driver.h"

void test_if_run() {
    pthread_t tid;
    fbuf_t fbuf;
    int err;
    void *thread_ret_val;

    /* error case: if_run(NULL) */
    err = pthread_create(&tid, NULL, if_run, NULL);
    if (err) {
        fprintf(stderr,
                "test_if_run failed: pthread_create(&tid, NULL, if_run, NULL). "
                "err: %d\n",
                err);
        errno = err;
        return;
    }

    err = pthread_join(tid, &thread_ret_val);
    if (err) {
        fprintf(
            stderr,
            "test_if_run failed: pthread_join(tid, &thread_ret_val). err: %d\n",
            err);
        errno = err;
        return;
    }

    int expected_errno = EINVAL;
    int actual_errno = (int)(intptr_t)thread_ret_val;
    if (expected_errno != actual_errno) {
        fprintf(stderr,
                "test_if_run failed: if_run(NULL). expected_errno: %d, "
                "actual_errno: %d\n",
                expected_errno, actual_errno);
    } else {
        passed_tests++;
    }

    vm.pc = 0x3000;
    vm.mem[0x3000] = 0x0000;
    vm.mem[0x3001] = 0x0001;
    vm.mem[0x3002] = 0x0002;
    vm.mem[0x3003] = 0x0003;
    vm.mem[0x3004] = 0x0004;

    for (int i = 0; i < 5; i++) {
        err = pthread_create(&tid, NULL, if_run, &fbuf);
        if (err) {
            fprintf(stderr,
                    "test_if_run failed: pthread_create(&tid, NULL, if_run, "
                    "&fbuf). err: %d\n",
                    err);
            errno = err;
            return;
        }

        err = pthread_join(tid, NULL);
        if (err) {
            fprintf(stderr,
                    "test_if_run failed: pthread_join(tid, NULL). err: %d\n",
                    err);
            errno = err;
            return;
        }

        int expected_fbuf_ready = 1;
        int actual_fbuf_ready = fbuf.ready;
        if (expected_fbuf_ready != actual_fbuf_ready) {
            fprintf(stderr,
                    "test_if_run failed: if_run(&fbuf). expected_fbuf_ready: "
                    "%d, actual_fbuf_ready: %d\n",
                    expected_fbuf_ready, actual_fbuf_ready);
        } else {
            passed_tests++;
        }

        int expected_fbuf_pc = 0x3000 + i;
        int actual_fbuf_pc = fbuf.pc;
        if (expected_fbuf_pc != actual_fbuf_pc) {
            fprintf(stderr,
                    "test_if_run failed: if_run(&fbuf). expected_fbuf_pc: "
                    "0x%X, actual_fbuf_pc: 0x%X\n",
                    expected_fbuf_pc, actual_fbuf_pc);
        } else {
            passed_tests++;
        }

        int expected_fbuf_ir = 0x0000 + i;
        int actual_fbuf_ir = fbuf.ir;
        if (expected_fbuf_ir != actual_fbuf_ir) {
            fprintf(stderr,
                    "test_if_run failed: if_run(&fbuf). expected_fbuf_ir: "
                    "0x%X, actual_fbuf_ir: 0x%X\n",
                    expected_fbuf_ir, actual_fbuf_ir);
        } else {
            passed_tests++;
        }

        int expected_vm_pc = 0x3000 + i + 1;
        int actual_vm_pc = vm.pc;
        if (expected_vm_pc != actual_vm_pc) {
            fprintf(stderr,
                    "test_if_run failed: if_run(&fbuf). expected_vm_pc: 0x%X, "
                    "actual_vm_pc: 0x%X\n",
                    expected_vm_pc, actual_vm_pc);
        } else {
            passed_tests++;
        }
    }
}
