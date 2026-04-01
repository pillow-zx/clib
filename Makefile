CC = clang
CFLAGS ?= -O2 -g -Wall -Wextra -Iinclude

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
TEST_DIR := test
TEST_BUILD_DIR := $(BUILD_DIR)/test

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRC))

STATIC_OBJ := $(BUILD_DIR)/clib.o
SHARED_LIB := $(BUILD_DIR)/clib.so

TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
TEST_BIN := $(patsubst $(TEST_DIR)/%.c,$(TEST_BUILD_DIR)/%,$(TEST_SRC))

.PHONY: all lib test clean

all: lib

lib: $(STATIC_OBJ) $(SHARED_LIB)

test: $(TEST_BIN)

format:
	@echo "Formatting codes..."
	@clang-format -i -style=file:./.clang-format ./**/*h
	@clang-format -i -style=file:./.clang-format ./**/*c
	@echo "Formatting finish..."

$(STATIC_OBJ): $(OBJ) | $(BUILD_DIR)
	$(CC) -r $(OBJ) -o $@

$(SHARED_LIB): $(OBJ) | $(BUILD_DIR)
	$(CC) -shared -o $@ $(OBJ)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(TEST_BUILD_DIR)/%: $(TEST_DIR)/%.c $(STATIC_OBJ) | $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) $< $(STATIC_OBJ) -o $@

$(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIR): | $(BUILD_DIR)
	mkdir -p $@

$(TEST_BUILD_DIR): | $(BUILD_DIR)
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
