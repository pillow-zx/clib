#ifndef __CLIB_FIFO_H__
#define __CLIB_FIFO_H__

#include <types.h>
#include <compiler.h>

struct fifo {
        char *buf;
        usize elem_size;
        usize capacity;
        usize head;
        usize tail;
        usize count;
};

#define FIFO_INIT(ptr, elem_sz, cap)                                           \
        {.buf = (char *)(ptr),                                                 \
         .elem_size = (elem_sz),                                               \
         .capacity = (cap),                                                    \
         .head = 0,                                                            \
         .tail = 0,                                                            \
         .count = 0}

#define FIFO_DEFINE(name, type, cap)                                           \
        type name##_storage[(cap)];                                            \
        struct fifo name = FIFO_INIT(name##_storage, sizeof(type), (cap))

static __always_inline __nonnull((1, 2)) void
fifo_init(struct fifo *q, void *buf, const usize elem_size, const usize cap)
{
        q->buf = (char *)buf;
        q->elem_size = elem_size;
        q->capacity = cap;
        q->head = 0;
        q->tail = 0;
        q->count = 0;
}

static __always_inline __must_check __pure __nonnull((1)) bool
fifo_empty(const struct fifo *q)
{
        return q->count == 0;
}

static __always_inline __must_check __pure __nonnull((1)) bool
fifo_full(const struct fifo *q)
{
        return q->count == q->capacity;
}

static __always_inline __must_check __pure __nonnull((1)) usize
fifo_size(const struct fifo *q)
{
        return q->count;
}

static __always_inline __must_check __pure __nonnull((1)) usize
fifo_capacity(const struct fifo *q)
{
        return q->capacity;
}

static __always_inline void fifo_reset(struct fifo *q)
{
        q->head = 0;
        q->tail = 0;
        q->count = 0;
}

__must_check __nonnull((1, 2)) i32 fifo_push(struct fifo *q, const void *elem);

__must_check __nonnull((1, 2)) i32 fifo_pop(struct fifo *q, void *out);

__must_check __nonnull((1, 2)) i32 fifo_peek(const struct fifo *q, void *out);

__must_check __nonnull((1, 2)) usize
fifo_write(struct fifo *q, const void *data, const usize count);

__must_check __nonnull((1, 2)) usize
fifo_read(struct fifo *q, void *data, const usize count);

#endif /* __CLIB_FIFO_H__ */
