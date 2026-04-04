#ifndef __CLIB_LIST_H__
#define __CLIB_LIST_H__

#include <types.h>
#include <compiler.h>

struct list_head {
        struct list_head *prev;
        struct list_head *next;
};

#define LIST_HEAD_INIT(name) {&(name), &(name)}

#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

static __always_inline void INIT_LIST_HEAD(struct list_head *list)
{
        list->prev = list;
        list->next = list;
}

static __always_inline void __list_add(struct list_head *new_node,
                                       struct list_head *prev,
                                       struct list_head *next)
{
        next->prev = new_node;
        new_node->next = next;
        new_node->prev = prev;
        prev->next = new_node;
}

static __always_inline void list_add(struct list_head *new_node,
                                     struct list_head *head)
{
        __list_add(new_node, head, head->next);
}

static __always_inline void list_add_tail(struct list_head *new_node,
                                          struct list_head *head)
{
        __list_add(new_node, head->prev, head);
}

static __always_inline void __list_del(struct list_head *prev,
                                       struct list_head *next)
{
        next->prev = prev;
        prev->next = next;
}

static __always_inline void list_del(struct list_head *entry)
{
        __list_del(entry->prev, entry->next);
        entry->prev = NULL;
        entry->next = NULL;
}

static __always_inline void list_del_init(struct list_head *entry)
{
        __list_del(entry->prev, entry->next);
        INIT_LIST_HEAD(entry);
}

static __always_inline void list_move(struct list_head *list,
                                      struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add(list, head);
}

static __always_inline void list_move_tail(struct list_head *list,
                                           struct list_head *head)
{
        __list_del(list->prev, list->next);
        list_add_tail(list, head);
}

static __always_inline __must_check __pure bool
list_empty(const struct list_head *head)
{
        return head->next == head;
}

static __always_inline __must_check __pure bool
list_empty_careful(const struct list_head *head)
{
        struct list_head *next = head->next;
        return (next == head) && (next == head->prev);
}

static __always_inline __must_check __pure struct list_head *
list_first(const struct list_head *head)
{
        return list_empty(head) ? NULL : head->next;
}

static __always_inline __must_check __pure struct list_head *
list_last(const struct list_head *head)
{
        return list_empty(head) ? NULL : head->prev;
}

#define list_for_each(pos, head)                                               \
        for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head)                                       \
        for (pos = (head)->next, n = pos->next; pos != (head);                 \
             pos = n, n = pos->next)

#define list_entry(ptr, type, member)                                          \
        ((type *)((char *)(ptr) - offsetof(type, member)))

#define list_for_each_entry(pos, head, member)                                 \
        for (pos = list_entry((head)->next, typeof(*pos), member);             \
             &pos->member != (head);                                           \
             pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member)                         \
        for (pos = list_entry((head)->next, typeof(*pos), member),             \
            n = list_entry(pos->member.next, typeof(*pos), member);            \
             &pos->member != (head);                                           \
             pos = n, n = list_entry(n->member.next, typeof(*pos), member))

static __always_inline __must_check usize list_size(const struct list_head *head)
{
        usize count = 0;
        struct list_head *pos;
        list_for_each (pos, head) {
                count++;
        }
        return count;
}

static __always_inline __must_check __nonnull((1, 2)) bool list_is_head(
        const struct list_head *list, const struct list_head *head)
{
        return list == head;
}

#endif
