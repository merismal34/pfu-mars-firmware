/* flash.h: declarations of flash.c, flash_dump.c. */
#ifndef FLASH_H
#define FLASH_H
#include "common.h"

struct flash_mdc_block_entry {
    unsigned char b0, b1, b2, b3, size_kb, attribute;
};

/* functions: flash code */
extern unsigned short flash_erase_settings_sector(void);
extern unsigned short flash_write_16(unsigned short *, unsigned short *);
extern unsigned short flash_erase_dump_sector_raw(void);

/* functions */
extern void flash_init_cmd_addrs(void);
extern void flash_build_block_table(unsigned char *p);
extern unsigned short flash_erase_data_sector(void);
extern unsigned short flash_program_range(void);
extern void flash_reset_read_mode(void);
extern void flash_mdc_run_download(void);
extern void flash_mdc_build_block_table(unsigned char *p, unsigned char sel);
extern void flash_save_settings_backup(void);
extern void flash_save_defect_lists(void);
extern void flash_save_settings_copy(void);
extern void flash_write_defect_area(void);
extern int flash_restore_settings_backup(void);
extern int flash_erase_work(void);
extern unsigned short flash_erase_dump_sector(void);
extern void flash_save_dump(void);

/* constants */
extern const unsigned flash_onchip_block_start[1];
extern const void *const flash_onchip_block_end[15];
extern const unsigned flash_ext_sector_start[1];
extern const void *const flash_ext_sector_end[18];
extern const struct flash_mdc_block_entry flash_mdc_block_table[8];
extern const unsigned flash_mdc_block_addr[8];
extern unsigned char flash_backup_area[][0x220];
extern unsigned char flash_settings_mirror[];

/* I/O */
extern volatile unsigned char mdc_info_table[];
extern volatile unsigned char mdc_info_table_sums[];
extern volatile unsigned char mdc_back_window_data[][4];
extern volatile unsigned char mdc_back_window_data_2[][4];

#endif
