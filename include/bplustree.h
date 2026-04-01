#ifndef __CLIB_BPLUSTREE_H__
#define __CLIB_BPLUSTREE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BPLUS_ORDER 64

#define BPLUS_MAX_KEYS (BPLUS_ORDER - 1)

#define BPLUS_MIN_KEYS ((BPLUS_ORDER - 1) / 2)

typedef int bplus_key_t;
typedef void *bplus_value_t;

typedef enum { NODE_INTERNAL, NODE_LEAF } node_type_t;

typedef struct bplus_node {
        node_type_t type;
        int key_count;
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
        int height;
        size_t count;
} bplus_tree_t;

typedef struct {
        bplus_key_t *keys;
        bplus_value_t *values;
        size_t count;
        size_t capacity;
} bplus_result_t;

static inline bool node_is_full(const bplus_node_t *node)
{
        return node->key_count >= BPLUS_MAX_KEYS;
}

static inline bool node_is_underflow(const bplus_node_t *node)
{
        return node->key_count < BPLUS_MIN_KEYS;
}

bplus_value_t bplus_search(const bplus_tree_t *tree, bplus_key_t key);

bplus_node_t *bplus_find_leaf(const bplus_tree_t *tree, bplus_key_t key);

bplus_result_t bplus_range_query(const bplus_tree_t *tree, bplus_key_t start,
                                 bplus_key_t end);

void bplus_result_free(bplus_result_t *result);

bool bplus_insert(bplus_tree_t *tree, bplus_key_t key, bplus_value_t value);

bool bplus_delete(bplus_tree_t *tree, bplus_key_t key);

bplus_tree_t *bplus_create(void);

void bplus_destroy(bplus_tree_t *tree);

size_t bplus_count(const bplus_tree_t *tree);

int bplus_height(const bplus_tree_t *tree);
#endif
