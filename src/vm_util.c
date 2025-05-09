#include "vm_util.h"

#include <errno.h>
#include <string.h>

#include "util/bitops.h"
#include "vm.h"

/**
 * Returns whether the VM is operating in supervisor or user mode.
 *
 * @returns 1 if supervisor mode, 0 if user mode
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

    return supervisor_mode;
}

static inline int privileged_mem_addr(uint16_t addr) {
    return addr <= 0x2FFF || addr >= 0xFE00;
}

int16_t get_mem(uint16_t addr) {
    if (!supervisor_mode() && privileged_mem_addr(addr)) {
        errno = EFAULT;
        return 0;
    }

    pthread_mutex_lock(&vm.mem_mutex);
    int16_t data = vm.mem[addr];
    pthread_mutex_unlock(&vm.mem_mutex);

    return data;
}

void set_mem(uint16_t addr, int16_t data) {
    if (!supervisor_mode() && privileged_mem_addr(addr)) {
        errno = EFAULT;
        return;
    }

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
    if (!vm.register_file[reg_num].busy_counter) {
        errno = EINVAL;
        pthread_mutex_unlock(&vm.register_file_mutex);
        return;
    }

    vm.register_file[reg_num].data = data;
    vm.register_file[reg_num]
        .busy_counter--; // TODO: decouple this logic from this method
    pthread_mutex_unlock(&vm.register_file_mutex);
}

void set_pc(uint16_t pc) {
    pthread_mutex_lock(&vm.pc_mutex);
    vm.pc = pc;
    pthread_mutex_unlock(&vm.pc_mutex);
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
