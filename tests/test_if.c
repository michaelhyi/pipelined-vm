#include "test_if.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include "../src/if.h"
#include "../src/vm.h"

void test_if_run(int *passed_tests) {
    if (!passed_tests) {
        fprintf(stderr, "test_if_run failed: passed_tests must "
                        "be non-null\n");
        errno = EINVAL;
        return;
    }

    vm.mem[0x3000] = 0x0000;
    vm.mem[0x3001] = 0x0001;
    vm.mem[0x3002] = 0x0002;
    vm.mem[0x3003] = 0x0003;
    vm.mem[0x3004] = 0x0004;

    for (int i = 0; i < 5; i++) {
        pthread_t tid;
        fbuf_t fbuf;

        int err = pthread_create(&tid, NULL, if_run, (void *)&fbuf);
        if (err) {
            fprintf(stderr, "test_if_run failed: failed to create thread %ld\n",
                    tid);
            errno = err;
            return;
        }

        err = pthread_join(tid, NULL);
        if (err) {
            fprintf(stderr, "test_if_run failed: failed to join thread %ld\n",
                    tid);
            errno = err;
            return;
        }

        int expected_fbuf_pc = 0x3000 + i;
        int actual_fbuf_pc = fbuf.pc;
        if (expected_fbuf_pc != actual_fbuf_pc) {
            fprintf(stderr,
                    "test_if_run failed: expected fbuf.pc: %x, actual fbuf.pc: "
                    "%x\n",
                    0x3000 + i, fbuf.pc);
        } else {
            (*passed_tests)++;
        }

        int expected_fbuf_ir = 0x0000 + i;
        int actual_fbuf_ir = fbuf.ir;
        if (expected_fbuf_ir != actual_fbuf_ir) {
            fprintf(stderr,
                    "test_if_run failed: expected_fbuf_ir: %x, actual_fbuf_ir: "
                    "%x\n",
                    0x0000 + i, fbuf.ir);
        } else {
            (*passed_tests)++;
        }

        int expected_vm_pc = 0x3000 + i + 1;
        int actual_vm_pc = vm.pc;
        if (expected_vm_pc != actual_vm_pc) {
            fprintf(
                stderr,
                "test_if_run failed: expected_vm_pc: %x, actual_vm_pc: %x\n",
                0x3000 + i + 1, fbuf.pc);
        } else {
            (*passed_tests)++;
        }
    }
}
