#include <bplustree.h>
#include <assert.h>
#include <stdio.h>

int main(void)
{
        bplus_tree_t *tree = bplus_create();
        assert(tree != NULL);
        assert(bplus_count(tree) == 0);
        assert(bplus_height(tree) == 0);

        assert(!bplus_delete(tree, 1));

        assert(bplus_search(tree, 1) == NULL);
        assert(bplus_find_leaf(tree, 1) == NULL);

        int values[100];
        for (int i = 0; i < 100; i++) {
                values[i] = i * 10;
        }

        for (int i = 99; i >= 0; i--) {
                assert(bplus_insert(tree, i, &values[i]));
        }
        assert(bplus_count(tree) == 100);

        for (int i = 0; i < 100; i++) {
                int *v = (int *)bplus_search(tree, i);
                assert(v && *v == values[i]);
        }

        int new_value = 9999;
        assert(bplus_insert(tree, 50, &new_value));
        assert(bplus_count(tree) == 100);
        int *v = (int *)bplus_search(tree, 50);
        assert(v && *v == 9999);

        assert(bplus_delete(tree, 50));
        assert(bplus_count(tree) == 99);
        assert(bplus_search(tree, 50) == NULL);

        assert(!bplus_delete(tree, 50));
        assert(!bplus_delete(tree, -1));
        assert(!bplus_delete(tree, 1000));

        assert(bplus_delete(tree, 0));
        assert(bplus_delete(tree, 99));
        assert(bplus_count(tree) == 97);

        for (int i = 1; i < 99; i++) {
                if (i != 50) {
                        assert(bplus_delete(tree, i));
                }
        }
        assert(bplus_count(tree) == 0);

        bplus_destroy(tree);

        tree = bplus_create();
        for (int i = 0; i < 10000; i++) {
                assert(bplus_insert(tree, i, (void *)(long)i));
        }
        assert(bplus_count(tree) == 10000);
        assert(bplus_height(tree) > 1);

        for (int i = 0; i < 10000; i++) {
                assert(bplus_search(tree, i) == (void *)(long)i);
        }

        bplus_result_t result = bplus_range_query(tree, 5000, 5010);
        assert(result.count == 10);
        for (int i = 0; i < 10; i++) {
                assert(result.keys[i] == 5000 + i);
        }
        bplus_result_free(&result);

        result = bplus_range_query(tree, 10000, 20000);
        assert(result.count == 0);
        bplus_result_free(&result);

        result = bplus_range_query(tree, 5555, 5556);
        assert(result.count == 1);
        assert(result.keys[0] == 5555);
        bplus_result_free(&result);

        bplus_destroy(tree);

        assert(bplus_search(NULL, 0) == NULL);
        assert(bplus_find_leaf(NULL, 0) == NULL);
        assert(!bplus_insert(NULL, 0, NULL));
        assert(!bplus_delete(NULL, 0));
        assert(bplus_count(NULL) == 0);
        assert(bplus_height(NULL) == 0);
        bplus_destroy(NULL);

        result = bplus_range_query(NULL, 0, 10);
        assert(result.count == 0);

        printf("All B+ tree extended tests passed!\n");
        return 0;
}
