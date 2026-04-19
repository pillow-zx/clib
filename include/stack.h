#ifndef CLIB_STACK_H
#define CLIB_STACK_H

#include <types.h>
#include <compiler.h>

struct stack {
        char *buf;
        usize elem_size;
        usize capacity;
        usize top;
};

#define STACK_INIT(ptr, elem_sz, cap)                                          \
        {.buf = (char *)(ptr),                                                 \
         .elem_size = (elem_sz),                                               \
         .capacity = (cap),                                                    \
         .top = 0}

#define STACK_DEFINE(name, type, cap)                                          \
        type name##_storage[(cap)];                                            \
        struct stack name = STACK_INIT(name##_storage, sizeof(type), (cap))

static __always_inline void stack_init(struct stack *st, void *buf,
                                       const usize elem_sz, const usize cap)
{
        st->buf = (char *)buf;
        st->elem_size = elem_sz;
        st->capacity = cap;
        st->top = 0;
}

static __always_inline __must_check __pure bool
stack_empty(const struct stack *st)
{
        return st->top == 0;
}

static __always_inline __must_check __pure bool
stack_full(const struct stack *st)
{
        return st->top == st->capacity;
}

static __always_inline __must_check __pure usize
stack_size(const struct stack *st)
{
        return st->top;
}

static __always_inline __must_check __pure usize
stack_capacity(const struct stack *st)
{
        return st->capacity;
}

static __always_inline void stack_reset(struct stack *st)
{
        st->top = 0;
}

static __always_inline __must_check i32 stack_push(struct stack *st,
                                                   const void *elem)
{
        if (stack_full(st))
                return -1;

        char *dst = st->buf + st->top * st->elem_size;
        const char *src = elem;

        for (usize i = 0; i < st->elem_size; i++)
                dst[i] = src[i];

        st->top++;
        return 0;
}

static __always_inline __must_check i32 stack_pop(struct stack *st, void *out)
{
        if (stack_empty(st))
                return -1;

        st->top--;

        const char *src = st->buf + st->top * st->elem_size;
        char *dst = out;

        for (usize i = 0; i < st->elem_size; i++)
                dst[i] = src[i];

        return 0;
}

static __always_inline __must_check i32 stack_peek(const struct stack *st,
                                                   void *out)
{
        if (stack_empty(st))
                return -1;

        const char *src = st->buf + (st->top - 1) * st->elem_size;
        char *dst = out;

        for (usize i = 0; i < st->elem_size; i++)
                dst[i] = src[i];

        return 0;
}

#endif
