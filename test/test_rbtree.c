#include "rbtree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct test_node {
        int key;
        struct rb_node rb;
};

static int cmp(int a, int b)
{
        return a - b;
}

static void insert_node(struct rb_root *root, struct test_node *node)
{
        struct rb_node **link = &root->node;
        struct rb_node *parent = NULL;

        while (*link) {
                parent = *link;
                struct test_node *entry =
                        container_of(parent, struct test_node, rb);
                int c = cmp(node->key, entry->key);
                if (c < 0)
                        link = &parent->left;
                else
                        link = &parent->right;
        }

        rb_link_node(&node->rb, parent, link);
        rb_insert_color(&node->rb, root);
}

static struct test_node *search_node(struct rb_root *root, int key)
{
        struct rb_node *node = root->node;
        while (node) {
                struct test_node *entry =
                        container_of(node, struct test_node, rb);
                int c = cmp(key, entry->key);
                if (c < 0)
                        node = node->left;
                else if (c > 0)
                        node = node->right;
                else
                        return entry;
        }
        return NULL;
}

int main(void)
{
        DEFINE_RBROOT(root);
        struct test_node nodes[10];

        for (int i = 0; i < 10; i++) {
                nodes[i].key = i;
                rb_node_init(&nodes[i].rb);
        }

        assert(RB_EMPTY_ROOT(&root));
        assert(rb_first(&root) == NULL);
        assert(rb_last(&root) == NULL);

        for (int i = 0; i < 10; i++) {
                insert_node(&root, &nodes[i]);
        }

        for (int i = 0; i < 10; i++) {
                struct test_node *found = search_node(&root, i);
                assert(found && found->key == i);
        }

        assert(!RB_EMPTY_ROOT(&root));

        struct rb_node *first = rb_first(&root);
        struct rb_node *last = rb_last(&root);
        assert(first && container_of(first, struct test_node, rb)->key == 0);
        assert(last && container_of(last, struct test_node, rb)->key == 9);

        int order = 0;
        for (struct rb_node *node = rb_first(&root); node;
             node = rb_next(node)) {
                struct test_node *entry =
                        container_of(node, struct test_node, rb);
                assert(entry->key == order++);
        }
        assert(order == 10);

        order = 9;
        for (struct rb_node *node = rb_last(&root); node;
             node = rb_prev(node)) {
                struct test_node *entry =
                        container_of(node, struct test_node, rb);
                assert(entry->key == order--);
        }

        struct rb_root single_root = RB_ROOT;
        struct test_node single = {.key = 42};
        rb_node_init(&single.rb);
        insert_node(&single_root, &single);
        rb_erase(&single.rb, &single_root);
        assert(RB_EMPTY_ROOT(&single_root));

        printf("All rbtree tests passed!\n");
        return 0;
}
