#ifndef ID_UTIL_H
#define ID_UTIL_H

#include <stdint.h>

#include "../vm.h"

/**
 * Returns the VM's current `fbuf`.
 */
fbuf_t get_fbuf(void);

/**
 * Returns an initialized `dbuf` based on `fbuf`.
 *
 * @param fbuf the VM's current `fbuf`
 * @returns a pointer to a new dynamically allocated dbuf
 */
dbuf_t *init_dbuf(fbuf_t fbuf);

/**
 * Sends a NOP bubble to the `ID` stage.
 */
void send_bubble_to_id(void);

/**
 * Sends a NOP bubble to the `EX` stage.
 */
void send_bubble_to_ex(void);

/**
 * Returns the opcode of an instruction.
 *
 * @param ir instruction to decode
 * @return the opcode of `ir`
 */
int16_t get_opcode(int16_t ir);

/**
 * Returns whether the instruction performs JSR.
 *
 * @param ir instruction to decode
 * @return 1 if the instruction performs JSR, 0 otherwise
 */
int id_instruction_is_jsr(int16_t ir);

/**
 * Returns whether the instruction performs JSRR.
 *
 * @param ir instruction to decode
 * @return 1 if the instruction performs JSRR, 0 otherwise
 */
int id_instruction_is_jsrr(int16_t ir);

/**
 * Decodes BR instruction.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_br(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes ADD and AND instructions.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_add_and(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes LD, LDI, and LEA instructions.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_ld_ldi_lea(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes ST and STI instructions.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_st_sti(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes JSR instruction.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_jsr(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes JMP and JSRR instructions.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_jmp_jsrr(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes LDR instruction.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_ldr(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes STR instruction.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_str(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes NOT instruction.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_not(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes TRAP instruction.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_trap(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Increments the busy counter of a register.
 *
 * @param register_num the number of the register to update
 */
void increment_busy_counter(uint16_t register_num);

/**
 * Updates `dbuf` with the new dbuf from the `ID` stage if its STAY signal is
 * not high.
 *
 * @param dbuf pointer to the new dbuf from the `ID` stage
 */
void update_dbuf(dbuf_t *dbuf);

#endif
