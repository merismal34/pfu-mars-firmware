/* util.h: declarations of util_mem.c, util.c.
   */
#ifndef UTIL_H
#define UTIL_H
#include "common.h"

/* functions */
extern void util_mem_clear(void *, int);
extern void util_mem_copy(void *, void *, int);
extern void util_mem_copy_to_odd(void *, void *, int);
extern void util_mem_copy_from_odd(int, int, int);
extern int util_mem_equal(void *, void *, int);
extern int util_mem_compare_count();
extern int util_mem_all_equal(void *, unsigned char, int, void *);
extern void util_mem_set(void *, unsigned char, int);
extern char util_sign_mag8_to_int(unsigned char value);
extern unsigned short util_get_be16(unsigned char *);
extern unsigned long util_get_be32(unsigned char *);
extern void util_put_be16();
extern int util_abs16(int value);
extern char util_read8(char *p);
extern void util_write8(char *p, char value);
extern short util_read16(short *);
extern void util_write16(short *p, short value);

#endif
