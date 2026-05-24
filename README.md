# clib

一个面向嵌入式与系统编程场景的轻量级 C 库。项目采用“头文件优先”的组织方式：简单组件直接在 `include/` 中以内联形式提供，复杂组件放在 `src/` 中实现。

README 按以下顺序组织：

1. 项目结构
2. 使用方法
3. 可用接口

## 项目结构

### 目录布局

```text
clib/
├── include/                    # 公共头文件
│   ├── types.h                 # 基础类型
│   ├── compiler.h              # 编译器属性与内建封装入口
│   ├── port.h                  # 平台抽象层
│   ├── tools.h                 # 常用宏工具
│   ├── debug.h                 # 调试输出
│   ├── autofree.h              # 自动释放辅助
│   ├── bitmap.h                # 位图
│   ├── sort.h                  # 排序宏
│   ├── list.h                  # 双向循环链表
│   ├── stack.h                 # 顺序栈
│   ├── ringbuf.h               # 环形缓冲区
│   ├── dsu.h                   # 并查集
│   ├── rbtree.h                # 红黑树
│   ├── fifo.h                  # 泛型 FIFO
│   ├── vec.h                   # 动态数组
│   └── bplustree.h             # B+ 树
├── include/compiler/
│   ├── compiler-attribute.h    # 属性宏定义
│   └── compiler-builtin.h      # 编译器内建封装
├── src/                        # 需要单独编译的实现
│   ├── rbtree.c
│   ├── fifo.c
│   ├── vec.c
│   └── bplustree.c
├── test/                       # 各模块测试
├── Makefile
└── README.md
```

### 分层关系

```text
types.h
  └─ compiler.h
      ├─ tools.h
      └─ port.h
          ├─ debug.h
          ├─ autofree.h
          ├─ vec.h
          ├─ fifo.h
          └─ bplustree.h

tools.h
  ├─ bitmap.h
  ├─ sort.h
  ├─ list.h
  ├─ stack.h
  ├─ ringbuf.h
  ├─ dsu.h
  └─ rbtree.h
```

可以把模块分成三类理解：

- 基础层：`types.h`、`compiler.h`、`tools.h`
- 无宿主依赖的数据结构：`bitmap.h`、`sort.h`、`list.h`、`stack.h`、`ringbuf.h`、`dsu.h`、`rbtree.h`
- 依赖 `port.h` 的组件：`debug.h`、`autofree.h`、`vec.h`、`fifo.h`、`bplustree.h`

## 使用方法

### 构建命令

仓库提供以下常用目标：

```bash
make lib               # 构建 hosted 版本目标文件与共享库
make freestanding      # 构建 freestanding 可用对象
make test              # 运行全部 hosted 测试
make test-freestanding # 运行 freestanding 测试
make analysis          # 根据编译器自动选择静态分析后端
make format            # 使用 clang-format 格式化代码
make clean             # 清理构建产物
```

项目默认使用：

- `gcc`
- `-std=gnu23`
- `-Werror`
- `-Iinclude`

也可以在执行 `make` 时通过变量覆盖默认行为，例如：

```bash
make CC=clang lib
make CC=clang test
make CC=gcc BUILD=release lib
```

其中：

- `CC` 用于选择编译器，支持 `gcc` / `clang`
- `BUILD=debug` 为默认构建模式
- `BUILD=release` 会启用发布优化参数

### 编译器相关行为

`Makefile` 会根据编译器自动切换一部分参数：

- `gcc` 默认启用 GCC 专属严格告警，并在 `analysis` 中使用 `-fanalyzer -fanalyzer-verbosity=2`
- `clang` 默认启用 Clang 专属严格告警，并在 `analysis` 中使用 `--analyze`

也就是说，下面两条命令会走不同的静态分析后端：

```bash
make analysis
make CC=clang analysis
```

### 直接在项目中引用头文件

对于头文件内联实现的模块，只需要把 `include/` 加入头文件搜索路径：

```bash
gcc -std=gnu23 -Iinclude demo.c -o demo
```

示例：

```c
#include <stack.h>

int main(void)
{
        STACK_DEFINE(st, int, 8);
        int value = 42;
        int out = 0;

        stack_push(&st, &value);
        stack_pop(&st, &out);
        return out != 42;
}
```

### 使用需要单独编译的模块

以下模块有 `.c` 实现，需要链接构建产物或对应源文件：

- `vec.h` 对应 `src/vec.c`
- `fifo.h` 对应 `src/fifo.c`
- `rbtree.h` 对应 `src/rbtree.c`
- `bplustree.h` 对应 `src/bplustree.c`

例如：

```bash
gcc -std=gnu23 -Iinclude app.c src/vec.c src/fifo.c src/rbtree.c src/bplustree.c -o app
```

或者先执行：

```bash
make lib
```

再把 `build/clib.o` 或 `build/libclib.so` 链接到你的工程中。

### `port.h` 的接入方式

`port.h` 抽象了内存和输出接口。依赖它的模块不会直接调用 `malloc`、`free`、`printf`，而是调用：

- `cmalloc`
- `ccalloc`
- `crealloc`
- `cfree`
- `cmemcpy`
- `cmemmove`
- `cmemset`
- `cmemcmp`
- `cprintf`

这些符号在本项目中属于“移植层 hook（integration ABI）”，用于对接宿主环境；它们不是 `libclib.so` 对外稳定公共 API 的一部分。项目内部提供的默认实现使用 `__hidden __weak`，以避免符号泄漏，同时允许外部以同名强符号覆盖。

有两种使用方式。

#### 1. hosted / libc 模式

仓库默认按这种方式构建。定义 `CLIB_USE_LIBC` 后，`port.h` 会提供基于 libc 的弱符号实现（`cmalloc/ccalloc/crealloc/cfree/cprintf`）；而 `cmemcpy/cmemmove/cmemset/cmemcmp` 提供默认弱实现，可在不依赖 `string.h` 的场景下使用。

```bash
gcc -std=gnu23 -DCLIB_USE_LIBC -Iinclude app.c src/vec.c -o app
```

#### 2. freestanding / 自定义宿主模式

如果你的环境没有 libc，或者你希望替换分配器与输出函数，则自行提供这些符号：

```c
#include <port.h>

void *cmalloc(usize size) { return my_alloc(size); }
void cfree(void *ptr) { my_free(ptr); }
int cprintf(const char *fmt, ...) { return 0; }
```

如果只使用 `types.h`、`tools.h`、`list.h`、`ringbuf.h` 这类不依赖 `port.h` 的模块，则不需要提供这些实现。

## 可用接口

下面按头文件分组给出库中的主要公开接口与用途。

### 公共 API 与移植层 Hook 边界

公共 API（`libclib.so` 对外接口）以导出符号为准，当前由 `test/test_exports.sh` 白名单约束，主要包含：

- `bplus_*`（树的创建、查询、插入、删除、销毁、统计）
- `fifo_*`（队列读写与状态）
- `vec_*`（动态数组读写与状态）
- `rb_insert_color` / `rb_erase_color` / `rb_erase`

移植层 hook（`port.h`）包括：

- `cmalloc` `ccalloc` `crealloc` `cfree`
- `cmemcpy` `cmemmove` `cmemset` `cmemcmp`
- `cprintf`

它们用于宿主适配和可替换实现，不作为共享库稳定导出契约；默认实现通过 `__hidden __weak` 提供。

### 基础层

#### `types.h`

提供固定宽度基础类型和指针大小类型：

- 整数：`u8` `u16` `u32` `u64` `i8` `i16` `i32` `i64`
- 浮点：`f32` `f64`
- 指针尺寸：`usize` `isize`
- 兼容别名：`uchar` `schar`

在旧标准环境下，还会兼容提供：

- `bool`
- `true` / `false`
- `nullptr`
- `auto`
- `typeof(x)`

#### `compiler.h`

聚合编译器属性与内建封装，常用能力包括：

- `__always_inline`
- `__noinline`
- `__packed`
- `__aligned(x)`
- `__cleanup(func)`
- `__must_check`
- `__pure`
- `__const`
- `__malloc`
- `__hot` / `__cold`
- `likely(x)` / `unlikely(x)`
- `__printf(fmt, args)`
- `__nonnull(...)`
- `__weak`

#### `tools.h`

提供通用宏工具：

- 拼接与类型检查：`CONCAT` `CONCAT3` `typesame` `typecheck`
- MMIO：`MMIO_READ(type, addr)` `MMIO_WRITE(type, addr, val)`
- 数组：`ARRLEN(arr)`
- 容器定位：`container_of(ptr, type, member)`
- 编译期表达式：`constexpr` `constexpr_if` `constexpr_val`
- 位操作：`BIT` `BITS_U8` `BITS_U32` `BITS_U64` `BIT_SET` `BIT_CLR` `BIT_FLIP` `BIT_TEST`
- 位域提取：`MASK(n)` `BITS(x, hi, lo)`
- 对齐：`ALIGN_UP` `ALIGN_DOWN` `IS_ALIGNED`
- 比较：`MAX(a, b)` `MIN(a, b)`

### 平台与调试

#### `port.h`

平台抽象接口：

- 内存分配：`cmalloc` `ccalloc` `crealloc` `cfree`
- 内存操作：`cmemcpy` `cmemmove` `cmemset` `cmemcmp`
- 输出：`cprintf`

#### `debug.h`

提供带颜色和级别过滤的调试输出宏。

日志级别：

- `ALL`
- `TRACE`
- `DEBUG`
- `INFO`
- `NOTICE`
- `WARNING`
- `ERROR`

输出宏：

- `print(level, format, ...)`
- `println(level, format, ...)`
- `error(format, ...)`
- `warn(format, ...)`
- `notice(format, ...)`
- `info(format, ...)`
- `debug(format, ...)`
- `trace(format, ...)`

使用前可通过 `LOGLEVEL` 控制编译期日志级别：

```c
#define LOGLEVEL INFO
#include <debug.h>
```

#### `autofree.h`

为自动释放场景提供：

- `auto_free(void *ptr)`
- `autofree`

示例：

```c
#include <autofree.h>

int main(void)
{
        autofree char *buf = cmalloc(128);
        return buf == nullptr;
}
```

### 容器与算法

#### `bitmap.h`

用于按位管理布尔状态，适合资源分配、标记位集合等场景。

类型与容量辅助：

- `struct bitmap`
- `BITMAP_WORD_BITS`
- `BITMAP_WORDS(nbits)`
- `BITMAP_BYTES(nbits)`
- `BITMAP_DECLARE(name, bits)`

主要接口：

- `bitmap_init`
- `bitmap_zero`
- `bitmap_fill`
- `bitmap_set`
- `bitmap_clear`
- `bitmap_test`
- `bitmap_assign`
- `bitmap_set_range`
- `bitmap_clear_range`
- `bitmap_find_first_set`
- `bitmap_find_first_zero`
- `bitmap_find_next_set`
- `bitmap_find_next_zero`

#### `sort.h`

提供基于宏的原地排序，要求传入真正的数组而不是指针。

- 插入排序：`insert_sort_asc` `insert_sort_desc` `insert_sort(arr, asc)`
- 冒泡排序：`bubble_sort_asc` `bubble_sort_desc` `bubble_sort(arr, asc)`
- 选择排序：`select_sort_asc` `select_sort_desc` `select_sort(arr, asc)`

#### `list.h`

Linux 内核风格的侵入式双向循环链表。

核心类型与初始化：

- `struct list_head`
- `LIST_HEAD_INIT(name)`
- `LIST_HEAD(name)`
- `INIT_LIST_HEAD`

节点操作：

- `list_add`
- `list_add_tail`
- `list_del`
- `list_del_init`
- `list_move`
- `list_move_tail`

查询：

- `list_empty`
- `list_empty_careful`
- `list_first`
- `list_last`
- `list_size`
- `list_is_head`

遍历：

- `list_for_each`
- `list_for_each_safe`
- `list_entry`
- `list_for_each_entry`
- `list_for_each_entry_safe`

#### `stack.h`

基于外部缓冲区的顺序栈，不做动态分配。

核心类型与定义：

- `struct stack`
- `STACK_INIT(ptr, elem_sz, cap)`
- `STACK_DEFINE(name, type, cap)`

主要接口：

- `stack_init`
- `stack_empty`
- `stack_full`
- `stack_size`
- `stack_capacity`
- `stack_reset`
- `stack_push`
- `stack_pop`
- `stack_peek`

#### `ringbuf.h`

面向字节流的环形缓冲区，使用外部提供的缓冲区。

核心类型与定义：

- `struct ringbuf`
- `RINGBUF_INIT(ptr, sz)`
- `RINGBUF_DEFINE(name, buffer, size)`
- `RINGBUF_SIZE(rb)`

主要接口：

- `ringbuf_init`
- `ringbuf_empty`
- `ringbuf_full`
- `ringbuf_used`
- `ringbuf_free`
- `ringbuf_write`
- `ringbuf_read`
- `ringbuf_write_byte`
- `ringbuf_read_byte`
- `ringbuf_peek_byte`
- `ringbuf_reset`
- `ringbuf_peek`
- `ringbuf_skip`

#### `dsu.h`

并查集实现，使用外部数组保存父节点和秩信息。

类型：

- `struct dsu`

主要接口：

- `dsu_init`
- `dsu_find`
- `dsu_find_iterative`
- `dsu_union`
- `dsu_connected`
- `dsu_count`

#### `rbtree.h`

红黑树基础设施，提供节点组织、旋转辅助、遍历宏与再平衡接口。比较逻辑与查找逻辑由使用者自己定义。

核心类型与定义：

- `struct rb_node`
- `struct rb_root`
- `RB_ROOT`
- `DEFINE_RBROOT(name)`
- `RB_EMPTY_ROOT(root)`

节点与颜色辅助：

- `rb_node_init`
- `rb_parent`
- `rb_is_red`
- `rb_set_red`
- `rb_set_black`
- `rb_link_node`

遍历与边界：

- `rb_first`
- `rb_last`
- `rb_next`
- `rb_prev`
- `rbtree_for_each`
- `rbtree_for_each_safe`
- `rbtree_for_each_reverse`
- `rbtree_for_each_reverse_safe`
- `rbtree_entry`
- `rbtree_for_each_entry`
- `rbtree_for_each_entry_safe`
- `rbtree_for_each_entry_reverse`
- `rbtree_for_each_entry_reverse_safe`

需要链接 `src/rbtree.c` 的接口：

- `rb_insert_color`
- `rb_erase_color`
- `rb_erase`

#### `fifo.h`

基于外部缓冲区的泛型 FIFO 队列。元素大小由 `elem_size` 指定，支持任意 POD 数据。

核心类型与定义：

- `struct fifo`
- `FIFO_INIT(ptr, elem_sz, cap)`
- `FIFO_DEFINE(name, type, cap)`

主要接口：

- `fifo_init`
- `fifo_empty`
- `fifo_full`
- `fifo_size`
- `fifo_capacity`
- `fifo_reset`
- `fifo_push`
- `fifo_pop`
- `fifo_peek`
- `fifo_write`
- `fifo_read`

#### `vec.h`

动态数组，依赖 `port.h` 提供的分配与内存操作接口。

核心类型与定义：

- `struct vector`
- `VECSZ`
- `VEC_INIT(elem_size)`
- `VEC_DEFINE(name, type)`

主要接口：

- `vec_init`
- `vec_empty`
- `vec_size`
- `vec_capacity`
- `vec_reset`
- `vec_push`
- `vec_pop`
- `vec_write`
- `vec_read`

说明：

- `vec_init` 和 `VEC_DEFINE` 会申请内部缓冲区
- 使用结束后应调用 `vec_destroy(&vec)` 释放

#### `bplustree.h`

B+ 树实现，键类型为 `i32`，值类型为 `void *`，适合做有序索引和范围查询。

核心常量与类型：

- `BPLUS_ORDER`
- `BPLUS_MAX_KEYS`
- `BPLUS_MIN_KEYS`
- `bplus_key_t`
- `bplus_value_t`
- `node_type_t`
- `bplus_node_t`
- `bplus_tree_t`
- `bplus_result_t`

节点状态辅助：

- `node_is_full`
- `node_is_underflow`

主要接口：

- `bplus_create`
- `bplus_destroy`
- `bplus_insert`
- `bplus_delete`
- `bplus_search`
- `bplus_find_leaf`
- `bplus_range_query`
- `bplus_result_free`
- `bplus_count`
- `bplus_height`

范围查询示例：

```c
#include <bplustree.h>

int main(void)
{
        bplus_tree_t *tree = bplus_create();
        int value = 123;

        bplus_insert(tree, 10, &value);

        bplus_result_t result = bplus_range_query(tree, 1, 20);
        bplus_result_free(&result);
        bplus_destroy(tree);
        return 0;
}
```

## 测试

仓库中的测试按模块拆分在 `test/` 下，例如：

- `test/test_list.c`
- `test/test_ringbuf.c`
- `test/test_fifo.c`
- `test/test_vec.c`
- `test/test_rbtree.c`
- `test/test_bplustree_basic.c`

日常修改后建议至少执行：

```bash
make analysis
make test
```
