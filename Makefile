CC = gcc
CFLAGS = -g -O2 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes

SRC_DIR = src
SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
SRC_OUTPUT = vm

TEST_DIR = tests
TEST_FILES = $(shell find $(SRC_DIR) $(TEST_DIR) -name '*.c' ! -name 'main.c')
TEST_OUTPUT = vm-tests


run: $(SRC_OUTPUT)
	./$(SRC_OUTPUT)

$(SRC_OUTPUT): $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(SRC_OUTPUT)

test: $(TEST_OUTPUT)
	./$(TEST_OUTPUT)

$(TEST_OUTPUT): $(TEST_FILES)
	$(CC) $(CFLAGS) $(TEST_FILES) -o $(TEST_OUTPUT)

valgrind: $(SRC_OUTPUT) $(TEST_OUTPUT)
	valgrind --leak-check=yes ./$(SRC_OUTPUT)
	valgrind --leak-check=yes ./$(TEST_OUTPUT)

gdb: $(SRC_OUTPUT)
	gdb ./$(SRC_OUTPUT)

gdb-test: $(TEST_OUTPUT)
	gdb ./$(TEST_OUTPUT)
	
clean:
	rm -f $(SRC_OUTPUT) $(TEST_OUTPUT) .clang-format

format: format-setup
	find . \( -name "*.c" -o -name "*.h" \) -exec clang-format -style=file -i {} +

format-setup:
	@if [ ! -f .clang-format ]; then \
		clang-format -style="{BasedOnStyle: LLVM, IndentWidth: 4}" -dump-config > .clang-format; \
	fi


.PHONY: run test valgrind gdb gdb-test clean format format-setup
