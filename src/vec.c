#include <vec.h>
#include <port.h>

static __always_inline __must_check __pure bool
vec_invalid(const struct vector *vec)
{
        return !vec || !vec->buf || vec->elem_sz == 0 || vec->capacity == 0 ||
               vec->size > vec->capacity;
}

static __always_inline __must_check i32
vec_resize_capacity(struct vector *vec, const usize new_capacity)
{
        char *buffer = (char *)cmalloc(new_capacity * vec->elem_sz);
        if (buffer == nullptr)
                return -1;

        cmemcpy(buffer, vec->buf, vec->size * vec->elem_sz);
        cfree(vec->buf);
        vec->buf = buffer;
        vec->capacity = new_capacity;

        return 0;
}

static __always_inline __must_check i32
vec_ensure_capacity(struct vector *vec, const usize add_count)
{
        usize required = vec->size + add_count;
        usize new_capacity = vec->capacity;

        if (required <= vec->capacity)
                return 0;

        while (new_capacity < required) {
                if (new_capacity > ((usize)-1) / 2)
                        return -1;

                new_capacity *= 2;
                if (new_capacity == 0)
                        return -1;
        }

        return vec_resize_capacity(vec, new_capacity);
}

static __always_inline i32 vec_decrease(struct vector *vec)
{
        if (vec->capacity <= 1 || vec->size > (vec->capacity / 4))
                return 0;

        usize new_capacity = vec->capacity / 2;
        if (new_capacity < 1)
                new_capacity = 1;
        if (new_capacity < vec->size)
                new_capacity = vec->size;

        return vec_resize_capacity(vec, new_capacity);
}

i32 vec_push(struct vector *vec, const void *elem)
{
        if (unlikely(vec_invalid(vec)))
                return -1;

        if (vec_ensure_capacity(vec, 1) < 0)
                return -2;

        cmemcpy(vec->buf + (vec->size * vec->elem_sz), elem, vec->elem_sz);
        vec->size += 1;
        return 0;
}

i32 vec_pop(struct vector *vec, void *out)
{
        if (unlikely(vec_invalid(vec) || vec->size == 0))
                return -1;

        vec->size -= 1;

        cmemcpy(out, vec->buf + (vec->size * vec->elem_sz), vec->elem_sz);
        if (vec_decrease(vec) < 0)
                return -2;

        return 0;
}

usize vec_write(struct vector *vec, const void *elem, const usize sz)
{
        if (unlikely(vec_invalid(vec) || sz == 0))
                return 0;

        if (vec_ensure_capacity(vec, sz) < 0)
                return 0;

        cmemcpy(vec->buf + (vec->size * vec->elem_sz), elem, vec->elem_sz * sz);
        vec->size += sz;
        return sz;
}

usize vec_read(struct vector *vec, void *data, usize sz)
{
        if (unlikely(vec_invalid(vec) || sz == 0))
                return 0;

        usize read_count = (sz > vec->size) ? vec->size : sz;
        usize start = vec->size - read_count;

        cmemcpy(data, vec->buf + (start * vec->elem_sz),
                read_count * vec->elem_sz);
        vec->size -= read_count;
        if (vec_decrease(vec) < 0)
                return 0;

        return read_count;
}
