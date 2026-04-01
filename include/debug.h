#ifndef __CLIB_DEBUG_H__
#define __CLIB_DEBUG_H__

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

#define ERRORLEVEL 4
#define INFOLEVEL 3
#define DEBUGLEVEL 2
#define TRACELEVEL 1

#ifndef LOGLEVEL
#define LOGLEVEL ERRORLEVEL
#endif

#define ANSI_FMT(str, fmt) fmt str ANSI_NONE

#define _Log(level, ...)                                                       \
        do {                                                                   \
                if (level >= LOGLEVEL)                                         \
                        printf(__VA_ARGS__);                                   \
        } while (0)

#define Info(place, format, ...)                                               \
        do {                                                                   \
                _Log(INFOLEVEL,                                                \
                     ANSI_FMT("[" place "][%s:%d %s] " format,                 \
                              ANSI_FG_GREEN) "\n",                             \
                     __FILE__, __LINE__, __func__, ##__VA_ARGS__);             \
        } while (0)

#define Debug(place, format, ...)                                              \
        do {                                                                   \
                _Log(DEBUGLEVEL,                                               \
                     ANSI_FMT("[" place "][%s:%d %s] " format,                 \
                              ANSI_FG_YELLOW) "\n",                            \
                     __FILE__, __LINE__, __func__, ##__VA_ARGS__);             \
        } while (0)

#define Trace(place, format, ...)                                              \
        do {                                                                   \
                _Log(TRACELEVEL,                                               \
                     ANSI_FMT("[" place "][%s:%d %s] " format,                 \
                              ANSI_FG_BLUE) "\n",                              \
                     __FILE__, __LINE__, __func__, ##__VA_ARGS__);             \
        } while (0)

#define Error(place, format, ...)                                              \
        do {                                                                   \
                _Log(ERRORLEVEL,                                               \
                     ANSI_FMT("[" place "][%s:%d %s] " format,                 \
                              ANSI_FG_RED) "\n",                               \
                     __FILE__, __LINE__, __func__, ##__VA_ARGS__);             \
        } while (0)

#ifndef static_assert
#define static_assert(cond, msg) _Static_assert(cond, msg)
#endif

#define Assert(cond)                                                           \
        do {                                                                   \
                if (!(cond))                                                   \
                        Error("Assert", "Assert failed: " #cond);              \
        } while (0)

#endif
