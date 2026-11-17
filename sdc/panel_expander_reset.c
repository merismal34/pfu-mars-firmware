#include "fw.h"
#pragma section H

/* Early-return variant of panel_init_expander: each failed check returns 0 at once; the
   ok flag is a char local that is never set, so the tail returns it as 0. */
int panel_expander_reset_outputs(void)
{
    char ok;

    ok = 0;
    if (iic_wait_bus_free() == 0) {
        return 0;
    }
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    iic_wait(0);
    iic_write_byte(PANEL_EXPANDER_WRITE);
    iic_write_byte(PANEL_EXPANDER_OUTPUT_0);
    iic_write_byte(0x30);
    iic_write_byte(0xff);
    iic_clear_iric();
    p_iic->ICCR.BYTE = 0xb0;
    iic_wait(3);
    p_iic->ICCR.BYTE = 0xa1;
    if (iic_wait_bus_free() == 0) {
        return 0;
    }
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    iic_wait(0);
    iic_write_byte(PANEL_EXPANDER_WRITE);
    iic_write_byte(PANEL_EXPANDER_OUTPUT_1);
    iic_write_byte(0xff);
    iic_write_byte(0x30);
    iic_clear_iric();
    p_iic->ICCR.BYTE = 0xb0;
    iic_wait(3);
    p_iic->ICCR.BYTE = 0xa1;
    err_display_flags.v = 0;
    err_display_flags.b.b3 = 1;
    iic_panel_set_port0(PANEL_LED_POWER);
    iic_panel_set_digit(panel_glyph_8_pattern[0]);
    return ok;
}
