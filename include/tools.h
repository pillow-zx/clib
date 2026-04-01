#ifndef __CLIB_TOOLS_H__
#define __CLIB_TOOLS_H__

#include <types.h>

#define CONCAT(a, b) a##b

#define BIT(type, n) ((type)(1) << (n))
#define BITS_U8(n) BIT(u8, n)
#define BITS_U32(n) BIT(u32, n)
#define BITS_U64(n) BIT(u64, n)
#define BIT_SET(x, n) ((x) |= BIT(usize, n))
#define BIT_CLR(x, n) ((x) &= ~BIT(usize, n))
#define BIT_FLIP(x, n) ((x) ^= BIT(usize, n))
#define BIT_TEST(x, n) (!!((x) & BIT(usize, n)))

#define MASK(n) (BIT(usize, n) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & MASK((hi) - (lo) + 1))

#define MMIO_READ(addr) (*(volatile u32 *)(addr))
#define MMIO_WRITE(addr, val) (*(volatile u32 *)(addr) = (val))

#define MAP(func, ...) func(__VA_ARGS__)

#define ARRLEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define container_of(ptr, type, member)                                        \
        ({                                                                     \
                const typeof(((type *)0)->member) *__mptr = (ptr);             \
                (type *)((char *)__mptr - offsetof(type, member));             \
        })

#endif
