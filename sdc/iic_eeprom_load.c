#include "fw.h"
#pragma section H
/* Constants (section C). */
const unsigned char iic_eeprom_cell_defaults[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


int iic_eeprom_load_settings(void)
{
    int ok;
    int i;
    unsigned short index;
    union {
        short w;
        struct { char h, l; } b;
    } cell;
    int pad;

    iic_eeprom_load_passes = 0;
    for (;;) {
        iic_eeprom_bad_cells = 0;
        for (i = 0; i < 0x200; i++) {
            iic_eeprom_cell.v = i;
            cell.w = iic_eeprom_read_cell();
            if (cell.b.h != 0) {
                if (iic_eeprom_cell.v != 0x26 && iic_eeprom_cell.v != 0x27) {
                    iic_eeprom_bad_cells++;
                }
                if (iic_eeprom_cell.v >= 0x100) {
                    cell.b.l = 0;
                } else {
                    cell.b.l = iic_eeprom_cell_defaults[iic_eeprom_cell.v & 0xff];
                }
                iic_eeprom_write_cell(cell.b.l);
                rt_delay_60k();
            }
            settings[iic_eeprom_cell.v] = cell.b.l;
        }
        for (i = 0x200; i < 0x400; i++) {
            iic_eeprom_cell.v = i;
            cell.w = iic_eeprom_read_word();
            index = iic_eeprom_cell.v - 0x200;
            index += index;
            iic_eeprom_area2[index] = cell.b.h;
            iic_eeprom_area2_byte_1[index] = cell.b.l;
        }
        util_mem_copy(iic_eeprom_area2_saved, iic_eeprom_area2, 0x400);
        if (++iic_eeprom_load_passes > 3) {
            ok = 0;
            iic_eeprom_error = 1;
            hw_device_errors.f.eeprom = 1;
            goto done;
        }
        if (iic_eeprom_bad_cells == 0) {
            ok = 1;
            iic_eeprom_error = 0;
            hw_device_errors.f.eeprom = 0;
            goto done;
        }
        err_display_flags.b.b5 = 1;
        err_display_flags.b.b5 = 0;
    }
done:
    if (settings_byte_0x1e[0] & 0x80) {
        hw_device_errors.f.panel_expander = 1;
    }
    iic_eeprom_cell.v = 0;
    iic_eeprom_mark_saved();
    return 1;
}
