#ifndef __CLIB_AUTOFREE_H__
#define __CLIB_AUTOFREE_H__

#include <port.h>
#include <compiler.h>

static __always_inline void auto_free(void **ptr)
{
        if (*ptr)
                cfree(*ptr);
}

#define autofree __cleanup(auto_free)

#endif
