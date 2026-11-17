#include "boot.h"
#pragma section 2
struct fw_update_block_record {
    unsigned char pad_00[4];
    unsigned char address[4];
    unsigned char size_kb, attribute;
    unsigned char x[6];
};
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);


/* Erases the flash block holding addr: on-chip blocks (fw_update_onchip_blocks, 15) with the FLMCR sequence and
   erase-verify, up to 60 tries; external sectors through fw_update_erase_ext_sector. Returns 0, 0x82 verify failed,
   0x84 no such block. */
short fw_update_erase_block(unsigned int addr)
{
    unsigned int mask;
    unsigned int *p;
    int polls;
    int i;
    int saved_imask;
    short error;
    unsigned char wdt_csr;

    mask = 1;
    polls = 0;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    i = 0;
    while (i < 15) {
        if (addr >= fw_update_onchip_blocks[i] && addr < fw_update_onchip_block_ends[i]) {
            break;
        } else {
            i++;
            mask <<= 1;
        }
    }
    if (mask < 0x8000 && (mask & fw_update_erased_mask)) {
        _builtin_set_imask(saved_imask);
        return 0;
    }
    if (i == 15) {
        i = 0;
        while (i < 16) {
            if (addr >= fw_update_ext_sectors[i] && addr < fw_update_ext_sector_ends[i]) {
                break;
            } else {
                i++;
                mask <<= 1;
            }
        }
        if (mask & fw_update_erased_mask) {
            _builtin_set_imask(saved_imask);
            return 0;
        }
        if (i == 17) {
            _builtin_set_imask(saved_imask);
            return 0x84;
        }
        return fw_update_erase_ext_sector((unsigned short *)fw_update_ext_sectors[i], mask);
    } else {
        fw_update_port_e->lo.b5 = 0;
        fw_update_p_flash->flmcr1.b6 = 1;
        fw_update_delay_20();
        p = (unsigned int *)fw_update_onchip_blocks[i];
        fw_update_p_flash->ebr1 = fw_update_erase_bits[i].ebr1;
        fw_update_p_flash->ebr2 = fw_update_erase_bits[i].ebr2;
        while (1) {
            error = 0;
            fw_update_p_wdt_write[1] = 0x5a40;
            fw_update_p_wdt_write[0] = 0xa57f;   /* WDT as the erase overrun timer */
            fw_update_p_flash->flmcr1.b5 = 1;
            fw_update_delay_386();
            fw_update_p_flash->flmcr1.b1 = 1;
            fw_update_delay_9656();
            fw_update_p_flash->flmcr1.b1 = 0;
            fw_update_delay_20();
            fw_update_p_flash->flmcr1.b5 = 0;
            fw_update_delay_20();
            wdt_csr = *fw_update_p_wdt_read;
            *fw_update_p_wdt_write = (wdt_csr & 0xffdf) | 0xa500;
            fw_update_p_wdt_write[1] = 0x5a00;
            fw_update_p_flash->flmcr1.b3 = 1;
            fw_update_delay_39();
            for (; p < (unsigned int *)*(unsigned int *)(fw_update_onchip_block_ends + i); p++) {
                *p = -1;
                fw_update_delay_4();
                if (*p != 0xffffffff) {
                    polls++;
                    error = 0x82;
                    break;
                }
            }
            fw_update_p_flash->flmcr1.b3 = 0;
            fw_update_delay_10();
            if (error != 0) {
                if (polls >= 60) {
                    break;
                }
                continue;
            }
            fw_update_erased_mask |= mask;
            if (0x80000 == mask || 0x200000 == mask || 0x800000 == mask || 0x2000000 == mask
                || 0x8000000 == mask || 0x20000000 == mask || 0x80000000 == mask) {
                fw_update_erased_mask |= mask << 1;
            } else if (0x100000 == mask || 0x400000 == mask || 0x1000000 == mask || 0x4000000 == mask
                || 0x10000000 == mask || 0x40000000 == mask || 0 == mask) {
                fw_update_erased_mask |= mask >> 1;
            }
            break;
        }
        fw_update_p_flash->flmcr1.b6 = 0;
        fw_update_port_e->lo.b5 = 1;
        _builtin_set_imask(saved_imask);
        if (error == 0) {
            fw_update_erased_mask |= mask;
            if (mask == 0x200 || mask == 0x800 || mask == 0x2000) {
                fw_update_erased_mask |= mask << 1;
            } else if (mask == 0x400 || mask == 0x1000 || mask == 0x4000) {
                fw_update_erased_mask |= mask >> 1;
            }
        }
        return error;
    }
}


/* Programs 128 bytes (on-chip: pulse, verify, re-program data; up to 1000 rounds). Returns 0 or 0x83. */
short fw_update_program_128(unsigned int dst_addr, unsigned int src_addr)
{
    unsigned short pulses;
    unsigned short k;
    unsigned int *p;
    unsigned int *q;
    unsigned int readback;
    int saved_imask;
    char flag;
    unsigned short error;

    flag = 0;
    error = 0;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (dst_addr >= 0x800000) {
        error = fw_update_program_ext((unsigned short *)dst_addr, (unsigned short *)src_addr);
        _builtin_set_imask(saved_imask);
        return error;
    }
    fw_update_mem_copy(fw_update_reprogram_data, (void *)src_addr, 0x80);
    fw_update_port_e->lo.b5 = 0;
    fw_update_p_flash->flmcr1.b6 = 1;
    fw_update_delay_4();
    pulses = 0;
    while (pulses < 1000) {
        if (error == 0) {
            fw_update_mem_copy((void *)dst_addr, (void *)src_addr, 0x80);
        } else if (flag) {
            fw_update_mem_copy((void *)dst_addr, fw_update_additional_data, 0x80);
        } else {
            fw_update_mem_copy((void *)dst_addr, fw_update_reprogram_data, 0x80);
        }
        fw_update_program_pulse(pulses, flag);
        if (flag) {
            break;
        }
        fw_update_p_flash->flmcr1.b2 = 1;
        fw_update_delay_8();
        error = 0;
        k = 0;
        p = (unsigned int *)dst_addr;
        q = (unsigned int *)src_addr;
        for (; k < 32; k++, p++, q++) {
            *p = -1;
            fw_update_delay_4();
            readback = *p;
            if (pulses <= 6) {
                fw_update_additional_data[k] = readback | fw_update_reprogram_data[k];
            }
            if (readback == *q) {
            } else {
                readback ^= *q;
                fw_update_reprogram_data[k] = ~readback | *q;
                error = 0x83;
            }
        }
        fw_update_p_flash->flmcr1.b2 = 0;
        fw_update_delay_8();
        if (error == 0) {
            if (pulses <= 6) {
                flag = 1;
            } else {
                break;
            }
        }
        pulses++;
    }
    fw_update_p_flash->flmcr1.b6 = 0;
    fw_update_port_e->lo.b5 = 1;
    fw_update_delay_193();
    _builtin_set_imask(saved_imask);
    return error;
}

void fw_update_program_pulse(unsigned short pulse, char skip)
{
    unsigned char wdt_csr;

    fw_update_p_wdt_write[1] = 0x5a40;
    fw_update_p_wdt_write[0] = 0xa57c;
    fw_update_p_flash->flmcr1.b4 = 1;
    fw_update_delay_97();
    if (skip) {
        fw_update_p_flash->flmcr1.b0 = 1;
        fw_update_delay_20();
    } else if (pulse < 6) {
        fw_update_p_flash->flmcr1.b0 = 1;
        fw_update_delay_58();
    } else {
        fw_update_p_flash->flmcr1.b0 = 1;
        fw_update_delay_386();
    }
    fw_update_p_flash->flmcr1.b0 = 0;
    fw_update_delay_20();
    fw_update_p_flash->flmcr1.b4 = 0;
    fw_update_delay_20();
    wdt_csr = *fw_update_p_wdt_read;
    *fw_update_p_wdt_write = (wdt_csr & 0xffdf) + 0xa500;
    fw_update_p_wdt_write[1] = 0x5a00;
}

/* SEND DIAGNOSTIC block 0xFB mode 0x10: start of an update session. */
void fw_update_begin_session(void)
{
    fw_update_erased_mask = 0;
    fw_update_write_addr.v = 0;
    fw_update_session_spare_4 = 0;
    fw_update_session_closed.v = 0;
    fw_update_session_active = 1;
    fw_update_status.v = 0;
    fw_update_write_block_index.v = 0;
    fw_update_write_len = 0;
    fw_update_session_spare_1 = 0;
    fw_update_session_spare_2 = 0;
    fw_update_session_spare_3 = 0;
    fw_update_ext_cmd_addr_1 = 0x800aaa;
    fw_update_ext_cmd_addr_2 = 0x800554;
    fw_update_ext_cmd_addr_3 = 0x800aaa;
    fw_update_ext_cmd_addr_4 = 0x800aaa;
    fw_update_ext_cmd_addr_5 = 0x800554;
    fw_update_ext_cmd_addr_6 = 0x800aaa;
}

void fw_update_end_session(void)
{
    if (fw_update_erased_mask == 0) {
        fw_update_status.v = 0x90;
    } else {
        fw_update_session_closed.v = 0xc0;
    }
    fw_update_session_active = 0;
    fw_update_restart_request = 64;
}

void fw_update_report_blocks(unsigned char *p)
{
    int i;
    unsigned char *q;
    unsigned char *r;

    i = 0;
    *p++ = 0;
    *p++ = 32;
    fw_update_mem_copy(p, (void *)0x87a000, 0x200);   /* the flash information block (VL/SM per block), rewritten by mode 0x01 */
    for (i = 0; i < 32; i++, p += 16) {
        if (i < 15) {
            ((struct fw_update_block_record *)p)->size_kb = (fw_update_onchip_block_ends[i] - fw_update_onchip_blocks[i]) >> 10;
            r = (unsigned char *)&fw_update_onchip_blocks[i];
        } else {
            ((struct fw_update_block_record *)p)->size_kb = (fw_update_ext_sector_ends[i - 15] - fw_update_ext_sectors[i - 15]) >> 10;
            r = (unsigned char *)&fw_update_ext_sectors[i - 15];
        }
        *(q = ((struct fw_update_block_record *)p)->address) = *r;
        q[1] = r[1];
        q[2] = r[2];
        q[3] = r[3];
        if (i >= 0 && i <= 8) {
            p[0] = fw_update_boot_version_hi;
            p[1] = fw_update_boot_version_lo;
            p[9] = 0x80;
        } else if (i == 31 || i == 30) {
            p[9] = 0xff;
        } else {
            p[9] = 0;
        }
        fw_update_mem_clear(p + 10, 6);
    }
}

void fw_update_write_info_block(void)
{
    int n;
    unsigned int addr;
    unsigned char *p;
    unsigned short error;

    fw_update_mem_copy(fw_update_info_block, fw_update_info_block_buf, 0x200);
    p = fw_update_info_block;
    addr = 0x87a000;
    fw_update_status.v = fw_update_erase_block(addr);
    if (fw_update_status.v) {
        return;
    }
    for (n = 0; n < 31; n += 8, p += 0x80, addr += 0x80) {
        error = fw_update_program_128(addr, (unsigned int)p);
        if (error != 0) {
            break;
        }
    }
    fw_update_status.v = error;
}

void fw_update_write_block(void)
{
    int x4;
    int x3;
    int x2;
    int x1;
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_status.v = 0;
    fw_update_write_block_index.v = fw_update_block_index.v;
    fw_update_write_len = fw_update_block_len;
    if (fw_update_write_block_index.v < 15) {
        fw_update_write_addr.v = fw_update_onchip_blocks[fw_update_write_block_index.v];
        if (fw_update_write_addr.v == fw_update_onchip_end[0]) {
            fw_update_status.v = 0x90;
            goto end;
        }
    } else {
        fw_update_write_addr.v = fw_update_ext_sectors[fw_update_write_block_index.v - 15];
        if (fw_update_write_addr.v == fw_update_ext_end[0]) {
            fw_update_status.v = 0x90;
            goto end;
        }
    }
    fw_update_write_remaining = fw_update_write_len;
    fw_update_write_src = (unsigned int)fw_update_rx_data;
    fw_update_status.v = fw_update_erase_block(fw_update_write_addr.v);
    if (fw_update_status.v) {
        goto end;
    }
    while (fw_update_write_remaining) {
        fw_update_status.v = fw_update_program_128(fw_update_write_addr.v, fw_update_write_src);
        if (fw_update_status.v) {
            goto end;
        }
        fw_update_write_addr.v += 0x80;
        fw_update_write_remaining -= 0x80;
        fw_update_write_src += 0x80;
    }
    if (fw_update_status.v == 0) {
        if (fw_update_session_closed.v != 0) {
            if (fw_update_session_closed.v & 1) {
                fw_update_status.v = 0x84;
            } else {
                fw_update_status.v = 0x90;
            }
        }
    }
end:
    _builtin_set_imask(saved_imask);
    return;
}

short fw_update_erase_ext_sector(unsigned short *addr, unsigned int mask)
{
    unsigned short *p;
    int i;
    short error;
    unsigned short status;

    error = 0;
    /* PE6 high: external flash ready */
    while (!fw_update_port_e->lo.ext_flash_ready) {
    }
    *(unsigned short *)fw_update_ext_cmd_addr_1 = 0xaa;
    *(unsigned short *)fw_update_ext_cmd_addr_2 = 0x55;
    *(unsigned short *)fw_update_ext_cmd_addr_3 = 0x80;
    *(unsigned short *)fw_update_ext_cmd_addr_4 = 0xaa;
    *(unsigned short *)fw_update_ext_cmd_addr_5 = 0x55;
    p = addr;
    *p = 0x30;
    while (1) {
        status = *p;
        if (status & 0x80) {
            break;
        }
        if (!(status & 0x20)) {
            continue;
        }
        status = *p;
        for (i = 0; i < 50; i++) {
            if (!(status & 0x80)) {
                error = 0x82;
            } else {
                error = 0;
                break;
            }
        }
        break;
    }
    if (error == 0) {
        fw_update_erased_mask |= mask;
        if (0x8000 == mask || 0x20000 == mask || 0x80000 == mask || 0x200000 == mask
            || 0x800000 == mask || 0x2000000 == mask || 0x8000000 == mask) {
            fw_update_erased_mask |= mask << 1;
        } else if (0x10000 == mask || 0x40000 == mask || 0x100000 == mask || 0x400000 == mask
                   || 0x1000000 == mask || 0x4000000 == mask || 0x10000000 == mask) {
            fw_update_erased_mask |= mask >> 1;
        }
    }
    return error;
}

short fw_update_program_ext(unsigned short *dst, unsigned short *src)
{
    int count;
    int i;
    short error;
    unsigned short status;

    error = 0;
    count = 0;
    while (count < 64 && error == 0) {
        while (!fw_update_port_e->lo.ext_flash_ready) {
            count++;
            count--;
        }
        *(unsigned short *)fw_update_ext_cmd_addr_1 = 0xaa;
        *(unsigned short *)fw_update_ext_cmd_addr_2 = 0x55;
        *(unsigned short *)fw_update_ext_cmd_addr_3 = 0xa0;
        *dst = *src;
        while (!fw_update_port_e->lo.ext_flash_ready) {
            count++;
            count--;
        }
        for (;;) {
            status = *dst;
            status ^= *src;
            if (!(status & 0x80)) {
                break;
            }
            if (!(status & 0x20)) {
                continue;
            }
            for (i = 0; i < 50; i++) {
                status = *dst;
                status ^= *src;
                if (status & 0x80) {
                    error = 0x83;
                } else {
                    error = 0;
                    break;
                }
            }
            break;
        }
        count++;
        dst++;
        src++;
    }
    return error;
}

void fw_update_flash_read_mode(void)
{
    *(unsigned short *)fw_update_ext_cmd_addr_3 = 0xf0;
}

/* flash block boundaries again (as program 1's), split at the names fw_update_flash.c uses: low ROM 0x0-0x40000, high ROM 0x800000-0x87C000, high ROM size */
const unsigned int fw_update_onchip_blocks[1] = {
    0x0
};
const unsigned int fw_update_onchip_block_ends[14] = {
    0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000,
    0x7000, 0x8000, 0x10000, 0x18000, 0x20000, 0x28000,
    0x30000, 0x38000
};
const unsigned int fw_update_onchip_end[1] = {
    0x40000
};
const unsigned int fw_update_ext_sectors[1] = {
    0x800000
};
const unsigned int fw_update_ext_sector_ends[16] = {
    0x808000, 0x810000, 0x818000, 0x820000, 0x828000, 0x830000,
    0x838000, 0x840000, 0x848000, 0x850000, 0x858000, 0x860000,
    0x868000, 0x870000, 0x878000, 0x87a000
};
const unsigned int fw_update_ext_end[2] = {
    0x87c000, 0x80000
};
/* single-bit masks as byte pairs (bit k of a 16-bit word: k < 8 in a, else in b; the
   last pairs repeat 2, 4, 8) */
const struct view_fw_update_erase_bits fw_update_erase_bits[16] = {
    {0x01, 0x00}, {0x02, 0x00}, {0x04, 0x00}, {0x08, 0x00},
    {0x10, 0x00}, {0x20, 0x00}, {0x40, 0x00}, {0x80, 0x00},
    {0x00, 0x01}, {0x00, 0x02}, {0x00, 0x02}, {0x00, 0x04},
    {0x00, 0x04}, {0x00, 0x08}, {0x00, 0x08}, {0x00, 0x00}
};
