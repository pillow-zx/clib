#include <port.h>
#include <bplustree.h>
#include <compiler.h>

static void insert_to_parent(bplus_tree_t *tree, bplus_node_t *left,
                             bplus_key_t key, bplus_node_t *right);
static void borrow_from_left(bplus_node_t *node, bplus_node_t *parent,
                             usize idx, bplus_node_t *left_sibling);
static void borrow_from_right(bplus_node_t *node, bplus_node_t *parent,
                              usize idx, bplus_node_t *right_sibling);
static void merge_into_left(bplus_node_t *left, bplus_node_t *right,
                            bplus_node_t *parent, usize parent_key_idx);

static bplus_node_t *node_create(node_type_t type)
{
        bplus_node_t *node = (bplus_node_t *)ccalloc(1, sizeof(bplus_node_t));
        if (unlikely(!node))
                return nullptr;
        node->type = type;
        node->key_count = 0;
        return node;
}

static void node_destroy(bplus_node_t *node)
{
        if (unlikely(!node))
                return;
        if (node->type == NODE_INTERNAL) {
                for (usize i = 0; i <= node->key_count; i++)
                        node_destroy(node->children[i]);
        }
        cfree(node);
}

static usize binary_search(const bplus_key_t *keys, usize count,
                           bplus_key_t key)
{
    usize left = 0, right = count;
    while (left < right) {
        usize mid = left + (right - left) / 2;
        if (keys[mid] < key)
            left = mid + 1;
        else
            right = mid;
    }
    return left;
}

static bplus_value_t leaf_search(const bplus_node_t *leaf, bplus_key_t key)
{
        usize idx = binary_search(leaf->keys, leaf->key_count, key);
        if (idx < leaf->key_count && leaf->keys[idx] == key)
                return leaf->leaf.values[idx];

        return nullptr;
}

bplus_value_t bplus_search(const bplus_tree_t *tree, bplus_key_t key)
{
        if (unlikely(!tree || !tree->root))
                return nullptr;

        bplus_node_t *node = tree->root;
        while (node->type == NODE_INTERNAL) {
                usize idx = binary_search(node->keys, node->key_count, key);
                if (idx < node->key_count && node->keys[idx] <= key)
                        idx++;

                node = node->children[idx];
        }

        return leaf_search(node, key);
}

bplus_node_t *bplus_find_leaf(const bplus_tree_t *tree, bplus_key_t key)
{
        if (unlikely(!tree || !tree->root))
                return nullptr;

        bplus_node_t *node = tree->root;
        while (node->type == NODE_INTERNAL) {
                usize idx = binary_search(node->keys, node->key_count, key);
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

        if (unlikely(!tree || !tree->first_leaf))
                return result;

        usize capacity = 64;
        result.keys = (bplus_key_t *)cmalloc(capacity * sizeof(bplus_key_t));
        result.values =
                (bplus_value_t *)cmalloc(capacity * sizeof(bplus_value_t));
        if (unlikely(!result.keys || !result.values)) {
                cfree(result.keys);
                cfree(result.values);
                return (bplus_result_t){0};
        }
        result.capacity = capacity;

        bplus_node_t *leaf = bplus_find_leaf(tree, start);

        while (leaf) {
                for (usize i = 0; i < leaf->key_count; i++) {
                        if (leaf->keys[i] >= end)
                                return result;

                        if (leaf->keys[i] >= start) {
                                if (result.count >= result.capacity) {
                                        result.capacity *= 2;
                                        result.keys = (bplus_key_t *)crealloc(
                                                result.keys,
                                                result.capacity *
                                                        sizeof(bplus_key_t));
                                        result.values = (bplus_value_t *)crealloc(
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
                cfree(result->keys);
                cfree(result->values);
                result->count = 0;
                result->capacity = 0;
        }
}

static void node_insert_key(bplus_node_t *node, usize idx, bplus_key_t key,
                            bplus_value_t value, bplus_node_t *child)
{
        if (node->type == NODE_LEAF) {
                cmemmove(&node->keys[idx + 1], &node->keys[idx],
                         (node->key_count - idx) * sizeof(bplus_key_t));
                cmemmove(&node->leaf.values[idx + 1], &node->leaf.values[idx],
                         (node->key_count - idx) * sizeof(bplus_value_t));
                node->leaf.values[idx] = value;
        } else {
                cmemmove(&node->keys[idx + 1], &node->keys[idx],
                         (node->key_count - idx) * sizeof(bplus_key_t));
                cmemmove(&node->children[idx + 2], &node->children[idx + 1],
                         (node->key_count - idx) * sizeof(bplus_node_t *));
                node->children[idx + 1] = child;
        }
        node->keys[idx] = key;
        node->key_count++;
}

static bplus_key_t leaf_split(bplus_node_t *left, bplus_node_t *right)
{
        usize mid = left->key_count / 2;

        right->type = NODE_LEAF;
        right->key_count = left->key_count - mid;

        cmemcpy(right->keys, &left->keys[mid],
                right->key_count * sizeof(bplus_key_t));
        cmemcpy(right->leaf.values, &left->leaf.values[mid],
                right->key_count * sizeof(bplus_value_t));

        left->key_count = mid;

        right->leaf.next = left->leaf.next;
        left->leaf.next = right;

        return right->keys[0];
}

static bplus_key_t internal_split(bplus_node_t *left, bplus_node_t *right)
{
        usize mid = left->key_count / 2;
        bplus_key_t split_key = left->keys[mid];

        right->type = NODE_INTERNAL;
        right->key_count = left->key_count - mid - 1;

        cmemcpy(right->keys, &left->keys[mid + 1],
                right->key_count * sizeof(bplus_key_t));
        cmemcpy(right->children, &left->children[mid + 1],
                (right->key_count + 1) * sizeof(bplus_node_t *));

        left->key_count = mid;

        return split_key;
}

static void node_split(bplus_tree_t *tree, bplus_node_t *node)
{
        bplus_node_t *right = node_create(node->type);
        bplus_key_t split_key;

        if (unlikely(!right))
                return;

        if (node->type == NODE_LEAF)
                split_key = leaf_split(node, right);
        else if (node->type == NODE_INTERNAL)
                split_key = internal_split(node, right);
        else
                unreachable();

        insert_to_parent(tree, node, split_key, right);
}

static void insert_non_full(bplus_tree_t *tree, bplus_node_t *node,
                            bplus_key_t key, bplus_value_t value)
{
        usize idx = binary_search(node->keys, node->key_count, key);

        if (node->type == NODE_LEAF) {
                if (idx < node->key_count && node->keys[idx] == key) {
                        node->leaf.values[idx] = value;
                        return;
                }
                node_insert_key(node, idx, key, value, nullptr);
                tree->count++;
        } else if (node->type == NODE_INTERNAL) {
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
        } else {
                unreachable();
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
        usize path_len = 0;

        bplus_node_t *node = tree->root;
        while (node != left && path_len < 32) {
                path[path_len++] = node;
                usize idx = binary_search(node->keys, node->key_count, key);
                if (idx < node->key_count && key >= node->keys[idx])
                        idx++;

                node = node->children[idx];
        }

        if (unlikely(path_len == 0))
                return;

        bplus_node_t *parent = path[path_len - 1];

        usize idx = 0;
        while (idx <= parent->key_count && parent->children[idx] != left) {
                idx++;
        }

        if (node_is_full(parent)) {
                cmemmove(&parent->keys[idx + 1], &parent->keys[idx],
                         (parent->key_count - idx) * sizeof(bplus_key_t));
                cmemmove(&parent->children[idx + 2], &parent->children[idx + 1],
                         (parent->key_count - idx) * sizeof(bplus_node_t *));
                parent->keys[idx] = key;
                parent->children[idx + 1] = right;
                parent->key_count++;

                node_split(tree, parent);
        } else {
                cmemmove(&parent->keys[idx + 1], &parent->keys[idx],
                         (parent->key_count - idx) * sizeof(bplus_key_t));
                cmemmove(&parent->children[idx + 2], &parent->children[idx + 1],
                         (parent->key_count - idx) * sizeof(bplus_node_t *));
                parent->keys[idx] = key;
                parent->children[idx + 1] = right;
                parent->key_count++;
        }
}

bool bplus_insert(bplus_tree_t *tree, bplus_key_t key, bplus_value_t value)
{
        if (unlikely(!tree))
                return false;

        if (!tree->root) {
                tree->root = node_create(NODE_LEAF);
                if (unlikely(!tree->root))
                        return false;
                tree->first_leaf = tree->root;
                tree->height = 1;
        }

        if (node_is_full(tree->root)) {
                bplus_node_t *new_root = node_create(NODE_INTERNAL);
                if (unlikely(!new_root))
                        return false;
                new_root->children[0] = tree->root;
                tree->root = new_root;
                tree->height++;
                node_split(tree, new_root->children[0]);
        }

        insert_non_full(tree, tree->root, key, value);
        return true;
}

static void leaf_remove(bplus_node_t *leaf, usize idx)
{
        cmemmove(&leaf->keys[idx], &leaf->keys[idx + 1],
                 (leaf->key_count - idx - 1) * sizeof(bplus_key_t));
        cmemmove(&leaf->leaf.values[idx], &leaf->leaf.values[idx + 1],
                 (leaf->key_count - idx - 1) * sizeof(bplus_value_t));
        leaf->key_count--;
}

static bplus_node_t *find_parent(bplus_node_t *root, bplus_node_t *target)
{
        if (root == target || root->type == NODE_LEAF)
                return nullptr;

        for (usize i = 0; i <= root->key_count; i++) {
                if (root->children[i] == target)
                        return root;
                bplus_node_t *found = find_parent(root->children[i], target);
                if (found)
                        return found;
        }
        return nullptr;
}

static usize find_child_index(bplus_node_t *parent, bplus_node_t *child)
{
        for (usize i = 0; i <= parent->key_count; i++) {
                if (parent->children[i] == child)
                        return i;
        }
        return 0;
}

static void rebalance_node(bplus_tree_t *tree, bplus_node_t *node)
{
        if (node == tree->root)
                return;

        bplus_node_t *parent = find_parent(tree->root, node);
        if (!parent)
                return;

        usize idx = find_child_index(parent, node);

        if (!node_is_underflow(node))
                return;

        bplus_node_t *left_sibling =
                (idx > 0) ? parent->children[idx - 1] : nullptr;
        bplus_node_t *right_sibling =
                (idx < parent->key_count) ? parent->children[idx + 1] : nullptr;

        if (left_sibling && left_sibling->key_count > BPLUS_MIN_KEYS) {
                borrow_from_left(node, parent, idx, left_sibling);
                return;
        }
        if (right_sibling && right_sibling->key_count > BPLUS_MIN_KEYS) {
                borrow_from_right(node, parent, idx, right_sibling);
                return;
        }

        if (left_sibling) {
                merge_into_left(left_sibling, node, parent, idx - 1);
                rebalance_node(tree, parent);
        } else if (right_sibling) {
                merge_into_left(node, right_sibling, parent, idx);
                rebalance_node(tree, parent);
        }
}

static void rebalance_after_delete(bplus_tree_t *tree, bplus_node_t *leaf)
{
        rebalance_node(tree, leaf);

        while (tree->root->type == NODE_INTERNAL &&
               tree->root->key_count == 0) {
                bplus_node_t *old_root = tree->root;
                tree->root = tree->root->children[0];
                cfree(old_root);
                tree->height--;
        }

        if (tree->count == 0 && tree->root) {
                cfree(tree->root);
                tree->root = nullptr;
                tree->first_leaf = nullptr;
                tree->height = 0;
        }
}

static void borrow_from_left(bplus_node_t *node, bplus_node_t *parent,
                             usize idx, bplus_node_t *left_sibling)
{
        cmemmove(&node->keys[1], &node->keys[0],
                 node->key_count * sizeof(bplus_key_t));

        if (node->type == NODE_LEAF) {
                cmemmove(&node->leaf.values[1], &node->leaf.values[0],
                         node->key_count * sizeof(bplus_value_t));
                node->keys[0] = left_sibling->keys[left_sibling->key_count - 1];
                node->leaf.values[0] =
                        left_sibling->leaf.values[left_sibling->key_count - 1];
                parent->keys[idx - 1] = node->keys[0];
        } else {
                cmemmove(&node->children[1], &node->children[0],
                         (node->key_count + 1) * sizeof(bplus_node_t *));
                node->keys[0] = parent->keys[idx - 1];
                node->children[0] =
                        left_sibling->children[left_sibling->key_count];
                parent->keys[idx - 1] =
                        left_sibling->keys[left_sibling->key_count - 1];
        }

        node->key_count++;
        left_sibling->key_count--;
}

static void borrow_from_right(bplus_node_t *node, bplus_node_t *parent,
                              usize idx, bplus_node_t *right_sibling)
{
        if (node->type == NODE_LEAF) {
                node->keys[node->key_count] = right_sibling->keys[0];
                node->leaf.values[node->key_count] =
                        right_sibling->leaf.values[0];
                parent->keys[idx] = right_sibling->keys[1];
        } else {
                node->keys[node->key_count] = parent->keys[idx];
                node->children[node->key_count + 1] =
                        right_sibling->children[0];
                parent->keys[idx] = right_sibling->keys[0];
        }

        cmemmove(&right_sibling->keys[0], &right_sibling->keys[1],
                 (right_sibling->key_count - 1) * sizeof(bplus_key_t));

        if (right_sibling->type == NODE_LEAF) {
                cmemmove(&right_sibling->leaf.values[0],
                         &right_sibling->leaf.values[1],
                         (right_sibling->key_count - 1) *
                                 sizeof(bplus_value_t));
        } else {
                cmemmove(&right_sibling->children[0],
                         &right_sibling->children[1],
                         right_sibling->key_count * sizeof(bplus_node_t *));
        }

        node->key_count++;
        right_sibling->key_count--;
}

static void merge_into_left(bplus_node_t *left, bplus_node_t *right,
                            bplus_node_t *parent, usize parent_key_idx)
{
        if (left->type == NODE_LEAF) {
                cmemcpy(&left->keys[left->key_count], right->keys,
                        right->key_count * sizeof(bplus_key_t));
                cmemcpy(&left->leaf.values[left->key_count], right->leaf.values,
                        right->key_count * sizeof(bplus_value_t));
                left->key_count += right->key_count;
                left->leaf.next = right->leaf.next;
        } else {
                left->keys[left->key_count] = parent->keys[parent_key_idx];
                cmemcpy(&left->keys[left->key_count + 1], right->keys,
                        right->key_count * sizeof(bplus_key_t));
                cmemcpy(&left->children[left->key_count + 1], right->children,
                        (right->key_count + 1) * sizeof(bplus_node_t *));
                left->key_count += right->key_count + 1;
        }

        cmemmove(&parent->keys[parent_key_idx],
                 &parent->keys[parent_key_idx + 1],
                 (parent->key_count - parent_key_idx - 1) *
                         sizeof(bplus_key_t));
        cmemmove(&parent->children[parent_key_idx + 1],
                 &parent->children[parent_key_idx + 2],
                 (parent->key_count - parent_key_idx - 1) *
                         sizeof(bplus_node_t *));
        parent->key_count--;

        cfree(right);
}

bool bplus_delete(bplus_tree_t *tree, bplus_key_t key)
{
        if (unlikely(!tree || !tree->root))
                return false;

        bplus_node_t *leaf = bplus_find_leaf(tree, key);
        if (unlikely(!leaf))
                return false;

        usize idx = binary_search(leaf->keys, leaf->key_count, key);
        if (idx >= leaf->key_count || leaf->keys[idx] != key)
                return false;

        leaf_remove(leaf, idx);
        tree->count--;

        rebalance_after_delete(tree, leaf);

        return true;
}

bplus_tree_t *bplus_create(void)
{
        bplus_tree_t *tree = (bplus_tree_t *)ccalloc(1, sizeof(bplus_tree_t));
        if (unlikely(!tree))
                return nullptr;

        tree->root = nullptr;
        tree->first_leaf = nullptr;
        tree->height = 0;
        tree->count = 0;

        return tree;
}

void bplus_destroy(bplus_tree_t *tree)
{
        if (unlikely(!tree))
                return;
        node_destroy(tree->root);
        cfree(tree);
}

usize bplus_count(const bplus_tree_t *tree)
{
        return tree ? tree->count : 0;
}

int bplus_height(const bplus_tree_t *tree)
{
        return tree ? tree->height : 0;
}
