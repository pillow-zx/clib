/**
 * @file port.h
 * @brief Platform abstraction layer for memory and I/O operations
 *
 * This header provides an abstraction layer over system libraries, allowing
 * the codebase to be used with libc, custom allocators, or in freestanding
 * environments (e.g., kernels, embedded systems).
 *
 * Usage:
 *   - Define CLIB_USE_LIBC to use the default libc implementation
 *   - Otherwise, provide your own implementations of the required functions
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
 * @brief Abstract memory allocation and manipulation functions
 *
 * Users must provide implementations when not using CLIB_USE_LIBC.
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
 * Default libc Implementation
 * ===========================================================================
 * Enable with -DCLIB_USE_LIBC during compilation.
 * In freestanding environments, do NOT define CLIB_USE_LIBC and provide
 * your own implementations of the above functions.
 * ===========================================================================
 */

#ifdef CLIB_USE_LIBC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__weak void *cmalloc(const usize size)
{
        return malloc(size);
}

__weak void *ccalloc(const usize num, const usize size)
{
        return calloc(num, size);
}

__weak void *crealloc(void *ptr, const usize size)
{
        return realloc(ptr, size);
}

__weak void cfree(void *ptr)
{
        free(ptr);
}

__weak void *cmemcpy(void *dest, const void *src, const usize size)
{
        return memcpy(dest, src, size);
}

__weak void *cmemmove(void *dest, const void *src, const usize size)
{
        return memmove(dest, src, size);
}

__weak void *cmemset(void *dest, const int value, const usize size)
{
        return memset(dest, value, size);
}

__weak int cmemcmp(const void *s1, const void *s2, const usize size)
{
        return memcmp(s1, s2, size);
}

__weak int cprintf(const char *fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        int ret = vprintf(fmt, args);
        va_end(args);
        return ret;
}

#endif /* CLIB_USE_LIBC */
#endif /* __CLIB_PORT_H__ */
