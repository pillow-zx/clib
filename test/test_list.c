#include <list.h>
#include <stdio.h>
#include <assert.h>
#include "tools.h"

struct my_data {
        int value;
        struct list_head node;
};

int main(void)
{
        LIST_HEAD(list);
        assert(list_empty(&list));
        assert(list_size(&list) == 0);
        assert(list_first(&list) == NULL);
        assert(list_last(&list) == NULL);

        struct list_head list2;
        INIT_LIST_HEAD(&list2);
        assert(list_empty(&list2));
        assert(list_empty_careful(&list2));

        struct my_data d1 = {.value = 1}, d2 = {.value = 2}, d3 = {.value = 3};
        INIT_LIST_HEAD(&d1.node);
        INIT_LIST_HEAD(&d2.node);
        INIT_LIST_HEAD(&d3.node);

        list_add(&d1.node, &list);
        assert(!list_empty(&list));
        assert(list_size(&list) == 1);
        assert(list_first(&list) == &d1.node);
        assert(list_last(&list) == &d1.node);

        list_add_tail(&d2.node, &list);
        assert(list_size(&list) == 2);
        assert(list_first(&list) == &d1.node);
        assert(list_last(&list) == &d2.node);

        list_add(&d3.node, &list);
        assert(list_size(&list) == 3);
        assert(list_first(&list) == &d3.node);
        assert(list_last(&list) == &d2.node);

        int expected[] = {3, 1, 2};
        int i = 0;
        struct list_head *pos;
        list_for_each (pos, &list) {
                struct my_data *data = container_of(pos, struct my_data, node);
                assert(data->value == expected[i++]);
        }

        list_del(&d1.node);
        assert(list_size(&list) == 2);
        assert(d1.node.prev == NULL);
        assert(d1.node.next == NULL);

        list_del_init(&d3.node);
        assert(list_size(&list) == 1);
        assert(d3.node.prev == &d3.node);
        assert(d3.node.next == &d3.node);

        LIST_HEAD(list3);
        list_move(&d2.node, &list3);
        assert(list_empty(&list));
        assert(list_size(&list3) == 1);
        assert(list_first(&list3) == &d2.node);

        struct my_data d4 = {.value = 4};
        INIT_LIST_HEAD(&d4.node);
        list_add(&d4.node, &list3);

        assert(list_first(&list3) == &d4.node);

        list_move_tail(&d4.node, &list3);

        assert(list_first(&list3) == &d2.node);
        assert(list_last(&list3) == &d4.node);

        LIST_HEAD(list4);
        struct my_data items[5];
        for (i = 0; i < 5; i++) {
                items[i].value = i;
                INIT_LIST_HEAD(&items[i].node);
                list_add_tail(&items[i].node, &list4);
        }
        assert(list_size(&list4) == 5);

        struct list_head *pos2, *n;
        list_for_each_safe (pos2, n, &list4) {
                list_del(pos2);
        }
        assert(list_empty(&list4));

        LIST_HEAD(list5);
        for (i = 0; i < 3; i++) {
                items[i].value = i * 10;
                INIT_LIST_HEAD(&items[i].node);
                list_add_tail(&items[i].node, &list5);
        }

        int vals[] = {0, 10, 20};
        i = 0;
        struct my_data *entry;
        list_for_each_entry (entry, &list5, node) {
                assert(entry->value == vals[i++]);
        }

        struct my_data *tmp;
        list_for_each_entry_safe (entry, tmp, &list5, node) {
                list_del(&entry->node);
        }
        assert(list_empty(&list5));

        assert(list_is_head(&list5, &list5));
        assert(!list_is_head(&d1.node, &list5));

        printf("All list tests passed!\n");
        return 0;
}
