#include "bplustree.h"
#include <assert.h>

int main(void)
{
        bplus_tree_t *tree = bplus_create();
        assert(tree != NULL);

        int values[5] = {100, 200, 300, 400, 500};
        for (int i = 0; i < 5; i++) {
                assert(bplus_insert(tree, i + 1, &values[i]));
        }

        bplus_result_t result = bplus_range_query(tree, 2, 5);
        assert(result.count == 3);
        assert(result.keys[0] == 2);
        assert(result.keys[1] == 3);
        assert(result.keys[2] == 4);

        int *v0 = (int *)result.values[0];
        int *v2 = (int *)result.values[2];
        assert(v0 && *v0 == 200);
        assert(v2 && *v2 == 400);

        bplus_result_free(&result);
        bplus_destroy(tree);
        return 0;
}
