#include <stdlib.h>
#include <string.h>
#include <bplustree.h>
#include <compiler.h>

static void insert_to_parent(bplus_tree_t *tree, bplus_node_t *left,
                             bplus_key_t key, bplus_node_t *right);

static bplus_node_t *node_create(node_type_t type)
{
        bplus_node_t *node = (bplus_node_t *)calloc(1, sizeof(bplus_node_t));
        if (!node)
                return NULL;
        node->type = type;
        node->key_count = 0;
        return node;
}

static void node_destroy(bplus_node_t *node)
{
        if (!node)
                return;
        if (node->type == NODE_INTERNAL) {
                for (int i = 0; i <= node->key_count; i++)
                        node_destroy(node->children[i]);
        }
        free(node);
}

static int binary_search(bplus_key_t *keys, int count, bplus_key_t key)
{
        int left = 0, right = count - 1;
        while (left <= right) {
                int mid = left + (right - left) / 2;
                if (keys[mid] < key)
                        left = mid + 1;
                else if (keys[mid] > key)
                        right = mid - 1;
                else
                        return mid;
        }
        return left;
}

static bplus_value_t leaf_search(const bplus_node_t *leaf, bplus_key_t key)
{
        int idx = binary_search((int *)leaf->keys, leaf->key_count, key);
        if (idx < leaf->key_count && leaf->keys[idx] == key)
                return leaf->leaf.values[idx];

        return NULL;
}

bplus_value_t bplus_search(const bplus_tree_t *tree, bplus_key_t key)
{
        if (!tree || !tree->root)
                return NULL;

        bplus_node_t *node = tree->root;
        while (node->type == NODE_INTERNAL) {
                int idx = binary_search(node->keys, node->key_count, key);
                if (idx < node->key_count && node->keys[idx] <= key)
                        idx++;

                node = node->children[idx];
        }

        return leaf_search(node, key);
}

bplus_node_t *bplus_find_leaf(const bplus_tree_t *tree, bplus_key_t key)
{
        if (!tree || !tree->root)
                return NULL;

        bplus_node_t *node = tree->root;
        while (node->type == NODE_INTERNAL) {
                int idx = binary_search(node->keys, node->key_count, key);
                if (idx < node->key_count && node->keys[idx] <= key)
                        idx++;

                node = node->children[idx];
        }
        return node;
}

bplus_result_t bplus_range_query(const bplus_tree_t *tree, bplus_key_t start,
                                 bplus_key_t end)
{
        bplus_result_t result = {0};

        if (!tree || !tree->first_leaf)
                return result;

        size_t capacity = 64;
        result.keys = (bplus_key_t *)malloc(capacity * sizeof(bplus_key_t));
        result.values =
                (bplus_value_t *)malloc(capacity * sizeof(bplus_value_t));
        if (!result.keys || !result.values) {
                free(result.keys);
                free(result.values);
                return (bplus_result_t){0};
        }
        result.capacity = capacity;

        bplus_node_t *leaf = bplus_find_leaf(tree, start);

        while (leaf) {
                for (int i = 0; i < leaf->key_count; i++) {
                        if (leaf->keys[i] >= end)
                                return result;

                        if (leaf->keys[i] >= start) {
                                if (result.count >= result.capacity) {
                                        result.capacity *= 2;
                                        result.keys = (bplus_key_t *)realloc(
                                                result.keys,
                                                result.capacity *
                                                        sizeof(bplus_key_t));
                                        result.values = (bplus_value_t *)realloc(
                                                result.values,
                                                result.capacity *
                                                        sizeof(bplus_value_t));
                                }
                                result.keys[result.count] = leaf->keys[i];
                                result.values[result.count] =
                                        leaf->leaf.values[i];
                                result.count++;
                        }
                }
                leaf = leaf->leaf.next;
        }

        return result;
}

void bplus_result_free(bplus_result_t *result)
{
        if (result) {
                free(result->keys);
                free(result->values);
                result->count = 0;
                result->capacity = 0;
        }
}

static void node_insert_key(bplus_node_t *node, int idx, bplus_key_t key,
                            bplus_value_t value, bplus_node_t *child)
{
        if (node->type == NODE_LEAF) {
                memmove(&node->keys[idx + 1], &node->keys[idx],
                        (node->key_count - idx) * sizeof(bplus_key_t));
                memmove(&node->leaf.values[idx + 1], &node->leaf.values[idx],
                        (node->key_count - idx) * sizeof(bplus_value_t));
                node->leaf.values[idx] = value;
        } else {
                memmove(&node->keys[idx + 1], &node->keys[idx],
                        (node->key_count - idx) * sizeof(bplus_key_t));
                memmove(&node->children[idx + 2], &node->children[idx + 1],
                        (node->key_count - idx) * sizeof(bplus_node_t *));
                node->children[idx + 1] = child;
        }
        node->keys[idx] = key;
        node->key_count++;
}

static bplus_key_t leaf_split(bplus_node_t *left, bplus_node_t *right)
{
        int mid = left->key_count / 2;

        right->type = NODE_LEAF;
        right->key_count = left->key_count - mid;

        memcpy(right->keys, &left->keys[mid],
               right->key_count * sizeof(bplus_key_t));
        memcpy(right->leaf.values, &left->leaf.values[mid],
               right->key_count * sizeof(bplus_value_t));

        left->key_count = mid;

        right->leaf.next = left->leaf.next;
        left->leaf.next = right;

        return right->keys[0];
}

static bplus_key_t internal_split(bplus_node_t *left, bplus_node_t *right)
{
        int mid = left->key_count / 2;
        bplus_key_t split_key = left->keys[mid];

        right->type = NODE_INTERNAL;
        right->key_count = left->key_count - mid - 1;

        memcpy(right->keys, &left->keys[mid + 1],
               right->key_count * sizeof(bplus_key_t));
        memcpy(right->children, &left->children[mid + 1],
               (right->key_count + 1) * sizeof(bplus_node_t *));

        left->key_count = mid;

        return split_key;
}

static void node_split(bplus_tree_t *tree, bplus_node_t *node)
{
        bplus_node_t *right = node_create(node->type);
        bplus_key_t split_key;

        if (node->type == NODE_LEAF)
                split_key = leaf_split(node, right);
        else
                split_key = internal_split(node, right);

        insert_to_parent(tree, node, split_key, right);
}

static void insert_non_full(bplus_tree_t *tree, bplus_node_t *node,
                            bplus_key_t key, bplus_value_t value)
{
        int idx = binary_search(node->keys, node->key_count, key);

        if (node->type == NODE_LEAF) {
                if (idx < node->key_count && node->keys[idx] == key) {
                        node->leaf.values[idx] = value;
                        return;
                }
                node_insert_key(node, idx, key, value, NULL);
                tree->count++;
        } else {
                if (idx < node->key_count && key >= node->keys[idx])
                        idx++;

                bplus_node_t *child = node->children[idx];

                if (node_is_full(child)) {
                        node_split(tree, child);

                        if (key >= node->keys[idx])
                                idx++;

                        child = node->children[idx];
                }
                insert_non_full(tree, child, key, value);
        }
}

static void insert_to_parent(bplus_tree_t *tree, bplus_node_t *left,
                             bplus_key_t key, bplus_node_t *right)
{
        if (left == tree->root) {
                bplus_node_t *new_root = node_create(NODE_INTERNAL);
                new_root->keys[0] = key;
                new_root->children[0] = left;
                new_root->children[1] = right;
                new_root->key_count = 1;
                tree->root = new_root;
                tree->height++;
                return;
        }

        bplus_node_t *path[32];
        int path_len = 0;

        bplus_node_t *node = tree->root;
        while (node != left && path_len < 32) {
                path[path_len++] = node;
                int idx = binary_search(node->keys, node->key_count, key);
                if (idx < node->key_count && key >= node->keys[idx])
                        idx++;

                node = node->children[idx];
        }

        if (path_len == 0)
                return;

        bplus_node_t *parent = path[path_len - 1];

        int idx = 0;
        while (idx <= parent->key_count && parent->children[idx] != left) {
                idx++;
        }

        if (node_is_full(parent)) {
                memmove(&parent->keys[idx + 1], &parent->keys[idx],
                        (parent->key_count - idx) * sizeof(bplus_key_t));
                memmove(&parent->children[idx + 2], &parent->children[idx + 1],
                        (parent->key_count - idx) * sizeof(bplus_node_t *));
                parent->keys[idx] = key;
                parent->children[idx + 1] = right;
                parent->key_count++;

                node_split(tree, parent);
        } else {
                memmove(&parent->keys[idx + 1], &parent->keys[idx],
                        (parent->key_count - idx) * sizeof(bplus_key_t));
                memmove(&parent->children[idx + 2], &parent->children[idx + 1],
                        (parent->key_count - idx) * sizeof(bplus_node_t *));
                parent->keys[idx] = key;
                parent->children[idx + 1] = right;
                parent->key_count++;
        }
}

bool bplus_insert(bplus_tree_t *tree, bplus_key_t key, bplus_value_t value)
{
        if (!tree)
                return false;

        if (!tree->root) {
                tree->root = node_create(NODE_LEAF);
                tree->first_leaf = tree->root;
                tree->height = 1;
        }

        if (node_is_full(tree->root)) {
                bplus_node_t *new_root = node_create(NODE_INTERNAL);
                new_root->children[0] = tree->root;
                tree->root = new_root;
                tree->height++;
                node_split(tree, new_root->children[0]);
        }

        insert_non_full(tree, tree->root, key, value);
        return true;
}

static void leaf_remove(bplus_node_t *leaf, int idx)
{
        memmove(&leaf->keys[idx], &leaf->keys[idx + 1],
                (leaf->key_count - idx - 1) * sizeof(bplus_key_t));
        memmove(&leaf->leaf.values[idx], &leaf->leaf.values[idx + 1],
                (leaf->key_count - idx - 1) * sizeof(bplus_value_t));
        leaf->key_count--;
}

__maybe_unused static void borrow_from_left(bplus_node_t *node,
                                            bplus_node_t *parent, int idx,
                                            bplus_node_t *left_sibling)
{
        memmove(&node->keys[1], &node->keys[0],
                node->key_count * sizeof(bplus_key_t));

        if (node->type == NODE_INTERNAL) {
                memmove(&node->children[1], &node->children[0],
                        (node->key_count + 1) * sizeof(bplus_node_t *));
                node->children[0] =
                        left_sibling->children[left_sibling->key_count];
        }

        node->keys[0] = parent->keys[idx - 1];
        parent->keys[idx - 1] = left_sibling->keys[left_sibling->key_count - 1];

        node->key_count++;
        left_sibling->key_count--;
}

__maybe_unused static void borrow_from_right(bplus_node_t *node,
                                             bplus_node_t *parent, int idx,
                                             bplus_node_t *right_sibling)
{
        node->keys[node->key_count] = parent->keys[idx];
        parent->keys[idx] = right_sibling->keys[0];

        if (node->type == NODE_INTERNAL) {
                node->children[node->key_count + 1] =
                        right_sibling->children[0];
                memmove(&right_sibling->children[0],
                        &right_sibling->children[1],
                        right_sibling->key_count * sizeof(bplus_node_t *));
        }

        memmove(&right_sibling->keys[0], &right_sibling->keys[1],
                (right_sibling->key_count - 1) * sizeof(bplus_key_t));

        node->key_count++;
        right_sibling->key_count--;
}

__maybe_unused static void merge_leaves(bplus_node_t *left, bplus_node_t *right)
{
        memcpy(&left->keys[left->key_count], right->keys,
               right->key_count * sizeof(bplus_key_t));
        memcpy(&left->leaf.values[left->key_count], right->leaf.values,
               right->key_count * sizeof(bplus_value_t));

        left->key_count += right->key_count;
        left->leaf.next = right->leaf.next;
}

__maybe_unused static void merge_internals(bplus_node_t *left, bplus_key_t key,
                                           bplus_node_t *right)
{
        left->keys[left->key_count] = key;
        memcpy(&left->keys[left->key_count + 1], right->keys,
               right->key_count * sizeof(bplus_key_t));
        memcpy(&left->children[left->key_count + 1], right->children,
               (right->key_count + 1) * sizeof(bplus_node_t *));

        left->key_count += right->key_count + 1;
}

bool bplus_delete(bplus_tree_t *tree, bplus_key_t key)
{
        if (!tree || !tree->root)
                return false;

        bplus_node_t *leaf = bplus_find_leaf(tree, key);
        if (!leaf)
                return false;

        int idx = binary_search(leaf->keys, leaf->key_count, key);
        if (idx >= leaf->key_count || leaf->keys[idx] != key)
                return false;

        leaf_remove(leaf, idx);
        tree->count--;

        if (tree->root->key_count == 0) {
                if (tree->root->type == NODE_INTERNAL) {
                        bplus_node_t *old_root = tree->root;
                        tree->root = tree->root->children[0];
                        free(old_root);
                        tree->height--;
                } else {
                }
        }

        return true;
}

bplus_tree_t *bplus_create(void)
{
        bplus_tree_t *tree = (bplus_tree_t *)calloc(1, sizeof(bplus_tree_t));
        if (!tree)
                return NULL;

        tree->root = NULL;
        tree->first_leaf = NULL;
        tree->height = 0;
        tree->count = 0;

        return tree;
}

void bplus_destroy(bplus_tree_t *tree)
{
        if (!tree)
                return;
        node_destroy(tree->root);
        free(tree);
}

size_t bplus_count(const bplus_tree_t *tree)
{
        return tree ? tree->count : 0;
}

int bplus_height(const bplus_tree_t *tree)
{
        return tree ? tree->height : 0;
}
