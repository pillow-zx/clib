#include <dsu.h>
#include <assert.h>
#include "test.h"

#define N 10

static void test_dsu_init_and_find(void)
{
        struct dsu dsu;
        int parent[N], rank[N];

        dsu_init(&dsu, parent, rank, N);
        ASSERT(dsu.count == N);

        for (int i = 0; i < N; i++) {
                ASSERT(dsu_find(&dsu, i) == i);
                ASSERT(dsu_find_iterative(&dsu, i) == i);
        }
}

static void test_dsu_union_and_connectivity(void)
{
        struct dsu dsu;
        int parent[N], rank[N];
        dsu_init(&dsu, parent, rank, N);

        dsu_union(&dsu, 0, 1);
        ASSERT(dsu_connected(&dsu, 0, 1));
        ASSERT(dsu.count == N - 1);

        dsu_union(&dsu, 2, 3);
        ASSERT(dsu_connected(&dsu, 2, 3));
        ASSERT(!dsu_connected(&dsu, 0, 2));
        ASSERT(dsu.count == N - 2);

        dsu_union(&dsu, 1, 3);
        ASSERT(dsu_connected(&dsu, 0, 2));
        ASSERT(dsu_connected(&dsu, 1, 2));
        ASSERT(dsu_connected(&dsu, 0, 3));
        ASSERT(dsu.count == N - 3);

        dsu_union(&dsu, 0, 2);
        ASSERT(dsu.count == N - 3);
}

static void test_dsu_root_and_count(void)
{
        struct dsu dsu;
        int parent[N], rank[N];
        dsu_init(&dsu, parent, rank, N);

        dsu_union(&dsu, 0, 1);
        dsu_union(&dsu, 2, 3);
        dsu_union(&dsu, 1, 3);

        int root = dsu_find(&dsu, 3);
        ASSERT(root == dsu_find(&dsu, 0));
        ASSERT(root == dsu_find(&dsu, 1));
        ASSERT(root == dsu_find(&dsu, 2));

        ASSERT(dsu_count(&dsu) == N - 3);
}

static void test_dsu_chain_union(void)
{
        struct dsu dsu;
        int parent[N], rank[N];

        dsu_init(&dsu, parent, rank, N);
        for (int i = 0; i < N - 1; i++) {
                dsu_union(&dsu, i, i + 1);
        }
        ASSERT(dsu.count == 1);
        for (int i = 0; i < N; i++) {
                ASSERT(dsu_connected(&dsu, 0, i));
        }
}

int main(void)
{
        RUN_TEST(test_dsu_init_and_find);
        RUN_TEST(test_dsu_union_and_connectivity);
        RUN_TEST(test_dsu_root_and_count);
        RUN_TEST(test_dsu_chain_union);
        TEST_SUMMARY();
        return TEST_EXIT_CODE();
}
