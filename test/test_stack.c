#include <stack.h>
#include <stdio.h>
#include <string.h>
#include "test.h"

static void test_init_state(void)
{
        int buffer[4];
        struct stack st;

        stack_init(&st, buffer, sizeof(buffer[0]), 4);
        ASSERT(st.buf == (char *)buffer);
        ASSERT(st.elem_size == sizeof(int));
        ASSERT(stack_capacity(&st) == 4);
        ASSERT(stack_size(&st) == 0);
        ASSERT(stack_empty(&st));
        ASSERT(!stack_full(&st));
}

static void test_push_pop_lifo(void)
{
        int buffer[4];
        struct stack st;
        stack_init(&st, buffer, sizeof(buffer[0]), 4);

        int a = 10, b = 20, c = 30;
        ASSERT(stack_push(&st, &a) == 0);
        ASSERT(stack_push(&st, &b) == 0);
        ASSERT(stack_push(&st, &c) == 0);
        ASSERT(stack_size(&st) == 3);

        int out = 0;
        ASSERT(stack_pop(&st, &out) == 0);
        ASSERT(out == 30);
        ASSERT(stack_pop(&st, &out) == 0);
        ASSERT(out == 20);
        ASSERT(stack_pop(&st, &out) == 0);
        ASSERT(out == 10);
        ASSERT(stack_empty(&st));
}

static void test_full_and_overflow(void)
{
        int buffer[2];
        struct stack st;
        stack_init(&st, buffer, sizeof(buffer[0]), 2);

        int a = 1, b = 2, c = 3;
        ASSERT(stack_push(&st, &a) == 0);
        ASSERT(stack_push(&st, &b) == 0);
        ASSERT(stack_full(&st));
        ASSERT(stack_push(&st, &c) == -1);
}

static void test_empty_pop_and_peek(void)
{
        int buffer[2];
        struct stack st;
        stack_init(&st, buffer, sizeof(buffer[0]), 2);

        int out = 0;
        ASSERT(stack_pop(&st, &out) == -1);
        ASSERT(stack_peek(&st, &out) == -1);
}

static void test_peek_no_remove(void)
{
        int buffer[3];
        struct stack st;
        stack_init(&st, buffer, sizeof(buffer[0]), 3);

        int a = 11, b = 22, out = 0;
        ASSERT(stack_push(&st, &a) == 0);
        ASSERT(stack_push(&st, &b) == 0);

        ASSERT(stack_peek(&st, &out) == 0);
        ASSERT(out == 22);
        ASSERT(stack_size(&st) == 2);
}

static void test_macro_define(void)
{
        STACK_DEFINE(st, int, 3);

        int value = 7;
        int out = 0;
        ASSERT(stack_empty(&st));
        ASSERT(stack_capacity(&st) == 3);
        ASSERT(stack_push(&st, &value) == 0);
        ASSERT(stack_pop(&st, &out) == 0);
        ASSERT(out == 7);
}

static void test_struct_elements(void)
{
        struct pair {
                int a;
                int b;
        };

        struct pair buffer[2];
        struct stack st;
        stack_init(&st, buffer, sizeof(buffer[0]), 2);

        struct pair in = {.a = 3, .b = 9};
        struct pair out = {0};

        ASSERT(stack_push(&st, &in) == 0);
        ASSERT(stack_peek(&st, &out) == 0);
        ASSERT(memcmp(&in, &out, sizeof(in)) == 0);
        out.a = 0;
        out.b = 0;
        ASSERT(stack_pop(&st, &out) == 0);
        ASSERT(memcmp(&in, &out, sizeof(in)) == 0);
}

static void test_reset(void)
{
        int buffer[3];
        struct stack st;
        stack_init(&st, buffer, sizeof(buffer[0]), 3);

        int v = 1;
        ASSERT(stack_push(&st, &v) == 0);
        ASSERT(stack_size(&st) == 1);

        stack_reset(&st);
        ASSERT(stack_empty(&st));
        ASSERT(stack_size(&st) == 0);
}

int main(void)
{
        RUN_TEST(test_init_state);
        RUN_TEST(test_push_pop_lifo);
        RUN_TEST(test_full_and_overflow);
        RUN_TEST(test_empty_pop_and_peek);
        RUN_TEST(test_peek_no_remove);
        RUN_TEST(test_macro_define);
        RUN_TEST(test_struct_elements);
        RUN_TEST(test_reset);

        printf("All stack tests passed!\n");
        return 0;
}
