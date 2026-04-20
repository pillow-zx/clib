#include <autofree.h>
#include "test.h"

static void test_autofree_accepts_typed_pointer(void)
{
        autofree char *buf = cmalloc(16);
        REQUIRE(buf != nullptr);

        buf[0] = 'o';
        buf[1] = 'k';
        buf[2] = '\0';
        ASSERT_STREQ("ok", buf);
}

int main(void)
{
        RUN_TEST(test_autofree_accepts_typed_pointer);
        TEST_SUMMARY();
        return TEST_EXIT_CODE();
}
