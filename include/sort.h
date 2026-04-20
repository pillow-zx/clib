#ifndef CLIB_SORTH_H
#define CLIB_SORTH_H

#include <types.h>
#include <tools.h>

#define insert_sort_asc(arr)                                                   \
        do {                                                                   \
                typeof(&(arr)[0]) _a = &(arr)[0];                              \
                usize _n = ARRLEN(arr);                                        \
                for (usize _i = 1; _i < _n; _i++) {                            \
                        typeof((_a)[0]) _key = (_a)[_i];                       \
                        isize _j = (isize)_i - 1;                              \
                        while (_j >= 0 && (_a)[_j] > _key) {                   \
                                (_a)[_j + 1] = (_a)[_j];                       \
                                _j--;                                          \
                        }                                                      \
                        (_a)[_j + 1] = _key;                                   \
                }                                                              \
        } while (0)

#define insert_sort_desc(arr)                                                  \
        do {                                                                   \
                typeof(&(arr)[0]) _a = &(arr)[0];                              \
                usize _n = ARRLEN(arr);                                        \
                for (usize _i = 1; _i < _n; _i++) {                            \
                        typeof((_a)[0]) _key = (_a)[_i];                       \
                        isize _j = (isize)_i - 1;                              \
                        while (_j >= 0 && (_a)[_j] < _key) {                   \
                                (_a)[_j + 1] = (_a)[_j];                       \
                                _j--;                                          \
                        }                                                      \
                        (_a)[_j + 1] = _key;                                   \
                }                                                              \
        } while (0)

#define insert_sort(arr, asc)                                                  \
        do {                                                                   \
                if (asc)                                                       \
                        insert_sort_asc(arr);                                  \
                else                                                           \
                        insert_sort_desc(arr);                                 \
        } while (0)

#define bubble_sort_asc(arr)                                                   \
        do {                                                                   \
                typeof(&(arr)[0]) _a = &(arr)[0];                              \
                usize _n = ARRLEN(arr);                                        \
                for (usize _i = 0; _i + 1 < _n; _i++) {                        \
                        bool _swap = false;                                    \
                        for (usize _j = 0; _j < _n - _i - 1; _j++) {           \
                                if ((_a)[_j] > (_a)[_j + 1]) {                 \
                                        typeof((_a)[0]) _t = (_a)[_j];         \
                                        (_a)[_j] = (_a)[_j + 1];               \
                                        (_a)[_j + 1] = _t;                     \
                                        _swap = true;                          \
                                }                                              \
                        }                                                      \
                        if (!_swap)                                            \
                                break;                                         \
                }                                                              \
        } while (0)

#define bubble_sort_desc(arr)                                                  \
        do {                                                                   \
                typeof(&(arr)[0]) _a = &(arr)[0];                              \
                usize _n = ARRLEN(arr);                                        \
                for (usize _i = 0; _i + 1 < _n; _i++) {                        \
                        bool _swap = false;                                    \
                        for (usize _j = 0; _j < _n - _i - 1; _j++) {           \
                                if ((_a)[_j] < (_a)[_j + 1]) {                 \
                                        typeof((_a)[0]) _t = (_a)[_j];         \
                                        (_a)[_j] = (_a)[_j + 1];               \
                                        (_a)[_j + 1] = _t;                     \
                                        _swap = true;                          \
                                }                                              \
                        }                                                      \
                        if (!_swap)                                            \
                                break;                                         \
                }                                                              \
        } while (0)

#define bubble_sort(arr, asc)                                                  \
        do {                                                                   \
                if (asc)                                                       \
                        bubble_sort_asc(arr);                                  \
                else                                                           \
                        bubble_sort_desc(arr);                                 \
        } while (0)

#define select_sort_asc(arr)                                                   \
        do {                                                                   \
                typeof(&(arr)[0]) _a = &(arr)[0];                              \
                usize _n = ARRLEN(arr);                                        \
                for (usize _i = 0; _i + 1 < _n; _i++) {                        \
                        usize _min = _i;                                       \
                        for (usize _j = _i + 1; _j < _n; _j++)                 \
                                if ((_a)[_j] < (_a)[_min])                     \
                                        _min = _j;                             \
                        if (_min != _i) {                                      \
                                typeof((_a)[0]) _t = (_a)[_i];                 \
                                (_a)[_i] = (_a)[_min];                         \
                                (_a)[_min] = _t;                               \
                        }                                                      \
                }                                                              \
        } while (0)

#define select_sort_desc(arr)                                                  \
        do {                                                                   \
                typeof(&(arr)[0]) _a = &(arr)[0];                              \
                usize _n = ARRLEN(arr);                                        \
                for (usize _i = 0; _i + 1 < _n; _i++) {                        \
                        usize _max = _i;                                       \
                        for (usize _j = _i + 1; _j < _n; _j++)                 \
                                if ((_a)[_j] > (_a)[_max])                     \
                                        _max = _j;                             \
                        if (_max != _i) {                                      \
                                typeof((_a)[0]) _t = (_a)[_i];                 \
                                (_a)[_i] = (_a)[_max];                         \
                                (_a)[_max] = _t;                               \
                        }                                                      \
                }                                                              \
        } while (0)

#define select_sort(arr, asc)                                                  \
        do {                                                                   \
                if (asc)                                                       \
                        select_sort_asc(arr);                                  \
                else                                                           \
                        select_sort_desc(arr);                                 \
        } while (0)

#endif /* ifndef CLIB_SORTH_H */
