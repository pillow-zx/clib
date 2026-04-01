#ifndef __CLIB_AUTO_FREE__
#define __CLIB_AUTO_FREE__

#include <stdlib.h>
#include <compiler.h>

static __always_inline void auto_free(void *ptr)
{
        void **p = (void **)ptr;
        if (*p)
                free(*p);
}

#define autofree __autofree(auto_free)

#endif
