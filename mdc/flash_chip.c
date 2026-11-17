#include "boot.h"

/* Flash chip erase. */
int flash_erase_chip(void)
{
    unsigned long unlock_1;
    unsigned long unlock_2;

    if (flash_wait_ready() == 1)
        return 1;
    unlock_1 = 0x100000;
    unlock_1 += 0xaaaa;
    unlock_2 = 0x100000;
    unlock_2 += 0x5554;
    *(unsigned short *)unlock_1 = 0xaa;
    *(unsigned short *)unlock_2 = 0x55;
    *(unsigned short *)unlock_1 = 0x80;
    *(unsigned short *)unlock_1 = 0xaa;
    *(unsigned short *)unlock_2 = 0x55;
    *(unsigned short *)unlock_1 = 0x10;
    if (flash_wait_ready() == 1)
        return 1;
    else
        return 0;
}
