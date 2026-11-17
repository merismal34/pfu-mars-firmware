/* start.h: declarations of start.c. */
#ifndef START_H
#define START_H
#include "common.h"

/* functions */
extern void start_entry(char sync_mode, char ultra);
extern void start_reset(void);
extern void start_init(char sync_mode, unsigned char ultra);

/* constants */
extern unsigned int *hw_section_table;
extern unsigned int *hw_section_lo_d_begin;
extern unsigned int *hw_section_lo_d_end;
extern unsigned int *hw_section_lo_b_begin;
extern unsigned int *hw_section_lo_b_end;
extern unsigned int *hw_section_set3_d_rom;
extern unsigned int *hw_section_set3_d_begin;
extern unsigned int *hw_section_set3_d_end;
extern unsigned int *hw_section_set3_b_begin;
extern unsigned int *hw_section_set3_b_end;
extern unsigned int *hw_section_onchip_begin;
extern unsigned int *hw_section_onchip_end;
extern unsigned int *hw_section_hi_d_rom;
extern unsigned int *hw_section_hi_d_begin;
extern unsigned int *hw_section_hi_d_end;
extern unsigned int *hw_section_hi_b_begin;
extern unsigned int *hw_section_hi_b_end;
extern void **hw_vbr_value;
extern unsigned int *hw_section_mem_begin;
extern unsigned int *hw_section_mem_end;

#endif
