#include "fw.h"
#pragma section H

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

/* Returns 0 on the early paths; the last path falls off the end (r0 holds the
   imask restore), so the function has no return value there. */
int panel_update_expander(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (iic_busy.b7 != 0) {
        _builtin_set_imask(saved_imask);
        return 0;
    }
    _builtin_set_imask(saved_imask);
    if (iic_wait_bus_free() == 0) {
        return 0;
    }
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    iic_wait(0);
    iic_write_byte(PANEL_EXPANDER_WRITE);
    iic_write_byte(PANEL_EXPANDER_CONFIG_0);
    iic_write_byte(iic_panel_port0);
    iic_write_byte(0);
    _builtin_set_imask(15);
    iic_clear_iric();
    p_iic->ICCR.BYTE = 0xb0;
    _builtin_set_imask(saved_imask);
    iic_wait(3);
    p_iic->ICCR.BYTE = 0xa1;
    if (iic_wait_bus_free() == 0) {
        return 0;
    }
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    iic_wait(0);
    iic_write_byte(PANEL_EXPANDER_WRITE);
    iic_write_byte(PANEL_EXPANDER_CONFIG_1);
    iic_write_byte(iic_panel_digit.v);
    iic_write_byte(iic_panel_port0);
    p_iic->ICCR.BYTE = 0xb0;
    iic_wait(3);
    p_iic->ICCR.BYTE = 0xa1;
    _builtin_set_imask(15);
    panel_current.display_flags.b6 = 1;
    panel_current.glyph_shown = 0xff;
    _builtin_set_imask(saved_imask);
}
