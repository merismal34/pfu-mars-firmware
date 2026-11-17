/* rt.h: the functions of rt.src. */
#ifndef RT_H
#define RT_H
#include "common.h"

/* functions */
extern void rt_restart(void);
extern void rt_delay_240();
extern void rt_delay_2400();
extern void rt_delay_24k(void);
extern void rt_delay_60k(void);
extern void rt_delay_180k(void);
extern int rt_divu16(int, int);
extern int rt_divu32();
extern void rt_mem_clear(void *, int);
extern void rt_mem_copy(void *, void *, int);
extern void rt_mem_copy_to_odd(void *, void *, int);
extern void rt_mem_copy_from_odd(int, int, int);
extern int rt_mem_equal(void *, void *, int);
extern void rt_mem_compare_count(int, int, int, int);
extern int rt_mem_all_equal(void *, int, int, void *);
extern void rt_mem_set(void *, int, int);

#endif
