#include <types.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "test.h"

static void test_types_unsigned_integers(void)
{
        u8 val8 = 255;
        ASSERT(sizeof(u8) == 1);
        ASSERT(val8 == 255);

        u16 val16 = 65535;
        ASSERT(sizeof(u16) == 2);
        ASSERT(val16 == 65535);

        u32 val32 = 4294967295U;
        ASSERT(sizeof(u32) == 4);
        ASSERT(val32 == 4294967295U);

        u64 val64 = 18446744073709551615ULL;
        ASSERT(sizeof(u64) == 8);
        ASSERT(val64 == 18446744073709551615ULL);
}

static void test_types_signed_integers(void)
{
        i8 ival8 = -128;
        ASSERT(sizeof(i8) == 1);
        ASSERT(ival8 == -128);

        i16 ival16 = -32768;
        ASSERT(sizeof(i16) == 2);
        ASSERT(ival16 == -32768);

        i32 ival32 = -2147483647 - 1;
        ASSERT(sizeof(i32) == 4);
        ASSERT(ival32 == -2147483648);

        i64 ival64 = -9223372036854775807LL;
        ASSERT(sizeof(i64) == 8);
        ASSERT(ival64 == -9223372036854775807LL);
}

static void test_types_floats(void)
{
        f32 fval32 = 3.14159f;
        ASSERT(sizeof(f32) == 4);
        (void)fval32;

        f64 fval64 = 3.14159265358979;
        ASSERT(sizeof(f64) == 8);
        (void)fval64;
}

static void test_types_boolean_and_aliases(void)
{
        bool bval = true;
        ASSERT(sizeof(bool) == 1);
        ASSERT(bval == true);
        bval = false;
        ASSERT(bval == false);

        uchar c = 255;
        ASSERT(sizeof(uchar) == 1);
        ASSERT(c == 255);

        usize us = 0;
        ASSERT(sizeof(usize) == sizeof(size_t));
        (void)us;

        isize is = 0;
        ASSERT(sizeof(isize) == sizeof(intptr_t));
        (void)is;
}

int main(void)
{
        RUN_TEST(test_types_unsigned_integers);
        RUN_TEST(test_types_signed_integers);
        RUN_TEST(test_types_floats);
        RUN_TEST(test_types_boolean_and_aliases);
        TEST_SUMMARY();
        return TEST_EXIT_CODE();
}
