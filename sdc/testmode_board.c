#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);


void testmode_panel_check(void)
{
    unsigned char expected;
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    panel_current.display_main = PANEL_MODE_MAINT;
    panel_current.char_count = 11;
    panel_current.chars[0] = PANEL_GLYPH_BLANK;
    panel_current.chars[1] = 0;
    panel_current.chars[2] = 1;
    panel_current.chars[3] = 2;
    panel_current.chars[4] = 3;
    panel_current.chars[5] = 4;
    panel_current.chars[6] = 5;
    panel_current.chars[7] = 6;
    panel_current.chars[8] = 7;
    panel_current.chars[9] = 8;
    panel_current.chars[10] = 9;
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = 50;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
    _builtin_set_imask(0);
    expected = 16;
    tick_wait_count = 0;
    while (tick_wait_count < 15000) {
        if (button_state.c == expected) {
            switch (expected) {
            case 16: expected = 64; break;
            case 64: expected = 128; break;
            case 128: expected = 8; break;
            default: break;
            }
        }
        if (button_state.c == BUTTON_STATE_POWER) {
            if (expected == 8)
                expected = 0;
            break;
        }
    }
    if (tick_wait_count >= 15000 && expected != 8) {
        testmode_result_1[0] = 85;
    } else {
        switch (expected) {
        case 16: testmode_result_1[0] = 128; break;
        case 8: testmode_result_1[0] = 8; break;
        case 64: testmode_result_1[0] = 64; break;
        case 128: testmode_result_1[0] = 32; break;
        default: testmode_result_1[0] = 0; break;
        }
    }
    if (testmode_result_1[0] != 0)
        err_code_list[0] = 255;
    else
        err_code_list[0] = 128;
    _builtin_set_imask(15);
    button_state.b.b3 = 0;
    panel_current.button_power_press_polls = 0;
    _builtin_set_imask(saved_imask);
    panel_set_display_mode(0, PANEL_MODE_FUNCTION);
}

void testmode_04(void) { err_code_list[0] = 128; return; }

void testmode_05(void) { err_code_list[0] = 128; return; }

void testmode_06(void) { err_code_list[0] = 128; return; }

void testmode_read_mdc_status(void)
{
    err_code_list[0] = mdc_mailbox.lamp_temperature_flatbed;
    testmode_result_1[0] = mdc_mailbox.lamp_temperature_front;
    testmode_result_2[0] = mdc_mailbox.lamp_temperature_back;
    testmode_result_3[0] = mdc_mailbox.lamp_temperature;
    testmode_result_4[0] = mdc_mailbox.byte_0x368;
    testmode_result_5[0] = mdc_mailbox.lamp_temperature_b;
}


void testmode_read_scsi_id(void)
{
    if (hw_port_f->b0) {
        err_code_list[0] = 0;
    } else {
        err_code_list[0] = 0xff;
    }
    testmode_result_1[0] = hw_read_scsi_id();
}


void testmode_lsi_memories(void)
{
    unsigned short result;
    unsigned short i;
    result = 0;
    for (i = 0; i < 8; i++) {
        *(volatile unsigned char *)(err_code_list + i) = 0;
    }
    result = selftest_vdcc_small_rams();
    if (result != 0) testmode_set_result(result);
    result = selftest_vdcc_bank_rams();
    if (result != 0) testmode_set_result(result);
    result = selftest_vdcc_bank_select();
    if (result != 0) testmode_set_result(result);
    result = selftest_pisces_indirect();
    if (result != 0) testmode_set_result(result);
    result = selftest_pisces_rams();
    if (result != 0) testmode_set_result(result);
    p_vdcc_front_bank[0]->bank = 0;
    p_vdcc_back_bank[0]->bank = 0;
}

void testmode_set_result(unsigned short result)
{
    unsigned char index;
    index = (result >> 8) & 0x7f;
    if (result & 0x8000) index += 4;
    err_code_list[index] |= (unsigned char)result;
}

void testmode_0a(void)
{
    unsigned char error;
    settings_mark_flags[0] = 0;
    error = selftest_measure_mark();
    if (error == 0) error = selftest_find_mark();
    if (error == 0) {
        err_code_list[0] = 0x80;
        testmode_result_1[0] = selftest_mark_offset;
    } else {
        err_code_list[0] = 0xff;
        testmode_result_1[0] = 0x40;
    }
    selftest_mark_offset = 0x40;
    return;
}
