#ifndef __CLIB_COMPILER_H__
#define __CLIB_COMPILER_H__

#ifndef __always_inline
#define __always_inline inline __attribute__((__always_inline__))
#endif

#define __noinline __attribute__((__noinline__))
#define __unused __attribute__((__unused__))
#define __used __attribute__((__used__))
#define __maybe_unused __attribute__((__unused__))
#define __packed __attribute__((__packed__))
#define __aligned(x) __attribute__((__aligned__(x)))
#define __noreturn __attribute__((__noreturn__))
#define __pure __attribute__((__pure__))
#define __const __attribute__((__const__))
#define __malloc __attribute__((__malloc__))
#define __must_check __attribute__((__warn_unused_result__))

#ifndef __nonnull
#define __nonnull(...) __attribute__((nonnull(__VA_ARGS__)))
#endif

#define __printf(a, b) __attribute__((__format__(printf, a, b)))
#define __scanf(a, b) __attribute__((__format__(scanf, a, b)))
#define __fallthrough __attribute__((__fallthrough__))
#define __cleanup(func) __attribute__((__cleanup__(func)))
#define __section(section) __attribute__((__section__(section)))
#define __weak __attribute__((__weak__))
#define __autofree(x) __attribute__((__cleanup__(x)))
#define __likely(x) __builtin_expect(!!(x), 1)
#define __unlikely(x) __builtin_expect(!!(x), 0)
#define unreachable() __builtin_unreachable()
#define __init __attribute__((__section__(".init.text")))
#define __initdata __attribute__((__section__(".init.data")))
#define __text __attribute__((__section__(".text")))
#define __rodata __attribute__((__section__(".rodata")))

#endif
