#include <debug.h>
#include <stdio.h>
#include <string.h>
#include "test.h"

static void test_debug_log_levels(void)
{
        ASSERT(ERROR == 4);
        ASSERT(DEBUG == 3);
        ASSERT(INFO == 2);
        ASSERT(TRACE == 1);
        ASSERT(ALL == 0);
}

static void test_debug_ansi_colors(void)
{
        ASSERT(strcmp(ANSI_FG_RED, "\33[1;31m") == 0);
        ASSERT(strcmp(ANSI_FG_GREEN, "\33[1;32m") == 0);
        ASSERT(strcmp(ANSI_FG_YELLOW, "\33[1;33m") == 0);
        ASSERT(strcmp(ANSI_FG_BLUE, "\33[1;34m") == 0);
        ASSERT(strcmp(ANSI_NONE, "\33[0m") == 0);
}

static void test_debug_log_color_macro(void)
{
        ASSERT(strcmp(LOG_COLOR(ERROR), ANSI_FG_RED) == 0);
        ASSERT(strcmp(LOG_COLOR(INFO), ANSI_FG_GREEN) == 0);
        ASSERT(strcmp(LOG_COLOR(DEBUG), ANSI_FG_YELLOW) == 0);
        ASSERT(strcmp(LOG_COLOR(TRACE), ANSI_FG_BLUE) == 0);
        ASSERT(strcmp(LOG_COLOR(999), ANSI_NONE) == 0);
}

static void test_debug_log_level_str_macro(void)
{
        ASSERT(strcmp(LOG_LEVEL_STR(ERROR), "ERROR") == 0);
        ASSERT(strcmp(LOG_LEVEL_STR(INFO), "INFO") == 0);
        ASSERT(strcmp(LOG_LEVEL_STR(DEBUG), "DEBUG") == 0);
        ASSERT(strcmp(LOG_LEVEL_STR(TRACE), "TRACE") == 0);
        ASSERT(strcmp(LOG_LEVEL_STR(999), "UNK") == 0);
}

static void test_debug_static_assert_macro(void)
{
        static_assert(1 == 1, "static_assert should work for true conditions");
        static_assert(sizeof(int) >= 4, "int should be at least 4 bytes");
}

static void test_debug_print_macros(void)
{
        print(ERROR, "TEST", "print without newline");
        printf("\n");
        println(TRACE, "TEST", "println with newline");
        print(INFO, "TEST", "value: %d", 42);
        printf("\n");
        println(DEBUG, "TEST", "mixed: %s=%d", "answer", 42);
}

int main(void)
{
        RUN_TEST(test_debug_log_levels);
        RUN_TEST(test_debug_ansi_colors);
        RUN_TEST(test_debug_log_color_macro);
        RUN_TEST(test_debug_log_level_str_macro);
        RUN_TEST(test_debug_static_assert_macro);
        RUN_TEST(test_debug_print_macros);

        printf("All debug tests passed!\n");
        return 0;
}
