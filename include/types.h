/**
 * @file types.h
 * @brief Portable type definitions
 *
 * This header provides fixed-width and pointer-sized integer types that work
 * across different platforms and compilers without libc dependency.
 *
 * @author clib
 * @date 2024
 */

#ifndef __CLIB_TYPES_H__
#define __CLIB_TYPES_H__

/* ===========================================================================
 * C23 Compatibility
 * ===========================================================================
 * In C23, 'auto' has new semantics for type deduction, and 'bool'/'true'/
 * 'false' are keywords. For older standards, provide compatibility macros.
 * ===========================================================================
 */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ < 202311L
#define auto __auto_type
typedef _Bool bool;
enum { false = 0, true = 1 };
#endif

/* ===========================================================================
 * Fixed-Width Integer Types
 * ===========================================================================
 * These types have exactly the specified width on all platforms.
 * Guaranteed by the C standard for the underlying types used here.
 * ===========================================================================
 */

/* Unsigned fixed-width types */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

/* Signed fixed-width types */
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

/* ===========================================================================
 * Floating-Point Types
 * ===========================================================================
 */

typedef float f32;
typedef double f64;
/* ===========================================================================
 * Pointer-Sized Integer Types
 * ===========================================================================
 * These types match the size of pointers on the target platform.
 * Uses compiler builtins for maximum portability (no libc dependency).
 *
 * Platform examples:
 *   - Linux/macOS 64-bit (LP64):  usize = 8 bytes, u64 = 8 bytes
 *   - Windows 64-bit (LLP64):     usize = 8 bytes, u64 = 8 bytes
 *   - 32-bit platforms:           usize = 4 bytes, u32 = 4 bytes
 * ===========================================================================
 */

/**
 * @brief Unsigned type matching pointer size
 * @note Equivalent to size_t, suitable for array indexing and sizes
 */
typedef __SIZE_TYPE__ usize;

/**
 * @brief Signed type matching pointer size
 * @note Equivalent to ptrdiff_t, suitable for pointer differences
 */
typedef __PTRDIFF_TYPE__ isize;

/* ===========================================================================
 * Convenience Aliases
 * ===========================================================================
 */

typedef unsigned char uchar;

/* Nullptr compatibility for pre-C23 */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#define nullptr ((void *)0)
#endif

#endif /* __CLIB_TYPES_H__ */
