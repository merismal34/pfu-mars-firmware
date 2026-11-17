/* peripherals.h: the firmware's ROM pointers to the chip's peripheral blocks (0x3EDB0-0x3EE3C,
   defined in hw_selftest.c), typed with Renesas' structs from periph.h (7144s.h); boot.h has its
   own for the boot programs. */
#ifndef PERIPHERALS_H
#define PERIPHERALS_H
#include "common.h"

struct ad_reg_bits {
    unsigned char :1,
                  cks:2,
                  b:2,
                  lo:3;
};
struct ad_regs { char p[104]; struct ad_reg_bits adcr_0, adcr_1; };
struct mtu_common_regs { struct Bits tstr; unsigned char tsyr; };
struct power_down_view { int pad_00[2], mstcr; };

/* constants */
extern struct mtu_common_regs *p_mtu;
extern void *p_mtu34;
extern void *p_pc;
extern void *p_pd;
extern struct ad_regs *p_ad;
extern void *p_flash;
extern void *p_ubc;
extern struct power_down_view *p_sbycr;
extern void *p_dmac0[3];
extern void *p_dmac3;

/* peripheral blocks: the ROM pointer table 0x3edb0-0x3ee3c, Renesas' structs from
   7144s.h. volatile as in the header's own address macros: no function compiles differently without it.
   Not header types (declared in the files): p_mtu (TSTR at 0xffff8240), p_ad (both A/D units),
   p_sbycr (a long at +8). No user yet: p_mtu34, p_pc, p_pd, p_flash, p_ubc,
   p_dmac0..p_dmac3. */
extern volatile struct st_sci *p_sci0;
extern volatile struct st_sci *p_sci1;
extern volatile struct st_sci *p_sci2;
extern volatile struct st_sci *p_sci3;
extern volatile struct st_mtu0 *p_mtu0;
extern volatile struct st_mtu1 *p_mtu1;
extern volatile struct st_mtu1 *p_mtu2;
extern volatile struct st_mtu3 *p_mtu3;
extern volatile struct st_mtu4 *p_mtu4;
extern volatile struct st_intc *p_intc;
extern volatile struct st_pfc *p_pfc;
extern volatile struct st_pa *p_pa;
extern volatile struct st_pb *p_pb;
extern volatile struct st_pe *p_pe;
extern volatile struct st_pf *p_pf;
extern volatile struct st_cmt *p_cmt;
extern volatile struct st_cmt0 *p_cmt0;
extern volatile struct st_cmt0 *p_cmt1;
extern volatile union un_wdt *p_wdt_read;
extern volatile union un_wdt *p_wdt_write;
extern volatile struct st_bsc *p_bsc;
extern volatile struct st_dmac *p_dmac;
extern volatile struct st_dtc *p_dtc;
extern volatile struct st_iic *p_iic;

#endif
