#include <bitmap.h>
#include "test.h"

static void test_bitmap_init_and_declare(void)
{
        BITMAP_DECLARE(map, 130);

        ASSERT(map.nbits == 130);
        ASSERT(map.nwords == BITMAP_WORDS(130));
        ASSERT(map.words != nullptr);
        ASSERT(BITMAP_BYTES(130) == sizeof(unsigned long[BITMAP_WORDS(130)]));
}

static void test_bitmap_zero_set_clear_and_test(void)
{
        unsigned long storage[BITMAP_WORDS(128)];
        struct bitmap map;

        bitmap_init(&map, storage, 128);
        bitmap_zero(&map);

        ASSERT(!bitmap_test(&map, 0));
        ASSERT(!bitmap_test(&map, 63));
        ASSERT(!bitmap_test(&map, 127));

        bitmap_set(&map, 0);
        bitmap_set(&map, 63);
        bitmap_set(&map, 127);

        ASSERT(bitmap_test(&map, 0));
        ASSERT(bitmap_test(&map, 63));
        ASSERT(bitmap_test(&map, 127));

        bitmap_clear(&map, 63);

        ASSERT(bitmap_test(&map, 0));
        ASSERT(!bitmap_test(&map, 63));
        ASSERT(bitmap_test(&map, 127));
}

static void test_bitmap_assign_and_range_ops(void)
{
        unsigned long storage[BITMAP_WORDS(100)];
        struct bitmap map;

        bitmap_init(&map, storage, 100);
        bitmap_zero(&map);

        bitmap_assign(&map, 7, true);
        bitmap_assign(&map, 8, false);
        ASSERT(bitmap_test(&map, 7));
        ASSERT(!bitmap_test(&map, 8));

        bitmap_set_range(&map, 5, 70);
        for (usize i = 0; i < 100; i++) {
                if (i >= 5 && i < 75)
                        ASSERT(bitmap_test(&map, i));
        }

        bitmap_clear_range(&map, 16, 32);
        for (usize i = 0; i < 100; i++) {
                if (i >= 16 && i < 48)
                        ASSERT(!bitmap_test(&map, i));
        }
}

static void test_bitmap_fill_and_tail_mask(void)
{
        unsigned long storage[BITMAP_WORDS(70)];
        struct bitmap map;

        bitmap_init(&map, storage, 70);
        bitmap_fill(&map);

        for (usize i = 0; i < 70; i++)
                ASSERT(bitmap_test(&map, i));

        ASSERT(bitmap_find_first_zero(&map) == 70);

        bitmap_clear(&map, 69);
        ASSERT(!bitmap_test(&map, 69));
}

static void test_bitmap_find_first_helpers(void)
{
        unsigned long storage[BITMAP_WORDS(96)];
        struct bitmap map;

        bitmap_init(&map, storage, 96);
        bitmap_zero(&map);

        ASSERT(bitmap_find_first_set(&map) == 96);
        ASSERT(bitmap_find_first_zero(&map) == 0);

        bitmap_set(&map, 31);
        bitmap_set(&map, 65);
        ASSERT(bitmap_find_first_set(&map) == 31);

        bitmap_fill(&map);
        ASSERT(bitmap_find_first_zero(&map) == 96);

        bitmap_clear(&map, 17);
        ASSERT(bitmap_find_first_zero(&map) == 17);
}

static void test_bitmap_weight_empty_and_full(void)
{
        unsigned long storage[BITMAP_WORDS(70)];
        struct bitmap map;

        bitmap_init(&map, storage, 70);
        bitmap_zero(&map);

        ASSERT(bitmap_weight(&map) == 0);
        ASSERT(bitmap_empty(&map));
        ASSERT(!bitmap_full(&map));

        bitmap_set(&map, 0);
        bitmap_set(&map, 7);
        bitmap_set(&map, 31);
        bitmap_set(&map, 69);

        ASSERT(bitmap_weight(&map) == 4);
        ASSERT(!bitmap_empty(&map));
        ASSERT(!bitmap_full(&map));

        bitmap_fill(&map);
        ASSERT(bitmap_weight(&map) == 70);
        ASSERT(!bitmap_empty(&map));
        ASSERT(bitmap_full(&map));

        bitmap_clear(&map, 13);
        ASSERT(bitmap_weight(&map) == 69);
        ASSERT(!bitmap_full(&map));
}

static void test_bitmap_find_next_helpers(void)
{
        unsigned long storage[BITMAP_WORDS(130)];
        struct bitmap map;

        bitmap_init(&map, storage, 130);
        bitmap_zero(&map);

        ASSERT(bitmap_find_next_set(&map, 0) == 130);
        ASSERT(bitmap_find_next_zero(&map, 0) == 0);
        ASSERT(bitmap_find_next_zero(&map, 129) == 129);
        ASSERT(bitmap_find_next_zero(&map, 130) == 130);
        ASSERT(bitmap_find_next_set(&map, 130) == 130);

        bitmap_set(&map, 5);
        bitmap_set(&map, 64);
        bitmap_set(&map, 129);

        ASSERT(bitmap_find_next_set(&map, 0) == 5);
        ASSERT(bitmap_find_next_set(&map, 5) == 5);
        ASSERT(bitmap_find_next_set(&map, 6) == 64);
        ASSERT(bitmap_find_next_set(&map, 64) == 64);
        ASSERT(bitmap_find_next_set(&map, 65) == 129);
        ASSERT(bitmap_find_next_set(&map, 129) == 129);

        bitmap_fill(&map);
        bitmap_clear(&map, 7);
        bitmap_clear(&map, 65);
        bitmap_clear(&map, 129);

        ASSERT(bitmap_find_next_zero(&map, 0) == 7);
        ASSERT(bitmap_find_next_zero(&map, 7) == 7);
        ASSERT(bitmap_find_next_zero(&map, 8) == 65);
        ASSERT(bitmap_find_next_zero(&map, 65) == 65);
        ASSERT(bitmap_find_next_zero(&map, 66) == 129);
        ASSERT(bitmap_find_next_zero(&map, 129) == 129);
        ASSERT(bitmap_find_next_zero(&map, 130) == 130);
}

int main(void)
{
        RUN_TEST(test_bitmap_init_and_declare);
        RUN_TEST(test_bitmap_zero_set_clear_and_test);
        RUN_TEST(test_bitmap_assign_and_range_ops);
        RUN_TEST(test_bitmap_fill_and_tail_mask);
        RUN_TEST(test_bitmap_find_first_helpers);
        RUN_TEST(test_bitmap_weight_empty_and_full);
        RUN_TEST(test_bitmap_find_next_helpers);
        TEST_SUMMARY();
        return TEST_EXIT_CODE();
}
