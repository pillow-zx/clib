#ifndef CLIB_DEBUG_H
#define CLIB_DEBUG_H

#include <port.h>

#define ANSI_FG_BLACK "\33[1;30m"
#define ANSI_FG_RED "\33[1;31m"
#define ANSI_FG_GREEN "\33[1;32m"
#define ANSI_FG_YELLOW "\33[1;33m"
#define ANSI_FG_BLUE "\33[1;34m"
#define ANSI_FG_MAGENTA "\33[1;35m"
#define ANSI_FG_CYAN "\33[1;36m"
#define ANSI_FG_WHITE "\33[1;37m"
#define ANSI_BG_BLACK "\33[1;40m"
#define ANSI_BG_RED "\33[1;41m"
#define ANSI_BG_GREEN "\33[1;42m"
#define ANSI_BG_YELLOW "\33[1;43m"
#define ANSI_BG_BLUE "\33[1;44m"
#define ANSI_BG_MAGENTA "\33[1;45m"
#define ANSI_BG_CYAN "\33[1;46m"
#define ANSI_BG_WHITE "\33[1;47m"
#define ANSI_NONE "\33[0m"

#define ALL 0
#define TRACE 1
#define DEBUG 2
#define INFO 3
#define NOTICE 4
#define WARNING 5
#define ERROR 6

#ifndef LOGLEVEL
#define LOGLEVEL ALL
#endif

#define ANSI_FMT(str) "%s" str ANSI_NONE

#define _Log(level, ...)                                                       \
        do {                                                                   \
                if (level >= LOGLEVEL)                                         \
                        cprintf(__VA_ARGS__);                                  \
        } while (0)

#define LOG_COLOR(level)                                                       \
        ((level) == ERROR     ? ANSI_FG_RED                                    \
         : (level) == WARNING ? ANSI_FG_YELLOW                                 \
         : (level) == NOTICE  ? ANSI_FG_CYAN                                   \
         : (level) == INFO    ? ANSI_FG_GREEN                                  \
         : (level) == DEBUG   ? ANSI_FG_BLUE                                   \
         : (level) == TRACE   ? ANSI_FG_MAGENTA                                \
                              : ANSI_NONE)

#define LOG_LEVEL_STR(level)                                                   \
        ((level) == ERROR     ? "ERROR"                                        \
         : (level) == WARNING ? "WARNING"                                      \
         : (level) == NOTICE  ? "NOTICE"                                       \
         : (level) == INFO    ? "INFO"                                         \
         : (level) == DEBUG   ? "DEBUG"                                        \
         : (level) == TRACE   ? "TRACE"                                        \
                              : "UNK")

#define print(level, place, format, ...)                                       \
        do {                                                                   \
                _Log(level, ANSI_FMT("[%s][%s][%s:%d %s] " format),            \
                     LOG_COLOR((level)), LOG_LEVEL_STR((level)), place,        \
                     __FILE__, __LINE__, __func__, ##__VA_ARGS__);             \
        } while (0)

#define println(level, place, format, ...)                                     \
        do {                                                                   \
                _Log(level, ANSI_FMT("[%s][%s][%s:%d %s] " format "\n"),       \
                     LOG_COLOR((level)), LOG_LEVEL_STR((level)), place,        \
                     __FILE__, __LINE__, __func__, ##__VA_ARGS__);             \
        } while (0)

#ifndef static_assert
#define static_assert(cond, msg) _Static_assert(cond, msg)
#endif

#endif
