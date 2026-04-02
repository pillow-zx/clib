#ifndef __CLIB_RINGBUF_H__
#define __CLIB_RINGBUF_H__

#include <types.h>
#include <compiler.h>

struct ringbuf {
        char *buf;
        usize size;
        usize head;
        usize tail;
};

#define RINGBUF_INIT(ptr, sz) {.buf = (ptr), .size = (sz), .head = 0, .tail = 0}

#define RINGBUF_DEFINE(name, buffer, size)                                     \
        struct ringbuf name = RINGBUF_INIT(buffer, size)

#define RINGBUF_SIZE(rb) ((rb)->size - 1)

static __always_inline void ringbuf_init(struct ringbuf *rb, void *buf,
                                         usize size)
{
        rb->buf = (char *)buf;
        rb->size = size;
        rb->head = 0;
        rb->tail = 0;
}

static __always_inline bool ringbuf_empty(const struct ringbuf *rb)
{
        return rb->head == rb->tail;
}

static __always_inline bool ringbuf_full(const struct ringbuf *rb)
{
        return ((rb->head + 1) % rb->size) == rb->tail;
}

static __always_inline usize ringbuf_used(const struct ringbuf *rb)
{
        if (rb->head >= rb->tail)
                return rb->head - rb->tail;
        return rb->size - rb->tail + rb->head;
}

static __always_inline usize ringbuf_free(const struct ringbuf *rb)
{
        return rb->size - ringbuf_used(rb) - 1;
}

static __always_inline usize ringbuf_write(struct ringbuf *rb, const void *data,
                                           usize len)
{
        usize free_space = ringbuf_free(rb);
        usize write_len = len > free_space ? free_space : len;
        const char *src = (const char *)data;

        for (usize i = 0; i < write_len; i++) {
                rb->buf[rb->head] = src[i];
                rb->head = (rb->head + 1) % rb->size;
        }

        return write_len;
}

static __always_inline usize ringbuf_read(struct ringbuf *rb, void *data,
                                          usize len)
{
        usize used = ringbuf_used(rb);
        usize read_len = len > used ? used : len;
        char *dst = (char *)data;

        for (usize i = 0; i < read_len; i++) {
                dst[i] = rb->buf[rb->tail];
                rb->tail = (rb->tail + 1) % rb->size;
        }

        return read_len;
}

static __always_inline i32 ringbuf_write_byte(struct ringbuf *rb, char byte)
{
        if (ringbuf_full(rb))
                return -1;

        rb->buf[rb->head] = byte;
        rb->head = (rb->head + 1) % rb->size;
        return 0;
}

static __always_inline i32 ringbuf_read_byte(struct ringbuf *rb)
{
        if (ringbuf_empty(rb))
                return -1;

        char byte = rb->buf[rb->tail];
        rb->tail = (rb->tail + 1) % rb->size;
        return (i32)byte;
}

static __always_inline i32 ringbuf_peek_byte(const struct ringbuf *rb)
{
        if (ringbuf_empty(rb))
                return -1;

        return (i32)rb->buf[rb->tail];
}

static __always_inline void ringbuf_reset(struct ringbuf *rb)
{
        rb->head = 0;
        rb->tail = 0;
}

static __always_inline usize ringbuf_peek(const struct ringbuf *rb, void *data,
                                          usize len)
{
        usize used = ringbuf_used(rb);
        usize peek_len = len > used ? used : len;
        char *dst = (char *)data;
        usize tail = rb->tail;

        for (usize i = 0; i < peek_len; i++) {
                dst[i] = rb->buf[tail];
                tail = (tail + 1) % rb->size;
        }

        return peek_len;
}

static __always_inline usize ringbuf_skip(struct ringbuf *rb, usize len)
{
        usize used = ringbuf_used(rb);
        usize skip_len = len > used ? used : len;

        rb->tail = (rb->tail + skip_len) % rb->size;
        return skip_len;
}

#endif
