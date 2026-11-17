#include "fw.h"
extern int _builtin_get_imask(void);
extern void _builtin_set_imask(int);


struct vdcc_reg_04 { char p[4]; struct vdcc_mdc_event_bits mdc_event; };

/* Uninitialised data (section B). */
int mdc_saved_imask;
#pragma section H


void mdc_poll(void)
{
    if (page_poll_count % 8) {
        return;
    }
    mdc_saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    mdc_wait_ticks++;
    _builtin_set_imask(mdc_saved_imask);
    if (mdc_busy.w == 0) {
        return;
    }
    if (power_state.f.saving && !mdc_busy.f.power_save) {
        _builtin_set_imask(15);
        mdc_busy.w = 0;
        _builtin_set_imask(mdc_saved_imask);
        return;
    }
    if (mdc_busy.f.queued) {
        mdc_send_queued_command();
    }
    if (mdc_busy.f.eject) {
        mdc_send_eject();
    } else if (mdc_busy.f.unused) {
        mdc_clear_busy_unused();
    } else if (mdc_busy.f.power_save) {
        mdc_send_power_save();
    }
}

void mdc_send_eject(void)
{
    if (mdc_cmd_posted.f.posted) {
        if (!mdc_poll_result()) {
            return;
        }
        _builtin_set_imask(15);
        mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
        mdc_cmd_posted.f.posted = 0;
        mdc_busy.f.eject = 0;
        mdc_queued_cmd = 0;
        mdc_df_test_result_log[mdc_cmd_log_index] = 255;
        (&mdc_df_test_result_log[mdc_cmd_log_index])[1] = 136;
        _builtin_set_imask(mdc_saved_imask);
        return;
    }
    else if (mdc_mailbox.cmd_state) {
        return;
    }
    mdc_send_command(MDC_CMD_EJECT);
    _builtin_set_imask(mdc_saved_imask);
    return;
}

void mdc_run_flatbed_scan_move(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (power_state.f.saving) {
        return;
    }
    if (job_long_page_flags & 0x80) {
        return;
    }
    mdc_mailbox.feed_options |= MDC_FEED_MULTIFEED_REPORT_ONLY;
    mdc_mailbox.source = 0;
    mdc_mailbox.lead_front_low = (unsigned char)line_page_timing[0].lead_lines;
    mdc_mailbox.lead_front_high = line_page_timing[0].lead_lines >> 8;
    mdc_mailbox.end_lead_front_low.v = 0xbd;
    mdc_mailbox.end_lead_front_high.v = 0;
    mdc_send_command(MDC_CMD_SCAN_MOVE);
    while (1) {
        if (((struct vdcc_reg_04 *)p_vdcc_front[0])->mdc_event.mdc_result_ready == 0) {
            if (mdc_wait_ticks >= 7500) {
                return;
            }
        } else {
            break;
        }
    }
    _builtin_set_imask(15);
    mdc_cmd_posted.c.last_result = mdc_mailbox.cmd_result;
    ((struct vdcc_reg_04 *)p_vdcc_front[0])->mdc_event.mdc_result_ready = 1;
    mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
    mdc_cmd_posted.f.posted = 0;
    mdc_busy.w = 0;
    mdc_queued_cmd = 0;
    if (job_mode_params[0].job_separation[0] & 0x80) {
        job_separation_motion_wait = 1;
        job_scan_complete_wait = 0x8000;
    }
    _builtin_set_imask(saved_imask);
    return;
}

void mdc_send_df_test(int on)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (on) {
        mdc_send_command(MDC_CMD_DF_TEST_ON);
    } else {
        mdc_send_command(MDC_CMD_DF_TEST_OFF);
    }
    while (1) {
        if (((struct vdcc_reg_04 *)p_vdcc_front[0])->mdc_event.mdc_result_ready == 0) {
            if (mdc_wait_ticks >= 7500) {
                return;
            }
        } else {
            break;
        }
    }
    _builtin_set_imask(15);
    mdc_cmd_posted.c.last_result = mdc_mailbox.cmd_result;
    ((struct vdcc_reg_04 *)p_vdcc_front[0])->mdc_event.mdc_result_ready = 1;
    mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
    mdc_cmd_posted.f.posted = 0;
    mdc_busy.w = 0;
    mdc_queued_cmd = 0;
    _builtin_set_imask(saved_imask);
    return;
}

void mdc_clear_busy_unused(void)
{
    _builtin_set_imask(15);
    mdc_busy.f.unused = 0;
    _builtin_set_imask(mdc_saved_imask);
    return;
}

void mdc_send_queued_command(void)
{
    if (mdc_cmd_posted.f.posted) {
        if (!mdc_poll_result()) {
            if (!mdc_cmd_posted.f.end_requested) {
                return;
            }
            if (mdc_mailbox.cmd_state != MDC_CMD_STATE_DONE) {
                return;
            }
        }
        _builtin_set_imask(15);
        mdc_cmd_posted.f.end_requested = 0;
        mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
        mdc_cmd_posted.f.posted = 0;
        mdc_busy.f.queued = 0;
        mdc_queued_cmd = 0;
        _builtin_set_imask(mdc_saved_imask);
        return;
    }
    if (mdc_queued_cmd == 0x48) {
        util_mem_copy_to_odd(mdc_cmd_params, mdc_queued_params, 16);
        mdc_send_command(mdc_queued_cmd);
        return;
    }
    if (mdc_mailbox.cmd_state != MDC_CMD_STATE_IDLE) {
        _builtin_set_imask(15);
        if (mdc_mailbox.cmd_state == MDC_CMD_STATE_DONE) {
            mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
        }
        _builtin_set_imask(mdc_saved_imask);
        return;
    }
    else {
        util_mem_copy_to_odd(mdc_cmd_params, mdc_queued_params, 16);
        mdc_send_command(mdc_queued_cmd);
    }
}

void mdc_send_power_save(void)
{
    if (mdc_cmd_posted.f.posted) {
        if (mdc_mailbox.cmd_state == MDC_CMD_STATE_DONE) {
            _builtin_set_imask(15);
            mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
            mdc_cmd_posted.f.posted = 0;
            mdc_busy.f.power_save = 0;
            mdc_queued_cmd = 0;
            _builtin_set_imask(mdc_saved_imask);
            return;
        }
    } else {
        if (mdc_mailbox.cmd_state) {
            return;
        }
        mdc_send_command(MDC_CMD_POWER_SAVE);
    }
    return;
}


void mdc_send_command(unsigned char cmd)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (mdc_capture_resume_repeat == 1) {  /* repeat the capture end the MDC was not ready for */
        mdc_capture_pause_sent = 1;
        mdc_request_capture(1);
        mdc_capture_resume_repeat = 0;
    }
    mdc_wait_ticks = 0;
    ((struct vdcc_reg_04 *)p_vdcc_front[0])->mdc_event.mdc_result_ready = 1;
    mdc_cmd_posted.c.last_result = MDC_RESULT_PENDING;
    mdc_cmd_posted.f.posted = 1;
    mdc_queued_cmd = cmd;
    mdc_mailbox.cmd_code = cmd;
    mdc_mailbox.cmd_state = MDC_CMD_STATE_POSTED;
    mdc_cmd_log_index++;
    if (mdc_cmd_log_index == 95) {
        mdc_cmd_log_index = 0;
    }
    mdc_cmd_log[mdc_cmd_log_index] = cmd;
    (&mdc_cmd_log[mdc_cmd_log_index])[1] = 136;
    log_add_event(1, cmd);
    log_add_record(1, cmd, 0, 0);
    _builtin_set_imask(saved_imask);
}

int mdc_poll_result(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (!((struct vdcc_reg_04 *)p_vdcc_front[0])->mdc_event.mdc_result_ready) {
        if (settings_manual_feed_timeout[0] && mdc_busy.f.feed) {
        if (4000 < mdc_wait_ticks) {
            _builtin_set_imask(15);
            mdc_wait_ticks = 1000;
            _builtin_set_imask(saved_imask);
        }
        }
        if (15000 <= mdc_wait_ticks) {
            mdc_cmd_result_log[mdc_cmd_log_index] = 0x99;
            log_add_event(0x81, 0xf0);
            log_add_record(0x81, 0xf0, 0, 0);
            if (mdc_busy.f.feed || mdc_busy.f.scan_move) {
                _builtin_set_imask(15);
                mdc_cmd_posted.c.last_result = MDC_RESULT_TIMEOUT;
                _builtin_set_imask(saved_imask);
            } else {
                err_raise(ERR_MDC_TIMEOUT);
            }
            _builtin_set_imask(saved_imask);
            return 1;
        } else if (5000 <= mdc_wait_ticks) {
            if (mdc_mailbox.error_code.v) {
                _builtin_set_imask(15);
                mdc_cmd_posted.c.last_result = mdc_mailbox.cmd_result;
                mdc_cmd_result_log[mdc_cmd_log_index] = mdc_cmd_posted.c.last_result;
                (&mdc_cmd_result_log[mdc_cmd_log_index])[1] = 0x88;
                log_add_event(0x81, 0xaa);
                log_add_record(0x81, 0xaa, 0, 0);
                if (mdc_cmd_posted.c.last_result != MDC_RESULT_ERROR) {
                    mdc_cmd_posted.c.last_result = MDC_RESULT_ERROR;
                }
                ((struct vdcc_reg_04 *)p_vdcc_front[0])->mdc_event.mdc_result_ready = 1;
                _builtin_set_imask(saved_imask);
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }
    _builtin_set_imask(15);
    mdc_cmd_posted.c.last_result = mdc_mailbox.cmd_result;
    mdc_cmd_result_log[mdc_cmd_log_index] = mdc_cmd_posted.c.last_result;
    (&mdc_cmd_result_log[mdc_cmd_log_index])[1] = 0x88;
    log_add_event(0x81, mdc_cmd_posted.c.last_result);
    log_add_record(0x81, mdc_cmd_posted.c.last_result, 0, 0);
    ((struct vdcc_reg_04 *)p_vdcc_front[0])->mdc_event.mdc_result_ready = 1;
    _builtin_set_imask(saved_imask);
    return 1;
}
