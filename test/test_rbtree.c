#include <rbtree.h>
#include <assert.h>
#include "test.h"

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
        struct rb_node *parent = nullptr;

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
        return nullptr;
}

static void test_rbtree_empty_root_state(void)
{
        DEFINE_RBROOT(root);
        ASSERT(RB_EMPTY_ROOT(&root));
        ASSERT(rb_first(&root) == nullptr);
        ASSERT(rb_last(&root) == nullptr);
}

static void test_rbtree_insert_and_search(void)
{
        DEFINE_RBROOT(root);
        struct test_node nodes[10];

        for (int i = 0; i < 10; i++) {
                nodes[i].key = i;
                rb_node_init(&nodes[i].rb);
                insert_node(&root, &nodes[i]);
        }

        ASSERT(!RB_EMPTY_ROOT(&root));

        for (int i = 0; i < 10; i++) {
                struct test_node *found = search_node(&root, i);
                ASSERT(found && found->key == i);
        }
}

static void test_rbtree_order_traversal(void)
{
        DEFINE_RBROOT(root);
        struct test_node nodes[10];

        for (int i = 0; i < 10; i++) {
                nodes[i].key = i;
                rb_node_init(&nodes[i].rb);
                insert_node(&root, &nodes[i]);
        }

        struct rb_node *first = rb_first(&root);
        struct rb_node *last = rb_last(&root);
        ASSERT(first && container_of(first, struct test_node, rb)->key == 0);
        ASSERT(last && container_of(last, struct test_node, rb)->key == 9);

        int order = 0;
        struct test_node *entry;
        rbtree_for_each_entry (entry, &root, rb) {
                ASSERT(entry->key == order++);
        }

        ASSERT(order == 10);

        order = 9;
        rbtree_for_each_entry_reverse (entry, &root, rb) {
                ASSERT(entry->key == order--);
        }
}

static void test_rbtree_erase_single_node(void)
{
        struct rb_root single_root = RB_ROOT;
        struct test_node single = {.key = 42};
        rb_node_init(&single.rb);
        insert_node(&single_root, &single);
        rb_erase(&single.rb, &single_root);
        ASSERT(RB_EMPTY_ROOT(&single_root));
}

int main(void)
{
        RUN_TEST(test_rbtree_empty_root_state);
        RUN_TEST(test_rbtree_insert_and_search);
        RUN_TEST(test_rbtree_order_traversal);
        RUN_TEST(test_rbtree_erase_single_node);

        printf("All rbtree tests passed!\n");
        return 0;
}
