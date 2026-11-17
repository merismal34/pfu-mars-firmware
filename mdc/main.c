#include "mdc.h"

void main(void)
{
    unsigned short i;
    unsigned short j;
    unsigned char chip_a_bit_6;
    unsigned char current_level;
    unsigned char low_count;
    unsigned char c12;
    unsigned char keep_warm_setting;
    unsigned char keep_warm_max;
    unsigned short keep_warm_ticks;

    low_count = 0;
    hw_copy_vectors();
    hw_init_sections();
    hw_hook_sections();
    main_progress_code = 4;
    hw_init();
    main_progress_code = 5;
    main_copy_image();
    main_progress_code = 6;
    tick_timeout = 5000;
    i = tick_timeout;
    main_flags_1.bit.fuse_check = 1;
    lamp_heater_control = 0;
    main_progress_code = 7;
    hw_init_motor_chips();
    p_chip_b->outputs.bit.b2 = 0;
    p_chip_b->outputs.bit.b6 = 0;
    p_p1->DR.BIT.B4 = 0;
    p_p1->DR.BIT.B7 = 0;
    p_p2->DR.BIT.B3 = 0;
    p_chip_b_uart->mcr.bit.b0 = 1;
    p_chip_b_uart->mcr.bit.b1 = 1;
    p_p2->DR.BYTE = (hw_p2_out.b &= ~HW_P2_OUTPUTS);
    p_p1->DR.BIT.B6 = 0;
    motor_set_current(MOTOR_FEED, MOTOR_CURRENT_OFF);
    motor_set_current(MOTOR_PICK, MOTOR_CURRENT_OFF);
    motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_OFF);
    motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_OFF);
    motor_set_current(MOTOR_FB, MOTOR_CURRENT_OFF);
    p_tpu->TSTR.BIT.CST5 = 0;
    p_tpu5->TIER.BIT.TGIEA = 0;
    p_p1->DR.BIT.B3 = 0;
    while (1) {
        if (low_count >= 100) {
            low_count = 0;
            break;
        }
        if (!p_p3->PORT.BIT.B3) {
            low_count++;
        } else {
            low_count = 0;
        }
    }
    hw_init_sensors();
    if (!sensor_flags.bit.adf_cover_open) {
        p_p2->DR.BYTE = hw_p2_out.b = HW_P2_FLATBED_HEATER | HW_P2_ADF_LAMPS;
        p_p1->DR.BIT.B6 = 1;
    } else {
        p_p2->DR.BYTE = hw_p2_out.b = HW_P2_FLATBED_HEATER;
        p_p1->DR.BIT.B6 = 1;
    }
    p_chip_b_uart->mcr.bit.b0 = 0;
    p_chip_b_uart->mcr.bit.b1 = 0;
    p_tpu->TSTR.BIT.CST5 = 1;
    p_tpu5->TIER.BIT.TGIEA = 1;
    main_progress_code = 8;
    bw_motor_backing_period = 0x2f7;
    bw_motor_arm_period_seek = 0x236;
    bw_motor_arm_period = 0x11b;
    bw_motor_arm_period_return = 0x3b0;
    fb_motor_ramp_end_class_0 = 0x17b;
    fb_motor_ramp_end_class_1 = 0x17b;
    fb_motor_ramp_end_class_2 = 0x17b;
    fb_motor_ramp_end_class_3 = 0xf7;
    fb_motor_ramp_end_class_4 = 0x8b;
    fb_motor_ramp_end_class_5 = 0x3d;
    fb_motor_ramp_end_class_6 = 0xf;
    lamp_heater_interval = 0x32;
    lamp_heater_flatbed_target = 0x38;
    lamp_heater_front_target = 0x38;
    lamp_heater_back_target = 0x38;
    feed_run_current = MOTOR_CURRENT_RUN;
    pick_run_current = MOTOR_CURRENT_RUN;
    bw_motor_arm_phase.b = 0;
    bw_motor_backing_phase = 0;
    main_arm_level_pending = 0;
    pick_arm_adjust_limit = pick_arm_adjust_steps;
    main_arm_seek_picks = 9;
    bw_motor_arm_lift_limit = bw_motor_arm_lift_steps;
    bw_motor_arm_steps_once = 0;
    bw_motor_arm_position_once = 0;
    main_pick_timeout_first_steps = main_pick_timeout_base << 2;
    main_pick_timeout_steps = main_pick_timeout_base << 1;
    fb_motor_home_sync = 0;
    main_fb_home_cmd = 0;
    main_chip_b_byte_1 = 0;
    main_chip_b_byte_2 = 0;
    pick_df_armed = 0;
    bw_motor_arm_mode = 0;
    bw_motor_arm_lifted = 0;
    pick_df_active = 0;
    pick_short_gap_stopped = 0;
    main_clear_path_pending = 0;
    multifeed_check.adjust_state = 0;
    pick_next_sheet = 0;
    ramp_feed_restore_pending = 0;
    ramp_feed_restore_step = 0;
    feed_max_sheet_length = 0x13ec;
    feed_jam_steps = FEED_JAM_LIMIT_DEFAULT;
    feed_speed_step = 0;
    feed_speed_step_target = 0;
    feed_speed_step_phase = 0;
    lamp_heater_flatbed_inhibit = 0;
    lamp_heater_front_inhibit = 0;
    lamp_heater_back_inhibit = 0;
    job_state_history_index = 0;
    job_cmd_history_index = 0;
    err_history_index = 0;
    job_pick_gap_min = 0x37;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
                motor_current_tables[i][j][0] = motor_current_template[i][j][0];
                motor_current_tables[i][j][1] = motor_current_template[i][j][1];
                motor_current_tables[i][j][2] = motor_current_template[i][j][2];
                motor_current_tables[i][j][3] = motor_current_template[i][j][3];
                motor_current_tables[i][j][4] = motor_current_template[i][j][4];
                motor_current_tables[i][j][5] = motor_current_template[i][j][5];
                motor_current_tables[i][j][6] = motor_current_template[i][j][6];
                motor_current_tables[i][j][7] = motor_current_template[i][j][7];
        }
    }
    CALL_AS(void, hw_clear_mailbox, (unsigned char))(mailbox.request);
    main_progress_code = 9;
    main_handshake();
    util_lamp_clear_timers();
    cmd_test_mode = 0;
    main_progress_code = 10;
    util_log_init();
    do {
        if (mailbox.startup_state) {
        } else {
            if (mailbox.startup_request == MDC_STARTUP_INIT_MECHANICS) {
            } else {
                continue;
            }
        }
    } while (mailbox.startup_request != MDC_STARTUP_INIT_MECHANICS);
    main_progress_code = 11;
    if (mailbox.lamp_off_minutes) {
        lamp_adf_off_limit = mailbox.lamp_off_minutes;
    } else {
        lamp_adf_off_limit = 60;
    }
    lamp_adf_off_count = lamp_adf_off_limit;
    lamp_keep_warm_delay = 0x1068;
    lamp_off_limit = 0x1068;
    lamp_off_count = lamp_off_limit;
    for (i = 0; i < 5; i++) {
        if (i == 0) {
            current_level = 0x98;
            j = 1;
        } else if (i == 1) {
            current_level = 0xb0;
            j = 1;
        } else if (i == 2) {
            current_level = 0xa3;
            j = 7;
        } else if (i == 3) {
            current_level = 0xa3;
            j = 7;
        } else if (i == 4) {
            current_level = 0x70;
            j = 7;
        } else {
            current_level = 0;
            j = 7;
        }
        motor_current_tables[i][j][0] = current_level;
        motor_current_tables[i][j][1] = current_level;
        motor_current_tables[i][j][2] = current_level;
        motor_current_tables[i][j][3] = current_level;
        motor_current_tables[i][j][4] = current_level;
        motor_current_tables[i][j][5] = current_level;
        motor_current_tables[i][j][6] = current_level;
        motor_current_tables[i][j][7] = current_level;
    }
    motor_set_current(MOTOR_FEED, MOTOR_CURRENT_BOOST);
    motor_set_current(MOTOR_PICK, MOTOR_CURRENT_BOOST);
    motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_BOOST);
    motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_BOOST);
    motor_set_current(MOTOR_FB, MOTOR_CURRENT_BOOST);
    motor_enable_chips(1);
    sensor_mux_cycles = 0;
    j = 8;
    for (i = 0; i < j; i++) {
        sensor_mux_alarm_counts[i] = 0;
    }
    sensor_mux_mode = 1;
    chip_a_bit_6 = p_chip_a->inputs.b & 0x40;
    p_chip_a->inputs.b &= 0xbf;
    imprinter_probe();
    main_progress_code = 0x10;
    main_backing_homed = 0;
    if (!sensor_flags.bit.adf_cover_open) {
        i = 6;
        while (!sensor_flags.bit.adf_cover_open) {
            if (sensor_mux_cycles && sensor_mux_alarm_counts[i] == 0) {
                break;
            }
            if (sensor_mux_cycles && sensor_mux_alarm_counts[i] >= 3) {
                break;
            }
        }
        if (!sensor_flags.bit.adf_cover_open && err_code != ERR_VDCC) {
            util_backing_to_home();
            main_backing_homed = 1;
        }
    }
    main_progress_code = 0x12;
    while (1) {
        if (sensor_mux_cycles && sensor_mux_alarm_counts[0] == 0 && sensor_mux_alarm_counts[1] == 0) {
            break;
        }
        if (sensor_mux_cycles && (sensor_mux_alarm_counts[0] >= 3 || sensor_mux_alarm_counts[1] >= 3)) {
            break;
        }
    }
    if (err_code != ERR_VDCC) {
        fb_motor_blocked = 1;
        util_fb_motor_home();
    }
    main_progress_code = 0x18;
    bw_motor_arm_period_seek = 0x236;
    bw_motor_arm_period = 0x11b;
    bw_motor_arm_period_return = 0x3b0;
    util_bw_motor_load_arm_settings();
    if (mailbox.settings_aa && mailbox.settings_aa > 10 && mailbox.settings_aa < 70) {
        lamp_heater_flatbed_target = mailbox.settings_aa;
    }
    if (mailbox.settings_ab && mailbox.settings_ab > 10 && mailbox.settings_ab < 70) {
        lamp_heater_front_target = mailbox.settings_ab;
    }
    if (mailbox.settings_ac && mailbox.settings_ac > 10 && mailbox.settings_ac < 70) {
        lamp_heater_back_target = mailbox.settings_ac;
    }
    if (mailbox.settings_8f) {
        multifeed_check.threshold = mailbox.settings_8f;
    } else {
        multifeed_check.threshold = ULTRASONIC_THRESHOLD_DEFAULT;
    }
    if (mailbox.settings_8d || mailbox.settings_8e) {
        sensor_df_level = mailbox.settings_8e;
        sensor_df_range = mailbox.settings_8d;
    } else {
        sensor_df_level = 0x30;
        sensor_df_range = 0;
    }
    log_frozen = 0;
    log_limit = 0xff;
    if (lamp_keep_warm_delay) {
        keep_warm_setting = 0;
        keep_warm_max = 10;
        keep_warm_ticks = 0;
        if (keep_warm_setting == 0) {
            keep_warm_setting = 2;
        } else if (keep_warm_setting == 15) {
            keep_warm_ticks = 0x1194;
        } else if (keep_warm_setting > keep_warm_max) {
            keep_warm_setting = keep_warm_max;
        }
        if (keep_warm_ticks == 0) {
            keep_warm_ticks = keep_warm_setting * 30;
        }
        lamp_keep_warm_delay = keep_warm_ticks;
        lamp_keep_warm_on_front = 60;
        lamp_keep_warm_on_back = 60;
        lamp_keep_warm_off_front = 70;
        lamp_keep_warm_off_back = 70;
    }
    if (mailbox.board_variant != 3 && mailbox.board_variant != 0) {
        cmd_standby_flags.bit.standby_allowed = 1;
        cmd_standby_flags.bit.wake_line_high = 1;
        if (!(mailbox.settings_48 & 1)) {
            cmd_standby_flags.bit.standby_allowed = 0;
        }
    } else {
        cmd_standby_flags.bit.standby_allowed = 0;
        cmd_standby_flags.bit.wake_line_high = 0;
    }
    if (cmd_standby_flags.bit.wake_line_high) {
        p_p3->DR.BIT.B5 = 1;
    }
    while (1) {
        if (sensor_mux_mode == 2) {
            break;
        }
    }
    p_chip_a->inputs.b |= chip_a_bit_6;
    for (i = 0; i < 5; i++) {
        if (i == 0) {
            j = 1;
        } else if (i == 1) {
            j = 1;
        } else if (i == 2) {
            j = 7;
        } else if (i == 3) {
            j = 7;
        } else if (i == 4) {
            j = 7;
        } else {
            j = 7;
        }
        motor_current_tables[i][j][0] = motor_current_template[i][j][0];
        motor_current_tables[i][j][1] = motor_current_template[i][j][1];
        motor_current_tables[i][j][2] = motor_current_template[i][j][2];
        motor_current_tables[i][j][3] = motor_current_template[i][j][3];
        motor_current_tables[i][j][4] = motor_current_template[i][j][4];
        motor_current_tables[i][j][5] = motor_current_template[i][j][5];
        motor_current_tables[i][j][6] = motor_current_template[i][j][6];
        motor_current_tables[i][j][7] = motor_current_template[i][j][7];
    }
    motor_set_current(MOTOR_FEED, MOTOR_CURRENT_OFF);
    if (p_chip_a->inputs.b & 0x40) {
        motor_set_current(MOTOR_PICK, MOTOR_CURRENT_HOLD);
    } else {
        motor_set_current(MOTOR_PICK, MOTOR_CURRENT_OFF);
    }
    motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_OFF);
    main_loop();
    main_progress_code = 0xfe;
    sleep();
}
