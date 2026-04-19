#ifndef __CLIB_VEC_H__
#define __CLIB_VEC_H__

#include <compiler.h>
#include <types.h>
#include <port.h>

struct vector {
        char *buf;
        usize elem_sz;
        usize capacity;
        usize size;
};

#define VECSZ 10

#define VEC_INIT(elem_size)                                                    \
        {.buf = (char *)cmalloc(elem_size * VECSZ),                            \
         .elem_sz = elem_size,                                                 \
         .capacity = VECSZ,                                                    \
         .size = 0}

#define VEC_DEFINE(name, type) struct vector name = VEC_INIT(sizeof(type))

static __always_inline __nonnull((1)) void vec_init(struct vector *vec,
                                                    const usize elem_size,
                                                    const usize cap)
{
        vec->buf = (char *)cmalloc(elem_size * cap);
        vec->elem_sz = elem_size;
        vec->capacity = cap;
        vec->size = 0;
}

static __always_inline __must_check __pure
        __nonnull((1)) bool vec_empty(const struct vector *vec)
{
        return vec->size == 0;
}

static __always_inline __must_check __pure __nonnull((1)) usize
        vec_size(const struct vector *vec)
{
        return vec->size;
}

static __always_inline __must_check __pure __nonnull((1)) usize
        vec_capacity(const struct vector *vec)
{
        return vec->capacity;
}

static __always_inline void vec_reset(struct vector *vec)
{
        vec->size = 0;
}

__must_check __nonnull((1, 2)) i32
vec_push(struct vector *vec, const void *elem);

__must_check __nonnull((1, 2)) i32 vec_pop(struct vector *vec, void *out);

__must_check __nonnull((1, 2)) usize
vec_write(struct vector *vec, const void *elem, const usize sz);

__must_check __nonnull((1, 2)) usize
vec_read(struct vector *vec, void *data, const usize sz);

#endif // !__CLIB_VEC_H__
