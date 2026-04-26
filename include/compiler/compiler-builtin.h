/**
 * @file compiler-builtin.h
 * @brief GCC/Clang 内建函数的宏封装。
 *
 * @defgroup compiler_builtin 编译器内建宏
 * @{
 */

#ifndef CLIB_COMPILER_COMPILER_BUILTIN_H
#define CLIB_COMPILER_COMPILER_BUILTIN_H

/* === 分支预测 === */

/** @brief 分支预测提示：条件 x 在运行时很可能为真，编译器据此优化指令布局。 */
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

/** @brief 分支预测提示：条件 x 在运行时很可能为假，编译器据此优化指令布局。 */
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

/* === 控制流 === */

/** @brief 标记此处代码路径不可达，允许编译器省略该分支并帮助诊断。 */
#ifndef unreachable
#define unreachable() __builtin_unreachable()
#endif

/** @brief 触发异常退出（SIGTRAP 或未定义指令），用于断言失败等不可恢复场景。 */
#ifndef trap
#define trap() __builtin_trap()
#endif

/* === 编译期工具 === */

/** @brief 返回 1 表示 exp 在编译期可确定为常量。 */
#ifndef constant_p
#define constant_p(exp) __builtin_constant_p(exp)
#endif

/** @brief 编译期三元选择：const_exp 非零返回 exp1，否则返回 exp2；未被选中的分支不被求值。 */
#ifndef choose_expr
#define choose_expr(const_exp, exp1, exp2) \
	__builtin_choose_expr(const_exp, exp1, exp2)
#endif

/** @brief 向编译器承诺 expr 为真，帮助消除死代码并优化路径。
 *  @note 基于 __builtin_unreachable 实现，GCC/Clang 通用。 */
#ifndef assume
#define assume(expr) ((void)((expr) ? (void)0 : __builtin_unreachable()))
#endif

/** @brief 获取指针所指对象的大小（字节）。
 *  @param ptr 目标指针。
 *  @param type 0 返回完整对象大小；1 返回最近包含对象的剩余大小。 */
#ifndef object_size
#define object_size(ptr, type) __builtin_object_size(ptr, type)
#endif

/* === 位操作 === */

/** @brief 最低置位索引（1‑indexed），x 为 0 时返回 0。 */
#ifndef ffs
#define ffs(x) __builtin_ffs(x)
#endif

/** @brief ffs 的 long 变体。 */
#ifndef ffsl
#define ffsl(x) __builtin_ffsl(x)
#endif

/** @brief ffs 的 long long 变体。 */
#ifndef ffsll
#define ffsll(x) __builtin_ffsll(x)
#endif

/** @brief 前导零计数，x 为 0 时行为未定义。 */
#ifndef clz
#define clz(x) __builtin_clz(x)
#endif

/** @brief clz 的 unsigned long 变体。 */
#ifndef clzl
#define clzl(x) __builtin_clzl(x)
#endif

/** @brief clz 的 unsigned long long 变体。 */
#ifndef clzll
#define clzll(x) __builtin_clzll(x)
#endif

/** @brief 尾部零计数，x 为 0 时行为未定义。 */
#ifndef ctz
#define ctz(x) __builtin_ctz(x)
#endif

/** @brief ctz 的 unsigned long 变体。 */
#ifndef ctzl
#define ctzl(x) __builtin_ctzl(x)
#endif

/** @brief ctz 的 unsigned long long 变体。 */
#ifndef ctzll
#define ctzll(x) __builtin_ctzll(x)
#endif

/** @brief 置位数（population count / Hamming 重量）。 */
#ifndef popcount
#define popcount(x) __builtin_popcount(x)
#endif

/** @brief popcount 的 unsigned long 变体。 */
#ifndef popcountl
#define popcountl(x) __builtin_popcountl(x)
#endif

/** @brief popcount 的 unsigned long long 变体。 */
#ifndef popcountll
#define popcountll(x) __builtin_popcountll(x)
#endif

/** @brief 置位数的奇偶校验：置位数为奇数返回 1，偶数返回 0。 */
#ifndef parity
#define parity(x) __builtin_parity(x)
#endif

/** @brief parity 的 unsigned long 变体。 */
#ifndef parityl
#define parityl(x) __builtin_parityl(x)
#endif

/** @brief parity 的 unsigned long long 变体。 */
#ifndef parityll
#define parityll(x) __builtin_parityll(x)
#endif

/** @brief 16 位字节序翻转。 */
#ifndef bswap16
#define bswap16(x) __builtin_bswap16(x)
#endif

/** @brief 32 位字节序翻转。 */
#ifndef bswap32
#define bswap32(x) __builtin_bswap32(x)
#endif

/** @brief 64 位字节序翻转。 */
#ifndef bswap64
#define bswap64(x) __builtin_bswap64(x)
#endif

/* === 溢出检查 === */

/**
 * @brief 加法溢出检查：res = a + b，溢出返回 true。
 * @param a 加数。
 * @param b 加数。
 * @param res 结果指针，类型自适应。
 */
#ifndef add_overflow
#define add_overflow(a, b, res) __builtin_add_overflow(a, b, res)
#endif

/**
 * @brief 减法溢出检查：res = a - b，溢出返回 true。
 * @param a 被减数。
 * @param b 减数。
 * @param res 结果指针，类型自适应。
 */
#ifndef sub_overflow
#define sub_overflow(a, b, res) __builtin_sub_overflow(a, b, res)
#endif

/**
 * @brief 乘法溢出检查：res = a * b，溢出返回 true。
 * @param a 乘数。
 * @param b 乘数。
 * @param res 结果指针，类型自适应。
 */
#ifndef mul_overflow
#define mul_overflow(a, b, res) __builtin_mul_overflow(a, b, res)
#endif

/* === 栈上动态分配 === */

/**
 * @brief 在调用者栈帧上动态分配 size 字节，函数返回时自动释放。
 * @param size 分配字节数。
 * @return 指向分配内存的指针（栈地址），失败后行为未定义。
 */
#ifndef alloca
#define alloca(size) __builtin_alloca(size)
#endif

/**
 * @brief 在调用者栈帧上动态分配并对齐。
 * @param size  分配字节数。
 * @param align 对齐边界（字节），须为 2 的幂。
 * @return 对齐后的栈内存指针。
 */
#ifndef alloca_align
#define alloca_align(size, align) __builtin_alloca_with_align(size, align)
#endif

/* === 调试 / 源位置 === */

/** @brief 返回当前源文件名（const char *）。 */
#ifndef builtin_file
#define builtin_file() __builtin_FILE()
#endif

/** @brief 返回当前行号（int）。 */
#ifndef builtin_line
#define builtin_line() __builtin_LINE()
#endif

/** @brief 返回当前函数名（const char *）。 */
#ifndef builtin_function
#define builtin_function() __builtin_FUNCTION()
#endif

/* === 类型工具 === */

/** @brief 获取成员 d 在结构体 t 中的字节偏移量。 */
#ifndef offsetof
#define offsetof(t, d) __builtin_offsetof(t, d)
#endif

/** @brief 检查 a 和 b 的类型是否兼容（typeof 层面），兼容返回 1。 */
#ifndef types_compatible
#define types_compatible(a, b)                                                 \
        __builtin_types_compatible_p(typeof(a), typeof(b))
#endif

/** @} */

#endif /* ifndef CLIB_COMPILER_COMPILER_BUILTIN */
