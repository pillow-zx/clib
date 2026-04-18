#ifndef __CLIB_TOOLS_H__
#define __CLIB_TOOLS_H__

#include <types.h>
#include <compiler.h>

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)
#define CONCAT3(a, b, c) CONCAT(CONCAT(a, b), c)

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

#define MMIO_READ(type, addr) (*(volatile type *)(addr))
#define MMIO_WRITE(type, addr, val) (*(volatile type *)(addr) = (val))

#define MAP(func, ...) func(__VA_ARGS__)

#define ARRLEN(arr)                                                            \
        (sizeof(arr) / sizeof((arr)[0]) +                                      \
         0 * sizeof(&(arr) == (typeof((arr)[0])(*)[]) nullptr))

#define typecheck(type, x) (!!(sizeof((type)1 == (x))))

#ifndef container_of
#define container_of(ptr, type, member)                                        \
        ({                                                                     \
                const typeof(((type *)0)->member) *__mptr = (ptr);             \
                (type *)((char *)__mptr - offsetof(type, member));             \
        })
#endif

static inline i16 max_i16(i16 a, i16 b)
{
        return a > b ? a : b;
}

static inline i32 max_i32(i32 a, i32 b)
{
        return a > b ? a : b;
}

static inline i64 max_i64(i64 a, i64 b)
{
        return a > b ? a : b;
}

static inline f32 max_f32(f32 a, f32 b)
{
        return a > b ? a : b;
}

static inline f64 max_f64(f64 a, f64 b)
{
        return a > b ? a : b;
}

static inline u16 max_u16(u16 a, u16 b)
{
        return a > b ? a : b;
}

static inline u32 max_u32(u32 a, u32 b)
{
        return a > b ? a : b;
}

static inline u64 max_u64(u64 a, u64 b)
{
        return a > b ? a : b;
}

static inline usize max_usize(usize a, usize b)
{
        return a > b ? a : b;
}

static inline isize max_isize(isize a, isize b)
{
        return a > b ? a : b;
}

static inline i16 min_i16(i16 a, i16 b)
{
        return a < b ? a : b;
}

static inline i32 min_i32(i32 a, i32 b)
{
        return a < b ? a : b;
}

static inline i64 min_i64(i64 a, i64 b)
{
        return a < b ? a : b;
}

static inline f32 min_f32(f32 a, f32 b)
{
        return a < b ? a : b;
}

static inline f64 min_f64(f64 a, f64 b)
{
        return a < b ? a : b;
}

static inline u16 min_u16(u16 a, u16 b)
{
        return a < b ? a : b;
}

static inline u32 min_u32(u32 a, u32 b)
{
        return a < b ? a : b;
}

static inline u64 min_u64(u64 a, u64 b)
{
        return a < b ? a : b;
}

static inline usize min_usize(usize a, usize b)
{
        return a < b ? a : b;
}

static inline isize min_isize(isize a, isize b)
{
        return a < b ? a : b;
}

#define min(a, b)                                                              \
        _Generic((a) + (b),                                                    \
                i16: min_i16,                                                  \
                i32: min_i32,                                                  \
                i64: min_i64,                                                  \
                u16: min_u16,                                                  \
                u32: min_u32,                                                  \
                u64: min_u64,                                                  \
                f32: min_f32,                                                  \
                f64: min_f64,                                                  \
                isize: min_isize,                                              \
                usize: min_usize)(a, b)

#define max(a, b)                                                              \
        _Generic((a) + (b),                                                    \
                i16: max_i16,                                                  \
                i32: max_i32,                                                  \
                i64: max_i64,                                                  \
                u16: max_u16,                                                  \
                u32: max_u32,                                                  \
                u64: max_u64,                                                  \
                f32: max_f32,                                                  \
                f64: max_f64,                                                  \
                isize: max_isize,                                              \
                usize: max_usize)(a, b)

#endif
