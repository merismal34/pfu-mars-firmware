#include "mdc.h"

#pragma interrupt (isr_tgi5a)
void isr_tgi5a(void)
{
    unsigned char u;

    tick_saved_imask = get_imask_exr();
    if (!p_tpu5->TSR.BIT.TGFA)
        return;
    set_imask_exr(7);
    p_tpu5->TSR.BIT.TGFA = 0;
    *p_wdt = 0x5a00;
    if (feed_wait_ticks) feed_wait_ticks--;
    if (feed_start_ticks) feed_start_ticks--;
    if (pick_wait_ticks) pick_wait_ticks--;
    if (pick_start_ticks) pick_start_ticks--;
    set_imask_exr(tick_saved_imask);
    set_imask_exr(7);
    if (pick_hold_ticks) {
        pick_hold_ticks--;
        if (pick_hold_ticks == 0) motor_set_current(MOTOR_PICK, MOTOR_CURRENT_OFF);
    }
    set_imask_exr(tick_saved_imask);
    if (tick_timeout) tick_timeout--;
    if (tick_period_count) {
        tick_period_count--;
    } else {
        tick_period_count = TICKS_PER_SECOND;
        job_scan_seconds++;
        main_error_seconds++;
        tick_warmup_seconds++;
        if (feed_empty_seconds < 0xfff0) feed_empty_seconds++;
        if (tick_seconds < 3600) tick_seconds++;
        if (lamp_off_count) {
            if (lamp_off_count == lamp_off_limit) {
                if (lamp_keep_warm_flags.w & LAMP_KEEP_WARM_ACTIVE) {
                    p_p2->DR.BYTE = hw_p2_out.b |= HW_P2_ADF_LAMPS;
                }
                lamp_keep_warm_flags.w &= ~(LAMP_KEEP_WARM_STARTED | LAMP_KEEP_WARM_ACTIVE);
                mailbox.lamp_status.bit.keep_warm = 0;
            }
            lamp_off_count--;
            if (lamp_off_count == 0) {
                p_p2->DR.BYTE = hw_p2_out.b &= ~HW_P2_OUTPUTS;
                p_p1->DR.BIT.B6 = 0;
            } else if (!(lamp_keep_warm_flags.w & LAMP_KEEP_WARM_STARTED)
                       && (unsigned short)(lamp_off_limit - lamp_off_count) > lamp_keep_warm_delay
                       && (mailbox.lamp_temperature_front >= lamp_keep_warm_off_front || mailbox.lamp_temperature_back >= lamp_keep_warm_off_back)) {
                lamp_keep_warm_flags.bit.started = 1;
                lamp_keep_warm_period_front = 10;
                lamp_keep_warm_period_back = 10;
                lamp_keep_warm_count_front = 1;
                lamp_keep_warm_count_back = 1;
                if (lamp_keep_warm_period_front && lamp_keep_warm_period_back) {
                    lamp_keep_warm_flags.bit.active = 1;
                    mailbox.lamp_status.bit.keep_warm = 1;
                    lamp_keep_warm_on_front = 60;
                    lamp_keep_warm_on_back = 60;
                    lamp_keep_warm_off_front = 70;
                    lamp_keep_warm_off_back = 70;
                }
            }
        }
        if (!sensor_flags.bit.adf_cover_open && (lamp_keep_warm_flags.w & LAMP_KEEP_WARM_ACTIVE)) tick_keep_lamps_warm();
        set_imask_exr(7);
        if (lamp_adf_off_count) {
            lamp_adf_off_count--;
            if (mailbox.settings_d4 & 2) lamp_adf_off_count = lamp_adf_off_limit;
            if (lamp_unit_front_on || lamp_unit_back_on || lamp_unit_flatbed_on) lamp_adf_off_count = lamp_adf_off_limit;
            if (lamp_adf_off_count == 0) {
                p_p2->DR.BYTE = hw_p2_out.b &= ~HW_P2_ADF_LAMPS;
                p_p1->DR.BIT.B6 = 0;
                if (p_chip_a_motor->control_1.bit.b3 && p_chip_a_motor->current_1[7] == 0)
                    p_chip_a_motor->control_1.bit.b3 = 0;
            }
        }
        lamp_adf_off_count = lamp_adf_off_limit;
        if (p_p2->PORT.BIT.B0 || (lamp_keep_warm_flags.w & LAMP_KEEP_WARM_ACTIVE)) mailbox.lamp_off = 0;
        else mailbox.lamp_off = 0xff;
        set_imask_exr(tick_saved_imask);
    }
    if (job_hold_ticks) job_hold_ticks--;
    if (job_wait_ticks) job_wait_ticks--;
    if (cmd_timeout) cmd_timeout--;
    if (bw_motor_arm_delay) bw_motor_arm_delay--;
    if (job_eject_ticks) job_eject_ticks--;
    if (bw_motor_backing_delay) bw_motor_backing_delay--;
    if (fb_motor_delay_ticks) fb_motor_delay_ticks--;
    if (sensor_mux_count) sensor_mux_count--;
    if (lamp_heater_ticks) lamp_heater_ticks--;
    if (cmd_delay) cmd_delay--;
    if (bw_motor_arm_idle_ticks) {
        bw_motor_arm_idle_ticks--;
        if (bw_motor_arm_idle_ticks == 0) {
            set_imask_exr(7);
            bw_motor_arm_request = BW_MOTOR_ARM_HOME;
            set_imask_exr(tick_saved_imask);
        }
    }
    if (tick_word_1) tick_word_1--;
    tick_poll_covers();
    if (tick_manual_end_ticks < 0xff00) tick_manual_end_ticks++;
    if (mailbox.manual_feed_end) {
        tick_manual_end_ticks = 0;
        tick_manual_end_active = 1;
    } else if (tick_manual_end_ticks > 5000) {
        tick_manual_end_active = 0;
    }
    if (hw_p2_out.b & HW_P2_WARMUP_OUTPUT) {
        if (tick_seconds > 8 && tick_warmup_hold == 0) {
            set_imask_exr(7);
            hw_p2_out.b &= ~HW_P2_WARMUP_OUTPUT;
            p_p2->DR.BYTE = hw_p2_out.b;
            mailbox.warmup_output.bit.on = 0;
            set_imask_exr(tick_saved_imask);
            lamp_heater_flatbed_warmup = 1;
            lamp_heater_front_warmup = 1;
            lamp_heater_back_warmup = 1;
            sensor_mux_enable = 1;
            lamp_heater_flatbed_on = 1;
            lamp_heater_front_on = 1;
            lamp_heater_back_on = 1;
            lamp_heater_control = 1;
            cmd_test_mode = 0;
        }
        if (tick_warmup_hold) {
            if (tick_warmup_seconds > 15 || tick_seconds > 180) tick_warmup_hold = 0;
        }
    }
    tick_read_sensor_mux();
    if (lamp_heater_control) tick_control_heaters();
    if (cmd_test_mode == 0) {
        switch (tick_adf_cover_state) {
        case 0:
            if (sensor_flags.bit.adf_cover_open) {
                pick_state = PICK_STATE_RESET;
                feed_state = FEED_STATE_RESET;
                pick_sheet_index = 0;
                pick_gap_index = 0;
                feed_length_index = 0;
                feed_gap_index = 0;
                job_length_history_reset = 0xff;
                bw_motor_arm_idle_ticks = 0;
                pick_short_gap_stopped = 0;
                main_clear_path_pending = 1;
                p_p2->DR.BYTE = hw_p2_out.b &= ~HW_P2_ADF_LAMPS;
                set_imask_exr(7);
                sensor_stop_ultrasonic();
                feed_ultrasonic_running = 0;
                multifeed_check.in_area = 0;
                set_imask_exr(tick_saved_imask);
                if (bw_motor_backing_request == 0) {
                    bw_motor_backing_quiet = 0;
                    if (bw_motor_backing_state == 0 || bw_motor_backing_state == 5) {
                        set_imask_exr(7);
                        motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_OFF);
                    } else {
                        set_imask_exr(7);
                        bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
                        bw_motor_backing_quiet = 1;
                    }
                } else {
                }
                mailbox.cover_open_event.bit.adf_cover_opened = 1;
                set_imask_exr(tick_saved_imask);
                if (!(mailbox.cmd_code == MDC_CMD_SCAN_MOVE && (mailbox.source & MDC_SOURCE_FLATBED) && mailbox.cmd_state)) {
                    mailbox.capture_flags.b = 0;
                    if (!(err_flags.b & ERR_FLAG_DEVICE) && err_code != ERR_SHIPPING_LOCK) {
                        err_flags.bit.document = 1;
                        err_saved_flags = err_flags.b;
                        err_saved_code = err_code = ERR_ADF_COVER_OPEN;
                    }
                    main_flags_1.bit.adf_cover_open = 1;
                    tick_adf_cover_state = 2;
                }
            }
            break;
        case 2:
            if (!sensor_flags.bit.adf_cover_open) {
                pick_sheet_index = 0;
                pick_gap_index = 0;
                if (!(mailbox.settings_c0 & 8)) feed_length_index = 0;
                feed_gap_index = 0;
                feed_speed_step = 0;
                feed_speed_step_target = 0;
                feed_speed_step_phase = 0;
                if (!(mailbox.cmd_code == MDC_CMD_SCAN_MOVE && (mailbox.source & MDC_SOURCE_FLATBED))) mailbox.capture_flags.b = 0;
                if (main_flags_1.b & MAIN_FLAG_ADF_COVER_OPEN) main_flags_1.bit.adf_cover_open = 0;
                bw_motor_arm_idle_ticks = 0;
                pick_short_gap_stopped = 0;
                main_clear_path_pending = 1;
                if (feed_ultrasonic_running || multifeed_check.in_area) {
                    if (mailbox.ultrasonic_timeout_count < 0xff) mailbox.ultrasonic_timeout_count++;
                }
                feed_ultrasonic_running = 0;
                multifeed_check.in_area = 0;
                tick_adf_cover_state = 0;
            } else {
                pick_state = PICK_STATE_RESET;
                feed_state = FEED_STATE_RESET;
                p_p2->DR.BYTE = hw_p2_out.b &= ~HW_P2_ADF_LAMPS;
                lamp_adf_off_count = lamp_adf_off_limit;
                lamp_off_count = lamp_off_limit;
            }
            break;
        }
    }
    if (!sensor_flags.bit.adf_cover_open && feed_ramp_state == RAMP_STATE_STOPPED) {
        if (tick_hopper_state == 0) {
            if (sensor_flags.bit.no_paper) tick_hopper_state = 2;
        } else if (tick_hopper_state == 2) {
            if (!sensor_flags.bit.no_paper && mailbox.startup_request == MDC_STARTUP_RUN) {
                if (!(hw_p2_out.b & HW_P2_ADF_FRONT_LAMP) || (lamp_keep_warm_flags.w & LAMP_KEEP_WARM_ACTIVE)) lamp_off_count = lamp_off_limit;
                p_p2->DR.BYTE = hw_p2_out.b |= HW_P2_ADF_LAMPS;
                lamp_adf_off_count = lamp_adf_off_limit;
                tick_hopper_state = 0;
                if (job_state == JOB_STATE_IDLE) feed_length_index = 0;
            }
        }
    }
    if (tick_document_cover_state == 0) {
        if (sensor_flags.bit.document_cover_open) {
            if (mailbox.startup_request == MDC_STARTUP_RUN) {
                if (job_state == JOB_STATE_IDLE && feed_ramp_state == RAMP_STATE_STOPPED && pick_ramp_state == RAMP_STATE_STOPPED && !job_capture_flags.bit.pause_active) {
                    if (lamp_off_count == 0 && !p_p1->DR.BIT.B6) lamp_off_count = lamp_off_limit;
                    p_p1->DR.BIT.B6 = 1;
                    lamp_adf_off_count = lamp_adf_off_limit;
                } else {
                    job_wake_flags.bit.wake_event = 1;
                }
            }
            tick_document_cover_state = 2;
        }
    } else if (tick_document_cover_state == 2 && !sensor_flags.bit.document_cover_open && mailbox.startup_request == MDC_STARTUP_RUN) {
        if (job_state == JOB_STATE_IDLE && feed_ramp_state == RAMP_STATE_STOPPED && pick_ramp_state == RAMP_STATE_STOPPED && !job_capture_flags.bit.pause_active) {
            if (lamp_off_count == 0 && !p_p1->DR.BIT.B6) lamp_off_count = lamp_off_limit;
            p_p1->DR.BIT.B6 = 1;
            lamp_adf_off_count = lamp_adf_off_limit;
        }
        tick_document_cover_state = 0;
        job_wake_flags.bit.wake_event = 1;
    }
    if (job_wake_flags.bit.flatbed_lamp_pending) {
        if (tick_flatbed_lamp_delay == 0) {
            p_p1->DR.BIT.B6 = 1;
            job_wake_flags.bit.flatbed_lamp_pending = 0;
        } else {
            tick_flatbed_lamp_delay--;
        }
    }
    if (main_flags_1.b & MAIN_FLAG_FUSE_CHECK) {
        set_imask_exr(7);
        if (!p_p1->PORT.BIT.B0) {
            if (tick_lamp_fuse_count++ > 0x5dc) {
                err_flags.bit.device = 1;
                err_saved_flags = err_flags.b;
                err_saved_code = err_code = ERR_LAMP_FUSE;
            }
        } else {
            tick_lamp_fuse_count = 0;
        }
        if (!p_chip_b->inputs.bit.b2) {
            if (tick_motor_fuse_count++ > 0x5dc) {
                err_flags.bit.device = 1;
                err_saved_flags = err_flags.b;
                err_saved_code = err_code = ERR_MOTOR_FUSE;
            }
        } else {
            tick_motor_fuse_count = 0;
        }
        set_imask_exr(tick_saved_imask);
    } else if (tick_word_2) {
        set_imask_exr(7);
        if (!p_p1->PORT.BIT.B0) {
            err_flags.bit.device = 1;
            err_saved_flags = err_flags.b;
            err_saved_code = err_code = ERR_LAMP_FUSE;
        }
    }
    if (hw_p2_out.b & HW_P2_ADF_FRONT_LAMP) mailbox.lamp_status.bit.adf_front_lamp = 1;
    else mailbox.lamp_status.bit.adf_front_lamp = 0;
    if (hw_p2_out.b & HW_P2_ADF_FRONT_LAMP) mailbox.lamp_status.bit.adf_back_lamp = 1;
    else mailbox.lamp_status.bit.adf_back_lamp = 0;
    if (p_p1->DR.BIT.B6) mailbox.lamp_status.bit.flatbed_lamp = 1;
    else mailbox.lamp_status.bit.flatbed_lamp = 0;
}

void tick_poll_covers(void)
{
    unsigned char pre[16];
    union byte_view inputs;
    unsigned char post[160];

    inputs.b = p_p1->PORT.BYTE;
    if (inputs.bit.b1) {
        if (sensor_cover_count >= 25) {
            sensor_cover_count = 49;
        } else {
            sensor_cover_count++;
            if (sensor_cover_count >= 25) sensor_cover_count = 49;
        }
    } else {
        if (sensor_cover_count < 25) {
            sensor_cover_count = 0;
        } else {
            sensor_cover_count--;
            if (sensor_cover_count < 25) sensor_cover_count = 0;
        }
    }
    if (sensor_cover_count >= 25) {
        sensor_flags.bit.adf_cover_open = 1;
        mailbox.sensor_status.bit.adf_cover_open = 1;
    } else {
        sensor_flags.bit.adf_cover_open = 0;
        mailbox.sensor_status.bit.adf_cover_open = 0;
    }
    if (inputs.bit.b5) {
        if (sensor_document_cover_count < 5) sensor_document_cover_count++;
    } else {
        if (sensor_document_cover_count) sensor_document_cover_count--;
    }
    if (sensor_document_cover_count > 2) {
        sensor_flags.bit.document_cover_open = 1;
        mailbox.sensor_status.bit.document_cover_open = 1;
    } else {
        sensor_flags.bit.document_cover_open = 0;
        mailbox.sensor_status.bit.document_cover_open = 0;
    }
    if (sensor_df_active) {
        if ((unsigned char)(p_ad->DRB >> 8) < 0xcc) mailbox.sensor_status_2.bit.df = 1;
        else mailbox.sensor_status_2.bit.df = 0;
    }
}

void tick_read_sensor_mux(void)
{
    unsigned char outputs;

    if (sensor_mux_enable && sensor_mux_count == 0) {
        sensor_mux_count = sensor_mux_interval;
        if (sensor_mux_step == 0) {
            sensor_mux_step = 1;
            sensor_temperature_a_level = p_ad->DRA >> 8;
            sensor_temperature_a = sensor_temperature_table_a[sensor_temperature_a_level];
            mailbox.lamp_temperature = sensor_temperature_a;
        } else if (sensor_mux_step == 1) {
            sensor_mux_step = 2;
            set_imask_exr(7);
            outputs = p_chip_b->outputs.b & 0xbb;
            outputs |= 4;
            p_chip_b->outputs.b = outputs;
            set_imask_exr(tick_saved_imask);
        } else if (sensor_mux_step == 2) {
            if (sensor_mux_mode == 1) {
                sensor_mux_step = 3;
                set_imask_exr(7);
                outputs = p_chip_b->outputs.b & 0xbb;
                p_chip_b->outputs.b = outputs;
                p_p1->DR.BIT.B4 = 0;
                p_p1->DR.BIT.B7 = 0;
                p_p2->DR.BIT.B3 = 0;
                set_imask_exr(tick_saved_imask);
            } else {
                sensor_mux_step = 0;
                sensor_temperature_b_level = p_ad->DRA >> 8;
                sensor_temperature_b = sensor_temperature_table_b[sensor_temperature_b_level];
                mailbox.lamp_temperature_b = sensor_temperature_b;
                set_imask_exr(7);
                outputs = p_chip_b->outputs.b & 0xbb;
                outputs |= 0x40;
                p_chip_b->outputs.b = outputs;
                set_imask_exr(tick_saved_imask);
            }
        } else if (sensor_mux_step == 3) {
            sensor_mux_levels[0] = p_ad->DRA >> 8;
            tick_check_mux_level(0);
            sensor_mux_step = 4;
            set_imask_exr(7);
            outputs = p_chip_b->outputs.b & 0xbb;
            p_chip_b->outputs.b = outputs;
            p_p1->DR.BIT.B4 = 1;
            p_p1->DR.BIT.B7 = 0;
            p_p2->DR.BIT.B3 = 0;
            set_imask_exr(tick_saved_imask);
        } else if (sensor_mux_step == 4) {
            sensor_mux_levels[1] = p_ad->DRA >> 8;
            tick_check_mux_level(1);
            sensor_mux_step = 5;
            set_imask_exr(7);
            outputs = p_chip_b->outputs.b & 0xbb;
            p_chip_b->outputs.b = outputs;
            p_p1->DR.BIT.B4 = 0;
            p_p1->DR.BIT.B7 = 1;
            p_p2->DR.BIT.B3 = 0;
            set_imask_exr(tick_saved_imask);
        } else if (sensor_mux_step == 5) {
            sensor_mux_levels[2] = p_ad->DRA >> 8;
            tick_check_mux_level(2);
            sensor_mux_step = 6;
            set_imask_exr(7);
            outputs = p_chip_b->outputs.b & 0xbb;
            p_chip_b->outputs.b = outputs;
            p_p1->DR.BIT.B4 = 1;
            p_p1->DR.BIT.B7 = 1;
            p_p2->DR.BIT.B3 = 0;
            set_imask_exr(tick_saved_imask);
        } else if (sensor_mux_step == 6) {
            sensor_mux_levels[3] = p_ad->DRA >> 8;
            tick_check_mux_level(3);
            sensor_mux_step = 7;
            set_imask_exr(7);
            outputs = p_chip_b->outputs.b & 0xbb;
            p_chip_b->outputs.b = outputs;
            p_p1->DR.BIT.B4 = 0;
            p_p1->DR.BIT.B7 = 0;
            p_p2->DR.BIT.B3 = 1;
            set_imask_exr(tick_saved_imask);
        } else if (sensor_mux_step == 7) {
            sensor_mux_levels[4] = p_ad->DRA >> 8;
            tick_check_mux_level(4);
            sensor_mux_step = 8;
            set_imask_exr(7);
            outputs = p_chip_b->outputs.b & 0xbb;
            p_chip_b->outputs.b = outputs;
            p_p1->DR.BIT.B4 = 1;
            p_p1->DR.BIT.B7 = 0;
            p_p2->DR.BIT.B3 = 1;
            set_imask_exr(tick_saved_imask);
        } else if (sensor_mux_step == 8) {
            sensor_mux_levels[5] = p_ad->DRA >> 8;
            tick_check_mux_level(5);
            sensor_mux_step = 9;
            set_imask_exr(7);
            outputs = p_chip_b->outputs.b & 0xbb;
            p_chip_b->outputs.b = outputs;
            p_p1->DR.BIT.B4 = 0;
            p_p1->DR.BIT.B7 = 1;
            p_p2->DR.BIT.B3 = 1;
            set_imask_exr(tick_saved_imask);
        } else if (sensor_mux_step == 9) {
            sensor_mux_levels[6] = p_ad->DRA >> 8;
            tick_check_mux_level(6);
            sensor_mux_step = 10;
            set_imask_exr(7);
            outputs = p_chip_b->outputs.b & 0xbb;
            p_chip_b->outputs.b = outputs;
            p_p1->DR.BIT.B4 = 1;
            p_p1->DR.BIT.B7 = 1;
            p_p2->DR.BIT.B3 = 1;
            set_imask_exr(tick_saved_imask);
        } else if (sensor_mux_step == 10) {
            sensor_mux_levels[7] = p_ad->DRA >> 8;
            tick_check_mux_level(7);
            sensor_mux_cycles++;
            if (sensor_mux_cycles >= 3) sensor_mux_mode = 2;
            sensor_mux_step = 0;
            set_imask_exr(7);
            outputs = p_chip_b->outputs.b & 0xbb;
            outputs |= 0x40;
            p_chip_b->outputs.b = outputs;
            p_p1->DR.BIT.B4 = 0;
            p_p1->DR.BIT.B7 = 0;
            p_p2->DR.BIT.B3 = 0;
            set_imask_exr(tick_saved_imask);
        }
        sensor_an1_level = p_ad->DRB >> 8;
        mailbox.lamp_temperature_flatbed = sensor_temperature_table[sensor_an1_level];
        sensor_an2_level = p_ad->DRC >> 8;
        mailbox.lamp_temperature_front = sensor_temperature_table[sensor_an2_level];
        sensor_an3_level = p_ad->DRD >> 8;
        mailbox.lamp_temperature_back = sensor_temperature_table[sensor_an3_level];
    }
}

void tick_check_mux_level(short input)
{
    unsigned char thr;

    if (input <= 1) thr = SENSOR_MUX_ALARM_LEVEL;
    else if (input >= 6) thr = SENSOR_MUX_ALARM_LEVEL_6_7;
    else thr = SENSOR_MUX_ALARM_LEVEL;
    if (sensor_mux_levels[input] >= thr) {
        sensor_mux_alarm_counts[input]++;
        if (sensor_mux_alarm_counts[input] >= SENSOR_MUX_ALARM_COUNT) {
            err_flags.bit.device = 1;
            err_saved_flags = err_flags.b;
            err_saved_code = err_code = ERR_VDCC;
        }
    } else {
        sensor_mux_alarm_counts[input] = 0;
    }
}

void tick_control_heaters(void)
{
    unsigned short unused;

    if (mailbox.settings_af & 1) {
        lamp_heater_flatbed_on = 0;
        lamp_heater_front_on = 0;
        lamp_heater_back_on = 0;
        lamp_heater_flatbed_warmup = 0;
        lamp_heater_front_warmup = 0;
        lamp_heater_back_warmup = 0;
        mailbox.lamp_temperature_flatbed = 0x41;
        mailbox.lamp_temperature_front = 0x41;
        mailbox.lamp_temperature_back = 0x41;
    }
    if (tick_warmup_hold != 0) {
        return;
    }
    if (lamp_unit_front_on != 0 || lamp_unit_back_on != 0 || lamp_unit_flatbed_on != 0) {
        return;
    }
    {
        {
            if (lamp_heater_flatbed_on != 0 && (!p_p1->DR.BIT.B6 || lamp_heater_flatbed_warmup != 0) && lamp_heater_flatbed_inhibit == 0) {
                if (lamp_heater_ticks == 0) {
                    set_imask_exr(7);
                    if (mailbox.lamp_temperature_flatbed < lamp_heater_flatbed_target) {
                        p_p2->DR.BIT.B4 = 1;
                    } else {
                        p_p2->DR.BIT.B4 = 0;
                        lamp_heater_flatbed_warmup = 0;
                    }
                    set_imask_exr(tick_saved_imask);
                }
            } else {
                set_imask_exr(7);
                p_p2->DR.BIT.B4 = 0;
                set_imask_exr(tick_saved_imask);
            }
            if (lamp_heater_front_on != 0 && (!p_p2->DR.BIT.B0 || lamp_heater_front_warmup != 0) && lamp_heater_front_inhibit == 0) {
                if (lamp_heater_ticks == 0) {
                }
            } else {
                set_imask_exr(7);
                p_p2->DR.BIT.B5 = 0;
                set_imask_exr(tick_saved_imask);
            }
            if (lamp_heater_back_on != 0 && (!p_p2->DR.BIT.B2 || lamp_heater_back_warmup != 0) && lamp_heater_back_inhibit == 0) {
                if (lamp_heater_ticks == 0) {
                }
            } else {
                set_imask_exr(7);
                p_p2->DR.BIT.B6 = 0;
                set_imask_exr(tick_saved_imask);
            }
            if (lamp_heater_ticks == 0) {
                lamp_heater_ticks = lamp_heater_interval;
            }
            return;
        }
    }
}

void tick_keep_lamps_warm(void)
{
    if (lamp_keep_warm_count_front != 0) {
        lamp_keep_warm_count_front--;
        if (lamp_keep_warm_count_front == 0) {
            lamp_keep_warm_count_front = lamp_keep_warm_period_front;
            set_imask_exr(7);
            if (mailbox.lamp_temperature_front < lamp_keep_warm_on_front) {
                hw_p2_out.b |= HW_P2_ADF_FRONT_LAMP;
                p_p2->DR.BYTE = hw_p2_out.b;
            } else if (mailbox.lamp_temperature_front >= lamp_keep_warm_off_front) {
                hw_p2_out.b &= ~HW_P2_ADF_FRONT_LAMP;
                p_p2->DR.BYTE = hw_p2_out.b;
            }
            if (mailbox.lamp_temperature_back < lamp_keep_warm_on_back) {
                hw_p2_out.b |= HW_P2_ADF_BACK_LAMP;
                p_p2->DR.BYTE = hw_p2_out.b;
            } else if (mailbox.lamp_temperature_back >= lamp_keep_warm_off_back) {
                hw_p2_out.b &= ~HW_P2_ADF_BACK_LAMP;
                p_p2->DR.BYTE = hw_p2_out.b;
            }
            set_imask_exr(tick_saved_imask);
        }
    }
}
