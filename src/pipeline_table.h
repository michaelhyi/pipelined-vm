#ifndef PIPELINE_TABLE_H
#define PIPELINE_TABLE_H

#include "vm.h"

typedef struct pipeline_table_t {
    char ***table;
    size_t size;
    size_t capacity;
} pipeline_table_t;

extern pipeline_table_t pipeline_table;

/**
 * Initializes the pipeline table.
 */
void pipeline_table_init(void);

/**
 * Adds a row to the pipeline table.
 */
void pipeline_table_add_row(void);

/**
 * Prints the pipeline table.
 */
void pipeline_table_print(void);

/**
 * Tears down the pipeline table;
 */
void pipeline_table_teardown(void);

#endif
