#include "fw.h"
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);


struct port_f_byte { struct Bits f; };


/* Constants (section C). */
const struct cell_tick_heater_ok_init tick_heater_ok_init = {{0, 0}};
/* Uninitialised data (section B). */
int tick_saved_imask;
static unsigned char tick_gap[4];

void tick_handler(void)
{
    struct page_record *scanning_page;
    struct page_record *current_page;
    int single_buf;

    scanning_page = &page_records[page_scanning.v];
    current_page = &page_records[page_current.v];
    single_buf = 0;
    if (page_side_state[0].last_buf) {
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    } else {
        single_buf = 1;
    }
    tick_saved_imask = _builtin_get_imask();
    tick_count_mdc_sensor_clear();
    tick_run_warmup_schedule();
    if (tick_heater_monitor != 0) {
        tick_run_heater_monitor();
    }
    if (tick_count.v & 1) {
        power_check_button();
        tick_hook_odd();
        tick_handler_step_6();
        power_check_pf1_sequence();
    } else {
        if ((current_page->flags.v & PAGE_FLAGS_FED_OR_SCANNED) != 0) {
            tick_page_idle_seconds.v = 0;
        }
        if (single_buf == 1 && scanning_page->flags.f.load_pending && tick_hopper_was_empty == 1) {
        } else if ((scanning_page->flags.v & PAGE_FLAGS_ACTIVE) || (flash_download_session & 0x80)) {
            tick_idle_seconds.v = 0;
            tick_idle_seconds_capped.v = 0;
            power_off_idle_seconds = 0;
            power_off_flags.f.idle_restarted = 1;
        }
        if (!(tick_count.v & 3)) {
            tick_panel_polls.v++;
            panel_current.page_activity = scanning_page->flags.v & PAGE_FLAGS_ACTIVE;
            if ((flash_download_session & 0x80) == 0) {
                button_poll();
                button_handle_in_scan();
            }
            panel_run();
            tick_hook_4th();
        }
        if (panel_update_pending) {
            panel_update_expander();
            panel_update_pending = 0;
        }
    }
    tick_wait_count++;
    tick_count.v++;
    tick_hostif_wait_ticks.v++;
    tick_capture_wait_ticks.v++;
    tick_motor_test_ticks.v++;
    if (tick_mdc_start_ticks <= 0xfff0) {
        tick_mdc_start_ticks++;
    }
    tick_page_wait_ticks.v++;
    tick_error_ticks.v++;
    tick_readtime_ticks.v++;
    tick_line_event_ticks.v++;
    if (tick_count.v > tick_count_last_second.v) {
        if ((unsigned short)(tick_count.v - tick_count_last_second.v) >= 500) {  /* 500 ticks = one second */
            mdc_check_fan_alarm();
            tick_count_last_second.v = tick_count.v;
            tick_seconds.v++;
            tick_idle_seconds.v++;
            tick_idle_seconds_capped.v++;
            tick_page_idle_seconds.v++;
            power_pf1_sequence_seconds++;
            log_seconds++;
            job_elapsed_seconds++;
            if (power_off_flags.f.enabled == 1) {
                power_off_idle_seconds++;
            }
            if ((job_manual_feed_flags.w & 0x8000)) {
                _builtin_set_imask(15);
                if (tick_manual_feed_seconds.v > 1999) {
                } else {
                    tick_manual_feed_seconds.v++;
                    tick_idle_seconds.v = 0;
                    power_off_idle_seconds = 0;
                    power_off_flags.f.idle_restarted = 1;
                }
                _builtin_set_imask(tick_saved_imask);
            }
            if (job_button_pause_flags & 1) {
                _builtin_set_imask(15);
                tick_idle_seconds.v = 0;
                power_off_idle_seconds = 0;
                power_off_flags.f.idle_restarted = 1;
                _builtin_set_imask(tick_saved_imask);
            }
            log_clock_tick();
            if (maint_mode_word.v & 0x5000) {
                power_off_idle_seconds = 0;
            }
            if (panel_current.display_main == PANEL_MODE_ERROR) {
                power_off_idle_seconds = 0;
                power_off_flags.f.idle_restarted = 1;
            }
        }
    } else {
        if ((unsigned short)(~tick_count_last_second.v + tick_count.v + 1) >= 500) {
            tick_count_last_second.v = tick_count.v;
            tick_seconds.v++;
            tick_idle_seconds.v++;
            tick_idle_seconds_capped.v++;
            tick_page_idle_seconds.v++;
            power_pf1_sequence_seconds++;
            if (power_off_flags.f.enabled == 1) {
                power_off_idle_seconds++;
            }
        }
    }
    if (tick_idle_seconds_capped.v >= 30) {
        tick_idle_seconds_capped.v = 30;
    }
    if ((maint_mode_word.v & 0xc000) && maint_sensor_test) {
        if ((unsigned int)tick_idle_seconds.v > 40) {
            tick_idle_seconds.v = 40;
        }
        if (tick_idle_seconds_capped.v > 20) {
            tick_idle_seconds_capped.v = 20;
        }
    }
}

void tick_count_mdc_sensor_clear(void)
{
    if (button_wake_press != 0 && (button_state.c & BUTTON_STATE_ANY) == 0) {
        _builtin_set_imask(15);
        button_wake_press = 0;
        _builtin_set_imask(tick_saved_imask);
    }
    if (power_state.f.saving) {
        if (tick_adf_closed_ticks == 0) {
            tick_adf_closed_ticks = 1;
        }
    } else {
        if (mdc_mailbox.sensor_status.f.adf_cover_open) {
            tick_adf_closed_ticks = 0;
            return;
        } else {
            if (!mdc_mailbox.sensor_status.f.adf_cover_open && !(tick_adf_closed_ticks + 1 > 1000)) {
                tick_adf_closed_ticks++;
                if (tick_adf_closed_ticks < 250) {
                    return;
                }
            }
        }
    }
    if (button_state.c & BUTTON_STATE_ANY) {
        _builtin_set_imask(15);
        power_off_idle_seconds = 0;
        power_off_flags.f.idle_restarted = 1;
        _builtin_set_imask(tick_saved_imask);
        if (power_state.v && tick_adf_closed_ticks && power_state.f.saving && !power_state.f.leave_request) {
            button_wake_press = 1;
            power_state.f.leave_request = 1;
            power_woken_by_button = 1;
        }
    }
    if (!((struct port_f_byte *)hw_port_f)->f.b1) {
        if (tick_hopper_was_empty == 0) {
            return;
        }
        _builtin_set_imask(15);
        power_off_idle_seconds = 0;
        power_off_flags.f.idle_restarted = 1;
        _builtin_set_imask(tick_saved_imask);
        if (power_state.v) {
            if (tick_adf_closed_ticks && power_state.f.saving && !power_state.f.leave_request) {
                power_state.f.leave_request = 1;
                return;
            }
            return;
        }
        tick_hopper_was_empty = 0;
        if (selftest_startup_done == 0) {
            return;
        }
    } else {
        tick_hopper_was_empty = 1;
        return;
    }
}

/* Every tick: the lamp clocks, held at 0 in power saving and while the MDC reports the lamps off (mdc_mailbox.lamp_off).
   The switch on the warm-up clock calib_warmup_ticks requests an exposure check (calib_exposure_due), an extrapolation (calib_extrapolate_due)
   or a level calibration (calib_levels_due) at fixed warm-up times; from 187 s on a level calibration every 60 s. */
void tick_run_warmup_schedule(void)
{
    int pad;
    unsigned int age_limit_ticks;

    if (power_state.f.saving) {
        _builtin_set_imask(15);
        calib_warmup_ticks = 0;
        calib_levels_due = 0;
        calib_exposure_due = 0;
        calib_extrapolate_due = 0;
        tick_lamp_on_ticks.v = 0;
        tick_awake_ticks.v = 0;
        tick_keep_warm_seen = 0;
        _builtin_set_imask(tick_saved_imask);
        return;
    }
    if (mdc_mailbox.lamp_off != 0) {
        _builtin_set_imask(15);
        calib_warmup_ticks = 0;
        calib_levels_due = 0;
        calib_exposure_due = 0;
        calib_extrapolate_due = 0;
        tick_lamp_on_ticks.v = 0;
        if (tick_lamp_off_ticks < 0xffff) {
            tick_lamp_off_ticks++;
        }
        tick_keep_warm_seen = 0;
        _builtin_set_imask(tick_saved_imask);
    }
    if (tick_keep_warm_seen == 0) {
        if (mdc_mailbox.lamp_status & MDC_LAMP_STATUS_KEEP_WARM) {
            _builtin_set_imask(15);
            tick_keep_warm_seen = 1;
            _builtin_set_imask(tick_saved_imask);
        }
    }
    if (tick_is_flatbed_lamp_on() == 0) {
        _builtin_set_imask(15);
        tick_flatbed_lamp_on_ticks.v = 0;
        job_calib_valid = 0;
        tick_calib_age_ticks = 0;
        _builtin_set_imask(tick_saved_imask);
    } else {
        _builtin_set_imask(15);
        if (tick_flatbed_lamp_on_ticks.v < 0x2ee0) {
            age_limit_ticks = 0xc350;
        } else if (tick_flatbed_lamp_on_ticks.v < 0x61a8) {
            age_limit_ticks = 0x9c4;
        } else if (tick_flatbed_lamp_on_ticks.v < 0xafc8) {
            age_limit_ticks = 0x1388;
        } else {
            age_limit_ticks = 0x2710;
            tick_flatbed_lamp_on_ticks.v = 0xc350;
        }
        if (tick_calib_age_ticks > age_limit_ticks) {
            if (!(job_calib_valid & 0x800)) {
                tick_calib_age_ticks = 0;
            }
            job_calib_valid |= 0x800;
        }
        tick_calib_age_ticks++;
        _builtin_set_imask(tick_saved_imask);
        _builtin_set_imask(15);
        if (mdc_mailbox.sensor_status.f.fb_home) {
            tick_flatbed_home_ticks++;
        } else {
            tick_flatbed_home_ticks = 0;
        }
        _builtin_set_imask(tick_saved_imask);
    }
    _builtin_set_imask(15);
    tick_flatbed_lamp_on_ticks.v++;
    tick_lamp_on_ticks.v++;
    tick_awake_ticks.v++;
    tick_sheet_gap_ticks++;
    tick_relight_ticks++;
    _builtin_set_imask(tick_saved_imask);
    if (settings_calib_options[0] & 0x40) {
        return;
    }
    _builtin_set_imask(15);
    /* 2 ms ticks: 0xcb2 = 6.5 s, 0x82dc = 67 s, 0xf80c = 127 s, 0x16d3c = 187 s */
    switch (calib_warmup_ticks) {
    case 0xcb2:
    case 0x1964:
    case 0x30d4:
    case 0xbd74:
        calib_exposure_due = 1;
        break;
    case 0x82dc:
    case 0xf80c:
        calib_levels_due = 1;
        break;
    case 0x1194:
    case 0x2328:
    case 0x3a98:
    case 0x445c:
        if (calib_warmup_idle) {
        } else {
            calib_extrapolate_due = 1;
        }
        break;
    case 0x5014:
        if (calib_warmup_idle) {
        } else {
            if (calib_lamp_cold) {
                calib_exposure_due = 1;
            } else {
                calib_extrapolate_due = 1;
            }
        }
        break;
    case 0x6978:
        if (calib_lamp_cold) {
            calib_exposure_due = 1;
        }
        break;
    case 0x157c:
    case 0x1e46:
    case 0x29fe:
        if (calib_lamp_relit != 0) {
            calib_extrapolate_due = 1;
        }
        break;
    case 0x1195:
        if (calib_warmup_idle != 0) {
            calib_levels_due = 1;
        }
        break;
    case 0x2134:
    case 0x2904:
    case 0x3e80:
    case 0x4e20:
    case 0x5dc0:
    case 0x6d60:
        if (calib_warmup_idle != 0) {
            calib_exposure_due = 1;
        }
        break;
    case 0x16d3c:
        calib_levels_due = 1;
        calib_warmup_ticks = 0xf80c;
        break;
    default:
        break;
    }
    calib_warmup_ticks++;
    _builtin_set_imask(tick_saved_imask);
}


/* Switches the ADF lamp heaters off (MDC requests 0x71-0x73) once a side's level stops rising or its lamp is warm. */
void tick_run_heater_monitor(void)
{
    int side;
    unsigned short level;
    int j;
    int block_word;
    unsigned short previous_mean;
    unsigned short latest_mean;
    unsigned short *history;
    int ok[2];
    int unused_frame;
    int limit;
    int block_rise;
    int level_rise;

    *(struct cell_tick_heater_ok_init *)ok = tick_heater_ok_init;
    if (tick_heater_monitor_ticks % 50 == 0) {
        _builtin_set_imask(15);
        tick_heater_monitor |= 1;
        tick_heater_level_sums[0] = 0;
        tick_heater_level_sum_back[0] = 0;
        tick_heater_sample_count = 0;
        tick_heater_monitor_sums[0].level_sum = 0;
        tick_heater_monitor_sums[0].b = 0;
        tick_heater_monitor_sum_back[0].level_sum = 0;
        tick_heater_monitor_sum_back[0].b = 0;
        _builtin_set_imask(tick_saved_imask);
    }
    if (!(tick_heater_monitor & 1)) {
        tick_heater_monitor_ticks++;
        return;
    }
    if (tick_heater_sample_count < 32) {
        level = p_vdcc_measure[0]->level;
        tick_heater_level_sums[0] += level;
        level = p_vdcc_measure[1]->level;
        tick_heater_level_sum_back[0] += level;
        tick_heater_monitor_sums[0].level_sum += p_vdcc_front_monitor[0]->level_left;
        tick_heater_monitor_sums[0].b += p_vdcc_front_monitor[0]->level_right;
        tick_heater_monitor_sum_back[0].level_sum += p_vdcc_back_monitor[0]->level_left;
        tick_heater_monitor_sum_back[0].b += p_vdcc_back_monitor[0]->level_right;
        tick_heater_sample_count++;
        tick_heater_monitor_ticks++;
        return;
    }
    _builtin_set_imask(15);
    tick_heater_monitor &= 0xfffe;
    _builtin_set_imask(tick_saved_imask);
    for (side = 0; side <= 1; side++) {
        tick_heater_level_sums[side] = tick_heater_level_sums[side] >> 5;
        tick_heater_monitor_sums[side].level_sum = tick_heater_monitor_sums[side].level_sum >> 5;
        tick_heater_monitor_sums[side].b = tick_heater_monitor_sums[side].b >> 5;
    }
    for (j = 0; j < 4; j++) {
        tick_heater_level_history_front[j] = *(tick_heater_level_history_front + j + 1);
        tick_heater_level_history_back[j] = *(tick_heater_level_history_back + j + 1);
    }
    tick_heater_level_history_front[j] = tick_heater_level_sums[0];
    tick_heater_level_history_back[j] = tick_heater_level_sum_back[0];
    for (side = 0; side <= 1; side++) {
        for (j = 0; j < 4; j++) {
            tick_heater_monitor_history[side][0][j] = *(tick_heater_monitor_history[side][0] + j + 1);
            tick_heater_monitor_history[side][1][j] = *(tick_heater_monitor_history[side][1] + j + 1);
        }
        tick_heater_monitor_history[side][0][j] = tick_heater_monitor_sums[side].level_sum;
        tick_heater_monitor_history[side][1][j] = tick_heater_monitor_sums[side].b;
    }
    if (tick_heater_rounds > 4) {
        for (side = 0; side <= 1; side++) {
            if (side == 0 && !(tick_heater_monitor & 0x8000)) {
                continue;
            }
            if (side == 1 && !(tick_heater_monitor & 0x80)) {
                continue;
            }
            if (tick_heater_lamp_temperature[side] > 74) {  /* lamp temperature reading of the MDC: warm enough */
                ok[side] = 1;
                continue;
            }
            history = (side == 0) ? tick_heater_level_history_front : tick_heater_level_history_back;
            latest_mean = previous_mean = 0;
            for (j = 0; j < 4; j++) {
                previous_mean += history[j];
                latest_mean += *(history + j + 1);
            }
            previous_mean >>= 2;
            latest_mean >>= 2;
            if (tick_heater_lamp_temperature[side] < 39 && tick_heater_monitor_ticks < 1000) {
                continue;
            }
            if (tick_heater_lamp_temperature[side] < 31 && tick_heater_monitor_ticks < 2500) {
                continue;
            }
            if (latest_mean - previous_mean <= 2) {
            ok[side] = 1;
            level_rise = (latest_mean - previous_mean) * 10000 / latest_mean;
            tick_heater_monitor_changed[side] = 0;
            for (block_word = 0; block_word < 2; block_word++) {
                latest_mean = previous_mean = 0;
                for (j = 0; j < 4; j++) {
                    previous_mean += tick_heater_monitor_history[side][block_word][j];
                    latest_mean += *(tick_heater_monitor_history[side][block_word] + j + 1);
                }
                previous_mean >>= 2;
                latest_mean >>= 2;
                block_rise = (latest_mean - previous_mean) * 10000 / latest_mean;
                if (block_rise > level_rise) {
                    block_rise = block_rise - level_rise;
                } else {
                    block_rise = 0;
                }
                limit = 50;
                if (block_rise >= limit) {
                    tick_heater_monitor_changed[side] = 1;
                }
            }
            }
        }
    }
    if ((tick_heater_monitor & 0x8000) && (tick_heater_monitor & 0x80) && ok[0] && ok[1]) {
        _builtin_set_imask(15);
        tick_heater_monitor &= 0x7fff;
        tick_heater_monitor &= 0xff7f;
        tick_awake_ticks.v = 0;
        _builtin_set_imask(tick_saved_imask);
        mdc_request_heater_off(3);
    } else if ((tick_heater_monitor & 0x8000) && ok[0] == 1) {
        _builtin_set_imask(15);
        tick_heater_monitor &= 0x7fff;
        tick_awake_ticks.v = 0;
        _builtin_set_imask(tick_saved_imask);
        mdc_request_heater_off(1);
    } else if ((tick_heater_monitor & 0x80) && ok[1] == 1) {
        _builtin_set_imask(15);
        tick_heater_monitor &= 0xff7f;
        tick_awake_ticks.v = 0;
        _builtin_set_imask(tick_saved_imask);
        mdc_request_heater_off(2);
    }
    tick_heater_rounds++;
    if (tick_heater_monitor_ticks > 10000 || mdc_mailbox.lamp_off != 0) {
        _builtin_set_imask(15);
        tick_heater_monitor = 0;
        _builtin_set_imask(tick_saved_imask);
        mdc_request_heater_off(3);
    }
    tick_heater_monitor_ticks++;
}

void tick_check_page_idle(void)
{
    struct page_record *scanning_page;

    scanning_page = &page_records[page_scanning.v];
    if (page_side_state[0].last_buf) {
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    }
    if (scanning_page->flags.v & PAGE_FLAGS_ACTIVE) {
        return;
    }
    if (win_params.flags.f.source_given || mdc_mailbox.sensor_status.f.adf_cover_open) {
        tick_hopper_paper_ticks.v = 0;
        tick_hopper_empty_ticks.v = 0;
    } else {
        if (!mdc_mailbox.sensor_status.f.no_paper) {
            if (tick_hopper_paper_ticks.v + 1 >= 250) {
                tick_hopper_empty_ticks.v = 0;
            } else {
                tick_hopper_paper_ticks.v++;
                if (tick_hopper_paper_ticks.v >= 25) {
                    tick_hopper_empty_ticks.v = 0;
                }
            }
        } else {
            if (tick_hopper_empty_ticks.v + 1 >= 30000) {
                tick_hopper_paper_ticks.v = 0;
            } else {
                tick_hopper_empty_ticks.v++;
                if (tick_hopper_empty_ticks.v >= 25) {
                    tick_hopper_paper_ticks.v = 0;
                }
            }
        }
    }
}

void tick_hook_4th(void)
{
}

void tick_handler_step_6(void)
{
    if (selftest_startup_done == 0) {
        return;
    }
    if (power_state.f.saving) {
        if (tick_pf6_low_reported) {
            if (tick_pf6_count > 2) {
                tick_pf6_low_reported = 0;
            }
        } else {
            if (tick_pf6_count < 3) {
                _builtin_set_imask(15);
                if (hw_hostif_flags.f.own_hostif) {
                    power_image_wake_flags |= 0x100; /* 0xC405C9 bit 0, in the RAM image of the MDC shared RAM */
                } else {
                    if (!p_board_ctrl->reg_02.standby) {
                        mdc_mailbox.wake_flags |= 1;
                    }
                }
                _builtin_set_imask(tick_saved_imask);
                tick_pf6_low_reported = 1;
            }
        }
        if (((struct port_f_byte *)hw_port_f)->f.b6) {
            if (tick_pf6_count < 5) {
                tick_pf6_count++;
            }
        } else {
            if (tick_pf6_count > 0) {
                tick_pf6_count--;
            }
        }
        return;
    } else {
        if (!mdc_mailbox.sensor_status.f.document_cover_open) {
            if (++tick_document_cover_closed_ticks == 15 && tick_document_cover_open_ticks.v >= 125) {
                tick_document_cover_event = 1;
                tick_document_cover_open_ticks.v = 0;
            }
            if (tick_document_cover_closed_ticks >= 765) {
                tick_document_cover_event = 0;
                return;
            }
        } else {
            tick_document_cover_event = 0;
            tick_document_cover_closed_ticks = 0;
            if (++tick_document_cover_open_ticks.v >= 130) {
                tick_document_cover_open_ticks.v = 130;
            }
        }
    }
}

void tick_hook_odd(void)
{
}

int tick_is_flatbed_lamp_on(void)
{
    if (mdc_mailbox.lamp_status & MDC_LAMP_STATUS_FLATBED) {
        return 1;
    } else {
        return 0;
    }
}

int tick_is_adf_front_lamp_on(void)
{
    if (mdc_mailbox.lamp_status & MDC_LAMP_STATUS_ADF_FRONT) {
        return 1;
    } else {
        return 0;
    }
}

int tick_is_adf_back_lamp_on(void)
{
    if (mdc_mailbox.lamp_status & MDC_LAMP_STATUS_ADF_BACK) {
        return 1;
    } else {
        return 0;
    }
}

int tick_both_adf_lamps_on(void)
{
    if ((mdc_mailbox.lamp_status & MDC_LAMP_STATUS_ADF_FRONT) && (mdc_mailbox.lamp_status & MDC_LAMP_STATUS_ADF_BACK)) {
        return 1;
    }
    return 0;
}

int tick_both_adf_lamps_off(void)
{
    if (!(mdc_mailbox.lamp_status & MDC_LAMP_STATUS_ADF_FRONT)) {
        if (!(mdc_mailbox.lamp_status & MDC_LAMP_STATUS_ADF_BACK)) {
            return 1;
        }
    }
    return 0;
}
