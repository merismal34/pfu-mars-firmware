/* The boot program: its own declarations. */
#ifndef BOOT_H
#define BOOT_H
#include <machine.h>
#include "shared.h"

/* mailbox.download_cmd: the firmware download steps of flash_run_download */
#define MDC_DOWNLOAD_WRITE      0x80
#define MDC_DOWNLOAD_READ       0x88
#define MDC_DOWNLOAD_INFO_TABLE 0xA0

/* flash_error_code / flash_saved_code: the boot program's error code */
#define ERR_BOOT_FLASH 0xEB

/* ---- On-chip RAM 0xFFDC00-0xFFFBFF ---- */
extern unsigned char flash_error_flags;
extern unsigned char flash_error_code;
extern unsigned char globals_filler_13[2];
extern unsigned char flash_saved_flags;
extern unsigned char flash_saved_code;

/* ---- Chip B's RAM 0x400100 up (the SDC's back window 0xC60000, odd bytes) ---- */
extern unsigned char flash_piece_buf[0x400];

/* ---- Functions ---- */
extern void boot_reset(void);
extern void boot_init_bus_ports(void);
extern int boot_main(void);
extern void boot_delay(void);
extern void boot_enter_download(void);
extern int boot_check_application(void);
extern int boot_check_block_sum(unsigned short *, unsigned short *, unsigned short);
extern void boot_test_mailbox(void);
extern void boot_handshake(void);
extern int flash_erase_chip(void);
extern void main(void);
extern void flash_run_download(void);
extern int flash_erase_sector(unsigned long, unsigned short *);
extern int flash_program_word(unsigned long, unsigned short *, unsigned short);
extern int flash_write_unlock_cycles(void);
extern int flash_wait_ready(void);
extern void flash_delay(void);

#endif /* BOOT_H */
