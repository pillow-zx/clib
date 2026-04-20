#include <sort.h>
#include "test.h"

static void assert_arr_eq(const int *expected, const int *actual, usize n)
{
        for (usize i = 0; i < n; i++)
                ASSERT_EQ_INT(expected[i], actual[i]);
}

static void test_insert_sort_asc_basic(void)
{
        int arr[] = {5, 1, 4, 2, 8, 5};
        int expected[] = {1, 2, 4, 5, 5, 8};

        insert_sort_asc(arr);
        assert_arr_eq(expected, arr, ARRLEN(arr));
}

static void test_insert_sort_desc_basic(void)
{
        int arr[] = {5, 1, 4, 2, 8, 5};
        int expected[] = {8, 5, 5, 4, 2, 1};

        insert_sort_desc(arr);
        assert_arr_eq(expected, arr, ARRLEN(arr));
}

static void test_insert_sort_with_flag(void)
{
        int asc_arr[] = {3, -1, 0, 9, 3};
        int desc_arr[] = {3, -1, 0, 9, 3};
        int expected_asc[] = {-1, 0, 3, 3, 9};
        int expected_desc[] = {9, 3, 3, 0, -1};

        insert_sort(asc_arr, true);
        insert_sort(desc_arr, false);

        assert_arr_eq(expected_asc, asc_arr, ARRLEN(asc_arr));
        assert_arr_eq(expected_desc, desc_arr, ARRLEN(desc_arr));
}

static void test_bubble_sort_asc_basic(void)
{
        int arr[] = {10, 7, 2, 7, -3, 0};
        int expected[] = {-3, 0, 2, 7, 7, 10};

        bubble_sort_asc(arr);
        assert_arr_eq(expected, arr, ARRLEN(arr));
}

static void test_bubble_sort_desc_basic(void)
{
        int arr[] = {10, 7, 2, 7, -3, 0};
        int expected[] = {10, 7, 7, 2, 0, -3};

        bubble_sort_desc(arr);
        assert_arr_eq(expected, arr, ARRLEN(arr));
}

static void test_bubble_sort_with_flag(void)
{
        int asc_arr[] = {42, 1, 42, 9, -1};
        int desc_arr[] = {42, 1, 42, 9, -1};
        int expected_asc[] = {-1, 1, 9, 42, 42};
        int expected_desc[] = {42, 42, 9, 1, -1};

        bubble_sort(asc_arr, true);
        bubble_sort(desc_arr, false);

        assert_arr_eq(expected_asc, asc_arr, ARRLEN(asc_arr));
        assert_arr_eq(expected_desc, desc_arr, ARRLEN(desc_arr));
}

static void test_select_sort_asc_basic(void)
{
        int arr[] = {4, 9, 1, 0, -2, 4};
        int expected[] = {-2, 0, 1, 4, 4, 9};

        select_sort_asc(arr);
        assert_arr_eq(expected, arr, ARRLEN(arr));
}

static void test_select_sort_desc_basic(void)
{
        int arr[] = {4, 9, 1, 0, -2, 4};
        int expected[] = {9, 4, 4, 1, 0, -2};

        select_sort_desc(arr);
        assert_arr_eq(expected, arr, ARRLEN(arr));
}

static void test_select_sort_with_flag(void)
{
        int asc_arr[] = {8, 3, 8, -5, 1};
        int desc_arr[] = {8, 3, 8, -5, 1};
        int expected_asc[] = {-5, 1, 3, 8, 8};
        int expected_desc[] = {8, 8, 3, 1, -5};

        select_sort(asc_arr, true);
        select_sort(desc_arr, false);

        assert_arr_eq(expected_asc, asc_arr, ARRLEN(asc_arr));
        assert_arr_eq(expected_desc, desc_arr, ARRLEN(desc_arr));
}

static void test_single_element(void)
{
        int arr1[] = {7};
        int arr2[] = {7};
        int arr3[] = {7};

        insert_sort_asc(arr1);
        bubble_sort_desc(arr2);
        select_sort(arr3, true);

        ASSERT_EQ_INT(7, arr1[0]);
        ASSERT_EQ_INT(7, arr2[0]);
        ASSERT_EQ_INT(7, arr3[0]);
}

int main(void)
{
        RUN_TEST(test_insert_sort_asc_basic);
        RUN_TEST(test_insert_sort_desc_basic);
        RUN_TEST(test_insert_sort_with_flag);
        RUN_TEST(test_bubble_sort_asc_basic);
        RUN_TEST(test_bubble_sort_desc_basic);
        RUN_TEST(test_bubble_sort_with_flag);
        RUN_TEST(test_select_sort_asc_basic);
        RUN_TEST(test_select_sort_desc_basic);
        RUN_TEST(test_select_sort_with_flag);
        RUN_TEST(test_single_element);
        TEST_SUMMARY();
        return TEST_EXIT_CODE();
}
