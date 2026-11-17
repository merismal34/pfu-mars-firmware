#include "fw.h"
#pragma section H

extern int _builtin_get_imask(void);
extern void _builtin_set_imask(int);
/* Constants (section C). */
const unsigned char mdc_gap_counts_one_side[8] = {0x68, 0x68, 0x4d, 0x3c, 0x16, 0x16, 0x1d, 0x1d};
const unsigned char mdc_gap_counts_both_sides[24] = {
    0x68, 0x68, 0x3c, 0x3c, 0x16, 0x16, 0x1d, 0x1d, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
    0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15
};
const unsigned char mdc_gap_lines_next_one_side[104] = {
    0x3a, 7, 8, 2, 2, 5, 2, 5, 3, 3, 5, 2, 2, 2, 2, 2,
    2, 3, 2, 2, 3, 2, 2, 2, 3, 2, 1, 2, 2, 2, 2, 2,
    1, 3, 2, 1, 2, 2, 1, 2, 1, 1, 2, 2, 2, 2, 1, 2,
    1, 2, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1,
    1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1,
    2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 1, 1, 1, 1
};
const unsigned char mdc_gap_lines_next_both_sides[104] = {
    0x3a, 7, 8, 2, 2, 5, 2, 5, 3, 3, 5, 2, 2, 2, 2, 2,
    2, 3, 2, 2, 3, 2, 2, 2, 3, 2, 1, 2, 2, 2, 2, 2,
    1, 3, 2, 1, 2, 2, 1, 2, 1, 1, 2, 2, 2, 2, 1, 2,
    1, 2, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 1,
    1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1,
    2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 1, 1, 1, 1
};
const unsigned char mdc_gap_lines_first_one_side[32] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    1, 2, 3, 3, 4, 4, 4, 5, 1, 1, 1, 1, 1, 1, 1, 1
};
const unsigned char mdc_gap_lines_first_both_sides[32] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    1, 2, 3, 3, 4, 4, 4, 5, 1, 1, 1, 1, 1, 1, 1, 1
};
const unsigned char mdc_gap_lines_next_long_page[90] = {
    1, 0x4b, 0x0b, 6, 3, 4, 5, 3, 2, 3, 4, 3, 3, 2, 2, 2,
    2, 2, 3, 3, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1,
    2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
const unsigned char mdc_gap_lines_first_long_page[32] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
    2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 0x46, 0x46, 0x46, 0x46
};
const unsigned char mdc_gap_lines_unused_1[24] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};
const unsigned char mdc_gap_lines_unused_2[24] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};
const unsigned char mdc_gap_lines_unused_3[24] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};
const unsigned char mdc_gap_lines_unused_4[24] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};
const unsigned char mdc_unused_word_table[18] = {
    0x00, 0x00, 0x04, 0xe2, 0x09, 0xc4, 0x13, 0x88, 0x1d, 0x4c, 0x7e, 0xf4, 0x27, 0x10, 0x30, 0xd4,
    0x3a, 0x98
};
/* Initialised data (section D). */
unsigned char * mdc_gap_tables_one_side[8] = {
    mdc_gap_lines_next_one_side, mdc_gap_lines_next_one_side, mdc_gap_lines_next_long_page, mdc_gap_lines_next_long_page,
    mdc_gap_lines_first_one_side, mdc_gap_lines_first_one_side, mdc_gap_lines_first_long_page, mdc_gap_lines_first_long_page
};
unsigned char * mdc_gap_tables_both_sides[24] = {
    mdc_gap_lines_next_both_sides, mdc_gap_lines_next_both_sides, mdc_gap_lines_next_long_page, mdc_gap_lines_next_long_page,
    mdc_gap_lines_first_both_sides, mdc_gap_lines_first_both_sides, mdc_gap_lines_first_long_page, mdc_gap_lines_first_long_page,
    mdc_gap_lines_unused_1, mdc_gap_lines_unused_1, mdc_gap_lines_unused_1, mdc_gap_lines_unused_1,
    mdc_gap_lines_unused_3, mdc_gap_lines_unused_3, mdc_gap_lines_unused_3, mdc_gap_lines_unused_3,
    mdc_gap_lines_unused_2, mdc_gap_lines_unused_2, mdc_gap_lines_unused_2, mdc_gap_lines_unused_2,
    mdc_gap_lines_unused_4, mdc_gap_lines_unused_4, mdc_gap_lines_unused_4, mdc_gap_lines_unused_4
};

int mdc_run_end_heater_warmup(char unit_bits)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    mdc_end_heater_warmup_bits.v = unit_bits;
    _builtin_set_imask(saved_imask);
    if (*(char *)&power_state & 32) {
        return 1;
    } else {
    }
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    util_mem_clear(mdc_queued_params, 16);
    if (((unsigned char *)&mdc_end_heater_warmup_bits.v)[0] & 2) {
        mdc_queued_params[0] = 0x80;
    }
    if (((unsigned char *)&mdc_end_heater_warmup_bits.v)[0] & 4) {
        mdc_queued_param_byte_2[0] = 64;
    }
    if (((unsigned char *)&mdc_end_heater_warmup_bits.v)[0] & 1) {
        mdc_queued_param_byte_4[0] = 32;
    }
    if (((unsigned char *)&mdc_end_heater_warmup_bits.v)[0] & 16) {
        mdc_queued_param_byte_1[0] = 16;
    }
    mdc_queued_cmd = MDC_CMD_END_HEATER_WARMUP;
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (1) {
        _builtin_set_imask(saved_imask);
        if (!mdc_busy.f.queued) {
            break;
        }
    }
    if (((unsigned char *)&mdc_mailbox)[0x493] == 0x84) {
        return 0;
    }
    return 1;
}

void mdc_run_motor_test(char motor, char speed_class)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    if (panel_current.roller_turn_request == 0 && err_check_pending() == 0) {
        mdc_motor_test_state.w = 0;
        return;
    }
    _builtin_set_imask(15);
    mdc_mailbox.jam_limit_high = 0xff;
    mdc_mailbox.jam_limit_low = 0xff;
    util_mem_clear(mdc_queued_params, 16);
    mdc_queued_params[0] = motor;
    mdc_queued_param_byte_2[0] = speed_class;
    mdc_queued_cmd = MDC_CMD_MOTOR_TEST;
    mdc_busy.f.queued = 1;
    tick_wait_count = 0;
    _builtin_set_imask(saved_imask);
    while (mdc_mailbox.cmd_state != MDC_CMD_STATE_RUNNING) {
        if (tick_wait_count >= 2500) {
            break;
        }
    }
    _builtin_set_imask(15);
    mdc_motor_test_state.w = 0x8000;
    _builtin_set_imask(saved_imask);
    return;
}

void mdc_end_motor_test(char motor, char speed_class)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    mdc_mailbox.jam_limit_high = 0;
    mdc_mailbox.jam_limit_low = 0;
    mdc_cmd_params[0] = motor;
    mdc_cmd_params[2] = speed_class;
    mdc_cmd_posted.f.end_requested = 1;
    mdc_mailbox.cmd_end = MDC_CMD_END;
    _builtin_set_imask(saved_imask);
    while (mdc_busy.f.queued) {
    }
    _builtin_set_imask(15);
    mdc_motor_test_state.w = 0;
    _builtin_set_imask(saved_imask);
    return;
}

void mdc_run_pick_arm(int move, int unused_flag)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    err_check_pending();
    _builtin_set_imask(15);
    mdc_queued_params[0] = testmode_param_1.v;
    if (move) {
        mdc_queued_cmd = MDC_CMD_PICK_ARM_MOVE;
    } else {
        mdc_queued_cmd = MDC_CMD_PICK_ARM_HOME;
    }
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (mdc_busy.f.queued) {
    }
    return;
}

void mdc_request_capture(int resume)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (resume) {
        _builtin_set_imask(15);
        if (mdc_capture_pause_sent == 0) {
            _builtin_set_imask(saved_imask);
            return;
        }
        _builtin_set_imask(saved_imask);
        mdc_send_request(MDC_REQUEST_CAPTURE_RESUME);
        _builtin_set_imask(15);
        mdc_capture_pause_sent = 0;
        _builtin_set_imask(saved_imask);
    } else {
        mdc_send_request(MDC_REQUEST_CAPTURE_PAUSE);
        _builtin_set_imask(15);
        mdc_capture_pause_sent = 1;
        _builtin_set_imask(saved_imask);
    }
    return;
}

void mdc_request_stop(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (power_state.f.saving) {
        return;
    }
    _builtin_set_imask(15);
    if (!win_params.flags.f.adf) {
        if (mdc_motor_test_flags.f.flatbed_stop_requested) {
            _builtin_set_imask(saved_imask);
            return;
        }
        mdc_motor_test_flags.f.flatbed_stop_requested = 1;
    }
    _builtin_set_imask(saved_imask);
    mdc_send_request(MDC_REQUEST_STOP);
    return;
}

void mdc_request_abort(void)
{
    if (power_state.f.saving) {
        return;
    }
    if (err_display_flags.b.b7) {
        return;
    }
    log_add_record(6, 1, 0, 0);
    mdc_send_request(MDC_REQUEST_ABORT);
}

void mdc_request_heater_off(unsigned char heaters)
{
    if (power_state.f.saving) {
        return;
    }
    if (heaters == 1) {
        mdc_send_request(MDC_REQUEST_HEATER_FRONT_OFF);
    } else if (heaters == 2) {
        mdc_send_request(MDC_REQUEST_HEATER_BACK_OFF);
    } else {
        mdc_send_request(MDC_REQUEST_HEATERS_OFF);
    }
}

void mdc_request_restart(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    err_check_pending();
    mdc_send_request(MDC_REQUEST_RESTART);
    mdc_wait_request_taken();
    return;
}

int mdc_request_log_state(void)
{
    mdc_send_request(MDC_REQUEST_LOG_STATE);
    mdc_wait_request_taken();
    return;
}


void mdc_send_request(unsigned char request)
{
    int saved_imask;
    unsigned char capture_flags;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (request == 69) {
        capture_flags = mdc_mailbox.capture_flags.v;
        if (capture_flags == MDC_CAPTURE_PAUSED) {
            request = 69;
            mdc_capture_resume_repeat = 1;
        }
    }
    if (++mdc_cmd_log_index == 95) {
        mdc_cmd_log_index = 0;
    }
    mdc_cmd_log[mdc_cmd_log_index] = request;
    (mdc_cmd_log + mdc_cmd_log_index)[1] = 0x88;
    mdc_mailbox.request = request;
    _builtin_set_imask(saved_imask);
}

void mdc_wake_board(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (power_state.f.saving) {
        p_board_ctrl->reg_0a.power_enable_8 = 1;
        p_board_ctrl->reg_0a.power_enable_9 = 1;
        rt_delay_60k();
        p_board_ctrl->reg_06.mdc_enable = 1;
        rt_delay_180k();
        _builtin_set_imask(15);
        if (hw_hostif_flags.f.own_hostif) {
            p_board_ctrl->reg_02.hostif_idle = 0;
            p_board_ctrl->reg_02.standby = 0;
            p_board_ctrl->reg_04.hostif_idle_enable = 0;
            if (edge_left_front[0] != 0 || power_image_startup_state[0] != 0) {
                util_mem_copy_to_odd((unsigned char *)&mdc_mailbox.handshake_high, edge_left_front, 0x400);
            }
        }
        _builtin_set_imask(saved_imask);
        hw_port_a->DR.BIT.B9 = 0;
    }
}

void mdc_run_read_overlap_patterns(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    mdc_queued_cmd = MDC_CMD_READ_OVERLAP_PATTERNS;
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (mdc_busy.f.queued) {
    }
    return;
}

void mdc_run_write_overlap_patterns(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    mdc_queued_cmd = MDC_CMD_WRITE_OVERLAP_PATTERNS;
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (mdc_busy.f.queued) {
    }
    return;
}

void mdc_run_dump_log(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    mdc_queued_cmd = MDC_CMD_DUMP_LOG;
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (mdc_busy.f.queued) {
    }
    return;
}

int mdc_wait_request_taken(void)
{
    int u;

    tick_wait_count = 0;
    while (tick_wait_count < 0x1388) {
        if (mdc_mailbox.request == 0) {
            return 1;
        } else {
        }
    }
    return 0;
}

int mdc_wait_pb9_low(void)
{
    short elapsed_ticks;

    tick_wait_count = 0;
    while (tick_wait_count < 0x1388) {
        elapsed_ticks = tick_wait_count;
        if (!hw_port_b->DR.BIT.B9) {
            if (!hw_port_b->DR.BIT.B9) {
                hw_port_a->DR.BIT.B9 = 1;
                return 1;
            }
        }
    }
    return 0;
}

void mdc_set_feed_params(void)
{
    unsigned char feed_options;
    int side;
    unsigned short max_length;
    unsigned short gap_extension;

    feed_options = 0;
    if ((maint_mode_word.v & 0x4000) != 0 || testmode_feed_test != 0 || testmode_number.v == '0') {
        feed_options |= MDC_FEED_ULTRASONIC_CHECK;
        if (testmode_feed_test != 0 || testmode_number.v == '0') {
            feed_options |= MDC_FEED_LENGTH_CHECK;
        }
        if (settings_byte_0x34[0] & 0x40) {
            feed_options = 0;
        }
    } else {
        if (job_double_feed_flags.b.length_check) {
            feed_options |= MDC_FEED_LENGTH_CHECK;
            if (job_double_feed_flags.b.length_tolerance == 0) {
                feed_options |= MDC_FEED_LENGTH_TOLERANCE_10MM;
            } else if (job_double_feed_flags.b.length_tolerance == 1) {
                feed_options |= MDC_FEED_LENGTH_TOLERANCE_15MM;
            } else {
                feed_options |= MDC_FEED_LENGTH_TOLERANCE_20MM;
            }
            if (job_double_feed_flags.b.length_report_only) {
                feed_options |= MDC_FEED_MULTIFEED_REPORT_ONLY;
            }
        }
        if (job_double_feed_flags.b.ultrasonic_check) {
            feed_options |= MDC_FEED_ULTRASONIC_CHECK;
            if (job_double_feed_flags.b.ultrasonic_report_only) {
                feed_options |= MDC_FEED_MULTIFEED_REPORT_ONLY;
            }
        }
    }
    if (job_prepick_on != 0) {
        feed_options |= MDC_FEED_PREPICK;
    }
    if (job_mode_params[0].job_separation[0] & 0x80) {
        feed_options &= (unsigned char)~MDC_FEED_PREPICK;
        feed_options |= MDC_FEED_JOB_SEPARATION;
    }
    if (jpeg_scan_flags.v & JPEG_SCAN_LONG_PAGE) {
        feed_options &= (unsigned char)~MDC_FEED_PREPICK;
    }
    mdc_mailbox.feed_options = feed_options;
    if (win_sides.f.selection != 1) {
        side = 0;
    } else {
        side = 1;
    }
    if (img_side_params[side].y_res_dpi >= 401) {
        mdc_mailbox.speed_class_a = 5;
    } else if (img_side_params[side].y_res_dpi >= 301) {
        mdc_mailbox.speed_class_a = 4;
    } else if (img_side_params[0].y_res_dpi >= 241) {
        mdc_mailbox.speed_class_a = 3;
    } else if (img_side_params[0].y_res_dpi >= 201) {
        mdc_mailbox.speed_class_a = 2;
    } else {
        mdc_mailbox.speed_class_a = 1;
    }
    if (!win_params.flags.f.adf) {
        if (img_side_params[0].y_res_dpi <= 240) {
            mdc_mailbox.speed_class_a = 1;
        }
    }
    if (win_params.flags.f.adf && img_side_params[side].y_res_dpi == 300 && settings_sheet_gap_extension[0] == 0) {
        mdc_mailbox.gap_extension_flags |= 1;
        if (hw_hostif_flags.f.own_hostif == 0) {
            mdc_mailbox.gap_extension = 0x4e;
        } else {
            mdc_mailbox.gap_extension = 0x27;
        }
    } else {
        mdc_mailbox.gap_extension_flags &= 0xfe;
        mdc_mailbox.gap_extension = 0;
    }
    if (settings_sheet_gap_extension[0] == 0 && mdc_adf_deskew_or_overscan_on() != 0 && hw_hostif_flags.f.own_hostif && win_params.flags.f.adf && (settings_overscan_control[0] & 0x80) != 0) {
        mdc_mailbox.gap_extension_flags |= 1;
        gap_extension = img_overscan_margin.v;
        gap_extension = rt_divu32(gap_extension, 6);
        mdc_mailbox.gap_extension = gap_extension + 39;
    }
    /* 31: the fast multifeed blink after a Function press */
    if (panel_multifeed_blink_polls.v == PANEL_MULTIFEED_BLINK_FAST) {
        mdc_mailbox.multifeed_bypass.f |= 0x80;
    } else {
        mdc_mailbox.multifeed_bypass.f &= 0x7f;
    }
    if (win_extra_long_length != 0) {
        max_length = img_side_params[0].scan_length_lines;
        max_length = rt_divu16(max_length * 110, img_side_params[0].y_res_dpi);
        if (max_length != 0) {
            mdc_mailbox.max_sheet_length_high = max_length >> 8;
            mdc_mailbox.max_sheet_length_low = max_length;
        } else {
            mdc_mailbox.max_sheet_length_high = 0x33;
            mdc_mailbox.max_sheet_length_low = 0x90;
        }
    } else {
        mdc_mailbox.max_sheet_length_high = 0;
        mdc_mailbox.max_sheet_length_low = 0;
    }
}

int mdc_is_paper_in_hopper(void)
{
    if (settings_test_flags[0] & 4) {
        return 1;
    }
    if (mdc_mailbox.sensor_status.f.no_paper == 0) {
        return 1;
    }
    return 0;
}

int mdc_is_adf_cover_open(void)
{
    if (settings_test_flags[0] & 4) {
        return 0;
    }
    if (!mdc_mailbox.sensor_status.f.adf_cover_open) {
        return 0;
    }
    return 1;
}


void mdc_check_fan_alarm(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (power_state.f.saving || power_state.f.enter_request) {
        _builtin_set_imask(saved_imask);
        return;
    }
    if (!hw_port_e->DRL.BIT.B1 || settings_byte_0x34[0] & 0x80) {
        mdc_fan_alarm.b6 = 0;
        _builtin_set_imask(saved_imask);
        return;
    }
    _builtin_set_imask(saved_imask);
    if (!mdc_fan_alarm.b7 || mdc_fan_alarm.b6) {
        return;
    }
    mdc_fan_alarm.b6 = 1;
    mdc_fan_alarm.b7 = 0;
    power_state.f.enter_request = 1;
    iic_panel_set_port0(PANEL_LED_POWER | PANEL_LED_CHECK);
    err_raise(ERR_FAN);
    return;
}

/* Picks the gap schedules for the page start and a capture restart by the sides scanned and the long page at 300 dpi. */
void mdc_select_scan_tables(void)
{
    int next_index;
    int first_index;
    unsigned char first_count;
    unsigned char next_count;

    line_gap_steps_left[0] = line_gap_steps_left[1] = 0;
    line_gap_count_next = line_gap_count_first = 0;
    line_gap_lines_left[0] = line_gap_lines_left[1] = 1;
    line_gap_lines_next = line_gap_lines_first = 1;
    if (job_long_page_flags & 0x80 && img_side_params[0].y_res_dpi == 300) {
        next_index = 2;
    } else {
        next_index = 0;
    }
    first_index = next_index + 4;
    if (win_sides.f.selection == 2) {
        next_count = mdc_gap_counts_both_sides[next_index];
        first_count = mdc_gap_counts_both_sides[first_index];
    } else {
        next_count = mdc_gap_counts_one_side[next_index];
        first_count = mdc_gap_counts_one_side[first_index];
    }
    if (first_count) {
        line_gap_count_first = first_count;
        if (win_sides.f.selection == 2) {
            line_gap_table_first.p = mdc_gap_tables_both_sides[first_index];
        } else {
            line_gap_table_first.p = mdc_gap_tables_one_side[first_index];
        }
        line_gap_lines_first = *line_gap_table_first.p++;
    }
    if (next_count) {
        line_gap_count_next = next_count;
        if (win_sides.f.selection == 2) {
            line_gap_table_next.p = mdc_gap_tables_both_sides[next_index];
        } else {
            line_gap_table_next.p = mdc_gap_tables_one_side[next_index];
        }
        line_gap_lines_next = *line_gap_table_next.p++;
    }
}

int mdc_adf_deskew_or_overscan_on(void)
{
    if (!job_auto_size_flags.b6 && !job_auto_size_flags.b7) {
        return 0;
    }
    if (win_params.flags.f.adf && *(long *)&job_black_backing) {
        return 1;
    }
    if (win_params.flags.f.adf && *(long *)&job_keep_backing) {
        return 1;
    }
    return 0;
}

int mdc_unused_1(void)
{
    return 0;
}

void mdc_unused_2(void) {}

void mdc_unused_3(void) {}
