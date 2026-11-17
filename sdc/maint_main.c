#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
/* Constants (section C). */
const struct testmode_entry testmode_table[31] = {
    {1, {0, 0, 0}, testmode_01}, {2, {0, 0, 0}, testmode_02}, {3, {0, 0, 0}, testmode_panel_check}, {4, {0, 0, 0}, testmode_04},
    {5, {0, 0, 0}, testmode_05}, {6, {0, 0, 0}, testmode_06}, {7, {0, 0, 0}, testmode_read_mdc_status}, {8, {0, 0, 0}, testmode_read_scsi_id},
    {9, {0, 0, 0}, testmode_lsi_memories}, {0x0a, {0, 0, 0}, testmode_0a}, {0x0b, {0, 0, 0}, testmode_read_exposure}, {0x0f, {0, 0, 0}, testmode_0f},
    {0x10, {0, 0, 0}, testmode_10}, {0x20, {0, 0, 0}, testmode_adjust_offset}, {0x21, {0, 0, 0}, testmode_adjust_magnification}, {0x22, {0, 0, 0}, testmode_adjust_white_level},
    {0x24, {0, 0, 0}, testmode_adjust_ultrasonic}, {0x25, {0, 0, 0}, testmode_panel_buttons}, {0x26, {0, 0, 0}, testmode_26}, {0x27, {0, 0, 0}, testmode_27},
    {0x28, {0, 0, 0}, testmode_28}, {0x29, {0, 0, 0}, testmode_lamp}, {0x2a, {0, 0, 0}, testmode_2a}, {0x2b, {0, 0, 0}, testmode_pick_arm},
    {0x2c, {0, 0, 0}, testmode_2c}, {0x2e, {0, 0, 0}, testmode_save_defect_lists}, {0x30, {0, 0, 0}, testmode_30}, {0x14, {0, 0, 0}, testmode_save_defect_lists_no_result},
    {0x15, {0, 0, 0}, testmode_15}, {0x17, {0, 0, 0}, testmode_save_settings_backup}, {0, {0, 0, 0}, 0}
};
/* Initialised data (section D). */
void (*maint_mode_table[16])(void) = {
    maint_run_mode_1, maint_run_mode_1, maint_run_mode_2, maint_run_mode_3,
    maint_run_mode_4, maint_run_mode_5, maint_run_mode_6, maint_run_mode_7,
    maint_run_mode_8, maint_end_mode, maint_run_mode_10, maint_run_mode_11,
    maint_end_mode, maint_end_mode, maint_end_mode, maint_end_mode
};
void (*maint_mode_1_states[16])(void) = {
    maint_mode_1_state_0, maint_mode_1_state_1, maint_mode_1_state_2, maint_mode_1_state_3,
    maint_mode_1_state_4, maint_mode_1_state_5, maint_mode_1_state_6, maint_mode_1_state_7,
    maint_end_mode, maint_end_mode, maint_end_mode, maint_end_mode,
    maint_end_mode, maint_end_mode, maint_end_mode, maint_end_mode
};


void maint_start(void)
{
    const struct testmode_entry *entry;
    struct page_record *scanning_page;

    scanning_page = &page_records[page_scanning.v];
    err_display_flags.b.b5 = 0;
    err_display_flags.b.b6 = 0;
    scanning_page->error_flags.v = scanning_page->hardware_error_bits = scanning_page->medium_error_bits.s = scanning_page->illegal_request_bits = 0;
    win_params.b13.f.b0 = 1;
    maint_start_flag = 0;
    if (testmode_number.v) {
        if (power_state.f.saving) {
            _builtin_set_imask(15);
            power_state.f.leave_request = 1;
            _builtin_set_imask(0);
            power_leave_save();
        }
        entry = testmode_table;
        while (entry->handler != 0) {
            if (entry->key == testmode_number.v) {
                break;
            }
            entry++;
        }
        if (entry->handler != 0) {
            entry->handler();
        } else {
        }
        if (scsi_resume_flags.f.resume_pending) {
            isr_request_command_layer(0);
        }
    } else {
        maint_state.v = 0;
        maint_item.v = 0;
        maint_run();
    }
}

/* maint_mode_table: one state table per maintenance mode, #1-#8 as in the service manual (6.1) */
void maint_run(void)
{
    maint_mode_table[maint_mode_word.c.mode]();
}

void maint_end_mode(void)
{
    maint_mode_word.v &= 0x7fff;
    maint_screen = MAINT_SCREEN_SELECT;
}

void maint_run_mode_1(void)
{
    maint_mode_1_states[maint_state.v]();
}

void maint_mode_1_state_0(void)
{
    unsigned short item;

    panel_current.unread_word_52 = 0;
    maint_screen = MAINT_SCREEN_ITEM;
    mdc_motor_test_flags.f.maint_started = 1;
    hw_hostif_flags.f.own_hostif = 0;
    win_params.side[0].use_window_area = 1;
    win_params.side[0].y_offset_inch1200 = win_params.side[1].y_offset_inch1200 = 0;
    win_params.side[0].length_inch1200 = win_params.side[1].length_inch1200 = 0x4d82;
    win_params.side[0].width_inch1200 = win_params.side[1].width_inch1200 = 100;
    if (maint_mode_word.c.mode == 0) {
        maint_screen = MAINT_SCREEN_SELECT;
        win_params.resolution_mode.v &= 0xfff8;
        win_params.resolution_mode.v |= 0xb;
        panel_set_display_mode(0, PANEL_MODE_MAINT);
    } else if (maint_mode_word.c.mode == MAINT_MODE_FEED_SENSOR_TEST) {
        win_params.resolution_mode.v &= 0xfff8;
        panel_set_display_mode(1, PANEL_MODE_MAINT);
    }
    while (!button_state.b.b7) {
        if (button_state.b.b4) {
            while (button_state.b.b4) {
            }
            if (maint_mode_word.c.mode != 0) {
                item = win_params.resolution_mode.v & 15;
                item++;
                if (item > 7) {
                    item = 0;
                }
                win_params.resolution_mode.v = item;
                panel_set_display_mode(1, PANEL_MODE_MAINT);
            }
        }
        if (button_state.b.b6) {
            while (button_state.b.b6) {
            }
            maint_state.v = 5;
            return;
        }
    }
    while (button_state.b.b7) {
    }
    maint_state.v++;
}
