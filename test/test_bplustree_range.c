#include <assert.h>
#include <bplustree.h>
#include <stdio.h>
#include "test.h"

static void test_bplustree_range_query_results(void)
{
        bplus_tree_t *tree = bplus_create();
        ASSERT(tree != NULL);

        int values[5] = {100, 200, 300, 400, 500};
        for (int i = 0; i < 5; i++) {
                ASSERT(bplus_insert(tree, i + 1, &values[i]));
        }

        bplus_result_t result = bplus_range_query(tree, 2, 5);
        ASSERT(result.count == 3);
        ASSERT(result.keys[0] == 2);
        ASSERT(result.keys[1] == 3);
        ASSERT(result.keys[2] == 4);

        int *v0 = (int *)result.values[0];
        int *v2 = (int *)result.values[2];
        ASSERT(v0 && *v0 == 200);
        ASSERT(v2 && *v2 == 400);

        bplus_result_free(&result);
        bplus_destroy(tree);
}

int main(void)
{
        RUN_TEST(test_bplustree_range_query_results);
        puts("All B+ tree range tests passed!");
        return 0;
}
