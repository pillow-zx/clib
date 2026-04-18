#include <fifo.h>
#include <port.h>
#include <compiler.h>

static __always_inline __must_check __pure bool fifo_invalid(const struct fifo *q)
{
        return !q || !q->buf || q->elem_size == 0 || q->capacity == 0;
}

static __always_inline __must_check __pure char *fifo_slot(struct fifo *q,
                                                           usize idx)
{
        return q->buf + (idx * q->elem_size);
}

static __always_inline __must_check __pure const char *
fifo_cslot(const struct fifo *q, usize idx)
{
        return q->buf + (idx * q->elem_size);
}

i32 fifo_push(struct fifo *q, const void *elem)
{
        if (unlikely(fifo_invalid(q) || fifo_full(q)))
                return -1;

        cmemcpy(fifo_slot(q, q->head), elem, q->elem_size);
        q->head = (q->head + 1) % q->capacity;
        q->count++;

        return 0;
}

i32 fifo_pop(struct fifo *q, void *out)
{
        if (unlikely(fifo_invalid(q) || fifo_empty(q)))
                return -1;

        cmemcpy(out, fifo_slot(q, q->tail), q->elem_size);
        q->tail = (q->tail + 1) % q->capacity;
        q->count--;

        return 0;
}

i32 fifo_peek(const struct fifo *q, void *out)
{
        if (unlikely(fifo_invalid(q) || fifo_empty(q)))
                return -1;

        cmemcpy(out, fifo_cslot(q, q->tail), q->elem_size);

        return 0;
}

usize fifo_write(struct fifo *q, const void *data, usize count)
{
        const char *src = (const char *)data;

        if (unlikely(fifo_invalid(q) || count == 0))
                return 0;

        usize free_count = q->capacity - q->count;
        usize write_count = count > free_count ? free_count : count;

        for (usize i = 0; i < write_count; i++) {
                cmemcpy(fifo_slot(q, q->head), src + (i * q->elem_size),
                        q->elem_size);
                q->head = (q->head + 1) % q->capacity;
        }

        q->count += write_count;
        return write_count;
}

usize fifo_read(struct fifo *q, void *data, usize count)
{
        char *dst = (char *)data;

        if (unlikely(fifo_invalid(q) || count == 0))
                return 0;

        usize read_count = count > q->count ? q->count : count;

        for (usize i = 0; i < read_count; i++) {
                cmemcpy(dst + (i * q->elem_size), fifo_slot(q, q->tail),
                        q->elem_size);
                q->tail = (q->tail + 1) % q->capacity;
        }

        q->count -= read_count;
        return read_count;
}
