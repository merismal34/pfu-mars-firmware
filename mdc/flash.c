#include "boot.h"

/* ---- objects ---- */
unsigned char flash_error_flags;
unsigned char flash_error_code;
unsigned char globals_filler_13[2];
unsigned char flash_saved_flags;
unsigned char flash_saved_code;

/* ---- functions ---- */

/* Firmware update loop. */
void flash_run_download(void)
{
    unsigned long addr;
    unsigned long u;
    unsigned short *flash_ptr;
    unsigned char *buf;
    unsigned short len;
    unsigned short word;
    unsigned short rc;
    unsigned char flags;

    addr = 0;
    flags = 0;
    mailbox.cmd_state = MDC_CMD_STATE_DONE;
    mailbox.cmd_result = MDC_RESULT_OK;
    for (;;) {
        if (mailbox.download_cmd == MDC_DOWNLOAD_READ) {
            buf = flash_piece_buf;
            addr = mailbox.download_offset_0;
            addr += (unsigned long)mailbox.download_offset_1 << 8;
            addr += (unsigned long)mailbox.download_offset_2 << 16;
            addr += (unsigned long)mailbox.download_offset_3 << 24;
            addr += 0x100000;
            flash_ptr = (unsigned short *)addr;
            len = mailbox.download_len_high << 8;
            len += mailbox.download_len_low;
            if (len != 0) {
                do {
                    word = *flash_ptr++;
                    *buf++ = (unsigned char)(word >> 8);
                    *buf++ = (unsigned char)word;
                } while ((len -= 2) != 0);
            }
            mailbox.download_cmd = 0;
        }
        if (mailbox.download_cmd == MDC_DOWNLOAD_INFO_TABLE) {
            flash_erase_sector(0x100000, (unsigned short *)0x178000);
            flags |= 4;
            buf = flash_piece_buf;
            flash_ptr = (unsigned short *)0x178000;
            len = 0x20;
            do {
                word = *buf++ << 8;
                word += *buf++;
                flash_program_word(0x100000, flash_ptr++, word);
            } while ((len -= 2) != 0);
            buf = (unsigned char *)0x178000;
            for (len = 0; len < 0x20; )
                mailbox.info_table[len++] = *buf++;
            mailbox.download_cmd = 0;
        }
        if (mailbox.download_cmd == MDC_DOWNLOAD_WRITE) {
            buf = flash_piece_buf;
            addr = mailbox.download_offset_0;
            addr += (unsigned long)mailbox.download_offset_1 << 8;
            addr += (unsigned long)mailbox.download_offset_2 << 16;
            addr += (unsigned long)mailbox.download_offset_3 << 24;
            addr += 0x100000;
            flash_ptr = (unsigned short *)addr;
            rc = 0;
            if (addr < 0x110000) {
                if (!(flags & 1)) {
                    rc = flash_erase_sector(0x100000, (unsigned short *)0x100000);
                    flags |= 1;
                }
            } else if (addr < 0x178000) {
                if (!(flags & 2)) {
                    rc = flash_erase_sector(0x100000, (unsigned short *)0x110000);
                    rc = flash_erase_sector(0x100000, (unsigned short *)0x120000);
                    rc = flash_erase_sector(0x100000, (unsigned short *)0x130000);
                    flags |= 2;
                }
            }
            if (rc != 0) {
                flash_error_flags |= 0x10;
                flash_saved_flags = flash_error_flags;
                flash_saved_code = flash_error_code = ERR_BOOT_FLASH;
            } else {
                len = mailbox.download_len_high << 8;
                len += mailbox.download_len_low;
                if (len > 0x400)
                    len = 0x400;
                if (len != 0) {
                    do {
                        if (flash_ptr > (unsigned short *)0x13ffe0)   /* the version string in the flash window (0x100000 + section VER): an address of the other link */
                            addr = mailbox.download_offset_0;
                        word = *buf++ << 8;
                        word += *buf++;
                        flash_program_word(0x100000, flash_ptr++, word);
                    } while ((len -= 2) != 0);
                    mailbox.download_cmd = 0;
                }
            }
        }
        if (mailbox.cmd_code == MDC_CMD_DOWNLOAD_END) {
            mailbox.handshake_high = 0;
            mailbox.handshake_low = 0;
            mailbox.cmd_state = MDC_CMD_STATE_DONE;
            mailbox.cmd_result = MDC_RESULT_OK;
            break;
        }
    }
    (*(void (**)(void))0)();
}

/* Flash command sequence. */
int flash_erase_sector(unsigned long base, unsigned short *sector)
{
    unsigned long unlock_1;
    unsigned long unlock_2;

    if (flash_wait_ready() == 1)
        return 1;
    unlock_1 = base;
    unlock_1 += 0xaaaa;
    unlock_2 = base;
    unlock_2 += 0x5554;
    *(unsigned short *)unlock_2 = 0x55;
    flash_delay();
    flash_delay();
    *(unsigned short *)unlock_2 = 0x55;
    flash_delay();
    flash_delay();
    *(unsigned short *)unlock_1 = 0xaa;
    *(unsigned short *)unlock_2 = 0x55;
    *(unsigned short *)unlock_1 = 0x80;
    *(unsigned short *)unlock_1 = 0xaa;
    *(unsigned short *)unlock_2 = 0x55;
    *sector = 0x30;
    if (flash_wait_ready() == 1)
        return 1;
    else
        return 0;
}

/* Flash word program. */
int flash_program_word(unsigned long base, unsigned short *dst, unsigned short data)
{
    unsigned long unlock_1;
    unsigned long unlock_2;

    if (flash_wait_ready() == 1)
        return 1;
    unlock_1 = base;
    unlock_1 += 0xaaaa;
    unlock_2 = base;
    unlock_2 += 0x5554;
    *(unsigned short *)unlock_1 = 0xaa;
    *(unsigned short *)unlock_2 = 0x55;
    *(unsigned short *)unlock_1 = 0xa0;
    *dst = data;
    if (flash_wait_ready() == 1)
        return 1;
    else
        return 0;
}

/* Flash helpers. */
int flash_write_unlock_cycles(void)
{
    unsigned long unlock_2;

    unlock_2 = 0x100000;
    unlock_2 += 0x5554;
    if (flash_wait_ready() == 1)
        return 1;
    *(unsigned short *)unlock_2 = 0x55;
    flash_delay();
    *(unsigned short *)unlock_2 = 0x55;
    flash_delay();
    *(unsigned short *)unlock_2 = 0x55;
    flash_delay();
    if (flash_wait_ready() == 1)
        return 1;
    else
        return 0;
}

int flash_wait_ready(void)
{
    unsigned short i;
    unsigned short j;
    unsigned long unlock_2;
    unsigned char *ready_port;

    ready_port = (unsigned char *)0xffff51;
    unlock_2 = 0x100000;
    unlock_2 += 0x5554;
    i = 0;
    j = 0;
    while ((*ready_port & 0x80) == 0) {
        if (i > 18000UL) {
            *(unsigned short *)unlock_2 = 0x55;
            i = 0;
            flash_delay();
            if (j > 50UL)
                return 1;
            j++;
        }
        i++;
    }
    return 0;
}

void flash_delay(void)
{
    unsigned char x;

    x = 0;
    x = 1;
    x = 2;
    x = 3;
    x = 4;
    x = 5;
    x = 6;
    x = 7;
    x = 8;
    x = 9;
}
