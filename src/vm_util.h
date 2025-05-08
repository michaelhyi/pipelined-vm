/**
 * This file provides a thread-safe interface that abstracts interactions with
 * the VM.
 */

#ifndef VM_UTIL_H
#define VM_UTIL_H

#include <stdint.h>

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
int16_t get_register(uint16_t reg_num);

/**
 * Sets data in the VM's register file in a thread-safe manner.
 *
 * @param reg_num register to set data in
 * @param data data to set in register
 * @note sets errno to EINVAL if reg_num > 7 or if the register is not busy
 */
void set_register(uint16_t reg_num, int16_t data);

#endif
