#include "dsu.h"
#include <assert.h>
#include <stdio.h>

#define N 10

int main(void)
{
        struct dsu dsu;
        int parent[N], rank[N];

        dsu_init(&dsu, parent, rank, N);
        assert(dsu.count == N);

        for (int i = 0; i < N; i++) {
                assert(dsu_find(&dsu, i) == i);
                assert(dsu_find_iterative(&dsu, i) == i);
        }

        dsu_union(&dsu, 0, 1);
        assert(dsu_connected(&dsu, 0, 1));
        assert(dsu.count == N - 1);

        dsu_union(&dsu, 2, 3);
        assert(dsu_connected(&dsu, 2, 3));
        assert(!dsu_connected(&dsu, 0, 2));
        assert(dsu.count == N - 2);

        dsu_union(&dsu, 1, 3);
        assert(dsu_connected(&dsu, 0, 2));
        assert(dsu_connected(&dsu, 1, 2));
        assert(dsu_connected(&dsu, 0, 3));
        assert(dsu.count == N - 3);

        dsu_union(&dsu, 0, 2);
        assert(dsu.count == N - 3);

        int root = dsu_find(&dsu, 3);
        assert(root == dsu_find(&dsu, 0));
        assert(root == dsu_find(&dsu, 1));
        assert(root == dsu_find(&dsu, 2));

        assert(dsu_count(&dsu) == N - 3);

        dsu_init(&dsu, parent, rank, N);
        for (int i = 0; i < N - 1; i++) {
                dsu_union(&dsu, i, i + 1);
        }
        assert(dsu.count == 1);
        for (int i = 0; i < N; i++) {
                assert(dsu_connected(&dsu, 0, i));
        }

        printf("All DSU tests passed!\n");
        return 0;
}
