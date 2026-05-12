ifeq ($(origin CC), default)
CC := gcc
endif
ifeq ($(origin CC), undefined)
CC := gcc
endif

ANALYZER ?= $(CC)
BUILD ?= debug
LIBC ?= -DCLIB_USE_LIBC
CVERSION ?= gnu23
FORMAT ?= clang-format

COMPILER := $(notdir $(firstword $(CC)))
ANALYZER_COMPILER := $(notdir $(firstword $(ANALYZER)))

ifeq ($(findstring clang,$(COMPILER)),clang)
	DEBUG_SANITIZERS := -fsanitize=address,undefined,nullability
	COMPILER_WARNINGS := \
		-Wshorten-64-to-32 \
		-Wunreachable-code \
		-Wcomma \
		-Wimplicit-fallthrough
else ifneq ($(findstring gcc,$(COMPILER)),)
	DEBUG_SANITIZERS := -fsanitize=address,undefined
	COMPILER_WARNINGS := \
		-Wlogical-op \
		-Wduplicated-cond \
		-Wduplicated-branches \
		-Walloc-zero \
		-Wimplicit-fallthrough=5
else
	DEBUG_SANITIZERS := -fsanitize=address,undefined
	COMPILER_WARNINGS :=
endif

ifeq ($(BUILD),release)
	OPT_CFLAGS := -O3 -DNDEBUG -flto -ffat-lto-objects -march=native
	OPT_LDFLAGS := -flto -s
else
	OPT_CFLAGS := -O0 -g $(DEBUG_SANITIZERS) -fno-omit-frame-pointer
	OPT_LDFLAGS := -fsanitize=address,undefined
endif

WARNINGS := \
	-Wall \
	-Wextra \
	-Wshadow \
	-Wformat=2 \
	-Wnonnull \
	-Wnull-dereference \
	-Wconversion \
	-Wstrict-prototypes \
	-Werror

COMMON_WARNINGS := \
	-Wundef \
	-Wwrite-strings \
	-Wcast-qual \
	-Wpointer-arith \
	-Wredundant-decls \
	-Wswitch-enum

COMMON_CFLAGS := $(OPT_CFLAGS) $(WARNINGS) $(COMMON_WARNINGS) \
	$(COMPILER_WARNINGS) -Iinclude -std=$(CVERSION) -fPIC \
	-fvisibility=hidden
HOSTED_CFLAGS := $(COMMON_CFLAGS) $(LIBC)
FREESTANDING_CFLAGS := $(COMMON_CFLAGS)

BUILD_DIR := build
HOSTED_OBJ_DIR := $(BUILD_DIR)/obj
HOSTED_TEST_DIR := $(BUILD_DIR)/test
FREESTANDING_DIR := $(BUILD_DIR)/freestanding
FREESTANDING_OBJ_DIR := $(FREESTANDING_DIR)/obj
FREESTANDING_TEST_DIR := $(FREESTANDING_DIR)/test

SRC := $(wildcard src/*.c)
HDR := $(wildcard include/*.h)
FREESTANDING_SRC := src/rbtree.c

HOSTED_OBJ := $(patsubst src/%.c,$(HOSTED_OBJ_DIR)/%.o,$(SRC))
HOSTED_DEPS := $(HOSTED_OBJ:.o=.d)
FREESTANDING_OBJ := $(patsubst src/%.c,$(FREESTANDING_OBJ_DIR)/%.o,$(FREESTANDING_SRC))
FREESTANDING_DEPS := $(FREESTANDING_OBJ:.o=.d)

STATIC_OBJ := $(BUILD_DIR)/clib.o
SHARED_LIB := $(BUILD_DIR)/libclib.so
FREESTANDING_LIB := $(BUILD_DIR)/clib-freestanding.o

TEST_DIR := test
TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
HOSTED_TEST_BIN := $(patsubst $(TEST_DIR)/%.c,$(HOSTED_TEST_DIR)/%,$(TEST_SRC))

FREESTANDING_TEST_NAMES := \
	test_bitmap.c \
	test_types.c \
	test_tools.c \
	test_sort.c \
	test_list.c \
	test_ringbuf.c \
	test_stack.c \
	test_dsu.c \
	test_rbtree.c
FREESTANDING_TEST_SRC := $(addprefix $(TEST_DIR)/,$(FREESTANDING_TEST_NAMES))
FREESTANDING_TEST_BIN := $(patsubst $(TEST_DIR)/%.c,$(FREESTANDING_TEST_DIR)/%,$(FREESTANDING_TEST_SRC))

.DEFAULT_GOAL := lib
.PHONY: lib analysis format clean freestanding test test-freestanding

lib: $(STATIC_OBJ) $(SHARED_LIB)

freestanding: $(FREESTANDING_LIB)

test: $(HOSTED_TEST_BIN) $(SHARED_LIB)
	@./test.sh
	@bash ./test/test_exports.sh

test-freestanding: $(FREESTANDING_TEST_BIN)
	@total=0; \
	failed=0; \
	for test in $(FREESTANDING_TEST_BIN); do \
		total=$$((total + 1)); \
		echo "=== Running $$(basename "$$test") ==="; \
		ASAN_OPTIONS=detect_leaks=0 "$$test"; \
		rc=$$?; \
		if [ "$$rc" -ne 0 ]; then \
			failed=$$((failed + 1)); \
		fi; \
	done; \
	echo "=== Test binaries: $$total, failed: $$failed ==="; \
	[ "$$failed" -eq 0 ]

analysis:
ifeq ($(findstring gcc,$(ANALYZER_COMPILER)),gcc)
	$(foreach src,$(SRC), \
		$(ANALYZER) \
		$(HOSTED_CFLAGS) \
		-fanalyzer -fanalyzer-verbosity=2 \
		-fdiagnostics-path-format=inline-events \
		-c $(src) -o /tmp/clib-analysis.o;)
	$(foreach hdr,$(HDR), \
		$(ANALYZER) \
		$(HOSTED_CFLAGS) \
		-fanalyzer -fanalyzer-verbosity=2 \
		-fdiagnostics-path-format=inline-events \
		-include $(hdr) \
		-x c -c /dev/null -o /tmp/clib-analysis.o;)
else
	$(foreach src,$(SRC), \
		$(ANALYZER) --analyze \
		$(HOSTED_CFLAGS) \
		-Xanalyzer -analyzer-checker=core,nullability,unix \
		$(src);)
	$(foreach hdr,$(HDR), \
		$(ANALYZER) --analyze \
		$(HOSTED_CFLAGS) \
		-Xanalyzer -analyzer-checker=core,nullability,unix \
		-include $(hdr) \
		-x c /dev/null;)
endif

format:
	find . \( -name '*.c' -o -name '*.h' \) -print0 | xargs -0 $(FORMAT) -i

$(STATIC_OBJ): $(HOSTED_OBJ) | $(BUILD_DIR)
	$(CC) -r $(HOSTED_OBJ) -o $@

$(SHARED_LIB): $(HOSTED_OBJ) | $(BUILD_DIR)
	$(CC) -shared $(OPT_LDFLAGS) -o $@ $(HOSTED_OBJ)

$(FREESTANDING_LIB): $(FREESTANDING_OBJ) | $(BUILD_DIR)
	$(CC) -r $(FREESTANDING_OBJ) -o $@

$(HOSTED_OBJ_DIR)/%.o: src/%.c | $(HOSTED_OBJ_DIR)
	$(CC) $(HOSTED_CFLAGS) -MMD -MP -c $< -o $@

$(FREESTANDING_OBJ_DIR)/%.o: src/%.c | $(FREESTANDING_OBJ_DIR)
	$(CC) $(FREESTANDING_CFLAGS) -MMD -MP -c $< -o $@

$(HOSTED_TEST_DIR)/%: $(TEST_DIR)/%.c $(STATIC_OBJ) | $(HOSTED_TEST_DIR)
	$(CC) $(HOSTED_CFLAGS) $< $(STATIC_OBJ) -o $@ $(OPT_LDFLAGS)

$(FREESTANDING_TEST_DIR)/%: $(TEST_DIR)/%.c $(FREESTANDING_LIB) | $(FREESTANDING_TEST_DIR)
	$(CC) $(FREESTANDING_CFLAGS) $< $(FREESTANDING_LIB) -o $@ $(OPT_LDFLAGS)

-include $(HOSTED_DEPS) $(FREESTANDING_DEPS)

$(BUILD_DIR) $(HOSTED_OBJ_DIR) $(HOSTED_TEST_DIR) \
$(FREESTANDING_DIR) $(FREESTANDING_OBJ_DIR) $(FREESTANDING_TEST_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f ./*.plist
