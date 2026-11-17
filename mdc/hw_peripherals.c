#include "mdc.h"

const unsigned long hw_flash_sector_records[36][4] = {
    { 0x00000000, 0x00200000, 0x10800000, 0x00000000 },
    { 0x00000000, 0x00204000, 0x08800000, 0x00000000 },
    { 0x00000000, 0x00206000, 0x08ff0000, 0x00000000 },
    { 0x00000000, 0x00208000, 0x20800000, 0x00000000 },
    { 0x00000000, 0x00210000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00218000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00220000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00228000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00230000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00238000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00240000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00248000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00250000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00258000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00260000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00268000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00270000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00278000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00000000, 0x20800000, 0x00000000 },
    { 0x00000000, 0x00008000, 0x20800000, 0x00000000 },
    { 0x00000000, 0x00010000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00018000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00020000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00028000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00030000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00038000, 0x08000000, 0x00000000 },
    { 0x00000000, 0x0003a000, 0x08000000, 0x00000000 },
    { 0x00000000, 0x0003c000, 0x10ff0000, 0x00000000 },
    { 0x00000000, 0x00040000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00048000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00050000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00058000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00060000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00068000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00070000, 0x20000000, 0x00000000 },
    { 0x00000000, 0x00078000, 0x20000000, 0x00000000 }
};
struct st_tpu * const p_tpu = (struct st_tpu *)0xffffc0;
struct st_tpu0 * const p_tpu0 = (struct st_tpu0 *)0xffffd0;
struct st_tpu1 * const p_tpu1 = (struct st_tpu1 *)0xffffe0;
struct st_tpu1 * const p_tpu2 = (struct st_tpu1 *)0xfffff0;
struct st_tpu0 * const p_tpu3 = (struct st_tpu0 *)0xfffe80;
struct st_tpu1 * const p_tpu4 = (struct st_tpu1 *)0xfffe90;
struct st_tpu1 * const p_tpu5 = (struct st_tpu1 *)0xfffea0;
struct st_p1 * const p_p1 = (struct st_p1 *)0xfffeb0;
struct st_p1 * const p_p2 = (struct st_p1 *)0xfffeb1;
struct st_p3 * const p_p3 = (struct st_p3 *)0xfffeb2;
struct st_p4 * const p_p4 = (struct st_p4 *)0xffff53;
struct st_pa * const p_pa = (struct st_pa *)0xfffeb9;
struct st_pb * const p_pb = (struct st_pb *)0xfffeba;
struct st_pb * const p_pc = (struct st_pb *)0xfffebb;
struct st_pb * const p_pd = (struct st_pb *)0xfffebc;
struct st_pb * const p_pe = (struct st_pb *)0xfffebd;
struct st_p1 * const p_pf = (struct st_p1 *)0xfffebe;
struct st_pg * const p_pg = (struct st_pg *)0xfffebf;
struct st_intc * const p_intc = (struct st_intc *)0xfffec4;
struct st_bsc * const p_bsc = (struct st_bsc *)0xfffed0;
struct st_dtc * const p_dtc = (struct st_dtc *)0xffff30;
union un_sbycr * const p_sbycr = (union un_sbycr *)0xffff38;
union un_syscr * const p_syscr = (union un_syscr *)0xffff39;
union un_sckcr * const p_sckcr = (union un_sckcr *)0xffff3a;
union un_mdcr * const p_mdcr = (union un_mdcr *)0xffff3b;
union un_mstpcr * const p_mstpcr = (union un_mstpcr *)0xffff3c;
union un_syscr2 * const p_syscr2 = (union un_syscr2 *)0xffff42;
struct st_sci * const p_sci0 = (struct st_sci *)0xffff78;
struct st_sci * const p_sci1 = (struct st_sci *)0xffff80;
struct st_ad * const p_ad = (struct st_ad *)0xffff90;
struct st_da * const p_da = (struct st_da *)0xffffa4;
struct st_tmr * const p_tmr = (struct st_tmr *)0xffffb0;
struct st_tmr0 * const p_tmr0 = (struct st_tmr0 *)0xffffb0;
struct st_tmr1 * const p_tmr1 = (struct st_tmr1 *)0xffffb1;
unsigned short *const p_wdt_read = (unsigned short *)0xffffbc;
unsigned short *const p_wdt = (unsigned short *)0xffffbc;
unsigned char *const p_ram_above_stack = (unsigned char *)0xfffbf0;
