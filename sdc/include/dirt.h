/* dirt.h: declarations of dirt_line.c, dirt_check.c, dirt_defects.c. */
#ifndef DIRT_H
#define DIRT_H
#include "common.h"

struct dirt_defect_list { short w[128]; };

/* functions */
extern void dirt_scan_line(unsigned char);
extern char dirt_bit_to_shift(unsigned char bit);
extern int dirt_shift_left(int value, char shift);
extern int dirt_is_known_defect(short pixel, unsigned char port);

/* functions */
extern void dirt_run_check(void);
extern void dirt_check(int side);
extern void dirt_check_defect_lists(void);

/* constants */
extern const unsigned char dirt_dip_filter[5];
extern const unsigned char dirt_flat_filter[7];

/* constants */
extern unsigned short flash_defect_area[];
extern unsigned short dirt_defects_adf_front_1[];
extern unsigned short dirt_defects_adf_front_2[];
extern unsigned short dirt_defects_adf_back[3][128];
extern unsigned short dirt_defects_flatbed[3][128];
extern unsigned short dirt_defects_last_word[];

/* variables */
extern unsigned short dirt_line_buf[];

#endif
