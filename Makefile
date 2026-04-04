CC = clang

BUILD ?= debug
ifeq ($(BUILD),release)
    CFLAGS := -O3 -DNDEBUG -flto -ffat-lto-objects -march=native
    LDFLAGS := -flto -s
else
    CFLAGS := -O0 -g -fsanitize=address,undefined,nullability \
							-fno-omit-frame-pointer
    LDFLAGS := -fsanitize=address,undefined
endif

WARNINGS := -Wall -Wextra -Wshadow -Wformat=2 \
						-Wnonnull \
						-Wnull-dereference \
						-Wconversion \
						-Wstrict-prototypes \
						-Werror

CFLAGS += $(WARNINGS) -Iinclude -fPIC -std=gnu23

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
TEST_DIR := test
TEST_BUILD_DIR := $(BUILD_DIR)/test

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRC))
DEPS := $(OBJ:.o=.d)

STATIC_OBJ := $(BUILD_DIR)/clib.o
SHARED_LIB := $(BUILD_DIR)/clib.so

TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
TEST_BIN := $(patsubst $(TEST_DIR)/%.c,$(TEST_BUILD_DIR)/%,$(TEST_SRC))

.PHONY: all lib test clean format

all: lib

lib: $(STATIC_OBJ) $(SHARED_LIB)

test: $(TEST_BIN)

analysis:
	$(foreach src,$(SRC), \
		clang --analyze \
		$(CFLAGS) \
		-Xanalyzer -analyzer-checker=core,nullability,unix \
		$(src);)

format:
	find . -name '*.c' -o -name '*.h' | xargs clang-format -i

$(STATIC_OBJ): $(OBJ) | $(BUILD_DIR)
	$(CC) -r $(OBJ) -o $@

$(SHARED_LIB): $(OBJ) | $(BUILD_DIR)
	$(CC) -shared $(LDFLAGS) -o $@ $(OBJ)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(TEST_BUILD_DIR)/%: $(TEST_DIR)/%.c $(STATIC_OBJ) | $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) $< $(STATIC_OBJ) -o $@ $(LDFLAGS)

-include $(DEPS)

$(BUILD_DIR) $(OBJ_DIR) $(TEST_BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
