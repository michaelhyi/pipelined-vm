CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow \
			-Wstrict-prototypes -Wold-style-definition -Werror=vla -pthread
C_OPT_FLAGS = -O2
C_DEBUG_FLAGS = -g -O0

SRC_DIR = src
SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
SRC_OUTPUT = vm

TEST_DIR = tests
TEST_FILES = $(shell find $(SRC_DIR) $(TEST_DIR) -name '*.c' ! -name 'main.c')
TEST_OUTPUT = vm-tests


run: compile
	./$(SRC_OUTPUT) $(ARGS)

test: compile-tests
	./$(TEST_OUTPUT)

debug: compile-debug
	gdb ./$(SRC_OUTPUT)

debug-tests: compile-tests-debug
	gdb ./$(TEST_OUTPUT)

valgrind: compile-debug compile-tests-debug
	valgrind --leak-check=yes ./$(SRC_OUTPUT)
	valgrind --leak-check=yes ./$(TEST_OUTPUT)

clean:
	rm -f $(SRC_OUTPUT) $(TEST_OUTPUT) .clang-format

format: format-setup
	find . \( -name "*.c" -o -name "*.h" \) -exec clang-format -style=file -i {} +

compile: $(SRC_FILES)
	$(CC) $(CFLAGS) $(C_OPT_FLAGS) $(SRC_FILES) -o $(SRC_OUTPUT)

compile-tests: $(TEST_FILES)
	$(CC) $(CFLAGS) $(C_OPT_FLAGS) $(TEST_FILES) -o $(TEST_OUTPUT)

compile-debug: $(SRC_FILES)
	$(CC) $(CFLAGS) $(C_DEBUG_FLAGS) $(SRC_FILES) -o $(SRC_OUTPUT)

compile-tests-debug: $(TEST_FILES)
	$(CC) $(CFLAGS) $(C_DEBUG_FLAGS) $(TEST_FILES) -o $(TEST_OUTPUT)

format-setup:
	@if [ ! -f .clang-format ]; then \
		clang-format -style="{BasedOnStyle: LLVM, IndentWidth: 4}" -dump-config > .clang-format; \
	fi


.PHONY: run test debug debug-tests valgrind clean format compile compile-tests compile-debug compile-tests-debug format-setup
