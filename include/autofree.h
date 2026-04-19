#ifndef CLIB_AUTOFREE_H
#define CLIB_AUTOFREE_H

#include <port.h>
#include <compiler.h>

static __always_inline void auto_free(void **ptr)
{
        if (*ptr)
                cfree(*ptr);
}

#define autofree __cleanup(auto_free)

#endif
