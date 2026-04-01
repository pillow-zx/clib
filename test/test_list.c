#include <list.h>
#include <stdio.h>
#include <assert.h>
#include <tools.h>
#include "test.h"

struct my_data {
        int value;
        struct list_head node;
};

static void test_list_init_state(void)
{
        LIST_HEAD(list);
        ASSERT(list_empty(&list));
        ASSERT(list_size(&list) == 0);
        ASSERT(list_first(&list) == NULL);
        ASSERT(list_last(&list) == NULL);

        struct list_head list2;
        INIT_LIST_HEAD(&list2);
        ASSERT(list_empty(&list2));
        ASSERT(list_empty_careful(&list2));
}

static void test_list_add_and_iterate(void)
{
        LIST_HEAD(list);

        struct my_data d1 = {.value = 1}, d2 = {.value = 2}, d3 = {.value = 3};
        INIT_LIST_HEAD(&d1.node);
        INIT_LIST_HEAD(&d2.node);
        INIT_LIST_HEAD(&d3.node);

        list_add(&d1.node, &list);
        ASSERT(!list_empty(&list));
        ASSERT(list_size(&list) == 1);
        ASSERT(list_first(&list) == &d1.node);
        ASSERT(list_last(&list) == &d1.node);

        list_add_tail(&d2.node, &list);
        ASSERT(list_size(&list) == 2);
        ASSERT(list_first(&list) == &d1.node);
        ASSERT(list_last(&list) == &d2.node);

        list_add(&d3.node, &list);
        ASSERT(list_size(&list) == 3);
        ASSERT(list_first(&list) == &d3.node);
        ASSERT(list_last(&list) == &d2.node);

        int expected[] = {3, 1, 2};
        int i = 0;
        struct list_head *pos;
        list_for_each (pos, &list) {
                struct my_data *data = container_of(pos, struct my_data, node);
                ASSERT(data->value == expected[i++]);
        }
}

static void test_list_delete_and_move_nodes(void)
{
        LIST_HEAD(list);
        struct my_data d1 = {.value = 1}, d2 = {.value = 2}, d3 = {.value = 3};
        INIT_LIST_HEAD(&d1.node);
        INIT_LIST_HEAD(&d2.node);
        INIT_LIST_HEAD(&d3.node);

        list_add_tail(&d1.node, &list);
        list_add_tail(&d2.node, &list);
        list_add(&d3.node, &list);

        list_del(&d1.node);
        ASSERT(list_size(&list) == 2);
        ASSERT(d1.node.prev == NULL);
        ASSERT(d1.node.next == NULL);

        list_del_init(&d3.node);
        ASSERT(list_size(&list) == 1);
        ASSERT(d3.node.prev == &d3.node);
        ASSERT(d3.node.next == &d3.node);

        LIST_HEAD(list3);
        list_move(&d2.node, &list3);
        ASSERT(list_empty(&list));
        ASSERT(list_size(&list3) == 1);
        ASSERT(list_first(&list3) == &d2.node);

        struct my_data d4 = {.value = 4};
        INIT_LIST_HEAD(&d4.node);
        list_add(&d4.node, &list3);

        ASSERT(list_first(&list3) == &d4.node);

        list_move_tail(&d4.node, &list3);

        ASSERT(list_first(&list3) == &d2.node);
        ASSERT(list_last(&list3) == &d4.node);
}

static void test_list_safe_iteration_delete(void)
{
        LIST_HEAD(list4);
        struct my_data items[5];
        int i;

        for (i = 0; i < 5; i++) {
                items[i].value = i;
                INIT_LIST_HEAD(&items[i].node);
                list_add_tail(&items[i].node, &list4);
        }
        ASSERT(list_size(&list4) == 5);

        struct list_head *pos2, *n;
        list_for_each_safe (pos2, n, &list4) {
                list_del(pos2);
        }
        ASSERT(list_empty(&list4));
}

static void test_list_entry_iteration_macros(void)
{
        LIST_HEAD(list5);
        struct my_data items[3];
        int i;

        for (i = 0; i < 3; i++) {
                items[i].value = i * 10;
                INIT_LIST_HEAD(&items[i].node);
                list_add_tail(&items[i].node, &list5);
        }

        int vals[] = {0, 10, 20};
        i = 0;
        struct my_data *entry;
        list_for_each_entry (entry, &list5, node) {
                ASSERT(entry->value == vals[i++]);
        }

        struct my_data *tmp;
        list_for_each_entry_safe (entry, tmp, &list5, node) {
                list_del(&entry->node);
        }
        ASSERT(list_empty(&list5));
}

static void test_list_is_head_check(void)
{
        LIST_HEAD(list5);
        struct my_data d1 = {.value = 1};
        INIT_LIST_HEAD(&d1.node);

        ASSERT(list_is_head(&list5, &list5));
        ASSERT(!list_is_head(&d1.node, &list5));
}

int main(void)
{
        RUN_TEST(test_list_init_state);
        RUN_TEST(test_list_add_and_iterate);
        RUN_TEST(test_list_delete_and_move_nodes);
        RUN_TEST(test_list_safe_iteration_delete);
        RUN_TEST(test_list_entry_iteration_macros);
        RUN_TEST(test_list_is_head_check);

        printf("All list tests passed!\n");
        return 0;
}
