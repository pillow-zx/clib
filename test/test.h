#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define ANSI_FG_GREEN "\33[1;32m"
#define ANSI_FG_RED "\33[1;31m"
#define ANSI_NONE "\33[0m"

typedef struct {
        size_t tests_run;
        size_t tests_failed;
        size_t asserts_run;
        size_t asserts_failed;
        bool current_test_failed;
        const char *current_test_name;
} test_state_t;

static test_state_t g_test_state = {0};

static inline void test_reset_state(void)
{
        g_test_state = (test_state_t){0};
}

static inline int test_exit_code(void)
{
        return g_test_state.tests_failed > 0 ? 1 : 0;
}

static inline void test_print_summary(void)
{
        printf("\n[SUMMARY] tests: %zu, failed: %zu, asserts: %zu, "
               "assert_failed: %zu\n",
               g_test_state.tests_run, g_test_state.tests_failed,
               g_test_state.asserts_run, g_test_state.asserts_failed);
}

#define TEST_SUMMARY() test_print_summary()
#define TEST_EXIT_CODE() test_exit_code()

#define TEST_FAIL_FMT(msg, ...)                                                \
        printf(ANSI_FG_RED "[FAILED] " msg ANSI_NONE "\n", ##__VA_ARGS__)

#define TEST_RECORD_ASSERT_FAILURE()                                           \
        do {                                                                   \
                g_test_state.asserts_failed++;                                 \
                g_test_state.current_test_failed = true;                       \
        } while (0)

#define RUN_TEST(fn, ...)                                                      \
        do {                                                                   \
                g_test_state.tests_run++;                                      \
                g_test_state.current_test_failed = false;                      \
                g_test_state.current_test_name = #fn;                          \
                fn(__VA_ARGS__);                                               \
                if (g_test_state.current_test_failed) {                        \
                        g_test_state.tests_failed++;                           \
                        printf(ANSI_FG_RED "[FAIL]: %s\n" ANSI_NONE, #fn);     \
                } else {                                                       \
                        printf(ANSI_FG_GREEN "[PASS]: %s\n" ANSI_NONE, #fn);   \
                }                                                              \
        } while (0)

#define ASSERT(cond)                                                           \
        do {                                                                   \
                g_test_state.asserts_run++;                                    \
                if (!(cond)) {                                                 \
                        TEST_RECORD_ASSERT_FAILURE();                          \
                        TEST_FAIL_FMT("ASSERT failed: %s (%s:%d %s)", #cond,   \
                                      __FILE__, __LINE__, __func__);           \
                }                                                              \
        } while (0)

#define REQUIRE(cond)                                                          \
        do {                                                                   \
                g_test_state.asserts_run++;                                    \
                if (!(cond)) {                                                 \
                        TEST_RECORD_ASSERT_FAILURE();                          \
                        TEST_FAIL_FMT("REQUIRE failed: %s (%s:%d %s)", #cond,  \
                                      __FILE__, __LINE__, __func__);           \
                        return;                                                \
                }                                                              \
        } while (0)

#define ASSERT_EQ_INT(expected, actual)                                        \
        do {                                                                   \
                long long _expected = (long long)(expected);                   \
                long long _actual = (long long)(actual);                       \
                g_test_state.asserts_run++;                                    \
                if (_expected != _actual) {                                    \
                        TEST_RECORD_ASSERT_FAILURE();                          \
                        TEST_FAIL_FMT("ASSERT_EQ_INT failed: expected=%lld "   \
                                      "actual=%lld (%s:%d %s)",                \
                                      _expected, _actual, __FILE__, __LINE__,  \
                                      __func__);                               \
                }                                                              \
        } while (0)

#define ASSERT_EQ_PTR(expected, actual)                                        \
        do {                                                                   \
                const void *_expected = (const void *)(expected);              \
                const void *_actual = (const void *)(actual);                  \
                g_test_state.asserts_run++;                                    \
                if (_expected != _actual) {                                    \
                        TEST_RECORD_ASSERT_FAILURE();                          \
                        TEST_FAIL_FMT("ASSERT_EQ_PTR failed: expected=%p "     \
                                      "actual=%p (%s:%d %s)",                  \
                                      _expected, _actual, __FILE__, __LINE__,  \
                                      __func__);                               \
                }                                                              \
        } while (0)

#define ASSERT_STREQ(expected, actual)                                         \
        do {                                                                   \
                const char *_expected = (const char *)(expected);              \
                const char *_actual = (const char *)(actual);                  \
                g_test_state.asserts_run++;                                    \
                if ((_expected == NULL && _actual != NULL) ||                  \
                    (_expected != NULL && _actual == NULL) ||                  \
                    (_expected != NULL && _actual != NULL &&                   \
                     strcmp(_expected, _actual) != 0)) {                       \
                        TEST_RECORD_ASSERT_FAILURE();                          \
                        TEST_FAIL_FMT("ASSERT_STREQ failed: expected=\"%s\" "  \
                                      "actual=\"%s\" (%s:%d %s)",              \
                                      _expected ? _expected : "(null)",        \
                                      _actual ? _actual : "(null)", __FILE__,  \
                                      __LINE__, __func__);                     \
                }                                                              \
        } while (0)

#define ASSERT_NULL(actual)                                                    \
        do {                                                                   \
                const void *_actual = (const void *)(actual);                  \
                g_test_state.asserts_run++;                                    \
                if (_actual != NULL) {                                         \
                        TEST_RECORD_ASSERT_FAILURE();                          \
                        TEST_FAIL_FMT(                                         \
                                "ASSERT_NULL failed: actual=%p (%s:%d %s)",    \
                                _actual, __FILE__, __LINE__, __func__);        \
                }                                                              \
        } while (0)

#endif
