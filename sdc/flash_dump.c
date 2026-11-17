#include "fw.h"
#pragma section H


int flash_erase_work(void)
{
    int pad;

    flash_init_cmd_addrs();
    flash_status.v = flash_erase_data_sector();
    if (flash_status.v != 0) {
        return 0;
    }
    return 1;
}

unsigned short flash_erase_dump_sector(void)
{
    int im;

    im = _builtin_get_imask();
    return flash_erase_dump_sector_raw();
}

void flash_save_dump(void)
{
    unsigned short *p;
    unsigned short *source;
    unsigned short i;
    int pad;

    flash_init_cmd_addrs();
    flash_status.v = flash_erase_dump_sector();
    if (flash_status.v) {
        return;
    }
    flash_dst = (unsigned char *)0x878000, flash_src = power_dump_buf, flash_len.v = 0x2000;
    flash_program_range();
    i = 0;
    p = (unsigned short *)0x878000;
    source = (unsigned short *)power_dump_buf;
    for (; i < 0x2000; i++, p++, source++) {
        if (*p != *source) {
            flash_status.v = 0x83;
            break;
        }
    }
    if (flash_status.v != 0) {
        return;
    }
}
