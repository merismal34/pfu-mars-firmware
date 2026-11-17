#include "fw.h"
#pragma section H

/* Ok flag is a char local: cleared on entry, each failed check clears it again,
   and it is set to 1 before the register clear (err_display_flags.b.b3 = 0). */
int panel_expander_clear_port1(void)
{
    char ok;

    ok = 0;
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
    if (iic_write_byte(PANEL_EXPANDER_OUTPUT_1) == 0) {
        ok = 0;
    }
    if (iic_write_byte(0) == 0) {
        ok = 0;
    }
    if (iic_write_byte(0) == 0) {
        ok = 0;
    }
    p_iic->ICCR.BYTE = 0xb0;
    if (iic_wait(3) == 0) {
        ok = 0;
    }
    ok = 1;
    p_iic->ICCR.BYTE = 0xa1;
    err_display_flags.v = 0;
    err_display_flags.b.b3 = 0;
    return ok;
}
