#ifndef CLIB_TOOLS_H
#define CLIB_TOOLS_H

#include <types.h>
#include <compiler.h>

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)
#define CONCAT3(a, b, c) CONCAT(CONCAT(a, b), c)

#define MMIO_READ(type, addr) (*(volatile type *)(addr))
#define MMIO_WRITE(type, addr, val) (*(volatile type *)(addr) = (val))

#define typecheck(type, x) _Generic((x), type: 1, default: 0)
#define typesame(a, b) types_compatible(a, b)

#define ISARR(arr, msg) static_assert(!typesame((arr), (&(arr)[0])), msg)

#define ARRLEN(arr)                                                            \
        ({                                                                     \
                ISARR(arr,                                                     \
                      "ARRLEN: argument must be an array, not a pointer");     \
                sizeof((arr)) / sizeof((arr)[0]);                              \
        })

#define MAX(a, b)                                                              \
        ({                                                                     \
                static_assert(                                                 \
                        typesame(a, b),                                        \
                        "MAX Require both arguments to be the same type");     \
                typeof(a) _a = (a);                                            \
                typeof(b) _b = (b);                                            \
                _a > _b ? _a : _b;                                             \
        })

#define MIN(a, b)                                                              \
        ({                                                                     \
                static_assert(                                                 \
                        typesame(a, b),                                        \
                        "MIN Require both arguments to be the same type");     \
                typeof(a) _a = (a);                                            \
                typeof(b) _b = (b);                                            \
                _a < _b ? _a : _b;                                             \
        })

#define container_of(ptr, type, member)                                        \
        ({                                                                     \
                static_assert(typesame(*(ptr), ((type *)0)->member) ||         \
                                      typesame(*(ptr), void),                  \
                              "pointer type mismatch in container_of()");      \
                (type *)((void *)((usize)(ptr) - offsetof(type, member)));     \
        })

#define container_of_const(ptr, type, member)                                  \
        ({                                                                     \
                static_assert(typesame(*(ptr), ((type *)0)->member) ||         \
                                      typesame(*(ptr), void),                  \
                              "pointer type mismatch in container_of()");      \
                _Generic((ptr),                                                \
                        const typeof(*(ptr)) *: (const type *)((               \
                                const void *)((const char *)(ptr) -            \
                                              offsetof(type, member))),        \
                        default: (                                             \
                                 (type *)((void *)((usize)(ptr) -              \
                                                   offsetof(type, member))))); \
        })

#define constexpr(expr)                                                        \
        ({                                                                     \
                static_assert(constant_p(expr),                                \
                              "constexpr: requires a compile-time constant "   \
                              "expression");                                   \
                (expr);                                                        \
        })

#define constexpr_if(const_expr, true_expr, false_expr)                        \
        choose_expr(const_expr, true_expr, false_expr)

#define constexpr_val(expr, fallback)                                          \
        choose_expr(constant_p(expr), (expr), (fallback))

#define BIT(type, n) ((type)(1) << constexpr((n)))
#define BITS_U8(n) BIT(u8, n)
#define BITS_U32(n) BIT(u32, n)
#define BITS_U64(n) BIT(u64, n)
#define BIT_SET(x, n) ((x) |= BIT(typeof(x), n))
#define BIT_CLR(x, n) ((x) &= ~BIT(typeof(x), n))
#define BIT_FLIP(x, n) ((x) ^= BIT(typeof(x), n))
#define BIT_TEST(x, n) (!!((x) & BIT(typeof(x), n)))

#define MASK(n) (BIT(usize, n) - 1)
#define BITS(x, hi, lo)                                                        \
        ({                                                                     \
                static_assert((hi) >= (lo), "BITS: hi must be >= lo");         \
                static_assert((lo) >= 0, "BITS: lo must be >= 0");             \
                (((x) >> (lo)) & MASK((hi) - (lo) + 1));                       \
        })

#define __ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))

#define ALIGN_UP(x, a)                                                         \
        ({                                                                     \
                typeof(x) _x = (x);                                            \
                typeof(a) _a = (a);                                            \
                static_assert(constant_p(_a) ? ((_a) != 0) : 1,                \
                              "ALIGN_UP: alignment must be non-zero");         \
                static_assert(constant_p(_a) ? (((_a) & ((_a) - 1)) == 0) : 1, \
                              "ALIGN_UP: alignment must be a power of two");   \
                __ALIGN_MASK(_x, _a - 1);                                      \
        })

#define ALIGN_DOWN(x, a)                                                       \
        ({                                                                     \
                typeof(x) _x = (x);                                            \
                typeof(a) _a = (a);                                            \
                static_assert(constant_p(_a) ? ((_a) != 0) : 1,                \
                              "ALIGN_DOWN: alignment must be non-zero");       \
                static_assert(constant_p(_a) ? (((_a) & ((_a) - 1)) == 0) : 1, \
                              "ALIGN_DOWN: alignment must be a power of two"); \
                _x & ~(_a - 1);                                                \
        })

#define IS_ALIGNED(x, a)                                                       \
        ({                                                                     \
                typeof(x) _x = (x);                                            \
                typeof(a) _a = (a);                                            \
                static_assert(constant_p(_a) ? ((_a) != 0) : 1,                \
                              "IS_ALIGNED: alignment must be non-zero");       \
                static_assert(constant_p(_a) ? (((_a) & ((_a) - 1)) == 0) : 1, \
                              "IS_ALIGNED: alignment must be a power of two"); \
                ((_x & (_a - 1)) == 0);                                        \
        })

#endif
