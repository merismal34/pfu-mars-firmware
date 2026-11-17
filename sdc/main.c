#define job_auto_size_flags job_auto_size_flags_hdr
#include "fw.h"
#undef job_auto_size_flags
#pragma section H


extern unsigned short job_auto_size_flags;

extern void _builtin_set_imask(int);

void main_loop(void)
{
    while (1)
        main_poll();
}

void main_poll(void)
{
    struct page_record *scanning_page;

    scanning_page = &page_records[page_scanning.v];
    _builtin_set_imask(15);
    if (scanning_page->flags.f.load_pending || scanning_page->sides[0].flags.f.requested || (job_manual_feed_flags.w & 0x8000)) {
        _builtin_set_imask(0);
        if (power_state.f.saving) {
            power_state.f.leave_request = 1;
            power_leave_save();
        }
        main_run_page_loop();
    }
    _builtin_set_imask(0);
    main_update_counters();
    main_task_maint_mode();
    main_task_testmode();
    main_task_wake_requests();
    power_task_power_save();
    main_task_start_updater();
    main_task_download();
    power_task_auto_off();
    if (!power_state.f.saving) {
        main_task_clear_error_display();
        main_task_restore_display();
        main_task_retry_mdc_error();
        main_task_turn_rollers();
        main_task_multifeed_buttons();
        main_task_apply_link_settings();
        main_task_save_settings();
        main_task_save_area2();
        main_task_select_adf_source();
        main_task_idle_lamp_off();
        main_task_idle_park();
        calib_task_recalibrate();
    }
    power_task_mdc_retry();
    if (power_mdc_restart_request)
        power_task_mdc_restart();
}

void main_run_page_loop(void)
{
    int saved_flags, saved_back_flags, saved_front_flags;
    char saved_multifeed_flags;
    unsigned char emulation;

    _builtin_set_imask(15);
    main_page_loop_phase = 1;
    job_aborted = 0;
    tick_page_wait_ticks.v = 0;
    calib_levels_deferred = 0;
    main_multifeed_retry_count = 0;
    main_park_pending = 1;
    if ((maint_mode_word.v & 0x5000) || testmode_number.v) {
        job_long_page_flags = 0;
        mdc_mailbox.long_page = 0;
    } else {
        page_records[0].flags.v = page_records[page_scanning.v].flags.v & PAGE_FLAGS_REPLY_PENDING;
        page_scanning.v = page_current.v = 0;
        page_records[page_scanning.v].flags.v &= PAGE_FLAGS_REPLY_PENDING;
        if (hw_hostif_flags.f.own_hostif && (settings_long_page_enable[0] & 8)) {
            job_long_page_flags |= 0x80;
            if (!win_params.paper_size.f.auto_length || (win_params.side[page_scanning.v].compression.f.type & 0x80) || win_params.y_res_dpi.v > 400
                || win_sides.f.selection == 2 || win_sides.f.selection == 1 || !win_params.flags.f.adf)
                job_long_page_flags = 0;
        } else
            job_long_page_flags = 0;
        if (job_long_page_flags & 0x80)
            mdc_mailbox.long_page = MDC_LONG_PAGE_ON;
        else
            mdc_mailbox.long_page = 0;
        if (job_sheet_count == 0) {
            if (job_long_page_flags & 0x80) {
                img_load_pisces_preset_3(0);
                img_load_pisces_preset_3(1);
                ((volatile unsigned char *)&mdc_mailbox)[0x21f] = 0;
            } else {
                img_load_pisces_preset_1(0);
                img_load_pisces_preset_1(1);
            }
        }
        if (job_long_page_flags & 0x80) {
            if (img_side_params[0].y_res_dpi == 300)
                line_long_page_sample_lines = 0xec;
            else
                line_long_page_sample_lines = 0x1d8;
            line_long_page_state = 0x80;
        } else
            line_long_page_state = 0;
    }
    if (hw_hostif_flags.f.own_hostif && testmode_number.v == 0) {
        if (job_buffer_mode_on || (settings_byte_0x48[0] & 0x10)) {
            job_buffer_feed_flags = 0x8000;
            if (scsi_object_position_done)
                job_buffer_feed_flags = 0;
        } else
            job_buffer_feed_flags = 0;
        emulation = settings_emulation[0] & 0x1c;
        if (emulation == 0x10 || emulation == 0xc || emulation == 0x12 || emulation == 0x13 || emulation == 0x1c || emulation == 0x18)
            job_buffer_feed_flags = 0;
        if (job_long_page_flags & 0x80)
            job_buffer_feed_flags = 0;
    } else
        job_buffer_feed_flags = 0;
    if (page_records[page_scanning.v].flags.f.load_pending)
        page_records[page_scanning.v].flags.f.adf = 1;
    if (page_records[page_scanning.v].sides[0].flags.f.requested)
        page_records[page_scanning.v].flags.f.scan_requested = 1;
    page_side_state[0].last_buf = 0;
    _builtin_set_imask(0);
    if (!job_run())
        return;
    _builtin_set_imask(15);
    saved_flags = page_records[page_scanning.v].flags.v;
    saved_front_flags = page_records[page_scanning.v].sides[0].flags.v;
    saved_back_flags = page_records[page_scanning.v].sides[1].flags.v;
    saved_multifeed_flags = page_records[page_scanning.v].multifeed_flags.v;
    jpeg_init_scan();
    if ((maint_mode_word.v & 0x5000) && (maint_mode_word.c.mode < 2 || maint_mode_word.c.mode > 4) && !win_params.flags.f.adf)
        hw_hostif_flags.f.own_hostif = 0;
    page_records[page_scanning.v].flags.v = saved_flags;
    page_records[page_scanning.v].sides[0].flags.v = saved_front_flags;
    page_records[page_scanning.v].sides[1].flags.v = saved_back_flags;
    page_records[page_scanning.v].multifeed_flags.v = saved_multifeed_flags;
    main_page_loop_phase = 2;
    _builtin_set_imask(0);
    if (!job_run())
        return;
    _builtin_set_imask(15);
    tick_page_wait_ticks.v = 0;
    calib_levels_deferred = 0;
    _builtin_set_imask(0);
    main_page_loop_phase = 0;
    for (;;) {
        _builtin_set_imask(0);
        if (page_side_state[0].last_buf == 0) {
            if (page_records[page_scanning.v].flags.f.scan_end)
                return;
            if (job_aborted) {
                _builtin_set_imask(0);
                return;
            }
            if (maint_mode_word.v || testmode_number.v) {
                if (testmode_number.v == 0x20 || testmode_number.v == 0x21 || testmode_number.v == 0x22
                    || ((maint_mode_word.v & 0x5000) && maint_mode_word.c.mode >= 2 && maint_mode_word.c.mode <= 4)) {
                    _builtin_set_imask(15);
                    if (line_page_state[0].state_flags.v != 0 || line_page_state[1].state_flags.v) {
                        if (p_vdcc_capture[0]->capture_ctrl.capture_run == 0 && p_vdcc_capture[1]->capture_ctrl.capture_run == 0) {
                            _builtin_set_imask(0);
                        } else {
                            _builtin_set_imask(0);
                            continue;
                        }
                    }
                }
                _builtin_set_imask(15);
                page_records[page_scanning.v].sides[0].flags.f.requested = 0;
                page_records[page_scanning.v].sides[1].flags.f.requested = 0;
                _builtin_set_imask(0);
            }
            if (!hw_hostif_flags.f.own_hostif)
                return;
            if (!main_run_page_loop_step_5())
                continue;
            _builtin_set_imask(15);
            if (line_page_state[0].state_flags.v != 0 || line_page_state[1].state_flags.v) {
                if (p_vdcc_capture[0]->capture_ctrl.capture_run == 0 && p_vdcc_capture[1]->capture_ctrl.capture_run == 0) {
                    _builtin_set_imask(0);
                } else {
                    _builtin_set_imask(0);
                    continue;
                }
            }
            if (page_records[page_scanning.v].sides[0].flags.f.requested || page_records[page_scanning.v].sides[1].flags.f.requested)
                continue;
            _builtin_set_imask(15);
            line_side_state[0].state_flags.v = line_side_state[1].state_flags.v = 0;
            _builtin_set_imask(0);
            return;
        }
        if (page_records[page_scanning.v].flags.f.scan_end) {
            main_run_page_loop_step_6();
            return;
        }
        if (!main_run_page_loop_step_5())
            continue;
        _builtin_set_imask(15);
        if (p_vdcc_capture[0]->capture_ctrl.capture_run == 0 && p_vdcc_capture[1]->capture_ctrl.capture_run == 0) {
        } else {
            _builtin_set_imask(0);
            continue;
        }
        _builtin_set_imask(0);
        if (page_side_records[page_side_state[0].capture_buf].flags.f.scan_end) {
            main_run_page_loop_step_6();
            return;
        }
        if (job_aborted)
            return;
        if (job_buffer_option & 0x100) {
            if (page_side_state[0].captured_count == page_scanned_count.v && page_scanned_count.v == page_sent_count.v) {
                job_buffer_option &= 0xfeff;
                return;
            }
            continue;
        } else {
            if (1) ;
            if (job_mode_params->page_count_limit != 0xffff) {
                if (page_fed_count.v >= job_mode_params->page_count_limit) {
                    if (page_side_state[0].captured_count == page_scanned_count.v && page_scanned_count.v == page_sent_count.v) {
                        mdc_request_stop();
                        _builtin_set_imask(15);
                        line_side_state[0].state_flags.v = line_side_state[1].state_flags.v = 0;
                        _builtin_set_imask(0);
                        return;
                    }
                    continue;
                } else {
                }
            }
            if (page_next_index(0))
                continue;
            _builtin_set_imask(15);
            page_side_records[page_side_state[0].capture_buf].flags.f.adf = 1;
            page_side_records[page_side_state[0].capture_buf].flags.f.scan_requested = 1;
            _builtin_set_imask(0);
            if (!job_run()) {
                main_run_page_loop_step_6();
                return;
            }
            _builtin_set_imask(15);
            tick_page_wait_ticks.v = 0;
            calib_levels_deferred = 0;
            _builtin_set_imask(0);
        }
    }
}

int main_run_page_loop_step_5(void)
{
    if (settings_test_flags[0] & 4)
        return 1;
    if ((job_long_page_flags & 0x80) && (job_long_page_flags & 2))
        return 1;
    if (!win_params.flags.f.adf)
        return 1;
    if (job_auto_size_flags & 0xc000) {
        if (line_side_state[0].state_flags.v & 0x20)
            return 1;
    } else {
        if (line_side_state[0].state_flags.v & 0x100)
            return 1;
    }
    if (tick_page_wait_ticks.v > 0xdac) {
        _builtin_set_imask(15);
        calib_levels_deferred = 1;
        _builtin_set_imask(0);
        return 1;
    }
    return 0;
}

void main_run_page_loop_step_6(void)
{
    if (maint_mode_word.v & 0x5000) {
        _builtin_set_imask(15);
        line_page_state[1].state_flags.v = 0;
        line_page_state[1].end_line = 0xffff;
        line_side_state[1].state_flags.v = 0;
        _builtin_set_imask(0);
        return;
    }
    while (page_side_state[0].events & 8) {
        if (page_records[page_scanning.v].flags.f.scan_end || job_aborted) {
            _builtin_set_imask(15);
            line_page_state[1].state_flags.v = 0;
            line_page_state[1].end_line = 0xffff;
            line_side_state[1].state_flags.v = 0;
            _builtin_set_imask(0);
            break;
        }
    }
    if (hw_hostif_flags.f.own_hostif) {
        while (1) {
            if (page_scanning.v == page_current.v && page_records[page_current.v].error_flags.v == 0)
                break;
            if (job_aborted)
                return;
        }
    }
}

void main_task_maint_mode(void)
{
    if (!(maint_mode_word.v & 0x4000))
        return;
    if (maint_save_eeprom_mode.b7) {
        testmode_save_eeprom_data();
        while (1)
            ;
    }
    if (maint_mode_word.v & 0x8000) {
        maint_run();
        return;
    }
    if (err_display_flags.b.b6 || err_display_flags.b.b5)
        return;
    if (maint_mode_word.c.mode == 0 || (link_unused_flags.b7 && maint_mode_word.c.mode == 11)) {
        panel_set_display_mode(11, PANEL_MODE_MAINT);
        if (maint_mode_word.c.mode == 0) {
            if (!button_state.b.b7 && !button_state.b.b6) {
                maint_mode_word.v |= 0x8000;
                maint_start();
            }
            return;
        }
        if (button_state.b.b7 || button_state.b.b4) {
            maint_mode_word.v |= 0x8000;
            maint_start();
        }
        return;
    }
    if (button_state.b.b4) {
        while (button_state.b.b4)
            ;
        if (maint_mode_word.c.mode == 0)
            ;
        else if (maint_mode_word.c.mode == MAINT_MODE_EMULATION)
            ;
        else {
            maint_mode_word.c.mode++;
            if (maint_mode_word.c.mode >= 9)
                maint_mode_word.c.mode = MAINT_MODE_FEED_SENSOR_TEST;
        }
        panel_set_display_mode(maint_mode_word.c.mode, PANEL_MODE_MAINT);
        return;
    }
    if (!button_state.b.b7)
        return;
    while (button_state.b.b7)
        ;
    maint_mode_word.v |= 0x8000;
    maint_start();
}


void main_update_counters(void)
{
    struct page_record *scanning_page;
    struct page_record *current_page;
    int x;
    unsigned short low_count;
    unsigned short count;
    unsigned char byte;

    scanning_page = (struct page_record *)page_records + page_scanning.v;
    current_page = (struct page_record *)page_records + page_current.v;
    if (power_state.f.saving)
        return;
    if (!(scanning_page->flags.v & PAGE_FLAGS_ACTIVE) && !(current_page->flags.v & PAGE_FLAGS_FED_OR_SCANNED)) {
        if (!main_startup_flags.b2) {
            if (mdc_mailbox.df_adjust_flags & MDC_DF_ADJUST_DF_OK) {
                settings_df_sensor_range[0] = mdc_settings_8d[0];
                settings_df_sensor_level[0] = mdc_settings_8e[0];
                _builtin_set_imask(0);
                main_settings_dirty.b6 = 1;
                main_startup_flags.b2 = 1;
            }
        }
        _builtin_set_imask(15);
        if (settings_adf_count_ones[0] != job_adf_sheet_ones.c.lo || settings_flatbed_count_ones[0] != job_flatbed_sheet_ones.c.lo) {
            settings_adf_count_ones[0] = job_adf_sheet_ones.c.lo;
            settings_flatbed_count_ones[0] = job_flatbed_sheet_ones.c.lo;
        }
        _builtin_set_imask(0);
        if (win_params.flags.f.adf) {
            if (main_adf_sheets_pending.s.v) {
                if ((job_long_page_flags & 0x80) && (jpeg_scan_flags.v & JPEG_SCAN_PAUSED)) {
                } else {
                    _builtin_set_imask(15);
                    main_settings_dirty.b6 = 1;
                    _builtin_set_imask(0);
                }
                _builtin_set_imask(15);
                low_count = settings_adf_count_low[0] + main_adf_sheets_pending.s.v;
                main_cleaning_sheets += main_adf_sheets_pending.s.v;
                main_adf_sheets_pending.s.v = 0;
                while (main_cleaning_sheets >= 5) {
                    count = ((unsigned short *)settings_cleaning_count)[0] << 8 | ((unsigned short *)settings_cleaning_count)[0] >> 8;
                    count &= 0x7fff;
                    if (count < 0x7fff)
                        count++;
                    *(unsigned char *)&((unsigned short *)settings_cleaning_count)[0] = count;
                    byte = settings_cleaning_count[1] & 0x80;
                    byte |= count >> 8;
                    settings_cleaning_count[1] = byte;
                    main_cleaning_sheets -= 5;
                }
                while (low_count >= 50) {
                    count = settings_adf_count.w[0] << 8 | settings_adf_count.w[0] >> 8;
                    count++;
                    *(unsigned char *)&settings_adf_count.w[0] = count;
                    settings_adf_count.c[1] = count >> 8;
                    count = settings_pick_roller_count.w[0] << 8 | settings_pick_roller_count.w[0] >> 8;
                    count++;
                    *(unsigned char *)&settings_pick_roller_count.w[0] = count;
                    settings_pick_roller_count.c[1] = count >> 8;
                    count = settings_brake_roller_count.w[0] << 8 | settings_brake_roller_count.w[0] >> 8;
                    count++;
                    *(unsigned char *)&settings_brake_roller_count.w[0] = count;
                    settings_brake_roller_count.c[1] = count >> 8;
                    low_count -= 50;
                }
                settings_adf_count_low[0] = low_count;
                _builtin_set_imask(0);
            }
        } else {
            if (main_flatbed_sheets_pending.v) {
                _builtin_set_imask(15);
                low_count = settings_flatbed_count_low[0] + main_flatbed_sheets_pending.v;
                main_flatbed_sheets_pending.v = 0;
                if (low_count >= 50) {
                    count = settings_flatbed_count.w[0] << 8 | settings_flatbed_count.w[0] >> 8;
                    count++;
                    settings_flatbed_count.w[0] = count << 8 | count >> 8;
                    low_count -= 50;
                }
                settings_flatbed_count_low[0] = low_count;
                _builtin_set_imask(0);
                _builtin_set_imask(15);
                main_settings_dirty.b6 = 1;
                _builtin_set_imask(0);
            }
        }
        if ((unsigned int)tick_idle_seconds.v > 30 && tick_page_idle_seconds.v > 30) if (main_settings_dirty.b6) {
            _builtin_set_imask(15);
            main_settings_dirty.b6 = 0;
            _builtin_set_imask(0);
            iic_eeprom_save_settings();
        }
    }
    if (power_state.v)
        return;
    if (tick_idle_seconds_capped.v >= 30) {
        _builtin_set_imask(15);
        tick_idle_seconds_capped.v = 0;
        panel_update_pending = 1;
        _builtin_set_imask(0);
    }
    if ((unsigned int)tick_idle_seconds.v < 60)
        return;
    if (tick_page_idle_seconds.v < 60)
        return;
    if ((mdc_mailbox.df_adjust_flags & (MDC_DF_ADJUST_ULTRASONIC_OK | MDC_DF_ADJUST_DF_OK)) != (MDC_DF_ADJUST_ULTRASONIC_OK | MDC_DF_ADJUST_DF_OK))
        return;
    if ((unsigned int)tick_idle_seconds.v > 60) {
        if (img_side_params[0].ccd_300dpi_mode) {
            img_side_params[0].ccd_300dpi_mode = 0;
            p_board_ctrl->reg_0e.ccd_300dpi_mode = 0;
        }
    }
    if (settings_byte_0x7f[0] & 1)
        return;
    _builtin_set_imask(15);
    byte = win_params.flags.v;
    _builtin_set_imask(0);
    win_params.flags.f.adf = 1;
    afe_select_source();
    _builtin_set_imask(15);
    win_params.flags.v = byte;
    _builtin_set_imask(0);
}

void main_task_select_adf_source(void)
{
    unsigned char saved_win_flags;

    if ((mdc_mailbox.df_adjust_flags & (MDC_DF_ADJUST_ULTRASONIC_OK | MDC_DF_ADJUST_DF_OK)) != (MDC_DF_ADJUST_ULTRASONIC_OK | MDC_DF_ADJUST_DF_OK)) {
        if ((scsi_update_request & 0x80) || (flash_download_session & 0x80))
            return;
        saved_win_flags = win_params.flags.v;
        win_params.flags.f.adf = 1;
        afe_select_source();
        win_params.flags.v = saved_win_flags;
    }
}

void main_task_testmode(void)
{
    if ((testmode_number.v != 0 && scsi_resume_flags.f.resume_pending) || testmode_number.v == 48) {
        main_testmode_started = 1;
        maint_start();
        _builtin_set_imask(15);
        power_off_idle_seconds = 0;
        power_off_flags.v |= 32;
        _builtin_set_imask(0);
    }
}
