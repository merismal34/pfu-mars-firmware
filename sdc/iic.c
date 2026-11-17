#define settings settings_hdr
#include "fw.h"
#undef settings
extern char settings[];
union iic_word_bytes {
    short w;
    struct { char h; char l; } b;
};
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

int iic_write_byte(unsigned char byte)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    p_iic->EQU.ICE1.UN_ICDR = byte;
    iic_clear_iric();
    _builtin_set_imask(saved_imask);
    return iic_wait(1);
}

int iic_wait_bus_free(void)
{
    int i;

    i = 0;
    for (;;) {
        i++;
        if (!p_iic->ICCR.BIT.BBSY)
            return 1;
        else if (i > 8000) {
            i = 0;
            while (1) {
                rt_delay_60k();
                i++;
                if (!p_iic->ICCR.BIT.BBSY)
                    return 1;
                else if (i > 2)
                    return 0;
            }
        }
    }
}

int iic_wait(unsigned char phase)
{
    int i;

    i = 0;
    if (phase == 3) {
        while (1) {
            i++;
            if (!p_iic->ICCR.BIT.BBSY)
                break;
            else if (i > 8000) {
                i = 0;
                while (1) {
                    rt_delay_240();
                    i++;
                    if (!p_iic->ICCR.BIT.BBSY)
                        break;
                    else if (i > 2)
                        return 0;
                }
            }
        }
        iic_clear_iric();
        return 1;
    }
    while (1) {
        i++;
        if (p_iic->ICCR.BIT.IRIC)
            break;
        else if (i > 8000) {
            i = 0;
            while (1) {
                rt_delay_240();
                i++;
                if (p_iic->ICCR.BIT.IRIC)
                    break;
                else if (i > 2)
                    return 0;
            }
        }
    }
    i = 0;
    if (phase == 0) {
        while (1) {
            i++;
            if (p_iic->ICSR.BIT.IRTR)
                break;
            else if (i > 8000) {
                i = 0;
                while (1) {
                    rt_delay_240();
                    i++;
                    if (p_iic->ICSR.BIT.IRTR)
                        break;
                    else if (i > 2)
                        return 0;
                }
            }
        }
        iic_clear_iric();
        return 1;
    }
    if (phase == 1) {
        while (1) {
            i++;
            if (!p_iic->ICSR.BIT.ACKB)
                break;
            else if (i > 8000) {
                i = 0;
                while (1) {
                    rt_delay_240();
                    i++;
                    if (!p_iic->ICSR.BIT.ACKB)
                        break;
                    else if (i > 2)
                        return 0;
                }
            }
        }
        iic_clear_iric();
        return 1;
    }
    if (phase == 2) {
        while (1) {
            i++;
            if (p_iic->SCRX.BIT.ICDRF0 && p_iic->ICCR.BIT.BBSY)
                break;
            else if (i > 8000) {
                i = 0;
                while (1) {
                    rt_delay_240();
                    i++;
                    if (p_iic->SCRX.BIT.ICDRF0 && p_iic->ICCR.BIT.BBSY)
                        break;
                    else if (i > 2)
                        return 0;
                }
            }
        }
        iic_clear_iric();
        return 1;
    }
}

void iic_clear_iric(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    p_iic->ICCR.BIT.IRIC = 0;
    _builtin_set_imask(saved_imask);
    return;
}

void iic_read_bytes(unsigned char *buf, int count)
{
    int saved_imask;
    char t;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    p_iic->ICCR.BIT.TRS = 0;
    p_iic->ICSR.BIT.ACKB = 0;
    p_iic->SCRX.BIT.HNDS = 1;
    iic_clear_iric();
    t = p_iic->EQU.ICE1.UN_ICDR;
    _builtin_set_imask(saved_imask);
    iic_wait(2);
    iic_clear_iric();
    for (;;) {
        _builtin_set_imask(15);
        *buf++ = p_iic->EQU.ICE1.UN_ICDR;
        iic_clear_iric();
        _builtin_set_imask(saved_imask);
        iic_wait(2);
        iic_clear_iric();
        if (--count == 0) {
            _builtin_set_imask(15);
            p_iic->ICSR.BIT.ACKB = 1;
            *buf++ = p_iic->EQU.ICE1.UN_ICDR;
            iic_clear_iric();
            _builtin_set_imask(saved_imask);
            iic_wait(2);
            iic_clear_iric();
            _builtin_set_imask(15);
            p_iic->ICCR.BIT.TRS = 1;
            t = p_iic->EQU.ICE1.UN_ICDR;
            _builtin_set_imask(saved_imask);
            break;
        }
    }
}

int iic_panel_set_port0(char bits)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (iic_busy.b7)
        return 0;
    if (hw_device_errors.f.panel_expander) {
        if (settings_byte_0x1e[0] & 0x80) {
        } else
            return 0;
    }
    if (iic_wait_bus_free() == 0)
        return 0;
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    _builtin_set_imask(saved_imask);
    iic_wait(0);
    iic_write_byte(PANEL_EXPANDER_WRITE);
    iic_write_byte(PANEL_EXPANDER_CONFIG_0);
    iic_write_byte((char)~bits);
    iic_write_byte(iic_panel_digit.v);
    _builtin_set_imask(15);
    iic_panel_port0 = ~bits;
    _builtin_set_imask(saved_imask);
    p_iic->ICCR.BYTE = 0xb0;
    iic_wait(3);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa1;
    _builtin_set_imask(saved_imask);
    return 1;
}

int iic_panel_set_digit(char digit)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (iic_busy.b7)
        return 0;
    if (hw_device_errors.f.panel_expander) {
        if (settings_byte_0x1e[0] & 0x80) {
        } else
            return 0;
    }
    if (iic_wait_bus_free() == 0)
        return 0;
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    _builtin_set_imask(saved_imask);
    if (iic_wait(0) == 0)
        return 0;
    if (iic_write_byte(PANEL_EXPANDER_WRITE) == 0)
        return 0;
    if (iic_write_byte(PANEL_EXPANDER_CONFIG_1) == 0)
        return 0;
    if (iic_write_byte(digit | 0x80) == 0)
        return 0;
    if (testmode_number.v == 0x25) {
        if (panel_current.char_index == 0 || panel_current.char_index == 2 || panel_current.char_index == 4 || panel_current.char_index == 6 || panel_current.char_index == 8 || panel_current.char_index == 10) {
            if (iic_write_byte(0x8f) == 0)
                return 0;
        } else {
            if (iic_write_byte(0xbf) == 0)
                return 0;
        }
    } else if (testmode_number.v == 3) {
        if (panel_current.char_index == 0 || panel_current.char_index == 2 || panel_current.char_index == 4 || panel_current.char_index == 6 || panel_current.char_index == 8 || panel_current.char_index == 10) {
            if (iic_write_byte(0x8f) == 0)
                return 0;
        } else {
            if (iic_write_byte(0xaf) == 0)
                return 0;
        }
    } else {
        if (iic_write_byte(iic_panel_port0) == 0)
            return 0;
    }
    _builtin_set_imask(15);
    iic_panel_digit.v = digit | 0x80;
    p_iic->ICCR.BYTE = 0xb0;
    _builtin_set_imask(saved_imask);
    if (iic_wait(3) == 0)
        return 0;
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa1;
    _builtin_set_imask(saved_imask);
    return 1;
}

char iic_panel_read_buttons(void)
{
    int saved_imask;
    unsigned char buf[8];

    saved_imask = _builtin_get_imask();
    if (iic_busy.b7)
        return 0;
    if (hw_device_errors.f.panel_expander) {
        if (settings_byte_0x1e[0] & 0x80) {
        } else
            return 0;
    }
    if (iic_wait_bus_free() == 0)
        return 0;
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    _builtin_set_imask(saved_imask);
    iic_wait(0);
    iic_write_byte(PANEL_EXPANDER_WRITE);
    iic_write_byte(PANEL_EXPANDER_INPUT_0);
    p_iic->ICCR.BYTE = 0xb0;
    iic_wait(3);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa1;
    _builtin_set_imask(saved_imask);
    if (iic_wait_bus_free() == 0)
        return 0;
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    _builtin_set_imask(saved_imask);
    iic_wait(0);
    iic_write_byte(PANEL_EXPANDER_READ);
    iic_read_bytes(buf, 2);
    buf[0] = ~buf[0] & PANEL_BUTTONS_ALL;
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa0;
    _builtin_set_imask(saved_imask);
    iic_wait(3);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa1;
    _builtin_set_imask(saved_imask);
    return buf[0];
}

void iic_eeprom_save_settings(void)
{
    int i;
    int saved_imask;
    union iic_word_bytes cell;

    saved_imask = _builtin_get_imask();
    if (power_state.f.saving)
        util_mem_copy(power_image_settings, settings, 0x200);
    else
        util_mem_copy_to_odd(&mdc_settings, settings, 0x200);
    if (hw_device_errors.f.eeprom)
        return;
    _builtin_set_imask(15);
    iic_busy.b7 = 1;
    _builtin_set_imask(saved_imask);
    for (i = 0; i < 0x200; i++) {
        iic_eeprom_cell.v = i;
        cell.w = iic_eeprom_read_cell();
        if (cell.b.h) {
            iic_eeprom_error = 1;
            cell.b.l = ~settings[i];
        }
        if (cell.b.l != settings[i]) {
            iic_eeprom_write_cell(settings[i]);
            rt_delay_60k();
            cell.w = iic_eeprom_read_cell();
            if (cell.b.h || cell.b.l != settings[i])
                iic_eeprom_error = 1;
        }
    }
    iic_eeprom_cell.v = 0;
    iic_eeprom_mark_saved();
    _builtin_set_imask(15);
    iic_busy.b7 = 0;
    _builtin_set_imask(saved_imask);
}

void iic_unused_1(void)
{
    int x;
}

void iic_unused_2(void)
{
    int x;
}

void iic_eeprom_mark_saved(void)
{
    int i;
    int x;

    for (i = 0; i < 0x200; i++)
        settings_saved[i] = settings[i];
}

int iic_eeprom_read_cell(void)
{
    int saved_imask;
    unsigned char device_addr;
    char buf[4];

    saved_imask = _builtin_get_imask();
    if (iic_wait_bus_free() == 0)
        return 0;
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    _builtin_set_imask(saved_imask);
    iic_wait(0);
    if (iic_eeprom_cell.v >= 0x380)
        device_addr = 0xae;
    else if (iic_eeprom_cell.v >= 0x300)
        device_addr = 0xac;
    else if (iic_eeprom_cell.v >= 0x280)
        device_addr = 0xaa;
    else if (iic_eeprom_cell.v >= 0x200)
        device_addr = 0xa8;
    else if (iic_eeprom_cell.v >= 0x180)
        device_addr = 0xa6;
    else if (iic_eeprom_cell.v >= 0x100)
        device_addr = 0xa4;
    else if (iic_eeprom_cell.v >= 0x80)
        device_addr = 0xa2;
    else
        device_addr = 0xa0;
    iic_write_byte(device_addr);
    iic_write_byte(iic_eeprom_cell.v << 1);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb4;
    _builtin_set_imask(saved_imask);
    iic_wait(0);
    iic_write_byte(device_addr + 1);
    iic_read_bytes((unsigned char *)buf, 2);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa0;
    _builtin_set_imask(saved_imask);
    iic_wait(3);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa1;
    _builtin_set_imask(saved_imask);
    buf[1] = ~buf[1];
    if (buf[0] != buf[1])
        return (unsigned char)buf[0] | 0xff00;
    return (unsigned char)buf[0];
}

void iic_eeprom_write_cell(char value)
{
    int saved_imask;
    unsigned char device_addr;

    saved_imask = _builtin_get_imask();
    if (iic_wait_bus_free() == 0)
        return;
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    _builtin_set_imask(saved_imask);
    iic_wait(0);
    if (iic_eeprom_cell.v >= 0x380)
        device_addr = 0xae;
    else if (iic_eeprom_cell.v >= 0x300)
        device_addr = 0xac;
    else if (iic_eeprom_cell.v >= 0x280)
        device_addr = 0xaa;
    else if (iic_eeprom_cell.v >= 0x200)
        device_addr = 0xa8;
    else if (iic_eeprom_cell.v >= 0x180)
        device_addr = 0xa6;
    else if (iic_eeprom_cell.v >= 0x100)
        device_addr = 0xa4;
    else if (iic_eeprom_cell.v >= 0x80)
        device_addr = 0xa2;
    else
        device_addr = 0xa0;
    iic_write_byte(device_addr);
    iic_write_byte(iic_eeprom_cell.v << 1);
    iic_write_byte(value);
    iic_write_byte((char)~value);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb0;
    _builtin_set_imask(saved_imask);
    iic_wait(3);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa1;
    _builtin_set_imask(saved_imask);
}

void iic_eeprom_save_area2(void)
{
    int i;
    union iic_word_bytes word;
    unsigned short idx;
    char m;
    int x;

    m = _builtin_get_imask();
    if (hw_device_errors.f.eeprom)
        return;
    iic_busy.b7 = 1;
    for (i = 0x200; i < 0x400; i++) {
        iic_eeprom_cell.v = i;
        idx = i + 0xfe00;
        idx += idx;
        word.b.h = iic_eeprom_area2[idx];
        word.b.l = (iic_eeprom_area2 + idx)[1];
        if (iic_eeprom_area2_saved[idx] != iic_eeprom_area2[idx] || iic_eeprom_area2_saved_byte_1[idx] != ((char *)(iic_eeprom_area2 + idx))[1]) {
            iic_eeprom_write_word(word.w);
            rt_delay_60k();
        }
    }
    iic_eeprom_cell.v = 0;
    iic_busy.b7 = 0;
    util_mem_copy(iic_eeprom_area2_saved, iic_eeprom_area2, 0x400);
}

void iic_eeprom_save_area2_range(unsigned short first, unsigned short last)
{
    int i;
    union iic_word_bytes word;
    unsigned short idx;
    char m;
    unsigned short first_word;
    unsigned short last_word;
    int x1;
    int x2;

    m = _builtin_get_imask();
    if (hw_device_errors.f.eeprom)
        return;
    iic_busy.b7 = 1;
    first_word = first >> 1;
    last_word = last >> 1;
    for (i = first_word + 0x200; i <= last_word + 0x200; i++) {
        iic_eeprom_cell.v = i;
        idx = i + 0xfe00;
        idx += idx;
        word.b.h = iic_eeprom_area2[idx];
        word.b.l = (iic_eeprom_area2 + idx)[1];
        if (iic_eeprom_area2_saved[idx] != iic_eeprom_area2[idx] || iic_eeprom_area2_saved_byte_1[idx] != ((char *)(iic_eeprom_area2 + idx))[1]) {
            iic_eeprom_write_word(word.w);
            rt_delay_60k();
        }
    }
    iic_eeprom_cell.v = 0;
    iic_busy.b7 = 0;
    util_mem_copy(iic_eeprom_area2_saved + first, iic_eeprom_area2 + first, last - first + 1);
}

short iic_eeprom_read_word(void)
{
    int saved_imask;
    unsigned char device_addr;
    unsigned char buf[4];
    short word;

    saved_imask = _builtin_get_imask();
    if (iic_wait_bus_free() == 0)
        return 0;
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    _builtin_set_imask(saved_imask);
    iic_wait(0);
    if (iic_eeprom_cell.v >= 0x380)
        device_addr = 0xae;
    else if (iic_eeprom_cell.v >= 0x300)
        device_addr = 0xac;
    else if (iic_eeprom_cell.v >= 0x280)
        device_addr = 0xaa;
    else if (iic_eeprom_cell.v >= 0x200)
        device_addr = 0xa8;
    else if (iic_eeprom_cell.v >= 0x180)
        device_addr = 0xa6;
    else if (iic_eeprom_cell.v >= 0x100)
        device_addr = 0xa4;
    else if (iic_eeprom_cell.v >= 0x80)
        device_addr = 0xa2;
    else
        device_addr = 0xa0;
    iic_write_byte(device_addr);
    iic_write_byte(iic_eeprom_cell.v << 1);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb4;
    _builtin_set_imask(saved_imask);
    iic_wait(0);
    iic_write_byte(device_addr + 1);
    iic_read_bytes(buf, 2);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa0;
    _builtin_set_imask(saved_imask);
    iic_wait(3);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa1;
    _builtin_set_imask(saved_imask);
    word = buf[0];
    word <<= 8;
    word += buf[1];
    return word;
}

void iic_eeprom_write_word(unsigned short word)
{
    int saved_imask;
    unsigned char device_addr;
    char byte;

    saved_imask = _builtin_get_imask();
    if (iic_wait_bus_free() == 0)
        return;
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb1;
    p_iic->ICCR.BYTE = 0xb4;
    _builtin_set_imask(saved_imask);
    iic_wait(0);
    if (iic_eeprom_cell.v >= 0x380)
        device_addr = 0xae;
    else if (iic_eeprom_cell.v >= 0x300)
        device_addr = 0xac;
    else if (iic_eeprom_cell.v >= 0x280)
        device_addr = 0xaa;
    else if (iic_eeprom_cell.v >= 0x200)
        device_addr = 0xa8;
    else if (iic_eeprom_cell.v >= 0x180)
        device_addr = 0xa6;
    else if (iic_eeprom_cell.v >= 0x100)
        device_addr = 0xa4;
    else if (iic_eeprom_cell.v >= 0x80)
        device_addr = 0xa2;
    else
        device_addr = 0xa0;
    iic_write_byte(device_addr);
    iic_write_byte(iic_eeprom_cell.v << 1);
    byte = word >> 8;
    iic_write_byte(byte);
    byte = word;
    iic_write_byte(byte);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xb0;
    _builtin_set_imask(saved_imask);
    iic_wait(3);
    _builtin_set_imask(15);
    p_iic->ICCR.BYTE = 0xa1;
    _builtin_set_imask(saved_imask);
}
