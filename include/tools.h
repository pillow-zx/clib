#ifndef __CLIB_TOOLS_H__
#define __CLIB_TOOLS_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BITS_U8(n) ((uint8_t)(1) << (n))

#define BITS_U32(n) ((uint32_t)(1) << (n))

#define BITS_U64(n) ((uint64_t)(1) << (n))

#define MAP(func, ...) func(__VA_ARGS__)

#define ARRLEN(arr) (sizeof(arr) / sizeof((arr)[0]))

#define container_of(ptr, type, member)                                        \
        ({                                                                     \
                const typeof(((type *)0)->member) *__mptr = (ptr);             \
                (type *)((char *)__mptr - offsetof(type, member));             \
        })

static __always_inline void clear_buffer()
{
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
                ;
}

#endif
