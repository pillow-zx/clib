#ifndef __CLIB_RBTREE_H__
#define __CLIB_RBTREE_H__

#include <types.h>
#include <compiler.h>

#ifndef container_of
#define container_of(ptr, type, member)                                        \
        ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

struct rb_node {
        struct rb_node *left;
        struct rb_node *right;
        struct rb_node *parent;
        u32 color : 1;
};

struct rb_root {
        struct rb_node *node;
};

#define RB_ROOT                                                                \
        (struct rb_root)                                                       \
        {                                                                      \
                .node = NULL                                                   \
        }

#define DEFINE_RBROOT(name) struct rb_root name = RB_ROOT

#define RB_EMPTY_ROOT(root) ((root)->node == NULL)

static __always_inline void rb_node_init(struct rb_node *node)
{
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->color = 0;
}

static __always_inline __must_check struct rb_node *
rb_parent(const struct rb_node *node)
{
        return node->parent;
}

static __always_inline __must_check __const int
rb_is_red(const struct rb_node *node)
{
        return node ? node->color : 0;
}

static __always_inline void rb_set_red(struct rb_node *node)

{
        if (node)
                node->color = 1;
}

static __always_inline void rb_set_black(struct rb_node *node)
{
        if (node)
                node->color = 0;
}

static __always_inline void rb_rotate_left(struct rb_node *node,
                                           struct rb_root *root)
{
        struct rb_node *right = node->right;

        node->right = right->left;
        if (right->left)
                right->left->parent = node;

        right->parent = node->parent;
        if (!node->parent)
                root->node = right;
        else if (node == node->parent->left)
                node->parent->left = right;
        else
                node->parent->right = right;

        right->left = node;
        node->parent = right;
}

static __always_inline void rb_rotate_right(struct rb_node *node,
                                            struct rb_root *root)
{
        struct rb_node *left = node->left;

        node->left = left->right;
        if (left->right)
                left->right->parent = node;

        left->parent = node->parent;
        if (!node->parent)
                root->node = left;
        else if (node == node->parent->right)
                node->parent->right = left;
        else
                node->parent->left = left;

        left->right = node;
        node->parent = left;
}

static __always_inline void rb_link_node(struct rb_node *node,
                                         struct rb_node *parent,
                                         struct rb_node **link)
{
        node->left = NULL;
        node->right = NULL;
        node->parent = parent;
        node->color = 1;
        *link = node;
}

static __always_inline __must_check __pure struct rb_node *
rb_first(const struct rb_root *root)
{
        struct rb_node *node = root->node;
        if (!node)
                return NULL;

        while (node->left)
                node = node->left;

        return node;
}

static __always_inline __must_check __pure struct rb_node *
rb_last(const struct rb_root *root)
{
        struct rb_node *node = root->node;
        if (!node)
                return NULL;

        while (node->right)
                node = node->right;
        return node;
}

static __always_inline __must_check __pure struct rb_node *
rb_next(const struct rb_node *node)
{
        if (node->right) {
                node = node->right;
                while (node->left)
                        node = node->left;

                return (struct rb_node *)node;
        }

        struct rb_node *parent = node->parent;
        while (parent && node == parent->right) {
                node = parent;
                parent = parent->parent;
        }
        return parent;
}

static __always_inline __must_check __pure struct rb_node *
rb_prev(const struct rb_node *node)
{
        if (node->left) {
                node = node->left;
                while (node->right)
                        node = node->right;
                return (struct rb_node *)node;
        }

        struct rb_node *parent = node->parent;
        while (parent && node == parent->left) {
                node = parent;
                parent = parent->parent;
        }
        return parent;
}

#define rbtree_for_each(pos, root)                                             \
        for ((pos) = rb_first(root); (pos) != NULL; (pos) = rb_next(pos))

#define rbtree_for_each_safe(pos, n, root)                                     \
        for ((pos) = rb_first(root), (n) = (pos) ? rb_next(pos) : NULL;        \
             (pos) != NULL; (pos) = (n), (n) = (n) ? rb_next(n) : NULL)

#define rbtree_for_each_reverse(pos, root)                                     \
        for ((pos) = rb_last(root); (pos) != NULL; (pos) = rb_prev(pos))

#define rbtree_for_each_reverse_safe(pos, n, root)                             \
        for ((pos) = rb_last(root), (n) = (pos) ? rb_prev(pos) : NULL;         \
             (pos) != NULL; (pos) = (n), (n) = (n) ? rb_prev(n) : NULL)

#define rbtree_entry(ptr, type, member) container_of(ptr, type, member)

#define rbtree_for_each_entry(pos, root, member)                               \
        for ((pos) = (root)->node ? rbtree_entry (rb_first(root),              \
                                                  typeof(*(pos)), member)      \
                                  : NULL;                                      \
             (pos) != NULL;                                                    \
             (pos) = rb_next(&(pos)->member)                                   \
                             ? rbtree_entry (rb_next(&(pos)->member),          \
                                             typeof(*(pos)), member)           \
                             : NULL)

#define rbtree_for_each_entry_safe(pos, n, root, member)                       \
        for ((pos) = (root)->node ? rbtree_entry (rb_first(root),              \
                                                  typeof(*(pos)), member)      \
                                  : NULL,                                      \
            (n) = (pos) && rb_next(&(pos)->member)                             \
                          ? rbtree_entry (rb_next(&(pos)->member),             \
                                          typeof(*(pos)), member)              \
                          : NULL;                                              \
             (pos) != NULL; (pos) = (n),                                       \
            (n) = (n) && rb_next(&(n)->member)                                 \
                                    ? rbtree_entry (rb_next(&(n)->member),     \
                                                    typeof(*(pos)), member)    \
                                    : NULL)

#define rbtree_for_each_entry_reverse(pos, root, member)                       \
        for ((pos) = (root)->node ? rbtree_entry (rb_last(root),               \
                                                  typeof(*(pos)), member)      \
                                  : NULL;                                      \
             (pos) != NULL;                                                    \
             (pos) = rb_prev(&(pos)->member)                                   \
                             ? rbtree_entry (rb_prev(&(pos)->member),          \
                                             typeof(*(pos)), member)           \
                             : NULL)

#define rbtree_for_each_entry_reverse_safe(pos, n, root, member)               \
        for ((pos) = (root)->node ? rbtree_entry (rb_last(root),               \
                                                  typeof(*(pos)), member)      \
                                  : NULL,                                      \
            (n) = (pos) && rb_prev(&(pos)->member)                             \
                          ? rbtree_entry (rb_prev(&(pos)->member),             \
                                          typeof(*(pos)), member)              \
                          : NULL;                                              \
             (pos) != NULL; (pos) = (n),                                       \
            (n) = (n) && rb_prev(&(n)->member)                                 \
                                    ? rbtree_entry (rb_prev(&(n)->member),     \
                                                    typeof(*(pos)), member)    \
                                    : NULL)

void rb_insert_color(struct rb_node *node, struct rb_root *root);

void rb_erase_color(struct rb_node *parent, struct rb_root *root);

void rb_erase(struct rb_node *node, struct rb_root *root);

#endif
