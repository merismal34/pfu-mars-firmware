#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);


int job_start_page(void)
{
    int u;
    unsigned short unused_frame;
    unsigned short last_page_count;
    int calib_requested;
    if (hw_hostif_flags.f.own_hostif) {
        _builtin_set_imask(15);
        err_display_flags.b.b4 = 1;
        _builtin_set_imask(0);
    }
    if ((settings_test_flags[0] & 4) || (settings_byte_0x1d[0] & 0x40)) {
        line_enable_capture(0);
        _builtin_set_imask(15);
        p_vdcc_capture[0]->capture_ctrl.b5 = 1;
        p_vdcc_capture[0]->capture_ctrl.line_gate = 1;
        _builtin_set_imask(0);
        if (win_sides.f.selection) {
            line_enable_capture(1);
            _builtin_set_imask(15);
            p_vdcc_capture[1]->capture_ctrl.b5 = 1;
            p_vdcc_capture[1]->capture_ctrl.line_gate = 1;
            _builtin_set_imask(0);
        }
        testmode_wait_lines(10);
        line_start_capture(0);
        if (win_sides.f.selection) {
            line_start_capture(1);
        }
    }
    _builtin_set_imask(15);
    line_page_timing[0].lines = 0;
    line_page_timing[1].lines = 0;
    if (hw_hostif_flags.f.own_hostif == 0) {
        if (hw_hostif_flags.f.cga_test_scan) {
            line_side_state[0].capture_lines = img_side_params[0].capture_length_lines + rt_divu16(img_side_params[0].y_res_dpi * 32 + 399, 400);
            line_side_state[1].capture_lines = img_side_params[1].capture_length_lines + rt_divu16(img_side_params[1].y_res_dpi * 32 + 399, 400);
        } else {
            line_side_state[0].capture_lines = img_side_params[0].capture_length_lines;
            line_side_state[1].capture_lines = img_side_params[1].capture_length_lines;
        }
        if (job_sheet_count == 0) {
            if (settings_manual_feed_timeout[0] > 4) {
                mdc_mailbox.manual_feed_timeout = 4;
            }
            if (settings_non_rectangular[0] & 1) {
                mdc_mailbox.non_rectangular &= 0xfe;
            }
        }
    }
    if (mdc_adf_deskew_or_overscan_on() && hw_hostif_flags.f.own_hostif && win_params.flags.f.adf && (settings_overscan_control[0] & 0x80)) {
        mdc_mailbox.overscan_control |= 0x80;
    } else {
        mdc_mailbox.overscan_control &= 0x7f;
    }
    _builtin_set_imask(0);
    if (mdc_fan_alarm.b6) {
        return 0;
    }
    if (win_params.flags.f.adf) {
        if (settings_test_flags[0] & 4) {
        } else {
            if (mdc_is_adf_cover_open()) {
                err_raise(ERR_ADF_COVER_OPEN);
                return 0;
            }
            if (!(maint_mode_word.v & 0x5000) && testmode_number.v == 0) {
                if (mdc_mailbox.cover_open_event == MDC_COVER_OPEN_EVENT_ADF_2) {
                    mdc_mailbox.cover_open_event = 0;
                    err_raise(ERR_ADF_COVER_OPEN_2);
                    return 0;
                }
                if (mdc_mailbox.cover_open_event == MDC_COVER_OPEN_EVENT_ADF) {
                    mdc_mailbox.cover_open_event = 0;
                    err_raise(ERR_ADF_COVER_OPEN);
                    return 0;
                }
            }
        }
    } else {
        _builtin_set_imask(15);
        if (win_sides.f.selection) {
            win_sides.f.selection = 0;
        }
        _builtin_set_imask(0);
    }
    if (settings_test_flags[0] & 4) {
        _builtin_set_imask(15);
        line_side_state[0].state_flags.f.page_state = 3;
        line_side_state[1].state_flags.f.page_state = 3;
        _builtin_set_imask(0);
        return 1;
    }
    {
        if (err_check_pending() == 0) {
            return 0;
        }
        _builtin_set_imask(15);
        line_motion_flags = 0;
        _builtin_set_imask(0);
        _builtin_set_imask(15);
        if ((job_long_page_flags & 0x80) && (job_long_page_flags & 2)) {
            if (win_sides.f.selection == 1) {
                line_side_state[1].state_flags.f.page_state = 3;
                line_page_state[1].state_flags.f.page_step_state = 3;
            } else {
                line_side_state[0].state_flags.f.page_state = 3;
                line_page_state[0].state_flags.f.page_step_state = 3;
                if (win_sides.f.selection == 2) {
                    line_side_state[1].state_flags.f.page_state = 3;
                    line_page_state[1].state_flags.f.page_step_state = 3;
                }
            }
            _builtin_set_imask(0);
            return 1;
        }
        {
            _builtin_set_imask(0);
            if (page_side_state[0].last_buf) {
                if (page_next_index(1)) {
                    mdc_mailbox.last_page.v = 0x80;
                } else if (page_side_state[0].last_page_forced) {
                    mdc_mailbox.last_page.v = 0x80;
                } else {
                    mdc_mailbox.last_page.v = 0;
                }
                if (job_mode_params[0].page_count_limit != 0xffff) {
                    last_page_count = job_mode_params[0].page_count_limit - 1;
                    if (page_fed_count.v >= last_page_count) {
                        mdc_mailbox.last_page.v = 0x80;
                    }
                }
                if ((settings_byte_0xcd[0] & 4) && img_side_params[0].y_res_dpi > 200) {
                    mdc_mailbox.last_page.v = 0;
                }
                if (job_black_backing == 0 && img_side_params[0].y_res_dpi == 600) {
                    mdc_mailbox.last_page.v = 0;
                }
                if (settings_manual_feed_timeout[0] != 0) {
                    mdc_mailbox.last_page.v = 0x80;
                }
                if (mdc_adf_deskew_or_overscan_on() && hw_hostif_flags.f.own_hostif && win_params.flags.f.adf && (settings_overscan_control[0] & 0x80)) {
                    mdc_mailbox.last_page.v = 0x80;
                }
                if ((job_buffer_option & 0x8000) || job_keep_backing) {
                    mdc_mailbox.last_page.v = 0x80;
                }
            }
            _builtin_set_imask(15);
            if (hw_hostif_flags.f.own_hostif && page_side_state[0].last_buf == 0) {
                calib_requested = 0;
            } else {
                calib_requested = 0;
                if (calib_levels_due) {
                    calib_levels_due = 0;
                    calib_levels_request = 1;
                    calib_requested = 1;
                }
                if (calib_exposure_due) {
                    calib_exposure_due = 0;
                    calib_exposure_request = 1;
                    calib_requested = 1;
                }
                if (calib_extrapolate_due) {
                    calib_extrapolate_due = 0;
                    if (job_black_backing) {
                        calib_extrapolate_request = 1;
                        mdc_mailbox.last_page.v = 0x80;
                    } else {
                        calib_extrapolate_request = 0;
                    }
                }
            }
            _builtin_set_imask(0);
            if (calib_requested) {
                mdc_mailbox.last_page.v = 0x81;
                if (job_black_backing) {
                    job_feed_backing_request = 0x80;
                } else {
                    job_feed_backing_request = 0;
                }
            } else {
                job_feed_backing_request = 0;
            }
            if (job_button_pause_flags & 0x8000) {
                mdc_mailbox.last_page.v = 0x80;
                mdc_mailbox.page_end |= MDC_PAGE_END_REQUEST;
            }
            if (job_pause_requested) {
                job_prepick_on = 0;
                job_pause_page_count.v++;
                mdc_mailbox.last_page.v = 0x80;
            }
            _builtin_set_imask(15);
            if (job_readtime_pages.v == 1) {
                tick_readtime_ticks.v = 0;
            } else {
                job_readtime_ticks.v = tick_readtime_ticks.v;
            }
            job_readtime_pages.v++;
            _builtin_set_imask(0);
            mdc_set_scan_params();
            return 1;
        }
    }
}


int job_wait_side_complete(int side)
{
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    while (1) {
        _builtin_set_imask(0);
        if (scanning_page->sides[side].flags.f.scanned) break;
        if (scanning_page->flags.v == 0) break;
        if (scanning_page->flags.f.scan_end != 0 || job_aborted) return 0;
        if (win_params.flags.f.adf && mdc_is_adf_cover_open()) {
            err_raise(ERR_ADF_COVER_OPEN);
            return 0;
        }
        if (job_check_mdc_error()) return 0;
        if (line_page_state[0].state_flags.f.page_error) {
            if (hw_hostif_flags.f.own_hostif) {
                err_set_illegal_request_detail(SCSI_SENSE_SKSV, 2, 0);
            } else {
                err_set_illegal_request_detail(0x20, 2, 0);
            }
            return 0;
        }
        if (mdc_cmd_posted.c.last_result == MDC_RESULT_TIMEOUT) return 0;
        if (job_buffer_feed_flags & 0x80) {
            if (line_motion_flags & 1) {
                _builtin_set_imask(15);
                scanning_page->flags.f.sheet_fed = 1;
                job_buffer_feed_flags &= 0xff7f;
                job_buffer_feed_flags |= 0x40;
                _builtin_set_imask(0);
            }
        }
    }
    return 1;
}

int job_wait_motion_end(void)
{
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    if (page_side_state[0].last_buf) {
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    }
    if (settings_test_flags[0] & 4) return 1;
    while (1) {
        _builtin_set_imask(0);
        if (line_motion_flags & 1) break;
        if (scanning_page->flags.f.scan_end != 0 || job_aborted) return 0;
        if (win_params.flags.f.adf && mdc_is_adf_cover_open()) {
            err_raise(ERR_ADF_COVER_OPEN);
            return 0;
        }
        if (job_check_mdc_error()) return 0;
        if (line_page_state[0].state_flags.f.page_error) {
            if (hw_hostif_flags.f.own_hostif) {
                err_set_illegal_request_detail(SCSI_SENSE_SKSV, 2, 0);
            } else {
                err_set_illegal_request_detail(0x20, 2, 0);
            }
            return 0;
        }
        if (mdc_cmd_posted.c.last_result == MDC_RESULT_TIMEOUT) return 0;
    }
    return 1;
}

int job_wait_page_event_80(int side)
{
    struct page_record *capture_page;
    capture_page = &page_side_records[page_side_state[side].capture_buf];
    while (1) {
        _builtin_set_imask(0);
        if ((page_side_state[side].events & 0x80) == 0) break;
        if (capture_page->flags.f.scan_end != 0 || job_aborted) return 0;
        if (line_page_state[0].state_flags.f.page_error) {
            if (hw_hostif_flags.f.own_hostif) {
                err_set_illegal_request_detail(SCSI_SENSE_SKSV, 2, 0);
            } else {
                err_set_illegal_request_detail(0x20, 2, 0);
            }
            return 0;
        }
    }
    return 1;
}

int job_check_mdc_error(void)
{
    unsigned char code;
    unsigned char cover_error;
    code = mdc_mailbox.error_code.v;
    if (code == 0x20) code = 0;
    cover_error = job_get_mdc_error();
    if (cover_error) {
        err_raise(cover_error);
        return 1;
    }
    if (code == 0x40) code = 0;
    if (code) return 1;
    return 0;
}

unsigned char job_get_mdc_error(void)
{
    if (win_params.flags.f.adf && !(maint_mode_word.v & 0x5000) && testmode_number.v == 0 && !(settings_test_flags[0] & 4)) {
        if (mdc_mailbox.cover_open_event == MDC_COVER_OPEN_EVENT_ADF_2 || mdc_mailbox.error_code.v == 0x4a) {
            mdc_mailbox.cover_open_event = 0;
            return 0x4a;
        }
        if (mdc_mailbox.cover_open_event == MDC_COVER_OPEN_EVENT_ADF || mdc_mailbox.error_code.v == ERR_ADF_COVER_OPEN) {
            mdc_mailbox.cover_open_event = 0;
            return 0x40;
        }
    }
    return 0;
}

int job_wait_link_page_lines(void)
{
    struct page_record *scanning_page;
    int at_pick_sensor;
    int side;
    scanning_page = &page_records[page_scanning.v];
    at_pick_sensor = job_page_end_at_pick_sensor;
    if (!win_params.flags.f.adf) at_pick_sensor = 0;
    if (win_sides.f.selection) {
        side = 1;
    } else {
        side = 0;
    }
    while (1) {
        _builtin_set_imask(0);
        if (at_pick_sensor) {
            if (line_side_state[0].state_flags.v & 0x200) break;
        }
        if (line_side_state[side].capture_lines == 0) break;
        if (scanning_page->flags.f.scan_end != 0 || job_aborted) return 0;
        if (win_params.flags.f.adf && mdc_is_adf_cover_open()) {
            err_raise(ERR_ADF_COVER_OPEN);
            return 0;
        }
        if (job_check_mdc_error()) return 0;
        if (mdc_cmd_posted.c.last_result == MDC_RESULT_TIMEOUT) return 0;
        if (settings_test_flags[0] & 4) break;
    }
    job_clear_page_record();
    return 1;
}

int job_clear_page_record(void)
{
    struct page_record *current_page;
    current_page = &page_records[page_current.v];
    if (testmode_number.v == 0x2a) return 1;
    _builtin_set_imask(15);
    util_mem_clear(current_page, 0x88);
    if (maint_mode_word.v & 0x5000) {
        _builtin_set_imask(0);
        return 1;
    }
    link_reply.c.code = 3;
    link_reply.f.b7 = 1;
    isr_request_command_layer(2);
    _builtin_set_imask(0);
    return 1;
}
