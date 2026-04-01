#include <bplustree.h>
#include <assert.h>

int main(void)
{
        bplus_tree_t *tree = bplus_create();
        assert(tree != NULL);

        int values[] = {10, 20, 30};
        assert(bplus_insert(tree, 1, &values[0]));
        assert(bplus_insert(tree, 2, &values[1]));
        assert(bplus_insert(tree, 3, &values[2]));

        int *v1 = (int *)bplus_search(tree, 1);
        int *v2 = (int *)bplus_search(tree, 2);
        int *v3 = (int *)bplus_search(tree, 3);

        assert(v1 && *v1 == 10);
        assert(v2 && *v2 == 20);
        assert(v3 && *v3 == 30);
        assert(bplus_count(tree) == 3);

        bplus_destroy(tree);
        return 0;
}
