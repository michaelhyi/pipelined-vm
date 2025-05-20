/**
 * This file provides a thread-safe interface that abstracts interactions with
 * the VM.
 */

#ifndef VM_UTIL_H
#define VM_UTIL_H

#include <stdint.h>

#include "../vm.h"

/**
 * Initializes all pthread mutexes and barriers.
 */
void init_mutexes_and_barriers(void);

/**
 * Destroys all pthread mutexes and barriers.
 */
void destroy_mutexes_and_barriers(void);

/**
 * Allows the user to view the vm's memory via user I/O. The user will manually
 * input memory addresses, which will return its respective data.
 *
 * Enter memory address 0x0000 to finish.
 */
void memory_viewer_run(void);

/**
 * Gets data from the VM's memory in a thread-safe manner.
 *
 * @param addr memory address to get data from
 * @returns the data stored at that memory address, 0 on an access violation
 * @note sets errno to EFAULT on an access violation
 */
int16_t get_mem(uint16_t addr);

/**
 * Sets data in the VM's memory in a thread-safe manner.
 *
 * @param addr memory address to set data in
 * @param data data to set in `mem[addr]`
 * @note sets errno to EFAULT on an access violation
 */
void set_mem(uint16_t addr, int16_t data);

/**
 * Gets data from the VM's register file in a thread-safe manner.
 *
 * @param reg_num register to get data from
 * @returns the data stored at that register, 0 on invalid arguments
 * @note sets errno to EINVAL if reg_num > 7
 */
int16_t get_register_data(uint16_t reg_num);

/**
 * Sets data in the VM's register file in a thread-safe manner.
 *
 * @param reg_num register to set data in
 * @param data data to set in register
 * @note sets errno to EINVAL if reg_num > 7 or if the register is not busy
 */
void set_register_data(uint16_t reg_num, int16_t data);

/**
 * Increments the busy counter of a register.
 *
 * @param register_num the number of the register to update
 */
void increment_busy_counter(uint16_t register_num);

/**
 * Decrements the busy counter of a register.
 *
 * @param register_num the number of the register to update
 */
void decrement_busy_counter(uint16_t register_num);

/**
 * Gets the VM's PC in a thread-safe manner.
 */
uint16_t get_pc(void);

/**
 * Sets the VM's PC in a thread-safe manner.
 *
 * @param pc the new PC to set
 */
void set_pc(uint16_t pc);

/**
 * Increments the condition code register's busy counter in a thread-safe
 * manner.
 */
void increment_cc_busy_counter(void);

/**
 * Gets the condition codes stored in the condition code register in a
 * thread-safe manner.
 *
 * @returns the vm's current conditions codes
 */
int16_t get_cc_data(void);

/**
 * Gets the VM's current FBUF in a thread-safe manner.
 *
 * @returns the current `fbuf` value
 */
fbuf_t get_fbuf(void);

/**
 * Sets the VM's FBUF in a thread-safe manner.
 *
 * @param fbuf the new FBUF value to set
 */
void set_fbuf(fbuf_t fbuf);

/**
 * Gets the VM's current DBUF in a thread-safe manner.
 *
 * @returns the current `dbuf` value
 */
dbuf_t get_dbuf(void);

/**
 * Sets the VM's DBUF in a thread-safe manner.
 *
 * @param dbuf the new DBUF value to set
 */
void set_dbuf(dbuf_t dbuf);

/**
 * Gets the VM's current EBUF in a thread-safe manner.
 *
 * @returns the current `ebuf` value
 */
ebuf_t get_ebuf(void);

/**
 * Sets the VM's EBUF in a thread-safe manner.
 *
 * @param ebuf the new EBUF value to set
 */
void set_ebuf(ebuf_t ebuf);

/**
 * Gets the VM's current MBUF in a thread-safe manner.
 *
 * @returns the current `mbuf` value
 */
mbuf_t get_mbuf(void);

/**
 * Sets the VM's MBUF in a thread-safe manner.
 *
 * @param mbuf the new MBUF value to set
 */
void set_mbuf(mbuf_t mbuf);

/**
 * Gets pc_override in a thread-safe manner.
 */
uint16_t get_pc_override(void);

/**
 * Sets pc_override and asserts its signal in a thread-safe manner.
 *
 * @param new_pc the new pc to set
 */
void set_pc_override(uint16_t new_pc);

/**
 * Gets pc_override_signal in a thread-safe manner.
 */
uint16_t get_pc_override_signal(void);

/**
 * Asserts the NOP bit to the `ID` stage of the pipeline.
 */
void send_bubble_to_id(void);

/**
 * Asserts the NOP bit to the `EX` stage of the pipeline.
 */
void send_bubble_to_ex(void);

/**
 * Asserts the NOP bit to the `MEM` stage of the pipeline.
 */
void send_bubble_to_mem(void);

/**
 * Asserts the NOP bit to the `WB` stage of the pipeline.
 */
void send_bubble_to_wb(void);

/**
 * Asserts the stay bit to both the `if` and `id` stages of the pipeline.
 */
void send_stay_to_id(void);

/**
 * Asserts the stay bit to the `if`, `id`, and `ex` stages of the pipeline.
 */
void send_stay_to_ex(void);

/**
 * Returns whether the nop bit to the `ex` stage of the pipeline is high.
 *
 * @returns 1 if a bubble is pending to `ex`, 0 otherwise
 */
unsigned int bubble_pending_ex(void);

#endif
