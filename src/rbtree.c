#include "types.h"
#include <assert.h>
#include <rbtree.h>

void rb_insert_color(struct rb_node *node, struct rb_root *root)
{
        while (rb_is_red(node->parent)) {
                struct rb_node *parent = node->parent;
                struct rb_node *grandparent = parent->parent;

                if (parent == grandparent->left) {
                        struct rb_node *uncle = grandparent->right;

                        if (rb_is_red(uncle)) {
                                rb_set_black(uncle);
                                rb_set_black(parent);
                                rb_set_red(grandparent);
                                node = grandparent;
                        } else {
                                if (node == parent->right) {
                                        node = parent;
                                        rb_rotate_left(node, root);
                                        parent = node->parent;
                                        grandparent = parent->parent;
                                }
                                rb_set_black(parent);
                                rb_set_red(grandparent);
                                rb_rotate_right(grandparent, root);
                        }
                } else {
                        struct rb_node *uncle = grandparent->left;

                        if (rb_is_red(uncle)) {
                                rb_set_black(uncle);
                                rb_set_black(parent);
                                rb_set_red(grandparent);
                                node = grandparent;
                        } else {
                                if (node == parent->left) {
                                        node = parent;
                                        rb_rotate_right(node, root);
                                        parent = node->parent;
                                        grandparent = parent->parent;
                                }
                                rb_set_black(parent);
                                rb_set_red(grandparent);
                                rb_rotate_left(grandparent, root);
                        }
                }
        }
        rb_set_black(root->node);
}

void rb_erase_color(struct rb_node *parent, struct rb_root *root)
{
        struct rb_node *node = NULL;

        while ((!node || !rb_is_red(node)) && node != root->node) {
                /* 此处断言仅为了消除clang警告，非语义问题 */
                assert(parent != NULL);
                if (parent->left == node) {
                        struct rb_node *sibling = parent->right;

                        if (rb_is_red(sibling)) {
                                rb_set_black(sibling);
                                rb_set_red(parent);
                                rb_rotate_left(parent, root);
                                sibling = parent->right;
                        }

                        /* 此处断言仅为了消除clang警告，非语义问题*/
                        assert(sibling != NULL);
                        if ((!sibling->left || !rb_is_red(sibling->left)) &&
                            (!sibling->right || !rb_is_red(sibling->right))) {
                                rb_set_red(sibling);
                                node = parent;
                                parent = node->parent;
                        } else {
                                if (!sibling->right ||
                                    !rb_is_red(sibling->right)) {
                                        rb_set_black(sibling->left);
                                        rb_set_red(sibling);
                                        rb_rotate_right(sibling, root);
                                        sibling = parent->right;
                                }
                                sibling->color = parent->color;
                                rb_set_black(parent);
                                rb_set_black(sibling->right);
                                rb_rotate_left(parent, root);
                                node = root->node;
                                break;
                        }
                } else {
                        struct rb_node *sibling = parent->left;

                        if (rb_is_red(sibling)) {
                                rb_set_black(sibling);
                                rb_set_red(parent);
                                rb_rotate_right(parent, root);
                                sibling = parent->left;
                        }

                        if ((!sibling->left || !rb_is_red(sibling->left)) &&
                            (!sibling->right || !rb_is_red(sibling->right))) {
                                rb_set_red(sibling);
                                node = parent;
                                parent = node->parent;
                        } else {
                                if (!sibling->left ||
                                    !rb_is_red(sibling->left)) {
                                        rb_set_black(sibling->right);
                                        rb_set_red(sibling);
                                        rb_rotate_left(sibling, root);
                                        sibling = parent->left;
                                }
                                sibling->color = parent->color;
                                rb_set_black(parent);
                                rb_set_black(sibling->left);
                                rb_rotate_right(parent, root);
                                node = root->node;
                                break;
                        }
                }
        }

        if (node)
                rb_set_black(node);
}

void rb_erase(struct rb_node *node, struct rb_root *root)
{
        struct rb_node *child, *parent;
        int color;

        if (!node->left)
                child = node->right;
        else if (!node->right)
                child = node->left;
        else {
                struct rb_node *successor = node->right;
                while (successor->left)
                        successor = successor->left;

                child = successor->right;
                parent = successor->parent;
                color = successor->color;

                if (successor != node->right) {
                        if (child)
                                child->parent = parent;

                        parent->left = child;
                        successor->right = node->right;
                        node->right->parent = successor;
                } else
                        parent = successor;

                successor->left = node->left;
                node->left->parent = successor;
                successor->parent = node->parent;
                successor->color = node->color;

                if (!node->parent)
                        root->node = successor;
                else if (node == node->parent->left)
                        node->parent->left = successor;
                else
                        node->parent->right = successor;

                if (color == 0)
                        rb_erase_color(parent, root);

                return;
        }

        parent = node->parent;
        color = node->color;

        if (child)
                child->parent = parent;

        if (!parent)
                root->node = child;
        else if (node == parent->left)
                parent->left = child;
        else
                parent->right = child;

        if (color == 0)
                rb_erase_color(parent, root);
}
