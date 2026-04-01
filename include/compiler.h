#ifndef __CLIB_COMPILER_H__
#define __CLIB_COMPILER_H__

#ifndef __always_inline
#define __always_inline inline __attribute__((__always_inline__))
#endif

#ifndef __noinline
#define __noinline __attribute__((__noinline__))
#endif

#ifndef __unused
#define __unused __attribute__((__unused__))
#endif

#ifndef __used
#define __used __attribute__((__used__))
#endif

#ifndef __maybe_unused
#define __maybe_unused __attribute__((__unused__))
#endif

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

#ifndef __noreturn
#define __noreturn __attribute__((__noreturn__))
#endif

#ifndef __pure
#define __pure __attribute__((__pure__))
#endif

#ifndef __const
#define __const __attribute__((__const__))
#endif

#ifndef __malloc
#define __malloc __attribute__((__malloc__))
#endif

#ifndef __must_chek
#define __must_chek __attribute__((__warn_unused_result__))
#endif

#ifndef __nonnull
#define __nonnull(...) __attribute__((nonnull(__VA_ARGS__)))
#endif

#ifndef __printf
#define __printf(a, b) __attribute__((__format__(printf, a, b)))
#endif

#ifndef __scanf
#define __scanf(a, b) __attribute__((__format__(scanf, a, b)))
#endif

#ifndef __fallthrough
#define __fallthrough __attribute__((__fallthrough__))
#endif

#ifndef __cleanup
#define __cleanup(func) __attribute__((__cleanup__(func)))
#endif
#ifndef __section
#define __section(section) __attribute__((__section__(section)))
#endif
#ifndef __weak
#define __weak __attribute__((__weak__))
#endif
#ifndef __autofree
#define __autofree(x) __attribute__((__cleanup__(x)))
#endif

#define __likely(x) __builtin_expect(!!(x), 1)

#define __unlikely(x) __builtin_expect(!!(x), 0)

#define unreachable() __builtin_unreachable()

#endif
