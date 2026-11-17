#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

/* Sibling of panel_init_expander (panel_expander_init.c) with a different init sequence (ok is set, never read). */
void panel_init_expander_dark(void)
{
    int saved_imask;
    int ok;

    saved_imask = _builtin_get_imask();
    ok = 1;
    _builtin_set_imask(15);
    p_iic->SCRX.BIT.IICE = 1;
    p_iic->ICCR.BIT.ICE = 1;
    p_iic->ICCR.BIT.ICE = 1;
    p_iic->ICSR.BYTE = 0;
    p_iic->SCRX.BIT.IICX0 = 0;
    p_iic->EQU.ICE1.UN_ICMR.BIT.MLS = 0;
    p_iic->EQU.ICE1.UN_ICMR.BIT.WAIT = 0;
    p_iic->EQU.ICE1.UN_ICMR.BIT.CKS = 3;
    p_iic->EQU.ICE1.UN_ICMR.BIT.BC = 0;
    p_iic->SCRX.BIT.HNDS = 1;
    p_iic->ICCR.BIT.IEIC = 0;
    p_iic->ICCR.BIT._MST = 0;
    p_iic->ICCR.BIT.TRS = 0;
    p_iic->ICCR.BIT.ACKE = 1;
    _builtin_set_imask(saved_imask);
    if (iic_wait_bus_free() == 0) {
        ok = 0;
    }
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    iic_wait(0);
    iic_write_byte(PANEL_EXPANDER_WRITE);
    iic_write_byte(PANEL_EXPANDER_OUTPUT_0);
    iic_write_byte(0);
    iic_write_byte(0);
    iic_clear_iric();
    p_iic->ICCR.BYTE = 0xb0;
    iic_wait(3);
    p_iic->ICCR.BYTE = 0xa1;
    if (iic_wait_bus_free() == 0) {
        ok = 0;
    }
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    iic_wait(0);
    iic_write_byte(PANEL_EXPANDER_WRITE);
    iic_write_byte(PANEL_EXPANDER_OUTPUT_1);
    iic_write_byte(0);
    iic_write_byte(0);
    iic_clear_iric();
    p_iic->ICCR.BYTE = 0xb0;
    iic_wait(3);
    p_iic->ICCR.BYTE = 0xa1;
    if (iic_wait_bus_free() == 0) {
        ok = 0;
    }
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    if (iic_wait(0) == 0) {
        ok = 0;
    }
    if (iic_write_byte(PANEL_EXPANDER_WRITE) == 0) {
        ok = 0;
    }
    if (iic_write_byte(PANEL_EXPANDER_CONFIG_0) == 0) {
        ok = 0;
    }
    if (iic_write_byte(0x8f) == 0) {
        ok = 0;
    }
    if (iic_write_byte(0) == 0) {
        ok = 0;
    }
    _builtin_set_imask(15);
    iic_clear_iric();
    p_iic->ICCR.BYTE = 0xb0;
    _builtin_set_imask(saved_imask);
    if (iic_wait(3) == 0) {
        ok = 0;
    }
    p_iic->ICCR.BYTE = 0xa1;
    if (iic_wait_bus_free() == 0) {
        ok = 0;
    }
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    if (iic_wait(0) == 0) {
        ok = 0;
    }
    if (iic_write_byte(PANEL_EXPANDER_WRITE) == 0) {
        ok = 0;
    }
    if (iic_write_byte(PANEL_EXPANDER_CONFIG_1) == 0) {
        ok = 0;
    }
    if (iic_write_byte(0) == 0) {
        ok = 0;
    }
    if (iic_write_byte(0x8f) == 0) {
        ok = 0;
    }
    p_iic->ICCR.BYTE = 0xb0;
    if (iic_wait(3) == 0) {
        ok = 0;
    }
    p_iic->ICCR.BYTE = 0xa1;
}
