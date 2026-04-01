#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

static int g_free_calls;
static void *g_last_freed;

static void test_free(void *ptr)
{
        g_free_calls++;
        g_last_freed = ptr;
        free(ptr);
}

#define free test_free
#include <autofree.h>
#undef free

static void reset_tracker(void)
{
        g_free_calls = 0;
        g_last_freed = NULL;
}

static void test_autofree_scope_cleanup(void)
{
        reset_tracker();

        char *allocated = NULL;
        {
                autofree char *buf = malloc(64);
                ASSERT(buf != NULL);
                allocated = buf;
                buf[0] = 'a';
        }

        ASSERT(g_free_calls == 1);
        ASSERT(g_last_freed == allocated);
}

static void test_autofree_null_pointer_cleanup(void)
{
        reset_tracker();

        {
                autofree char *buf = NULL;
                (void)buf;
        }

        ASSERT(g_free_calls == 0);
        ASSERT(g_last_freed == NULL);
}

static int autofree_allocate_and_return_early(int early)
{
        autofree char *buf = malloc(16);
        ASSERT(buf != NULL);
        if (early)
                return 1;
        return 0;
}

static void test_autofree_early_return_cleanup(void)
{
        reset_tracker();

        int ret = autofree_allocate_and_return_early(1);
        ASSERT(ret == 1);
        ASSERT(g_free_calls == 1);
}

int main(void)
{
        RUN_TEST(test_autofree_scope_cleanup);
        RUN_TEST(test_autofree_null_pointer_cleanup);
        RUN_TEST(test_autofree_early_return_cleanup);
        puts("All autofree tests passed!");
        return 0;
}
