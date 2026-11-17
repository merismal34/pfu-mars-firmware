#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);

void job_check_warmup_idle(void)
{
    unsigned int idle_limit_ticks;
    if (calib_warmup_ticks > 0x6d60) {
        return;
    }
    if (hw_hostif_flags.f.own_hostif) {
        if (mdc_mailbox.last_page.v != 0x80) {
            return;
        }
    }
    {
        if (calib_warmup_idle) {
            return;
        }
        idle_limit_ticks = 600;
        if (tick_sheet_gap_ticks > idle_limit_ticks) {
            _builtin_set_imask(15);
            job_early_warmup = 0;
            calib_warmup_idle = 1;
            calib_levels_request = 1;
            calib_levels_due = 0;
            calib_exposure_due = 0;
            calib_extrapolate_due = 0;
            calib_extrapolate_request = 0;
            _builtin_set_imask(0);
        }
    }
}

/* Starts the warm-up clock calib_warmup_ticks from the lamp-on time tick_lamp_on_ticks and the MDC's lamp temperature
   mdc_mailbox.lamp_temperature, then runs the lamp exposure check; 0 when the check fails. */
int job_check_lamp(int from_lamp_time)
{
    unsigned char result;
    unsigned char temperature;
    int i;
    _builtin_set_imask(15);
    calib_levels_due = 0;
    if (from_lamp_time != 0) {
        if (!win_params.flags.f.adf && (job_calib_valid & 1))
            return 1;
        if (tick_lamp_on_ticks.v < 0x1d4c) {  /* lamps on for less than 15 s */
            for (i = 0; i <= 5; i++) {
                temperature = mdc_mailbox.lamp_temperature;
                if (temperature != 0)
                    break;
                else
                    rt_delay_24k();
            }
            if (win_params.flags.f.adf && tick_awake_ticks.v < 0x8ca) {
                calib_warmup_ticks = 0x9c4;
            } else {
                if (temperature > 34)
                    calib_warmup_ticks = 0x9c4;
                else
                    calib_warmup_ticks = 0;
            }
        } else if (tick_lamp_on_ticks.v < 0x82dc) {
            calib_warmup_ticks = 0;
        } else if (tick_lamp_on_ticks.v < 0x105b8) {
            calib_warmup_ticks = 0x82dd;
        } else {
            calib_warmup_ticks = 0xf80d;
        }
        if (calib_lamp_relit) {
            for (i = 0; i <= 5; i++) {
                temperature = mdc_mailbox.lamp_temperature;
                if (temperature != 0)
                    break;
                else
                    rt_delay_24k();
            }
            if (temperature < 13)
                calib_lamp_cold = 1;
            else
                calib_lamp_cold = 0;
            calib_warmup_ticks = 0x6d6;
        }
        if (win_params.flags.f.adf && calib_warmup_ticks < 0x82dc && job_black_backing) {
            calib_exposure_history_on = 1;
            if (hw_hostif_flags.f.own_hostif && tick_awake_ticks.v < 0x1d4c)
                job_early_warmup = 1;
            else
                job_early_warmup = 0;
        } else {
            calib_exposure_history_on = 0;
            job_early_warmup = 0;
        }
    } else {
        calib_warmup_ticks = 0x6d6;
        job_early_warmup = 0;
        if (win_params.flags.f.adf && job_black_backing)
            calib_exposure_history_on = 1;
        else
            calib_exposure_history_on = 0;
    }
    calib_in_lamp_check = 1;
    calib_exposure_request = 1;
    calib_extrapolate_request = 0;
    calib_extrapolate_due = 0;
    calib_warmup_idle = 0;
    tick_keep_warm_seen = 0;
    _builtin_set_imask(0);
    result = calib_check_lamp_exposure();
    if (result == 1) {
        _builtin_set_imask(15);
        calib_in_lamp_check = 0;
        calib_exposure_request = 0;
        calib_exposure_due = 0;
        _builtin_set_imask(0);
        return 0;
    }
    if (result == 2) {
        _builtin_set_imask(15);
        calib_levels_due = 1;
    }
    _builtin_set_imask(15);
    calib_in_lamp_check = 0;
    calib_exposure_request = 0;
    calib_exposure_due = 0;
    _builtin_set_imask(0);
    return 1;
}


int job_run_calibration(void)
{
    int backing_moved;
    unsigned char cover_error;
    backing_moved = 0;
    if (job_adf_sheet_count != 0) {
        job_wait_line_count();
    }
    if (calib_levels_request != 0) {
        _builtin_set_imask(15);
        job_adf_sheet_count = 0;
        _builtin_set_imask(0);
    }
    if (job_black_backing == 0) {
        _builtin_set_imask(15);
        calib_exposure_request = 1;
        _builtin_set_imask(0);
    }
    if (calib_levels_request != 0 || calib_exposure_request != 0) {
        _builtin_set_imask(15);
        calib_extrapolate_request = 0;
        _builtin_set_imask(0);
    } else if (calib_extrapolate_request != 0) {
        calib_check_lamp_exposure();
        _builtin_set_imask(15);
        calib_extrapolate_request = 0;
        _builtin_set_imask(0);
        return 1;
    }
    if (job_calibrate_levels() == 0) {
        cover_error = job_get_mdc_error();
        if (cover_error) {
            err_raise(cover_error);
        }
        return job_abort();
    }
    if (job_black_backing != 0 && (job_adf_sheet_count != 0 || job_sheet_count != 0) && job_keep_warm_relit == 0) {
        backing_moved = 1;
        while (mdc_busy.w) {
        }
        if (err_check_pending() == 0) {
            return 0;
        }
        _builtin_set_imask(15);
        mdc_queued_cmd = MDC_CMD_BACKING_BLACK;
        mdc_busy.f.queued = 1;
        _builtin_set_imask(0);
    }
    dirt_run_check();
    calib_load_white_level();
    if (!win_params.flags.f.adf && (settings_byte_0x7f[0] & 2) && (settings_byte_0x7f[0] & 1) && hw_hostif_flags.f.own_hostif && (maint_mode_word.v & 0x5000) == 0) {
        _builtin_set_imask(15);
        job_calib_valid = 0x8000;
        tick_calib_age_ticks = 0;
        _builtin_set_imask(0);
    } else {
        _builtin_set_imask(15);
        job_calib_valid = 0;
        _builtin_set_imask(0);
    }
    if (backing_moved == 1) {
        while (mdc_busy.f.queued) {
        }
        if (mdc_cmd_posted.c.last_result == MDC_RESULT_ERROR) {
            return 0;
        }
        if (job_check_adf_page_start() != 0) {
            return 0;
        }
    }
    return 1;
}


int job_calibrate(void)
{
    int needed;
    needed = 0;
    if (!win_params.flags.f.adf) {
        if ((job_calib_valid & 0x8000) != 0 && tick_flatbed_lamp_on_ticks.v < 15000) {
            _builtin_set_imask(15);
            job_calib_valid = 0;
            _builtin_set_imask(0);
        }
        if ((job_calib_valid & 0x8000) != 0 && (job_calib_valid & 0x800) != 0 && tick_calib_age_ticks > 1000) {
            _builtin_set_imask(15);
            job_calib_valid = 0;
            _builtin_set_imask(0);
        }
        if ((job_calib_valid & 0x8000) != 0 && (mdc_mailbox.lamp_status & MDC_LAMP_STATUS_FLATBED) != 0 && mdc_mailbox.sensor_status.f.fb_home) {
            _builtin_set_imask(15);
            job_calib_valid |= 1;
            _builtin_set_imask(0);
        } else {
            _builtin_set_imask(15);
            calib_levels_request = 1;
            _builtin_set_imask(0);
        }
    } else if (job_adf_sheet_count == 0) {
        _builtin_set_imask(15);
        calib_levels_request = 1;
        _builtin_set_imask(0);
    } else {
        if (hw_hostif_flags.f.own_hostif && page_side_state[0].last_buf == 0) {
            if (calib_levels_due != 0) {
                _builtin_set_imask(15);
                calib_levels_due = 0;
                calib_levels_request = 1;
                _builtin_set_imask(0);
            }
            if (calib_exposure_due != 0) {
                _builtin_set_imask(15);
                calib_exposure_due = 0;
                calib_exposure_request = 1;
                _builtin_set_imask(0);
            }
            if (calib_extrapolate_due != 0) {
                calib_extrapolate_due = 0;
                if (job_black_backing != 0) {
                    calib_extrapolate_request = 1;
                    needed = 1;
                } else {
                    calib_extrapolate_request = 0;
                }
            }
        }
        if (job_black_backing != 0) {
            if (calib_exposure_request != 0) {
                needed = 1;
            }
            if (calib_extrapolate_request != 0) {
                needed = 1;
            }
        } else {
            needed = 1;
        }
    }
    if (settings_byte_0x1e[0] & 4) {
        _builtin_set_imask(15);
        calib_levels_request = 1;
        _builtin_set_imask(0);
    }
    if (calib_levels_request != 0) {
        needed = 1;
    }
    return needed;
}

int job_calibrate_levels()
{
    int r;
    int i, j, exposure_retries;
    unsigned char code;
    unsigned char exposure_result;
    unsigned int awake_limit_ticks;
    unsigned char temperature;
    int level_retries;
    unsigned short restart_limit_ticks;
    r = 0;
    if (!win_params.flags.f.adf) {
        _builtin_set_imask(15);
        calib_levels_deferred = 0;
        _builtin_set_imask(0);
        if (err_check_pending() == 0) {
            return 0;
        }
        if ((settings_byte_0x7f[0] & 2) != 0 && (settings_byte_0x7f[0] & 1) != 0 && hw_hostif_flags.f.own_hostif && (maint_mode_word.v & 0x5000) == 0) {
            if ((mdc_mailbox.lamp_status & MDC_LAMP_STATUS_FLATBED) == 0 && mdc_run_lamps(1) == 0) {
                return 0;
            }
        } else {
            if (mdc_run_lamps(1) == 0) {
                return 0;
            }
        }
    } else {
        if ((mdc_mailbox.lamp_status & MDC_LAMP_STATUS_ADF_BOTH) != MDC_LAMP_STATUS_ADF_BOTH || (settings_test_flags[0] & 4) != 0) {
            if (err_check_pending() == 0) {
                return 0;
            }
            if (mdc_run_lamps(6) == 0) {
                return 0;
            }
        }
    }
    afe_write_config(0);
    afe_set_test_mode(0);
    if (win_params.flags.f.adf) {
        afe_write_config(1);
        afe_set_test_mode(1);
        if (mdc_is_adf_cover_open() != 0) {
            err_raise(ERR_ADF_COVER_OPEN);
            return 0;
        }
    }
    if (job_sheet_count == 0) {
        i = 0;
        while (i <= 5) {
            temperature = mdc_mailbox.lamp_temperature;
            if (temperature != 0) {
                break;
            } else {
                rt_delay_24k();
                i++;
            }
        }
        if ((settings_byte_0x1d[0] & 0x10) != 0 || (settings_calib_options[0] & 2) != 0 || (settings_test_flags[0] & 0x10) != 0 || (settings_test_flags[0] & 2) != 0) {
        } else {
            if (win_params.flags.f.adf) {
                if (tick_heater_monitor_changed[0] || tick_heater_monitor_changed_back[0]) {
                    if (temperature < 6) {
                        awake_limit_ticks = 0x1d4c;
                    } else if (temperature < 0x15) {
                        awake_limit_ticks = 0x1388;
                    } else {
                        awake_limit_ticks = 0x9c4;
                    }
                    tick_heater_monitor_changed[0] = tick_heater_monitor_changed_back[0] = 0;
                } else {
                    awake_limit_ticks = 0xfa;
                }
                while (tick_awake_ticks.v < awake_limit_ticks) {
                    if (job_aborted != 0) {
                        return 0;
                    }
                }
                while (tick_mdc_start_ticks < 0xd16) {
                    if (job_aborted != 0) {
                        return 0;
                    }
                }
                while (tick_lamp_on_ticks.v < 0xd16) {
                    if (job_aborted != 0) {
                        return 0;
                    }
                    if (mdc_mailbox.lamp_off != 0 && mdc_is_adf_cover_open() != 0) {
                        err_raise(ERR_ADF_COVER_OPEN);
                        return 0;
                    }
                }
                if (calib_lamp_relit != 0) {
                    while (tick_relight_ticks < 0x76c) {
                        if (job_aborted != 0) {
                            return 0;
                        }
                    }
                }
                if (temperature > 0x22) {
                    while (tick_lamp_on_ticks.v < 0x16da) {
                        if (job_aborted != 0) {
                            return 0;
                        }
                    }
                }
                if ((job_long_page_flags & 0x80) != 0) {
                    while (tick_mdc_start_ticks < 0x30d4) {
                        if (job_aborted != 0) {
                            return 0;
                        }
                    }
                }
            } else {
                job_calibrate_levels_step_8();
                while (tick_flatbed_lamp_on_ticks.v < 0xdac) {
                    if (job_aborted != 0) {
                        return 0;
                    }
                }

            }
        }
    } else if (job_keep_warm_relit != 0) {
        _builtin_set_imask(15);
        tick_relight_ticks = 0;
        _builtin_set_imask(0);
        while (tick_relight_ticks < 0x5dc) {
        }
    }
    restart_limit_ticks = tick_mdc_start_ticks;
    if (job_calibrated_since_mdc_start == 0) {
        if (restart_limit_ticks < 10000) {
            restart_limit_ticks += 2000;
        }
    }
    job_calibrated_since_mdc_start = 1;
    exposure_retries = 0;
    level_retries = 0;
    calib_flatbed_high_target = 0;
top:
    {
        if (calib_levels_request != 0) {
            if (!win_params.flags.f.adf) {
            } else if (mdc_run_backing(0) == 0) {
                return 0;
            }
            i = 0;
            for (;;) {
                if (i != 0) {
                    afe_select_source();
                    if (err_check_pending() == 0) {
                        return 0;
                    }
                    if (mdc_run_lamps(0) == 0) {
                        return 0;
                    }
                    for (j = 0; j <= 100; j++) {
                        rt_delay_180k();
                        rt_delay_60k();
                    }
                    if (err_check_pending() == 0) {
                        return 0;
                    }
                    if (mdc_run_lamps(7) == 0) {
                        return 0;
                    }
                    for (j = 0; j <= 25; j++) {
                        rt_delay_180k();
                        rt_delay_60k();
                    }
                    if (!win_params.flags.f.adf) {
                        if (err_check_pending() == 0) {
                            return 0;
                        }
                        mdc_run_command(MDC_CMD_FB_MOTOR_HOME);
                    } else {
                        if (mdc_is_adf_cover_open() != 0) {
                            err_raise(ERR_ADF_COVER_OPEN);
                            return 0;
                        }
                    }
                    for (j = 0; j <= 25; j++) {
                        rt_delay_180k();
                        rt_delay_60k();
                    }
                    if (win_params.flags.f.adf) {
                        if (mdc_run_backing(0) == 0) {
                            return 0;
                        }
                        for (j = 0; j <= 25; j++) {
                            rt_delay_180k();
                            rt_delay_60k();
                        }
                        if (mdc_run_backing(1) == 0) {
                            return 0;
                        }
                        for (j = 0; j <= 25; j++) {
                            rt_delay_180k();
                            rt_delay_60k();
                        }
                        if (mdc_run_backing(0) == 0) {
                            return 0;
                        }
                    }
                }
                code = calib_run_sequence();
                i++;
                if (code == 0) {
                    calib_levels_request = 0;
                    goto tail;
                } else if (i >= 3) {
                    _builtin_set_imask(15);
                    calib_levels_request = 0;
                    calib_levels_deferred = 0;
                    _builtin_set_imask(0);
                    for (j = 0; j < 3; j++) {
                        if (hw_port_f->b7 != 0) {
                            break;
                        }
                        rt_delay_60k();
                    }
                    if (j == 3) {
                        code = 0x84;
                    }
                    err_raise(code);
                    return 0;
                }
            }
        }
tail:
        if ((!win_params.flags.f.adf && !job_adf_sheet_count) || job_sheet_count == 0) {
            code = job_calibrate_levels_step_15();
            level_retries++;
            if (code != 0) {
                if (level_retries > 1) {
                    err_raise(code);
                    return 0;
                }
                mdc_run_unit_power(0);
                mdc_run_lamps(0);
                while (tick_mdc_start_ticks < restart_limit_ticks) {
                    _builtin_set_imask(0);
                }
                for (j = 0; j <= 0x96; j++) {
                    rt_delay_180k();
                    rt_delay_60k();
                }
                mdc_run_warmup_on();
                for (j = 0; j <= 0x190; j++) {
                    rt_delay_180k();
                    rt_delay_60k();
                }
                mdc_run_warmup_off();
                mdc_run_unit_power(0);
                for (j = 0; j <= 100; j++) {
                    rt_delay_180k();
                    rt_delay_60k();
                }
                calib_flatbed_high_target = 1;
                _builtin_set_imask(15);
                calib_levels_request = 1;
                _builtin_set_imask(0);
                goto top;
            }
        }
        exposure_result = calib_check_lamp_exposure();
        if (exposure_result == 0) {
            return 1;
        }
        if (exposure_result == 1) {
            return 0;
        }
        if (exposure_result == 2) {
            job_adf_sheet_count = 0;
            exposure_retries++;
            if (exposure_retries > 2) {
                return 1;
            }
            _builtin_set_imask(15);
            calib_levels_request = 1;
            _builtin_set_imask(0);
            goto top;
        }
        return 1;
    }
}

void job_calibrate_levels_step_8()
{
    int i;
    unsigned char temperature;
    for (i = 0; i <= 5; i++) {
        temperature = mdc_mailbox.lamp_temperature;
        if (temperature != 0) {
            break;
        } else {
            rt_delay_24k();
        }
    }
    if (temperature <= 12) {
        while (1) {
            _builtin_set_imask(0);
            if (tick_mdc_start_ticks > 0x1388) {
                mdc_run_end_heater_warmup(23);
                break;
            }
        }
    } else {
        while (1) {
            _builtin_set_imask(0);
            if (tick_mdc_start_ticks > 0x7d0) {
                if (job_calibrated_since_mdc_start == 1) {
                    break;
                } else {
                    mdc_run_end_heater_warmup(23);
                    break;
                }
            }
        }
        while (1) {
            _builtin_set_imask(0);
            if (tick_mdc_start_ticks > 0xbb8) {
                if (job_calibrated_since_mdc_start == 1) {
                    break;
                } else if ((mdc_mailbox.warmup_output & MDC_WARMUP_OUTPUT_ON) == 0) {
                    mdc_run_end_heater_warmup(23);
                    while (1) {
                        _builtin_set_imask(0);
                        if (tick_mdc_start_ticks > 0xdac) {
                            mdc_run_end_heater_warmup(23);
                            break;
                        }
                    }
                    break;
                } else if (tick_mdc_start_ticks > 0xdac) {
                    mdc_run_end_heater_warmup(23);
                    break;
                }
            }
        }
    }
}


int job_setup_sensor(void)
{
    int saved_imask;
    unsigned short front_error;
    unsigned short unused_frame;
    unsigned char back_error;

    if (hw_hostif_flags.f.own_hostif) {
        job_set_deskew_and_auto_width(job_mode_params[0].deskew, job_mode_params[0].auto_width, 0);
        job_set_auto_length(job_mode_params[0].auto_length, 0);
        job_set_overscan(job_mode_params[0].overscan, 0);
    }
    _builtin_set_imask(15);
    if (job_dropout[1].w != job_dropout_previous[1].w) {
        job_adf_sheet_count = 0;
    }
    if (job_dropout[2].w != job_dropout_previous[2].w) {
        job_adf_sheet_count = 0;
    }
    job_dropout_previous[1].w = job_dropout[1].w;
    job_dropout_previous[2].w = job_dropout[2].w;
    if (settings_byte_0x33[0] & 8) {
        hw_hostif_flags.f.cga_test_scan = 0;
        link_global_params_ptr->scan_flags.scan_mode = 3;
        img_link_scan_mode = 3;
        win_params.side[0].composition.f.color = 1;
        win_params.side[1].composition.f.color = 1;
        p_vdcc_capture[0]->capture_ctrl.b4 = 1;
        p_vdcc_capture[1]->capture_ctrl.b4 = 1;
    }
    if (job_sensor_sides.v != (win_sides.v & 3)) {
        job_adf_sheet_count = 0;
    }
    job_sensor_sides.v = win_sides.v & 3;
    _builtin_set_imask(0);
    front_error = img_setup_side(0);
    if (front_error) {
        err_set_illegal_request_detail(front_error, 2, 0);
        return 0;
    }
    if (!(win_sides.f.selection == 0)) {
        back_error = img_setup_side(1);
        if (back_error) {
            err_set_illegal_request_detail(back_error, 2, 0);
            return 0;
        }
    }
    afe_select_source();
    _builtin_set_imask(15);
    if (img_side_params[0].ccd_300dpi_mode) {
        p_board_ctrl->reg_0e.ccd_300dpi_mode = 1;
        p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 1;
    } else {
        p_board_ctrl->reg_0e.ccd_300dpi_mode = 0;
        p_vdcc_front_sensor->source_gate.b.b2 = 0;
        p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
    }
    _builtin_set_imask(0);
    testmode_wait_lines(6);
    if (!win_params.flags.f.adf && job_auto_size_flags.b7) {
        saved_imask = _builtin_get_imask();
        jpeg_sync_buffer_pointer(0, saved_imask);
        jpeg_sync_buffer_pointer(1, saved_imask);
    }
    return 1;
}
