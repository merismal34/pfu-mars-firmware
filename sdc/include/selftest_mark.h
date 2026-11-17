/* selftest_mark.h: declarations of selftest_mark.c. */
#ifndef SELFTEST_MARK_H
#define SELFTEST_MARK_H
#include "common.h"

/* functions */
extern unsigned char selftest_find_mark(void);
extern void selftest_find_mark_edges(unsigned short *fall_pos, unsigned short *rise_pos, long *fall_index, unsigned short *levels, unsigned long start, unsigned long end_pos);

#endif
