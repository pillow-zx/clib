#include <assert.h>
#include <stdint.h>
#include <tools.h>
#include "test.h"

static void test_tools_likely_unlikely_macros(void)
{
        ASSERT(likely(1) == 1);
        ASSERT(likely(0) == 0);
        ASSERT(unlikely(1) == 1);
        ASSERT(unlikely(0) == 0);
}

static void test_tools_concat_macros(void)
{
#define CAT_NAME my_var
#define my_var 123
        ASSERT(CONCAT(my_, var) == 123);

#define CAT3_PREFIX pre_
#define CAT3_MID fix_
#define pre_fix_suffix 456
        ASSERT(CONCAT3(CAT3_PREFIX, CAT3_MID, suffix) == 456);
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

static void test_tools_typecheck_macro(void)
{
        i32 i = 1;
        usize u = 2;

        ASSERT(typecheck(i32, i));
        ASSERT(typecheck(usize, u));
}

static void test_tools_min_max_macros(void)
{
        ASSERT(MIN((i16)-3, (i16)7) == (i16)-3);
        ASSERT(MAX((i16)-3, (i16)7) == (i16)7);

        ASSERT(MIN((i32)-10, (i32)5) == (i32)-10);
        ASSERT(MAX((i32)-10, (i32)5) == (i32)5);

        ASSERT(MIN((i64)-100, (i64)-99) == (i64)-100);
        ASSERT(MAX((i64)-100, (i64)-99) == (i64)-99);

        ASSERT(MIN((u16)9, (u16)3) == (u16)3);
        ASSERT(MAX((u16)9, (u16)3) == (u16)9);

        ASSERT(MIN((u32)100, (u32)101) == (u32)100);
        ASSERT(MAX((u32)100, (u32)101) == (u32)101);

        ASSERT(MIN((u64)1000, (u64)999) == (u64)999);
        ASSERT(MAX((u64)1000, (u64)999) == (u64)1000);

        ASSERT(MIN((isize)-8, (isize)-2) == (isize)-8);
        ASSERT(MAX((isize)-8, (isize)-2) == (isize)-2);

        ASSERT(MIN((usize)8, (usize)2) == (usize)2);
        ASSERT(MAX((usize)8, (usize)2) == (usize)8);

        ASSERT(MIN((f32)1.5f, (f32)1.25f) == (f32)1.25f);
        ASSERT(MAX((f32)1.5f, (f32)1.25f) == (f32)1.5f);

        ASSERT(MIN((f64)2.5, (f64)3.5) == (f64)2.5);
        ASSERT(MAX((f64)2.5, (f64)3.5) == (f64)3.5);
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

static void test_tools_constexpr_macro(void)
{
        ASSERT(constexpr(1 + 2) == 3);
        ASSERT(constexpr(10 * 3) == 30);
        ASSERT(constexpr(sizeof(int)) > 0);

        enum { VAL = 100 };
        ASSERT(constexpr(VAL) == 100);

        ASSERT(constexpr(constant_p(42)) == 1);
}

static void test_tools_constexpr_if_macro(void)
{
        ASSERT(constexpr_if(1, 10, 20) == 10);
        ASSERT(constexpr_if(0, 10, 20) == 20);

        ASSERT(constexpr_if(sizeof(int) >= 4, 1, 0) == 1);
        ASSERT(constexpr_if(sizeof(char) == 1, 1, 0) == 1);

        ASSERT(constexpr_if(1, constexpr_if(0, 100, 200), 300) == 200);

        int x = constexpr_if(1, 42, "not_an_int");
        ASSERT(x == 42);
}

static void test_tools_constexpr_val_macro(void)
{
        ASSERT(constexpr_val(42, 0) == 42);
        ASSERT(constexpr_val(sizeof(int), 0) > 0);

        int x = 123;
        ASSERT(constexpr_val(x, 999) == 999);

        enum { TEN = 10 };
        ASSERT(constexpr_val(TEN, 0) == 10);
}

static void test_tools_align_macros(void)
{
        ASSERT(ALIGN_UP((usize)0, (usize)8) == (usize)0);
        ASSERT(ALIGN_UP((usize)1, (usize)8) == (usize)8);
        ASSERT(ALIGN_UP((usize)13, (usize)8) == (usize)16);
        ASSERT(ALIGN_UP((usize)64, (usize)64) == (usize)64);

        ASSERT(ALIGN_DOWN((usize)0, (usize)8) == (usize)0);
        ASSERT(ALIGN_DOWN((usize)15, (usize)8) == (usize)8);
        ASSERT(ALIGN_DOWN((usize)16, (usize)8) == (usize)16);
        ASSERT(ALIGN_DOWN((usize)127, (usize)64) == (usize)64);

        ASSERT(IS_ALIGNED((usize)0, (usize)8));
        ASSERT(IS_ALIGNED((usize)64, (usize)64));
        ASSERT(!IS_ALIGNED((usize)14, (usize)8));
        ASSERT(!IS_ALIGNED((usize)65, (usize)64));
}

int main(void)
{
        RUN_TEST(test_tools_likely_unlikely_macros);
        RUN_TEST(test_tools_concat_macros);
        RUN_TEST(test_tools_bits_macros);
        RUN_TEST(test_tools_arrlen_macro);
        RUN_TEST(test_tools_typecheck_macro);
        RUN_TEST(test_tools_min_max_macros);
        RUN_TEST(test_tools_container_of_macro);
        RUN_TEST(test_tools_constexpr_macro);
        RUN_TEST(test_tools_constexpr_if_macro);
        RUN_TEST(test_tools_constexpr_val_macro);
        RUN_TEST(test_tools_align_macros);
        TEST_SUMMARY();
        return TEST_EXIT_CODE();
}
