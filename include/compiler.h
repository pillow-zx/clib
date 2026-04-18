/**
 * @file compiler.h
 * @brief Compiler attributes and built-in wrappers for optimization and code
 * analysis
 *
 * This header provides a collection of compiler attribute macros that enable
 * various optimizations, static analysis hints, and code generation controls.
 * These attributes are primarily GCC/Clang extensions that help the compiler
 * generate better code and catch potential bugs at compile time.
 *
 * @author clib
 * @date 2026
 */

#ifndef __CLIB_COMPILER_H__
#define __CLIB_COMPILER_H__

/**
 * @defgroup inlining Inlining Control Attributes
 * @brief Attributes controlling function inlining behavior
 * @{
 */

/**
 * @def __always_inline
 * @brief Forces the compiler to always inline the function
 *
 * Use this for small, performance-critical functions that should always be
 * inlined regardless of optimization level. Overrides compiler heuristics.
 *
 * @warning Overuse can increase code size and compilation time
 *
 * Example:
 * @code
 * static __always_inline int add(int a, int b) {
 *     return a + b;
 * }
 * @endcode
 */
#ifndef __always_inline
#define __always_inline inline __attribute__((__always_inline__))
#endif

/**
 * @def __noinline
 * @brief Prevents the compiler from inlining the function
 *
 * Useful for:
 * - Debugging (easier to see in stack traces)
 * - Reducing code size for large functions
 * - Ensuring function address is unique
 *
 * Example:
 * @code
 * __noinline void error_handler(void) {
 *     // Complex error handling code
 * }
 * @endcode
 */
#ifndef __noinline
#define __noinline __attribute__((__noinline__))
#endif

/** @} */ // end of inlining group

/**
 * @defgroup usage Usage and Visibility Attributes
 * @brief Attributes controlling symbol usage and visibility
 * @{
 */

/**
 * @def __unused
 * @brief Suppresses warnings for unused variables/functions/parameters
 *
 * Tells the compiler that an entity is intentionally unused. Prevents
 * -Wunused-* warnings.
 *
 * Example:
 * @code
 * void callback(__unused void *user_data) {
 *     // user_data intentionally not used in this callback
 * }
 * @endcode
 */
#ifndef __unused
#define __unused __attribute__((__unused__))
#endif

/**
 * @def __used
 * @brief Forces the compiler to emit the symbol even if it appears unused
 *
 * Prevents the linker from removing apparently unused functions/variables.
 * Useful for functions called only from assembly or through function pointers.
 *
 * Example:
 * @code
 * __used static void debug_helper(void) {
 *     // Called from debugger, keep it even if "unused"
 * }
 * @endcode
 */
#ifndef __used
#define __used __attribute__((__used__))
#endif

/**
 * @def __maybe_unused
 * @brief Alias for __unused, indicates the entity may or may not be used
 *
 * More semantically clear name for conditionally compiled code.
 *
 * Example:
 * @code
 * __maybe_unused static int debug_flag = 1; // Only used in debug builds
 * @endcode
 */
#ifndef __maybe_unused
#define __maybe_unused __attribute__((__unused__))
#endif

/** @} */ // end of usage group

/**
 * @defgroup layout Memory Layout Attributes
 * @brief Attributes controlling data structure layout and alignment
 * @{
 */

/**
 * @def __packed
 * @brief Removes padding from structures, packing fields tightly
 *
 * Forces the compiler to pack structure members without padding bytes.
 * Useful for binary protocol structures and hardware register layouts.
 *
 * @warning May cause alignment issues and performance degradation on some
 * architectures
 *
 * Example:
 * @code
 * struct __packed protocol_header {
 *     u8 version;
 *     u32 length;  // No padding between u8 and u32
 *     u16 checksum;
 * };
 * @endcode
 */
#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

/**
 * @def __aligned(x)
 * @brief Forces the variable/structure to be aligned to x bytes
 *
 * Ensures data is aligned to specified boundary. Important for:
 * - SIMD operations (16, 32, or 64 byte alignment)
 * - DMA buffers
 * - Cache line alignment (typically 64 bytes)
 *
 * @param x Alignment in bytes (must be power of 2)
 *
 * Example:
 * @code
 * __aligned(64) static char cache_aligned_buffer[1024];
 * @endcode
 */
#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

/** @} */ // end of layout group

/**
 * @defgroup control Control Flow Attributes
 * @brief Attributes for control flow and function behavior
 * @{
 */

/**
 * @def __noreturn
 * @brief Indicates the function never returns to its caller
 *
 * Used for functions that terminate the program (exit, abort) or
 * transfer control elsewhere (longjmp, throw). Helps compiler optimize
 * and eliminates "control reaches end of non-void function" warnings.
 *
 * Example:
 * @code
 * __noreturn void panic(const char *msg) {
 *     fprintf(stderr, "PANIC: %s\n", msg);
 *     abort();
 * }
 * @endcode
 */
#ifndef __noreturn
#define __noreturn __attribute__((__noreturn__))
#endif

/**
 * @def __fallthrough
 * @brief Suppresses warnings for intentional switch case fallthrough
 *
 * Documents that falling through from one case to another is intentional.
 * Silences -Wimplicit-fallthrough warnings.
 *
 * Example:
 * @code
 * switch (value) {
 *     case 1:
 *         do_something();
 *         __fallthrough;
 *     case 2:
 *         do_something_else();
 *         break;
 * }
 * @endcode
 */
#ifndef __fallthrough
#define __fallthrough __attribute__((__fallthrough__))
#endif

/** @} */ // end of control group

/**
 * @defgroup purity Function Purity Attributes
 * @brief Attributes describing function side effects for optimization
 * @{
 */

/**
 * @def __pure
 * @brief Function has no side effects and return value depends only on
 * parameters and globals
 *
 * A pure function:
 * - Does not modify any global state
 * - Does not perform I/O
 * - Return value depends only on parameters and global variables
 * - Can read (but not write) global memory
 *
 * Allows compiler to eliminate redundant calls with same arguments if
 * no intervening code modifies relevant globals.
 *
 * @note Less strict than __const
 *
 * Example:
 * @code
 * __pure int string_length(const char *s) {
 *     return strlen(s); // Only depends on parameter
 * }
 * @endcode
 */
#ifndef __pure
#define __pure __attribute__((__pure__))
#endif

/**
 * @def __const
 * @brief Function has no side effects and return value depends only on
 * parameters
 *
 * A const function:
 * - Does not read or write any global memory (including pointers)
 * - Does not perform I/O
 * - Return value depends ONLY on parameter values
 * - Stricter than __pure
 *
 * Enables aggressive optimization - compiler can CSE across arbitrary code.
 *
 * @warning Do not use if function dereferences pointers or accesses globals
 *
 * Example:
 * @code
 * __const int add(int a, int b) {
 *     return a + b; // Pure computation, no memory access
 * }
 * @endcode
 */
#ifndef __const
#define __const __attribute__((__const__))
#endif

/** @} */ // end of purity group

/**
 * @defgroup memory Memory Management Attributes
 * @brief Attributes related to memory allocation and management
 * @{
 */

/**
 * @def __malloc
 * @brief Indicates function returns newly allocated memory
 *
 * Tells compiler that:
 * - Returned pointer does not alias any existing pointer
 * - Returned memory is uninitialized
 * - Enables better alias analysis and optimization
 *
 * Example:
 * @code
 * __malloc void *my_allocator(size_t size) {
 *     return malloc(size);
 * }
 * @endcode
 */
#ifndef __malloc
#define __malloc __attribute__((__malloc__))
#endif

/**
 * @def __cleanup(func)
 * @brief Automatically calls cleanup function when variable goes out of scope
 *
 * Enables RAII-style automatic resource management in C. The cleanup
 * function is called with a pointer to the variable when it goes out of scope.
 *
 * @param func Cleanup function taking void** or appropriate pointer type
 *
 * Example:
 * @code
 * void cleanup_fd(int *fd) {
 *     if (*fd >= 0) close(*fd);
 * }
 *
 * void example(void) {
 *     __cleanup(cleanup_fd) int fd = open("file.txt", O_RDONLY);
 *     // fd automatically closed when function returns
 * }
 * @endcode
 */
#ifndef __cleanup
#define __cleanup(func) __attribute__((__cleanup__(func)))
#endif

/** @} */ // end of memory group

/**
 * @defgroup diagnostics Diagnostic and Checking Attributes
 * @brief Attributes for static analysis and runtime checks
 * @{
 */

/**
 * @def __must_check
 * @brief Forces caller to check the return value
 *
 * Generates a warning if the return value is not used. Essential for
 * functions where ignoring the return value would be an error:
 * - Error codes
 * - Resource allocations
 * - Partial I/O operations
 *
 * Example:
 * @code
 * __must_check int write_data(int fd, const void *buf, size_t len) {
 *     return write(fd, buf, len); // Must check if all data was written
 * }
 * @endcode
 */
#ifndef __must_check
#define __must_check __attribute__((__warn_unused_result__))
#endif

/**
 * @def __nonnull(...)
 * @brief Specifies which pointer parameters must not be NULL
 *
 * Enables compile-time and runtime NULL pointer checking. Can list
 * specific parameter indices (1-based) or leave empty for all pointers.
 *
 * @param ... Optional list of 1-based parameter indices
 *
 * Example:
 * @code
 * // Both parameters must be non-NULL
 * __nonnull(1, 2) void strcpy_safe(char *dst, const char *src);
 *
 * // All pointer parameters must be non-NULL
 * __nonnull() void process_data(void *buf, const char *name);
 * @endcode
 *
 * Note: On most systems, __nonnull is already defined by <sys/cdefs.h>.
 * We only define it if it's not already available.
 */
#ifndef __nonnull
#define __nonnull(params) __attribute__((__nonnull__ params))
#endif

/**
 * @def __printf(a, b)
 * @brief Enables printf-style format string checking
 *
 * Validates format string against arguments at compile time.
 * Catches type mismatches and missing arguments.
 *
 * @param a Format string parameter position (1-based)
 * @param b First variadic argument position (1-based)
 *
 * Example:
 * @code
 * __printf(1, 2) void log_message(const char *fmt, ...);
 * // log_message("%d %s", 42, "test"); // OK
 * // log_message("%d %s", "bad", 42);  // Compile error
 * @endcode
 */
#ifndef __printf
#define __printf(a, b) __attribute__((__format__(printf, a, b)))
#endif

/**
 * @def __scanf(a, b)
 * @brief Enables scanf-style format string checking
 *
 * Similar to __printf but for scanf-family functions.
 *
 * @param a Format string parameter position (1-based)
 * @param b First variadic argument position (1-based)
 *
 * Example:
 * @code
 * __scanf(1, 2) int my_scanf(const char *fmt, ...);
 * @endcode
 */
#ifndef __scanf
#define __scanf(a, b) __attribute__((__format__(scanf, a, b)))
#endif

/** @} */ // end of diagnostics group

/**
 * @defgroup optimization Optimization Hints
 * @brief Attributes providing optimization hints to the compiler
 * @{
 */

/**
 * @def __hot
 * @brief Marks function as frequently executed (hot path)
 *
 * Tells compiler this function is called frequently. Compiler will:
 * - Optimize more aggressively
 * - Place in hot code section for better cache locality
 * - Inline more aggressively
 *
 * Example:
 * @code
 * __hot void inner_loop_function(void) {
 *     // Called millions of times
 * }
 * @endcode
 */
#ifndef __hot
#define __hot __attribute__((__hot__))
#endif

/**
 * @def __cold
 * @brief Marks function as rarely executed (cold path)
 *
 * Tells compiler this function is rarely called. Compiler will:
 * - Optimize for size over speed
 * - Place in cold code section
 * - Avoid inlining
 * - Optimize calling context for "not taken" branch
 *
 * Example:
 * @code
 * __cold void error_handler(const char *msg) {
 *     // Only called on errors
 * }
 * @endcode
 */
#ifndef __cold
#define __cold __attribute__((__cold__))
#endif

/**
 * @def __likely(x)
 * @brief Hints that condition is likely to be true
 *
 * Helps compiler optimize branch prediction. Use for conditions that
 * are true in the vast majority of cases (>90%).
 *
 * @param x Boolean expression
 * @return The value of x
 *
 * Example:
 * @code
 * if (__likely(ptr != NULL)) {
 *     // Normal case - optimized as fall-through
 * } else {
 *     // Rare case - may be moved out of line
 * }
 * @endcode
 */
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

/**
 * @def __unlikely(x)
 * @brief Hints that condition is likely to be false
 *
 * Helps compiler optimize branch prediction. Use for error conditions
 * and rare cases.
 *
 * @param x Boolean expression
 * @return The value of x
 *
 * Example:
 * @code
 * if (__unlikely(error_occurred)) {
 *     // Error handling - moved out of hot path
 *     handle_error();
 * }
 * @endcode
 */
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

/**
 * @def unreachable()
 * @brief Marks code as unreachable
 *
 * Tells compiler that this point in the code can never be reached.
 * Enables optimizations and eliminates warnings. Reaching this point
 * is undefined behavior.
 *
 * @warning If control actually reaches this point, behavior is undefined
 *
 * Example:
 * @code
 * switch (value) {
 *     case A: return handle_a();
 *     case B: return handle_b();
 *     default: unreachable(); // All cases covered
 * }
 * @endcode
 */
#ifndef unreachable
#define unreachable() __builtin_unreachable()
#endif

/**
 * @def offsetof(t, d)
 * @brief Calculates the byte offset of a member within a struct type
 *
 * Returns the offset in bytes from the beginning of the struct type to
 * the specified member. This is useful for:
 * - Low-level memory manipulation
 * - Implementing container_of() macros
 * - Working with binary data structures
 *
 * @param t Struct type name
 * @param d Member name within the struct
 * @return Byte offset of the member within the struct
 *
 * @note Uses __builtin_offsetof for compiler-specific implementation
 *
 * Example:
 * @code
 * struct header {
 *     u32 magic;
 *     u16 version;
 *     u32 crc;
 * };
 *
 * size_t version_offset = offsetof(struct header, version); // Returns 4
 * size_t crc_offset = offsetof(struct header, crc);         // Returns 8
 * @endcode
 */
#ifndef offsetof
#define offsetof(t, d) __builtin_offsetof(t, d)
#endif

/** @} */ // end of optimization group

/**
 * @defgroup sections Section Placement Attributes
 * @brief Attributes for controlling object file section placement
 * @{
 */

/**
 * @def __section(section)
 * @brief Places function/variable in specified linker section
 *
 * Allows precise control over memory layout through linker scripts.
 *
 * @param section Section name string
 *
 * Example:
 * @code
 * __section(".fast_ram") int critical_data[100];
 * @endcode
 */
#ifndef __section
#define __section(section) __attribute__((__section__(section)))
#endif

/**
 * @def __weak
 * @brief Marks symbol as weak (can be overridden by strong symbol)
 *
 * Useful for:
 * - Providing default implementations that can be overridden
 * - Optional hooks/callbacks
 * - Platform-specific function variants
 *
 * Example:
 * @code
 * __weak void platform_init(void) {
 *     // Default implementation - can be overridden
 * }
 * @endcode
 */
#ifndef __weak
#define __weak __attribute__((__weak__))
#endif

/**
 * @def __init
 * @brief Places function in initialization section
 *
 * For functions only called during initialization. May be discarded
 * after initialization to save memory (kernel-style usage).
 */
#ifndef __init
#define __init __attribute__((__section__(".init.text")))
#endif

/**
 * @def __initdata
 * @brief Places data in initialization data section
 *
 * For data only used during initialization. May be discarded after
 * initialization to save memory.
 */
#ifndef __initdata
#define __initdata __attribute__((__section__(".init.data")))
#endif

/**
 * @def __text
 * @brief Explicitly places function in .text section
 *
 * Forces function into standard code section.
 */
#ifndef __text
#define __text __attribute__((__section__(".text")))
#endif

/**
 * @def __rodata
 * @brief Places data in read-only data section
 *
 * For constant data that should be in read-only memory.
 */
#ifndef __rodata
#define __rodata __attribute__((__section__(".rodata")))
#endif

/** @} */ // end of sections group

#endif
