#ifndef __CLIB_BPLUSTREE_H__
#define __CLIB_BPLUSTREE_H__

#include <types.h>
#include <compiler.h>

#define BPLUS_ORDER 64

#define BPLUS_MAX_KEYS (BPLUS_ORDER - 1)

#define BPLUS_MIN_KEYS ((BPLUS_ORDER - 1) / 2)

typedef i32 bplus_key_t;
typedef void *bplus_value_t;

typedef enum { NODE_INTERNAL, NODE_LEAF } node_type_t;

typedef struct bplus_node {
        node_type_t type;
        usize key_count;
        bplus_key_t keys[BPLUS_MAX_KEYS];
        union {
                struct bplus_node *children[BPLUS_ORDER];
                struct {
                        bplus_value_t values[BPLUS_MAX_KEYS];
                        struct bplus_node *next;
                } leaf;
        };
} bplus_node_t;

typedef struct {
        bplus_node_t *root;
        bplus_node_t *first_leaf;
        i32 height;
        usize count;
} bplus_tree_t;

typedef struct {
        bplus_key_t *keys;
        bplus_value_t *values;
        usize count;
        usize capacity;
} bplus_result_t;

static __always_inline __must_check __pure bool
node_is_full(const bplus_node_t *node)
{
        return node->key_count >= BPLUS_MAX_KEYS;
}

static __always_inline __must_check __pure bool
node_is_underflow(const bplus_node_t *node)
{
        return node->key_count < BPLUS_MIN_KEYS;
}

__must_check bplus_value_t bplus_search(const bplus_tree_t *tree,
                                        bplus_key_t key);

__must_check bplus_node_t *bplus_find_leaf(const bplus_tree_t *tree,
                                           bplus_key_t key);

__must_check bplus_result_t bplus_range_query(const bplus_tree_t *tree,
                                              bplus_key_t start,
                                              bplus_key_t end);

void bplus_result_free(bplus_result_t *result);

__must_check bool bplus_insert(bplus_tree_t *tree, bplus_key_t key,
                               bplus_value_t value);

__must_check bool bplus_delete(bplus_tree_t *tree, bplus_key_t key);

__must_check __malloc bplus_tree_t *bplus_create(void);

void bplus_destroy(bplus_tree_t *tree);

__must_check __pure usize bplus_count(const bplus_tree_t *tree);

__must_check __pure i32 bplus_height(const bplus_tree_t *tree);

#endif
