# clib

一个轻量级的 C 语言数据结构与工具库，采用头文件为主的设计，提供高性能、易用的基础组件。

## 特性

- **头文件为主**：大部分实现为内联函数，无需编译链接即可使用
- **零依赖**：支持 freestanding 环境，可完全脱离 libc 运行
- **高性能**：使用编译器优化属性，减少函数调用开销
- **类型安全**：提供统一的类型定义，消除平台差异
- **现代 C**：采用 C23 标准，支持 `auto` 类型推导、`nullptr` 等特性

## 架构设计

```
┌─────────────────────────────────────────────────────────────┐
│                       应用层                                │
├─────────────────────────────────────────────────────────────┤
│  list.h  │  rbtree.h  │  bplustree.h  │  dsu.h  │ ringbuf.h │
├──────────┴────────────┴───────────────┴─────────┴───────────┤
│                编译器抽象层 (compiler.h)                    │
├─────────────────────────────────────────────────────────────┤
│                   平台抽象层 (port.h)                       │
├─────────────────────────────────────────────────────────────┤
│                  类型定义层 (types.h)                       │
└─────────────────────────────────────────────────────────────┘
```

**分层设计特点**：

- 底层不依赖上层，支持在内核、嵌入式等 freestanding 环境使用
- 通过不定义 `CLIB_USE_LIBC` 可完全脱离 libc 依赖
- 使用 `__weak` 符号允许用户覆盖默认实现

## 组件

### 基础类型 (`types.h`)

提供统一的类型别名，消除不同平台间的类型差异：

```c
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef __SIZE_TYPE__ usize;
typedef __PTRDIFF_TYPE__ isize;

// 支持 C23 bool 和 auto 关键字
bool flag = true;
auto value = 42;  // 自动推导类型
```

**特点**：使用编译器内置类型（`__SIZE_TYPE__`），无需 libc 头文件即可工作。

### 编译器属性 (`compiler.h`)

提供 GCC/Clang 编译器属性的便携宏定义，包含详尽的文档注释：

```c
__always_inline   // 强制内联
__packed          // 取消对齐填充
__aligned(x)      // 指定对齐
__cleanup(func)   // 自动清理属性
__must_check      // 警告未使用的返回值
__pure            // 纯函数（可读全局，无副作用）
__const           // 常量函数（仅依赖参数）
__hot / __cold    // 热路径/冷路径提示
__likely(x)       // 分支预测提示
```

### 平台抽象层 (`port.h`)

提供内存管理和 I/O 的抽象接口，支持两种模式：

**libc 模式**（定义 `CLIB_USE_LIBC`）：
```c
cmalloc, ccalloc, crealloc, cfree  // 内存管理
cmemcpy, cmemmove, cmemset, cmemcmp // 内存操作
cprintf                              // 格式化输出
```

**freestanding 模式**：用户需自行实现上述函数。

### 双向链表 (`list.h`)

Linux 内核风格的双向循环链表实现：

```c
LIST_HEAD(my_list);                      // 定义并初始化链表
list_add(&node->list, &my_list);         // 头部插入
list_add_tail(&node->list, &my_list);    // 尾部插入
list_del(&node->list);                   // 删除节点
list_for_each(pos, &my_list)             // 遍历链表
list_for_each_entry(pos, &my_list, list) // 遍历并获取容器结构
```

**时间复杂度**：插入/删除 O(1)

### 红黑树 (`rbtree.h`)

自平衡二叉搜索树实现，支持高效查找、插入、删除操作：

```c
DEFINE_RBROOT(my_tree);                   // 定义红黑树根
rb_link_node(&node->rb, parent, &link);   // 链接节点
rb_insert_color(&node->rb, &my_tree);     // 插入后平衡
rb_erase(&node->rb, &my_tree);            // 删除节点

// 迭代遍历
rbtree_for_each(pos, &my_tree)
rbtree_for_each_entry(pos, &my_tree, rb)
```

**时间复杂度**：查找/插入/删除 O(log n)

### B+ 树 (`bplustree.h`)

B+ 树实现，适用于范围查询和顺序访问场景：

```c
bplus_tree_t *tree = bplus_create();
bplus_insert(tree, key, value);               // 插入
bplus_value_t val = bplus_search(tree, key);  // 查找
bplus_delete(tree, key);                      // 删除
bplus_result_t result = bplus_range_query(tree, start, end); // 范围查询
bplus_destroy(tree);
```

**特点**：
- 阶数 64，适合大量数据存储
- 叶子节点链表支持高效范围查询
- 时间复杂度 O(log n)

### 并查集 (`dsu.h`)

带路径压缩和按秩合并的并查集实现：

```c
struct dsu dsu;
i32 parent[N], rank[N];
dsu_init(&dsu, parent, rank, N);
dsu_union(&dsu, x, y);               // 合并集合
bool connected = dsu_connected(&dsu, x, y); // 查询连通性
i32 count = dsu_count(&dsu);         // 获取集合数量
```

**时间复杂度**：近似 O(α(n))，α 为反阿克曼函数

### 环形缓冲区 (`ringbuf.h`)

环形缓冲区实现，适用于生产者-消费者场景：

```c
char buffer[1024];
struct ringbuf rb;
ringbuf_init(&rb, buffer, sizeof(buffer));
ringbuf_write(&rb, data, len);   // 写入数据
ringbuf_read(&rb, buf, len);     // 读取数据
ringbuf_peek(&rb, buf, len);     // 查看数据（不移除）
ringbuf_skip(&rb, len);          // 跳过数据
```

**特点**：单字节操作、容量查询、重置等功能

### 调试工具 (`debug.h`)

带颜色和日志级别的调试输出：

```c
// 设置日志级别
#define LOGLEVEL DEBUG

// 使用方法
print(INFO, "module", "value = %d", value);
println(ERROR, "module", "failed to open file");
```

日志级别：`ALL < TRACE < DEBUG < INFO < ERROR`

### 工具宏 (`tools.h`)

常用工具宏集合：

```c
BIT(u32, 5)           // 生成位掩码 (1 << 5)
BIT_SET(x, 3)         // 设置第 3 位
BIT_CLR(x, 3)         // 清除第 3 位
BIT_TEST(x, 3)        // 测试第 3 位
ARRLEN(arr)           // 数组长度
container_of(ptr, type, member) // 获取包含结构体指针

// MMIO 操作（嵌入式场景）
MMIO_READ(addr)
MMIO_WRITE(addr, val)
```

### 自动内存释放 (`autofree.h`)

利用编译器属性实现自动释放：

```c
void example(void) {
    autofree char *buffer = malloc(1024);
    // 函数退出时自动调用 free(buffer)
}
```

## 构建

### 编译

```bash
# Debug 模式（默认）：包含完整 Sanitizer
make

# Release 模式：LTO + native 优化
make BUILD=release

# 运行测试
make test

# 静态分析
make analysis

# 格式化代码
make format

# 清理
make clean
```

### 编译选项说明

**Debug 模式**：
| 选项 | 作用 |
|------|------|
| `-O0` | 禁用优化，便于调试 |
| `-g` | 生成调试符号 |
| `-fsanitize=address` | 检测内存越界、释放后使用等 |
| `-fsanitize=undefined` | 检测整数溢出、空指针解引用等 |

**Release 模式**：
| 选项 | 作用 |
|------|------|
| `-O3` | 最高级别优化 |
| `-flto` | 链接时优化 |
| `-march=native` | 针对本地 CPU 优化 |

## 项目结构

```
clib/
├── include/           # 头文件 (11 个)
│   ├── types.h        # 基础类型
│   ├── compiler.h     # 编译器属性
│   ├── port.h         # 平台抽象层
│   ├── debug.h        # 调试工具
│   ├── tools.h        # 工具宏
│   ├── list.h         # 双向链表
│   ├── rbtree.h       # 红黑树
│   ├── bplustree.h    # B+ 树
│   ├── dsu.h          # 并查集
│   ├── ringbuf.h      # 环形缓冲区
│   └── autofree.h     # 自动释放
├── src/               # 源文件实现 (2 个)
│   ├── rbtree.c       # 红黑树平衡算法
│   └── bplustree.c    # B+ 树实现
├── test/              # 测试用例 (11 个)
├── Makefile
└── README.md
```

## 使用方式

### 头文件直接使用

大部分组件为头文件内实现，直接包含即可：

```c
#include <types.h>
#include <list.h>

int main(void) {
    LIST_HEAD(my_list);
    // ...
}
```

### 编译链接

对于需要编译的组件（如红黑树、B+树）：

```c
#include <rbtree.h>
#include <bplustree.h>

// 编译时链接
// gcc main.c clib.o -o main
```

### Freestanding 环境

不定义 `CLIB_USE_LIBC`，并自行实现 `port.h` 中声明的函数：

```c
// 提供自定义实现
void *cmalloc(usize size) { return my_allocator_alloc(size); }
void cfree(void *ptr) { my_allocator_free(ptr); }
// ... 其他函数
```

## 适用场景

- **嵌入式/内核开发**：freestanding 支持，零 libc 依赖
- **高性能数据处理**：B+ 树范围查询，红黑树快速查找
- **算法竞赛**：轻量级数据结构，即插即用
- **教学目的**：清晰的代码结构和详尽的注释

## 许可证

MIT License
