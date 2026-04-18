#include <bplustree.h>
#include <assert.h>
#include "test.h"

static void test_bplustree_extended_empty_tree_and_invalid_ops(void)
{
        bplus_tree_t *tree = bplus_create();
        ASSERT(tree != nullptr);
        ASSERT(bplus_count(tree) == 0);
        ASSERT(bplus_height(tree) == 0);

        ASSERT(!bplus_delete(tree, 1));

        ASSERT(bplus_search(tree, 1) == nullptr);
        ASSERT(bplus_find_leaf(tree, 1) == nullptr);

        bplus_destroy(tree);
}

static void test_bplustree_extended_insert_search_update_delete(void)
{
        bplus_tree_t *tree = bplus_create();
        ASSERT(tree != nullptr);

        int values[100];
        for (int i = 0; i < 100; i++) {
                values[i] = i * 10;
        }

        for (int i = 99; i >= 0; i--) {
                ASSERT(bplus_insert(tree, i, &values[i]));
        }
        ASSERT(bplus_count(tree) == 100);

        for (int i = 0; i < 100; i++) {
                int *v = (int *)bplus_search(tree, i);
                ASSERT(v && *v == values[i]);
        }

        int new_value = 9999;
        ASSERT(bplus_insert(tree, 50, &new_value));
        ASSERT(bplus_count(tree) == 100);
        int *v = (int *)bplus_search(tree, 50);
        ASSERT(v && *v == 9999);

        ASSERT(bplus_delete(tree, 50));
        ASSERT(bplus_count(tree) == 99);
        ASSERT(bplus_search(tree, 50) == nullptr);

        ASSERT(!bplus_delete(tree, 50));
        ASSERT(!bplus_delete(tree, -1));
        ASSERT(!bplus_delete(tree, 1000));

        ASSERT(bplus_delete(tree, 0));
        ASSERT(bplus_delete(tree, 99));
        ASSERT(bplus_count(tree) == 97);

        for (int i = 1; i < 99; i++) {
                if (i != 50) {
                        ASSERT(bplus_delete(tree, i));
                }
        }
        ASSERT(bplus_count(tree) == 0);

        bplus_destroy(tree);
}

static void test_bplustree_extended_large_dataset_range_queries(void)
{
        bplus_tree_t *tree = bplus_create();
        ASSERT(tree != nullptr);

        for (int i = 0; i < 10000; i++) {
                ASSERT(bplus_insert(tree, i, (void *)(long)i));
        }
        ASSERT(bplus_count(tree) == 10000);
        ASSERT(bplus_height(tree) > 1);

        for (int i = 0; i < 10000; i++) {
                ASSERT(bplus_search(tree, i) == (void *)(long)i);
        }

        bplus_result_t result = bplus_range_query(tree, 5000, 5010);
        ASSERT(result.count == 10);
        for (int i = 0; i < 10; i++) {
                ASSERT(result.keys[i] == 5000 + i);
        }
        bplus_result_free(&result);

        result = bplus_range_query(tree, 10000, 20000);
        ASSERT(result.count == 0);
        bplus_result_free(&result);

        result = bplus_range_query(tree, 5555, 5556);
        ASSERT(result.count == 1);
        ASSERT(result.keys[0] == 5555);
        bplus_result_free(&result);

        bplus_destroy(tree);
}

static void test_bplustree_extended_null_tree_api_guards(void)
{
        bplus_result_t result;

        ASSERT(bplus_search(nullptr, 0) == nullptr);
        ASSERT(bplus_find_leaf(nullptr, 0) == nullptr);
        ASSERT(!bplus_insert(nullptr, 0, nullptr));
        ASSERT(!bplus_delete(nullptr, 0));
        ASSERT(bplus_count(nullptr) == 0);
        ASSERT(bplus_height(nullptr) == 0);
        bplus_destroy(nullptr);

        result = bplus_range_query(nullptr, 0, 10);
        ASSERT(result.count == 0);
}

int main(void)
{
        RUN_TEST(test_bplustree_extended_empty_tree_and_invalid_ops);
        RUN_TEST(test_bplustree_extended_insert_search_update_delete);
        RUN_TEST(test_bplustree_extended_large_dataset_range_queries);
        RUN_TEST(test_bplustree_extended_null_tree_api_guards);
        TEST_SUMMARY();
        return TEST_EXIT_CODE();
}
