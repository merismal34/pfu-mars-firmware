#include "fw.h"
#pragma section H

extern int _builtin_get_imask(void);
extern void _builtin_set_imask(int);

void mdc_run_eject(void)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    if (err_check_pending() == 0) {
        return;
    }
    _builtin_set_imask(15);
    mdc_busy.f.eject = 1;
    _builtin_set_imask(saved_imask);
    while (mdc_busy.f.eject) {
    }
    return;
}

void mdc_set_scan_params(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (win_params.flags.f.adf && (jpeg_scan_flags.v & JPEG_SCAN_LONG_PAGE))
        mdc_select_scan_tables();
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    if (win_params.flags.f.adf && !(settings_test_flags[0] & 4))
        err_feed_params_pending = 1;
    err_check_pending();
    _builtin_set_imask(15);
    mdc_set_feed_params();
    if (win_params.flags.f.adf == 0) {
        mdc_mailbox.source = MDC_SOURCE_FLATBED;
        mdc_motor_test_flags.f.flatbed_stop_requested = 0;
    } else {
        mdc_mailbox.source = MDC_SOURCE_ADF;
    }
    mdc_mailbox.lead_front_low = (unsigned char)line_page_timing[0].lead_lines;
    mdc_mailbox.lead_front_high = line_page_timing[0].lead_lines >> 8;
    if (win_sides.f.selection == 0) {
        mdc_mailbox.lead_back_low = (unsigned char)line_page_timing[0].lead_lines;
        mdc_mailbox.lead_back_high = line_page_timing[0].lead_lines >> 8;
    } else {
        mdc_mailbox.lead_back_low = (unsigned char)line_page_timing[1].lead_lines;
        mdc_mailbox.lead_back_high = line_page_timing[1].lead_lines >> 8;
    }
    if (win_params.flags.f.adf == 0) {
        mdc_mailbox.lead_flatbed_low = (unsigned char)line_page_timing[0].lead_lines;
        mdc_mailbox.lead_flatbed_high = line_page_timing[0].lead_lines >> 8;
    }
    mdc_mailbox.end_lead_front_low.v = 0xbd;
    mdc_mailbox.end_lead_front_high.v = 0;
    if (mdc_adf_deskew_or_overscan_on() && hw_hostif_flags.f.own_hostif && win_params.flags.f.adf && (settings_overscan_control[0] & 0x80)) {
        mdc_mailbox.end_lead_back_low = 0xbd;
        mdc_mailbox.end_lead_back_high = 0;
    } else {
        mdc_mailbox.end_lead_back_low = 0;
        mdc_mailbox.end_lead_back_high = 0;
    }
    if (img_side_params[0].y_res_dpi >= 0x191) {
        mdc_mailbox.speed_class_b = 5;
    } else if (img_side_params[0].y_res_dpi >= 0x12d) {
        mdc_mailbox.speed_class_b = 4;
    } else if (img_side_params[0].y_res_dpi >= 0xf1) {
        mdc_mailbox.speed_class_b = 3;
    } else if (img_side_params[0].y_res_dpi >= 0xc9) {
        mdc_mailbox.speed_class_b = 2;
    } else {
        mdc_mailbox.speed_class_b = 1;
    }
    mdc_mailbox.feed_backing_request = 0;
    if (win_params.flags.f.adf == 0) {
        if (img_side_params[0].y_res_dpi <= 0xf0)
            mdc_mailbox.speed_class_b = 1;
        if (maint_mode_word.v & 0x5000) {
            if (img_side_params[0].y_res_dpi >= 0x191 && (win_params.resolution_mode.v & 7) == 5)
                mdc_mailbox.speed_class_b = 6;
        }
    }
    mdc_busy.f.scan_move = 1;
    _builtin_set_imask(saved_imask);
    return;
}

/* Queues MDC command cmd (mdc_queued_cmd, mdc_busy.f.queued) and waits until mdc_poll
   has sent it and the result is in mdc_cmd_posted (member c.last_result). */
void mdc_run_command(unsigned char cmd)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    if (cmd == 102) {
        mdc_mailbox.source = MDC_SOURCE_FLATBED;
        mdc_mailbox.speed_class_b = 5;
    }
    _builtin_set_imask(15);
    mdc_queued_cmd = cmd;
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (mdc_busy.f.queued) {
    }
    return;
}

void mdc_run_power_save(int on)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (on) {
        while (mdc_busy.w) {
            _builtin_set_imask(saved_imask);
        }
        hw_port_a->DR.BIT.B9 = 1;
        _builtin_set_imask(15);
        mdc_busy.f.power_save = 1;
        _builtin_set_imask(saved_imask);
        while (1) {
            _builtin_set_imask(saved_imask);
            if (mdc_cmd_posted.f.posted)
                break;
        }
        while (1) {
            _builtin_set_imask(saved_imask);
            if (mdc_mailbox.cmd_state == MDC_CMD_STATE_ENDED) {
                _builtin_set_imask(15);
                mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
                mdc_cmd_posted.f.posted = 0;
                mdc_busy.f.power_save = 0;
                mdc_queued_cmd = 0;
                _builtin_set_imask(saved_imask);
                return;
            }
        }
    } else {
        hw_port_a->DR.BIT.B9 = 0;
        return;
    }
}

int mdc_run_power_save_timeout(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    hw_port_a->DR.BIT.B9 = 1;
    _builtin_set_imask(15);
    mdc_busy.f.power_save = 1;
    tick_capture_wait_ticks.v = 0;
    _builtin_set_imask(saved_imask);
    while (1) {
        _builtin_set_imask(saved_imask);
        if (mdc_cmd_posted.f.posted)
            break;
        if (tick_capture_wait_ticks.v > 0x2710) {
            _builtin_set_imask(15);
            mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
            mdc_cmd_posted.f.posted = 0;
            mdc_busy.f.power_save = 0;
            power_state.f.saving = 0;
            power_state.f.enter_request = 0;
            _builtin_set_imask(saved_imask);
            return 0;
        }
    }
    _builtin_set_imask(15);
    tick_capture_wait_ticks.v = 0;
    _builtin_set_imask(saved_imask);
    while (1) {
        _builtin_set_imask(saved_imask);
        if (mdc_mailbox.cmd_state == MDC_CMD_STATE_ENDED) {
            _builtin_set_imask(15);
            mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
            mdc_cmd_posted.f.posted = 0;
            mdc_busy.f.power_save = 0;
            mdc_queued_cmd = 0;
            _builtin_set_imask(saved_imask);
            return 1;
        }
        if (tick_capture_wait_ticks.v > 0x2710) {
            _builtin_set_imask(15);
            mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
            mdc_cmd_posted.f.posted = 0;
            mdc_busy.f.power_save = 0;
            power_state.f.saving = 0;
            power_state.f.enter_request = 0;
            _builtin_set_imask(saved_imask);
            return 0;
        }
    }
}

int mdc_run_backing(int black)
{
    char cmd;
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (black != 0 && !mdc_mailbox.backing_position)
        return 1;
    else if (!black && mdc_mailbox.backing_position == MDC_BACKING_WHITE)
        return 1;
    if ((settings_test_flags[0] & 2) || !win_params.flags.f.adf)
        return 1;
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    if (!err_check_pending())
        return 0;
    _builtin_set_imask(15);
    if (black != 0)
        cmd = 0x63;
    else
        cmd = 0x62;
    mdc_queued_cmd = cmd;
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (mdc_busy.f.queued) {
    }
    if (mdc_cmd_posted.c.last_result == MDC_RESULT_ERROR)
        return 0;
    return 1;
}

void mdc_run_command_flag(unsigned char cmd, int flag)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    if (cmd == 30 || cmd == 28) {
        if (flag)
            mdc_queued_params[0] = 85;
        else
            mdc_queued_params[0] = 0;
    } else {
        if (flag)
            mdc_queued_params[0] = 0;
        else
            mdc_queued_params[0] = 85;
    }
    mdc_queued_cmd = cmd;
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (mdc_busy.f.queued) {
    }
    return;
}

int mdc_run_lamps(unsigned char lamp_bits)
{
    int saved_imask;
    int i;
    unsigned char requested;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    requested = lamp_bits;
    if (lamp_bits == 0x99) {
        lamp_bits = 1;
    }
    mdc_lamps_bits = lamp_bits;
    _builtin_set_imask(saved_imask);
    if (power_state.f.saving) {
        return 1;
    }
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    util_mem_clear(mdc_queued_params, 16);
    if (mdc_lamps_bits & 2) {
        mdc_queued_params[0] = 0x80;
    }
    if (mdc_lamps_bits & 4) {
        mdc_queued_param_byte_2[0] = 0x40;
    }
    if (mdc_lamps_bits & 1) {
        mdc_queued_param_byte_4[0] = 0x20;
    }
    if (mdc_lamp_cover_override) {
        mdc_queued_param_byte_1[0] = 1;
    } else {
        mdc_queued_param_byte_1[0] = 0;
    }
    mdc_queued_cmd = MDC_CMD_LAMPS;
    mdc_busy.f.queued = 1;

    _builtin_set_imask(saved_imask);
    while (1) {
        _builtin_set_imask(saved_imask);
        if (!mdc_busy.f.queued) {
            break;
        }
    }
    if (settings_test_flags[0] & 2) {
        return 1;
    }
    if (requested == 0x99) {
        return 1;
    }
    if (mdc_queued_params[0] || mdc_queued_param_byte_2[0] || mdc_queued_param_byte_4[0]) {
        for (i = 0; i < 100; i++) {
            rt_delay_60k();
        }
    }
    if (mdc_mailbox.error_code.v == ERR_LAMP_FUSE) {
        return 0;
    }
    if (mdc_cmd_posted.c.last_result == MDC_RESULT_ERROR) {
        return 0;
    }
    return 1;
}

int mdc_run_unit_power(unsigned char power_bits)
{
    int saved_imask;
    int u;
    int u2;

    saved_imask = _builtin_get_imask();
    if (power_state.f.saving) {
        return 1;
    }
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    util_mem_clear(mdc_queued_params, 16);
    mdc_queued_params[0] = power_bits;
    mdc_queued_param_byte_1[0] = power_bits;
    mdc_queued_param_byte_2[0] = 0;
    mdc_queued_cmd = MDC_CMD_UNIT_POWER;
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (1) {
        _builtin_set_imask(saved_imask);
        if (!mdc_busy.f.queued) {
            break;
        }
    }
    if (mdc_cmd_posted.c.last_result == MDC_RESULT_ERROR) {
        return 0;
    }
    return 1;
}

int mdc_run_warmup_on(void)
{
    int saved_imask;
    int u;
    int u2;

    saved_imask = _builtin_get_imask();
    if (power_state.f.saving) {
        return 1;
    }
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    mdc_queued_cmd = MDC_CMD_WARMUP_ON;
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (1) {
        _builtin_set_imask(saved_imask);
        if (!mdc_busy.f.queued) {
            break;
        }
    }
    if (mdc_cmd_posted.c.last_result == MDC_RESULT_ERROR) {
        return 0;
    }
    return 1;
}

int mdc_run_warmup_off(void)
{
    int saved_imask;
    int u;
    int u2;

    saved_imask = _builtin_get_imask();
    if (power_state.f.saving) {
        return 1;
    }
    while (mdc_busy.w) {
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    mdc_queued_cmd = MDC_CMD_WARMUP_OFF;
    mdc_busy.f.queued = 1;
    _builtin_set_imask(saved_imask);
    while (1) {
        _builtin_set_imask(saved_imask);
        if (!mdc_busy.f.queued) {
            break;
        }
    }
    if (mdc_cmd_posted.c.last_result == MDC_RESULT_ERROR) {
        return 0;
    }
    return 1;
}
