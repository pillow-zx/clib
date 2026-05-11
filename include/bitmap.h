#ifndef CLIB_BITMAP_H
#define CLIB_BITMAP_H

#include <types.h>
#include <compiler.h>
#include <tools.h>

#define BITMAP_WORD_BITS ((usize)(sizeof(unsigned long) * 8U))
#define BITMAP_WORDS(nbits)                                                    \
        (ALIGN_UP((usize)(nbits), BITMAP_WORD_BITS) / BITMAP_WORD_BITS)
#define BITMAP_BYTES(nbits) (BITMAP_WORDS(nbits) * sizeof(unsigned long))

struct bitmap {
        unsigned long *words;
        usize nbits;
        usize nwords;
};

#define BITMAP_DECLARE(name, bits)                                             \
        unsigned long name##_storage[BITMAP_WORDS(bits)];                      \
        struct bitmap name = {.words = name##_storage,                         \
                              .nbits = (usize)(bits),                          \
                              .nwords = BITMAP_WORDS(bits)}

static __always_inline __must_check __const usize bitmap_word_index(usize bit)
{
        return bit / BITMAP_WORD_BITS;
}

static __always_inline __must_check __const usize bitmap_word_offset(usize bit)
{
        return bit % BITMAP_WORD_BITS;
}

static __always_inline __must_check __const unsigned long
bitmap_mask_lo(usize bits)
{
        if (bits == 0)
                return 0UL;
        if (bits >= BITMAP_WORD_BITS)
                return ~0UL;
        return (1UL << bits) - 1UL;
}

static __always_inline __must_check __const unsigned long
bitmap_mask_hi(usize bit)
{
        return ~bitmap_mask_lo(bit);
}

static __always_inline __must_check __const unsigned long
bitmap_tail_mask(usize nbits)
{
        usize tail = nbits % BITMAP_WORD_BITS;

        return tail == 0 ? ~0UL : bitmap_mask_lo(tail);
}

static __always_inline __nonnull((1, 2)) void bitmap_init(struct bitmap *map,
                                                          unsigned long *words,
                                                          usize nbits)
{
        map->words = words;
        map->nbits = nbits;
        map->nwords = BITMAP_WORDS(nbits);
}

static __always_inline __nonnull((1)) void bitmap_zero(struct bitmap *map)
{
        for (usize i = 0; i < map->nwords; i++)
                map->words[i] = 0UL;
}

static __always_inline __nonnull((1)) void bitmap_fill(struct bitmap *map)
{
        for (usize i = 0; i < map->nwords; i++)
                map->words[i] = ~0UL;

        if (map->nwords > 0)
                map->words[map->nwords - 1] &= bitmap_tail_mask(map->nbits);
}

static __always_inline __nonnull((1)) void bitmap_set(struct bitmap *map,
                                                      usize bit)
{
        if (unlikely(bit >= map->nbits))
                return;

        map->words[bitmap_word_index(bit)] |= 1UL << bitmap_word_offset(bit);
}

static __always_inline __nonnull((1)) void bitmap_clear(struct bitmap *map,
                                                        usize bit)
{
        if (unlikely(bit >= map->nbits))
                return;

        map->words[bitmap_word_index(bit)] &= ~(1UL << bitmap_word_offset(bit));
}

static __always_inline __must_check
        __nonnull((1)) bool bitmap_test(const struct bitmap *map, usize bit)
{
        if (unlikely(bit >= map->nbits))
                return false;

        return !!(map->words[bitmap_word_index(bit)] &
                  (1UL << bitmap_word_offset(bit)));
}

static __always_inline __nonnull((1)) void bitmap_assign(struct bitmap *map,
                                                         usize bit, bool value)
{
        if (value)
                bitmap_set(map, bit);
        else
                bitmap_clear(map, bit);
}

static __always_inline __nonnull((1)) void bitmap_set_range(struct bitmap *map,
                                                            usize start,
                                                            usize count)
{
        if (count == 0 || unlikely(start >= map->nbits))
                return;

        usize end = start + MIN(count, map->nbits - start);
        usize first = bitmap_word_index(start);
        usize last = bitmap_word_index(end - 1);
        usize start_bit = bitmap_word_offset(start);
        usize end_bit = bitmap_word_offset(end);
        unsigned long last_mask = end_bit == 0 ? ~0UL : bitmap_mask_lo(end_bit);

        if (first == last) {
                map->words[first] |= bitmap_mask_hi(start_bit) & last_mask;
                return;
        }

        map->words[first] |= bitmap_mask_hi(start_bit);

        for (usize i = first + 1; i < last; i++)
                map->words[i] = ~0UL;

        map->words[last] |= last_mask;
}

static __always_inline __nonnull((1)) void
        bitmap_clear_range(struct bitmap *map, usize start, usize count)
{
        if (count == 0 || unlikely(start >= map->nbits))
                return;

        usize end = start + MIN(count, map->nbits - start);
        usize first = bitmap_word_index(start);
        usize last = bitmap_word_index(end - 1);
        usize start_bit = bitmap_word_offset(start);
        usize end_bit = bitmap_word_offset(end);
        unsigned long last_mask = end_bit == 0 ? ~0UL : bitmap_mask_lo(end_bit);

        if (first == last) {
                map->words[first] &= ~(bitmap_mask_hi(start_bit) & last_mask);
                return;
        }

        map->words[first] &= ~bitmap_mask_hi(start_bit);

        for (usize i = first + 1; i < last; i++)
                map->words[i] = 0UL;

        map->words[last] &= ~last_mask;
}

static __always_inline __must_check __nonnull((1)) usize
        bitmap_find_first_set(const struct bitmap *map)
{
        for (usize i = 0; i < map->nwords; i++) {
                unsigned long word = map->words[i];

                if (i + 1 == map->nwords)
                        word &= bitmap_tail_mask(map->nbits);

                if (word != 0UL)
                        return i * BITMAP_WORD_BITS + (usize)ctzl(word);
        }

        return map->nbits;
}

static __always_inline __must_check __nonnull((1)) usize
        bitmap_find_first_zero(const struct bitmap *map)
{
        for (usize i = 0; i < map->nwords; i++) {
                unsigned long word = ~map->words[i];

                if (i + 1 == map->nwords)
                        word &= bitmap_tail_mask(map->nbits);

                if (word != 0UL)
                        return i * BITMAP_WORD_BITS + (usize)ctzl(word);
        }

        return map->nbits;
}

static __always_inline __must_check __nonnull((1)) usize
        bitmap_find_next_set(const struct bitmap *map, usize start)
{
        if (unlikely(start >= map->nbits))
                return map->nbits;

        usize i = bitmap_word_index(start);
        usize offset = bitmap_word_offset(start);
        unsigned long word = map->words[i] & bitmap_mask_hi(offset);

        if (i + 1 == map->nwords)
                word &= bitmap_tail_mask(map->nbits);

        if (word != 0UL)
                return i * BITMAP_WORD_BITS + (usize)ctzl(word);

        for (i += 1; i < map->nwords; i++) {
                word = map->words[i];
                if (i + 1 == map->nwords)
                        word &= bitmap_tail_mask(map->nbits);

                if (word != 0UL)
                        return i * BITMAP_WORD_BITS + (usize)ctzl(word);
        }

        return map->nbits;
}

static __always_inline __must_check __nonnull((1)) usize
        bitmap_find_next_zero(const struct bitmap *map, usize start)
{
        if (unlikely(start >= map->nbits))
                return map->nbits;

        usize i = bitmap_word_index(start);
        usize offset = bitmap_word_offset(start);
        unsigned long word = ~map->words[i] & bitmap_mask_hi(offset);

        if (i + 1 == map->nwords)
                word &= bitmap_tail_mask(map->nbits);

        if (word != 0UL)
                return i * BITMAP_WORD_BITS + (usize)ctzl(word);

        for (i += 1; i < map->nwords; i++) {
                word = ~map->words[i];
                if (i + 1 == map->nwords)
                        word &= bitmap_tail_mask(map->nbits);

                if (word != 0UL)
                        return i * BITMAP_WORD_BITS + (usize)ctzl(word);
        }

        return map->nbits;
}

static __always_inline __must_check __nonnull((1)) usize
        bitmap_weight(const struct bitmap *map)
{
        usize count = 0;

        for (usize i = 0; i < map->nwords; i++) {
                unsigned long word = map->words[i];

                if (i + 1 == map->nwords)
                        word &= bitmap_tail_mask(map->nbits);

                count += (usize)popcountl(word);
        }

        return count;
}

static __always_inline __must_check __nonnull((1)) bool
        bitmap_empty(const struct bitmap *map)
{
        return bitmap_weight(map) == 0;
}

static __always_inline __must_check __nonnull((1)) bool
        bitmap_full(const struct bitmap *map)
{
        return bitmap_weight(map) == map->nbits;
}

#endif
