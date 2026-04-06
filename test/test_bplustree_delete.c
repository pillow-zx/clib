#include <bplustree.h>
#include <assert.h>
#include <stdio.h>
#include "test.h"

static void test_bplustree_delete_rebalance_borrow_from_right(void)
{
        bplus_tree_t *tree = bplus_create();
        ASSERT(tree != nullptr);

        /* 插入足够多的元素以创建多个叶子节点 */
        for (int i = 0; i < 200; i++) {
                ASSERT(bplus_insert(tree, i, (void *)(long)i));
        }
        ASSERT(bplus_count(tree) == 200);

        /* 从左边删除，触发从右兄弟借键 */
        for (int i = 0; i < 100; i++) {
                ASSERT(bplus_delete(tree, i));
        }
        ASSERT(bplus_count(tree) == 100);

        /* 验证剩余元素仍然正确 */
        for (int i = 100; i < 200; i++) {
                ASSERT(bplus_search(tree, i) == (void *)(long)i);
        }

        /* 验证已删除元素不存在 */
        for (int i = 0; i < 100; i++) {
                ASSERT(bplus_search(tree, i) == nullptr);
        }

        bplus_destroy(tree);
}

static void test_bplustree_delete_rebalance_borrow_from_left(void)
{
        bplus_tree_t *tree = bplus_create();
        ASSERT(tree != nullptr);

        /* 插入足够多的元素 */
        for (int i = 0; i < 200; i++) {
                ASSERT(bplus_insert(tree, i, (void *)(long)i));
        }

        /* 从右边删除，触发从左兄弟借键 */
        for (int i = 199; i >= 100; i--) {
                ASSERT(bplus_delete(tree, i));
        }
        ASSERT(bplus_count(tree) == 100);

        /* 验证剩余元素正确 */
        for (int i = 0; i < 100; i++) {
                ASSERT(bplus_search(tree, i) == (void *)(long)i);
        }

        bplus_destroy(tree);
}

static void test_bplustree_delete_rebalance_merge(void)
{
        bplus_tree_t *tree = bplus_create();
        ASSERT(tree != nullptr);

        /* 插入元素 */
        for (int i = 0; i < 500; i++) {
                ASSERT(bplus_insert(tree, i, (void *)(long)i));
        }
        ASSERT(bplus_height(tree) > 1);

        /* 删除大部分元素，触发合并 */
        for (int i = 0; i < 400; i++) {
                ASSERT(bplus_delete(tree, i));
        }
        ASSERT(bplus_count(tree) == 100);

        /* 验证树仍然有效 */
        for (int i = 400; i < 500; i++) {
                ASSERT(bplus_search(tree, i) == (void *)(long)i);
        }

        bplus_destroy(tree);
}

static void test_bplustree_delete_root_collapse(void)
{
        bplus_tree_t *tree = bplus_create();
        ASSERT(tree != nullptr);

        /* 插入大量元素构建多层树（64阶B+树需要较多元素才能达到高度3） */
        for (int i = 0; i < 10000; i++) {
                ASSERT(bplus_insert(tree, i, (void *)(long)i));
        }
        i32 max_height = bplus_height(tree);
        ASSERT(max_height >= 2);

        /* 删除到只剩少数元素，观察高度下降 */
        for (int i = 0; i < 9950; i++) {
                ASSERT(bplus_delete(tree, i));
        }
        ASSERT(bplus_count(tree) == 50);
        ASSERT(bplus_height(tree) <= max_height);

        /* 删除到只剩一个元素 */
        for (int i = 9950; i < 9999; i++) {
                ASSERT(bplus_delete(tree, i));
        }
        ASSERT(bplus_count(tree) == 1);
        ASSERT(bplus_height(tree) == 1);
        ASSERT(bplus_search(tree, 9999) == (void *)9999L);

        /* 删除最后一个元素 */
        ASSERT(bplus_delete(tree, 9999));
        ASSERT(bplus_count(tree) == 0);
        ASSERT(bplus_height(tree) == 0);

        bplus_destroy(tree);
}

static void test_bplustree_delete_alternating_pattern(void)
{
        bplus_tree_t *tree = bplus_create();
        ASSERT(tree != nullptr);

        /* 交替插入 */
        for (int i = 0; i < 100; i++) {
                ASSERT(bplus_insert(tree, i * 2, (void *)(long)(i * 2)));
                ASSERT(bplus_insert(tree, i * 2 + 1,
                                    (void *)(long)(i * 2 + 1)));
        }
        ASSERT(bplus_count(tree) == 200);

        /* 交替删除 */
        for (int i = 0; i < 100; i++) {
                ASSERT(bplus_delete(tree, i * 2));
                ASSERT(bplus_count(tree) == (usize)(200 - i - 1));
        }
        ASSERT(bplus_count(tree) == 100);

        /* 验证剩余奇数键 */
        for (int i = 0; i < 100; i++) {
                ASSERT(bplus_search(tree, i * 2 + 1) ==
                       (void *)(long)(i * 2 + 1));
                ASSERT(bplus_search(tree, i * 2) == nullptr);
        }

        bplus_destroy(tree);
}

int main(void)
{
        RUN_TEST(test_bplustree_delete_rebalance_borrow_from_right);
        RUN_TEST(test_bplustree_delete_rebalance_borrow_from_left);
        RUN_TEST(test_bplustree_delete_rebalance_merge);
        RUN_TEST(test_bplustree_delete_root_collapse);
        RUN_TEST(test_bplustree_delete_alternating_pattern);

        printf("All B+ tree delete rebalance tests passed!\n");
        return 0;
}
