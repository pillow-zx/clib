#ifndef __CLIB_DSU_H__
#define __CLIB_DSU_H__

#include <compiler.h>
#include <stdbool.h>
#include <stddef.h>

struct dsu {
        int *parent;
        int *rank;
        int count;
};

static inline void dsu_init(struct dsu *dsu, int *parent_mem, int *rank_mem,
                            int n)
{
        dsu->parent = parent_mem;
        dsu->rank = rank_mem;
        dsu->count = n;

        for (int i = 0; i < n; ++i) {
                dsu->parent[i] = i;
                dsu->rank[i] = 0;
        }
}

static inline int dsu_find(struct dsu *dsu, int x)
{
        if (dsu->parent[x] != x) {
                dsu->parent[x] = dsu_find(dsu, dsu->parent[x]);
        }
        return dsu->parent[x];
}

static inline int dsu_find_iterative(struct dsu *dsu, int x)
{
        int root = x;
        while (dsu->parent[root] != root) {
                root = dsu->parent[root];
        }
        while (dsu->parent[x] != root) {
                int next = dsu->parent[x];
                dsu->parent[x] = root;
                x = next;
        }
        return root;
}

static inline void dsu_union(struct dsu *dsu, int x, int y)
{
        int rootX = dsu_find(dsu, x);
        int rootY = dsu_find(dsu, y);

        if (rootX == rootY)
                return;

        if (dsu->rank[rootX] < dsu->rank[rootY]) {
                dsu->parent[rootX] = rootY;
        } else if (dsu->rank[rootX] > dsu->rank[rootY]) {
                dsu->parent[rootY] = rootX;
        } else {
                dsu->parent[rootY] = rootX;
                ++dsu->rank[rootX];
        }
        --dsu->count;
}

static __always_inline bool dsu_connected(struct dsu *dsu, int x, int y)
{
        return dsu_find(dsu, x) == dsu_find(dsu, y);
}

static __always_inline int dsu_count(const struct dsu *dsu)
{
        return dsu->count;
}

#endif
