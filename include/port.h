/**
 * @file port.h
 * @brief Platform abstraction layer for memory and I/O operations
 *
 * This header provides an abstraction layer over system libraries, allowing
 * the codebase to be used with libc, custom allocators, or in freestanding
 * environments (e.g., kernels, embedded systems).
 *
 * Usage:
 *   - Memory operation defaults (cmemcpy/cmemmove/cmemset/cmemcmp) are provided
 *     as weak pure-C implementations.
 *   - Define CLIB_USE_LIBC to use default libc-backed allocator and printf.
 *   - Otherwise, provide your own implementations of cmalloc/ccalloc/crealloc/
 *     cfree/cprintf.
 *
 * Visibility and API notes:
 *   - Declarations in this header are portability hooks (integration ABI),
 *     not stable exported library APIs.
 *   - Default fallback definitions in this header are marked __hidden __weak
 *     to avoid leaking symbols from libclib.so while remaining overrideable.
 *
 * @author clib
 * @date 2026
 */

#ifndef CLIB_PORT_H
#define CLIB_PORT_H

#include <types.h>
#include <compiler.h>
#include <stdarg.h>

/**
 * @defgroup memory Memory Management Interface
 * @brief Abstract memory allocation and manipulation hooks
 *
 * These are portability hooks used by modules depending on port.h.
 * They are intentionally declared without __export in this header.
 * Users must provide allocator implementations when not using CLIB_USE_LIBC.
 * All functions follow standard libc semantics.
 * @{
 */

/**
 * @brief Allocate memory block
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
void *cmalloc(const usize size);

/**
 * @brief Allocate and zero-initialize array
 * @param num Number of elements
 * @param size Size of each element
 * @return Pointer to allocated memory, or NULL on failure
 */
void *ccalloc(const usize num, const usize size);

/**
 * @brief Reallocate memory block
 * @param ptr Pointer to previously allocated memory (or NULL)
 * @param size New size in bytes
 * @return Pointer to reallocated memory, or NULL on failure
 */
void *crealloc(void *ptr, const usize size);

/**
 * @brief Free memory block
 * @param ptr Pointer to memory to free (or NULL for no-op)
 */
void cfree(void *ptr);

/**
 * @brief Copy memory region
 * @param dest Destination buffer
 * @param src Source buffer
 * @param size Number of bytes to copy
 * @return Pointer to dest
 *
 * @note Behavior is undefined if regions overlap (use cmemmove for that)
 */
void *cmemcpy(void *dest, const void *src, const usize size);

/**
 * @brief Copy memory region (handles overlapping regions)
 * @param dest Destination buffer
 * @param src Source buffer
 * @param size Number of bytes to copy
 * @return Pointer to dest
 */
void *cmemmove(void *dest, const void *src, const usize size);

/**
 * @brief Set memory region to a value
 * @param dest Pointer to memory region
 * @param value Value to set (converted to unsigned char)
 * @param size Number of bytes to set
 * @return Pointer to dest
 */
void *cmemset(void *dest, const int value, const usize size);

/**
 * @brief Compare two memory regions
 * @param s1 First memory region
 * @param s2 Second memory region
 * @param size Number of bytes to compare
 * @return 0 if equal, <0 if s1 < s2, >0 if s1 > s2
 */
int cmemcmp(const void *s1, const void *s2, const usize size);

/** @} */

/**
 * @defgroup io I/O Interface
 * @brief Abstract I/O functions
 * @{
 */

/**
 * @brief Formatted print to standard output
 * @param fmt Format string
 * @param ... Format arguments
 * @return Number of characters printed, or negative on error
 */
__printf(1, 2) int cprintf(const char *fmt, ...);

/** @} */

/* ===========================================================================
 * Default weak implementation for basic memory operations
 * ===========================================================================
 * These fallbacks are hidden to avoid being treated as public shared-library
 * APIs, and weak so integrators can override them with strong definitions.
 */

__hidden __weak void *cmemcpy(void *dest, const void *src, const usize size)
{
        unsigned char *d = dest;
        const unsigned char *s = src;

        for (usize i = 0; i < size; i++)
                d[i] = s[i];

        return dest;
}

__hidden __weak void *cmemmove(void *dest, const void *src, const usize size)
{
        unsigned char *d = dest;
        const unsigned char *s = src;

        if (d == s || size == 0)
                return dest;

        if (d < s || d >= s + size) {
                for (usize i = 0; i < size; i++)
                        d[i] = s[i];
        } else {
                for (usize i = size; i > 0; i--)
                        d[i - 1] = s[i - 1];
        }

        return dest;
}

__hidden __weak void *cmemset(void *dest, const int value, const usize size)
{
        unsigned char *d = dest;
        const unsigned char v = (unsigned char)value;

        for (usize i = 0; i < size; i++)
                d[i] = v;

        return dest;
}

__hidden __weak int cmemcmp(const void *s1, const void *s2, const usize size)
{
        const unsigned char *a = s1;
        const unsigned char *b = s2;

        for (usize i = 0; i < size; i++) {
                if (a[i] != b[i])
                        return (int)a[i] - (int)b[i];
        }

        return 0;
}

/* ===========================================================================
 * Default libc Implementation
 * ===========================================================================
 * Enable with -DCLIB_USE_LIBC during compilation.
 * In freestanding environments, do NOT define CLIB_USE_LIBC and provide
 * your own implementations of cmalloc/ccalloc/crealloc/cfree/cprintf.
 * ===========================================================================
 */

#ifdef CLIB_USE_LIBC

#include <stdio.h>
#include <stdlib.h>

__hidden __weak void *cmalloc(const usize size)
{
        return malloc(size);
}

__hidden __weak void *ccalloc(const usize num, const usize size)
{
        return calloc(num, size);
}

__hidden __weak void *crealloc(void *ptr, const usize size)
{
        return realloc(ptr, size);
}

__hidden __weak void cfree(void *ptr)
{
        free(ptr);
}

__hidden __weak int cprintf(const char *fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        const int ret = vprintf(fmt, args);
        va_end(args);
        return ret;
}

#endif /* CLIB_USE_LIBC */
#endif /* __CLIB_PORT_H__ */
