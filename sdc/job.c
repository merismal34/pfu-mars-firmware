#include "fw.h"

#pragma section H
extern void _builtin_set_imask(int);


/* One page of the job: source check and MDC feed (0x81), calibration, backing, page start (0x82), the waits for
   capture and motion end, job_end; in the page loop's phase 0 job_run_next_page does the following pages. */
int job_run(void)
{
    int z;
    int calibrated;
    int u8;
    unsigned char cover_error;
    int ret;
    z = 0;
    if (job_early_warmup == 1 && job_sheet_count)
        job_check_warmup_idle();
    _builtin_set_imask(15);
    if ((job_long_page_flags & 0x80) && (job_long_page_flags & 2)) {
        _builtin_set_imask(0);
        return ret = job_check_mdc_handshake();
    } else
        _builtin_set_imask(0);
    if (main_page_loop_phase == 0)
    {
        ret = job_run_next_page();
        return ret;
    }
    if (main_page_loop_phase == 2) {
    } else {
        /* PF1 high: the Empty sensor sees no paper */
        if (job_pause_requested != 0 && (job_pause_page_count.v >= 1 || hw_port_f->b1) && hw_hostif_flags.f.own_hostif) {
            err_raise(ERR_PAUSED_BY_USER);
            _builtin_set_imask(15);
            job_pause_requested = 0;
            job_pause_page_count.v = 0;
            _builtin_set_imask(0);
            return job_abort();
        }
        if (hw_hostif_flags.f.own_hostif && job_sheet_count && (job_button_pause_flags & 0x8000) && (mdc_mailbox.page_end & MDC_PAGE_END_REQUEST)) {
            ret = job_wait_mdc_page_end(0);
            if (ret)
                return job_abort();
        }
        if (job_sheet_count == 0) {
            mdc_mailbox.last_page.v = 0x80;
            if (!win_params.flags.f.adf) {
                job_feed_backing_request = 0;
            } else {
                if (mdc_mailbox.backing_position == MDC_BACKING_BLACK)
                    job_feed_backing_request = 0x80;
                else
                    job_feed_backing_request = 0;
            }
        } else if (settings_manual_feed_timeout[0] != 0) {
            mdc_mailbox.last_page.v = 0x80;
        } else {
            if (!hw_hostif_flags.f.own_hostif && img_side_params[0].y_res_dpi > 200) {
                if (settings_byte_0xcd[0] & 8)
                    mdc_mailbox.last_page.v = 0x80;
                else
                    mdc_mailbox.last_page.v = 0;
            } else if (hw_hostif_flags.f.own_hostif && img_side_params[0].y_res_dpi > 200 && (settings_byte_0xcd[0] & 4)) {
                mdc_mailbox.last_page.v = 0;
            } else {
                mdc_mailbox.last_page.v = 0x80;
            }
            if (mdc_adf_deskew_or_overscan_on() && hw_hostif_flags.f.own_hostif && win_params.flags.f.adf && (settings_overscan_control[0] & 0x80))
                mdc_mailbox.last_page.v = 0x80;
        }
        if (job_check_source() == 0)
            return job_abort();
        if (job_sheet_count == 0) {
            if (((settings_intelligent_multifeed[0] & 4) || (settings_intelligent_multifeed[0] & 6)) && (settings_intelligent_multifeed[0] & 0x80)) {
                mdc_run_read_overlap_patterns();
                util_mem_copy_from_odd((int)iic_eeprom_area2, (int)&mdc_back_window_overlap_patterns, 0x88);
                iic_eeprom_save_area2_range(0, 0x88);
            }
        }
        if (job_wait_mdc_ready() == 0)
            return job_abort();
        main_clear_error_display(0);
        if (job_prepare_pipeline() == 0)
            return job_abort();
        job_prepare_side(0);
        if (job_adf_sheet_count == 0 && job_sheet_count == 0 && job_switch_off_idle_heaters() == 0)
            return job_abort();
        if (job_setup_sensor() == 0)
            return job_abort();
        if (main_page_loop_phase == 1)
            return 1;
    }
    calibrated = job_calibrate();
    if (calibrated) {
        if (job_adf_sheet_count == 0 && job_sheet_count == 0) {
            _builtin_set_imask(15);
            job_readtime_pages.v = 0;
            if (win_params.flags.f.adf && img_side_params[0].y_res_dpi == 0xf0)
                calib_measure_mode_swap = 0x8000;
            else
                calib_measure_mode_swap = 0;
            _builtin_set_imask(0);
        }
        if (job_run_calibration() == 0)
            return job_abort();
    }
    job_prepare_side(1);
    job_apply_vdcc_debug_settings();
    if (job_adf_sheet_count == 0 && job_sheet_count == 0) {
        if (job_check_lamp(1) == 0)
            return job_abort();
    }
    if (win_params.flags.f.adf) {
        if (mdc_run_backing(job_black_backing) == 0) {
            cover_error = job_get_mdc_error();
            if (cover_error)
                err_raise(cover_error);
            return job_abort();
        }
        if (job_black_backing && job_check_adf_page_start()) {
            cover_error = job_get_mdc_error();
            if (cover_error)
                err_raise(cover_error);
            return job_abort();
        }
    }
    if (job_start_page() == 0)
        return job_abort();
    if (hw_hostif_flags.f.own_hostif) {
        if (page_side_state[0].last_buf) {
            _builtin_set_imask(15);
            job_scan_complete_wait = 0;
            _builtin_set_imask(0);
            if (job_wait_motion_end() == 0)
                return job_abort();
            if (job_buffer_feed_flags & 0x80) {
                _builtin_set_imask(15);
                page_records[0].flags.f.sheet_fed = 1;
                job_buffer_feed_flags &= 0xff7f;
                job_buffer_feed_flags |= 0x40;
                _builtin_set_imask(0);
            }
            if (job_wait_page_event_80(0) == 0)
                return job_abort();
            if (win_sides.f.selection) {
                if (job_wait_page_event_80(1) == 0)
                    return job_abort();
            }
        } else {
            _builtin_set_imask(15);
            if (win_params.flags.f.adf && !(job_long_page_flags & 0x80))
                job_scan_complete_wait = 0x8000;
            else
                job_scan_complete_wait = 0;
            _builtin_set_imask(0);
            if (job_wait_side_complete(0) == 0)
                return job_abort();
            if (win_sides.f.selection) {
                if (job_wait_side_complete(1) == 0)
                    return job_abort();
            }
            if (job_buffer_feed_flags & 0x8000) {
                if (job_wait_side_requested() == 0)
                    return job_abort();
            }
            if (job_scan_complete_wait) {
                if (job_wait_motion_end() == 0)
                    return job_abort();
                _builtin_set_imask(15);
                job_scan_complete_wait = 0;
                _builtin_set_imask(0);
            }
        }
    } else {
        if (job_wait_link_page_lines() == 0)
            return job_abort();
        if (win_params.flags.f.adf && job_page_end_at_pick_sensor) {
            _builtin_set_imask(15);
            job_link_abort_flags = 0x8000;
            _builtin_set_imask(0);
            if (job_wait_motion_end() == 0)
                return job_abort();
        }
        _builtin_set_imask(15);
        job_link_abort_flags = 0;
        _builtin_set_imask(0);
    }
    if (job_end() == 0)
        return job_abort();
    return 1;
}

int job_run_next_page(void)
{
    unsigned char cover_error;
    int ret;
    if (job_pause_requested) {
        if ((job_pause_page_count.v >= 2 || hw_port_f->b1) && hw_hostif_flags.f.own_hostif) {
            err_raise(ERR_PAUSED_BY_USER);
            _builtin_set_imask(15);
            job_pause_requested = 0;
            job_pause_page_count.v = 0;
            _builtin_set_imask(0);
            return job_abort();
        }
    }
    if (hw_hostif_flags.f.own_hostif && (job_button_pause_flags & 0x8000) && (mdc_mailbox.page_end & MDC_PAGE_END_REQUEST)) {
        if ((ret = job_wait_mdc_page_end(1)) != 0)
            return job_abort();
    }
    if (job_aborted)
        return job_abort();
    else {
        cover_error = job_get_mdc_error();
        if (cover_error) {
            err_raise(cover_error);
            return job_abort();
        }
    }
    if (mdc_run_feed() == 0)
        return job_abort();
    job_prepare_side(0);
    if (job_check_feed_start() == 0)
        return job_abort();
    if (calib_levels_request != 0 || calib_exposure_request != 0 || calib_extrapolate_request != 0 || job_black_backing == 0) {
        if (job_run_calibration() == 0)
            return job_abort();
        if (job_keep_warm_relit) {
            _builtin_set_imask(15);
            job_keep_warm_relit = 0;
            _builtin_set_imask(0);
            if (job_check_lamp(0) == 0)
                return job_abort();
        }
        if (win_params.flags.f.adf) {
            if (mdc_run_backing(job_black_backing) == 0) {
                cover_error = job_get_mdc_error();
                if (cover_error)
                    err_raise(cover_error);
                return job_abort();
            }
            if (job_black_backing && job_check_adf_page_start()) {
                cover_error = job_get_mdc_error();
                if (cover_error)
                    err_raise(cover_error);
                return job_abort();
            }
        }
    } else {
        if (win_params.flags.f.adf && job_black_backing) {
        if (mdc_mailbox.backing_position != MDC_BACKING_BLACK) {
            if (mdc_mailbox.last_page.v == 0x80) {
                if (mdc_run_backing(job_black_backing) == 0) {
                    cover_error = job_get_mdc_error();
                    if (cover_error)
                        err_raise(cover_error);
                    return job_abort();
                }
                if (job_check_adf_page_start()) {
                    cover_error = job_get_mdc_error();
                    if (cover_error)
                        err_raise(cover_error);
                    return job_abort();
                }
            } else {
                mdc_request_abort();
                _builtin_set_imask(15);
                tick_wait_count = 0;
                _builtin_set_imask(0);
                while (tick_wait_count < 2500) {
                    if (mdc_mailbox.error_code.v != 0) {
                        mdc_run_command(MDC_CMD_CLEAR_ERRORS);
                        break;
                    }
                }
                err_raise(ERR_JAM);
                return job_abort();
            }
        }
        }
    }
    job_prepare_side(1);
    if (job_start_page() == 0)
        return job_abort();
    if (job_wait_motion_end() == 0)
        return job_abort();
    if (job_wait_page_event_80(0) == 0)
        return job_abort();
    if (win_sides.f.selection) {
        if (job_wait_page_event_80(1) == 0)
            return job_abort();
    }
    if (job_end() == 0)
        return job_abort();
    return 1;
}

int job_check_source(void)
{
    struct page_record *scanning_page;
    int ok;
    int u12;
    unsigned short page_limit;
    int i;
    unsigned char err_class;
    scanning_page = &page_records[page_scanning.v];
    ok = 1;
    _builtin_set_imask(15);
    if (mdc_capture_pause_sent == 1) {
        _builtin_set_imask(0);
        mdc_request_capture(1);
        for (i = 0; i <= 50; i++) {
            if (!mdc_mailbox.request)
                break;
            rt_delay_180k();
            rt_delay_60k();
        }
    }
    _builtin_set_imask(0);
    if (job_aborted)
        return 0;
    if (mdc_fan_alarm.b6)
        return 0;
    if (err_shipping_lock_pending) {
        if (job_manual_feed_flags.w & 0x8000) {
            _builtin_set_imask(15);
            job_manual_feed_flags.w |= 8;
            _builtin_set_imask(0);
            job_wait_load_pending();
        }
        err_raise(ERR_SHIPPING_LOCK);
        _builtin_set_imask(15);
        err_shipping_lock_pending = 0;
        _builtin_set_imask(0);
        return 0;
    }
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.own_hostif && !(maint_mode_word.v & 0x5000)) {
        if (scanning_page->flags.f.adf)
            win_params.flags.f.adf = 1;
        else
            win_params.flags.f.adf = 0;
    } else if (settings_byte_0x9a[0] & 1) {
        win_params.flags.f.adf = 0;
    }
    if (job_manual_feed_flags.w & 0x8000)
        win_params.flags.f.adf = 1;
    _builtin_set_imask(0);
    if (win_params.flags.f.adf) {
        mdc_mailbox.source = MDC_SOURCE_ADF;
    } else {
        mdc_mailbox.source = MDC_SOURCE_FLATBED;
        afe_select_source();
    }
    _builtin_set_imask(15);
    panel_multifeed_blink_state.v &= 0x7fff;
    job_scan_complete_wait = 0;
    _builtin_set_imask(0);
    if (!(maint_mode_word.v & 0x5000))
        panel_set_display_mode(0, PANEL_MODE_FUNCTION);
    if (!win_params.flags.f.adf)
        return job_report_page_to_link();
    _builtin_set_imask(15);
    link_reply_11_value.v = 0;
    scsi_unusual_paper_state = 0;
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    job_separation_sheet_flags = 0;
    _builtin_set_imask(0);
    if (job_sheet_count == 0) {
        if (mdc_mailbox.error_code.v != 0) {
            err_class = err_get_class(mdc_mailbox.error_code.v);
            switch (err_class) {
            case 1:
            case 2:
            case 3:
                mdc_run_command(MDC_CMD_CLEAR_ERRORS);
                break;
            default:
                break;
            }
        }
        if (err_check_pending() == 0)
            return 0;
        _builtin_set_imask(15);
        job_elapsed_seconds = 0;
        button_send_to_wait_release.b6 = 1;
        mdc_mailbox.page_end = 0;
        job_button_pause_flags = 0;
        _builtin_set_imask(0);
        if (hw_hostif_flags.f.own_hostif) {
            if (job_mode_params[0].page_count_limit == 0xffff) {
                page_limit = 0;
            } else {
                page_limit = job_mode_params[0].page_count_limit;
                if (page_fed_count.v != 0 && page_limit > page_fed_count.v)
                    page_limit = page_limit - page_fed_count.v;
            }
            _builtin_set_imask(15);
            mdc_mailbox.page_count_limit_low.v = page_limit;
            mdc_mailbox.page_count_limit_high.v = page_limit >> 8;
            _builtin_set_imask(0);
        } else {
            _builtin_set_imask(15);
            mdc_mailbox.page_count_limit_low.v = 0;
            mdc_mailbox.page_count_limit_high.v = 0;
            _builtin_set_imask(0);
            if (settings_manual_feed_timeout[0] > 4) {
                _builtin_set_imask(15);
                mdc_mailbox.manual_feed_timeout = 4;
                _builtin_set_imask(0);
            }
            if (settings_non_rectangular[0] & 1) {
                _builtin_set_imask(15);
                mdc_mailbox.non_rectangular &= 0xfe;
                _builtin_set_imask(0);
            }
        }
    } else {
        if (!(settings_test_flags[0] & 4) && !(maint_mode_word.v & 0x5000) && testmode_number.v == 0) {
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
    _builtin_set_imask(15);
    mdc_mailbox.manual_feed_end = 0;
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    job_feed_starting = 1;
    _builtin_set_imask(0);
    if (job_sheet_count == 0) {
        _builtin_set_imask(15);
        calib_lamp_relit = 0;
        job_keep_warm_relit = 0;
        calib_lamp_cold = 0;
        _builtin_set_imask(0);
        if (tick_lamp_off_ticks > 0x1388) {
            win_params.flags.f.adf = 1;
            afe_select_source();
            power_restart_sensor_path(0);
        } else {
            if (tick_keep_warm_seen) {
                _builtin_set_imask(15);
                calib_lamp_relit = 1;
                tick_relight_ticks = 0;
                _builtin_set_imask(0);
            }
            _builtin_set_imask(15);
            tick_lamp_off_ticks = 0;
            _builtin_set_imask(0);
        }
    }
    if ((job_long_page_flags & 0x80) && job_sheet_count == 0) {
        if ((mdc_mailbox.lamp_status & MDC_LAMP_STATUS_ADF_BOTH) != MDC_LAMP_STATUS_ADF_BOTH)
            mdc_run_lamps(7);
        _builtin_set_imask(15);
        tick_wait_count = 0;
        _builtin_set_imask(0);
        while (tick_wait_count < 0x9c4)
            ;
    }
    if (tick_mdc_start_ticks < 0x7d0 && job_sheet_count == 0) {
        _builtin_set_imask(15);
        tick_wait_count = 0;
        _builtin_set_imask(0);
        while (tick_wait_count < 0x3e8)
            ;
    }
    if (mdc_run_feed() == 0) {
        if (job_manual_feed_flags.w & 0x8000) {
            _builtin_set_imask(15);
            job_manual_feed_flags.w |= 8;
            _builtin_set_imask(0);
            job_wait_load_pending();
        }
        if (mdc_mailbox.error_code.v == ERR_PICK && mdc_is_paper_in_hopper() && !(job_dirty_roller_flags.v & 0x100)) {
            _builtin_set_imask(15);
            /* the next pick error 0x50 is reported as 0x39 (dirty rollers) */
            if (++job_pick_error_count >= 4) {
                job_dirty_roller_flags.v |= 0x100;
                job_pick_error_count = 0;
            }
            _builtin_set_imask(0);
        }
        err_check_pending();
        _builtin_set_imask(15);
        job_feed_starting = 0;
        _builtin_set_imask(0);
        return 0;
    }
    if (hw_hostif_flags.f.own_hostif && settings_manual_feed_timeout[0] != 0) {
        if (job_aborted)
            return 0;
    }
    if (job_manual_feed_flags.w & 0x8000) {
        _builtin_set_imask(15);
        job_manual_feed_flags.w |= 1;
        _builtin_set_imask(0);
        job_wait_load_pending();
    }
    _builtin_set_imask(15);
    job_feed_starting = 0;
    job_separation_sheet_flags = 0x80;
    if (mdc_mailbox.separation_sheet_flags & MDC_SEPARATION_SHEET_DETECTED) {
        job_separation_sheet_flags |= 0x40;
        job_buffer_feed_flags = 0;
    }
    _builtin_set_imask(0);
    return job_mark_sheet_fed(0);
}

int job_check_feed_start(void)
{
    _builtin_set_imask(15);
    job_feed_starting = 0;
    _builtin_set_imask(0);
    while (mdc_busy.f.feed) {
        if (job_aborted)
            return 0;
        if (win_params.flags.f.adf && mdc_is_adf_cover_open()) {
            err_raise(ERR_ADF_COVER_OPEN);
            return 0;
        }
        if (settings_manual_feed_timeout[0] != 0 && job_pause_requested && hw_hostif_flags.f.own_hostif && hw_port_f->b1) {
            _builtin_set_imask(15);
            mdc_mailbox.manual_feed_end = 1;
            job_manual_feed_flags.w |= 0x80;
            _builtin_set_imask(0);
            err_raise(ERR_PAUSED_BY_USER);
            _builtin_set_imask(15);
            job_pause_requested = 0;
            job_pause_page_count.v = 0;
            _builtin_set_imask(0);
        }
    }
    if (mdc_cmd_posted.c.last_result == MDC_RESULT_ERROR) {
        _builtin_set_imask(15);
        job_feed_starting = 1;
        _builtin_set_imask(0);
        err_check_pending();
        return 0;
    }
    if (mdc_cmd_posted.c.last_result == MDC_RESULT_TIMEOUT) {
        _builtin_set_imask(15);
        job_feed_starting = 1;
        _builtin_set_imask(0);
        return 0;
    }
    if (job_manual_feed_flags.w & 0x8000) {
        _builtin_set_imask(15);
        job_manual_feed_flags.w |= 1;
        _builtin_set_imask(0);
        if (tick_manual_feed_seconds.v > 1) {
            if (calib_levels_due) {
                _builtin_set_imask(15);
                calib_levels_due = 0;
                calib_levels_request = 1;
                _builtin_set_imask(0);
            } else if (calib_exposure_due) {
                _builtin_set_imask(15);
                calib_exposure_due = 0;
                calib_exposure_request = 1;
                _builtin_set_imask(0);
            }
            if (mdc_mailbox.lamp_status & MDC_LAMP_STATUS_KEEP_WARM) {
                _builtin_set_imask(15);
                job_keep_warm_relit = 1;
                calib_lamp_relit = 1;
                calib_levels_request = 1;
                _builtin_set_imask(0);
            }
        }
    }
    return job_mark_sheet_fed(1);
}

int job_mark_sheet_fed(int from_capture_buf)
{
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    if (from_capture_buf)
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    _builtin_set_imask(15);
    job_read_8_byte_3 = 3;
    if (win_params.paper_size.f.largest_paper) {
        win_params.paper_size.f.paper_code = 0;
        win_params.paper_size.v |= 3;
    }
    if (from_capture_buf == 0 && (job_buffer_feed_flags & 0x8000))
        job_buffer_feed_flags |= 0x80;
    else
        scanning_page->flags.f.sheet_fed = 1;
    if (settings_manual_feed_timeout[0] != 0) {
        tick_page_wait_ticks.v = 0;
        calib_levels_deferred = 0;
    }
    _builtin_set_imask(0);
    return job_report_page_to_link();
}

int job_report_page_to_link(void)
{
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    if (page_side_state[0].last_buf)
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    _builtin_set_imask(15);
    if (scanning_page->flags.f.scan_end) {
        _builtin_set_imask(0);
        return job_hook_page_flag();
    }
    if (!hw_hostif_flags.f.own_hostif && scanning_page->flags.f.load_pending) {
        scanning_page->flags.f.load_pending = 0;
        link_reply.c.code = 2;
        link_reply.f.b7 = 1;
        isr_request_command_layer(2);
    }
    _builtin_set_imask(0);
    return 1;
}

int job_wait_mdc_ready(void)
{
    int cover_open;
    int ok;
    int u4;
    struct page_record *scanning_page;
    ok = 1;
    scanning_page = &page_records[page_scanning.v];
    _builtin_set_imask(15);
    scanning_page->sides[0].size_final = 0;
    scanning_page->sides[1].size_final = 0;
    tick_wait_count = 0;
    cover_open = 0;
    mdc_mailbox.cover_open_event = 0;
    _builtin_set_imask(0);
    if (job_buffer_feed_flags & 0x80) {
        job_wait_mdc_lines();
        return 1;
    }
    for (;;) {
        _builtin_set_imask(15);
        if ((main_page_loop_phase == 1 && scanning_page->sides[0].flags.f.requested && win_sides.f.selection != 1)
            || (main_page_loop_phase == 1 && scanning_page->sides[1].flags.f.requested && win_sides.f.selection == 1)
            || (main_page_loop_phase != 1 && scanning_page->flags.f.scan_requested)) {
            if (!hw_hostif_flags.f.own_hostif) {
                if (settings_test_flags[0] & 4) {
                    _builtin_set_imask(0);
                    break;
                }
                if (line_side_state[0].state_flags.s.page_state == 0 && line_side_state[1].state_flags.s.page_state == 0) {
                    line_side_state[0].state_flags.v = line_side_state[1].state_flags.v = 0;
                    _builtin_set_imask(0);
                    break;
                }
            } else {
                _builtin_set_imask(0);
                break;
            }
        }
        if (scanning_page->flags.f.load_pending && !scanning_page->flags.f.sheet_fed) {
            scanning_page->flags.f.adf = 1;
            _builtin_set_imask(0);
            if (job_check_source() == 0)
                return 0;
            continue;
        }
        if (scanning_page->flags.f.scan_end) {
            _builtin_set_imask(0);
            job_wait_mdc_lines();
            return 0;
        }
        if (job_aborted) {
            _builtin_set_imask(0);
            return 0;
        }
        _builtin_set_imask(0);
        if (settings_test_flags[0] & 4) {
            cover_open = 0;
            continue;
        }
        if (!mdc_mailbox.sensor_status.f.adf_cover_open) {
            tick_wait_count = 0;
            continue;
        }
        if (tick_wait_count >= 50)
            cover_open = 1;
    }
    if (cover_open != 0) {
        cover_open = 0;
        err_raise(ERR_ADF_COVER_OPEN);
        return 0;
    }
    if (!hw_hostif_flags.f.own_hostif && err_check_pending() == 0)
        return 0;
    return 1;
}

int job_wait_side_requested(void)
{
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    if (job_buffer_feed_flags & 0x80) {
        _builtin_set_imask(15);
        scanning_page->flags.f.sheet_fed = 1;
        job_buffer_feed_flags &= 0xff7f;
        job_buffer_feed_flags |= 0x40;
        _builtin_set_imask(0);
    }
    for (;;) {
        if ((scanning_page->sides[0].flags.f.requested && win_sides.f.selection != 1) || (scanning_page->sides[1].flags.f.requested && win_sides.f.selection == 1))
            break;
        if (job_aborted)
            return 0;
    }
    return 1;
}

int job_wait_load_pending(void)
{
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    if (settings_test_flags[0] & 4)
        return 1;
    if (!hw_hostif_flags.f.own_hostif)
        return 1;
    for (;;) {
        if (scanning_page->flags.f.load_pending)
            break;
        if (job_aborted)
            return 0;
        if (tick_manual_feed_seconds.v > 5) {
            _builtin_set_imask(15);
            job_manual_feed_flags.w |= 8;
            _builtin_set_imask(0);
        }
    }
    return 1;
}

/* Heater of the side not scanned off; on the ADF waits until the monitor has switched the other one off. */
int job_switch_off_idle_heaters(void)
{
    if (tick_heater_monitor == 0)
        return 1;
    if ((settings_byte_0x1d[0] & 0x10) || (settings_calib_options[0] & 4) || (settings_test_flags[0] & 0x10) || (settings_test_flags[0] & 2)) {
        _builtin_set_imask(15);
        tick_heater_monitor = 0;
        _builtin_set_imask(0);
        mdc_request_heater_off(3);
        return 1;
    }
    if (win_sides.f.selection == 0) {
        _builtin_set_imask(15);
        tick_heater_monitor &= 0xff7f;
        _builtin_set_imask(0);
        mdc_request_heater_off(2);
    } else if (win_sides.f.selection == 1) {
        _builtin_set_imask(15);
        tick_heater_monitor &= 0x7fff;
        _builtin_set_imask(0);
        mdc_request_heater_off(1);
    }
    if (win_params.flags.f.adf) {
        while (tick_heater_monitor & 0x8080) {
            if (job_aborted)
                return 0;
        }
        _builtin_set_imask(15);
        if (win_sides.f.selection == 0)
            tick_heater_monitor_changed_back[0] = 0;
        else if (win_sides.f.selection == 1)
            tick_heater_monitor_changed[0] = 0;
        _builtin_set_imask(0);
    } else {
        _builtin_set_imask(15);
        tick_heater_monitor = 0;
        _builtin_set_imask(0);
        mdc_request_heater_off(3);
    }
    return 1;
}

