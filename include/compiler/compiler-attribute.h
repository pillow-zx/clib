/**
 * @file compiler-attribute.h
 * @brief GCC/Clang __attribute__ 指令的宏封装。
 *
 * @defgroup compiler_attr 编译器属性宏
 * @{
 */

#ifndef CLIB_COMPILER_COMPILER_ATTRIBUTE_H
#define CLIB_COMPILER_COMPILER_ATTRIBUTE_H

/* === 内联控制 === */

/** @brief 强制内联函数，即使未启用优化也展开函数体。 */
#ifndef __always_inline
#define __always_inline inline __attribute__((__always_inline__))
#endif

/** @brief 阻止函数内联。 */
#ifndef __noinline
#define __noinline __attribute__((__noinline__))
#endif

/* === 符号使用 === */

/** @brief 声明符号可能未被使用，抑制 -Wunused 警告。 */
#ifndef __unused
#define __unused __attribute__((__unused__))
#endif

/** @brief 确保符号保留在输出文件中，即使编译器认为未被引用。 */
#ifndef __used
#define __used __attribute__((__used__))
#endif

/** @brief 同 __unused，语义更清晰地表达"可能未使用"。 */
#ifndef __maybe_unused
#define __maybe_unused __attribute__((__unused__))
#endif

/* === 内存布局 === */

/** @brief 取消结构体对齐填充，成员按最小字节紧密排列。 */
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

/** @brief 指定变量或类型的对齐字节数。
 *  @param x 对齐边界（字节），须为 2 的幂。 */
#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

/* === 控制流 === */

/** @brief 声明函数永不返回（如 exit()、abort()），帮助编译器消除死代码警告。 */
#ifndef __noreturn
#define __noreturn __attribute__((__noreturn__))
#endif

/** @brief 显式标记 switch case 故意落空，抑制 -Wimplicit-fallthrough。 */
#ifndef __fallthrough
#define __fallthrough __attribute__((__fallthrough__))
#endif

/* === 函数纯度 === */

/** @brief 纯函数：可以读取全局状态但无副作用，相同参数多次调用可被合并。 */
#ifndef __pure
#define __pure __attribute__((__pure__))
#endif

/** @brief 常量函数：仅依赖参数值，不访问全局状态，无副作用。比 __pure 约束更强。 */
#ifndef __const
#define __const __attribute__((__const__))
#endif

/** @brief 指示返回值是新分配且唯一的指针，帮助编译器进行别名分析。 */
#ifndef __malloc
#define __malloc __attribute__((__malloc__))
#endif

/* === 诊断检查 === */

/** @brief 调用者必须使用返回值，否则产生 -Wunused-result 编译警告。 */
#ifndef __must_check
#define __must_check __attribute__((__warn_unused_result__))
#endif

/** @brief 声明指定参数不可为空指针，启用 -Wnonnull 检查。
 *  @param params 参数索引列表，如 (1, 2)。 */
#ifndef __nonnull
#define __nonnull(params) __attribute__((__nonnull__ params))
#endif

/** @brief 启用 printf 风格格式化字符串类型检查。
 *  @param a 格式串参数索引（从 1 开始）。
 *  @param b 可变参数起始索引（从 1 开始）。 */
#ifndef __printf
#define __printf(a, b) __attribute__((__format__(printf, a, b)))
#endif

/** @brief 启用 scanf 风格格式化字符串类型检查。
 *  @param a 格式串参数索引（从 1 开始）。
 *  @param b 可变参数起始索引（从 1 开始）。 */
#ifndef __scanf
#define __scanf(a, b) __attribute__((__format__(scanf, a, b)))
#endif

/* === 自动清理 === */

/** @brief 变量离开作用域时自动调用指定函数进行资源释放。
 *  @param func 清理函数名，签名为 void func(type *ptr)。 */
#ifndef __cleanup
#define __cleanup(func) __attribute__((__cleanup__(func)))
#endif

/* === 优化提示 === */

/** @brief 标记函数为热路径，编译器将其靠近调用者放置并优先优化执行速度。 */
#ifndef __hot
#define __hot __attribute__((__hot__))
#endif

/** @brief 标记函数为冷路径，编译器将其远离热代码放置并优先优化体积。 */
#ifndef __cold
#define __cold __attribute__((__cold__))
#endif

/* === 段放置 === */

/** @brief 将符号放入指定段。
 *  @param section 目标段名称。 */
#ifndef __section
#define __section(section) __attribute__((__section__(section)))
#endif

/** @brief 弱符号定义，允许用户层提供同名强符号覆盖。 */
#ifndef __weak
#define __weak __attribute__((__weak__))
#endif

/** @brief 将函数放入 .init.text 段，内核初始化阶段结束后可被回收。 */
#ifndef __init
#define __init __attribute__((__section__(".init.text")))
#endif

/** @brief 将数据放入 .init.data 段，内核初始化阶段结束后可被回收。 */
#ifndef __initdata
#define __initdata __attribute__((__section__(".init.data")))
#endif

/** @brief 将函数显式放入 .text 代码段。 */
#ifndef __text
#define __text __attribute__((__section__(".text")))
#endif

/** @brief 将数据显式放入 .rodata 只读数据段。 */
#ifndef __rodata
#define __rodata __attribute__((__section__(".rodata")))
#endif

/** @} */

#endif /* ifndef CLIB_COMPILER_COMPILER_ATTRIBUTE */
