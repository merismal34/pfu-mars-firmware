#include "fw.h"

void util_mem_clear(void *dst, int len)
{
    rt_mem_clear(dst, len);
}

void util_mem_copy(void *dst, void *src, int len)
{
    rt_mem_copy(dst, src, len);
}
