/* The pointer table to the on-chip peripherals: every register block is reached through a
 * pointer object, p_sci0->SSR.BIT.TDRE (SCI0). */
#ifndef MDC_IO_H
#define MDC_IO_H
#include "2318s.h"

extern struct st_tpu * const p_tpu;   /* TPU */
extern struct st_tpu0 * const p_tpu0;   /* TPU0 */
extern struct st_tpu1 * const p_tpu1;   /* TPU1 */
extern struct st_tpu1 * const p_tpu2;   /* TPU2 */
extern struct st_tpu0 * const p_tpu3;   /* TPU3 */
extern struct st_tpu1 * const p_tpu4;   /* TPU4 */
extern struct st_tpu1 * const p_tpu5;   /* TPU5 */
extern struct st_p1 * const p_p1;   /* P1 */
extern struct st_p1 * const p_p2;   /* P2 */
extern struct st_p3 * const p_p3;   /* P3 */
extern struct st_p4 * const p_p4;   /* P4 */
extern struct st_pa * const p_pa;   /* PA */
extern struct st_pb * const p_pb;   /* PB */
extern struct st_pb * const p_pc;   /* PC */
extern struct st_pb * const p_pd;   /* PD */
extern struct st_pb * const p_pe;   /* PE */
extern struct st_p1 * const p_pf;   /* PF */
extern struct st_pg * const p_pg;   /* PG */
extern struct st_intc * const p_intc;   /* INTC */
extern struct st_bsc * const p_bsc;   /* BSC */
extern struct st_dtc * const p_dtc;   /* DTC */
extern union un_sbycr * const p_sbycr;   /* SBYCR */
extern union un_syscr * const p_syscr;   /* SYSCR */
extern union un_sckcr * const p_sckcr;   /* SCKCR */
extern union un_mdcr * const p_mdcr;   /* MDCR */
extern union un_mstpcr * const p_mstpcr;   /* MSTPCR */
extern union un_syscr2 * const p_syscr2;   /* SYSCR2 */
extern struct st_sci * const p_sci0;   /* SCI0 */
extern struct st_sci * const p_sci1;   /* SCI1 */
extern struct st_ad * const p_ad;   /* AD */
extern struct st_da * const p_da;   /* DA */
extern struct st_tmr * const p_tmr;   /* TMR */
extern struct st_tmr0 * const p_tmr0;   /* TMR0 */
extern struct st_tmr1 * const p_tmr1;   /* TMR1 */
extern unsigned short *const p_wdt_read;
/* 0x2f9e8, 0x2f9ec -> 0xFFFFBC (WDT), 0x2f9f0 -> 0xFFFBF0: not in 2318s.h; p_wdt is declared in mdc.h */

#endif
