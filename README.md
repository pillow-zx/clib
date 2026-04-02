# clib

一个轻量级的 C 语言数据结构与工具库，采用头文件为主的设计，提供高性能、易用的基础组件。

## 特性

- **头文件为主**：大部分实现为内联函数，无需编译链接即可使用
- **零依赖**：仅依赖标准 C 库
- **高性能**：使用编译器优化属性，减少函数调用开销
- **类型安全**：提供统一的类型定义，避免平台差异

## 组件

### 基础类型 (`types.h`)

提供统一的类型别名，消除不同平台间的类型差异：

```c
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;
typedef size_t usize;
```

### 编译器属性 (`compiler.h`)

提供 GCC/Clang 编译器属性的便携宏定义：

```c
__always_inline   // 强制内联
__packed          // 取消对齐填充
__aligned(x)      // 指定对齐
__cleanup(func)   // 自动清理属性
__must_check      // 警告未使用的返回值
```

### 双向链表 (`list.h`)

Linux 内核风格的双向循环链表实现：

```c
LIST_HEAD(my_list);           // 定义并初始化链表
list_add(&node->list, &my_list);    // 头部插入
list_add_tail(&node->list, &my_list); // 尾部插入
list_del(&node->list);        // 删除节点
list_for_each(pos, &my_list)  // 遍历链表
```

### 红黑树 (`rbtree.h`)

自平衡二叉搜索树实现，支持高效查找、插入、删除操作：

```c
DEFINE_RBROOT(my_tree);       // 定义红黑树根
rb_link_node(&node->rb, parent, &link); // 链接节点
rb_insert_color(&node->rb, &my_tree);   // 插入后平衡
rb_erase(&node->rb, &my_tree); // 删除节点
```

### B+ 树 (`bplustree.h`)

B+ 树实现，适用于范围查询和顺序访问场景：

```c
bplus_tree_t *tree = bplus_create();
bplus_insert(tree, key, value);      // 插入
bplus_value_t val = bplus_search(tree, key); // 查找
bplus_delete(tree, key);             // 删除
bplus_result_t result = bplus_range_query(tree, start, end); // 范围查询
bplus_destroy(tree);
```

### 并查集 (`dsu.h`)

带路径压缩和按秩合并的并查集实现：

```c
struct dsu dsu;
i32 parent[N], rank[N];
dsu_init(&dsu, parent, rank, N);
dsu_union(&dsu, x, y);           // 合并集合
bool connected = dsu_connected(&dsu, x, y); // 查询连通性
i32 count = dsu_count(&dsu);     // 获取集合数量
```

### 环形缓冲区 (`ringbuf.h`)

无锁环形缓冲区实现，适用于生产者-消费者场景：

```c
char buffer[1024];
struct ringbuf rb;
ringbuf_init(&rb, buffer, sizeof(buffer));
ringbuf_write(&rb, data, len);   // 写入数据
ringbuf_read(&rb, buf, len);     // 读取数据
ringbuf_peek(&rb, buf, len);     // 查看数据（不移除）
```

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

```bash
# 编译静态库和动态库
make

# 运行测试
make test

# 格式化代码
make format

# 清理
make clean
```

## 项目结构

```
clib/
├── include/           # 头文件
│   ├── types.h        # 基础类型
│   ├── compiler.h     # 编译器属性
│   ├── debug.h        # 调试工具
│   ├── tools.h        # 工具宏
│   ├── list.h         # 双向链表
│   ├── rbtree.h       # 红黑树
│   ├── bplustree.h    # B+ 树
│   ├── dsu.h          # 并查集
│   ├── ringbuf.h      # 环形缓冲区
│   └── autofree.h     # 自动释放
├── src/               # 源文件实现
├── test/              # 测试用例
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

## 许可证

MIT License
