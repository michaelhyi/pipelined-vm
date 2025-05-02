#ifndef ID_UTIL_H
#define ID_UTIL_H

#include <stdint.h>

#include "../stages/id.h"
#include "../stages/if.h"

/**
 * Returns whether the instruction performs JSR.
 *
 * @param ir instruction to decode
 * @return 1 if the instruction performs JSR, 0 otherwise
 */
int is_jsr(int16_t ir);

/**
 * Returns whether the instruction performs JSRR.
 *
 * @param ir instruction to decode
 * @return 1 if the instruction performs JSRR, 0 otherwise
 */
int is_jsrr(int16_t ir);

/**
 * Decodes ADD and AND instructions.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_add_and(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes NOT instruction.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_not(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes BR instruction.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_br(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes JMP and JSRR instructions.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_jmp_jsrr(fbuf_t fbuf, dbuf_t *dbuf);

/**
 * Decodes JSR instruction.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_jsr(fbuf_t fbuf, dbuf_t *dbuf);

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
 * Decodes TRAP instruction.
 *
 * @param fbuf fbuf in use by ID stage
 * @param dbuf pointer to dbuf to write to
 */
void decode_trap(fbuf_t fbuf, dbuf_t *dbuf);

#endif
