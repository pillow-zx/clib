#include <ringbuf.h>
#include <stdio.h>
#include <string.h>
#include "test.h"

static void test_init(void)
{
        char buffer[16];
        struct ringbuf rb;

        ringbuf_init(&rb, buffer, sizeof(buffer));
        ASSERT(rb.buf == buffer);
        ASSERT(rb.size == 16);
        ASSERT(rb.head == 0);
        ASSERT(rb.tail == 0);
        ASSERT(ringbuf_empty(&rb));
}

static void test_write_read_byte(void)
{
        char buffer[8];
        struct ringbuf rb;
        ringbuf_init(&rb, buffer, sizeof(buffer));

        ASSERT(ringbuf_write_byte(&rb, 'A') == 0);
        ASSERT(ringbuf_write_byte(&rb, 'B') == 0);
        ASSERT(ringbuf_write_byte(&rb, 'C') == 0);

        ASSERT(ringbuf_used(&rb) == 3);
        ASSERT(!ringbuf_empty(&rb));
        ASSERT(!ringbuf_full(&rb));

        ASSERT(ringbuf_read_byte(&rb) == 'A');
        ASSERT(ringbuf_read_byte(&rb) == 'B');
        ASSERT(ringbuf_read_byte(&rb) == 'C');

        ASSERT(ringbuf_empty(&rb));
}

static void test_full_buffer(void)
{
        char buffer[4];
        struct ringbuf rb;
        ringbuf_init(&rb, buffer, sizeof(buffer));

        ASSERT(ringbuf_write_byte(&rb, 1) == 0);
        ASSERT(ringbuf_write_byte(&rb, 2) == 0);
        ASSERT(ringbuf_write_byte(&rb, 3) == 0);

        ASSERT(ringbuf_full(&rb));
        ASSERT(ringbuf_write_byte(&rb, 4) == -1);

        ASSERT(ringbuf_read_byte(&rb) == 1);
        ASSERT(!ringbuf_full(&rb));
        ASSERT(ringbuf_write_byte(&rb, 4) == 0);
}

static void test_wrap_around(void)
{
        char buffer[5];
        struct ringbuf rb;
        ringbuf_init(&rb, buffer, sizeof(buffer));

        for (int i = 0; i < 100; i++) {
                ASSERT(ringbuf_write_byte(&rb, (char)i) == 0);
                ASSERT(ringbuf_read_byte(&rb) == (char)i);
        }

        ASSERT(ringbuf_empty(&rb));
}

static void test_write_read_batch(void)
{
        char buffer[16];
        struct ringbuf rb;
        ringbuf_init(&rb, buffer, sizeof(buffer));

        const char *msg = "Hello";
        usize written = ringbuf_write(&rb, msg, 5);
        ASSERT(written == 5);

        char out[8] = {0};
        usize read = ringbuf_read(&rb, out, 5);
        ASSERT(read == 5);
        ASSERT(memcmp(out, "Hello", 5) == 0);
}

static void test_peek(void)
{
        char buffer[8];
        struct ringbuf rb;
        ringbuf_init(&rb, buffer, sizeof(buffer));

        /* ringbuf_write(&rb, "ABC", 3); */
        ASSERT(ringbuf_write(&rb, "ABC", 3) == 3);

        char out[4] = {0};
        ASSERT(ringbuf_peek(&rb, out, 3) == 3);
        ASSERT(memcmp(out, "ABC", 3) == 0);

        ASSERT(!ringbuf_empty(&rb));
        ASSERT(ringbuf_used(&rb) == 3);
}

static void test_skip(void)
{
        char buffer[8];
        struct ringbuf rb;
        ringbuf_init(&rb, buffer, sizeof(buffer));

        /* ringbuf_write(&rb, "ABCDE", 5); */
        ASSERT(ringbuf_write(&rb, "ABCDE", 5) == 5);

        ASSERT(ringbuf_skip(&rb, 2) == 2);
        ASSERT(ringbuf_used(&rb) == 3);

        char out[4] = {0};
        /* ringbuf_read(&rb, out, 3); */
        ASSERT(ringbuf_read(&rb, out, 3) == 3);
        ASSERT(memcmp(out, "CDE", 3) == 0);
}

static void test_free_space(void)
{
        char buffer[8];
        struct ringbuf rb;
        ringbuf_init(&rb, buffer, sizeof(buffer));

        ASSERT(ringbuf_free(&rb) == 7);
        ASSERT(RINGBUF_SIZE(&rb) == 7);

        /* ringbuf_write_byte(&rb, 1); */
        ASSERT(ringbuf_write_byte(&rb, 1) == 0);
        ASSERT(ringbuf_free(&rb) == 6);
        ASSERT(ringbuf_used(&rb) == 1);
}

static void test_reset(void)
{
        char buffer[8];
        struct ringbuf rb;
        ringbuf_init(&rb, buffer, sizeof(buffer));

        /* ringbuf_write(&rb, "Test", 4); */
        ASSERT(ringbuf_write(&rb, "Test", 4) == 4);
        ASSERT(ringbuf_used(&rb) == 4);

        ringbuf_reset(&rb);
        ASSERT(ringbuf_empty(&rb));
        ASSERT(ringbuf_used(&rb) == 0);
}

static void test_macro_init(void)
{
        char buffer[8];
        RINGBUF_DEFINE(rb, buffer, sizeof(buffer));

        ASSERT(rb.buf == buffer);
        ASSERT(rb.size == 8);
        ASSERT(ringbuf_empty(&rb));
}

int main(void)
{
        RUN_TEST(test_init);
        RUN_TEST(test_write_read_byte);
        RUN_TEST(test_full_buffer);
        RUN_TEST(test_wrap_around);
        RUN_TEST(test_write_read_batch);
        RUN_TEST(test_peek);
        RUN_TEST(test_skip);
        RUN_TEST(test_free_space);
        RUN_TEST(test_reset);
        RUN_TEST(test_macro_init);

        printf("All ringbuf tests passed!\n");
        return 0;
}
