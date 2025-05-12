#include "pipeline_table.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "isa.h"
#include "util/id_util.h"
#include "util/vm_util.h"

pipeline_table_t pipeline_table;

/**
 * Converts an opcode to a string.
 *
 * @param opcode the opcode to convert to a string
 * @returns a string representation of the opcode
 */
static char *stropcode(int16_t opcode);

static char *stropcode(int16_t opcode) {
    switch (opcode & 0xF) { // Mask to 4 bits
    case OP_BR:
        return "br";
    case OP_ADD:
        return "add";
    case OP_LD:
        return "ld";
    case OP_ST:
        return "st";
    case OP_JSR:
        return "jsr"; // jsr and jsrr share the same base opcode
    case OP_AND:
        return "and";
    case OP_LDR:
        return "ldr";
    case OP_STR:
        return "str";
    case OP_RTI:
        return "rti";
    case OP_NOT:
        return "not";
    case OP_LDI:
        return "ldi";
    case OP_STI:
        return "sti";
    case OP_JMP:
        return "jmp";
    case OP_RESERVED:
        return "reserved";
    case OP_LEA:
        return "lea";
    case OP_TRAP:
        return "trap";
    default:
        return "invalid";
    }
}

void pipeline_table_init(void) {
    memset(&pipeline_table, 0, sizeof(pipeline_table_t));
    pipeline_table.table = malloc(sizeof(char **) * 10);
    pipeline_table.capacity = 10;
}

void pipeline_table_add_row(void) {
    if (pipeline_table.size == pipeline_table.capacity) {
        char ***temp_table =
            realloc(pipeline_table.table,
                    sizeof(char **) * pipeline_table.capacity * 2);

        if (!temp_table) {
            errno = ENOMEM;
            return;
        }

        pipeline_table.table = temp_table;
        pipeline_table.capacity *= 2;
    }

    char **row = malloc(sizeof(char *) * NUM_PIPELINE_STAGES);
    row[0] = stropcode(get_opcode(get_mem(vm.pc)));
    row[1] = vm.fbuf.nop ? "nop" : stropcode(get_opcode(vm.fbuf.ir));
    row[2] = vm.dbuf.nop ? "nop" : stropcode(vm.dbuf.opcode);
    row[3] = vm.ebuf.nop ? "nop" : stropcode(vm.ebuf.opcode);
    row[4] = vm.mbuf.nop ? "nop" : stropcode(vm.mbuf.opcode);

    pipeline_table.table[pipeline_table.size] = row;
    pipeline_table.size++;
}

void pipeline_table_print(void) {
    for (size_t i = 0; i < pipeline_table.size; i++) {
        printf("cycle %ld: ", i + 1);
        char **row = pipeline_table.table[i];

        for (size_t j = 0; j < NUM_PIPELINE_STAGES; j++) {
            printf("%s ", row[j]);
        }

        printf("\n");
    }
}

void pipeline_table_teardown(void) {
    for (size_t i = 0; i < pipeline_table.size; i++) {
        char **row = pipeline_table.table[i];
        free(row);
    }

    free(pipeline_table.table);
}
