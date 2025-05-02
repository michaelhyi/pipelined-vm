CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -O2

SRC_DIR = src
SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
SRC_OUTPUT = vm

TEST_DIR = tests
TEST_FILES = $(shell find $(SRC_DIR) $(TEST_DIR) -name '*.c' ! -name 'main.c')
TEST_OUTPUT = vm-tests


all: $(SRC_OUTPUT)

$(SRC_OUTPUT): $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(SRC_OUTPUT)
	./$(SRC_OUTPUT)

test: $(TEST_FILES)
	$(CC) $(CFLAGS) $(TEST_FILES) -o $(TEST_OUTPUT)
	./$(TEST_OUTPUT)

valgrind: $(SRC_OUTPUT) test
	valgrind --leak-check=yes ./$(SRC_OUTPUT)
	valgrind --leak-check=yes ./$(TEST_OUTPUT)
	
clean:
	rm -f $(SRC_OUTPUT) $(TEST_OUTPUT)

format-setup:
	@if [ ! -f .clang-format ]; then \
		clang-format -style="{BasedOnStyle: LLVM, IndentWidth: 4}" -dump-config > .clang-format; \
	fi

format: format-setup
	find . \( -name "*.c" -o -name "*.h" \) -exec clang-format -style=file -i {} +


.PHONY: all test valgrind clean format-setup format
