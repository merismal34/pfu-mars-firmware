/* The flash file: hi code (section PH) with the third set's flash routines
   (`#pragma section 3`, section P3) between flash_reset_read_mode and
   flash_mdc_run_download: flash_init_cmd_addrs sets up the flash command pointers flash_cmd_addr1..flash_hi_cmd_addr3,
   which only the flash routines read; flash_erase_data_sector / flash_program_range call flash_erase_settings_sector /
   flash_write_16 (flash_erase_dump_sector_raw is called from flash_dump.c); flash_reset_read_mode writes the reset command. */
#include "fw.h"

#pragma section H
/* Constants (section CH). */
const unsigned flash_onchip_block_start[1] = {0};
const void *const flash_onchip_block_end[15] = {
    (const void *)0x1000,
    (const void *)0x2000,
    (const void *)0x3000,
    (const void *)0x4000,
    (const void *)0x5000,
    (const void *)0x6000,
    (const void *)0x7000,
    (const void *)0x8000,
    (const void *)0x10000,
    (const void *)0x18000,
    (const void *)0x20000,
    (const void *)0x28000,
    (const void *)0x30000,
    (const void *)0x38000,
    (const void *)0x40000
};
const unsigned flash_ext_sector_start[1] = {0x800000};
const void *const flash_ext_sector_end[18] = {
    (const void *)0x808000,
    (const void *)0x810000,
    (const void *)0x818000,
    (const void *)0x820000,
    (const void *)0x828000,
    (const void *)0x830000,
    (const void *)0x838000,
    (const void *)0x840000,
    (const void *)0x848000,
    (const void *)0x850000,
    (const void *)0x858000,
    (const void *)0x860000,
    (const void *)0x868000,
    (const void *)0x870000,
    (const void *)0x878000,
    (const void *)0x87a000,
    (const void *)0x87c000,
    (const void *)0x80000
};
/* 8 records of 6 bytes. */
const struct flash_mdc_block_entry flash_mdc_block_table[8] = {
    {0x00, 0x00, 0x00, 0x00, 0x20, 0x80}, {0x00, 0x00, 0x80, 0x00, 0x20, 0x80}, {0x00, 0x01, 0x00, 0x00, 0x20, 0x00}, {0x00, 0x01, 0x80, 0x00, 0x20, 0x00},
    {0x00, 0x02, 0x00, 0x00, 0x20, 0x00}, {0x00, 0x02, 0x80, 0x00, 0x20, 0x00}, {0x00, 0x03, 0x00, 0x00, 0x20, 0x00}, {0x00, 0x03, 0x80, 0x00, 0x20, 0x00}
};
const unsigned flash_mdc_block_addr[8] = {
    0, 0x8000, 0x10000, 0x18000, 0x20000, 0x28000, 0x30000, 0x38000
};

void flash_init_cmd_addrs(void)
{
    flash_cmd_addrs_set = 1;
    flash_status.v = 0;
    flash_cmd_addr1 = (unsigned short *)0x800aaa;
    flash_cmd_addr2 = (unsigned short *)0x800554;
    flash_cmd_addr3 = (unsigned short *)0x800aaa;
    flash_hi_cmd_addr1 = (unsigned short *)0x800aaa;
    flash_hi_cmd_addr2 = (unsigned short *)0x800554;
    flash_hi_cmd_addr3 = (unsigned short *)0x800aaa;
}

struct flash_block_record {
    unsigned char pad_00[4];
    unsigned char address[4];
    unsigned char size_kb, attribute;
    unsigned char x[6];
};
/* FB 81 reply: 0, 32, the information block at 0x87A000, then 32 block records {VL, SM, start,
   size in KB, attribute}: 15 on-chip blocks (flash_onchip_block_start), 17 external sectors (flash_ext_sector_start). */
void flash_build_block_table(unsigned char *p)
{
    int i = 0;
    unsigned char *addr_bytes;
    const unsigned char *start_bytes;

    *p++ = 0;
    *p++ = 0x20;
    util_mem_copy(p, (void *)0x87a000, 0x200);
    for (i = 0; i < 0x20; i++, p += 16) {
        if (i < 15) {
            ((struct flash_block_record *)p)->size_kb = ((unsigned)flash_onchip_block_end[i] - flash_onchip_block_start[i]) >> 10;
            start_bytes = (const unsigned char *)&flash_onchip_block_start[i];
        } else {
            ((struct flash_block_record *)p)->size_kb = ((unsigned)flash_ext_sector_end[i - 15] - flash_ext_sector_start[i - 15]) >> 10;
            start_bytes = (const unsigned char *)&flash_ext_sector_start[i - 15];
        }
        *(addr_bytes = ((struct flash_block_record *)p)->address) = start_bytes[0];
        addr_bytes[1] = start_bytes[1];
        addr_bytes[2] = start_bytes[2];
        addr_bytes[3] = start_bytes[3];
        if (i == 0x1f || i == 0x1e) {
            p[9] = 0xff;
        } else if (i >= 0 && i <= 8) {
            p[0] = *(unsigned char *)0x40e;   /* the boot header's block version (section HEADER + 0xe, + 0xf of the other link): numbers that must follow the link */
            p[1] = *(unsigned char *)0x40f;
            p[9] = 0x80;
        } else {
            p[9] = 0;
        }
        util_mem_clear(p + 10, 6);
    }
}

unsigned short flash_erase_data_sector(void)
{
    int im;

    im = _builtin_get_imask();
    return flash_erase_settings_sector();
}

unsigned short flash_program_range(void)
{
    int saved_imask;
    unsigned short status;

    saved_imask = _builtin_get_imask();
    status = 0;
    _builtin_set_imask(15);
    while (flash_len.v && (flash_len.v & 0x80000000) == 0) {
        if (flash_dst < (unsigned char *)0x878000 || flash_dst >= (unsigned char *)0x880000) {
            flash_status.v = 0x84;
            break;
        }
        status = flash_write_16((unsigned short *)flash_dst, (unsigned short *)flash_src);
        if (flash_status.v) {
            break;
        }
        flash_dst += 0x20;
        flash_src += 0x20;
        flash_len.v -= 32;
    }
    _builtin_set_imask(saved_imask);
    return status;
}

void flash_reset_read_mode(void)
{
    *flash_cmd_addr3 = 0xf0;
}
#pragma section

#pragma section 3
unsigned short flash_erase_settings_sector(void)
{
    unsigned short *sector;
    int i;
    unsigned short result;
    unsigned short status;
    int saved_imask;

    result = 0;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    /* PE6 high: external flash ready */
    while (!hw_port_e->DRL.BIT.B6)
        ;
    *flash_cmd_addr1 = 0xaa;
    *flash_cmd_addr2 = 0x55;
    *flash_cmd_addr3 = 0x80;
    *flash_hi_cmd_addr1 = 0xaa;
    *flash_hi_cmd_addr2 = 0x55;
    sector = (unsigned short *)0x87c000;
    *sector = 48;
    for (;;) {
        status = *sector;
        if (status & 128)
            break;
        if (!(status & 32))
            continue;
        status = *sector;
        for (i = 0; i < 50; i++) {
            if (!(status & 128))
                result = 130;
            else {
                result = 0;
                break;
            }
        }
        break;
    }
    _builtin_set_imask(saved_imask);
    return result;
}

unsigned short flash_write_16(unsigned short *dst, unsigned short *src)
{
    int i;
    int j;
    unsigned short result;
    unsigned short diff;

    result = 0;
    for (i = 0; i < 16 && result == 0; i++, dst++, src++) {
        while (!hw_port_e->DRL.BIT.B6) {
            i++;
            i--;
        }
        *flash_cmd_addr1 = 0xaa;
        *flash_cmd_addr2 = 0x55;
        *flash_cmd_addr3 = 0xa0;
        *dst = *src;
        while (!hw_port_e->DRL.BIT.B6) {
            i++;
            i--;
        }
        for (;;) {
            diff = *dst;
            diff ^= *src;
            if ((diff & 128) == 0)
                break;
            if ((diff & 32) == 0)
                continue;
            for (j = 0; j < 50; j++) {
                diff = *dst;
                diff ^= *src;
                if (diff & 128)
                    result = 131;
                else {
                    result = 0;
                    break;
                }
            }
            break;
        }
    }
    return result;
}

unsigned short flash_erase_dump_sector_raw(void)
{
    unsigned short *sector;
    int i;
    unsigned short result;
    unsigned short status;
    int saved_imask;

    result = 0;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    while (!hw_port_e->DRL.BIT.B6)
        ;
    *flash_cmd_addr1 = 0xaa;
    *flash_cmd_addr2 = 0x55;
    *flash_cmd_addr3 = 0x80;
    *flash_hi_cmd_addr1 = 0xaa;
    *flash_hi_cmd_addr2 = 0x55;
    sector = (unsigned short *)0x878000;
    *sector = 48;
    for (;;) {
        status = *sector;
        if (status & 128)
            break;
        if (!(status & 32))
            continue;
        status = *sector;
        for (i = 0; i < 50; i++) {
            if (!(status & 128))
                result = 130;
            else {
                result = 0;
                break;
            }
        }
        break;
    }
    _builtin_set_imask(saved_imask);
    return result;
}
#pragma section

#pragma section H
struct flash_download_target_view {
    char pad_00[5];
    unsigned char block_count;
    unsigned char st;
    unsigned char idx;
    int pad_08;
    unsigned write_len;
    int pad_10;
    unsigned read_len;
    char tail[0xfc - 24];
};

/* Runs the state that the FC/FE block parser (scsi_cmd_send_diagnostic) left in flash_download_targets[flash_download_session & 3], through the
   MDC mailbox: command byte 0xC40053, length 0xC40055/57, address 0xC40059..5F, data at the odd
   bytes from 0xC60001; the MDC clears the command byte when done (5000 ticks at most). */
void flash_mdc_run_download(void)
{
    int im;
    struct flash_download_target_view *target;
    unsigned len;
    int mdc_offset;
    int off;
    int i;
    unsigned char saved_win_flags;

    im = _builtin_get_imask();
    target = &flash_download_targets[flash_download_session & 3];
    if (target->st == 2) {
        mdc_run_command(MDC_CMD_CLEAR_ERRORS);
        mdc_run_command(MDC_CMD_DOWNLOAD_START);   /* MDC 0xC0: enter its download loop (0xC1 leaves it) */
    } else if (target->st == 3) {
        saved_win_flags = win_params.flags.v;
        win_params.flags.f.adf = 1;
        afe_select_source();
        win_params.flags.v = saved_win_flags;
        mdc_run_command(MDC_CMD_DOWNLOAD_END);
        selftest_run_power_on();
        flash_download_session &= 0x7f;
    } else if (target->st == 4) {
        mdc_offset = flash_mdc_block_addr[target->idx];
        off = 0;
        target->st = 5;
        while (target->write_len != 0) {
            mdc_mailbox.download_offset_3 = (unsigned short)(mdc_offset >> 16) >> 8;
            mdc_mailbox.download_offset_2 = mdc_offset >> 16;
            mdc_mailbox.download_offset_1 = (unsigned short)mdc_offset >> 8;
            mdc_mailbox.download_offset_0 = mdc_offset;
            if (target->write_len >= 0x400) {
                len = 0x400;
            } else {
                len = target->write_len;
            }
            mdc_mailbox.download_len_high = (unsigned short)len >> 8;
            mdc_mailbox.download_len_low = len;
            util_mem_copy_to_odd(mdc_back_window_data, (char *)((int)flash_download_buf + off + 8), len);
            mdc_mailbox.download_cmd = MDC_DOWNLOAD_WRITE;   /* write the piece to MDC flash offset mdc_offset */
            tick_wait_count = 0;
            while (1) {
                if (mdc_mailbox.download_cmd == 0) {
                    break;
                }
                if (tick_wait_count >= 0x1388) {
                    return;
                }
            }
            target->write_len -= len;
            mdc_offset += len;
            off += len;
        }
    } else if (target->st == 6) {
        len = target->block_count * 4;
        mdc_mailbox.download_len_high = (unsigned short)len >> 8;
        mdc_mailbox.download_len_low = len;
        for (i = 0; i <= target->block_count; i++) {
            mdc_back_window_data[i][0] = (*(char (*)[4][16])&flash_download_buf)[i][0];
            mdc_back_window_data_2[i][0] = flash_download_block_info[i][0];
            mdc_back_window[i * 4 + 33] = flash_download_block_info[i][1];
            mdc_back_window[i * 4 + 35] = flash_download_block_info[i][2];
        }
        mdc_mailbox.download_cmd = MDC_DOWNLOAD_INFO_TABLE;   /* store the information table (VL/SM per block) */
        tick_wait_count = 0;
        while (1) {
            if (mdc_mailbox.download_cmd == 0) {
                break;
            }
            if (tick_wait_count >= 0x1388) {
                return;
            }
        }
    } else if (target->st == 7) {
        mdc_offset = flash_mdc_block_addr[target->idx];
        off = 0;
        target->st = 8;
        while (target->read_len != 0) {
            mdc_mailbox.download_offset_3 = (unsigned short)(mdc_offset >> 16) >> 8;
            mdc_mailbox.download_offset_2 = mdc_offset >> 16;
            mdc_mailbox.download_offset_1 = (unsigned short)mdc_offset >> 8;
            mdc_mailbox.download_offset_0 = mdc_offset;
            if (target->read_len >= 0x400) {
                len = 0x400;
            } else {
                len = target->write_len;
            }
            mdc_mailbox.download_len_high = (unsigned short)len >> 8;
            mdc_mailbox.download_len_low = len;
            mdc_mailbox.download_cmd = MDC_DOWNLOAD_READ;   /* read the piece at MDC flash offset mdc_offset into 0xC60001.. */
            tick_wait_count = 0;
            while (1) {
                if (mdc_mailbox.download_cmd == 0) {
                    break;
                }
                if (tick_wait_count >= 0x1388) {
                    return;
                }
            }
            util_mem_copy_from_odd((int)flash_download_buf + off, (int)mdc_back_window_data, len);
            target->read_len -= len;
            mdc_offset += len;
            off += len;
        }
    } else if (target->st == 1) {
    } else if (target->st == 9) {
        if (power_state.f.saving) {
            flash_download_targets[flash_download_session & 3].mdc_firmware_version[0] = power_image_mdc_version_0[0].b[1];
            flash_download_targets[flash_download_session & 3].mdc_firmware_version[1] = power_image_mdc_version_1[0].w >> 8;
            flash_download_targets[flash_download_session & 3].mdc_firmware_version[2] = power_image_mdc_version_1[0].b[1];
            flash_download_targets[flash_download_session & 3].mdc_firmware_version[3] = power_image_mdc_version_2[0].w >> 8;
        } else {
            flash_download_targets[flash_download_session & 3].mdc_firmware_version[0] = mdc_version[0];
            flash_download_targets[flash_download_session & 3].mdc_firmware_version[1] = mdc_version_1[0];
            flash_download_targets[flash_download_session & 3].mdc_firmware_version[2] = mdc_version_2[0];
            flash_download_targets[flash_download_session & 3].mdc_firmware_version[3] = mdc_version_3[0];
        }
    }
    return;
}

/* FC/FE 81 reply: 0, block count, then per block {VL, SM from the MDC (0xC40013/0xC40033), address,
   size, attribute from flash_mdc_block_table}; also fills flash_download_targets[sel]'s block records used to check later blocks. */
void flash_mdc_build_block_table(unsigned char *p, unsigned char sel)
{
    int i;
    unsigned char versions[40];
    unsigned char sums[40];

    if (power_state.f.saving) {
        util_mem_copy_to_odd(versions, (unsigned char *)power_image_mdc_version_2 + 1, 16);
        util_mem_copy_to_odd(sums, power_image_mdc_block_info, 16);
    } else {
        util_mem_copy(versions, mdc_info_table, 32);
        util_mem_copy(sums, mdc_info_table_sums, 32);
    }
    *p++ = 0;
    *p++ = flash_download_targets[sel].block_count;
    for (i = 0; i <= flash_download_targets[sel].block_count; i++) {
        *p++ = versions[i * 4];
        *p++ = versions[i * 4 + 2];
        flash_download_targets[sel].blocks[i].version = (versions[i * 4 + 2] << 8) | versions[i * 4];
        *p++ = sums[i * 4];
        *p++ = sums[i * 4 + 2];
        flash_download_targets[sel].blocks[i].sum = (sums[i * 4 + 2] << 8) | sums[i * 4];
        *p++ = flash_mdc_block_table[i].b0;
        *p++ = flash_mdc_block_table[i].b1;
        *p++ = flash_mdc_block_table[i].b2;
        *p++ = flash_mdc_block_table[i].b3;
        flash_download_targets[sel].blocks[i].address = (unsigned short)((flash_mdc_block_table[i].b3 << 8) | flash_mdc_block_table[i].b2) << 16 | (unsigned short)((flash_mdc_block_table[i].b1 << 8) | flash_mdc_block_table[i].b0);
        *p++ = flash_mdc_block_table[i].size_kb;
        flash_download_targets[sel].blocks[i].size_kb = flash_mdc_block_table[i].size_kb;
        *p++ = flash_mdc_block_table[i].attribute;
        flash_download_targets[sel].blocks[i].attribute = flash_mdc_block_table[i].attribute;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
    }
}

/* 16 blocks of 0x220 bytes (512 data + 32 header) at 0x87c000. */
struct flash_backup_block { unsigned char saved_settings[0x200]; unsigned char h[0x20]; };

void flash_save_settings_backup(void)
{
    unsigned short i;
    unsigned short j;
    unsigned short *p;
    int flag;

    flag = 1;
    p = flash_defect_area;
    for (i = 0; i < 0x480; i++, p++) {
        if (*p != 0xffff) {
            if (*p != 0) {
                flag = 0;
                break;
            }
        }
    }
    for (j = 0; j < 16; j++) {
        if (((struct flash_backup_block *)flash_backup_area)[j].h[0] == 0xff) {
            break;
        }
        if (((struct flash_backup_block *)flash_backup_area)[j].h[1] == 0xff) {
            flash_status.v = 0x88;
            return;
        }
    }
    flash_init_cmd_addrs();
    if (flag) {
        flash_status.v = flash_erase_data_sector();
        j = 0;
    }
    if (flash_status.v) {
        return;
    }
    flash_dst = &((struct flash_backup_block *)flash_backup_area)[j].saved_settings[0];
    flash_src = settings;
    flash_len.v = 0x200;
    flash_program_range();
    for (i = 0; i < 0x200; i++) {
        if (((struct flash_backup_block *)flash_backup_area)[j].saved_settings[i] != settings[i]) {
            flash_status.v = 0x83;
        }
    }
    if (flash_status.v) {
        return;
    }
    util_mem_set(flash_backup_header, 0xff, 0x20);
    flash_backup_header[0] = 0;
    flash_dst = ((struct flash_backup_block *)flash_backup_area)[j].h;
    flash_src = flash_backup_header;
    flash_len.v = 0x20;
    flash_program_range();
    if (((struct flash_backup_block *)flash_backup_area)[j].h[0] != 0 || ((struct flash_backup_block *)flash_backup_area)[j].h[1] != 0xff) {
        flash_status.v = 0x83;
    }
    settings_byte_0x1e[0] |= 0x80;
    iic_eeprom_save_settings();
}

void flash_save_defect_lists(void)
{
    unsigned short *entry;
    unsigned short index;
    unsigned short list;

    for (list = 0; list < 9; list++) {
        entry = flash_defect_lists + list * 0x80 + 1;
        for (index = 1; index < 0x80; index++, entry++) {
            if (entry[-1] >= *entry && *entry != 0xffff) {
                return;
            }
        }
    }
    util_mem_copy(flash_settings_copy, flash_settings_mirror, 0x200);
    flash_write_defect_area();
}

void flash_save_settings_copy(void)
{
    util_mem_copy(flash_defect_lists, flash_defect_area, 0x900);
    util_mem_copy(flash_settings_copy, settings, 0x200);
    flash_write_defect_area();
}

void flash_write_defect_area(void)
{
    unsigned short *p;
    unsigned short i;

    flash_init_cmd_addrs();
    flash_status.v = flash_erase_data_sector();
    if (1) ;
    if (flash_status.v) {
        return;
    }
    flash_dst = (unsigned char *)flash_defect_area;
    flash_src = (unsigned char *)flash_defect_lists;
    flash_len.v = 0x900;
    flash_program_range();
    for (i = 0, p = flash_defect_area; i < 0x480; i++, p++) {
        if (*p != flash_defect_lists[i]) {
            flash_status.v = 0x83;
            break;
        }
    }
    if (flash_status.v) {
        return;
    }
    flash_dst = flash_settings_mirror;
    flash_src = flash_settings_copy;
    flash_len.v = 0x200;
    flash_program_range();
    for (i = 0; i < 0x200; i++) {
        if (flash_settings_mirror[i] != flash_settings_copy[i]) {
            flash_status.v = 0x83;
        }
    }
}

int flash_restore_settings_backup(void)
{
    unsigned short j;
    int flag;

    flag = 0;
    if (settings_byte_0x1e[0] & 0x80) {
        return 0;
    }
    for (j = 0; j < 16; j++) {
        if (((struct flash_backup_block *)flash_backup_area)[j].h[0] != 0) {
            break;
        }
        if (((struct flash_backup_block *)flash_backup_area)[j].h[1] == 0xff) {
            flag = 1;
            break;
        }
    }
    if (!flag) {
        return 0;
    }
    util_mem_copy(settings, &((struct flash_backup_block *)flash_backup_area)[j].saved_settings[0], 0x200);
    hw_device_errors.f.eeprom = 0;
    iic_eeprom_save_settings();
    if (hw_device_errors.f.eeprom) {
        hw_device_errors.f.eeprom = 0;
        iic_eeprom_save_settings();
        if (hw_device_errors.f.eeprom) {
            return 0;
        }
    }
    flash_init_cmd_addrs();
    util_mem_set(flash_backup_header, 0xff, 0x20);
    flash_backup_header[0] = 0;
    flash_backup_header[1] = 0;
    flash_dst = ((struct flash_backup_block *)flash_backup_area)[j].h;
    flash_src = flash_backup_header;
    flash_len.v = 0x20;
    flash_program_range();
    if (((struct flash_backup_block *)flash_backup_area)[j].h[0] != 0 || ((struct flash_backup_block *)flash_backup_area)[j].h[1] != 0) {
        flash_status.v = 0x83;
    }
    tick_wait_count = 0;
    while (tick_wait_count < 1000) {
    }
    return 1;
}
#pragma section
