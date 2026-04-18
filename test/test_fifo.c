#include <fifo.h>
#include <string.h>
#include "test.h"

static void test_init_state(void)
{
        int buffer[4];
        struct fifo q;

        fifo_init(&q, buffer, sizeof(buffer[0]), 4);
        ASSERT(q.buf == (char *)buffer);
        ASSERT(q.elem_size == sizeof(int));
        ASSERT(fifo_capacity(&q) == 4);
        ASSERT(fifo_size(&q) == 0);
        ASSERT(fifo_empty(&q));
        ASSERT(!fifo_full(&q));
}

static void test_push_pop_fifo_order(void)
{
        int buffer[4];
        struct fifo q;
        fifo_init(&q, buffer, sizeof(buffer[0]), 4);

        int a = 10, b = 20, c = 30;
        ASSERT(fifo_push(&q, &a) == 0);
        ASSERT(fifo_push(&q, &b) == 0);
        ASSERT(fifo_push(&q, &c) == 0);
        ASSERT(fifo_size(&q) == 3);

        int out = 0;
        ASSERT(fifo_pop(&q, &out) == 0);
        ASSERT(out == 10);
        ASSERT(fifo_pop(&q, &out) == 0);
        ASSERT(out == 20);
        ASSERT(fifo_pop(&q, &out) == 0);
        ASSERT(out == 30);
        ASSERT(fifo_empty(&q));
}

static void test_full_and_overflow(void)
{
        int buffer[2];
        struct fifo q;
        fifo_init(&q, buffer, sizeof(buffer[0]), 2);

        int a = 1, b = 2, c = 3;
        ASSERT(fifo_push(&q, &a) == 0);
        ASSERT(fifo_push(&q, &b) == 0);
        ASSERT(fifo_full(&q));
        ASSERT(fifo_push(&q, &c) == -1);
}

static void test_empty_pop_and_peek(void)
{
        int buffer[2];
        struct fifo q;
        fifo_init(&q, buffer, sizeof(buffer[0]), 2);

        int out = 0;
        ASSERT(fifo_pop(&q, &out) == -1);
        ASSERT(fifo_peek(&q, &out) == -1);
}

static void test_peek_no_remove(void)
{
        int buffer[3];
        struct fifo q;
        fifo_init(&q, buffer, sizeof(buffer[0]), 3);

        int a = 11, b = 22, out = 0;
        ASSERT(fifo_push(&q, &a) == 0);
        ASSERT(fifo_push(&q, &b) == 0);

        ASSERT(fifo_peek(&q, &out) == 0);
        ASSERT(out == 11);
        ASSERT(fifo_size(&q) == 2);
}

static void test_wrap_around(void)
{
        int buffer[3];
        struct fifo q;
        fifo_init(&q, buffer, sizeof(buffer[0]), 3);

        int a = 1, b = 2, c = 3, d = 4;
        int out = 0;

        ASSERT(fifo_push(&q, &a) == 0);
        ASSERT(fifo_push(&q, &b) == 0);
        ASSERT(fifo_push(&q, &c) == 0);
        ASSERT(fifo_pop(&q, &out) == 0);
        ASSERT(out == 1);
        ASSERT(fifo_push(&q, &d) == 0);

        ASSERT(fifo_pop(&q, &out) == 0);
        ASSERT(out == 2);
        ASSERT(fifo_pop(&q, &out) == 0);
        ASSERT(out == 3);
        ASSERT(fifo_pop(&q, &out) == 0);
        ASSERT(out == 4);
}

static void test_bulk_write_read(void)
{
        int buffer[5];
        struct fifo q;
        fifo_init(&q, buffer, sizeof(buffer[0]), 5);

        int in[] = {1, 2, 3, 4, 5, 6};
        int out[5] = {0};

        ASSERT(fifo_write(&q, in, 6) == 5);
        ASSERT(fifo_full(&q));
        ASSERT(fifo_read(&q, out, 3) == 3);
        ASSERT(out[0] == 1);
        ASSERT(out[1] == 2);
        ASSERT(out[2] == 3);

        ASSERT(fifo_write(&q, &in[5], 1) == 1);
        ASSERT(fifo_read(&q, out, 3) == 3);
        ASSERT(out[0] == 4);
        ASSERT(out[1] == 5);
        ASSERT(out[2] == 6);
        ASSERT(fifo_empty(&q));
}

static void test_macro_define(void)
{
        FIFO_DEFINE(q, int, 3);

        int value = 7;
        int out = 0;
        ASSERT(fifo_empty(&q));
        ASSERT(fifo_capacity(&q) == 3);
        ASSERT(fifo_push(&q, &value) == 0);
        ASSERT(fifo_pop(&q, &out) == 0);
        ASSERT(out == 7);
}

static void test_struct_elements(void)
{
        struct pair {
                int a;
                int b;
        };

        struct pair buffer[2];
        struct fifo q;
        fifo_init(&q, buffer, sizeof(buffer[0]), 2);

        struct pair p1 = {.a = 3, .b = 9};
        struct pair p2 = {.a = 5, .b = 7};
        struct pair out = {0};

        ASSERT(fifo_push(&q, &p1) == 0);
        ASSERT(fifo_push(&q, &p2) == 0);
        ASSERT(fifo_pop(&q, &out) == 0);
        ASSERT(memcmp(&p1, &out, sizeof(p1)) == 0);
        ASSERT(fifo_pop(&q, &out) == 0);
        ASSERT(memcmp(&p2, &out, sizeof(p2)) == 0);
}

static void test_reset(void)
{
        int buffer[3];
        struct fifo q;
        fifo_init(&q, buffer, sizeof(buffer[0]), 3);

        int v = 1;
        ASSERT(fifo_push(&q, &v) == 0);
        ASSERT(fifo_size(&q) == 1);

        fifo_reset(&q);
        ASSERT(fifo_empty(&q));
        ASSERT(fifo_size(&q) == 0);
}

int main(void)
{
        RUN_TEST(test_init_state);
        RUN_TEST(test_push_pop_fifo_order);
        RUN_TEST(test_full_and_overflow);
        RUN_TEST(test_empty_pop_and_peek);
        RUN_TEST(test_peek_no_remove);
        RUN_TEST(test_wrap_around);
        RUN_TEST(test_bulk_write_read);
        RUN_TEST(test_macro_define);
        RUN_TEST(test_struct_elements);
        RUN_TEST(test_reset);
        TEST_SUMMARY();
        return TEST_EXIT_CODE();
}
