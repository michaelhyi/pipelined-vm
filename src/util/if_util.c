#include "if_util.h"

#include <string.h>

int16_t get_instruction_and_increment_pc(void) {
    pthread_mutex_lock(&vm.pc_mutex);

    // get instruction
    pthread_mutex_lock(&vm.mem_mutex);
    int16_t ir = vm.mem[vm.pc];
    pthread_mutex_unlock(&vm.mem_mutex);

    // increment pc
    vm.pc++;
    pthread_mutex_unlock(&vm.pc_mutex);

    return ir;
}

void save_pc(fbuf_t *fbuf) {
    pthread_mutex_lock(&vm.pc_mutex);
    fbuf->pc = vm.pc;
    pthread_mutex_unlock(&vm.pc_mutex);
}

void update_fbuf(fbuf_t *fbuf) {
    pthread_mutex_lock(&vm.id_stay_mutex);

    if (!vm.id_stay) {
        pthread_mutex_lock(&vm.fbuf_mutex);
        memcpy(&vm.fbuf, fbuf, sizeof(fbuf_t));
        pthread_mutex_unlock(&vm.fbuf_mutex);
    } else {
        vm.id_stay = 0;

        // decrement PC, since the pipline is stalled
        pthread_mutex_lock(&vm.pc_mutex);
        vm.pc--;
        pthread_mutex_unlock(&vm.pc_mutex);
    }

    pthread_mutex_unlock(&vm.id_stay_mutex);
}
