#include "mdc.h"

void main_loop(void)
{
    unsigned char saved_byte_0x3cd;
    unsigned char saved_speed_class_a;
    unsigned char saved_speed_class_b;
    short count;
    unsigned char *p;

    for (;;) {
        switch (mailbox.startup_state) {
        case MDC_STARTUP_RUN_4:
        case MDC_STARTUP_RUN_6:
        case MDC_STARTUP_RUN:
            if (mailbox.startup_request != MDC_STARTUP_RESTART_MECHANICS) {
                {
                if (err_code == 0) {
                    if (main_init_state != 0) {
                        main_init_mechanics();
                        if (main_init_state == 0) {
                            mailbox.ready = 0xff;
                        }
                    }
                    job_run();
                    if (err_deferred != 0) {
                        if (feed_pick_edge_count == 0 || tick_timeout == 0) {
                            if (err_code == 0) {
                                err_code = err_deferred;
                            }
                            main_handle_error();
                        }
                    }
                } else {
                    if (err_code != ERR_NO_PAPER || feed_pick_edge_count == 0) {
                        main_handle_error();
                    } else {
                        err_deferred = err_code;
                        err_code = 0;
                        tick_timeout = 15000;
                    }
                }
                }
            } else {
                mailbox.startup_state = MDC_STARTUP_RESTART_MECHANICS;
            }
            break;
        case MDC_STARTUP_POWER_ON:
            if (mailbox.startup_request == MDC_STARTUP_INIT_MECHANICS) {
                main_init_state = 2;
                main_init_mechanics();
                if (main_init_state == 0) {
                    mailbox.request = 0;
                    hw_hook_started();
                    mailbox.ready = 0xff;
                    mailbox.startup_state = MDC_STARTUP_INIT_MECHANICS;
                }
            }
            break;
        case MDC_STARTUP_INIT_MECHANICS:
            if (mailbox.startup_request == MDC_STARTUP_RUN_4) {
                mailbox.startup_state = MDC_STARTUP_RUN_4;
            } else if (mailbox.startup_request == MDC_STARTUP_RUN_6) {
                mailbox.startup_state = MDC_STARTUP_RUN_6;
            } else if (mailbox.startup_request == MDC_STARTUP_RUN) {
                tick_timeout = 0x5dc;
                while (!p_chip_b->inputs.bit.b3) {
                    if (tick_timeout == 0) {
                        err_flags.b |= ERR_FLAG_DEVICE;
                        err_saved_flags = err_flags.b;
                        err_saved_code = err_code = ERR_LAMP_FUSE_2;
                    }
                }
                tick_timeout = 0x5dc;
                while (!p_chip_b->inputs.bit.b1) {
                    if (tick_timeout == 0) {
                        err_flags.b |= ERR_FLAG_DEVICE;
                        err_saved_flags = err_flags.b;
                        err_saved_code = err_code = ERR_CGA_BOARD;
                        break;
                    }
                }
                mailbox.startup_state = MDC_STARTUP_RUN;
            } else if (mailbox.cmd_state == MDC_CMD_STATE_POSTED) {
                lamp_off_count = lamp_off_limit;
                if (mailbox.cmd_code == MDC_CMD_ADJUST_DF_SENSOR) {
                    job_set_cmd_state(MDC_CMD_STATE_RUNNING);
                    cmd_phase = CMD_PHASE_START;
                    do {
                        cmd_dispatch();
                    } while (cmd_phase != CMD_PHASE_IDLE && err_flags.b == 0);
                } else {
                    job_set_cmd_state(MDC_CMD_STATE_RUNNING);
                    nop();
                    job_set_cmd_state(MDC_CMD_STATE_DONE);
                    mailbox.cmd_result = MDC_RESULT_OK;
                }
            } else if (err_flags.b != 0) {
                main_handle_error();
                if (main_init_state != 0) {
                    main_init_mechanics();
                    if (main_init_state == 0) {
                        mailbox.ready = 0xff;
                    }
                }
            }
            break;
        case MDC_STARTUP_RESTART_MECHANICS:
            set_imask_exr(7);
            p_p2->DR.BYTE = hw_p2_out.b = 0;
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_OFF);
            motor_set_current(MOTOR_PICK, MOTOR_CURRENT_OFF);
            motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_OFF);
            motor_set_current(MOTOR_FB, MOTOR_CURRENT_OFF);
            motor_enable_chips(0);
            saved_byte_0x3cd = mailbox.byte_0x3cd;
            saved_speed_class_a = mailbox.speed_class_a;
            saved_speed_class_b = mailbox.speed_class_b;
            count = 1;
            p = &mailbox.ready;
            do {
                *p++ = 0;
            } while (--count);
            count = &mailbox.cmd_result - &job_state;
            p = &job_state;
            do {
                *p++ = 0;
            } while (--count);
            mailbox.byte_0x3cd = saved_byte_0x3cd;
            mailbox.speed_class_a = saved_speed_class_a;
            mailbox.speed_class_b = saved_speed_class_b;
            main_init_state = 4;
            main_init_mechanics();
            if (main_init_state == 0) {
                mailbox.request = 0;
                hw_hook_started();
                mailbox.ready = 0xff;
                mailbox.startup_state = MDC_STARTUP_INIT_MECHANICS;
            }
            break;
        case MDC_STARTUP_HOLD:
        case MDC_STARTUP_HOLD_2:
        case MDC_STARTUP_HOLD_3:
            break;
        }
    }
}

void main_handle_error(void)
{
    unsigned short error_seen;
    unsigned char stop_option;

    error_seen = 0;
    stop_option = mailbox.settings_c0 & 8;
    if (err_code == ERR_NO_PAPER) {
        if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE && mailbox.cmd_state == MDC_CMD_STATE_POSTED && mailbox.long_page == MDC_LONG_PAGE_ON)
            job_set_cmd_state(MDC_CMD_STATE_RUNNING);
        if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE && mailbox.cmd_state == MDC_CMD_STATE_RUNNING) {
            err_code = ERR_JAM;
        } else if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE && (mailbox.motion_flags.b & (MDC_MOTION_FRONT | MDC_MOTION_BACK | MDC_MOTION_BACK_PENDING))) {
            if (bw_motor_arm_request) {
                cmd_timeout = CMD_TIMEOUT_TICKS;
                while (bw_motor_arm_request) {
                    if (cmd_timeout == 0)
                        break;
                }
            }
            err_code = ERR_EJECT_JAM;
        }
    }
    while (1) {
        switch (err_state) {
        case 0:
            if ((main_flags_1.b & MAIN_FLAG_ADF_COVER_OPEN) && !(err_flags.b & ERR_FLAG_DEVICE) && err_code != ERR_SHIPPING_LOCK) {
                mailbox.error_flags.bit.document = 1;
                mailbox.error_code = ERR_ADF_COVER_OPEN;
            } else if (err_saved_flags) {
                mailbox.error_flags.b = err_flags.b = err_saved_flags;
                mailbox.error_code = err_code = err_saved_code;
            } else {
                mailbox.error_flags.b = err_flags.b;
                mailbox.error_code = err_code;
            }
            if (mailbox.error_code && (stop_option != 8 || sensor_flags.bit.no_paper)) {
                feed_length_index = 0;
                job_length_history_reset = 0xff;
            }
            error_seen = 1;
            main_error_seconds = 0;
            main_error_seconds = 0;
            if (mailbox.error_code == ERR_VDCC) {
                motor_set_current(MOTOR_FEED, MOTOR_CURRENT_OFF);
                motor_set_current(MOTOR_PICK, MOTOR_CURRENT_OFF);
                motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_OFF);
                motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_OFF);
                motor_set_current(MOTOR_FB, MOTOR_CURRENT_OFF);
            }
            if (sensor_df_active == 1 && sensor_df_test == 0)
                hw_set_ad_mode(AD_SCAN_AN0_3);
            if (mailbox.error_code == ERR_ADF_COVER_OPEN)
                mailbox.mechanics_init_mode = 1;
            tick_timeout = 0x3a98;
            err_state = 1;
            break;
        case 1:
            switch (mailbox.error_code) {
            case ERR_NO_PAPER:
                if (mailbox.manual_feed_timeout)
                    feed_top_edge_steps = 0;
                util_overlap_clear_after_error();
                pick_next_sheet = 0;
                break;
            case ERR_MULTIFEED_LENGTH:
            case ERR_MULTIFEED_ULTRASONIC:
                if (!(mailbox.feed_options.b & MDC_FEED_MULTIFEED_REPORT_ONLY)) {
                    pick_state = PICK_STATE_RESET;
                    feed_state = FEED_STATE_RESET;
                }
                main_clear_path_pending = 1;
                break;
            case ERR_SHEET_GAP:
                p_p2->DR.BYTE = (hw_p2_out.b &= ~HW_P2_ADF_LAMPS);
                pick_state = PICK_STATE_RESET;
                feed_state = FEED_STATE_RESET;
                break;
            case ERR_PICK:
            case ERR_PICK_SENSOR_JAM:
                if (mailbox.error_code == ERR_PICK)
                    feed_top_edge_steps = 0;
                p_p2->DR.BYTE = (hw_p2_out.b &= ~HW_P2_ADF_LAMPS);
                pick_state = PICK_STATE_RESET;
                break;
            case ERR_ADF_COVER_OPEN:
                break;
            case ERR_STOPPED:
                pick_state = PICK_STATE_RESET;
                feed_state = FEED_STATE_RESET;
                set_imask_exr(7);
                sensor_stop_ultrasonic();
                feed_ultrasonic_running = 0;
                multifeed_check.in_area = 0;
                multifeed_check.flags.bit.b8 = 0;
                set_imask_exr(0);
                bw_motor_backing_request = 0;
                motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_OFF);
                set_imask_exr(7);
                if (bw_motor_arm_request)
                    bw_motor_arm_state = BW_MOTOR_ARM_STATE_START;
                bw_motor_arm_request = 0;
                set_imask_exr(0);
                motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_OFF);
                if (mailbox.manual_feed_timeout) {
                    feed_empty_seconds = feed_manual_timeout_limit + 1;
                    if (feed_manual_empty) {
                        sensor_flags.bit.no_paper = 1;
                        mailbox.sensor_status.bit.no_paper = 1;
                    }
                }
                break;
            case ERR_EJECT_JAM:
            default:
                p_p2->DR.BYTE = (hw_p2_out.b &= ~HW_P2_ADF_LAMPS);
                pick_state = PICK_STATE_RESET;
                feed_state = FEED_STATE_RESET;
                bw_motor_backing_request = 0;
                motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_OFF);
                set_imask_exr(7);
                if (bw_motor_arm_request)
                    bw_motor_arm_state = BW_MOTOR_ARM_STATE_START;
                bw_motor_arm_request = 0;
                set_imask_exr(0);
                motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_OFF);
                break;
            }
            if (log_error_armed && mailbox.error_code != ERR_NO_PAPER) {
                log_hook_begin(0xfa);
                p_log_buf[log_index] = 0xee;
                log_advance_255();
                p_log_buf[log_index] = 0xee;
                log_advance_255();
                p_log_buf[log_index] = mailbox.error_code;
                log_advance_255();
                p_log_buf[log_index] = mailbox.error_flags.b;
                log_advance_255();
                p_log_buf[log_index] = LOG_END;
                log_end();
            } else if (mailbox.error_code == ERR_NO_PAPER) {
                util_log_add_error_word();
            }
            err_state = 2;
            break;
        case 2:
            if (bw_motor_backing_request) {
                if (tick_timeout == 0) {
                    bw_motor_backing_request = 0;
                    motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_OFF);
                }
                break;
            }
            bw_motor_backing_quiet = 0;
            if (pick_ramp_state == RAMP_STATE_STOPPED) {
                if (feed_state == FEED_STATE_WAIT_SHEET)
                    feed_state = FEED_STATE_RESET;
                if (mailbox.error_code == ERR_ADF_COVER_OPEN && bw_motor_backing_request == 0 && bw_motor_backing_state != 0 && bw_motor_backing_state != 5 && tick_timeout != 0) {
                    set_imask_exr(7);
                    bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
                    bw_motor_backing_quiet = 1;
                    set_imask_exr(0);
                }
                err_state = 3;
            } else if (tick_timeout == 0) {
                pick_state = PICK_STATE_RESET;
            }
            break;
        case 3:
            if (bw_motor_backing_request) {
                if (tick_timeout == 0) {
                    bw_motor_backing_request = 0;
                    motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_OFF);
                }
                break;
            }
            bw_motor_backing_quiet = 0;
            if (feed_ramp_state == RAMP_STATE_STOPPED) {
                feed_mode.bit.half_step = 0;
                if (mailbox.cmd_state == MDC_CMD_STATE_RUNNING || mailbox.cmd_state == MDC_CMD_STATE_ENDED) {
                    if (mailbox.cmd_code == MDC_CMD_POWER_SAVE) {
                    } else {
                        job_set_cmd_state(MDC_CMD_STATE_DONE);
                        mailbox.cmd_result = MDC_RESULT_ERROR;
                    }
                }
                if (mailbox.error_code == ERR_ADF_COVER_OPEN)
                    motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_OFF);
                if (job_wake_flags.bit.wake_event) {
                    if (err_flags.b == ERR_FLAG_DOCUMENT) {
                        tick_flatbed_lamp_delay = 0;
                        job_wake_flags.bit.flatbed_lamp_pending = 1;
                    }
                }
                err_state = 5;
            } else if (tick_timeout == 0) {
                feed_state = FEED_STATE_RESET;
            }
            break;
        case 5:
            if (mailbox.error_code == ERR_ADF_COVER_OPEN) {
                if (tick_adf_cover_state == 0) {
                    mailbox.mechanics_init_mode = 1;
                    main_flags_1.bit.adf_cover_open = 0;
                    mailbox.motion_flags.b = 0;
                    mailbox.error_flags.b = 0;
                    mailbox.error_code = 0;
                    err_flags.b = 0;
                    err_code = 0;
                    err_saved_flags = 0;
                    err_saved_code = 0;
                    err_deferred = 0;
                    mailbox.sheet_count_high = 0;
                    mailbox.sheet_count_low = 0;
                    job_busy_flags.b = 0;
                    job_state = JOB_STATE_RESET;
                    set_imask_exr(7);
                    pick_state = PICK_STATE_IDLE;
                    feed_state = FEED_STATE_IDLE;
                    set_imask_exr(0);
                    feed_pick_start_flags.w = 0;
                    main_init_state = 6;
                    err_state = 0;
                } else if (mailbox.cmd_state == MDC_CMD_STATE_POSTED) {
                    set_imask_exr(7);
                    if (job_cmd_history_last != mailbox.cmd_code) {
                        job_cmd_history_last = mailbox.cmd_code;
                        if (job_cmd_history_index > 0x31)
                            job_cmd_history_index = 0;
                        job_cmd_history[job_cmd_history_index++] = mailbox.cmd_code;
                    }
                    set_imask_exr(0);
                    lamp_off_count = lamp_off_limit;
                    if (mailbox.cmd_code == MDC_CMD_MOTOR_TEST || mailbox.cmd_code == MDC_CMD_BACKING_TEST || mailbox.cmd_code == MDC_CMD_LAMPS) {
                        job_set_cmd_state(MDC_CMD_STATE_RUNNING);
                        cmd_phase = CMD_PHASE_START;
                        do {
                            cmd_dispatch();
                        } while (cmd_phase);
                        if (mailbox.cmd_state == MDC_CMD_STATE_RUNNING) {
                            job_set_cmd_state(MDC_CMD_STATE_DONE);
                            mailbox.cmd_result = MDC_RESULT_OK;
                        }
                    } else if (mailbox.cmd_code == MDC_CMD_FB_MOTOR_HOME) {
                        main_fb_home_cmd = 2;
                        cmd_test_mode_on();
                        err_flags.b = 0;
                        err_code = 0;
                        err_saved_flags = 0;
                        err_saved_code = 0;
                        err_deferred = 0;
                        mailbox.error_flags.b = 0;
                        mailbox.error_code = 0;
                        return;
                    } else if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE && (mailbox.source & MDC_SOURCE_FLATBED)) {
                        err_flags.b = 0;
                        err_code = 0;
                        err_saved_flags = 0;
                        err_saved_code = 0;
                        err_deferred = 0;
                        mailbox.error_flags.b = 0;
                        mailbox.error_code = 0;
                        main_init_state = 0;
                        job_busy_flags.b = 0;
                        job_state = JOB_STATE_RESET;
                        err_state = 0;
                        return;
                    } else if (mailbox.cmd_code == MDC_CMD_POWER_SAVE) {
                    } else if (mailbox.cmd_code == MDC_CMD_DF_TEST_ON || mailbox.cmd_code == MDC_CMD_DF_TEST_OFF) {
                        job_set_cmd_state(MDC_CMD_STATE_RUNNING);
                        cmd_phase = CMD_PHASE_START;
                        cmd_dispatch();
                        job_set_cmd_state(MDC_CMD_STATE_DONE);
                        mailbox.cmd_result = MDC_RESULT_OK;
                    } else if (mailbox.cmd_code == MDC_CMD_POWER_SAVE) {
                    } else {
                        job_set_cmd_state(MDC_CMD_STATE_DONE);
                        mailbox.cmd_result = MDC_RESULT_ERROR;
                    }
                }
            } else if (mailbox.cmd_state == MDC_CMD_STATE_POSTED) {
                set_imask_exr(7);
                if (job_cmd_history_last != mailbox.cmd_code) {
                    job_cmd_history_last = mailbox.cmd_code;
                    if (job_cmd_history_index > 0x31)
                        job_cmd_history_index = 0;
                    job_cmd_history[job_cmd_history_index++] = mailbox.cmd_code;
                }
                set_imask_exr(0);
                lamp_off_count = lamp_off_limit;
                if (mailbox.cmd_code == MDC_CMD_CLEAR_ERRORS) {
                    job_set_cmd_state(MDC_CMD_STATE_RUNNING);
                    if (mailbox.error_code != ERR_BACKING && bw_motor_arm_request == 0 && bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL) {
                        bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                        while (bw_motor_arm_request)
                            ;
                    }
                    mailbox.motion_flags.b = 0;
                    mailbox.sheet_count_high = 0;
                    mailbox.sheet_count_low = 0;
                    job_busy_flags.b = 0;
                    job_state = JOB_STATE_RESET;
                    err_flags.b = 0;
                    err_code = 0;
                    err_saved_flags = 0;
                    err_saved_code = 0;
                    err_deferred = 0;
                    mailbox.error_flags.b = 0;
                    mailbox.error_code = 0;
                    job_set_cmd_state(MDC_CMD_STATE_DONE);
                    mailbox.cmd_result = MDC_RESULT_OK;
                    feed_pick_start_flags.w = 0;
                    err_state = 0;
                } else if (mailbox.cmd_code == MDC_CMD_POWER_SAVE) {
                } else if (mailbox.cmd_code == MDC_CMD_DF_TEST_OFF) {
                    job_set_cmd_state(MDC_CMD_STATE_RUNNING);
                    cmd_phase = CMD_PHASE_START;
                    do {
                        cmd_dispatch();
                    } while (cmd_phase);
                    if (mailbox.cmd_state == MDC_CMD_STATE_RUNNING) {
                        job_set_cmd_state(MDC_CMD_STATE_DONE);
                        mailbox.cmd_result = MDC_RESULT_OK;
                    }
                } else {
                    job_set_cmd_state(MDC_CMD_STATE_DONE);
                    mailbox.cmd_result = MDC_RESULT_ERROR;
                }
            }
            break;
        }
        if (mailbox.error_code != ERR_ADF_COVER_OPEN) {
            if ((main_flags_1.b & MAIN_FLAG_ADF_COVER_OPEN) && !(err_flags.b & ERR_FLAG_DEVICE) && err_code != ERR_SHIPPING_LOCK) {
                err_flags.bit.document = 1;
                err_code = ERR_ADF_COVER_OPEN;
                err_state = 0;
            } else if (err_saved_code && mailbox.error_code != err_saved_code) {
                err_state = 0;
            } else if (err_state == 5 && mailbox.error_flags.b == 0) {
                err_state = 0;
            }
        }
        if (err_code == 0) {
            if (mailbox.page_end.b & MDC_PAGE_END_ACCEPTED)
                mailbox.page_end.b &= ~MDC_PAGE_END_ACCEPTED;
            return;
        }
        if (mailbox.startup_request == MDC_STARTUP_RESTART_MECHANICS)
            return;
        if (mailbox.cmd_code == MDC_CMD_DOWNLOAD_START) {
            job_set_cmd_state(MDC_CMD_STATE_RUNNING);
            util_flash_enter_download();
        }
        if (mailbox.cmd_code == MDC_CMD_POWER_SAVE && mailbox.cmd_state == MDC_CMD_STATE_POSTED) {
            lamp_off_count = lamp_off_limit;
            main_power_save_error_flags = err_flags.b;
            main_power_save_error_code = err_code;
            cmd_test_mode_on();
            job_set_cmd_state(MDC_CMD_STATE_RUNNING);
            cmd_power_save();
            err_flags.b = main_power_save_error_flags;
            err_code = main_power_save_error_code;
            mailbox.error_flags.b = main_power_save_error_flags;
            mailbox.error_code = main_power_save_error_code;
            if (mailbox.error_code && (stop_option != 8 || sensor_flags.bit.no_paper)) {
                feed_length_index = 0;
            }
            cmd_test_mode_off();
            mailbox.mechanics_init_mode = 0;
        }
        if (main_error_seconds >= 0xc && error_seen == 1 && feed_ramp_state == RAMP_STATE_STOPPED && !job_capture_flags.bit.pause_active && mailbox.cmd_code == MDC_CMD_SCAN_MOVE && (mailbox.cmd_state == MDC_CMD_STATE_ENDED || mailbox.cmd_state == MDC_CMD_STATE_RUNNING)) {
            mailbox.cmd_result = MDC_RESULT_ERROR;
            error_seen = 0;
        }
    }
}

void main_init_mechanics(void)
{
    unsigned char *p;
    unsigned char count;

    lamp_heater_flatbed_on = 0;
    lamp_heater_front_on = 0;
    lamp_heater_back_on = 0;
    for (;;) {
        if (sensor_flags.bit.adf_cover_open) {
            multifeed_check.adjust_state = 2;
            main_init_state = 0;
        }
        if (mailbox.error_code == ERR_VDCC || err_code == ERR_VDCC) {
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_OFF);
            motor_set_current(MOTOR_PICK, MOTOR_CURRENT_OFF);
            motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_OFF);
            motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_OFF);
            motor_set_current(MOTOR_FB, MOTOR_CURRENT_OFF);
            main_init_state = 0;
        }
        switch (main_init_state) {
        case 2:
            main_init_state = 4;
            break;
        case 4:
            if (mailbox.mechanics_init_mode != 6) {
                pick_state = PICK_STATE_IDLE;
                feed_state = FEED_STATE_IDLE;
            }
            main_init_state = 6;
            break;
        case 6:
            tick_timeout = 0x1f4;
            p = mailbox.imprinter_version;
            count = 4;
            do {
                *p++ = 0x20;
            } while (--count);
            util_put_be16(&mailbox.top_to_front_distance_high, main_top_to_front_distance);
            util_put_be16(&mailbox.top_to_back_distance_high, main_top_to_back_distance);
            util_put_be16(&mailbox.byte_0x35e, 0x2c4);
            set_imask_exr(7);
            util_put_be16(&mailbox.byte_0x350, main_mailbox_350_value);
            if (mailbox.settings_8f) {
                multifeed_check.threshold = mailbox.settings_8f;
            } else if (multifeed_check.idle_threshold) {
                multifeed_check.threshold = multifeed_check.idle_threshold;
            } else {
                multifeed_check.threshold = ULTRASONIC_THRESHOLD_DEFAULT;
            }
            multifeed_check.low_limit = 2;
            multifeed_check.w28 = 0x1e;
            multifeed_check.w30 = 2;
            main_scale_ultrasonic_threshold();
            set_imask_exr(0);
            main_init_state = 8;
            break;
        case 8:
            if (mailbox.mechanics_init_mode == 6 && !sensor_flags.bit.top) {
                if (sensor_flags.bit.pick) {
                    if (sensor_flags.bit.pick && (job_busy_flags.b & JOB_BUSY_PREPICK)) {
                        main_init_state = 0x12;
                        break;
                    }
                } else {
                    main_init_state = 0x12;
                    break;
                }
            }
            if (mailbox.mechanics_init_mode == 6)
                main_init_after_wake = 1;
            mailbox.mechanics_init_mode = 2;
            if (main_fb_home_at_init) {
                main_fb_home_at_init = 0;
                fb_motor_request = FB_MOTOR_HOME;
                fb_motor_state = FB_MOTOR_STATE_START;
                main_init_state = 0xa;
            } else {
                main_init_state = 0xe;
            }
            break;
        case 0xa:
            util_fb_motor_home();
            main_init_state = 0xe;
            break;
        case 0xe:
            util_feed_clear_path();
            main_init_state = 0x10;
            break;
        case 0x10:
            mailbox.mechanics_init_mode = 3;
            if (mailbox.mechanics_init_mode != 5 && mailbox.mechanics_init_mode != 6 && multifeed_check.adjust_state == 0) {
                multifeed_check.adjust_state = 1;
                cmd_adjust_ultrasonic(1);
                cmd_adjust_df_sensor();
            }
            main_init_state = 0x12;
            break;
        case 0x12:
            set_imask_exr(0);
            mailbox.byte_0x34a = 2;
            main_init_state = 0x14;
            break;
        case 0x14:
            if (main_backing_homed)
                main_backing_homed = 0;
            else
                util_backing_to_home();
            main_init_state = 0;
            break;
        }
        main_byte_9 = 0xd0;
        if (err_flags.b && (main_init_error_flags == 0 || (err_flags.b & ERR_FLAG_DEVICE))) {
            main_init_error_flags = err_flags.b;
            main_init_error_code = err_code;
            err_flags.b = 0;
            err_code = 0;
        }
        if (main_init_state == 0) {
            if (!sensor_flags.bit.adf_cover_open)
                p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_ADF_LAMPS);
            if (main_init_error_flags) {
                mailbox.error_flags.b = err_flags.b = main_init_error_flags;
                mailbox.error_code = err_code = main_init_error_code;
                main_init_error_flags = 0;
                main_init_error_code = 0;
                if (mailbox.error_code)
                    feed_length_index = 0;
            }
            mailbox.mechanics_init_mode = 0;
            break;
        }
    }
    lamp_heater_flatbed_on = 1;
    lamp_heater_front_on = 1;
    lamp_heater_back_on = 1;
}

void main_copy_image(void)
{
    unsigned short i;
    unsigned short blk;
    unsigned long lim;
    unsigned long j;
    unsigned short *dst;
    unsigned short *src;

    i = 9;
    if (i != 0)
        return;
    for (; i < 9; i++) {
        switch (i) {
        case 0:
            dst = (unsigned short *)0x100000;
            src = (unsigned short *)0x80000;
            blk = 0x10;
            break;
        case 1:
            dst = (unsigned short *)0x108000;
            src = (unsigned short *)0x88000;
            blk = 0x10;
            break;
        case 2:
            dst = (unsigned short *)0x110000;
            src = (unsigned short *)0x90000;
            blk = 0x10;
            break;
        case 3:
            dst = (unsigned short *)0x118000;
            src = (unsigned short *)0x98000;
            blk = 0x10;
            break;
        case 4:
            dst = (unsigned short *)0x120000;
            src = (unsigned short *)0xa0000;
            blk = 0x10;
            break;
        case 5:
            dst = (unsigned short *)0x128000;
            src = (unsigned short *)0xa8000;
            blk = 0x10;
            break;
        case 6:
            dst = (unsigned short *)0x130000;
            src = (unsigned short *)0xb0000;
            blk = 0x10;
            break;
        case 7:
            dst = (unsigned short *)0x138000;
            src = (unsigned short *)0xb8000;
            blk = 0x10;
            break;
        case 8:
            dst = (unsigned short *)0x178000;
            src = (unsigned short *)0xf8000;
            blk = 4;
            break;
        }
        flash_write_unlock_cycles();
        if (!(i & 1))
            flash_erase_sector(0x100000, dst, 1);
        tick_timeout = 0x14;
        while (tick_timeout)
            ;
        lim = (unsigned long)((long)blk << 8) << 2;
        for (j = 0; j < lim; j++) {
            flash_program_word(0x100000, dst, *src, 1);
            dst++;
            src++;
        }
        tick_timeout = 0x14;
        while (tick_timeout)
            ;
    }
    while (1)
        ;
}
