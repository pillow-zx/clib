#include <vec.h>
#include "test.h"

static void test_init_and_push_pop(void)
{
        struct vector vec;
        int out = 0;
        int in[] = {11, 22, 33};

        vec_init(&vec, sizeof(in[0]), 2);
        REQUIRE(vec.buf != NULL);
        ASSERT(vec_capacity(&vec) == 2);
        ASSERT(vec_size(&vec) == 0);
        ASSERT(vec_empty(&vec));

        ASSERT(vec_push(&vec, &in[0]) == 0);
        ASSERT(vec_push(&vec, &in[1]) == 0);
        ASSERT(vec_push(&vec, &in[2]) == 0);
        ASSERT(vec_size(&vec) == 3);
        ASSERT(vec_capacity(&vec) >= 3);

        ASSERT(vec_pop(&vec, &out) == 0);
        ASSERT(out == 33);
        ASSERT(vec_pop(&vec, &out) == 0);
        ASSERT(out == 22);
        ASSERT(vec_pop(&vec, &out) == 0);
        ASSERT(out == 11);
        ASSERT(vec_empty(&vec));
        ASSERT(vec_pop(&vec, &out) == -1);

        cfree(vec.buf);
}

static void test_write_read_bulk(void)
{
        struct vector vec;
        int in[] = {1, 2, 3, 4, 5};
        int out[8] = {0};

        vec_init(&vec, sizeof(in[0]), 2);
        REQUIRE(vec.buf != NULL);

        ASSERT(vec_write(&vec, in, 5) == 5);
        ASSERT(vec_size(&vec) == 5);

        ASSERT(vec_read(&vec, out, 3) == 3);
        ASSERT(out[0] == 3);
        ASSERT(out[1] == 4);
        ASSERT(out[2] == 5);
        ASSERT(vec_size(&vec) == 2);

        ASSERT(vec_read(&vec, out, 8) == 2);
        ASSERT(out[0] == 1);
        ASSERT(out[1] == 2);
        ASSERT(vec_size(&vec) == 0);

        cfree(vec.buf);
}

static void test_decrease_capacity(void)
{
        struct vector vec;
        usize grown_capacity = 0;
        int out = 0;

        vec_init(&vec, sizeof(int), 2);
        REQUIRE(vec.buf != NULL);

        for (int i = 0; i < 32; i++)
                ASSERT(vec_push(&vec, &i) == 0);

        grown_capacity = vec_capacity(&vec);
        REQUIRE(grown_capacity >= 32);

        for (int i = 0; i < 31; i++)
                ASSERT(vec_pop(&vec, &out) == 0);

        ASSERT(vec_size(&vec) == 1);
        ASSERT(vec_capacity(&vec) < grown_capacity);

        cfree(vec.buf);
}

static void test_macro_define(void)
{
        VEC_DEFINE(vec, int);
        int in = 7;
        int out = 0;

        ASSERT(vec_capacity(&vec) == VECSZ);
        ASSERT(vec_push(&vec, &in) == 0);
        ASSERT(vec_pop(&vec, &out) == 0);
        ASSERT(out == 7);
        ASSERT(vec_empty(&vec));

        cfree(vec.buf);
}

int main(void)
{
        RUN_TEST(test_init_and_push_pop);
        RUN_TEST(test_write_read_bulk);
        RUN_TEST(test_decrease_capacity);
        RUN_TEST(test_macro_define);
        TEST_SUMMARY();
        return TEST_EXIT_CODE();
}
