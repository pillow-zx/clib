#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <tools.h>
#include "test.h"

static int sum(int a, int b, int c)
{
        return a + b + c;
}

static void test_tools_bits_macros(void)
{
        ASSERT(BITS_U8(0) == 0x01);
        ASSERT(BITS_U8(1) == 0x02);
        ASSERT(BITS_U8(7) == 0x80);

        ASSERT(BITS_U32(0) == 0x00000001U);
        ASSERT(BITS_U32(16) == 0x00010000U);
        ASSERT(BITS_U32(31) == 0x80000000U);

        ASSERT(BITS_U64(0) == 0x0000000000000001ULL);
        ASSERT(BITS_U64(32) == 0x0000000100000000ULL);
        ASSERT(BITS_U64(63) == 0x8000000000000000ULL);
}

static void test_tools_arrlen_macro(void)
{
        int arr[] = {1, 2, 3, 4, 5};
        ASSERT(ARRLEN(arr) == 5);

        char str[] = "hello";
        ASSERT(ARRLEN(str) == 6);
}

static void test_tools_container_of_macro(void)
{
        struct my_struct {
                int a;
                char b;
                long member;
                double d;
        };

        struct my_struct obj = {.a = 1, .b = 'x', .member = 42, .d = 3.14};
        long *ptr = &obj.member;

        struct my_struct *container =
                container_of(ptr, struct my_struct, member);
        ASSERT(container == &obj);
        ASSERT(container->a == 1);
        ASSERT(container->b == 'x');
        ASSERT(container->member == 42);

        int *first_ptr = &obj.a;
        struct my_struct *container2 =
                container_of(first_ptr, struct my_struct, a);
        ASSERT(container2 == &obj);
}

static void test_tools_map_macro(void)
{
        ASSERT(MAP(sum, 1, 2, 3) == 6);
}

int main(void)
{
        RUN_TEST(test_tools_bits_macros);
        RUN_TEST(test_tools_arrlen_macro);
        RUN_TEST(test_tools_container_of_macro);
        RUN_TEST(test_tools_map_macro);

        printf("All tools tests passed!\n");
        return 0;
}
