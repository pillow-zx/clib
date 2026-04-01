#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "test.h"

#define LOGLEVEL TRACELEVEL
#include <debug.h>

static bool contains(const char *haystack, const char *needle)
{
        return strstr(haystack, needle) != NULL;
}

static void test_debug_level_ordering_rules(void)
{
        static_assert(ERRORLEVEL > INFOLEVEL, "error level ordering");
        static_assert(INFOLEVEL > DEBUGLEVEL, "info level ordering");
        static_assert(DEBUGLEVEL > TRACELEVEL, "debug level ordering");
}

static void test_debug_log_output_content(void)
{
        int stdout_fd = dup(fileno(stdout));
        ASSERT(stdout_fd != -1);

        FILE *capture = tmpfile();
        ASSERT(capture != NULL);

        int rc = dup2(fileno(capture), fileno(stdout));
        ASSERT(rc != -1);

        Info("DebugTest", "hello %s", "info");
        Debug("DebugTest", "hello %s", "debug");
        Trace("DebugTest", "hello %s", "trace");
        Error("DebugTest", "hello %s", "error");
        ASSERT(1 == 1);
        ASSERT(1 == 2);

        fflush(stdout);

        rc = dup2(stdout_fd, fileno(stdout));
        ASSERT(rc != -1);
        close(stdout_fd);

        rc = fseek(capture, 0, SEEK_END);
        ASSERT(rc == 0);
        long size = ftell(capture);
        ASSERT(size >= 0);
        rc = fseek(capture, 0, SEEK_SET);
        ASSERT(rc == 0);

        char *buf = malloc((size_t)size + 1);
        ASSERT(buf != NULL);

        size_t nread = fread(buf, 1, (size_t)size, capture);
        ASSERT(nread == (size_t)size);
        buf[size] = '\0';

        ASSERT(contains(buf, ANSI_FG_GREEN));
        ASSERT(contains(buf, ANSI_FG_YELLOW));
        ASSERT(contains(buf, ANSI_FG_BLUE));
        ASSERT(contains(buf, ANSI_FG_RED));
        ASSERT(contains(buf, ANSI_NONE));

        ASSERT(contains(buf, "[DebugTest]"));
        ASSERT(contains(buf, "hello info"));
        ASSERT(contains(buf, "hello debug"));
        ASSERT(contains(buf, "hello trace"));
        ASSERT(contains(buf, "hello error"));
        ASSERT(contains(buf, "[ASSERT]"));
        ASSERT(contains(buf, "ASSERT failed: 1 == 2"));

        free(buf);
        fclose(capture);
}

int main(void)
{
        RUN_TEST(test_debug_level_ordering_rules);
        RUN_TEST(test_debug_log_output_content);

        puts("All debug tests passed!");
        return 0;
}
