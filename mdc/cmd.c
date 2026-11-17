#include "mdc.h"

/* cmd_phase 2 runs the posted command, 4 repeats a continuous one until the SDC sets cmd_end = 0x80,
   6 reports it done (cmd_state 8, result 0). */
void cmd_dispatch(void)
{
    switch (cmd_phase) {
    case CMD_PHASE_START:
        switch (mailbox.cmd_code) {
        case MDC_CMD_READ_MEMORY:
            cmd_read_memory(&mailbox.cmd_params);
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_WRITE_MEMORY:
            cmd_write_memory(&mailbox.cmd_params);
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_BW_MOTOR_CYCLE:
            cmd_bw_motor_cycle_state = 2;
            cmd_bw_motor_cycle(1);
            cmd_phase = CMD_PHASE_RUNNING;
            break;
        case MDC_CMD_LAMPS:
            cmd_lamps();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_MOTOR_TEST:
            cmd_motor_test(1);
            cmd_phase = CMD_PHASE_RUNNING;
            break;
        case MDC_CMD_ADJUST_DF_SENSOR:
            cmd_adjust_df_sensor();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_PICK_SHEET:
            cmd_pick_sheet();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_ADJUST_ULTRASONIC:
            cmd_adjust_ultrasonic(0);
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_NO_OPERATION:
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_IMPRINTER_INK:
            if ((mailbox.cmd_params.b[0] & 8) && !(mailbox.imprinter_flags.b & MDC_IMPRINTER_NOT_INSTALLED)) {
                imprinter_send_24(1);
                imprinter_send_26(1);
                mailbox.imprinter_ink_percent = CALL_AS(unsigned char, imprinter_get_ink_percent, (unsigned short))(1);
            }
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_BACKING_TEST:
            cmd_byte_1 = 0;
            cmd_backing_test(mailbox.cmd_params.b, 1);
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_BACKING_WHITE:
            cmd_backing(MDC_CMD_BACKING_WHITE);
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_BACKING_BLACK:
            cmd_backing(MDC_CMD_BACKING_BLACK);
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_BACKING_HOME:
            cmd_backing(MDC_CMD_BACKING_HOME);
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_BACKING_REVERSE:
            bw_motor_backing_request = BW_MOTOR_BACKING_REVERSE;
            bw_motor_backing_state = 10;
            while (bw_motor_backing_request) {
            }
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_PICK_ARM_MOVE:
            cmd_pick_arm(MDC_CMD_PICK_ARM_MOVE);
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_PICK_ARM_HOME:
            cmd_pick_arm(MDC_CMD_PICK_ARM_HOME);
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_POWER_SAVE:
            cmd_power_save();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_END_HEATER_WARMUP:
            cmd_end_heater_warmup();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_UNIT_POWER:
            cmd_unit_power();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_WARMUP_ON:
            cmd_warmup_on();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_WARMUP_OFF:
            cmd_warmup_off();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_EJECT_ALIAS:
        case MDC_CMD_EJECT:
            cmd_eject();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_FEED_RUN_TEST:
            cmd_run_test_phase = 0;
            cmd_run_test_picking = 0;
            cmd_run_test_class = mailbox.cmd_params.b[0];
            if (cmd_run_test_class > 6) {
                cmd_run_test_class = 6;
            }
            cmd_run_test_sheets = mailbox.cmd_params.b[1];
            if (cmd_run_test_sheets == 0) {
                cmd_run_test_sheets = CMD_RUN_TEST_DEFAULT_SHEETS;
            }
            cmd_run_test_pause_ticks = mailbox.cmd_params.b[2] * CMD_RUN_TEST_PAUSE_UNIT_TICKS;
            tick_timeout = 0;
            cmd_feed_run_test(1);
            cmd_phase = CMD_PHASE_RUNNING;
            break;
        case MDC_CMD_BW_MOTOR_RUN_TEST:
            cmd_run_test_phase = 0;
            tick_timeout = 0;
            cmd_bw_motor_run_test(1);
            cmd_phase = CMD_PHASE_RUNNING;
            break;
        case MDC_CMD_FEED_FLATBED_RUN_TEST:
            cmd_run_test_phase = 0;
            cmd_run_test_picking = 0;
            cmd_run_test_sheets = mailbox.cmd_params.b[0];
            cmd_run_test_pause_ticks = mailbox.cmd_params.b[2] * CMD_RUN_TEST_PAUSE_UNIT_TICKS;
            tick_timeout = 0;
            cmd_feed_flatbed_run_test(1);
            cmd_phase = CMD_PHASE_RUNNING;
            break;
        case MDC_CMD_DF_TEST_ON:
            if (mailbox.cmd_params.b[0] & MDC_TEST_MODE_PARAM) {
                cmd_test_mode_on();
            }
            sensor_df_on();
            sensor_df_test = 1;
            cmd_delay = CMD_SETTLE_DELAY_TICKS;
            while (cmd_delay) {
            }
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_DF_TEST_OFF:
            if (mailbox.cmd_params.b[0] & MDC_TEST_MODE_PARAM) {
                cmd_test_mode_off();
            }
            sensor_df_off();
            sensor_df_test = 0;
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_ULTRASONIC_MONITOR_ON:
            if (mailbox.cmd_params.b[0] & MDC_TEST_MODE_PARAM) {
                cmd_test_mode_on();
            }
            sensor_us_monitor = 1;
            feed_state = FEED_STATE_ULTRASONIC_PAPER;
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_ULTRASONIC_MONITOR_OFF:
            if (mailbox.cmd_params.b[0] & MDC_TEST_MODE_PARAM) {
                cmd_test_mode_off();
            }
            sensor_us_monitor = 0;
            cmd_delay = CMD_FEED_WAIT_TICKS;
            while (feed_state != FEED_STATE_ULTRASONIC_DONE) {
                if (cmd_delay == 0) {
                    cmd_phase = CMD_PHASE_DONE;
                    break;
                }
            }
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_CLEAR_OVERLAP_PATTERNS:
            cmd_clear_overlap_patterns();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_READ_OVERLAP_PATTERNS:
            cmd_read_overlap_patterns();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_WRITE_OVERLAP_PATTERNS:
            cmd_write_overlap_patterns();
            cmd_phase = CMD_PHASE_DONE;
            break;
        case MDC_CMD_DUMP_LOG:
            cmd_dump_log();
            cmd_phase = CMD_PHASE_DONE;
            break;
        default:
            cmd_phase = CMD_PHASE_DONE;
            break;
        }
        break;
    case CMD_PHASE_RUNNING:
        if (mailbox.cmd_end != MDC_CMD_END) {
            if (mailbox.cmd_code == MDC_CMD_BW_MOTOR_CYCLE) {
                cmd_bw_motor_cycle(1);
            } else if (mailbox.cmd_code == MDC_CMD_BACKING_TEST) {
                cmd_backing_test(mailbox.cmd_params.b, 1);
            } else if (mailbox.cmd_code == MDC_CMD_FEED_RUN_TEST) {
                cmd_feed_run_test(1);
            } else if (mailbox.cmd_code == MDC_CMD_BW_MOTOR_RUN_TEST) {
                cmd_bw_motor_run_test(1);
            } else if (mailbox.cmd_code == MDC_CMD_FEED_FLATBED_RUN_TEST) {
                cmd_feed_flatbed_run_test(1);
            }
        } else {
            if (mailbox.cmd_code == MDC_CMD_BW_MOTOR_CYCLE) {
                cmd_bw_motor_cycle(0);
            } else if (mailbox.cmd_code == MDC_CMD_MOTOR_TEST) {
                cmd_motor_test(0);
            } else if (mailbox.cmd_code == MDC_CMD_BACKING_TEST) {
                cmd_backing_test(mailbox.cmd_params.b, 0);
            } else if (mailbox.cmd_code == MDC_CMD_FEED_RUN_TEST) {
                cmd_feed_run_test(0);
            } else if (mailbox.cmd_code == MDC_CMD_BW_MOTOR_RUN_TEST) {
                cmd_bw_motor_run_test(0);
            } else if (mailbox.cmd_code == MDC_CMD_FEED_FLATBED_RUN_TEST) {
                cmd_feed_flatbed_run_test(0);
            }
            mailbox.cmd_end = 0;
            cmd_phase = CMD_PHASE_DONE;
        }
        break;
    case CMD_PHASE_DONE:
        if (err_flags.b == 0 && mailbox.cmd_state == MDC_CMD_STATE_RUNNING) {
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
        }
        cmd_phase = CMD_PHASE_IDLE;
        break;
    }
}

void cmd_read_memory(union mailbox_cmd_params *params)
{
    eepmov((char *)params->memory.data, (char *)params->memory.address, params->memory.count > 8 ? 8 : params->memory.count);
}

void cmd_write_memory(union mailbox_cmd_params *params)
{
    unsigned char *dst;
    unsigned char *src;
    unsigned short count;

    count = params->memory.count > 8 ? 8 : params->memory.count;
    dst = params->memory.address;
    src = params->memory.data;
    for (; count != 0; count--) {
        if (dst < (unsigned char *)0x20000 || dst == &mailbox.cmd_result
            || (dst >= (unsigned char *)0xe0000 && dst < (unsigned char *)0xf8000)
            || (dst >= (unsigned char *)0xffd10 && dst < (unsigned char *)0xfff10)) {
            dst++;
            src++;
        } else {
            *dst++ = *src++;
        }
    }
}

void cmd_bw_motor_cycle(unsigned char on)
{
    unsigned char done;

    done = 0;
    do {
        switch (cmd_bw_motor_cycle_state) {
        case 2:
            feed_pick_edge_state = 4;
            cmd_bw_motor_cycle_state = 4;
            break;
        case 4:
            if (bw_motor_arm_request == 0) {
                set_imask_exr(7);
                bw_motor_arm_request = BW_MOTOR_ARM_TO_LEVEL;
                set_imask_exr(0);
                cmd_bw_motor_cycle_state = 6;
            }
            break;
        case 6:
            if (bw_motor_arm_request == 0) {
                set_imask_exr(7);
                bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                set_imask_exr(0);
                cmd_bw_motor_cycle_state = 0xe;
            }
            break;
        case 0xe:
            if (bw_motor_backing_request == 0) {
                set_imask_exr(7);
                bw_motor_backing_request = BW_MOTOR_BACKING_BLACK;
                set_imask_exr(0);
                cmd_bw_motor_cycle_state = 0x10;
            }
            break;
        case 0x10:
            if (bw_motor_backing_request == 0) {
                set_imask_exr(7);
                bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
                set_imask_exr(0);
                cmd_bw_motor_cycle_state = 4;
            }
            break;
        }
        if (on == 1) {
            break;
        }
    } while (done == 0);
    if (on == 0) {
        p_p2->DR.BYTE = (hw_p2_out.b &= ~HW_P2_ADF_LAMPS);
        util_backing_to_home();
        while (feed_ramp_state || pick_ramp_state) {
        }
        feed_pick_edge_state = 0;
    }
}

void cmd_lamps(void)
{
    union mailbox_cmd_params *params;

    params = &mailbox.cmd_params;
    if (params->b[1]) {
        cmd_test_mode_on();
    } else {
        cmd_test_mode_off();
    }
    if (mailbox.lamp_off_minutes) {
        lamp_adf_off_limit = mailbox.lamp_off_minutes;
    } else {
        lamp_adf_off_limit = 60;
    }
    if ((params->b[0] & 0x80)) {
        if (!sensor_flags.bit.adf_cover_open || params->b[1]) {
            hw_p2_out.bit.adf_front_lamp = 1;
            p_p2->DR.BYTE = hw_p2_out.b;
            lamp_adf_off_count = lamp_adf_off_limit;
        }
    } else {
        hw_p2_out.bit.adf_front_lamp = 0;
        p_p2->DR.BYTE = hw_p2_out.b;
    }
    if ((params->b[2] & 0x40)) {
        if (!sensor_flags.bit.adf_cover_open || params->b[1]) {
            hw_p2_out.bit.adf_back_lamp = 1;
            p_p2->DR.BYTE = hw_p2_out.b;
            lamp_adf_off_count = lamp_adf_off_limit;
        }
    } else {
        hw_p2_out.bit.adf_back_lamp = 0;
        p_p2->DR.BYTE = hw_p2_out.b;
    }
    if ((params->b[4] & 0x20)) {
        p_p1->DR.BIT.B6 = 1;
        lamp_adf_off_count = lamp_adf_off_limit;
    } else {
        p_p1->DR.BIT.B6 = 0;
    }
}

void cmd_motor_test(unsigned char on)
{
    unsigned char speed_class;
    union mailbox_cmd_params *params;

    params = &mailbox.cmd_params;
    speed_class = params->b[2];
    if (params->b[0] == MDC_MOTOR_TEST_FEED) {
        if (on) {
            cmd_test_mode_on();
            feed_pick_edge_state = 4;
            feed_top_edge_state = 4;
            if (speed_class > 6) {
                speed_class = 6;
            }
            feed_mode.b &= ~FEED_MODE_HALF_STEP;
            feed_ramp_steps = feed_ramp_steps_by_class[speed_class];
            job_feed_speed_class = speed_class;
            feed_action = MOTOR_ACTION_START;
            while (feed_ramp_state != RAMP_STATE_CONSTANT) {
            }
            cmd_delay = cmd_settle_ticks;
            while (cmd_delay) {
            }
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_RUN);
            feed_current_mode = CURRENT_MODE_RUN;
        } else {
            feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            while (feed_action && !err_flags.b) {
            }
            feed_pick_edge_state = 0;
            feed_top_edge_state = 0;
            cmd_test_mode_off();
        }
    } else if (params->b[0] == MDC_MOTOR_TEST_PICK) {
        if (on) {
            cmd_test_mode_on();
            if (speed_class > 5) {
                speed_class = 5;
            }
            pick_ramp_steps = pick_ramp_steps_by_class[speed_class];
            pick_speed_select = 1;
            if (speed_class == 5) {
                pick_run_current = MOTOR_CURRENT_RUN_CLASS_5;
            } else if (speed_class == 4) {
                pick_run_current = MOTOR_CURRENT_RUN_CLASS_4;
            } else {
                pick_run_current = MOTOR_CURRENT_RUN;
            }
            pick_action = MOTOR_ACTION_START;
        } else {
            pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            while (pick_action && !err_flags.b) {
            }
            cmd_test_mode_off();
        }
    } else if (params->b[0] == MDC_MOTOR_TEST_PICK_ARM) {
        if (on) {
            cmd_test_mode_on();
            set_imask_exr(7);
            bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_HOME;
            bw_motor_arm_request = BW_MOTOR_ARM_TEST;
            set_imask_exr(0);
        } else {
            set_imask_exr(7);
            bw_motor_arm_request = BW_MOTOR_ARM_HOME;
            set_imask_exr(0);
            while (bw_motor_arm_request && !err_flags.b) {
            }
            cmd_test_mode_off();
        }
    } else if (params->b[0] == MDC_MOTOR_TEST_BACKING) {
        if (on) {
            bw_motor_backing_request = BW_MOTOR_BACKING_TEST;
        } else {
            while (bw_motor_backing_request && !err_flags.b) {
            }
        }
    } else if (params->b[0] == MDC_MOTOR_TEST_FB_MOTOR) {
        if (on) {
            if (!sensor_flags.bit.fb_home) {
                fb_motor_request = FB_MOTOR_HOME;
                fb_motor_state = FB_MOTOR_STATE_START;
            } else {
                fb_motor_scan_end = FB_MOTOR_TEST_SCAN_END;
                fb_motor_done_position = FB_MOTOR_DONE_POSITION;
                fb_motor_scan_ramp_end = fb_motor_ramp_end_by_class[speed_class];
                p_p1->DR.BIT.B6 = 1;
                fb_motor_request = FB_MOTOR_TEST;
            }
        } else {
            fb_motor_request = FB_MOTOR_STOP;
            while (fb_motor_request && !err_flags.b) {
            }
        }
    } else if (params->b[0] == MDC_MOTOR_TEST_PICK_AND_FEED) {
        if (on) {
            cmd_test_mode_on();
            feed_jam_steps = FEED_JAM_DISABLED;
            if (speed_class > 5) {
                speed_class = 5;
            }
            pick_ramp_steps = pick_ramp_steps_by_class[speed_class];
            pick_speed_select = 1;
            if (speed_class == 5) {
                pick_run_current = MOTOR_CURRENT_RUN_CLASS_5;
            } else if (speed_class == 4) {
                pick_run_current = MOTOR_CURRENT_RUN_CLASS_4;
            } else {
                pick_run_current = MOTOR_CURRENT_RUN;
            }
            pick_action = MOTOR_ACTION_START;
            cmd_delay = CMD_SETTLE_DELAY_TICKS;
            while (cmd_delay) {
            }
            feed_pick_edge_state = 4;
            feed_top_edge_state = 4;
            if (speed_class > 6) {
                speed_class = 6;
            }
            feed_mode.b &= ~FEED_MODE_HALF_STEP;
            feed_ramp_steps = feed_ramp_steps_by_class[speed_class];
            job_feed_speed_class = speed_class;
            feed_action = MOTOR_ACTION_START;
            cmd_delay = CMD_FEED_WAIT_TICKS;
            tick_timeout = 1500;
            while (feed_ramp_state != RAMP_STATE_CONSTANT) {
                if (tick_timeout == 0) {
                    tick_timeout = 1500;
                    feed_action = MOTOR_ACTION_START;
                }
                if (cmd_delay == 0) {
                    break;
                }
            }
            cmd_delay = cmd_settle_ticks;
            while (cmd_delay) {
            }
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_RUN);
            feed_current_mode = CURRENT_MODE_RUN;
        } else {
            pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            while (pick_action && !err_flags.b) {
            }
            feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            while (feed_action && !err_flags.b) {
            }
            feed_pick_edge_state = 0;
            feed_top_edge_state = 0;
            feed_jam_steps = FEED_JAM_LIMIT_DEFAULT;
            cmd_test_mode_off();
        }
    }
}

void cmd_adjust_df_sensor(void)
{
    unsigned char level;
    unsigned char range;

    sensor_df_level = 0;
    sensor_df_range = 0;
    set_imask_exr(7);
    sensor_df_on();
    set_imask_exr(0);
    level = 0;
    range = 0;
    set_imask_exr(7);
    p_da->DR1 = level;
    p_chip_b->outputs.b &= 0xfc;
    set_imask_exr(0);
    while (1) {
        set_imask_exr(7);
        p_da->DR1 = level++;
        set_imask_exr(0);
        tick_timeout = 1;
        while (tick_timeout) {
        }
        if ((unsigned char)(p_ad->DRB >> 8) > SENSOR_DF_ADJUST_LEVEL) {
            mailbox.df_adjust_flags |= MDC_DF_ADJUST_DF_OK;
            break;
        }
        if (level == 0xff) {
            if (range == 0) {
                set_imask_exr(7);
                p_chip_b->outputs.b &= 0xfc;
                p_chip_b->outputs.b |= 1;
                set_imask_exr(0);
            } else if (range == 1) {
                set_imask_exr(7);
                p_chip_b->outputs.b &= 0xfc;
                p_chip_b->outputs.b |= 2;
                set_imask_exr(0);
            } else if (range == 2) {
                set_imask_exr(7);
                p_chip_b->outputs.b &= 0xfc;
                p_chip_b->outputs.b |= 3;
                set_imask_exr(0);
            } else {
                mailbox.df_adjust_flags &= ~MDC_DF_ADJUST_DF_OK;
                break;
            }
            level = 0;
            range++;
        }
    }
    sensor_df_level = level;
    sensor_df_range = range;
    if (mailbox.df_adjust_flags & MDC_DF_ADJUST_DF_OK) {
        mailbox.settings_8e = sensor_df_level;
        mailbox.settings_8d = sensor_df_range;
    }
    set_imask_exr(7);
    sensor_df_off();
    set_imask_exr(0);
}

void cmd_pick_sheet(void)
{
    unsigned char unused;
    unsigned char state;

    unused = 0;
    state = 2;
    while (err_flags.b == 0 && state != 0) {
        switch (state) {
        case 2:
            if (sensor_flags.bit.no_paper) {
                err_flags.b |= ERR_FLAG_DOCUMENT;
                err_code = ERR_NO_PAPER;
                state = 0;
                break;
            }
            state = 4;
            break;
        case 4:
            if (sensor_flags.bit.no_paper) {
                err_flags.b |= ERR_FLAG_DOCUMENT;
                err_code = ERR_NO_PAPER;
                state = 0;
                break;
            } else {
                pick_ramp_steps = pick_ramp_steps_slow;
                pick_speed_select = 0;
                pick_step_count = 0;
                pick_action = MOTOR_ACTION_START;
                state = 6;
            }
            break;
        case 6:
            if (sensor_flags.bit.pick) {
                pick_step_count = 0;
                state = 8;
            } else if (pick_step_count > pick_timeout_steps) {
                err_flags.b |= ERR_FLAG_DOCUMENT;
                err_code = ERR_PICK;
                state = 0;
            }
            break;
        case 8:
            if (pick_step_count >= cmd_pick_overrun_steps) {
                pick_action = MOTOR_ACTION_STOP;
                state = 0xa;
            }
            break;
        case 0xa:
            if (pick_ramp_state == RAMP_STATE_STOPPED) {
                state = 0xc;
            }
            break;
        case 0xc:
            state = 0xe;
            break;
        case 0xe:
            cmd_eject();
            state = 0;
            break;
        }
    }
}

void cmd_adjust_ultrasonic(unsigned char on)
{
    unsigned char state;
    unsigned char failed;

    state = 1;
    mailbox.cmd_params.b[0] = mailbox.settings_8f;
    sensor_df_test = 0;
    set_imask_exr(7);
    sensor_stop_ultrasonic();
    feed_mode.b &= ~FEED_MODE_BIT_1;
    set_imask_exr(0);
    while (err_flags.b == 0 && state != 0) {
        switch (state) {
        case 1:
            if (on) {
                state = 2;
                break;
            }
            if (sensor_flags.bit.no_paper) {
                err_flags.b |= ERR_FLAG_DOCUMENT;
                err_code = ERR_NO_PAPER;
                state = 0;
                break;
            }
            state = 2;
            break;
        case 2:
            if (on == 0) {
                feed_pick_edge_state = 4;
                feed_top_edge_state = 4;
                feed_ramp_steps = feed_ramp_steps_default;
                feed_action = MOTOR_ACTION_START;
                bw_motor_arm_idle_ticks = 0;
                bw_motor_arm_request = BW_MOTOR_ARM_TO_LEVEL;
            }
            feed_state = FEED_STATE_ULTRASONIC_IDLE;
            cmd_delay = CMD_FEED_WAIT_TICKS;
            state = 3;
            break;
        case 3:
            if (feed_state == FEED_STATE_ULTRASONIC_DONE) {
                if (on) {
                    state = 7;
                } else {
                    state = 4;
                }
            } else if (cmd_delay == 0) {
                err_flags.b |= ERR_FLAG_DEVICE;
                err_code = ERR_ULTRASONIC_SENSOR;
                state = 0;
            }
            break;
        case 4:
            if (bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL) {
                if (sensor_flags.bit.no_paper) {
                    err_flags.b |= ERR_FLAG_DOCUMENT;
                    err_code = ERR_NO_PAPER;
                    state = 0;
                    break;
                }
                pick_ramp_steps = pick_ramp_steps_slow;
                pick_step_count = 0;
                pick_timeout_steps = main_pick_timeout_first_steps;
                pick_action = MOTOR_ACTION_START;
                state = 5;
            }
            break;
        case 5:
            if (sensor_flags.bit.pick) {
                pick_step_count = 0;
                state = 6;
            } else if (pick_step_count > pick_timeout_steps) {
                err_flags.b |= ERR_FLAG_DOCUMENT;
                err_code = ERR_PICK;
                state = 0;
            }
            break;
        case 6:
            if (sensor_flags.bit.top) {
                pick_action = MOTOR_ACTION_STOP;
                feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                feed_wait_steps = 0;
                state = 7;
            } else if (pick_step_count > pick_top_timeout_steps) {
                if (--pick_top_retry_limit != 0) {
                    pick_step_count = 0;
                    pick_top_timeout_steps = pick_top_timeout;
                    break;
                }
                err_flags.b |= ERR_FLAG_DOCUMENT;
                err_code = ERR_PICK_SENSOR_JAM;
                feed_action = MOTOR_ACTION_STOP;
                state = 0;
            }
            break;
        case 7:
            if (feed_ramp_state == RAMP_STATE_STOPPED) {
                feed_state = FEED_STATE_ULTRASONIC_PAPER;
                cmd_delay = CMD_FEED_WAIT_TICKS;
                state = 8;
            }
            break;
        case 8:
            if (feed_state == FEED_STATE_ULTRASONIC_DONE) {
                feed_action = MOTOR_ACTION_STOP;
                if (on) {
                    failed = cmd_adjust_ultrasonic_idle();
                } else {
                    failed = cmd_adjust_ultrasonic_result();
                    cmd_eject();
                }
                p_chip_a_motor->period_1_high.f_0_8.f = 0xff;
                p_chip_a_motor->period_1_low.f_0_8.f = 0xff;
                if (failed) {
                    err_flags.b |= ERR_FLAG_DEVICE;
                    err_code = ERR_ULTRASONIC_SENSOR;
                    state = 0;
                }
                state = 0;
            } else if (cmd_delay == 0) {
                if (on == 0) {
                    cmd_eject();
                }
                err_flags.b |= ERR_FLAG_DEVICE;
                err_code = ERR_ULTRASONIC_SENSOR;
                state = 0;
            }
            break;
        }
    }
}

void cmd_backing_test(unsigned char *params, unsigned char on)
{
    if (params[2] == 0) {
        set_imask_exr(7);
        if (params[3] == 0) {
            bw_motor_backing_request = BW_MOTOR_BACKING_BLACK;
        } else {
            bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
        }
        set_imask_exr(0);
    } else if (params[2] == 1) {
        params[15] = 0;
        do {
            switch (params[15]) {
            case 0:
                set_imask_exr(7);
                bw_motor_backing_state = 0;
                bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
                set_imask_exr(0);
                params[15] = 1;
                break;
            case 1:
                if (bw_motor_backing_request == 0) {
                    set_imask_exr(7);
                    bw_motor_backing_request = BW_MOTOR_BACKING_BLACK;
                    set_imask_exr(0);
                    params[15] = 2;
                }
                break;
            case 2:
                if (bw_motor_backing_request == 0) {
                    set_imask_exr(7);
                    bw_motor_backing_request = BW_MOTOR_BACKING_TEST;
                    set_imask_exr(0);
                    params[15] = 3;
                }
                break;
            case 3:
                if (bw_motor_backing_request == 0) {
                    params[15] = 2;
                }
                break;
            }
            if (params[16] & 0x80) {
                set_imask_exr(7);
                bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
                set_imask_exr(0);
                cmd_timeout = CMD_TIMEOUT_TICKS;
                while (bw_motor_backing_state != 5 && err_flags.b == 0) {
                    if (cmd_timeout == 0) {
                        break;
                    }
                }
                params[16] = 0;
                break;
            }
        } while (err_flags.b == 0);
    }
    while (err_flags.b == 0 && bw_motor_backing_request != 0) {
    }
}

void cmd_backing(unsigned char code)
{
    if (code == MDC_CMD_BACKING_WHITE) {
        set_imask_exr(7);
        bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
        set_imask_exr(0);
        cmd_timeout = CMD_TIMEOUT_TICKS;
        while (bw_motor_backing_request != 0 && err_flags.b == 0) {
            if (cmd_timeout == 0) {
                bw_motor_record_backing(0x90);
                err_flags.b |= ERR_FLAG_DEVICE;
                err_saved_flags = err_flags.b;
                err_saved_code = err_code = ERR_BACKING;
                break;
            }
        }
    } else if (code == MDC_CMD_BACKING_BLACK) {
        set_imask_exr(7);
        bw_motor_backing_request = BW_MOTOR_BACKING_BLACK;
        set_imask_exr(0);
        cmd_timeout = CMD_TIMEOUT_TICKS;
        while (bw_motor_backing_request != 0 && err_flags.b == 0) {
            if (cmd_timeout == 0) {
                bw_motor_record_backing(0xa0);
                err_flags.b |= ERR_FLAG_DEVICE;
                err_saved_flags = err_flags.b;
                err_saved_code = err_code = ERR_BACKING;
                break;
            }
        }
    } else if (code == MDC_CMD_BACKING_HOME) {
        util_backing_to_home();
    }
}

void cmd_pick_arm(unsigned char code)
{
    if (code == MDC_CMD_PICK_ARM_MOVE) {
        if (mailbox.cmd_params.b[0] == 0) {
            bw_motor_arm_steps_once = 0;
            bw_motor_arm_position_once = 0;
        } else if (mailbox.cmd_params.b[0] == 1) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_0;
            bw_motor_arm_position_once = bw_motor_arm_level_0;
        } else if (mailbox.cmd_params.b[0] == 2) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_1;
            bw_motor_arm_position_once = bw_motor_arm_level_1;
        } else if (mailbox.cmd_params.b[0] == 3) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_2;
            bw_motor_arm_position_once = bw_motor_arm_level_2;
        } else if (mailbox.cmd_params.b[0] == 4) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_3;
            bw_motor_arm_position_once = bw_motor_arm_level_3;
        } else if (mailbox.cmd_params.b[0] == 5) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_4;
            bw_motor_arm_position_once = bw_motor_arm_level_4;
        } else if (mailbox.cmd_params.b[0] == 6) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_5;
            bw_motor_arm_position_once = bw_motor_arm_level_5;
        } else if (mailbox.cmd_params.b[0] == 7) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_6;
            bw_motor_arm_position_once = bw_motor_arm_level_6;
        } else if (mailbox.cmd_params.b[0] == 8) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_7;
            bw_motor_arm_position_once = bw_motor_arm_level_7;
        } else if (mailbox.cmd_params.b[0] == 9) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_8;
            bw_motor_arm_position_once = bw_motor_arm_level_8;
        } else if (mailbox.cmd_params.b[0] == 10) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_9;
            bw_motor_arm_position_once = bw_motor_arm_level_9;
        } else if (mailbox.cmd_params.b[0] == 11) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_10;
            bw_motor_arm_position_once = bw_motor_arm_level_10;
        } else if (mailbox.cmd_params.b[0] == 12) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_11;
            bw_motor_arm_position_once = bw_motor_arm_level_11;
        } else if (mailbox.cmd_params.b[0] == 13) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_12;
            bw_motor_arm_position_once = bw_motor_arm_level_12;
        } else if (mailbox.cmd_params.b[0] == 14) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_13;
            bw_motor_arm_position_once = bw_motor_arm_level_13;
        } else if (mailbox.cmd_params.b[0] == 15) {
            bw_motor_arm_steps_once = cmd_arm_base_steps + bw_motor_arm_level_14;
            bw_motor_arm_position_once = bw_motor_arm_level_14;
        } else {
            bw_motor_arm_steps_once = 0;
            bw_motor_arm_position_once = 0;
        }
        set_imask_exr(7);
        bw_motor_arm_request = BW_MOTOR_ARM_TO_LEVEL;
        set_imask_exr(0);
        cmd_timeout = CMD_TIMEOUT_TICKS;
        while (bw_motor_arm_request != 0 && err_flags.b == 0) {
            if (cmd_timeout == 0) {
                err_flags.b |= ERR_FLAG_DEVICE;
                err_saved_flags = err_flags.b;
                err_saved_code = err_code = ERR_PICK_ARM;
                break;
            }
        }
    } else if (code == MDC_CMD_PICK_ARM_HOME) {
        set_imask_exr(7);
        bw_motor_arm_request = BW_MOTOR_ARM_HOME;
        set_imask_exr(0);
        cmd_timeout = CMD_TIMEOUT_TICKS;
        while (bw_motor_arm_request != 0 && err_flags.b == 0) {
            if (cmd_timeout == 0) {
                err_flags.b |= ERR_FLAG_DEVICE;
                err_saved_flags = err_flags.b;
                err_saved_code = err_code = ERR_PICK_ARM;
                break;
            }
        }
    }
}

void cmd_power_save(void)
{
    short last_fuse_low;
    short unused_frame;
    unsigned char chip_b_outputs;
    unsigned char cover_state;
    unsigned char cover_count;
    unsigned char standby_setting;
    short i;
    short wake_level;
    short idle_level;

    standby_setting = mailbox.settings_48;
    if (mailbox.board_variant != 3 && mailbox.board_variant != 0 && (standby_setting & 1)) {
        cmd_standby_flags.bit.standby_allowed = 1;
    } else {
        cmd_standby_flags.bit.standby_allowed = 0;
    }
    if (cmd_standby_flags.bit.wake_line_high && !p_p3->PORT.BIT.B4) {
        cmd_standby_flags.bit.wake_line_high = 0;
        p_p3->DR.BIT.B5 = 0;
    } else if (!cmd_standby_flags.bit.wake_line_high && p_p3->PORT.BIT.B4) {
        cmd_standby_flags.bit.wake_line_high = 1;
        p_p3->DR.BIT.B5 = 1;
    }
    if (cmd_standby_flags.bit.wake_line_high) {
        wake_level = 0;
        idle_level = 1;
    } else {
        wake_level = 1;
        idle_level = 0;
    }
    log_hook_begin(0);
    p_log_buf[log_index] = LOG_POWER_SAVE_ENTER;
    log_advance();
    p_log_buf[log_index] = LOG_POWER_SAVE_ENTER;
    log_advance();
    p_log_buf[log_index] = LOG_END;
    log_end();
    tick_timeout = 1500;
    while (bw_motor_backing_request != 0) {
        if (tick_timeout == 0) {
            bw_motor_backing_request = 0;
            break;
        }
    }
    if (bw_motor_backing_state != 0 && bw_motor_backing_state != 5 && tick_timeout != 0) {
        bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
        bw_motor_backing_quiet = 1;
    }
    main_flags_1.bit.fuse_check = 0;
    lamp_heater_flatbed_on = 0;
    lamp_heater_front_on = 0;
    lamp_heater_back_on = 0;
    sensor_mux_enable = 0;
    chip_b_outputs = p_chip_b->outputs.b & 0xbb;
    p_chip_b->outputs.b = chip_b_outputs;
    tick_timeout = 50;
    while (tick_timeout) {
    }
    lamp_heater_control = 0;
    if (bw_motor_backing_request) {
        tick_timeout = TICKS_PER_SECOND;
        while (bw_motor_backing_request) {
            if (tick_timeout == 0) {
                bw_motor_backing_request = 0;
                break;
            }
        }
    }
    bw_motor_backing_quiet = 0;
    set_imask_exr(7);
    p_p2->DR.BYTE = hw_p2_out.b = 0;
    p_p1->DR.BIT.B6 = 0;
    set_imask_exr(7);
    p_p2->DR.BYTE = hw_p2_out.b = 0;
    motor_set_current(MOTOR_FEED, MOTOR_CURRENT_OFF);
    motor_set_current(MOTOR_PICK, MOTOR_CURRENT_OFF);
    motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_OFF);
    motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_OFF);
    motor_set_current(MOTOR_FB, MOTOR_CURRENT_OFF);
    motor_enable_chips(0);
    p_da->CR.BYTE = 0;
    p_da->DR0 = 0;
    p_da->DR1 = 0;
    p_tpu5->TIER.BIT.TGIEA = 0;
    p_intc->IPRA.BYTE = 0;
    p_intc->IPRB.BYTE = 0;
    p_intc->IPRC.BYTE = 0;
    p_intc->IPRD.BYTE = 0;
    p_intc->IPRE.BYTE = 0;
    p_intc->IPRF.BYTE = 0;
    p_intc->IPRG.BYTE = 0;
    p_intc->IPRH.BYTE = 0;
    p_intc->IPRI.BYTE = 0;
    p_intc->IPRJ.BYTE = 0;
    p_intc->IPRK.BYTE = 0;
    p_intc->IER.BYTE = 0;
    while (p_intc->ISR.BYTE & 0x81) {
        p_intc->ISR.BYTE = 0;
    }
    if (cmd_standby_flags.bit.standby_allowed) {
        if (!cmd_standby_flags.bit.wake_line_high) {
            p_intc->ISCR.BIT.IRQ4SC = 2;
        }
        i = 0;
        while (p_intc->ISR.BYTE & 0x10) {
            p_intc->ISR.BYTE = 0;
            if (i++ > 10) {
                break;
            }
        }
        p_intc->IPRB.BYTE = 7;
        p_intc->IER.BIT.IRQ4E = 1;
        isr_irq4_count = 0;
    }
    p_chip_b->outputs.bit.b2 = 0;
    p_chip_b->outputs.bit.b6 = 0;
    p_p1->DR.BIT.B4 = 0;
    p_p1->DR.BIT.B7 = 0;
    p_p2->DR.BIT.B3 = 0;
    p_chip_b_uart->mcr.bit.b0 = 1;
    p_chip_b_uart->mcr.bit.b1 = 1;
    NOPROTO(feed_set_cmd_state)(MDC_CMD_STATE_ENDED);
    while (p_p1->PORT.BIT.B0) {
        nop();
        nop();
        nop();
        nop();
        nop();
        if (console_command[1] == 0x60 && console_command[2] != 0) {
            break;
        }
    }
    if (cmd_standby_flags.bit.standby_allowed) {
        tick_seconds = 0;
        p_sbycr->BIT.IRQ37S = 1;
        p_sbycr->BIT.SSBY = 1;
        set_imask_exr(0);
        sleep();
        cover_state = tick_document_cover_state;
        cover_count = sensor_document_cover_count;
    } else {
        set_imask_exr(0);
        cover_state = tick_document_cover_state;
        cover_count = sensor_document_cover_count;
    }
    do {
        p_p2->DR.BYTE = (hw_p2_out.b &= ~HW_P2_OUTPUTS);
        tick_seconds = 0;
        if (cover_state == 0) {
            if (sensor_flags.bit.document_cover_open) {
                cover_state = 2;
            }
        } else if (cover_state == 2) {
            if (!sensor_flags.bit.document_cover_open) {
                cover_state = 0;
                job_wake_flags.bit.wake_event = 1;
            }
        }
        if (p_p1->PORT.BIT.B5) {
            if (cover_count < SENSOR_DEBOUNCE_MAX) {
                cover_count++;
            }
        } else if (cover_count) {
            cover_count--;
        }
        if (cover_count > SENSOR_DEBOUNCE_ON) {
            sensor_flags.bit.document_cover_open = 1;
        } else {
            sensor_flags.bit.document_cover_open = 0;
        }
    } while (p_p3->PORT.BIT.B4 != wake_level);
    if (cmd_standby_flags.bit.standby_allowed && (mailbox.wake_flags & 1)) {
        job_wake_flags.bit.wake_event = 1;
    }
    p_sbycr->BIT.SSBY = 0;
    hw_init();
    p_tpu->TSTR.BIT.CST5 = 1;
    p_tpu5->TIER.BIT.TGIEA = 1;
    p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_HEATERS);
    tick_timeout = 5000;
    last_fuse_low = tick_timeout;
    while (!p_p1->PORT.BIT.B0 || (unsigned short)(last_fuse_low - tick_timeout) < 0x32) {
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
        if (tick_timeout == 0) {
            err_flags.b |= ERR_FLAG_DEVICE;
            err_saved_flags = err_flags.b;
            err_saved_code = err_code = ERR_LAMP_FUSE;
            break;
        }
        if (!p_p1->PORT.BIT.B0) {
            last_fuse_low = tick_timeout;
        }
    }
    if (tick_timeout) {
        main_flags_1.bit.fuse_check = 1;
    }
    if (!sensor_flags.bit.adf_cover_open) {
        p_p2->DR.BYTE = hw_p2_out.b = HW_P2_HEATERS | HW_P2_ADF_LAMPS;
        p_p1->DR.BIT.B6 = 1;
    } else {
        p_p2->DR.BYTE = hw_p2_out.b = HW_P2_HEATERS;
        p_p1->DR.BIT.B6 = 1;
    }
    cmd_test_mode = 0;
    hw_init_motor_chips();
    motor_enable_chips(1);
    if (!sensor_flags.bit.adf_cover_open) {
        util_backing_to_home();
        main_backing_homed = 1;
    } else {
        main_backing_homed = 0;
    }
    p_p3->DR.BIT.B5 = wake_level;
    p_chip_b_uart->mcr.bit.b0 = 0;
    p_chip_b_uart->mcr.bit.b1 = 0;
    util_lamp_clear_timers();
    log_hook_begin(0);
    p_log_buf[log_index] = LOG_POWER_SAVE_LEAVE;
    log_advance();
    p_log_buf[log_index] = LOG_POWER_SAVE_LEAVE;
    log_advance();
    p_log_buf[log_index] = LOG_END;
    log_end();
    do {
    } while (p_p3->PORT.BIT.B4 == wake_level);
    p_p3->DR.BIT.B5 = idle_level;
    mailbox.mechanics_init_mode = 6;
    main_init_state = 4;
    ramp_load();
    lamp_adf_off_count = lamp_adf_off_limit;
    lamp_off_count = lamp_off_limit;
    main_progress_code &= 0xc0;
    main_progress_code |= 0x24;
}

void cmd_end_heater_warmup(void)
{
    union mailbox_cmd_params *params;

    params = &mailbox.cmd_params;
    if (params->b[0] & 0x80) {
        lamp_heater_front_warmup = 0;
    }
    if (params->b[2] & 0x40) {
        lamp_heater_back_warmup = 0;
    }
    if (params->b[4] & 0x20) {
        lamp_heater_flatbed_warmup = 0;
    }
    if (params->b[1] & 0x10) {
        set_imask_exr(7);
        hw_p2_out.b &= ~HW_P2_WARMUP_OUTPUT;
        p_p2->DR.BYTE = hw_p2_out.b;
        mailbox.warmup_output.b &= ~MDC_WARMUP_OUTPUT_ON;
        sensor_mux_enable = 1;
        lamp_heater_flatbed_on = 1;
        lamp_heater_front_on = 1;
        lamp_heater_back_on = 1;
        lamp_heater_control = 1;
        cmd_test_mode = 0;
        set_imask_exr(0);
    }
}

void cmd_unit_power(void)
{
    set_imask_exr(7);
    lamp_adf_off_count = lamp_adf_off_limit;
    if (mailbox.cmd_params.b[0] == 0x80) {
        lamp_unit_front_on = 1;
        p_p2->DR.BIT.B5 = 1;
        p_p2->DR.BIT.B6 = 1;
        hw_p2_out.b |= HW_P2_ADF_HEATERS;
        if (!sensor_flags.bit.adf_cover_open) {
            hw_p2_out.b |= HW_P2_ADF_FRONT_LAMP;
            p_p2->DR.BYTE = hw_p2_out.b;
        }
    } else {
        lamp_unit_front_on = 0;
        hw_p2_out.b &= ~HW_P2_ADF_HEATERS;
        p_p2->DR.BIT.B5 = 0;
        p_p2->DR.BIT.B6 = 0;
    }
    if (mailbox.cmd_params.b[1] == 0x80) {
        lamp_unit_back_on = 1;
        p_p2->DR.BIT.B5 = 1;
        p_p2->DR.BIT.B6 = 1;
        hw_p2_out.b |= HW_P2_ADF_HEATERS;
        if (!sensor_flags.bit.adf_cover_open) {
            hw_p2_out.b |= HW_P2_ADF_BACK_LAMP;
            p_p2->DR.BYTE = hw_p2_out.b;
        }
    } else {
        lamp_unit_back_on = 0;
        hw_p2_out.b &= ~HW_P2_ADF_HEATERS;
        p_p2->DR.BIT.B5 = 0;
        p_p2->DR.BIT.B6 = 0;
    }
    if (mailbox.cmd_params.b[2] == 0x80) {
        lamp_unit_flatbed_on = 1;
        p_p2->DR.BIT.B4 = 1;
        p_p1->DR.BIT.B6 = 1;
        hw_p2_out.b |= HW_P2_FLATBED_HEATER;
    } else {
        lamp_unit_flatbed_on = 0;
        p_p2->DR.BIT.B4 = 0;
        hw_p2_out.b &= ~HW_P2_FLATBED_HEATER;
    }
    lamp_adf_off_count = lamp_adf_off_limit;
    set_imask_exr(0);
}

void cmd_eject(void)
{
    feed_pick_start_flags.w = 0;
    util_feed_clear_path();
}

void cmd_feed_run_test(unsigned char on)
{
    if (on) {
        if (tick_timeout != 0) {
            return;
        }
        p_p1->DR.BIT.B6 = 0;
        p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_ADF_LAMPS);
        if (cmd_run_test_phase == 0) {
            if (feed_ramp_state != RAMP_STATE_CONSTANT) {
                feed_pick_edge_state = 4;
                feed_top_edge_state = 4;
                feed_mode.b &= ~FEED_MODE_HALF_STEP;
                if (cmd_run_test_class == 1) {
                    feed_ramp_steps = feed_fast_ramp_steps[cmd_run_test_class];
                    feed_fast_ramp = 1;
                } else {
                    feed_ramp_steps = feed_ramp_steps_by_class[cmd_run_test_class];
                    feed_fast_ramp = 0;
                }
                if (cmd_run_test_class == 5) {
                    feed_run_current = MOTOR_CURRENT_RUN_CLASS_5;
                } else if (cmd_run_test_class == 4) {
                    feed_run_current = MOTOR_CURRENT_RUN_CLASS_4;
                } else {
                    feed_run_current = MOTOR_CURRENT_RUN;
                }
                feed_gap_history[0] = 0;
                feed_gap_index = 0;
                feed_action = MOTOR_ACTION_START;
                while (feed_ramp_state != RAMP_STATE_CONSTANT) {
                }
            }
            if (cmd_run_test_sheets != 0) {
                if (cmd_run_test_picking == 0) {
                    if (feed_top_edge_steps > 0x3de) {
                        if (cmd_run_test_class == 1) {
                            pick_ramp_steps = pick_ramp_steps_next_by_class[cmd_run_test_class];
                        } else {
                            pick_ramp_steps = pick_ramp_steps_by_class[cmd_run_test_class];
                        }
                        pick_speed_select = 1;
                        pick_edge_steps = 0;
                        cmd_run_test_picking = 1;
                        if (cmd_run_test_class == 5) {
                            pick_run_current = MOTOR_CURRENT_RUN_CLASS_5;
                        } else if (cmd_run_test_class == 4) {
                            pick_run_current = MOTOR_CURRENT_RUN_CLASS_4;
                        } else {
                            pick_run_current = MOTOR_CURRENT_RUN;
                        }
                        pick_action = MOTOR_ACTION_START;
                    }
                } else if (pick_edge_steps > 0xcf) {
                    pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                    while (pick_action && !err_flags.b) {
                    }
                    feed_top_edge_steps = 0;
                    feed_jam_steps = 0;
                    cmd_run_test_picking = 0;
                    cmd_run_test_sheets--;
                }
            } else {
                cmd_run_test_phase = 1;
            }
        } else {
            pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            while (pick_action && !err_flags.b) {
            }
            tick_timeout = 1000;
            while (tick_timeout) {
            }
            feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            while (feed_action && !err_flags.b) {
            }
            tick_timeout = cmd_run_test_pause_ticks;
            cmd_run_test_sheets = mailbox.cmd_params.b[1];
            if (cmd_run_test_sheets == 0) {
                cmd_run_test_sheets = CMD_RUN_TEST_DEFAULT_SHEETS;
            }
            cmd_run_test_phase = 0;
        }
    } else {
        pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
        while (pick_action && !err_flags.b) {
        }
        feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
        while (feed_action && !err_flags.b) {
        }
        feed_pick_edge_state = 0;
        feed_top_edge_state = 0;
    }
}

void cmd_bw_motor_run_test(unsigned char on)
{
    if (on) {
        if (tick_timeout != 0) {
            return;
        }
        if (cmd_run_test_phase == 0) {
            cmd_run_test_phase = 1;
            bw_motor_arm_state = BW_MOTOR_ARM_STATE_START;
            bw_motor_arm_request = BW_MOTOR_ARM_HOME;
            while (bw_motor_arm_request) {
            }
        } else {
            cmd_run_test_phase = 0;
            bw_motor_backing_state = 0;
            bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
            while (bw_motor_backing_request) {
            }
        }
        err_flags.b = 0;
        err_code = 0;
        tick_timeout = TICKS_PER_SECOND;
    } else {
        while (bw_motor_backing_request) {
        }
        while (bw_motor_arm_request) {
        }
    }
}

void cmd_feed_flatbed_run_test(unsigned char on)
{
    if (on) {
        if (tick_timeout != 0) {
            return;
        }
        if (cmd_run_test_phase == 0) {
            if (feed_ramp_state != RAMP_STATE_CONSTANT) {
                feed_pick_edge_state = 4;
                feed_top_edge_state = 4;
                feed_mode.b &= ~FEED_MODE_HALF_STEP;
                feed_ramp_steps = feed_ramp_steps_by_class[cmd_run_test_class];
                feed_run_current = MOTOR_CURRENT_RUN;
                feed_gap_history[0] = 0;
                feed_gap_index = 0;
                feed_action = MOTOR_ACTION_START;
                while (feed_ramp_state != RAMP_STATE_CONSTANT) {
                }
            }
            if (cmd_run_test_sheets != 0) {
                if (cmd_run_test_picking == 0) {
                    if (feed_top_edge_steps > 0x3de) {
                        pick_ramp_steps = pick_ramp_steps_by_class[cmd_run_test_class];
                        pick_speed_select = 1;
                        pick_edge_steps = 0;
                        cmd_run_test_picking = 1;
                        if (cmd_run_test_class == 5) {
                            pick_run_current = MOTOR_CURRENT_RUN_CLASS_5;
                        } else if (cmd_run_test_class == 4) {
                            pick_run_current = MOTOR_CURRENT_RUN_CLASS_4;
                        } else {
                            pick_run_current = MOTOR_CURRENT_RUN;
                        }
                        pick_action = MOTOR_ACTION_START;
                    }
                } else if (pick_edge_steps > 0xcf) {
                    pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                    while (pick_action && !err_flags.b) {
                    }
                    feed_top_edge_steps = 0;
                    feed_jam_steps = 0;
                    cmd_run_test_picking = 0;
                    cmd_run_test_sheets--;
                }
            } else {
                cmd_run_test_phase = 1;
            }
        } else {
            pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            while (pick_action && !err_flags.b) {
            }
            tick_timeout = 1000;
            while (tick_timeout) {
            }
            feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            while (feed_action && !err_flags.b) {
            }
            fb_motor_scan_end = FB_MOTOR_TEST_SCAN_END;
            fb_motor_done_position = FB_MOTOR_DONE_POSITION;
            fb_motor_scan_ramp_end = fb_motor_ramp_end_by_class[1];
            p_p2->DR.BYTE = (hw_p2_out.b &= ~HW_P2_ADF_LAMPS);
            p_p1->DR.BIT.B6 = 1;
            fb_motor_request = FB_MOTOR_RUN_TEST;
            while (fb_motor_request && !err_flags.b) {
            }
            lamp_adf_off_count = 0;
            lamp_off_count = 0;
            p_p1->DR.BIT.B6 = 0;
            p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_ADF_LAMPS);
            tick_timeout = cmd_run_test_pause_ticks;
            cmd_run_test_sheets = mailbox.cmd_params.b[0];
            cmd_run_test_phase = 0;
        }
    } else {
        pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
        while (pick_action && !err_flags.b) {
        }
        feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
        while (feed_action && !err_flags.b) {
        }
        feed_pick_edge_state = 0;
        feed_top_edge_state = 0;
    }
}

void cmd_clear_overlap_patterns(void)
{
    short i;
    short j;

    for (i = 0; i < OVERLAP_PATTERN_COUNT; i++) {
        for (j = 0; j < 4; j++) {
            job_overlap_patterns_a[i][j] = 0;
            job_overlap_patterns_b[i][j] = 0;
        }
    }
    job_overlap_pattern_count = 0;
}

void cmd_read_overlap_patterns(void)
{
    unsigned char *flags_dst;
    unsigned char *dst_a;
    unsigned char *dst_b;
    unsigned short first;
    unsigned short wrapped_first;
    short i;
    short j;

    util_put_be16(cmd_overlap_pattern_block.count, job_overlap_pattern_count);
    flags_dst = &cmd_overlap_pattern_block.flags;
    *flags_dst = job_overlap_flags.b;
    dst_a = cmd_overlap_pattern_block.patterns_a;
    dst_b = cmd_overlap_pattern_block.patterns_b;
    if (job_overlap_pattern_count >= 8) {
        first = job_overlap_pattern_count - 8;
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 4; j++) {
                *dst_a++ = (unsigned char)(job_overlap_patterns_a[first + i][j] >> 8);
                *dst_a++ = (unsigned char)job_overlap_patterns_a[first + i][j];
                *dst_b++ = (unsigned char)(job_overlap_patterns_b[first + i][j] >> 8);
                *dst_b++ = (unsigned char)job_overlap_patterns_b[first + i][j];
            }
        }
    } else {
        first = job_overlap_pattern_count - 8;
        wrapped_first = job_overlap_pattern_count + 24;
        for (i = 0; i < 8; i++) {
            if (wrapped_first + i < OVERLAP_PATTERN_COUNT) {
                for (j = 0; j < 4; j++) {
                    *dst_a++ = (unsigned char)(job_overlap_patterns_a[wrapped_first + i][j] >> 8);
                    *dst_a++ = (unsigned char)job_overlap_patterns_a[wrapped_first + i][j];
                    *dst_b++ = (unsigned char)(job_overlap_patterns_b[wrapped_first + i][j] >> 8);
                    *dst_b++ = (unsigned char)job_overlap_patterns_b[wrapped_first + i][j];
                }
            } else {
                for (j = 0; j < 4; j++) {
                    *dst_a++ = (unsigned char)(job_overlap_patterns_a[first + i][j] >> 8);
                    *dst_a++ = (unsigned char)job_overlap_patterns_a[first + i][j];
                    *dst_b++ = (unsigned char)(job_overlap_patterns_b[first + i][j] >> 8);
                    *dst_b++ = (unsigned char)job_overlap_patterns_b[first + i][j];
                }
            }
        }
    }
}

void cmd_write_overlap_patterns(void)
{
    unsigned char *flags_src;
    unsigned char *src_a;
    unsigned char *src_b;
    unsigned char *dst_a;
    unsigned char *dst_b;
    unsigned short count;
    short i;
    short j;
    short k;

    cmd_clear_overlap_patterns();
    flags_src = &cmd_overlap_pattern_block.flags;
    job_overlap_flags.b = *flags_src;
    src_a = cmd_overlap_pattern_block.patterns_a;
    src_b = cmd_overlap_pattern_block.patterns_b;
    dst_a = (unsigned char *)job_overlap_patterns_a[16];
    dst_b = (unsigned char *)job_overlap_patterns_b[16];
    for (i = 0; i < 64; i++) {
        *dst_a++ = *src_a++;
        *dst_b++ = *src_b++;
    }
    count = job_overlap_pattern_count = 0;
    for (i = 0; i < 8; i++) {
        if (job_overlap_patterns_b[16 + i][0]) {
            count = job_overlap_pattern_count = 8 - i;
            for (k = 0; k < count; k++, i++) {
                for (j = 0; j < 4; j++) {
                    job_overlap_patterns_a[k][j] = job_overlap_patterns_a[16 + i][j];
                    job_overlap_patterns_b[k][j] = job_overlap_patterns_b[16 + i][j];
                }
            }
            break;
        }
    }
    for (i = 16; i < 24; i++) {
        for (j = 0; j < 4; j++) {
            job_overlap_patterns_a[i][j] = 0;
            job_overlap_patterns_b[i][j] = 0;
        }
    }
}

void cmd_dump_log(void)
{
    unsigned short count;
    unsigned short last_word;
    unsigned char *src;
    unsigned char *dst;
    short i;

    if (log_frozen == 0) {
        count = 0xf0;
    } else {
        count = log_index - 16;
        last_word = (log_buf[14 + count] << 8) | log_buf[15 + count];
        if (last_word != 0xffff) {
            count++;
        }
    }
    util_put_be16(cmd_log_dump_count, count);
    src = &log_buf[16];
    dst = cmd_log_dump;
    for (i = 0; i < count; i++) {
        *dst++ = *src++;
    }
    log_index = 16;
    for (i = 0; i < count; i++) {
        log_buf[16 + i] = 0;
    }
}

void cmd_test_mode_on(void)
{
    cmd_test_mode = 1;
    cmd_delay = cmd_settle_ticks;
    while (cmd_delay) {
    }
    err_flags.b = 0;
    err_code = 0;
    mailbox.error_flags.b = 0;
    mailbox.error_code = 0;
    cmd_delay = cmd_settle_ticks;
    while (cmd_delay) {
    }
    cmd_delay = CMD_SETTLE_DELAY_TICKS;
    while (cmd_delay) {
    }
}

void cmd_test_mode_off(void)
{
    cmd_test_mode = 0;
    cmd_delay = cmd_settle_ticks;
    while (cmd_delay) {
    }
}

unsigned char cmd_adjust_ultrasonic_idle(void)
{
    multifeed_check.idle_threshold = multifeed_check.idle_level + 10;
    if (mailbox.settings_8f) {
        multifeed_check.threshold = mailbox.settings_8f;
    } else if (multifeed_check.idle_threshold) {
        multifeed_check.threshold = multifeed_check.idle_threshold;
    } else {
        multifeed_check.threshold = ULTRASONIC_THRESHOLD_DEFAULT;
    }
    if (multifeed_check.idle_level < ULTRASONIC_IDLE_MIN || multifeed_check.idle_level > ULTRASONIC_IDLE_MAX) {
        mailbox.df_adjust_flags &= ~MDC_DF_ADJUST_ULTRASONIC_OK;
        return 1;
    }
    if (multifeed_check.paper_level < ULTRASONIC_PAPER_MIN_IDLE) {
        mailbox.df_adjust_flags &= ~MDC_DF_ADJUST_ULTRASONIC_OK;
        return 1;
    }
    main_scale_ultrasonic_threshold();
    mailbox.df_adjust_flags |= MDC_DF_ADJUST_ULTRASONIC_OK;
    return 0;
}

unsigned char cmd_adjust_ultrasonic_result(void)
{
    unsigned char threshold;

    if (multifeed_check.paper_level < ULTRASONIC_PAPER_MIN || multifeed_check.paper_level > ULTRASONIC_PAPER_MAX) {
        return 1;
    }
    multifeed_check.paper_level_adjusted = multifeed_check.paper_level;
    threshold = (unsigned char)multifeed_check.idle_level + (multifeed_check.paper_level - multifeed_check.idle_level) * 2 / 5;
    if (threshold < ULTRASONIC_IDLE_MIN || threshold > ULTRASONIC_THRESHOLD_MAX) {
        return 1;
    }
    mailbox.settings_8f = threshold;
    multifeed_check.threshold = mailbox.settings_8f;
    return 0;
}

void cmd_warmup_on(void)
{
    set_imask_exr(7);
    tick_warmup_hold = 1;
    tick_warmup_seconds = 0;
    lamp_adf_off_count = lamp_adf_off_limit;
    if (!sensor_flags.bit.adf_cover_open) {
        p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_ADF_LAMPS);
        lamp_unit_front_on = 1;
        lamp_unit_back_on = 1;
    }
    p_p1->DR.BIT.B6 = 1;
    lamp_unit_flatbed_on = 1;
    p_p2->DR.BIT.B5 = 1;
    p_p2->DR.BIT.B6 = 1;
    p_p2->DR.BIT.B4 = 1;
    hw_p2_out.b |= HW_P2_HEATERS;
    hw_p2_out.b |= HW_P2_WARMUP_OUTPUT;
    p_p2->DR.BYTE = hw_p2_out.b;
    mailbox.warmup_output.b |= MDC_WARMUP_OUTPUT_ON;
    lamp_adf_off_count = lamp_adf_off_limit;
    set_imask_exr(0);
}

void cmd_warmup_off(void)
{
    set_imask_exr(7);
    hw_p2_out.b &= ~HW_P2_WARMUP_OUTPUT;
    p_p2->DR.BYTE = hw_p2_out.b;
    mailbox.warmup_output.b &= ~MDC_WARMUP_OUTPUT_ON;
    tick_warmup_hold = 0;
    set_imask_exr(0);
}
