#ifndef __CLIB_DSU_H__
#define __CLIB_DSU_H__

#include <types.h>
#include <compiler.h>

struct dsu {
        i32 *parent;
        i32 *rank;
        i32 count;
};

static __always_inline void dsu_init(struct dsu *dsu, i32 *parent_mem,
                                     i32 *rank_mem, i32 n)
{
        dsu->parent = parent_mem;
        dsu->rank = rank_mem;
        dsu->count = n;

        for (i32 i = 0; i < n; ++i) {
                dsu->parent[i] = i;
                dsu->rank[i] = 0;
        }
}

static __must_check i32 dsu_find(struct dsu *dsu, i32 x)
{
        if (dsu->parent[x] != x) {
                dsu->parent[x] = dsu_find(dsu, dsu->parent[x]);
        }
        return dsu->parent[x];
}

static __always_inline __must_check i32 dsu_find_iterative(struct dsu *dsu,
                                                           i32 x)
{
        i32 root = x;
        while (dsu->parent[root] != root) {
                root = dsu->parent[root];
        }
        while (dsu->parent[x] != root) {
                i32 next = dsu->parent[x];
                dsu->parent[x] = root;
                x = next;
        }
        return root;
}

static __always_inline void dsu_union(struct dsu *dsu, i32 x, i32 y)
{
        i32 rootX = dsu_find(dsu, x);
        i32 rootY = dsu_find(dsu, y);

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

static __always_inline __must_check bool dsu_connected(struct dsu *dsu, i32 x,
                                                       i32 y)
{
        return dsu_find(dsu, x) == dsu_find(dsu, y);
}

static __always_inline __must_check __pure i32 dsu_count(const struct dsu *dsu)
{
        return dsu->count;
}

#endif
