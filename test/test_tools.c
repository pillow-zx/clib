#include "tools.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

static int sum(int a, int b, int c)
{
        return a + b + c;
}

int main(void)
{
        assert(BITS_U8(0) == 0x01);
        assert(BITS_U8(1) == 0x02);
        assert(BITS_U8(7) == 0x80);

        assert(BITS_U32(0) == 0x00000001U);
        assert(BITS_U32(16) == 0x00010000U);
        assert(BITS_U32(31) == 0x80000000U);

        assert(BITS_U64(0) == 0x0000000000000001ULL);
        assert(BITS_U64(32) == 0x0000000100000000ULL);
        assert(BITS_U64(63) == 0x8000000000000000ULL);

        int arr[] = {1, 2, 3, 4, 5};
        assert(ARRLEN(arr) == 5);

        char str[] = "hello";
        assert(ARRLEN(str) == 6);

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
        assert(container == &obj);
        assert(container->a == 1);
        assert(container->b == 'x');
        assert(container->member == 42);

        int *first_ptr = &obj.a;
        struct my_struct *container2 =
                container_of(first_ptr, struct my_struct, a);
        assert(container2 == &obj);

        assert(MAP(sum, 1, 2, 3) == 6);

        printf("All tools tests passed!\n");
        return 0;
}
