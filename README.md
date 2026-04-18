# clib

一个轻量级的 C 语言数据结构与工具库，采用头文件优先的设计，提供高性能、零依赖的基础组件。

## 特性

- **头文件优先**：大部分实现为 `static inline` 函数，无需额外编译链接
- **零依赖**：支持 freestanding 环境，可完全脱离 libc 运行
- **现代 C**：采用 C23 标准，支持 `auto` 类型推导、`nullptr`、`bool` 关键字
- **类型安全**：统一的固定宽度类型定义，消除平台差异
- **高质量**：编译开启完整 Sanitizer，所有组件均有测试覆盖

## 架构

```
┌──────────────────────────────────────────────────────────────────┐
│                            应用层                                │
├──────────────────────────────────────────────────────────────────┤
│  list.h  │  rbtree.h  │  bplustree.h  │  dsu.h  │  ringbuf.h     │
├──────────┴────────────┴───────────────┴─────────┴────────────────┤
│                    编译器抽象层 (compiler.h)                     │
├──────────────────────────────────────────────────────────────────┤
│                     平台抽象层 (port.h)                          │
├──────────────────────────────────────────────────────────────────┤
│                     类型定义层 (types.h)                         │
└──────────────────────────────────────────────────────────────────┘
```

底层不依赖上层，各层可独立在内核、嵌入式等 freestanding 环境中使用。

## 项目结构

```
clib/
├── include/
│   ├── types.h        # 基础类型定义
│   ├── compiler.h     # 编译器属性宏
│   ├── port.h         # 平台抽象层（内存/IO）
│   ├── tools.h        # 工具宏
│   ├── debug.h        # 调试输出
│   ├── autofree.h     # 自动内存释放
│   ├── list.h         # 双向循环链表
│   ├── rbtree.h       # 红黑树
│   ├── bplustree.h    # B+ 树
│   ├── dsu.h          # 并查集
│   ├── stack.h        # 栈
│   └── ringbuf.h      # 环形缓冲区
├── src/
│   ├── rbtree.c       # 红黑树平衡算法实现
│   └── bplustree.c    # B+ 树完整实现
├── test/              # 测试用例（11 个套件）
├── Makefile
└── README.md
```

## 组件

### `types.h` — 基础类型

提供固定宽度整数类型和指针大小类型，使用编译器内置类型，无需 libc：

```c
typedef unsigned char  u8;   typedef signed char  i8;
typedef unsigned short u16;  typedef signed short i16;
typedef unsigned int   u32;  typedef signed int   i32;
typedef unsigned long long u64; typedef signed long long i64;

typedef float  f32;
typedef double f64;

typedef __SIZE_TYPE__    usize;  // 等价 size_t，无 libc 依赖
typedef __PTRDIFF_TYPE__ isize;  // 等价 ptrdiff_t
```

C23 兼容：`bool`/`true`/`false`/`nullptr`/`auto` 在旧标准下自动提供兼容宏。

---

### `compiler.h` — 编译器属性

GCC/Clang 编译器属性的便携宏，带详细文档：

```c
__always_inline    // 强制内联
__noinline         // 禁止内联
__packed           // 取消对齐填充
__aligned(x)       // 指定对齐字节数
__cleanup(func)    // 作用域结束时自动调用 func
__must_check       // 警告调用方未检查返回值
__pure             // 纯函数（可读全局状态，无副作用）
__const            // 常量函数（仅依赖参数，不读全局）
__malloc           // 返回值为新分配的内存指针
__hot / __cold     // 热路径 / 冷路径优化提示
likely(x) / unlikely(x)      // 分支预测提示
__printf(fmt, args)          // printf 格式检查
__nonnull(...)               // 参数非空检查
__weak             // 弱符号，允许用户覆盖实现
```

---

### `port.h` — 平台抽象层

抽象内存管理和格式化输出，支持两种工作模式：

**libc 模式**（编译时定义 `CLIB_USE_LIBC`）：

```c
// 内存管理
void *cmalloc(usize size);
void *ccalloc(usize num, usize size);
void *crealloc(void *ptr, usize size);
void  cfree(void *ptr);

// 内存操作
void *cmemcpy(void *dest, const void *src, usize size);
void *cmemmove(void *dest, const void *src, usize size);
void *cmemset(void *dest, int value, usize size);
int   cmemcmp(const void *s1, const void *s2, usize size);

// 格式化输出
int cprintf(const char *fmt, ...);
```

所有 libc 实现均标记为 `__weak`，用户可在链接时覆盖。

**freestanding 模式**（不定义 `CLIB_USE_LIBC`）：

```c
// 自行提供上述函数的实现，例如：
void *cmalloc(usize size) { return my_allocator(size); }
void  cfree(void *ptr)    { my_free(ptr); }
```

---

### `list.h` — 双向循环链表

Linux 内核风格的侵入式双向循环链表，O(1) 插入/删除：

```c
// 定义链表头
LIST_HEAD(my_list);

// 节点操作
list_add(&node->list, &my_list);       // 头部插入
list_add_tail(&node->list, &my_list);  // 尾部插入
list_del(&node->list);                 // 删除节点
list_move(&node->list, &other_list);   // 移动到另一个链表头部
list_move_tail(&node->list, &other);   // 移动到另一个链表尾部

// 查询
list_empty(&my_list);         // 是否为空
list_first(&my_list);         // 第一个节点
list_last(&my_list);          // 最后一个节点
list_size(&my_list);          // 节点数量（O(n)）

// 遍历
list_for_each(pos, &my_list)
list_for_each_safe(pos, n, &my_list)           // 遍历中可安全删除
list_for_each_entry(pos, &my_list, member)     // 遍历并获取容器结构体
list_for_each_entry_safe(pos, n, &my_list, member)
```

---

### `rbtree.h` — 红黑树

自平衡二叉搜索树，O(log n) 查找/插入/删除，支持双向迭代：

```c
// 定义树根
DEFINE_RBROOT(my_tree);

// 插入（需用户实现比较逻辑）
struct rb_node **link = &my_tree.node;
struct rb_node *parent = nullptr;
// ... 找到插入位置 ...
rb_link_node(&node->rb, parent, link);
rb_insert_color(&node->rb, &my_tree);   // 插入后重新平衡

// 删除
rb_erase(&node->rb, &my_tree);

// 查找最小/最大值
struct rb_node *min = rb_first(&my_tree);
struct rb_node *max = rb_last(&my_tree);

// 前驱/后继
struct rb_node *next = rb_next(node);
struct rb_node *prev = rb_prev(node);

// 正序遍历
rbtree_for_each(pos, &my_tree)
rbtree_for_each_safe(pos, n, &my_tree)
rbtree_for_each_entry(pos, &my_tree, member)
rbtree_for_each_entry_safe(pos, n, &my_tree, member)

// 逆序遍历
rbtree_for_each_reverse(pos, &my_tree)
rbtree_for_each_reverse_safe(pos, n, &my_tree)
rbtree_for_each_entry_reverse(pos, &my_tree, member)
rbtree_for_each_entry_reverse_safe(pos, n, &my_tree, member)
```

---

### `bplustree.h` / `bplustree.c` — B+ 树

阶数为 64 的 B+ 树实现，叶子节点以链表相连，支持高效范围查询：

```c
// 创建/销毁
bplus_tree_t *tree = bplus_create();
bplus_destroy(tree);

// 增删查
bool ok  = bplus_insert(tree, key, value);   // 插入键值对
bool ok  = bplus_delete(tree, key);          // 删除键
void *val = bplus_search(tree, key);         // 精确查找

// 范围查询（返回动态分配结果，需手动释放）
bplus_result_t result = bplus_range_query(tree, start, end);
for (usize i = 0; i < result.count; i++) {
    // result.keys[i], result.values[i]
}
bplus_result_free(&result);

// 元信息
usize count = bplus_count(tree);    // 键数量
i32 height  = bplus_height(tree);   // 树高度
```

类型说明：`bplus_key_t` 为 `i32`，`bplus_value_t` 为 `void *`。

---

### `dsu.h` — 并查集

带路径压缩和按秩合并的并查集，时间复杂度近似 O(α(n))：

```c
struct dsu dsu;
i32 parent[N], rank[N];

dsu_init(&dsu, parent, rank, N);         // 初始化，使用外部数组（无动态分配）

dsu_union(&dsu, x, y);                   // 合并 x 和 y 所在的集合
bool ok = dsu_connected(&dsu, x, y);    // 判断 x、y 是否连通
i32 cnt = dsu_count(&dsu);              // 当前集合数量

// 两种 find 实现
i32 root = dsu_find(&dsu, x);           // 递归版（路径压缩）
i32 root = dsu_find_iterative(&dsu, x); // 迭代版（路径压缩）
```

---

### `ringbuf.h` — 环形缓冲区

固定大小的环形缓冲区，适用于生产者-消费者和流式数据场景：

```c
char buf[1024];
struct ringbuf rb;
ringbuf_init(&rb, buf, sizeof(buf));
// 或使用宏定义：
// RINGBUF_DEFINE(rb, buf, sizeof(buf));

// 块操作
usize written = ringbuf_write(&rb, data, len);  // 写入（返回实际写入字节数）
usize read    = ringbuf_read(&rb, dst, len);    // 读取并消费
usize peeked  = ringbuf_peek(&rb, dst, len);    // 读取但不消费
usize skipped = ringbuf_skip(&rb, len);         // 跳过（不读取内容）

// 单字节操作
int ret  = ringbuf_write_byte(&rb, byte);   // 写入一字节，满时返回 -1
int byte = ringbuf_read_byte(&rb);          // 读取一字节，空时返回 -1
int byte = ringbuf_peek_byte(&rb);          // 查看首字节，空时返回 -1

// 状态查询
bool   empty = ringbuf_empty(&rb);
bool   full  = ringbuf_full(&rb);
usize  used  = ringbuf_used(&rb);
usize  free  = ringbuf_free(&rb);

// 重置
ringbuf_reset(&rb);
```

---

### `debug.h` — 调试输出

带颜色显示和日志级别过滤的调试宏：

```c
// 通过宏控制最低日志级别（默认 ALL，即全部输出）
#define LOGLEVEL INFO   // 只输出 INFO 及以上级别

// 日志级别（数值从低到高）
// ALL(0) < TRACE(1) < DEBUG(3) < INFO(2) < ERROR(4)

print(INFO,  "module", "count = %d", count);       // 不换行
println(ERROR, "net", "connection failed: %s", msg); // 换行
```

输出格式：`[LEVEL][place][file:line func] message`，按级别着色显示。

---

### `tools.h` — 工具宏

```c
// 位操作
BIT(u32, 5)           // (u32)(1 << 5)
BITS_U8(n)            // u8 位掩码
BIT_SET(x, n)         // 置位第 n 位
BIT_CLR(x, n)         // 清除第 n 位
BIT_FLIP(x, n)        // 翻转第 n 位
BIT_TEST(x, n)        // 测试第 n 位（返回 0/1）
MASK(n)               // 低 n 位掩码
BITS(x, hi, lo)       // 提取 x 的 [hi:lo] 位段

// 数组
ARRLEN(arr)           // 编译期数组长度

// 嵌入式 MMIO
MMIO_READ(type, addr)       // 读 type 长度寄存器
MMIO_WRITE(type, addr, val) // 写 type 长度寄存器

// 容器指针
container_of(ptr, type, member)  // 由成员指针获取外层结构体指针

// 类型检测
typecheck(type, x) // 检查类型是否正确

// 获得最大/最小值
max(a, b)       // 获得a, b中的最大值
min(a, b)       // 获得a, b中的最小值
```

---

### `autofree.h` — 自动内存释放

基于 `__cleanup` 属性，在变量离开作用域时自动调用 `cfree`：

```c
#include <autofree.h>

void example(void) {
    autofree char *buf = cmalloc(1024);
    autofree char *tmp = cmalloc(256);
    // 函数返回时，buf 和 tmp 自动释放，无需手动 cfree
}
```

## 构建

```bash
# Debug 模式（默认）：禁用优化，开启完整 Sanitizer
make

# Release 模式：LTO + O3 + native 优化
make BUILD=release

# 运行全部测试
make test

# 静态分析
make analysis

# 代码格式化
make format

# 清理构建产物
make clean
```

### 编译选项

| 模式 | 选项 | 说明 |
|------|------|------|
| Debug | `-O0 -g` | 关闭优化，保留调试信息 |
| Debug | `-fsanitize=address` | 检测内存越界、释放后使用等 |
| Debug | `-fsanitize=undefined` | 检测未定义行为（整数溢出、空指针等） |
| Debug | `-fsanitize=nullability` | 检测可空性违规 |
| Release | `-O3 -flto` | 最高优化 + 链接时优化 |
| Release | `-march=native` | 针对本机 CPU 指令集优化 |

编译始终开启 `-Wall -Wextra -Wshadow -Wformat=2 -Wconversion -Werror`。

## 使用方式

### 头文件直接引用

大部分组件为纯头文件实现，直接包含即可使用：

```c
#include <types.h>
#include <list.h>
#include <dsu.h>
#include <ringbuf.h>
```

### 需要链接的组件

红黑树和 B+ 树有独立 `.c` 文件，需要一起编译：

```bash
# 编译库目标文件
make

# 链接到自己的程序
gcc main.c build/clib.o -Iinclude -DCLIB_USE_LIBC -o main
```

### Freestanding 环境

不定义 `CLIB_USE_LIBC`，自行实现 `port.h` 中声明的函数：

```c
// 提供自定义内存分配器
void *cmalloc(usize size)  { return my_heap_alloc(size); }
void  cfree(void *ptr)     { my_heap_free(ptr); }
void *cmemset(void *d, int v, usize n) { /* ... */ return d; }
// ... 其余函数同理
```

## 适用场景

- **嵌入式 / 内核开发**：freestanding 模式，零 libc 依赖
- **高性能数据处理**：B+ 树范围查询，红黑树 O(log n) 有序访问
- **系统级编程**：侵入式链表、MMIO 宏、自动内存管理
- **算法竞赛**：轻量数据结构，头文件即插即用

## 许可证

MIT License
