#include "fw.h"
#pragma section H


void page_set_stall_limit(int side)
{
    int limit;

    limit = 0x1450;
    page_stall_limit[side].v = limit;
}
