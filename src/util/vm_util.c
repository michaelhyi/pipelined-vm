#include "vm_util.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../vm.h"
#include "bitops.h"

/**
 * Returns whether the VM is operating in supervisor or user mode.
 *
 * @returns 1 if supervisor mode, 0 if user mode or on error
 */
static int supervisor_mode(void);

/**
 * Returns whether the given memory address is attempting to access privileged
 * memory.
 *
 * @return 1 if the address is in the system space (0x0000-0x2FFF) or the I/O
 * page (0xFE00-0xFFFF), 0 otherwise
 */
static inline int privileged_mem_addr(uint16_t addr);

static int supervisor_mode(void) {
    pthread_mutex_lock(&vm.psr_mutex);
    int supervisor_mode = !bit_range(vm.psr, 15, 15);
    pthread_mutex_unlock(&vm.psr_mutex);

    if (errno) {
        errno = 0;
        return 0;
    }

    return supervisor_mode;
}

static inline int privileged_mem_addr(uint16_t addr) {
    return addr <= 0x2FFF || addr >= 0xFE00;
}

void init_mutexes_and_barriers(void) {
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

void destroy_mutexes_and_barriers(void) {
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

void memory_viewer_run(void) {
    while (1) {
        uint16_t addr;
        int16_t data;

        printf("enter mem addr: 0x");
        assert(scanf("%hx", &addr) == 1);

        if (!addr) {
            break;
        }

        data = vm.mem[addr];
        printf("mem[0x%hx]=0x%x\n", addr, data);
    }
}

int16_t get_mem(uint16_t addr) {
    (void)(supervisor_mode());
    // if (!supervisor_mode() && privileged_mem_addr(addr)) {
    //     errno = EFAULT;
    //     return 0;
    // }

    pthread_mutex_lock(&vm.mem_mutex);
    int16_t data = vm.mem[addr];
    pthread_mutex_unlock(&vm.mem_mutex);

    return data;
}

void set_mem(uint16_t addr, int16_t data) {
    // if (!supervisor_mode() && privileged_mem_addr(addr)) {
    //     errno = EFAULT;
    //     return;
    // }

    pthread_mutex_lock(&vm.mem_mutex);
    vm.mem[addr] = data;
    pthread_mutex_unlock(&vm.mem_mutex);
}

int16_t get_register_data(uint16_t reg_num) {
    if (reg_num > 7) {
        errno = EINVAL;
        return 0;
    }

    pthread_mutex_lock(&vm.register_file_mutex);
    int16_t data = vm.register_file[reg_num].data;
    pthread_mutex_unlock(&vm.register_file_mutex);

    return data;
}

void set_register_data(uint16_t reg_num, int16_t data) {
    if (reg_num > 7) {
        errno = EINVAL;
        return;
    }

    pthread_mutex_lock(&vm.register_file_mutex);
    // TODO: abstract conditional to function
    if (!vm.register_file[reg_num].busy_counter) {
        errno = EINVAL;
        pthread_mutex_unlock(&vm.register_file_mutex);
        return;
    }

    vm.register_file[reg_num].data = data;
    pthread_mutex_unlock(&vm.register_file_mutex);
}

void increment_busy_counter(uint16_t register_num) {
    pthread_mutex_lock(&vm.register_file_mutex);
    vm.register_file[register_num].busy_counter++;
    pthread_mutex_unlock(&vm.register_file_mutex);
}

void decrement_busy_counter(uint16_t register_num) {
    pthread_mutex_lock(&vm.register_file_mutex);
    vm.register_file[register_num].busy_counter--;
    pthread_mutex_unlock(&vm.register_file_mutex);
}

uint16_t get_pc(void) {
    pthread_mutex_lock(&vm.pc_mutex);
    uint16_t pc = vm.pc;
    pthread_mutex_unlock(&vm.pc_mutex);

    return pc;
}

void set_pc(uint16_t pc) {
    pthread_mutex_lock(&vm.pc_mutex);
    vm.pc = pc;
    pthread_mutex_unlock(&vm.pc_mutex);
}

int16_t get_cc_data(void) {
    pthread_mutex_lock(&vm.cc_mutex);
    int16_t cc = vm.cc.data;
    pthread_mutex_unlock(&vm.cc_mutex);

    return cc;
}

void increment_cc_busy_counter(void) {
    pthread_mutex_lock(&vm.cc_mutex);
    vm.cc.busy_counter++;
    pthread_mutex_unlock(&vm.cc_mutex);
}

void decrement_cc_busy_counter(void) {
    pthread_mutex_lock(&vm.cc_mutex);
    vm.cc.busy_counter--;
    pthread_mutex_unlock(&vm.cc_mutex);
}

int16_t get_psr(void) {
    pthread_mutex_lock(&vm.psr_mutex);
    int16_t psr = vm.psr;
    pthread_mutex_unlock(&vm.psr_mutex);
    return psr;
}

void set_psr(int16_t new_psr) {
    pthread_mutex_lock(&vm.psr_mutex);
    vm.psr = new_psr;
    pthread_mutex_unlock(&vm.psr_mutex);
}

uint16_t get_saved_ssp(void) {
    pthread_mutex_lock(&vm.saved_ssp_mutex);
    uint16_t saved_ssp = vm.saved_ssp;
    pthread_mutex_unlock(&vm.saved_ssp_mutex);
    return saved_ssp;
}

void set_saved_ssp(uint16_t new_saved_ssp) {
    pthread_mutex_lock(&vm.saved_ssp_mutex);
    vm.saved_ssp = new_saved_ssp;
    pthread_mutex_unlock(&vm.saved_ssp_mutex);
}

uint16_t get_saved_usp(void) {
    pthread_mutex_lock(&vm.saved_usp_mutex);
    uint16_t saved_usp = vm.saved_usp;
    pthread_mutex_unlock(&vm.saved_usp_mutex);
    return saved_usp;
}

void set_saved_usp(uint16_t new_saved_usp) {
    pthread_mutex_lock(&vm.saved_usp_mutex);
    vm.saved_usp = new_saved_usp;
    pthread_mutex_unlock(&vm.saved_usp_mutex);
}

void set_running(uint16_t new_running) {
    pthread_mutex_lock(&vm.running_mutex);
    vm.running = new_running;
    pthread_mutex_unlock(&vm.running_mutex);
}

fbuf_t get_fbuf(void) {
    pthread_mutex_lock(&vm.fbuf_mutex);
    fbuf_t fbuf = vm.fbuf;
    pthread_mutex_unlock(&vm.fbuf_mutex);

    return fbuf;
}

void set_fbuf(fbuf_t fbuf) {
    pthread_mutex_lock(&vm.fbuf_mutex);
    memcpy(&vm.fbuf, &fbuf, sizeof(fbuf_t));
    pthread_mutex_unlock(&vm.fbuf_mutex);
}

dbuf_t get_dbuf(void) {
    pthread_mutex_lock(&vm.dbuf_mutex);
    dbuf_t dbuf = vm.dbuf;
    pthread_mutex_unlock(&vm.dbuf_mutex);

    return dbuf;
}

void set_dbuf(dbuf_t dbuf) {
    pthread_mutex_lock(&vm.dbuf_mutex);
    memcpy(&vm.dbuf, &dbuf, sizeof(dbuf_t));
    pthread_mutex_unlock(&vm.dbuf_mutex);
}

ebuf_t get_ebuf(void) {
    pthread_mutex_lock(&vm.ebuf_mutex);
    ebuf_t ebuf = vm.ebuf;
    pthread_mutex_unlock(&vm.ebuf_mutex);

    return ebuf;
}

void set_ebuf(ebuf_t ebuf) {
    pthread_mutex_lock(&vm.ebuf_mutex);
    memcpy(&vm.ebuf, &ebuf, sizeof(ebuf_t));
    pthread_mutex_unlock(&vm.ebuf_mutex);
}

mbuf_t get_mbuf(void) {
    pthread_mutex_lock(&vm.mbuf_mutex);
    mbuf_t mbuf = vm.mbuf;
    pthread_mutex_unlock(&vm.mbuf_mutex);
    return mbuf;
}

void set_mbuf(mbuf_t mbuf) {
    pthread_mutex_lock(&vm.mbuf_mutex);
    memcpy(&vm.mbuf, &mbuf, sizeof(mbuf_t));
    pthread_mutex_unlock(&vm.mbuf_mutex);
}

uint16_t get_pc_override(void) {
    pthread_mutex_lock(&vm.pc_override_mutex);
    uint16_t pc_override = vm.pc_override;
    pthread_mutex_unlock(&vm.pc_override_mutex);
    return pc_override;
}

void set_pc_override(uint16_t new_pc) {
    pthread_mutex_lock(&vm.pc_override_mutex);
    vm.pc_override = new_pc;
    pthread_mutex_unlock(&vm.pc_override_mutex);

    pthread_mutex_lock(&vm.pc_override_signal_mutex);
    vm.pc_override_signal = 1;
    pthread_mutex_unlock(&vm.pc_override_signal_mutex);
}

uint16_t get_pc_override_signal(void) {
    pthread_mutex_lock(&vm.pc_override_signal_mutex);
    uint16_t pc_override_signal = vm.pc_override_signal;
    pthread_mutex_unlock(&vm.pc_override_signal_mutex);
    return pc_override_signal;
}

void set_pc_override_signal(uint16_t new_signal) {
    if (new_signal != 0 && new_signal != 1) {
        errno = EINVAL;
        return;
    }

    pthread_mutex_lock(&vm.pc_override_signal_mutex);
    vm.pc_override_signal = new_signal;
    pthread_mutex_unlock(&vm.pc_override_signal_mutex);
}

void send_bubble_to_id(void) {
    pthread_mutex_lock(&vm.id_nop_mutex);
    vm.id_nop = 1;
    pthread_mutex_unlock(&vm.id_nop_mutex);
}

void send_bubble_to_ex(void) {
    pthread_mutex_lock(&vm.ex_nop_mutex);
    vm.ex_nop = 1;
    pthread_mutex_unlock(&vm.ex_nop_mutex);
}

void send_bubble_to_mem(void) {
    pthread_mutex_lock(&vm.mem_nop_mutex);
    vm.mem_nop = 1;
    pthread_mutex_unlock(&vm.mem_nop_mutex);
}

void send_bubble_to_wb(void) {
    pthread_mutex_lock(&vm.wb_nop_mutex);
    vm.wb_nop = 1;
    pthread_mutex_unlock(&vm.wb_nop_mutex);
}

void send_stay_to_id(void) {
    pthread_mutex_lock(&vm.id_stay_mutex);
    vm.id_stay = 1;
    pthread_mutex_unlock(&vm.id_stay_mutex);
}

void send_stay_to_ex(void) {
    pthread_mutex_lock(&vm.ex_stay_mutex);
    vm.ex_stay = 1;
    pthread_mutex_unlock(&vm.ex_stay_mutex);

    send_stay_to_id();
}

unsigned int bubble_pending_ex(void) {
    pthread_mutex_lock(&vm.ex_nop_mutex);
    unsigned int result = vm.ex_nop;
    pthread_mutex_unlock(&vm.ex_nop_mutex);
    return result;
}
