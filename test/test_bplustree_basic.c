#include <bplustree.h>
#include <assert.h>
#include <stdio.h>
#include "test.h"

static void test_bplustree_basic_create_insert_search_count(void)
{
        bplus_tree_t *tree = bplus_create();
        ASSERT(tree != nullptr);

        int values[] = {10, 20, 30};
        ASSERT(bplus_insert(tree, 1, &values[0]));
        ASSERT(bplus_insert(tree, 2, &values[1]));
        ASSERT(bplus_insert(tree, 3, &values[2]));

        int *v1 = (int *)bplus_search(tree, 1);
        int *v2 = (int *)bplus_search(tree, 2);
        int *v3 = (int *)bplus_search(tree, 3);

        ASSERT(v1 && *v1 == 10);
        ASSERT(v2 && *v2 == 20);
        ASSERT(v3 && *v3 == 30);
        ASSERT(bplus_count(tree) == 3);

        bplus_destroy(tree);
}

int main(void)
{
        RUN_TEST(test_bplustree_basic_create_insert_search_count);
        puts("All B+ tree basic tests passed!");
        return 0;
}
