#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);

int job_abort(void)
{
    struct page_record *scanning_page;
    int u1;
    int u2;
    scanning_page = &page_records[page_scanning.v];
    _builtin_set_imask(15);
    tick_idle_seconds_capped.v = 0;
    panel_update_pending = 1;
    _builtin_set_imask(0);
    if ((page_side_state[0].events & 8)) {
    } else {
        err_check_pending();
    }
    if (mdc_cmd_posted.c.last_result == MDC_RESULT_TIMEOUT)
        err_raise(ERR_MDC_TIMEOUT);
    _builtin_set_imask(15);
    if (page_side_state[0].last_buf == 0 || main_page_loop_phase != 0)
        page_flush_image_size();
    mdc_mailbox.page_end = 0;
    _builtin_set_imask(0);
    if (job_aborted != 0)
        return 0;
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.own_hostif) {
        scsi_read_sides[0].flags.w = 0;
        scsi_read_sides[1].flags.w = 0;
    }
    err_display_flags.b.b4 = 0;
    line_side_state[0].state_flags.v = 0;
    line_page_state[0].state_flags.v = 0;
    line_page_state[0].end_line = 0xffff;
    if (!hw_hostif_flags.f.own_hostif || page_side_state[0].last_buf == 0 || main_page_loop_phase != 0) {
        line_side_state[1].state_flags.v = 0;
        line_page_state[1].state_flags.v = 0;
        line_page_state[1].end_line = 0xffff;
    }
    _builtin_set_imask(0);
    if (jpeg_scan_flags.v & JPEG_SCAN_PAUSED)
        mdc_request_capture(1);
    _builtin_set_imask(15);
    if ((scanning_page->flags.v & PAGE_FLAGS_REPLY_PENDING) && !hw_hostif_flags.f.own_hostif && testmode_number.v != 42) {
        scanning_page->flags.v &= ~PAGE_FLAGS_REPLY_PENDING;
        link_reply.c.code = 1;
        link_reply.f.b7 = 1;
        if (hw_hostif_flags.f.own_hostif)
            scsi_resume_flags.f.resume_pending = 1;
        isr_request_command_layer(2);
    }
    _builtin_set_imask(0);
    return 0;
}

void job_set_driver_function_confirm(unsigned char value, int record)
{
    if (value & 1) {
        job_driver_function_confirm = 1;
    } else {
        job_driver_function_confirm = 0;
    }
    job_mode_params[record].driver_function_confirm.c = value;
}

void job_set_prepick(unsigned char value, int record)
{
    if ((value & 0x80) == 0) {
        job_prepick_on = job_prepick_default;
    } else {
        if (value & 0x40) {
            job_prepick_on = 1;
        } else {
            job_prepick_on = 0;
        }
    }
    job_mode_params[record].prepick = value;
}

void job_set_sleep_timer(unsigned char value, int record)
{
    if (settings_sleep_timer_default[0] == 0xfd) {
        job_mode_params[record].sleep_timer = value;
        tick_idle_seconds.v = 0;
        power_save_seconds.v = -1;
        return;
    } else if (settings_sleep_timer_default[0] == 0xfc) {
        job_mode_params[record].sleep_timer = value;
        tick_idle_seconds.v = 0;
        power_save_seconds.v = 30;
        return;
    }
    if (value == 0) {
        power_save_seconds.v = 900;
    } else if (settings_emulation[0] & 0x1c) {
        if (value > 60) {
            value = 60;
        }
        power_save_seconds.v = value * 60;
    } else {
        if (value & 0x80) {
            value &= 0x7f;
            if (value > 96) {
                value = 96;
            } else if (value == 0) {
                value = 1;
            }
            power_save_seconds.v = value * 300;
            value |= 0x80;
        } else {
            if (value > 60) {
                value = 60;
            }
            power_save_seconds.v = value * 60;
        }
    }
    if (power_save_seconds.v < 0x564) {
        power_save_seconds.v -= 20;
    } else {
        power_save_seconds.v *= 985;
        power_save_seconds.v /= 1000;
    }
    job_mode_params[record].sleep_timer = value;
    tick_idle_seconds.v = 0;
}

void job_set_interlace_read(unsigned char value, int record)
{
    if (value == 1) {
        job_interlace_read = 1;
    } else {
        job_interlace_read = 0;
    }
    job_mode_params[record].interlace_read = value;
}

void job_set_backing(unsigned char value, int record)
{
    int flag = 1;
    if ((value & 0x80) && (value & 0x20)) {
        if (job_black_backing == 0) {
            flag = 0;
        }
        job_black_backing = 1;
    } else {
        if (job_black_backing) {
            flag = 0;
        }
        job_black_backing = 0;
    }
    job_mode_params[record].backing = value;
    if (value & 4) {
        job_keep_backing = 1;
    } else {
        job_keep_backing = 0;
    }
    if (settings_calib_options[0] & 1) {
        job_black_backing = 1;
    }
    if (flag == 0) {
        job_adf_sheet_count = 0;
    }
}

void job_set_double_feed(unsigned char value, int record)
{
    unsigned char tolerance;
    if (value & 0x80) {
        job_double_feed_flags.b.length_check = 0;
        job_double_feed_flags.b.ultrasonic_check = 0;
        if (value & 8) {
            job_double_feed_flags.b.length_check = 1;
            tolerance = value & 3;
            if (tolerance) {
                job_double_feed_flags.b.length_tolerance = tolerance + 255;
            } else {
                job_double_feed_flags.b.length_tolerance = job_double_feed_default & 3;
            }
        }
        if (value & 0x10) {
            job_double_feed_flags.b.ultrasonic_check = 1;
        } else {
            job_double_feed_flags.b.ultrasonic_check = 0;
        }
        if (value & 0x40) {
            job_double_feed_flags.b.length_report_only = 1;
            job_double_feed_flags.b.ultrasonic_report_only = 1;
        } else {
            job_double_feed_flags.b.length_report_only = 0;
            job_double_feed_flags.b.ultrasonic_report_only = 0;
        }
    } else {
        job_double_feed_flags.c = job_double_feed_default;
    }
    job_mode_params[record].double_feed = value;
}

extern int _builtin_get_imask(void);

void job_set_buffer_mode(unsigned char mode, unsigned char clear, short page_limit, int record)
{
    unsigned char saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    switch (mode & 0xc0) {
    case 0xc0:
        job_buffer_mode_on = 1;
        break;
    case 0x80:
        job_buffer_mode_on = 0;
        break;
    default:
        job_buffer_mode_on = 0;
        break;
    }
    if (settings_byte_0x34[0] & 2) {
        job_buffer_mode_on = 1;
    }
    job_aborted = 1;
    line_side_state[0].state_flags.v = line_side_state[1].state_flags.v = 0;
    line_page_state[0].state_flags.v = line_page_state[1].state_flags.v = 0;
    job_adf_sheet_count = 0;
    job_sheet_count = 0;
    line_page_state[0].end_line = 0xffff;
    line_page_state[1].end_line = 0xffff;
    line_long_page_state = 0;
    job_long_page_flags &= 0x80;
    mdc_mailbox.long_page = 0;
    job_manual_feed_flags.w = 0;
    main_testmode_started = 0;
    job_scan_complete_wait = 0;
    scsi_object_position_done = 0;
    job_button_pause_flags &= 0xfffe;
    switch (clear & 0xc0) {
    case 0xc0:
        util_mem_clear(&page_buf_count, 0x11ac);
        util_mem_clear(page_side_records, 0x198);
        util_mem_clear(page_side_state, 0x30);
        util_mem_clear(&page_side_state[1], 0x30);
        break;
    default:
        util_mem_clear(&page_buf_count, 0x11ac);
        util_mem_clear(page_side_records, 0x198);
        util_mem_clear(page_side_state, 0x30);
        util_mem_clear(&page_side_state[1], 0x30);
        break;
    }
    job_mode_params[record].buffer_clear = clear;
    if (page_limit == 0 || job_buffer_mode_on == 0) {
        job_mode_params[record].page_count_limit = 0xffff;
    } else {
        job_mode_params[record].page_count_limit = page_limit;
    }
    page_fed_count.v = 0;
    _builtin_set_imask(saved_imask);
}


void job_set_deskew_and_auto_width(unsigned char deskew, unsigned char auto_width, int record)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (deskew == 0x80) {
        job_auto_size_flags.b7 = 1;
    } else {
        job_auto_size_flags.b7 = 0;
    }
    if ((auto_width & 0x80) && win_params.paper_size.f.auto_length) {
        job_auto_size_flags.b5 = 0;
    } else {
        job_auto_size_flags.b5 = 1;
    }
    if (job_auto_size_flags.b5 == 0 && (auto_width & 1)) {
        job_auto_size_flags.b2 = 1;
    } else {
        job_auto_size_flags.b2 = 0;
    }
    _builtin_set_imask(saved_imask);
    job_mode_params[record].auto_width = auto_width;
    job_mode_params[record].deskew = deskew;
}

void job_set_auto_length(unsigned char value, int record)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (value & 0x80) {
        win_params.paper_size.f.auto_length = 1;
    } else {
        win_params.paper_size.f.auto_length = 0;
    }
    _builtin_set_imask(saved_imask);
    job_mode_params[record].auto_length = value;
}


void job_set_overscan(unsigned char value, int record)
{
    char backing;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (value == 0xc0) {
        job_auto_size_flags.b6 = 1;
    } else {
        job_auto_size_flags.b6 = 0;
    }
    if (value & 0x20) {
        job_auto_size_flags.b4 = 1;
    } else {
        job_auto_size_flags.b4 = 0;
    }
    if (job_auto_size_flags.b7 && (settings_byte_0x35[0] & 4)) {
        job_auto_size_flags.b4 = 1;
    }
    if (job_auto_size_flags.b4) {
        job_auto_size_flags.b7 = 1;
        job_auto_size_flags.b5 = 0;
        job_auto_size_flags.b6 = 0;
    } else {
        job_auto_size_flags.b3 = 0;
    }
    if (job_auto_size_flags.b7 || job_auto_size_flags.b6) {
        if (job_keep_backing) {
        } else {
            if (job_black_backing == 0) {
                job_adf_sheet_count = 0;
            }
            job_black_backing = 1;
        }
    } else {
        backing = job_mode_params[0].backing;
        job_set_backing(backing, 0);
        job_keep_backing = 0;
    }
    _builtin_set_imask(saved_imask);
    job_mode_params[record].overscan = value;
}


void job_set_dropout_color(unsigned char value, int record)
{
    int u;
    unsigned char code;
    if (!hw_hostif_flags.f.own_hostif && value) {
        if (record == 0) {
            job_dropout[0].v = 1;
            job_dropout[1].v = 1;
            if (value == 2) {
                job_dropout[0].w = 1;
                job_dropout[1].w = 1;
            } else if (value == 1) {
                job_dropout[0].w = 0;
                job_dropout[1].w = 0;
            } else if (value == 3) {
                job_dropout[0].w = 2;
                job_dropout[1].w = 2;
            } else {
                job_dropout[0].w = 1;
                job_dropout[1].w = 1;
            }
        } else {
            job_dropout[2].v = 1;
            if (value == 2) {
                job_dropout[2].w = 1;
            } else if (value == 1) {
                job_dropout[2].w = 0;
            } else if (value == 3) {
                job_dropout[2].w = 2;
            } else {
                job_dropout[2].w = 1;
            }
        }
    } else {
        job_mode_params[record].dropout_color = value;
        if ((value & 8) == 0) {
            job_dropout[0].v = 0;
            job_dropout[1].v = 0;
            job_dropout[0].w = job_dropout_default[0];
            job_dropout[1].w = job_dropout_default[1];
            if (hw_hostif_flags.f.own_hostif == 0) {
                if (job_dropout[0].w == 4 || job_dropout[0].w == 3) {
                    job_dropout[0].w = 1;
                }
                if (job_dropout[1].w == 4 || job_dropout[1].w == 3) {
                    job_dropout[1].w = 1;
                }
            }
        } else {
            job_dropout[0].v = 1;
            job_dropout[1].v = 1;
            code = value;
            code &= 15;
            if (code == 8) {
                job_dropout[0].w = 1;
                job_dropout[1].w = 1;
            } else if (code == 9) {
                job_dropout[0].w = 0;
                job_dropout[1].w = 0;
            } else if (code == 11) {
                job_dropout[0].w = 2;
                job_dropout[1].w = 2;
            } else if (code == 12) {
                job_dropout[0].w = 4;
                job_dropout[1].w = 4;
            } else {
                job_dropout[0].w = 3;
                job_dropout[1].w = 3;
            }
        }
        if ((value & 128) == 0 || !hw_hostif_flags.f.own_hostif) {
            job_dropout[2].v = 0;
            job_dropout[2].w = job_dropout_default[2];
            if (hw_hostif_flags.f.own_hostif == 0) {
                if (job_dropout[2].w == 4 || job_dropout[2].w == 3) {
                    job_dropout[2].w = 1;
                }
            }
        } else {
            job_dropout[2].v = 1;
            code = value;
            code &= 240;
            code = code >> 4;
            if (code == 8) {
                job_dropout[2].w = 1;
            } else if (code == 9) {
                job_dropout[2].w = 0;
            } else if (code == 11) {
                job_dropout[2].w = 2;
            } else if (code == 12) {
                job_dropout[2].w = 4;
            } else {
                job_dropout[2].w = 3;
            }
        }
    }
}

void job_set_dropout_level(unsigned char level, unsigned short index, int record)
{
    unsigned short scaled;
    if (!hw_hostif_flags.f.own_hostif)
        return;
    if (level > 100)
        level = 100;
    scaled = level << 8;
    scaled >>= 1;
    job_mode_params[record].dropout_levels[index] = rt_divu16(scaled, 100);
    if (index == 5) {
    job_dropout[0].c[0] = 0;
    job_dropout[0].c[1] = 0;
    job_dropout[0].c[2] = 0;
    if (job_dropout[0].v) {
        if (job_dropout[0].w == 4) {
            if (job_mode_params[record].dropout_levels[0])
                { job_dropout[0].c[0] = job_mode_params[record].dropout_levels[0]; }
            if (job_mode_params[record].dropout_levels[1] != 0)
                job_dropout[0].c[1] = job_mode_params[record].dropout_levels[1];
            if (job_mode_params[record].dropout_levels[2] != 0)
                job_dropout[0].c[2] = job_mode_params[record].dropout_levels[2];
            if (job_dropout[0].c[0] != 0) {
                if (job_dropout[0].c[1] == 0) if (!job_dropout[0].c[2]) {
                        job_dropout[0].w = 0;
                        job_dropout[1].w = 0;
                    } else {
                    } else {
                    if (job_dropout[0].c[2] == 0) {
                    } else if (job_dropout[0].c[0] == job_dropout[0].c[1] && job_dropout[0].c[0] == job_dropout[0].c[2]) {
                            job_dropout[0].w = 3;
                            job_dropout[1].w = 3;
                        } else {
                            job_dropout[0].w = 4;
                            job_dropout[1].w = 4;
                        }
                }
            } else {
                if (job_dropout[0].c[1] == 0) {
                    if (job_dropout[0].c[2] == 0) {
                        job_dropout[0].w = 1;
                        job_dropout[1].w = 1;
                    } else {
                        job_dropout[0].w = 2;
                        job_dropout[1].w = 2;
                    }
                } else {
                    if (!job_dropout[0].c[2]) {
                        job_dropout[0].w = 1;
                        job_dropout[1].w = 1;
                    } else {
                    }
                }
            }
        }
    }
    job_dropout[2].c[0] = 0;
    job_dropout[2].c[1] = 0;
    job_dropout[2].c[2] = 0;
    if (job_dropout[2].v) {
        if (job_dropout[2].w == 4) {
            if (job_mode_params[record].dropout_levels[3])
                job_dropout[2].c[0] = job_mode_params[record].dropout_levels[3];
            if (job_mode_params[record].dropout_levels[4] != 0)
                job_dropout[2].c[1] = job_mode_params[record].dropout_levels[4];
            if (job_mode_params[record].dropout_levels[5] != 0)
                job_dropout[2].c[2] = job_mode_params[record].dropout_levels[5];
            if (job_dropout[2].c[0] != 0) if (!job_dropout[2].c[1]) {
                    if (!job_dropout[2].c[2]) {
                        job_dropout[2].w = 0;
                    } else {
                    }
                } else if (job_dropout[2].c[2] == 0) {
                    } else {
                        if (job_dropout[2].c[0] == job_dropout[2].c[1] && job_dropout[2].c[0] == job_dropout[2].c[2]) {
                            job_dropout[2].w = 3;
                        } else {
                            job_dropout[2].w = 4;
                        }
                    } else {
                if (0 == job_dropout[2].c[1]) {
                    if (!(job_dropout[2].c[2] != 0)) {
                        job_dropout[2].w = 1;
                    } else job_dropout[2].w = 2;
                } else {
                    if (!job_dropout[2].c[2]) {
                        job_dropout[2].w = 1;
                    } else {
                    }
                }
            }
        }
    }
    }
}

void job_set_device_controls(unsigned char value, int record)
{
    int u, v;
    job_mode_params[record].device_controls = value;
    if ((value & 128) == 0 || !hw_hostif_flags.f.own_hostif) {
        job_device_controls.b3 = 0;
    } else {
        job_device_controls.b3 = 1;
        if (value & 1) {
            job_device_controls.b0 = 1;
        } else {
            job_device_controls.b0 = 0;
        }
        if (value & 2) {
            job_device_controls.b1 = 1;
        } else {
            job_device_controls.b1 = 0;
        }
    }
}

void job_set_auto_power_off(char on)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    if (on == 0) {
        _builtin_set_imask(15);
        power_off_flags.f.enabled = 0;
        power_off_idle_seconds = 0;
        power_off_flags.f.idle_restarted = 1;
        _builtin_set_imask(saved_imask);
    } else {
        _builtin_set_imask(15);
        power_off_flags.f.enabled = 1;
        _builtin_set_imask(saved_imask);
    }
}

extern unsigned int __divlu();

void job_compute_power_off_limit(void)
{
    unsigned char off_time;
    unsigned char warning;
    int switch_enabled;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    off_time = ((char *)settings_power_off_time)[0];
    warning = ((char *)settings_power_off_warning)[0];
    switch_enabled = hw_power_switch_enabled();
    _builtin_set_imask(15);
    if ((settings_emulation[0] & 0x1c) != 0 || (settings_power_off_enable[0] & 0x80) == 0 || switch_enabled == 0 || hw_board_variant == 0 || (off_time & 0x80) != 0) {
        power_off_limit_seconds.v = -1;
    } else {
        if (off_time == 0) {
            power_off_limit_seconds.v = 0x3840;  /* 14400 s = 4 h, the factory default */
        } else if (off_time == 127) {
            power_off_limit_seconds.v = 180;
        } else if (off_time == 126) {
            power_off_limit_seconds.v = 60;
        } else if (off_time == 125) {
            power_off_limit_seconds.v = 30;
        } else {
            power_off_limit_seconds.v = off_time * 900;  /* SET POWOFF TIME in 15-minute units */
        }
        power_off_limit_seconds.v *= 0x3d9;  /* x 985 / 1000 */
        power_off_limit_seconds.v /= 1000;
    }
    if ((settings_emulation[0] & 0x1c) != 0 || (settings_power_off_enable[0] & 0x80) == 0 || switch_enabled == 0 || hw_board_variant == 0 || (off_time & 0x80) != 0) {
        power_off_warning_seconds.v = -1;
    } else {
        if (warning == 0) {
            power_off_warning_seconds.v = 300;
        } else {
            power_off_warning_seconds.v = warning * 10;
        }
        if (power_off_limit_seconds.v >= power_off_warning_seconds.v) {
            power_off_warning_seconds.v = power_off_limit_seconds.v - power_off_warning_seconds.v;
        } else {
            power_off_warning_seconds.v = 0;
        }
    }
    job_applied_power_off_time = off_time;
    job_applied_power_off_warning = warning;
    job_applied_power_off_enable = switch_enabled;
    _builtin_set_imask(saved_imask);
}

void job_set_power_off_time(char off_time, char warning)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    if (((char *)settings_power_off_time)[0] != off_time || ((char *)settings_power_off_warning)[0] != warning) {
        if (((char *)settings_power_off_time)[0] != off_time) {
            _builtin_set_imask(15);
            power_off_idle_seconds = 0;
            power_off_flags.f.idle_restarted = 1;
            _builtin_set_imask(saved_imask);
        }
        ((char *)settings_power_off_time)[0] = off_time;
        ((char *)settings_power_off_warning)[0] = warning;
        job_compute_power_off_limit();
    }
}

int job_check_date(unsigned char year, unsigned char month, unsigned char day)
{
    unsigned char min_year, min_month, min_day;
    int ok;
    ok = 1;
    min_year = settings_byte_0x53[0];
    min_month = settings_byte_0x54[0];
    min_day = settings_byte_0x55[0];
    if (min_month == 0 || min_day == 0) {
    } else {
        if (year < min_year) {
            ok = 0;
        } else if (year == min_year) {
            if (month < min_month) {
                ok = 0;
            } else if (month == min_month) {
                if (day < min_day) {
                    ok = 0;
                }
            }
        }
    }
    if (ok == 0) {
        panel_set_display_mode(8, 4);
        tick_wait_count = 0;
        while (tick_wait_count < 1500) {
        }
        panel_set_display_mode(0, PANEL_MODE_FUNCTION);
        return 0;
    } else {
        return 1;
    }
}

void job_apply_settings(void)
{
    unsigned char u;
    unsigned char i;
    int switch_enabled;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    switch_enabled = hw_power_switch_enabled();
    if ((*(unsigned char *)&settings_multifeed_check & 3) > 2) {
        job_double_feed_default = (*(unsigned char *)&settings_multifeed_check & 0x78) | 2;
    } else {
        job_double_feed_default = *(unsigned char *)&settings_multifeed_check & 0x7b;
    }
    if ((job_mode_params[0].double_feed & 0x80) == 0) {
        job_set_double_feed(0, 0);
    }
    if (settings_scan_options[0] & 2) {
        job_prepick_default = 0;
    } else {
        job_prepick_default = 1;
    }
    if ((job_mode_params[0].prepick & 0x80) == 0) {
        job_set_prepick(0, 0);
    }
    switch (((settings_scan_options[0] & 0x70) >> 4)) {
    case 1:
        job_dropout_default[0] = 0;
        break;
    case 2:
        job_dropout_default[0] = 1;
        break;
    case 4:
        job_dropout_default[0] = 2;
        break;
    case 7:
        job_dropout_default[0] = 3;
        break;
    default:
        job_dropout_default[0] = 1;
    }
    switch ((settings_dropout_codes[0] & 7)) {
    case 1:
        job_dropout_default[1] = 0;
        break;
    case 2:
        job_dropout_default[1] = 1;
        break;
    case 4:
        job_dropout_default[1] = 2;
        break;
    case 7:
        job_dropout_default[1] = 3;
        break;
    default:
        job_dropout_default[1] = 1;
    }
    switch (((settings_dropout_codes[0] & 0x70) >> 4)) {
    case 1:
        job_dropout_default[2] = 0;
        break;
    case 2:
        job_dropout_default[2] = 1;
        break;
    case 4:
        job_dropout_default[2] = 2;
        break;
    case 7:
        job_dropout_default[2] = 3;
        break;
    default:
        job_dropout_default[2] = 1;
    }
    if ((job_mode_params[0].dropout_color & 0x88) == 0) {
        job_set_dropout_color(0, 0);
        for (i = 0; i < 6; i++) {
            job_set_dropout_level(0, i, 0);
        }
    }
    job_adf_sheet_ones.v = settings_adf_count_ones[0];
    job_flatbed_sheet_ones.v = settings_flatbed_count_ones[0];
    if (((char *)settings_power_off_time)[0] != job_applied_power_off_time || ((char *)settings_power_off_warning)[0] != job_applied_power_off_warning || switch_enabled != job_applied_power_off_enable) {
        if (((char *)settings_power_off_time)[0] != job_applied_power_off_time || switch_enabled != job_applied_power_off_enable) {
            _builtin_set_imask(15);
            power_off_idle_seconds = 0;
            power_off_flags.f.idle_restarted = 1;
            _builtin_set_imask(saved_imask);
        }
        job_compute_power_off_limit();
    }
}
