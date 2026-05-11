CC = clang

# Debug Model
# ┌─────────────────────────┬───────────────────────────────────────────────────────────────────────────────┐
# │          选项           │                                     作用                                      │
# ├─────────────────────────┼───────────────────────────────────────────────────────────────────────────────┤
# │ -O0                     │ 完全禁用优化，便于调试                                                        │
# ├─────────────────────────┼───────────────────────────────────────────────────────────────────────────────┤
# │ -g                      │ 生成调试符号 (DWARF 格式)                                                     │
# ├─────────────────────────┼───────────────────────────────────────────────────────────────────────────────┤
# │ -fsanitize=address      │ AddressSanitizer (ASan) - 检测内存错误：越界、释放后使用、双重释放等          │
# ├─────────────────────────┼───────────────────────────────────────────────────────────────────────────────┤
# │ -fsanitize=undefined    │ UndefinedBehaviorSanitizer (UBSan) - 检测未定义行为：整数溢出、空指针解引用等 │
# ├─────────────────────────┼───────────────────────────────────────────────────────────────────────────────┤
# │ -fsanitize=nullability  │ 检测空指针解引用和 nonnull 参数违规                                           │
# ├─────────────────────────┼───────────────────────────────────────────────────────────────────────────────┤
# │ -fno-omit-frame-pointer │ 保留帧指针，便于调试时堆栈回溯                                                │
# └─────────────────────────┴───────────────────────────────────────────────────────────────────────────────┘

# Sanitizer options
# ┌────────────────────────────┬───────┬──────────────────────────────────────┬──────────────────────┐
# │         Sanitizer          │ 缩写  │               检测内容               │       启用方式       │
# ├────────────────────────────┼───────┼──────────────────────────────────────┼──────────────────────┤
# │ AddressSanitizer           │ ASan  │ 内存错误：越界、释放后使用、双重释放 │ -fsanitize=address   │
# ├────────────────────────────┼───────┼──────────────────────────────────────┼──────────────────────┤
# │ UndefinedBehaviorSanitizer │ UBSan │ 未定义行为：整数溢出、空指针解引用   │ -fsanitize=undefined │
# ├────────────────────────────┼───────┼──────────────────────────────────────┼──────────────────────┤
# │ MemorySanitizer            │ MSan  │ 未初始化内存读取                     │ -fsanitize=memory    │
# ├────────────────────────────┼───────┼──────────────────────────────────────┼──────────────────────┤
# │ ThreadSanitizer            │ TSan  │ 数据竞争                             │ -fsanitize=thread    │
# ├────────────────────────────┼───────┼──────────────────────────────────────┼──────────────────────┤
# │ LeakSanitizer              │ LSan  │ 内存泄漏                             │ -fsanitize=leak      │
# └────────────────────────────┴───────┴──────────────────────────────────────┴──────────────────────┘

# Release
# ┌───────────────────┬─────────────────────────────────────────────────────┐
# │       选项        │                        作用                         │
# ├───────────────────┼─────────────────────────────────────────────────────┤
# │ -O3               │ 最高级别优化 (内联、循环展开、向量等)               │
# ├───────────────────┼─────────────────────────────────────────────────────┤
# │ -DNDEBUG          │ 定义 NDEBUG 宏，禁用 assert() 断言                  │
# ├───────────────────┼─────────────────────────────────────────────────────┤
# │ -flto             │ Link-Time Optimization，跨编译单元优化              │
# ├───────────────────┼─────────────────────────────────────────────────────┤
# │ -ffat-lto-objects │ 生成同时包含 bitcode 和目标代码的对象文件，兼容 LTO │
# ├───────────────────┼─────────────────────────────────────────────────────┤
# │ -march=native     │ 针对本地 CPU 优化 (使用本地 CPU 指令集)             │
# ├───────────────────┼─────────────────────────────────────────────────────┤
# │ -s                │ strip 符号，减小二进制体积                          │
# └───────────────────┴─────────────────────────────────────────────────────┘

BUILD ?= debug
LIBC ?= -DCLIB_USE_LIBC
CVERSION ?= gnu23

ifeq ($(BUILD),release)
	OPT_CFLAGS := -O3 -DNDEBUG -flto -ffat-lto-objects -march=native
	OPT_LDFLAGS := -flto -s
else
	OPT_CFLAGS := -O0 -g -fsanitize=address,undefined,nullability \
		-fno-omit-frame-pointer
	OPT_LDFLAGS := -fsanitize=address,undefined
endif

# Warnings
# ┌─────────────────────┬─────────────────────────┬───────────────────────────────────┐
# │        选项         │        检测内容         │               示例                │
# ├─────────────────────┼─────────────────────────┼───────────────────────────────────┤
# │ -Wall               │ 常见警告                │ 未使用的变量、未使用的函数原型    │
# ├─────────────────────┼─────────────────────────┼───────────────────────────────────┤
# │ -Wextra             │ 额外警告                │ 多余的参数、比较符号              │
# ├─────────────────────┼─────────────────────────┼───────────────────────────────────┤
# │ -Wshadow            │ 变量遮蔽                │ 内层变量遮蔽外层同名变量          │
# ├─────────────────────┼─────────────────────────┼───────────────────────────────────┤
# │ -Wformat=2          │ printf/scanf 格式字符串 │ %s 配 int 类型                    │
# ├─────────────────────┼─────────────────────────┼───────────────────────────────────┤
# │ -Wnonnull           │ nonnull 属性违规        │ 传入 NULL 给标记为 nonnull 的参数 │
# ├─────────────────────┼─────────────────────────┼───────────────────────────────────┤
# │ -Wnull-dereference  │ 空指针解引用检测        │ 编译期检测明显空指针使用          │
# ├─────────────────────┼─────────────────────────┼───────────────────────────────────┤
# │ -Wconversion        │ 隐式类型转换            │ int → char 可能丢失数据           │
# ├─────────────────────┼─────────────────────────┼───────────────────────────────────┤
# │ -Wstrict-prototypes │ 函数声明缺少参数类型    │ void foo() 而非 void foo(void)    │
# ├─────────────────────┼─────────────────────────┼───────────────────────────────────┤
# │ -Werror             │ 将警告视为错误          │ 任何警告都导致编译失败            │
# └─────────────────────┴─────────────────────────┴───────────────────────────────────┘

WARNINGS := -Wall -Wextra -Wshadow -Wformat=2 \
	-Wnonnull \
	-Wnull-dereference \
	-Wconversion \
	-Wstrict-prototypes \
	-Werror

# Other compiler options
# ┌────────────┬─────────────────────────────────────────┐
# │    选项    │                  作用                   │
# ├────────────┼─────────────────────────────────────────┤
# │ -Iinclude  │ 添加头文件搜索路径                      │
# ├────────────┼─────────────────────────────────────────┤
# │ -fPIC      │ Position Independent Code，用于共享库   │
# ├────────────┼─────────────────────────────────────────┤
# │ -std=gnu23 │ GNU C23 标准，支持 typeof、匿名结构体等 │
# └────────────┴─────────────────────────────────────────┘

COMMON_CFLAGS := $(OPT_CFLAGS) $(WARNINGS) -Iinclude -std=$(CVERSION) -fPIC
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
SHARED_LIB := $(BUILD_DIR)/clib.so
FREESTANDING_LIB := $(BUILD_DIR)/clib-freestanding.o

TEST_DIR := test
TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
HOSTED_TEST_BIN := $(patsubst $(TEST_DIR)/%.c,$(HOSTED_TEST_DIR)/%,$(TEST_SRC))

FREESTANDING_TEST_NAMES := \
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

.PHONY: all lib freestanding freestandin test test-freestanding clean format analysis

all: lib

lib: $(STATIC_OBJ) $(SHARED_LIB)

freestanding: $(FREESTANDING_LIB)

freestandin: freestanding

test: $(HOSTED_TEST_BIN)
	@./test.sh

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

# Analysis
# ┌────────────┬────────────────────────┐
# │    选项    │          作用          │
# ├────────────┼────────────────────────┤
# │ --analyze  │ 启动 Clang 静态分析器  │
# ├────────────┼────────────────────────┤
# │ -Xanalyzer │ 将后续参数传递给分析器 │
# └────────────┴────────────────────────┘

# Analysis options
# ┌─────────────┬──────────────────────────────────────────────────────────┐
# │   检查器    │                         检测内容                         │
# ├─────────────┼──────────────────────────────────────────────────────────┤
# │ core        │ 核心 bug：空指针解引用、除零、内存泄漏、越界访问         │
# ├─────────────┼──────────────────────────────────────────────────────────┤
# │ nullability │ 空值相关：nonnull 注解违反、nullable 使用不当            │
# ├─────────────┼──────────────────────────────────────────────────────────┤
# │ unix        │ Unix/POSIX API 误用：malloc 未检查返回值、fopen 错误处理 │
# └─────────────┴──────────────────────────────────────────────────────────┘

analysis:
	$(foreach src,$(SRC), \
		clang --analyze \
		$(HOSTED_CFLAGS) \
		-Xanalyzer -analyzer-checker=core,nullability,unix \
		$(src);)
	$(foreach hdr,$(HDR), \
		clang --analyze \
		$(HOSTED_CFLAGS) \
		-Xanalyzer -analyzer-checker=core,nullability,unix \
		-include $(hdr) \
		-x c /dev/null;)

format:
	find . -name '*.c' -o -name '*.h' | xargs clang-format -i

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
