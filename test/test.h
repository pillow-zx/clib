#ifndef __TEST_H__
#define __TEST_H__

#define ANSI_FG_GREEN "\33[1;32m"
#define ANSI_FG_RED "\33[1;31m"
#define ANSI_NONE "\33[0m"

#define RUN_TEST(fn, ...)                                                      \
        do {                                                                   \
                fn(__VA_ARGS__);                                               \
                printf(ANSI_FG_GREEN "[PASS]: %s:%d %s\n" ANSI_NONE, __FILE__, \
                       __LINE__, __func__);                                    \
        } while (0)

#define ASSERT(cond)                                                           \
        do {                                                                   \
                if (!(cond)) {                                                 \
                        printf(ANSI_FG_RED "[FAILED] ASSERT failed: " #cond    \
                                           " (%s:%d %s)\n" ANSI_NONE,          \
                               __FILE__, __LINE__, __func__);                  \
                }                                                              \
        } while (0)

#endif
