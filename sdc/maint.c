#include "fw.h"
#pragma section H
struct vdcc_gpio_reg_05_bits {
    char pad_00[5];
    unsigned char b7:1,
                  :7;
};

extern void _builtin_set_imask(int);
/* Initialised data (section D). */
void (*maint_mode_2_states[16])(void) = {
    maint_mode_2_state_0, maint_mode_2_state_1, maint_mode_2_state_2, maint_mode_2_state_3,
    maint_mode_2_state_4, maint_mode_2_state_5, maint_end_mode, maint_end_mode,
    maint_end_mode, maint_end_mode, maint_end_mode, maint_end_mode,
    maint_end_mode, maint_end_mode, maint_end_mode, maint_end_mode
};
int (*maint_mode_3_states[16])(void) = {
    maint_mode_3_state_0,
    maint_mode_3_state_1,
    maint_mode_3_state_2,
    maint_mode_3_state_3,
    maint_mode_3_state_4,
    maint_mode_3_state_5,
    (int(*)(void))maint_end_mode,
    (int(*)(void))maint_end_mode,
    (int(*)(void))maint_end_mode,
    (int(*)(void))maint_end_mode,
    (int(*)(void))maint_end_mode,
    (int(*)(void))maint_end_mode,
    (int(*)(void))maint_end_mode,
    (int(*)(void))maint_end_mode,
    (int(*)(void))maint_end_mode,
    (int(*)(void))maint_end_mode
};
void (*maint_mode_4_states[16])(void) = {
    maint_mode_4_state_0, maint_mode_4_state_1, maint_mode_4_state_2, maint_mode_4_state_3,
    maint_mode_4_state_4, maint_mode_4_state_5, maint_end_mode, maint_end_mode,
    maint_end_mode, maint_end_mode, maint_end_mode, maint_end_mode,
    maint_end_mode, maint_end_mode, maint_end_mode, maint_end_mode
};
void (*maint_mode_5_states[16])() = {
    maint_mode_5_state_0,
    maint_mode_5_state_1,
    maint_mode_5_state_2,
    maint_mode_5_state_3,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode
};
void (*maint_mode_6_states[16])() = {
    maint_mode_6_state_0,
    maint_mode_6_state_1,
    maint_mode_6_state_2,
    maint_mode_6_state_3,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode
};
void (*maint_mode_7_states[16])() = {
    maint_mode_7_state_0,
    maint_mode_7_state_1,
    maint_mode_7_state_2,
    maint_mode_7_state_3,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode
};
void (*maint_mode_8_states[16])() = {
    maint_mode_8_state_0,
    maint_mode_8_state_1,
    maint_mode_8_state_2,
    maint_mode_8_state_3,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode,
    (void(*)())maint_end_mode
};
void (*maint_mode_10_states[16])(void) = {
    maint_mode_10_state_0, maint_mode_10_state_1, maint_mode_10_state_2, maint_mode_10_state_3,
    maint_end_mode, maint_end_mode, maint_end_mode, maint_end_mode,
    maint_end_mode, maint_end_mode, maint_end_mode, maint_end_mode,
    maint_end_mode, maint_end_mode, maint_end_mode, maint_end_mode
};

void maint_mode_1_state_1(void)
{
    unsigned short item;
    unsigned short res_dpi;

    item = win_params.resolution_mode.v & 15;
    if (item == 7) {
        maint_screen = MAINT_SCREEN_ADJUSTED;
        maint_state.v = 6;
        tick_wait_count = 0;
        maint_sensor_test = 1;
        win_params.flags.f.adf = 1;
        afe_select_source();
        mdc_run_command_flag(MDC_CMD_DF_TEST_ON, 0);
        return;
    }
    if (item == 6) {
        win_params.flags.f.adf = 1;
        maint_item.v = 6;
        maint_screen = MAINT_SCREEN_RUNNING;
        maint_state.v = 7;
        panel_set_display_mode(1, PANEL_MODE_MAINT);
        return;
    }
    if (item == 0) res_dpi = 200;
    else if (item == 1) res_dpi = 240;
    else if (item == 2) res_dpi = 300;
    else if (item == 3) res_dpi = 400;
    else if (item == 4) res_dpi = 600;
    else if (item == 5) res_dpi = 600;
    else res_dpi = 300;
    win_params.x_res_dpi.v = win_params.y_res_dpi.v = res_dpi;
    maint_item.v = win_params.resolution_mode.c.lo & 15;
    maint_screen = MAINT_SCREEN_RUNNING;
    panel_set_display_mode(1, PANEL_MODE_MAINT);
    maint_state.v++;
}

void maint_mode_1_state_2(void)
{
    if ((((volatile char *)&mdc_mailbox)[0x487] & MDC_SENSOR_NO_PAPER) == 0 && (((volatile char *)&mdc_mailbox)[0x487] & MDC_SENSOR_ADF_COVER_OPEN) == 0) {
        win_params.flags.f.adf = 1;
        win_sides.f.selection = 2;
    } else {
        win_params.flags.f.adf = 0;
        win_sides.f.selection = 0;
        maint_state.v += 1;
        return;
    }
    maint_state.v++;
}

void maint_mode_1_state_3(void)
{
    struct page_record *scanning_page;
    int done;

    scanning_page = &page_records[page_scanning.v];
    if (panel_current.display_main == PANEL_MODE_ERROR) {
        maint_state.v += 2;
        return;
    }
    util_mem_clear(scanning_page, 0x88);
    scanning_page->flags.f.adf = 1;
    scanning_page->flags.f.scan_requested = 1;
    scanning_page->sides[0].flags.f.requested = 1;
    hw_hostif_flags.f.own_hostif = 1;
    ((struct vdcc_gpio_reg_05_bits *)p_vdcc_front_gpio[0])->b7 = 0;
    if (win_params.flags.f.adf != 0) {
        job_buffer_mode_on = 1;
        job_mode_params[0].auto_length = 0x80;
    } else {
        job_buffer_mode_on = 0;
        job_mode_params[0].auto_length = 0;
    }
    if (maint_pick_test == 1) {
        done = 0;
        testmode_param_1.v = 13;
        err_check_pending();
        button_state.b.b6 = 0;
        while (1) {
            _builtin_set_imask(0);
            if (button_state.b.b6) {
                mdc_end_motor_test(MDC_MOTOR_TEST_PICK, MDC_MOTOR_TEST_SPEED_CLASS);
                mdc_run_pick_arm(0, 1);
                break;
            }
            if (done == 0) {
                mdc_run_pick_arm(1, 1);
                mdc_run_motor_test(MDC_MOTOR_TEST_PICK, MDC_MOTOR_TEST_SPEED_CLASS);
                done = 1;
            }
        }
    } else {
        main_run_page_loop();
    }
    hw_hostif_flags.f.own_hostif = 0;
    job_buffer_mode_on = 0;
    maint_state.v++;
}

void maint_mode_1_state_4(void)
{
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    if (scanning_page->error_flags.v == 0) {
        if (!win_params.flags.f.adf || (win_params.flags.f.adf && !mdc_mailbox.sensor_status.f.no_paper)) {
            maint_state.v = 2;
            return;
        }
    }
    if (scanning_page->medium_error_bits.v & 0x80) {
        scanning_page->medium_error_bits.v &= 0xff7f;
        if (!win_params.flags.f.adf) {}
    }
    maint_state.v++;
}

void maint_mode_1_state_5(void)
{
    maint_mode_word.v &= 0x7fff;
    maint_screen = MAINT_SCREEN_SELECT;
    if (panel_current.display_main != PANEL_MODE_ERROR) {
        panel_set_display_mode(1, PANEL_MODE_MAINT);
    } else if (panel_current.chars[0] != PANEL_GLYPH_U && panel_current.chars[0] != PANEL_GLYPH_J && panel_current.chars[0] != PANEL_GLYPH_E) {
        panel_set_display_mode(1, PANEL_MODE_MAINT);
    }
}

void maint_mode_1_state_6(void)
{
    if (button_state.b.b4) {
        if ((((volatile char *)&mdc_mailbox)[0x487] & MDC_SENSOR_ADF_COVER_OPEN) == 0) {
            if (mdc_motor_test_state.w == 0) {
                mdc_run_motor_test(MDC_MOTOR_TEST_PICK_AND_FEED, MDC_MOTOR_TEST_SPEED_CLASS);
                if (tick_wait_count < 25)
                    return;
            }
            panel_set_display_mode(1, PANEL_MODE_MAINT);
            tick_wait_count = 0;
            return;
        }
    }
    if (mdc_motor_test_state.w)
        mdc_end_motor_test(MDC_MOTOR_TEST_PICK_AND_FEED, MDC_MOTOR_TEST_SPEED_CLASS);
    if (tick_wait_count < 25)
        return;
    panel_set_display_mode(1, PANEL_MODE_MAINT);
    tick_wait_count = 0;
    if (button_state.b.b6) {
        while (button_state.b.b6)
            ;
        maint_state.v = 5;
        mdc_run_command_flag(MDC_CMD_DF_TEST_OFF, 0);
        maint_sensor_test = 0;
    }
}

void maint_mode_1_state_7(void)
{
    int i;
    while (mdc_mailbox.sensor_status.f.adf_cover_open) {
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_state.v = 5;
            return;
        }
    }
    tick_wait_count = 0;
    while (tick_wait_count < 250) {
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_state.v = 5;
            return;
        }
    }
    for (i = 0; i < 20; i++) {
        mdc_run_backing(1);
        tick_wait_count = 0;
        while (tick_wait_count < 250)
            ;
        mdc_run_backing(0);
        tick_wait_count = 0;
        while (tick_wait_count < 250) {
            if (button_state.b.b6) {
                while (button_state.b.b6)
                    ;
                maint_state.v = 5;
                return;
            }
        }
    }
    maint_state.v = 5;
    return;
}

void maint_run_mode_2(void) { maint_mode_2_states[(unsigned char)maint_state.v](); }

void maint_mode_2_state_0(void)
{
    short item;
    panel_current.unread_word_52 = 0;
    maint_adjust_error = 0;
    if (testmode_number.v == 0) {
        maint_screen = MAINT_SCREEN_ITEM;
    }
    mdc_motor_test_flags.f.maint_started = 1;
    maint_adjust_target.v = 0;
    if (testmode_number.v == 0) {
        hw_hostif_flags.f.own_hostif = 1;
        ((struct vdcc_gpio_reg_05_bits *)p_vdcc_front_gpio[0])->b7 = 0;
    }
    win_params.paper_size.f.auto_length = 0;
    win_params.paper_size.f.custom_size = 1;
    win_params.paper_width_inch1200.v = 0x3900;
    win_params.paper_length_inch1200.v = 0x4fb0;
    win_params.side[0].composition.f.multi_level = 1;
    win_params.side[0].composition.f.color = 0;
    win_params.side[1].composition.f.multi_level = 1;
    win_params.side[1].composition.f.color = 0;
    win_params.side[0].use_window_area = 0;
    win_params.side[0].x_offset_inch1200 = 0;
    win_params.side[0].width_inch1200 = 0x3900;
    win_params.side[0].y_offset_inch1200 = 0;
    win_params.side[0].length_inch1200 = 0x4fb0;
    win_params.side[1].use_window_area = 0;
    win_params.side[1].x_offset_inch1200 = 0;
    win_params.side[1].width_inch1200 = 0x3900;
    win_params.side[1].y_offset_inch1200 = 0;
    win_params.side[1].length_inch1200 = 0x4fb0;
    win_params.resolution_mode.v &= 0xfff8;
    win_params.x_res_dpi.v = 400;
    win_params.y_res_dpi.v = 400;
    if (testmode_number.v != 0) {
        if (testmode_adjust_mode.v == 1) {
            maint_adjust_target.v = 2;
        } else {
            if (testmode_adjust_mode.v == 2) {
                maint_adjust_target.v = 0;
            } else {
                maint_adjust_target.v = 1;
            }
        }
        return;
    }
    maint_item.v = maint_adjust_target.v;
    panel_set_display_mode(2, PANEL_MODE_MAINT);
    while (!button_state.b.b7) {
        if (button_state.b.b4) {
            while (button_state.b.b4)
                ;
            switch (maint_adjust_target.v) {
            case 0:
                maint_adjust_target.v = 2;
                item = 1;
                break;
            case 1:
                maint_adjust_target.v = 2;
                item = 1;
                break;
            case 2:
            default:
                maint_adjust_target.v = 0;
                item = 0;
                break;
            }
            maint_item.v = item;
            panel_set_display_mode(2, PANEL_MODE_MAINT);
        }
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_state.v = 5;
            return;
        }
    }
    while (button_state.b.b7)
        ;
    maint_state.v++;
}

void maint_mode_2_state_1(void)
{
    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_RUNNING;
        panel_set_display_mode(2, PANEL_MODE_MAINT);
    }
    maint_mode_2_set_settings(0);
    if (maint_adjust_target.v == 0 || maint_adjust_target.v == 1) {
        if (mdc_mailbox.sensor_status.f.no_paper) {
            maint_mode_2_set_settings(1);
            if (!testmode_number.v) {
                maint_state.v = 5;
                return;
            } else {
                err_code_list[0] = 5;
                return;
            }
        }
        win_params.flags.f.adf = 1;
        if (maint_adjust_target.v == 0) {
            win_sides.f.selection = 0;
        } else {
            win_sides.f.selection = 2;
        }
    } else if (maint_adjust_target.v == 2) {
        win_params.flags.f.adf = 0;
        win_sides.f.selection = 0;
    }
    if (!testmode_number.v) {
        maint_state.v++;
    }
}


void maint_mode_2_state_2(void)
{
    int i;
    struct page_record *scanning_page;
    unsigned char saved_backing;
    scanning_page = &page_records[page_scanning.v];
    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_RUNNING;
        panel_set_display_mode(2, PANEL_MODE_MAINT);
        if (panel_current.display_main == PANEL_MODE_ERROR) {
            maint_mode_2_set_settings(1);
            maint_state.v = 5;
            return;
        }
    }
    saved_backing = job_mode_params[0].backing;
    switch (maint_adjust_target.v) {
    case 0:
    case 1:
        job_mode_params[0].backing = 0xa0;
        break;
    case 2:
    default:
        break;
    }
    util_mem_clear(scanning_page, 136);
    if (maint_adjust_target.v != 2) {
        scanning_page->flags.f.adf = 1;
    }
    scanning_page->flags.f.scan_requested = 1;
    scanning_page->sides[0].flags.f.requested = 1;
    if (!testmode_number.v) {
        hw_hostif_flags.f.own_hostif = 1;
        ((struct vdcc_gpio_reg_05_bits *)p_vdcc_front_gpio[0])->b7 = 0;
    }
    job_buffer_mode_on = 0;
    err_last_code.s.v = 0;
    main_run_page_loop();
    if (!testmode_number.v) {
        hw_hostif_flags.f.own_hostif = 0;
    }
    job_buffer_mode_on = 0;
    job_mode_params[0].backing = saved_backing;
    if (err_last_code.s.v) {
        maint_adjust_error = 33;
    }
    if (testmode_number.v)
        return;
    maint_state.v++;
}

void maint_mode_2_state_3(void)
{
    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_RUNNING;
        panel_set_display_mode(2, PANEL_MODE_MAINT);
    }
    switch (maint_adjust_target.v) {
    case 0:
        maint_pisces_side.v = 0;
        break;
    case 1:
        maint_pisces_side.v = 1;
        break;
    case 2:
    default:
        maint_pisces_side.v = 0;
        break;
    }
    while ((short)p_pisces_pass_dest[maint_pisces_side.v]->lines_done != p_pisces_pass_dest[maint_pisces_side.v]->length_lines.v)
        ;
    _builtin_set_imask(15);
    p_pisces_mode[maint_pisces_side.v]->run.v = 0;
    p_pisces_ctrl[maint_pisces_side.v]->ctrl.f.sdram_window = 1;
    _builtin_set_imask(0);
    if (maint_adjust_error != 0)
        goto L1;
    if (!maint_mode_2_state_3_step_2())
        goto L1;
    if (maint_mode_2_state_3_step_3() == 0)
        goto L1;
    {
        p_pisces_ctrl[maint_pisces_side.v]->ctrl.f.sdram_window = 0;
        if (testmode_number.v)
            return;
        maint_screen = MAINT_SCREEN_ADJUSTED;
        panel_set_display_mode(2, PANEL_MODE_MAINT);
        while (!button_state.b.b4 || button_state.b.b7) {
            if (button_state.b.b6) {
                while (button_state.b.b6)
                    ;
                maint_mode_2_set_settings(1);
                maint_screen = MAINT_SCREEN_SELECT;
                maint_state.v = 0;
                return;
            }
        }
        while (button_state.b.b4)
            ;
        maint_state.v++;
        return;
    }
L1:
    p_pisces_ctrl[maint_pisces_side.v]->ctrl.f.sdram_window = 0;
    maint_mode_2_set_settings(1);
    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_FAILED;
        panel_set_display_mode(2, PANEL_MODE_MAINT);
        while (!button_state.b.b4 && !button_state.b.b6) {
            if (button_state.b.b6) {
                while (button_state.b.b6)
                    ;
                maint_screen = MAINT_SCREEN_SELECT;
                maint_state.v = 0;
                return;
            }
        }
        while (button_state.b.b4)
            ;
    } else {
        switch (maint_adjust_error) {
        case 33:
            err_code_list[0] = 3;
            break;
        case 34:
            err_code_list[0] = 1;
            break;
        case 36:
            err_code_list[0] = 4;
            break;
        case 38:
            err_code_list[0] = 2;
            break;
        case 35:
        case 37:
            err_code_list[0] = 7;
            break;
        }
        return;
    }
    switch (maint_adjust_error) {
    case 33:
        maint_screen = 0xe3;
        break;
    case 34:
        maint_screen = 0xe5;
        break;
    case 38:
    case 35:
        maint_screen = 0xe4;
        break;
    case 36:
    case 37:
        maint_screen = 0xe7;
        break;
    }
    panel_set_display_mode(2, PANEL_MODE_MAINT);
    while (!button_state.b.b6)
        ;
    while (button_state.b.b6)
        ;
    maint_screen = MAINT_SCREEN_SELECT;
    maint_state.v = 0;
    return;
}

void maint_mode_2_state_4(void)
{
    maint_screen = MAINT_SCREEN_CONFIRM;
    panel_set_display_mode(2, PANEL_MODE_MAINT);
    while (1) {
        if (button_state.b.b7 && button_state.b.b4)
            break;
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_mode_2_set_settings(1);
            maint_screen = MAINT_SCREEN_SELECT;
            maint_state.v = 0;
            return;
        }
    }
    while (button_state.b.b4 || button_state.b.b7)
        ;
    maint_mode_2_set_settings(2);
    maint_screen = MAINT_SCREEN_WRITING;
    panel_set_display_mode(2, PANEL_MODE_MAINT);
    if (maint_save_settings(maint_item.v))
        maint_screen = MAINT_SCREEN_WRITTEN;
    else
        maint_screen = MAINT_SCREEN_FAILED;
    panel_set_display_mode(2, PANEL_MODE_MAINT);
    while (!button_state.b.b6)
        ;
    while (button_state.b.b6)
        ;
    maint_state.v++;
}

void maint_mode_2_state_5(void)
{
    maint_mode_word.v &= 0x7fff;
    maint_screen = MAINT_SCREEN_SELECT;
    if (panel_current.display_main != PANEL_MODE_ERROR) {
        panel_set_display_mode(2, PANEL_MODE_MAINT);
    } else {
        if (panel_current.chars[0] != PANEL_GLYPH_U && panel_current.chars[0] != PANEL_GLYPH_J && panel_current.chars[0] != PANEL_GLYPH_E)
            panel_set_display_mode(2, PANEL_MODE_MAINT);
    }
}

void maint_mode_2_set_settings(int action)
{
    if (action == 0) {
        switch (maint_adjust_target.v) {
        case 0:
            maint_saved_value_0[0] = settings_main_offset_adf_front[0];
            maint_saved_value_1[0] = settings_sub_offset_adf_front[0];
            maint_saved_value_2[0] = settings_sub_magnification_adf[0];
            maint_saved_value_3[0] = settings_white_level_adf_front_chip0[0];
            maint_saved_value_4[0] = settings_white_level_adf_front_chip1[0];
            maint_saved_value_5[0] = settings_white_level_adf_front_chip2[0];
            settings_main_offset_adf_front[0] = 0;
            settings_sub_offset_adf_front[0] = 0xff;
            settings_sub_magnification_adf[0] = 0x9e;
            settings_white_level_adf_front_chip0[0] = 0xff;
            settings_white_level_adf_front_chip1[0] = 0xff;
            settings_white_level_adf_front_chip2[0] = 0xff;
            break;
        case 1:
            maint_saved_value_0[0] = settings_main_offset_adf_back[0];
            maint_saved_value_1[0] = settings_sub_offset_adf_back[0];
            maint_saved_value_2[0] = settings_sub_magnification_adf[0];
            maint_saved_value_3[0] = settings_white_level_adf_back_chip0[0];
            maint_saved_value_4[0] = settings_white_level_adf_back_chip1[0];
            maint_saved_value_5[0] = settings_white_level_adf_back_chip2[0];
            settings_main_offset_adf_back[0] = 0;
            settings_sub_offset_adf_back[0] = 0xff;
            settings_sub_magnification_adf[0] = 0x9e;
            settings_white_level_adf_back_chip0[0] = 0xff;
            settings_white_level_adf_back_chip1[0] = 0xff;
            settings_white_level_adf_back_chip2[0] = 0xff;
            break;
        case 2:
        default:
            maint_saved_value_0[0] = settings[0];
            maint_saved_value_1[0] = settings_sub_offset_flatbed[0];
            maint_saved_value_2[0] = settings_sub_magnification_flatbed[0];
            maint_saved_value_3[0] = settings_white_level_flatbed_chip0[0];
            maint_saved_value_4[0] = settings_white_level_flatbed_chip1[0];
            maint_saved_value_5[0] = settings_white_level_flatbed_chip2[0];
            settings[0] = 0xff;
            settings_sub_offset_flatbed[0] = 0xff;
            settings_sub_magnification_flatbed[0] = 0x9e;
            settings_white_level_flatbed_chip0[0] = 0xa0;
            settings_white_level_flatbed_chip1[0] = 0xa0;
            settings_white_level_flatbed_chip2[0] = 0xa0;
            break;
        }
        maint_saved_value_6[0] = settings_byte_0x34[0];
        settings_byte_0x34[0] = 1;
        maint_saved_value_7[0] = settings_test_flags[0];
        settings_test_flags[0] = 8;
    } else {
        settings_test_flags[0] = maint_saved_value_7[0];
        settings_byte_0x34[0] = maint_saved_value_6[0];
        switch (maint_adjust_target.v) {
        case 0:
            settings_main_offset_adf_front[0] = maint_saved_value_0[0];
            settings_sub_offset_adf_front[0] = maint_saved_value_1[0];
            settings_white_level_adf_front_chip0[0] = maint_saved_value_3[0];
            settings_white_level_adf_front_chip1[0] = maint_saved_value_4[0];
            settings_white_level_adf_front_chip2[0] = maint_saved_value_5[0];
            break;
        case 1:
            settings_main_offset_adf_back[0] = maint_saved_value_0[0];
            settings_sub_offset_adf_back[0] = maint_saved_value_1[0];
            settings_white_level_adf_back_chip0[0] = maint_saved_value_3[0];
            settings_white_level_adf_back_chip1[0] = maint_saved_value_4[0];
            settings_white_level_adf_back_chip2[0] = maint_saved_value_5[0];
            break;
        case 2:
        default:
            settings[0] = maint_saved_value_0[0];
            settings_sub_offset_flatbed[0] = maint_saved_value_1[0];
            settings_white_level_flatbed_chip0[0] = maint_saved_value_3[0];
            settings_white_level_flatbed_chip1[0] = maint_saved_value_4[0];
            settings_white_level_flatbed_chip2[0] = maint_saved_value_5[0];
            break;
        }
        if (action == 1) {
            switch (maint_adjust_target.v) {
            case 0:
                settings_sub_magnification_adf[0] = maint_saved_value_2[0];
                break;
            case 1:
                settings_sub_magnification_adf[0] = maint_saved_value_2[0];
                break;
            case 2:
            default:
                settings_sub_magnification_flatbed[0] = maint_saved_value_2[0];
                break;
            }
        }
    }
    util_mem_copy_to_odd((void *)0xc40081, settings, 0x200);
}

char maint_read_sdram_byte(int side, int byte_offset, char *window, int base_64k)
{
    unsigned short page;
    unsigned int address;
    base_64k = base_64k << 16;
    address = base_64k + byte_offset;
    page = address >> 16;
    p_pisces_ctrl[side]->sdram_page = page;
    return window[(unsigned short)address];
}

int maint_find_rising_edge(unsigned char *samples, int len)
{
    int i;
    int k;
    int j;
    int slope;
    int later_slope;
    unsigned int sample;
    char rising;
    char in_edge;
    int edge;
    unsigned short *slope_log;
    in_edge = 0;
    edge = -1;
    slope_log = edge_right_front;
    for (i = 1; i < len; i++) {
        slope = 0;
        sample = samples[i];
        sample <<= 3;
        slope = sample;
        for (k = i - 1; k > i - 8; k--) {
            if (k < 0)
                sample = samples[0];
            else
                sample = samples[k];
            if (k == i - 1)
                sample <<= 1;
            slope -= sample;
        }
        *slope_log++ = slope;
        rising = 0;
        if (slope > 0x100) {
            rising = 1;
            for (j = 1; j < 32; j++) {
                sample = samples[i + j];
                sample <<= 3;
                later_slope = sample;
                for (k = i - 1; k > i - 8; k--) {
                    if (k < 0)
                        sample = samples[0];
                    else
                        sample = samples[k];
                    if (k == i - 1)
                        sample <<= 1;
                    later_slope -= sample;
                }
                if (later_slope < 0x100) {
                    rising = 0;
                    break;
                }
            }
            if (rising && !in_edge) {
                in_edge = 1;
                edge = i;
            }
        }
        if (!rising)
            in_edge = 0;
    }
    return edge;
}


int maint_mode_2_state_3_step_2(void)
{
    int remainder;
    int deviation;
    unsigned short j;
    unsigned short i;
    unsigned short unused2;
    unsigned short measured_lines;
    unsigned short top_edge;
    unsigned short bottom_edge;
    unsigned int left_mean;
    unsigned int right_mean;
    int addr;
    unsigned short width_px;
    unsigned short magnification;
    unsigned char *buf;
    int edge;
    int unused;
    char *window;
    int base_64k;
    base_64k = p_pisces_pass_dest[maint_pisces_side.v]->base_64k.v;
    width_px = img_side_params[maint_pisces_side.v].width_px;
    buf = (unsigned char *)edge_left_front;
    for (i = 0; i < 8; i++) {
        maint_top_edges.edges[i] = 0;
        switch (maint_adjust_target.v) {
        case 0:
        case 1:
            if (i < 4)
                addr = i * 8 + 0x101;
            else
                addr = 0x11ff - (i - 4) * 8;
            break;
        case 2:
        default:
            if (i < 4)
                addr = i * 8 + 0x153;
            else
                addr = 0x11b4 - (i - 4) * 8;
            break;
        }
        if (maint_pisces_side.v == 0)
            window = p_pisces_front_sdram.p;
        else
            window = p_pisces_back_sdram.p;
        for (j = 0; j < 0x177; j++) {
            buf[j] = maint_read_sdram_byte(maint_pisces_side.v, addr, window, base_64k);
            addr += width_px;
        }
        edge = maint_find_rising_edge(buf, 0x157);
        if (edge < 0) {
            maint_adjust_error = 33;
            return 0;
        }
        maint_top_edges.edges[i] = edge;
        maint_bottom_edges.edges[i] = 0;
        switch (i) {
        case 0:
        case 1:
        case 2:
        case 3:
            addr = i * 8 + 0x153;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            addr = 0x11b4 - (i - 4) * 8;
            break;
        }
        addr += width_px * 0x1a5c;
        if (maint_pisces_side.v == 0)
            window = p_pisces_front_sdram.p;
        else
            window = p_pisces_back_sdram.p;
        for (j = 0; j < 0x246; j++) {
            buf[j] = maint_read_sdram_byte(maint_pisces_side.v, addr, window, base_64k);
            addr -= width_px;
        }
        edge = maint_find_rising_edge(buf, 0x226);
        if (edge < 0) {
            maint_adjust_error = 38;
            return 0;
        }
        maint_bottom_edges.edges[i] = edge;
    }
    left_mean = maint_top_edges.edges[0] + maint_top_edges.edges[1] + maint_top_edges.edges[2] + maint_top_edges.edges[3];
    right_mean = maint_top_edges.edges[4] + maint_top_edges.edges[5] + maint_top_edges.edges[6] + maint_top_edges.edges[7];
    left_mean >>= 2;
    right_mean >>= 2;
    if (left_mean < right_mean)
        top_edge = right_mean - left_mean;
    else
        top_edge = left_mean - right_mean;
    left_mean = maint_bottom_edges.edges[0] + maint_bottom_edges.edges[1] + maint_bottom_edges.edges[2] + maint_bottom_edges.edges[3];
    right_mean = maint_bottom_edges.edges[4] + maint_bottom_edges.edges[5] + maint_bottom_edges.edges[6] + maint_bottom_edges.edges[7];
    left_mean >>= 2;
    right_mean >>= 2;
    if (left_mean < right_mean)
        bottom_edge = right_mean - left_mean;
    else
        bottom_edge = left_mean - right_mean;
    if (top_edge > 43 || bottom_edge > 43) {
        maint_adjust_error = 35;
        return 0;
    }
    top_edge = maint_top_edges.edges[0] + maint_top_edges.edges[1] + maint_top_edges.edges[2] + maint_top_edges.edges[3] + maint_top_edges.edges[4] + maint_top_edges.edges[5] + maint_top_edges.edges[6] + maint_top_edges.edges[7];
    bottom_edge = maint_bottom_edges.edges[0] + maint_bottom_edges.edges[1] + maint_bottom_edges.edges[2] + maint_bottom_edges.edges[3] + maint_bottom_edges.edges[4] + maint_bottom_edges.edges[5] + maint_bottom_edges.edges[6] + maint_bottom_edges.edges[7];
    top_edge >>= 3;
    bottom_edge >>= 3;
    measured_lines = 0x1a5d - top_edge - bottom_edge;
    if (measured_lines < 0x1911)
        deviation = 0x1911 - measured_lines;
    else
        deviation = measured_lines - 0x1911;
    remainder = deviation * 1000;
    magnification = 0;
    while ((unsigned int)remainder >= 0x1911) {
        remainder -= 0x1911;
        magnification++;
    }
    if (magnification > 37) {
        maint_adjust_error = 38;
        return 0;
    }
    if (measured_lines < 0x1911)
        magnification &= 0x7f;
    else
        magnification |= 0x80;
    switch (maint_adjust_target.v) {
    case 0:
    case 1:
        settings_sub_magnification_adf[0] = magnification;
        break;
    case 2:
    default:
        settings_sub_magnification_flatbed[0] = magnification;
        break;
    }
    return 1;
}

int maint_mode_2_state_3_step_3(void) { return 1; }

void maint_run_mode_3(void) { maint_mode_3_states[(unsigned char)maint_state.v](); }

void maint_mode_3_state_0(void)
{
    short item;
    panel_current.unread_word_52 = 0;
    maint_adjust_error = 0;
    if (testmode_number.v == 0) {
        maint_screen = MAINT_SCREEN_ITEM;
    }
    mdc_motor_test_flags.f.maint_started = 1;
    maint_adjust_target.v = 0;
    if (testmode_number.v == 0) {
        hw_hostif_flags.f.own_hostif = 0;
    }
    win_params.paper_size.f.auto_length = 0;
    win_params.paper_size.f.custom_size = 1;
    win_params.paper_width_inch1200.v = 0x3900;
    win_params.paper_length_inch1200.v = 0x51e1;
    win_params.side[0].composition.f.multi_level = 1;
    win_params.side[0].composition.f.color = 0;
    win_params.side[1].composition.f.multi_level = 1;
    win_params.side[1].composition.f.color = 0;
    win_params.side[0].use_window_area = 0;
    win_params.side[0].x_offset_inch1200 = 0;
    win_params.side[0].width_inch1200 = 0x3900;
    win_params.side[0].y_offset_inch1200 = 0;
    win_params.side[0].length_inch1200 = 0x51e1;
    win_params.side[1].use_window_area = 0;
    win_params.side[1].x_offset_inch1200 = 0;
    win_params.side[1].width_inch1200 = 0x3900;
    win_params.side[1].y_offset_inch1200 = 0;
    win_params.side[1].length_inch1200 = 0x51e1;
    win_params.resolution_mode.v &= 0xfff8;
    win_params.x_res_dpi.v = 400;
    win_params.y_res_dpi.v = 400;
    if (testmode_number.v != 0) {
        if (testmode_adjust_mode.v == 1) {
            maint_adjust_target.v = 2;
        } else {
            if (testmode_adjust_mode.v == 2) {
                maint_adjust_target.v = 0;
            } else {
                maint_adjust_target.v = 1;
            }
        }
        return;
    }
    maint_item.v = maint_adjust_target.v;
    panel_set_display_mode(3, PANEL_MODE_MAINT);
    while (!button_state.b.b7) {
        if (button_state.b.b4) {
            while (button_state.b.b4)
                ;
            switch (maint_adjust_target.v) {
            case 0:
                maint_adjust_target.v = 1;
                item = 1;
                break;
            case 1:
                maint_adjust_target.v = 2;
                item = 2;
                break;
            case 2:
            default:
                maint_adjust_target.v = 0;
                item = 0;
                break;
            }
            maint_item.v = item;
            panel_set_display_mode(3, PANEL_MODE_MAINT);
        }
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_state.v = 5;
            return;
        }
    }
    while (button_state.b.b7)
        ;
    maint_state.v++;
}

void maint_mode_3_state_1(void)
{
    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_RUNNING;
        panel_set_display_mode(3, PANEL_MODE_MAINT);
    }
    maint_mode_3_set_settings(0);
    if (maint_adjust_target.v == 0 || maint_adjust_target.v == 1) {
        if (mdc_mailbox.sensor_status.f.no_paper) {
            maint_mode_3_set_settings(1);
            if (!testmode_number.v) {
                maint_state.v = 5;
                return;
            } else {
                err_code_list[0] = 5;
                return;
            }
        }
        win_params.flags.f.adf = 1;
        if (maint_adjust_target.v == 0) {
            win_sides.f.selection = 0;
        } else {
            win_sides.f.selection = 2;
        }
    } else if (maint_adjust_target.v == 2) {
        win_params.flags.f.adf = 0;
        win_sides.f.selection = 0;
    }
    if (!testmode_number.v)
        maint_state.v++;
}

void maint_mode_3_state_2(void)
{
    int unused;
    struct page_record *scanning_page;
    unsigned char saved_backing;

    scanning_page = &page_records[page_scanning.v];
    if (!testmode_number.v) {
        if (panel_current.display_main == PANEL_MODE_ERROR) {
            maint_mode_3_set_settings(1);
            maint_state.v = 5;
            return;
        }
    }
    saved_backing = job_mode_params[0].backing;
    switch (maint_adjust_target.v) {
    case 0:
    case 1:
        job_mode_params[0].backing = 0xa0;
        break;
    case 2:
    default:
        break;
    }
    util_mem_clear(scanning_page, 0x88);
    if (maint_adjust_target.v != 2)
        scanning_page->flags.f.adf = 1;
    scanning_page->flags.f.scan_requested = 1;
    switch (maint_adjust_target.v) {
    case 1:
        scanning_page->sides[0].flags.f.requested = 1;
        scanning_page->sides[1].flags.f.requested = 1;
        break;
    case 0:
    case 2:
    default:
        scanning_page->sides[0].flags.f.requested = 1;
        break;
    }
    if (testmode_number.v == 0) {
        hw_hostif_flags.f.own_hostif = 1;
        ((struct vdcc_gpio_reg_05_bits *)p_vdcc_front_gpio[0])->b7 = 0;
    }
    job_buffer_mode_on = 0;
    err_last_code.s.v = 0;
    main_run_page_loop();
    if (testmode_number.v == 0)
        hw_hostif_flags.f.own_hostif = 0;
    job_buffer_mode_on = 0;
    job_mode_params[0].backing = saved_backing;
    if (err_last_code.s.v)
        maint_adjust_error = 33;
    if (testmode_number.v)
        return;
    maint_state.v++;
}

void maint_mode_3_state_3(void)
{
    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_RUNNING;
        panel_set_display_mode(3, PANEL_MODE_MAINT);
    }
    switch (maint_adjust_target.v) {
    case 0:
        maint_pisces_side.v = 0;
        break;
    case 1:
        maint_pisces_side.v = 1;
        break;
    case 2:
    default:
        maint_pisces_side.v = 0;
        break;
    }
    while ((short)p_pisces_pass_dest[maint_pisces_side.v]->lines_done != p_pisces_pass_dest[maint_pisces_side.v]->length_lines.v)
        ;
    _builtin_set_imask(15);
    p_pisces_mode[0]->run.v = 0;
    p_pisces_mode[1]->run.v = 0;
    p_pisces_ctrl[0]->ctrl.f.sdram_window = 1;
    p_pisces_ctrl[1]->ctrl.f.sdram_window = 1;
    _builtin_set_imask(0);
    if (maint_adjust_error)
        goto L1;
    if (!maint_mode_3_state_3_step_2())
        goto L1;
    if (maint_mode_3_state_3_step_3() == 0)
        goto L1;
    {
        p_pisces_ctrl[0]->ctrl.f.sdram_window = 0;
        p_pisces_ctrl[1]->ctrl.f.sdram_window = 0;
        if (testmode_number.v)
            return;
        maint_screen = MAINT_SCREEN_ADJUSTED;
        panel_set_display_mode(3, PANEL_MODE_MAINT);
        while (!button_state.b.b4 || button_state.b.b7) {
            if (button_state.b.b6) {
                while (button_state.b.b6)
                    ;
                maint_mode_3_set_settings(1);
                maint_screen = MAINT_SCREEN_SELECT;
                maint_state.v = 0;
                return;
            }
        }
        while (button_state.b.b4)
            ;
        maint_state.v++;
        return;
    }
L1:
    p_pisces_ctrl[0]->ctrl.f.sdram_window = 0;
    p_pisces_ctrl[1]->ctrl.f.sdram_window = 0;
    maint_mode_3_set_settings(1);
    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_FAILED;
        panel_set_display_mode(3, PANEL_MODE_MAINT);
        while (!button_state.b.b4 || button_state.b.b6) {
            if (button_state.b.b6) {
                while (button_state.b.b6)
                    ;
                maint_screen = MAINT_SCREEN_SELECT;
                maint_state.v = 0;
                return;
            }
        }
        while (button_state.b.b4)
            ;
    } else {
        switch (maint_adjust_error) {
        case 33:
        case 51:
            err_code_list[0] = 3;
            break;
        case 34:
        case 53:
            err_code_list[0] = 1;
            break;
        case 36:
            err_code_list[0] = 4;
            break;
        case 38:
            err_code_list[0] = 2;
            break;
        case 52:
        case 55:
            err_code_list[0] = 7;
            break;
        }
        return;
    }
    switch (maint_adjust_error) {
    case 33:
    case 51:
        maint_screen = 0xe3;
        break;
    case 34:
    case 53:
        maint_screen = 0xe5;
        break;
    case 38:
    case 52:
        maint_screen = 0xe4;
        break;
    case 36:
    case 55:
        maint_screen = 0xe7;
        break;
    }
    panel_set_display_mode(3, PANEL_MODE_MAINT);
    while (!button_state.b.b6)
        ;
    while (button_state.b.b6)
        ;
    maint_screen = MAINT_SCREEN_SELECT;
    maint_state.v = 0;
    return;
}

void maint_mode_3_state_4(void)
{
    maint_screen = MAINT_SCREEN_CONFIRM;
    panel_set_display_mode(3, PANEL_MODE_MAINT);
    while (1) {
        if (button_state.b.b7 && button_state.b.b4)
            break;
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_mode_3_set_settings(1);
            maint_screen = MAINT_SCREEN_SELECT;
            maint_state.v = 0;
            return;
        }
    }
    while (button_state.b.b4 || button_state.b.b7)
        ;
    maint_mode_3_set_settings(2);
    maint_screen = MAINT_SCREEN_WRITING;
    panel_set_display_mode(3, PANEL_MODE_MAINT);
    if (maint_save_settings(maint_item.v) != 0)
        maint_screen = MAINT_SCREEN_WRITTEN;
    else
        maint_screen = MAINT_SCREEN_FAILED;
    panel_set_display_mode(3, PANEL_MODE_MAINT);
    while (!button_state.b.b6)
        ;
    while (button_state.b.b6)
        ;
    maint_state.v++;
}

void maint_mode_3_state_5(void)
{
    maint_mode_word.v &= 0x7fff;
    maint_screen = MAINT_SCREEN_SELECT;
    if (panel_current.display_main != PANEL_MODE_ERROR) {
        panel_set_display_mode(3, PANEL_MODE_MAINT);
    } else {
        if (panel_current.chars[0] != PANEL_GLYPH_U && panel_current.chars[0] != PANEL_GLYPH_J && panel_current.chars[0] != PANEL_GLYPH_E)
            panel_set_display_mode(3, PANEL_MODE_MAINT);
    }
}

void maint_mode_3_set_settings(int action)
{
    if (action == 0) {
        switch (maint_adjust_target.v) {
        case 0:
            maint_saved_value_0[0] = settings_main_offset_adf_front[0];
            maint_saved_value_1[0] = settings_sub_offset_adf_front[0];
            maint_saved_value_3[0] = settings_white_level_adf_front_chip0[0];
            maint_saved_value_4[0] = settings_white_level_adf_front_chip1[0];
            maint_saved_value_5[0] = settings_white_level_adf_front_chip2[0];
            settings_main_offset_adf_front[0] = 0;
            settings_sub_offset_adf_front[0] = 0xff;
            settings_white_level_adf_front_chip0[0] = 0xff;
            settings_white_level_adf_front_chip1[0] = 0xff;
            settings_white_level_adf_front_chip2[0] = 0xff;
            break;
        case 1:
            maint_saved_value_0[0] = settings_main_offset_adf_back[0];
            maint_saved_value_1[0] = settings_sub_offset_adf_back[0];
            maint_saved_value_3[0] = settings_white_level_adf_back_chip0[0];
            maint_saved_value_4[0] = settings_white_level_adf_back_chip1[0];
            maint_saved_value_5[0] = settings_white_level_adf_back_chip2[0];
            settings_main_offset_adf_back[0] = 0;
            settings_sub_offset_adf_back[0] = 0xff;
            settings_white_level_adf_back_chip0[0] = 0xff;
            settings_white_level_adf_back_chip1[0] = 0xff;
            settings_white_level_adf_back_chip2[0] = 0xff;
            break;
        case 2:
        default:
            maint_saved_value_0[0] = settings[0];
            maint_saved_value_1[0] = settings_sub_offset_flatbed[0];
            maint_saved_value_3[0] = settings_white_level_flatbed_chip0[0];
            maint_saved_value_4[0] = settings_white_level_flatbed_chip1[0];
            maint_saved_value_5[0] = settings_white_level_flatbed_chip2[0];
            settings[0] = 0xff;
            settings_sub_offset_flatbed[0] = 0xff;
            settings_white_level_flatbed_chip0[0] = 0xa0;
            settings_white_level_flatbed_chip1[0] = 0xa0;
            settings_white_level_flatbed_chip2[0] = 0xa0;
            break;
        }
        maint_saved_value_6[0] = settings_byte_0x34[0];
        settings_byte_0x34[0] = 1;
        maint_saved_value_7[0] = settings_test_flags[0];
        settings_test_flags[0] = 8;
    } else {
        settings_test_flags[0] = maint_saved_value_7[0];
        settings_byte_0x34[0] = maint_saved_value_6[0];
        switch (maint_adjust_target.v) {
        case 0:
            settings_white_level_adf_front_chip0[0] = maint_saved_value_3[0];
            settings_white_level_adf_front_chip1[0] = maint_saved_value_4[0];
            settings_white_level_adf_front_chip2[0] = maint_saved_value_5[0];
            break;
        case 1:
            settings_white_level_adf_back_chip0[0] = maint_saved_value_3[0];
            settings_white_level_adf_back_chip1[0] = maint_saved_value_4[0];
            settings_white_level_adf_back_chip2[0] = maint_saved_value_5[0];
            break;
        case 2:
        default:
            settings_white_level_flatbed_chip0[0] = maint_saved_value_3[0];
            settings_white_level_flatbed_chip1[0] = maint_saved_value_4[0];
            settings_white_level_flatbed_chip2[0] = maint_saved_value_5[0];
            break;
        }
        if (action == 1) {
            switch (maint_adjust_target.v) {
            case 0:
                settings_main_offset_adf_front[0] = maint_saved_value_0[0];
                settings_sub_offset_adf_front[0] = maint_saved_value_1[0];
                break;
            case 1:
                settings_main_offset_adf_back[0] = maint_saved_value_0[0];
                settings_sub_offset_adf_back[0] = maint_saved_value_1[0];
                break;
            case 2:
            default:
                settings[0] = maint_saved_value_0[0];
                settings_sub_offset_flatbed[0] = maint_saved_value_1[0];
                break;
            }
        }
    }
    util_mem_copy_to_odd((void *)0xc40081, settings, 0x200);
}

int maint_mode_3_state_3_step_2(void)
{
    unsigned short j;
    unsigned short i;
    unsigned short sum;
    unsigned short top_skew;
    unsigned short bottom_skew;
    unsigned short unused2;
    unsigned int left_mean;
    unsigned int right_mean;
    int addr;
    unsigned short width_px;
    unsigned short value;
    unsigned short sign;
    unsigned char *buf;
    int edge;
    char *window;
    int base_64k;
    int bottom_missing;
    base_64k = p_pisces_pass_dest[maint_pisces_side.v]->base_64k.v;
    bottom_missing = 0;
    width_px = img_side_params[maint_pisces_side.v].width_px;
    buf = (unsigned char *)edge_left_front;
    for (i = 0; i < 8; i++) {
        maint_top_edges.edges[i] = 0;
        switch (maint_adjust_target.v) {
        case 0:
        case 1:
            if (i < 4)
                addr = i * 8 + 0x101;
            else
                addr = 0x11ff - (i - 4) * 8;
            break;
        case 2:
        default:
            if (i < 4)
                addr = i * 8 + 0x153;
            else
                addr = 0x11b4 - (i - 4) * 8;
            break;
        }
        if (maint_pisces_side.v == 0)
            window = p_pisces_front_sdram.p;
        else
            window = p_pisces_back_sdram.p;
        for (j = 0; j < 0x177; j++) {
            buf[j] = maint_read_sdram_byte(maint_pisces_side.v, addr, window, base_64k);
            addr += width_px;
        }
        edge = maint_find_rising_edge(buf, 0x157);
        if (edge < 0) {
            maint_adjust_error = 33;
            return 0;
        }
        maint_top_edges.edges[i] = edge;
        maint_bottom_edges.edges[i] = 0;
        switch (i) {
        case 0:
        case 1:
        case 2:
        case 3:
            addr = i * 8 + 0x153;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            addr = 0x11b4 - (i - 4) * 8;
            break;
        }
        addr += width_px * 0x1b4a;
        if (maint_pisces_side.v == 0)
            window = p_pisces_front_sdram.p;
        else
            window = p_pisces_back_sdram.p;
        for (j = 0; j < 0x236; j++) {
            buf[j] = maint_read_sdram_byte(maint_pisces_side.v, addr, window, base_64k);
            addr -= width_px;
        }
        edge = maint_find_rising_edge(buf, 0x236);
        if (edge < 0)
            bottom_missing = 1;
        maint_bottom_edges.edges[i] = edge;
    }
    left_mean = maint_top_edges.edges[0] + maint_top_edges.edges[1] + maint_top_edges.edges[2] + maint_top_edges.edges[3];
    right_mean = maint_top_edges.edges[4] + maint_top_edges.edges[5] + maint_top_edges.edges[6] + maint_top_edges.edges[7];
    left_mean >>= 2;
    right_mean >>= 2;
    if (left_mean < right_mean)
        top_skew = right_mean - left_mean;
    else
        top_skew = left_mean - right_mean;
    if (bottom_missing) {
        bottom_skew = 0;
    } else {
        left_mean = maint_bottom_edges.edges[0] + maint_bottom_edges.edges[1] + maint_bottom_edges.edges[2] + maint_bottom_edges.edges[3];
        right_mean = maint_bottom_edges.edges[4] + maint_bottom_edges.edges[5] + maint_bottom_edges.edges[6] + maint_bottom_edges.edges[7];
        left_mean >>= 2;
        right_mean >>= 2;
        if (left_mean < right_mean)
            bottom_skew = right_mean - left_mean;
        else
            bottom_skew = left_mean - right_mean;
    }
    if (top_skew > 43 || bottom_skew > 43) {
        maint_adjust_error = 52;
        return 0;
    }
    sum = maint_top_edges.edges[0] + maint_top_edges.edges[1] + maint_top_edges.edges[2] + maint_top_edges.edges[3] + maint_top_edges.edges[4] + maint_top_edges.edges[5] + maint_top_edges.edges[6] + maint_top_edges.edges[7];
    sum >>= 3;
    if (sum < 101) {
        value = 101 - sum;
        sign = 0x80;
    } else {
        value = sum + 0xff9b;
        sign = 0;
    }
    if (value > 84) {
        maint_adjust_error = 51;
        return 0;
    }
    value = value * 6;
    value >>= 2;
    value |= sign;
    switch (maint_adjust_target.v) {
    case 0:
        settings_sub_offset_adf_front[0] = value;
        break;
    case 1:
        settings_sub_offset_adf_back[0] = value;
        break;
    case 2:
    default:
        settings_sub_offset_flatbed[0] = value;
        break;
    }
    return 1;
}

int maint_mode_3_state_3_step_3(void)
{
    unsigned short j;
    unsigned short i;
    int addr;
    unsigned int top_mean;
    unsigned int bottom_mean;
    unsigned short near_edge;
    unsigned short far_skew;
    unsigned short width_px;
    unsigned short unused3;
    unsigned short value;
    unsigned short sign;
    unsigned char *buf;
    int edge;
    unsigned short size;
    unsigned short unused4;
    char *window;
    int base_64k;
    base_64k = p_pisces_pass_dest[maint_pisces_side.v]->base_64k.v;
    width_px = img_side_params[maint_pisces_side.v].width_px;
    buf = (unsigned char *)edge_left_front;
    for (i = 0; i < 8; i++) {
        maint_left_edges.edges[i] = 0;
        switch (i) {
        case 0:
        case 1:
        case 2:
        case 3:
            addr = (i * 8 + 0x157) * width_px;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            addr = (0x1884 - (i - 4) * 8) * width_px;
            break;
        }
        switch (maint_adjust_target.v) {
        case 0:
        case 1:
            size = 0x100;
            break;
        case 2:
        default:
            size = 0x104;
            break;
        }
        if (maint_pisces_side.v == 0)
            window = p_pisces_front_sdram.p;
        else
            window = p_pisces_back_sdram.p;
        for (j = 0; j < size + 32; j++) {
            buf[j] = maint_read_sdram_byte(maint_pisces_side.v, addr, window, base_64k);
            addr++;
        }
        edge = maint_find_rising_edge(buf, size);
        if (edge < 0) {
            maint_adjust_error = 34;
            return 0;
        }
        maint_left_edges.edges[i] = edge;
        maint_left_edges_far.edges[i] = 0;
        switch (i) {
        case 0:
        case 1:
        case 2:
        case 3:
            addr = (i * 8 + 0x157) * width_px;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            addr = (0x1884 - (i - 4) * 8) * width_px;
            break;
        }
        addr += 0x1300;
        switch (maint_adjust_target.v) {
        case 0:
        case 1:
            size = 0x100;
            break;
        case 2:
        default:
            size = 500;
            break;
        }
        if (maint_pisces_side.v == 0)
            window = p_pisces_front_sdram.p;
        else
            window = p_pisces_back_sdram.p;
        for (j = 0; j < size + 32; j++) {
            buf[j] = maint_read_sdram_byte(maint_pisces_side.v, addr, window, base_64k);
            addr++;
        }
        edge = maint_find_rising_edge(buf, size);
        if (edge < 0) {
            maint_adjust_error = 36;
            return 0;
        }
        maint_left_edges_far.edges[i] = edge;
    }
    top_mean = maint_left_edges.edges[0] + maint_left_edges.edges[1] + maint_left_edges.edges[2] + maint_left_edges.edges[3];
    bottom_mean = maint_left_edges.edges[4] + maint_left_edges.edges[5] + maint_left_edges.edges[6] + maint_left_edges.edges[7];
    top_mean >>= 2;
    bottom_mean >>= 2;
    if (top_mean < bottom_mean)
        near_edge = bottom_mean - top_mean;
    else
        near_edge = top_mean - bottom_mean;
    top_mean = maint_left_edges_far.edges[0] + maint_left_edges_far.edges[1] + maint_left_edges_far.edges[2] + maint_left_edges_far.edges[3];
    bottom_mean = maint_left_edges_far.edges[4] + maint_left_edges_far.edges[5] + maint_left_edges_far.edges[6] + maint_left_edges_far.edges[7];
    top_mean >>= 2;
    bottom_mean >>= 2;
    if (top_mean < bottom_mean)
        far_skew = bottom_mean - top_mean;
    else
        far_skew = top_mean - bottom_mean;
    if (near_edge > 64 || far_skew > 64) {
        maint_adjust_error = 55;
        return 0;
    }
    near_edge = maint_left_edges.edges[0] + maint_left_edges.edges[1] + maint_left_edges.edges[2] + maint_left_edges.edges[3] + maint_left_edges.edges[4] + maint_left_edges.edges[5] + maint_left_edges.edges[6] + maint_left_edges.edges[7];
    near_edge >>= 3;
    switch (maint_adjust_target.v) {
    case 0:
    case 1:
        if (near_edge < 105) {
            value = 105 - near_edge;
            sign = 0x80;
        } else {
            value = near_edge + 0xff97;
            sign = 0;
        }
        break;
    case 2:
    default:
        if (near_edge < 97) {
            value = 97 - near_edge;
            sign = 0x80;
        } else {
            value = near_edge + 0xff9f;
            sign = 0;
        }
        break;
    }
    if (value > 84) {
        maint_adjust_error = 53;
        return 0;
    }
    value = value * 6;
    value >>= 2;
    value |= sign;
    switch (maint_adjust_target.v) {
    case 0:
        settings_main_offset_adf_front[0] = value;
        break;
    case 1:
        settings_main_offset_adf_back[0] = value;
        break;
    case 2:
    default:
        settings[0] = value;
        break;
    }
    return 1;
}


void maint_run_mode_4(void)
{
    maint_mode_4_states[maint_state.v]();
}

void maint_mode_4_state_0(void)
{
    short item;

    panel_current.unread_word_52 = 0;
    maint_adjust_error = 0;
    if (testmode_number.v == 0)
        maint_screen = MAINT_SCREEN_ITEM;
    mdc_motor_test_flags.f.maint_started = 1;
    maint_white_level_target.v = 0;
    maint_item.v = 0;
    if (testmode_number.v == 0)
        hw_hostif_flags.f.own_hostif = 0;
    win_params.paper_size.f.custom_size = 1;
    win_params.paper_size.f.auto_length = 0;
    win_params.paper_width_inch1200.v = 0x36d0;
    win_params.paper_length_inch1200.v = 0x26c1;
    win_params.side[0].use_window_area = 1;
    win_params.side[1].use_window_area = 1;
    win_params.side[0].x_offset_inch1200 = 0x2138;
    win_params.side[1].x_offset_inch1200 = 0x2138;
    win_params.side[0].y_offset_inch1200 = 0x762;
    win_params.side[1].y_offset_inch1200 = 0x762;
    win_params.side[0].width_inch1200 = 0x762;
    win_params.side[1].width_inch1200 = 0x762;
    win_params.side[0].length_inch1200 = 0x1360;
    win_params.side[1].length_inch1200 = 0x1360;
    win_params.side[0].composition.f.multi_level = 1;
    win_params.side[0].composition.f.color = 0;
    win_params.side[1].composition.f.multi_level = 1;
    win_params.side[1].composition.f.color = 0;
    win_params.resolution_mode.v &= 0xfff8;
    win_params.x_res_dpi.v = 400;
    win_params.y_res_dpi.v = 400;
    if (testmode_number.v != 0) {
        if (testmode_adjust_mode.v == 1)
            maint_white_level_target.v = 2;
        else if (testmode_adjust_mode.v == 2)
            maint_white_level_target.v = 0;
        else
            maint_white_level_target.v = 1;
        return;
    }
    panel_set_display_mode(4, PANEL_MODE_MAINT);
    while (button_state.b.b7 == 0) {
        if (button_state.b.b4) {
            while (button_state.b.b4)
                ;
            switch (maint_white_level_target.v) {
            case 0:
                maint_white_level_target.v = 1;
                item = 1;
                break;
            case 1:
                maint_white_level_target.v = 2;
                item = 2;
                break;
            case 2:
            default:
                maint_white_level_target.v = 0;
                item = 0;
                break;
            }
            maint_item.v = item;
            panel_set_display_mode(4, PANEL_MODE_MAINT);
        }
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_state.v = 7;
            return;
        }
    }
    while (button_state.b.b7)
        ;
    maint_state.v++;
}

void maint_mode_4_state_1(void)
{
    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_RUNNING;
        panel_set_display_mode(4, PANEL_MODE_MAINT);
    }
    maint_adjust_white_level(0);
    if (maint_white_level_target.v == 0 || maint_white_level_target.v == 1) {
        if (mdc_mailbox.sensor_status.f.no_paper) {
            maint_adjust_white_level(1);
            if (!testmode_number.v) {
                maint_state.v = 5;
                return;
            } else {
                err_code_list[0] = 1;
                return;
            }
        }
        win_params.flags.f.adf = 1;
        if (maint_white_level_target.v == 0)
            win_sides.f.selection = 0;
        else
            win_sides.f.selection = 2;
    } else if (maint_white_level_target.v == 2) {
        win_params.flags.f.adf = 0;
        win_sides.f.selection = 0;
    }
    if (!testmode_number.v)
        maint_state.v++;
}

void maint_mode_4_state_2(void)
{
    struct page_record *scanning_page;

    scanning_page = &page_records[page_scanning.v];
    if (!testmode_number.v) {
        if (panel_current.display_main == PANEL_MODE_ERROR) {
            maint_adjust_white_level(1);
            maint_state.v = 5;
            return;
        }
    }
    util_mem_clear(scanning_page, 0x88);
    if (maint_white_level_target.v != 2)
        scanning_page->flags.f.adf = 1;
    scanning_page->flags.f.scan_requested = 1;
    switch (maint_white_level_target.v) {
    case 1:
        scanning_page->sides[0].flags.f.requested = 1;
        scanning_page->sides[1].flags.v |= 8;
        break;
    case 0:
    case 2:
    default:
        scanning_page->sides[0].flags.f.requested = 1;
        break;
    }
    if (!testmode_number.v) {
        hw_hostif_flags.f.own_hostif = 1;
        ((struct vdcc_gpio_reg_05_bits *)p_vdcc_front_gpio[0])->b7 = 0;
    }
    job_buffer_mode_on = 0;
    if (maint_white_level_target.v == 2)
        mdc_run_lamps(1);
    else
        mdc_run_lamps(6);
    tick_wait_count = 0;
    while (tick_wait_count < 5000)
        ;
    err_last_code.s.v = 0;
    main_run_page_loop();
    if (!testmode_number.v)
        hw_hostif_flags.f.own_hostif = 0;
    job_buffer_mode_on = 0;
    if (err_last_code.s.v != 0)
        maint_adjust_error = 65;
    if (testmode_number.v)
        return;
    maint_state.v++;
}

extern void _builtin_set_imask(int);

void maint_mode_4_state_3(void)
{
    int side;

    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_RUNNING;
        panel_set_display_mode(4, PANEL_MODE_MAINT);
    }
    switch (maint_white_level_target.v) {
    case 0:
        side = 0;
        break;
    case 1:
        side = 1;
        break;
    case 2:
    default:
        side = 0;
        break;
    }
    maint_white_level_side.v = side;
    while ((short)p_pisces_pass_dest[maint_white_level_side.v]->lines_done != p_pisces_pass_dest[maint_white_level_side.v]->length_lines.v)
        ;
    _builtin_set_imask(15);
    p_pisces_mode[side]->run.v = 0;
    p_pisces_ctrl[side]->ctrl.f.sdram_window = 1;
    _builtin_set_imask(0);
    maint_white_level_side.v = side;
    if (maint_adjust_error != 0)
        goto L_b;
    if (maint_mode_4_state_3_step_2() != 0)
        goto L_a;
L_b:
    {
        p_pisces_ctrl[maint_white_level_side.v]->ctrl.f.sdram_window = 0;
        maint_adjust_white_level(1);
        if (testmode_number.v) {
            err_code_list[0] = 1;
            return;
        }
        maint_screen = MAINT_SCREEN_FAILED;
        panel_set_display_mode(4, PANEL_MODE_MAINT);
        while (!button_state.b.b4 || button_state.b.b7) {
            if (button_state.b.b6) {
                while (button_state.b.b6)
                    ;
                maint_screen = MAINT_SCREEN_SELECT;
                maint_state.v = 0;
                return;
            }
        }
        while (button_state.b.b4)
            ;
        switch (maint_adjust_error & 7) {
        case 1: maint_screen = MAINT_SCREEN_ERROR_1; break;
        case 2: maint_screen = 0xe2; break;
        case 3: maint_screen = 0xe3; break;
        case 4: maint_screen = 0xe4; break;
        case 5: maint_screen = 0xe5; break;
        case 6: maint_screen = 0xe6; break;
        case 7: maint_screen = 0xe7; break;
        }
        panel_set_display_mode(4, PANEL_MODE_MAINT);
        while (button_state.b.b6 == 0)
            ;
        while (button_state.b.b6)
            ;
        maint_screen = MAINT_SCREEN_SELECT;
        maint_state.v = 0;
        return;
    }
L_a:
    {
        p_pisces_ctrl[maint_white_level_side.v]->ctrl.f.sdram_window = 0;
        if (testmode_number.v)
            return;
        maint_screen = MAINT_SCREEN_ADJUSTED;
        panel_set_display_mode(4, PANEL_MODE_MAINT);
        while (!button_state.b.b4 || button_state.b.b7) {
            if (button_state.b.b6) {
                while (button_state.b.b6)
                    ;
                maint_adjust_white_level(1);
                maint_screen = MAINT_SCREEN_SELECT;
                maint_state.v = 0;
                return;
            }
        }
        while (button_state.b.b4)
            ;
        maint_state.v++;
    }
}

void maint_mode_4_state_4(void)
{
    maint_screen = MAINT_SCREEN_CONFIRM;
    panel_set_display_mode(4, PANEL_MODE_MAINT);
    while (1) {
        if (button_state.b.b7 && button_state.b.b4)
            break;
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_adjust_white_level(1);
            maint_screen = MAINT_SCREEN_SELECT;
            maint_state.v = 0;
            return;
        }
    }
    while (button_state.b.b4 || button_state.b.b7)
        ;
    maint_adjust_white_level(2);
    maint_screen = MAINT_SCREEN_WRITING;
    panel_set_display_mode(4, PANEL_MODE_MAINT);
    if (maint_save_settings(maint_item.v))
        maint_screen = MAINT_SCREEN_WRITTEN;
    else
        maint_screen = MAINT_SCREEN_FAILED;
    panel_set_display_mode(4, PANEL_MODE_MAINT);
    while (button_state.b.b6 == 0)
        ;
    while (button_state.b.b6)
        ;
    maint_state.v++;
}

void maint_mode_4_state_5(void)
{
    maint_mode_word.v &= 0x7fff;
    maint_screen = MAINT_SCREEN_SELECT;
    if (panel_current.display_main != PANEL_MODE_ERROR)
        panel_set_display_mode(4, PANEL_MODE_MAINT);
    else {
        if (panel_current.chars[0] != PANEL_GLYPH_U && panel_current.chars[0] != PANEL_GLYPH_J && panel_current.chars[0] != PANEL_GLYPH_E)
            panel_set_display_mode(4, PANEL_MODE_MAINT);
    }
}

/* Maintenance white level: saves or restores the white level block. */
void maint_adjust_white_level(int action)
{
    if (action == 0) {
        switch (maint_white_level_target.v) {
        case 0:
            maint_saved_value_0[0] = settings_white_level_adf_front_chip0[0];
            maint_saved_value_1[0] = settings_white_level_adf_front_chip1[0];
            maint_saved_value_2[0] = settings_white_level_adf_front_chip2[0];
            maint_saved_value_3[0] = settings_white_level_high_adf_front_chip0[0];
            maint_saved_value_4[0] = settings_white_level_high_adf_front_chip1[0];
            maint_saved_value_5[0] = settings_white_level_high_adf_front_chip2[0];
            settings_white_level_adf_front_chip0[0] = 44;
            settings_white_level_adf_front_chip1[0] = 44;
            settings_white_level_adf_front_chip2[0] = 44;
            settings_white_level_high_adf_front_chip0[0] = 1;
            settings_white_level_high_adf_front_chip1[0] = 1;
            settings_white_level_high_adf_front_chip2[0] = 1;
            break;
        case 1:
            maint_saved_value_0[0] = settings_white_level_adf_back_chip0[0];
            maint_saved_value_1[0] = settings_white_level_adf_back_chip1[0];
            maint_saved_value_2[0] = settings_white_level_adf_back_chip2[0];
            maint_saved_value_3[0] = settings_white_level_high_adf_back_chip0[0];
            maint_saved_value_4[0] = settings_white_level_high_adf_back_chip1[0];
            maint_saved_value_5[0] = settings_white_level_high_adf_back_chip2[0];
            settings_white_level_adf_back_chip0[0] = 44;
            settings_white_level_adf_back_chip1[0] = 44;
            settings_white_level_adf_back_chip2[0] = 44;
            settings_white_level_high_adf_back_chip0[0] = 1;
            settings_white_level_high_adf_back_chip1[0] = 1;
            settings_white_level_high_adf_back_chip2[0] = 1;
            break;
        case 2:
        default:
            maint_saved_value_0[0] = settings_white_level_flatbed_chip0[0];
            maint_saved_value_1[0] = settings_white_level_flatbed_chip1[0];
            maint_saved_value_2[0] = settings_white_level_flatbed_chip2[0];
            maint_saved_value_3[0] = settings_white_level_high_flatbed_chip0[0];
            maint_saved_value_4[0] = settings_white_level_high_flatbed_chip1[0];
            maint_saved_value_5[0] = settings_white_level_high_flatbed_chip2[0];
            settings_white_level_flatbed_chip0[0] = 192;
            settings_white_level_flatbed_chip1[0] = 192;
            settings_white_level_flatbed_chip2[0] = 192;
            settings_white_level_high_flatbed_chip0[0] = 0;
            settings_white_level_high_flatbed_chip1[0] = 0;
            settings_white_level_high_flatbed_chip2[0] = 0;
            break;
        }
        maint_saved_value_6[0] = settings_test_flags[0];
        settings_test_flags[0] = 8;
    } else {
        settings_test_flags[0] = maint_saved_value_6[0];
        if (action == 1) {
            switch (maint_white_level_target.v) {
            case 0:
                settings_white_level_adf_front_chip0[0] = maint_saved_value_0[0];
                settings_white_level_adf_front_chip1[0] = maint_saved_value_1[0];
                settings_white_level_adf_front_chip2[0] = maint_saved_value_2[0];
                settings_white_level_high_adf_front_chip0[0] = maint_saved_value_3[0];
                settings_white_level_high_adf_front_chip1[0] = maint_saved_value_4[0];
                settings_white_level_high_adf_front_chip2[0] = maint_saved_value_5[0];
                break;
            case 1:
                settings_white_level_adf_back_chip0[0] = maint_saved_value_0[0];
                settings_white_level_adf_back_chip1[0] = maint_saved_value_1[0];
                settings_white_level_adf_back_chip2[0] = maint_saved_value_2[0];
                settings_white_level_high_adf_back_chip0[0] = maint_saved_value_3[0];
                settings_white_level_high_adf_back_chip1[0] = maint_saved_value_4[0];
                settings_white_level_high_adf_back_chip2[0] = maint_saved_value_5[0];
                break;
            case 2:
            default:
                settings_white_level_flatbed_chip0[0] = maint_saved_value_0[0];
                settings_white_level_flatbed_chip1[0] = maint_saved_value_1[0];
                settings_white_level_flatbed_chip2[0] = maint_saved_value_2[0];
                settings_white_level_high_flatbed_chip0[0] = maint_saved_value_3[0];
                settings_white_level_high_flatbed_chip1[0] = maint_saved_value_4[0];
                settings_white_level_high_flatbed_chip2[0] = maint_saved_value_5[0];
                break;
            }
        }
    }
}

int maint_mode_4_state_3_step_2(void)
{
    int u48;
    unsigned int sum;
    unsigned short white_mean;
    unsigned short white_target;
    unsigned short black;
    short u36;
    int u32;
    unsigned int j;
    unsigned int dark_count;
    unsigned char chip;
    unsigned char level;
    short width_px;
    int window;
    int base_64k;
    float expected;
    float scaled;
    float corrected;

    base_64k = p_pisces_pass_dest[maint_white_level_side.v]->base_64k.v;
    width_px = img_side_params[maint_white_level_side.v].width_px;
    switch (maint_white_level_target.v) {
    case 0:
    case 2:
        if (maint_white_level_target.v == 0)
            white_target = 120;
        else
            white_target = 160;
        for (chip = 0; chip < 3; chip++) {
            if (maint_mode_4_state_3_step_2_1(chip, maint_white_level_target.v))
                return 0;
            window = p_pisces_front_sdram.v;
            sum = 0;
            dark_count = 0;
            for (j = 0; j < 0x80000; j++) {
                level = maint_read_sdram_byte(maint_white_level_side.v, j, (char *)window, base_64k);
                if (level < 96)
                    dark_count++;
                sum += level;
            }
            if (dark_count > 0x147b) {
                maint_adjust_error = 65;
                return 0;
            }
            white_mean = (int)(sum / 8) >> 16;
            white_mean += 2;
            switch (chip) {
            case 0:
                black = (unsigned short)(calib_black_pass1_front_chip0.w + calib_black_pass2_front_chip0.w) >> 3;
                break;
            case 1:
                black = (unsigned short)(calib_black_pass1_front_chip1.w + calib_black_pass2_front_chip1.w) >> 3;
                break;
            default:
                black = (unsigned short)(calib_black_pass1_front_chip2.w + calib_black_pass2_front_chip2.w) >> 3;
                break;
            }
            if (maint_white_level_target.v == 0)
                expected = black + ((float)((white_target - black) * 167) + 50) / 100;
            else
                expected = black + ((float)((white_target - black) * 125) + 50) / 100;
            scaled = black + ((expected - black) * white_mean + 128) / 255;
            corrected = black + ((scaled - black) * 255 + 123) / 246;
            maint_white_level_result[chip].v = ((corrected - black) * 256 + (white_target - black) / 2) / (white_target - black);
            if (maint_white_level_result[chip].v < 0x80) {
                maint_white_level_result[chip].v = 0x80 - maint_white_level_result[chip].v;
                *(unsigned short *)&maint_white_level_result[chip] |= 0x8000;
            } else
                maint_white_level_result[chip].v = maint_white_level_result[chip].v - 0x80;
        }
        if (maint_white_level_target.v == 0) {
            for (chip = 0; chip < 3; chip++) {
                if (maint_white_level_result[chip].v < 0xa0 || maint_white_level_result[chip].v > 0x17f || (maint_white_level_result[chip].v & 0x8000)) {
                    maint_adjust_error = 65;
                    return 0;
                }
            }
        } else {
            for (chip = 0; chip < 3; chip++) {
                if (maint_white_level_result[chip].v < 0x48 || maint_white_level_result[chip].v > 0xa8 || (maint_white_level_result[chip].v & 0x8000)) {
                    maint_adjust_error = 65;
                    return 0;
                }
            }
        }
        switch (maint_white_level_target.v) {
        case 0:
            settings_white_level_adf_front_chip0[0] = maint_white_level_result[0].v;
            settings_white_level_adf_front_chip1[0] = maint_white_level_result[1].v;
            settings_white_level_adf_front_chip2[0] = maint_white_level_result[2].v;
            settings_white_level_high_adf_front_chip0[0] = maint_white_level_result[0].v >> 8;
            settings_white_level_high_adf_front_chip1[0] = maint_white_level_result[1].v >> 8;
            settings_white_level_high_adf_front_chip2[0] = maint_white_level_result[2].v >> 8;
            break;
        case 2:
        default:
            settings_white_level_flatbed_chip0[0] = maint_white_level_result[0].v;
            settings_white_level_flatbed_chip1[0] = maint_white_level_result[1].v;
            settings_white_level_flatbed_chip2[0] = maint_white_level_result[2].v;
            settings_white_level_high_flatbed_chip0[0] = maint_white_level_result[0].v >> 8;
            settings_white_level_high_flatbed_chip1[0] = maint_white_level_result[1].v >> 8;
            settings_white_level_high_flatbed_chip2[0] = maint_white_level_result[2].v >> 8;
            break;
        }
        break;
    case 1:
        white_target = 120;
        for (chip = 0; chip < 3; chip++) {
            if (maint_mode_4_state_3_step_2_1(chip, maint_white_level_target.v))
                return 0;
            window = p_pisces_back_sdram.v;
            sum = 0;
            dark_count = 0;
            for (j = 0; j < 0x80000; j++) {
                level = maint_read_sdram_byte(maint_white_level_side.v, j, (char *)window, base_64k);
                if (level < 96)
                    dark_count++;
                sum += level;
            }
            if (dark_count > 0x147b) {
                maint_adjust_error = 65;
                return 0;
            }
            white_mean = (int)(sum / 8) >> 16;
            white_mean += 2;
            switch (chip) {
            case 0:
                black = (unsigned short)(calib_black_pass1_back_chip0.w + calib_black_pass2_back_chip0.w) >> 3;
                break;
            case 1:
                black = (unsigned short)(calib_black_pass1_back_chip1.w + calib_black_pass2_back_chip1.w) >> 3;
                break;
            default:
                black = (unsigned short)(calib_black_pass1_back_chip2.w + calib_black_pass2_back_chip2.w) >> 3;
                break;
            }
            expected = black + ((float)((white_target - black) * 167) + 50) / 100;
            scaled = black + ((expected - black) * white_mean + 128) / 255;
            corrected = black + ((scaled - black) * 255 + 123) / 246;
            maint_white_level_result[chip].v = ((corrected - black) * 256 + (white_target - black) / 2) / (white_target - black);
            if (maint_white_level_result[chip].v < 0x80) {
                maint_white_level_result[chip].v = 0x80 - maint_white_level_result[chip].v;
                *(unsigned short *)&maint_white_level_result[chip] |= 0x8000;
            } else
                maint_white_level_result[chip].v = maint_white_level_result[chip].v - 0x80;
        }
        for (chip = 0; chip < 3; chip++) {
            if (maint_white_level_result[chip].v < 0xa0 || maint_white_level_result[chip].v > 0x17f || (maint_white_level_result[chip].v & 0x8000)) {
                maint_adjust_error = 65;
                return 0;
            }
        }
        settings_white_level_adf_back_chip0[0] = maint_white_level_result[0].v;
        settings_white_level_adf_back_chip1[0] = maint_white_level_result[1].v;
        settings_white_level_adf_back_chip2[0] = maint_white_level_result[2].v;
        settings_white_level_high_adf_back_chip0[0] = maint_white_level_result[0].v >> 8;
        settings_white_level_high_adf_back_chip1[0] = maint_white_level_result[1].v >> 8;
        settings_white_level_high_adf_back_chip2[0] = maint_white_level_result[2].v >> 8;
        break;
    }
    return 1;
}

int maint_save_settings(unsigned char item)
{
    hw_device_errors.f.eeprom = 0;
    panel_current.unread_word_52 = 0;
    iic_eeprom_save_settings();
    if (hw_device_errors.f.eeprom) {
        hw_device_errors.f.eeprom = 0;
        iic_eeprom_save_settings();
        if (hw_device_errors.f.eeprom)
            return 0;
    }
    tick_wait_count = 0;
    while (tick_wait_count < 1000)
        ;
    maint_adjust_error = 0x80;
    return 1;
}

unsigned char maint_mode_4_state_3_step_2_1(unsigned char chip, unsigned char white_level_target)
{
    unsigned short wait_count;
    int side;

    switch (white_level_target) {
    case 0:
        side = 0;
        break;
    case 1:
        side = 1;
        break;
    default:
        side = 0;
        break;
    }
    p_pisces_mode[side]->format.f.f5 = 1;
    switch (chip) {
    case 0:
        p_pisces_gray_row[side]->y_weight = 0x100;
        p_pisces_gray_row[side]->cb_weight.v = 0;
        p_pisces_gray_row[side]->cr_weight.v = 0x167;
        break;
    case 1:
        p_pisces_gray_row[side]->y_weight = 0x100;
        p_pisces_gray_row[side]->cb_weight.v = 600;
        p_pisces_gray_row[side]->cr_weight.v = 0x2b7;
        break;
    case 2:
        p_pisces_gray_row[side]->y_weight = 0x100;
        p_pisces_gray_row[side]->cb_weight.v = 0x1c6;
        p_pisces_gray_row[side]->cr_weight.v = 0;
        break;
    }
    _builtin_set_imask(15);
    p_pisces_mode[side]->run.f.b8 = 0;
    p_pisces_mode[side]->run.f.b4 = 0;
    p_pisces_mode[side]->run.f.b8 = 1;
    p_pisces_mode[side]->run.f.b4 = 1;
    _builtin_set_imask(0);
    wait_count = 0;
    while (!p_pisces_pass_dest[side]->done.enable) {
        rt_delay_180k();
        if (++wait_count > 500)
            return 255;
    }
    return 0;
}

void maint_run_mode_5(void)
{
    maint_mode_5_states[maint_state.v]();
}

void maint_mode_5_state_0(void)
{
    panel_current.unread_word_52 = 0;
    maint_screen = MAINT_SCREEN_ITEM;
    panel_set_display_mode(5, PANEL_MODE_MAINT);
    while (button_state.b.b7 == 0) {
        if (button_state.b.b4) {
            while (button_state.b.b4)
                ;
            maint_item.v++;
            if (maint_item.v > 1)
                maint_item.v = 0;
            panel_set_display_mode(5, PANEL_MODE_MAINT);
        }
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_screen = MAINT_SCREEN_SELECT;
            maint_state.v = 3;
            return;
        }
    }
    maint_state.v++;
}

void maint_mode_5_state_1(void)
{
    unsigned char item;

    item = maint_item.v;
    switch (item) {
    case 0:
        panel_set_display_mode(4, PANEL_MODE_MAINT_INFO);
        break;
    case 1:
        panel_set_display_mode(5, PANEL_MODE_MAINT_INFO);
        break;
    }
    while (1) {
        if (button_state.b.b4)
            break;
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_state.v = 3;
            return;
        }
    }
    while (button_state.b.b4)
        ;
    maint_screen = MAINT_SCREEN_CONFIRM;
    panel_set_display_mode(5, PANEL_MODE_MAINT);
    while (1) {
        if (button_state.b.b7 && button_state.b.b4)
            break;
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_state.v = 3;
            return;
        }
    }
    while (button_state.b.b4 || button_state.b.b7)
        ;
    maint_screen = MAINT_SCREEN_WRITING;
    panel_set_display_mode(5, PANEL_MODE_MAINT);
    if (maint_mode_5_state_1_step_2(item))
        maint_screen = MAINT_SCREEN_WRITTEN;
    else
        maint_screen = MAINT_SCREEN_FAILED;
    panel_set_display_mode(5, PANEL_MODE_MAINT);
    maint_screen = MAINT_SCREEN_SELECT;
    maint_state.v++;
}

void maint_mode_5_state_2(void)
{
    if (!button_state.b.b6)
        return;
    while (button_state.b.b6)
        ;
    maint_state.v++;
}

void maint_mode_5_state_3(void)
{
    maint_mode_word.v &= 0x7fff;
    maint_screen = MAINT_SCREEN_SELECT;
    panel_set_display_mode(5, PANEL_MODE_MAINT);
}

int maint_mode_5_state_1_step_2(unsigned char item)
{
    unsigned char saved_low, saved_high;

    switch (item) {
    case 0:
        saved_low = settings_pick_roller_count.c[0];
        saved_high = settings_pick_roller_count.c[1];
        settings_pick_roller_count.c[0] = 0;
        settings_pick_roller_count.c[1] = 0;
        break;
    case 1:
        saved_low = settings_brake_roller_count.c[0];
        saved_high = settings_brake_roller_count.c[1];
        settings_brake_roller_count.c[0] = 0;
        settings_brake_roller_count.c[1] = 0;
        break;
    }
    hw_device_errors.f.eeprom = 0;
    iic_eeprom_save_settings();
    if (hw_device_errors.f.eeprom) {
        hw_device_errors.f.eeprom = 0;
        iic_eeprom_save_settings();
        if (hw_device_errors.f.eeprom) {
            switch (item) {
            case 0:
                settings_pick_roller_count.c[0] = saved_low;
                settings_pick_roller_count.c[1] = saved_high;
                break;
            case 1:
                settings_brake_roller_count.c[0] = saved_low;
                settings_brake_roller_count.c[1] = saved_high;
                break;
            }
            return 0;
        }
    }
    tick_wait_count = 0;
    while (tick_wait_count < 1000)
        ;
    return 1;
}

void maint_run_mode_6(void)
{
    maint_mode_6_states[maint_state.v]();
}

void maint_mode_6_state_0(void)
{
    maint_screen = MAINT_SCREEN_ITEM;
    panel_set_display_mode(6, PANEL_MODE_MAINT);
    while (!button_state.b.b7) {
        if (button_state.b.b4) {
            while (button_state.b.b4)
                ;
            maint_item.v++;
            if (maint_item.v > 3)
                maint_item.v = 0;
            panel_set_display_mode(6, PANEL_MODE_MAINT);
        }
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_screen = MAINT_SCREEN_SELECT;
            maint_state.v = 3;
            return;
        }
    }
    maint_state.v++;
}

void maint_mode_6_state_1(void)
{
    unsigned char item;

    item = maint_item.v;
    switch (item) {
    case 0:
        panel_set_display_mode(0, PANEL_MODE_MAINT_INFO);
        break;
    case 1:
        panel_set_display_mode(1, PANEL_MODE_MAINT_INFO);
        break;
    case 2:
        panel_set_display_mode(2, PANEL_MODE_MAINT_INFO);
        break;
    case 3:
        panel_set_display_mode(3, PANEL_MODE_MAINT_INFO);
        break;
    }
    maint_screen = MAINT_SCREEN_SELECT;
    maint_state.v++;
}

void maint_mode_6_state_2(void)
{
    if (!button_state.b.b6)
        return;
    while (button_state.b.b6)
        ;
    maint_state.v++;
}

void maint_mode_6_state_3(void)
{
    maint_mode_word.v &= 0x7fff;
    maint_screen = MAINT_SCREEN_SELECT;
    panel_set_display_mode(6, PANEL_MODE_MAINT);
}

void maint_run_mode_7(void)
{
    maint_mode_7_states[maint_state.v]();
}

void maint_mode_7_state_0(void)
{
    int dummy;

    panel_current.unread_word_52 = 0;
    maint_screen = MAINT_SCREEN_CONFIRM;
    panel_set_display_mode(7, PANEL_MODE_MAINT);
    while (1) {
        if (button_state.b.b7 && button_state.b.b4)
            break;
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_state.v = 3;
            return;
        }
    }
    while (button_state.b.b7 || button_state.b.b4)
        ;
    maint_state.v++;
}

void maint_mode_7_state_1(void)
{
    maint_screen = MAINT_SCREEN_WRITING;
    panel_set_display_mode(7, PANEL_MODE_MAINT);
    if (flash_restore_settings_backup())
        maint_screen = MAINT_SCREEN_WRITTEN;
    else
        maint_screen = MAINT_SCREEN_FAILED;
    panel_set_display_mode(7, PANEL_MODE_MAINT);
    maint_state.v++;
}

void maint_mode_7_state_2(void)
{
    if (!button_state.b.b6)
        return;
    while (button_state.b.b6)
        ;
    maint_state.v++;
}

void maint_mode_7_state_3(void)
{
    maint_mode_word.v &= 0x7fff;
    maint_screen = MAINT_SCREEN_SELECT;
    panel_set_display_mode(7, PANEL_MODE_MAINT);
}

void maint_run_mode_8(void)
{
    maint_mode_8_states[maint_state.v]();
}

void maint_mode_8_state_0(void)
{
    int dummy;

    panel_current.unread_word_52 = 0;
    maint_screen = MAINT_SCREEN_ITEM;
    maint_item.v = 5;
    panel_set_display_mode(2, PANEL_MODE_MAINT);
    while (button_state.b.b7)
        ;
    maint_state.v++;
}

void maint_mode_8_state_1(void)
{
    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_RUNNING;
        panel_set_display_mode(2, PANEL_MODE_MAINT);
    }
    maint_saved_value_0[0] = settings_ultrasonic_threshold[0];
    win_params.flags.f.adf = 1;
    afe_select_source();
    mdc_run_command(MDC_CMD_ADJUST_ULTRASONIC);
    if (mdc_mailbox.cmd_result) {
        err_check_pending();
        if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_FAILED;
        panel_set_display_mode(2, PANEL_MODE_MAINT);
        while (!button_state.b.b4 && !button_state.b.b6) {
            if (button_state.b.b6) {
                while (button_state.b.b6)
                    ;
                maint_screen = MAINT_SCREEN_SELECT;
                maint_state.v = 3;
                return;
            }
        }
        while (button_state.b.b4)
            ;
        maint_screen = MAINT_SCREEN_ERROR_1;
        panel_set_display_mode(2, PANEL_MODE_MAINT);
        while (!button_state.b.b6)
            ;
        while (button_state.b.b6)
            ;
        maint_state.v = 3;
        return;
        } else {
            _builtin_set_imask(15);
            err_code_list[0] = 1;
            _builtin_set_imask(0);
            return;
        }
    }
    err_ultrasonic_error = 0;
    if (!testmode_number.v) {
        maint_screen = MAINT_SCREEN_ADJUSTED;
        panel_set_display_mode(2, PANEL_MODE_MAINT);
        while (!button_state.b.b4 || button_state.b.b7) {
            if (button_state.b.b6) {
                while (button_state.b.b6)
                    ;
                maint_mode_3_set_settings(1);
                maint_screen = MAINT_SCREEN_SELECT;
                maint_state.v = 3;
                return;
            }
        }
        while (button_state.b.b4)
            ;
        maint_state.v++;
    }
}

void maint_mode_8_state_2(void)
{
    maint_screen = MAINT_SCREEN_CONFIRM;
    panel_set_display_mode(2, PANEL_MODE_MAINT);
    while (1) {
        if (button_state.b.b7 && button_state.b.b4)
            break;
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            mdc_settings_8f[0] = maint_saved_value_0[0];
            maint_screen = MAINT_SCREEN_SELECT;
            maint_state.v = 3;
            return;
        }
    }
    while (button_state.b.b4 || button_state.b.b7)
        ;
    settings_ultrasonic_threshold[0] = mdc_settings_8f[0];
    maint_screen = MAINT_SCREEN_WRITING;
    panel_set_display_mode(2, PANEL_MODE_MAINT);
    if (maint_save_settings(maint_item.v))
        maint_screen = MAINT_SCREEN_WRITTEN;
    else
        maint_screen = MAINT_SCREEN_FAILED;
    panel_set_display_mode(2, PANEL_MODE_MAINT);
    while (!button_state.b.b6)
        ;
    while (button_state.b.b6)
        ;
    maint_state.v++;
}

extern void _builtin_set_imask(int);

void maint_mode_8_state_3(void)
{
    maint_mode_word.v &= 0x7fff;
    maint_screen = MAINT_SCREEN_SELECT;
    panel_set_display_mode(8, PANEL_MODE_MAINT);
}

void maint_run_mode_10(void)
{
    maint_mode_10_states[maint_state.v]();
}

void maint_mode_10_state_0(void)
{
    unsigned char item;

    item = settings_emulation[0] & 0x1c;
    switch (item) {
    case 24:
    case 28:
        item = 1;
        break;
    case 16:
        item = 2;
        break;
    case 12:
        item = 3;
        break;
    default:
        item = 0;
        break;
    }
    maint_item.v = item;
    maint_screen = MAINT_SCREEN_ITEM;
    panel_set_display_mode(10, PANEL_MODE_MAINT);
    while (!button_state.b.b7) {
        if (button_state.b.b4) {
            while (button_state.b.b4)
                ;
            maint_item.v++;
            if (maint_item.v > 3)
                maint_item.v = 0;
            panel_set_display_mode(10, PANEL_MODE_MAINT);
        }
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_screen = MAINT_SCREEN_SELECT;
            maint_state.v = 3;
            return;
        }
    }
    while (button_state.b.b7)
        ;
    maint_state.v++;
}

void maint_mode_10_state_1(void)
{
    maint_screen = MAINT_SCREEN_ADJUSTED;
    panel_set_display_mode(10, PANEL_MODE_MAINT);
    while (1) {
        if (button_state.b.b4)
            break;
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_state.v = 3;
            return;
        }
    }
    while (button_state.b.b4)
        ;
    maint_screen = MAINT_SCREEN_CONFIRM;
    panel_set_display_mode(10, PANEL_MODE_MAINT);
    while (1) {
        if (button_state.b.b7 && button_state.b.b4)
            break;
        if (button_state.b.b6) {
            while (button_state.b.b6)
                ;
            maint_screen = MAINT_SCREEN_SELECT;
            maint_state.v = 3;
            return;
        }
    }
    while (button_state.b.b4 || button_state.b.b7)
        ;
    maint_screen = MAINT_SCREEN_WRITING;
    panel_set_display_mode(10, PANEL_MODE_MAINT);
    if (maint_mode_10_state_1_step_2(maint_item.v))
        maint_screen = MAINT_SCREEN_WRITTEN;
    else
        maint_screen = MAINT_SCREEN_FAILED;
    panel_set_display_mode(10, PANEL_MODE_MAINT);
    maint_state.v++;
}

void maint_mode_10_state_2(void)
{
    if (!button_state.b.b6)
        return;
    while (button_state.b.b6)
        ;
    maint_state.v++;
}

void maint_mode_10_state_3(void)
{
    maint_mode_word.v &= 0x7fff;
    maint_screen = MAINT_SCREEN_SELECT;
    panel_set_display_mode(10, PANEL_MODE_MAINT);
}

int maint_mode_10_state_1_step_2(unsigned char item)
{
    char emulation;
    char saved_emulation;

    saved_emulation = settings_emulation[0];
    emulation = settings_emulation[0] & 0xe3;
    switch (item) {
    case 1:
        emulation = 24;
        break;
    case 2:
        emulation = 16;
        break;
    case 3:
        emulation = 12;
        break;
    default:
        emulation = 0;
        break;
    }
    settings_emulation[0] = emulation;
    hw_device_errors.f.eeprom = 0;
    panel_current.unread_word_52 = 0;
    iic_eeprom_save_settings();
    if (hw_device_errors.f.eeprom) {
        hw_device_errors.f.eeprom = 0;
        iic_eeprom_save_settings();
        if (hw_device_errors.f.eeprom) {
            settings_emulation[0] = saved_emulation;
            return 0;
        }
    }
    tick_wait_count = 0;
    while (tick_wait_count < 1000)
        ;
    return 1;
}

void maint_run_mode_11(void)
{
}

void maint_unused_01(void)
{
}

void maint_unused_02(void)
{
}

void maint_unused_03(void)
{
}

void maint_unused_04(void)
{
}

void maint_unused_05(void)
{
}

void maint_unused_06(void)
{
}

void maint_unused_07(void)
{
}

void maint_unused_08(void)
{
}

void maint_unused_09(void)
{
}

void maint_unused_10(void)
{
}

void maint_unused_11(void)
{
}

void maint_unused_12(void)
{
}

void maint_unused_13(void)
{
}

void maint_unused_14(void)
{
}

void maint_unused_15(void)
{
}

void maint_unused_16(void)
{
}

void maint_unused_17(void)
{
}

void maint_unused_18(void)
{
}

void maint_unused_19(void)
{
}
