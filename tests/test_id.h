#ifndef TEST_ID_H
#define TEST_ID_H

void test_id_exec_cycle(int *passed_tests, int *total_tests);

void test_id_add_register_exec_cycle(int *passed_tests, int *total_tests);
void test_id_add_immediate_exec_cycle(int *passed_tests, int *total_tests);
void test_id_and_register_exec_cycle(int *passed_tests, int *total_tests);
void test_id_and_immediate_exec_cycle(int *passed_tests, int *total_tests);
void test_id_not_exec_cycle(int *passed_tests, int *total_tests);
void test_id_br_exec_cycle(int *passed_tests, int *total_tests);
void test_id_jmp_exec_cycle(int *passed_tests, int *total_tests);
void test_id_jsr_exec_cycle(int *passed_tests, int *total_tests);
void test_id_jsrr_exec_cycle(int *passed_tests, int *total_tests);
void test_id_ld_exec_cycle(int *passed_tests, int *total_tests);
void test_id_ldi_exec_cycle(int *passed_tests, int *total_tests);
void test_id_ldr_exec_cycle(int *passed_tests, int *total_tests);
void test_id_lea_exec_cycle(int *passed_tests, int *total_tests);
void test_id_st_exec_cycle(int *passed_tests, int *total_tests);
void test_id_sti_exec_cycle(int *passed_tests, int *total_tests);
void test_id_str_exec_cycle(int *passed_tests, int *total_tests);
void test_id_trap_exec_cycle(int *passed_tests, int *total_tests);

#endif
