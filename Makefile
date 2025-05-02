CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wstrict-prototypes -O2

SRC_DIR = src
SRC_FILES = $(shell find $(SRC_DIR) -name '*.c')
SRC_OUTPUT = vm

TEST_DIR = tests
TEST_FILES = $(shell find $(TEST_DIR) -name '*.c')
TEST_OUTPUT = vm-tests


all: $(SRC_OUTPUT)

$(SRC_OUTPUT): $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(SRC_OUTPUT)

test: $(TEST_FILES)
	$(CC) $(CFLAGS) $(TEST_FILES) -o $(TEST_OUTPUT)
	
clean:
	rm -f $(SRC_OUTPUT) $(TEST_OUTPUT)

format-setup:
	clang-format -style=llvm -dump-config > .clang-format

format:
	find . \( -name "*.c" -o -name "*.h" \) -exec clang-format -style=file -i {} +


.PHONY: all test clean format-setup format
