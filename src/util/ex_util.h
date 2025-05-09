#ifndef EX_UTIL_H
#define EX_UTIL_H

#include "../vm.h"

/**
 * Returns an initialized `ebuf` based on `dbuf`.
 *
 * @param dbuf the VM's current `dbuf`
 * @returns a new initialized ebuf
 */
ebuf_t init_ebuf(dbuf_t dbuf);

/**
 * Returns whether or not the instruction needs the ALU for adding based on its
 * opcode.
 *
 * @param opcode the opcode of the instruction
 * @returns 1 if the instruction needs the ALU for adding, 0 otherwise
 */
int instruction_needs_add(int16_t opcode);

/**
 * Returns whether the instruction being executed is `JSR`.
 *
 * @returns 1 if executing `JSR`, 0 otherwise
 */
int ex_instruction_is_jsr(dbuf_t dbuf);

/**
 * Returns whether the instruction being executed is `JSRR`.
 *
 * @returns 1 if executing `JSRR`, 0 otherwise
 */
int ex_instruction_is_jsrr(dbuf_t dbuf);

/**
 * Updates `ebuf` with the new ebuf from the `EX` stage if its STAY signal is
 * not high.
 *
 * @param ebuf pointer to the new ebuf from the `EX` stage
 */
void update_ebuf(ebuf_t *ebuf);

#endif
