#ifndef CLIB_TOOLS_H
#define CLIB_TOOLS_H

#include <types.h>
#include <compiler.h>

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)
#define CONCAT3(a, b, c) CONCAT(CONCAT(a, b), c)

#define BIT(type, n) ((type)(1) << (n))
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

#define MMIO_READ(type, addr) (*(volatile type *)(addr))
#define MMIO_WRITE(type, addr, val) (*(volatile type *)(addr) = (val))

#define typesame(a, b) _Generic((a), typeof(b): 1, default: 0)

#define ARRLEN(arr)                                                            \
        ({                                                                     \
                static_assert(!types_compatible((arr), &(arr)[0]),             \
                              "ARRLEN: argument must be an array, "            \
                              "not a pointer");                                \
                sizeof((arr)) / sizeof((arr)[0]);                              \
        })

#define typecheck(type, x) _Generic((x), type: 1, default: 0)

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

#ifndef container_of
#define container_of(ptr, type, member)                                        \
        ({                                                                     \
                const typeof(((type *)0)->member) *__mptr = (ptr);             \
                (type *)((char *)__mptr - offsetof(type, member));             \
        })
#endif

#endif
