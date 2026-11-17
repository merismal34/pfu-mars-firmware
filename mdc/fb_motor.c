#include "mdc.h"

/* ---- objects ---- */
const unsigned short fb_motor_pause_ticks = 0x0032;
const unsigned short fb_motor_run_pause_ticks = 0x00fa;

/* ---- functions ---- */

#pragma interrupt (isr_tgi4a)
void isr_tgi4a(void)
{
    short unused_frame;
    union byte_view inputs;
    unsigned char unused_frame_2;

    p_tpu->TSTR.BIT.CST4 = 0;
    fb_motor_saved_imask = get_imask_exr();
    if (!p_tpu4->TSR.BIT.TGFA) {
        return;
    }
    p_tpu4->TSR.BIT.TGFA = 0;
    inputs.b = p_chip_a->inputs.b;
    set_imask_exr(fb_motor_saved_imask);
    if (inputs.bit.b7) {
        if (sensor_home_count < SENSOR_DEBOUNCE_MAX) {
            sensor_home_count++;
        }
    } else {
        if (sensor_home_count) {
            sensor_home_count--;
        }
    }
    if (sensor_home_count > SENSOR_DEBOUNCE_ON) {
        sensor_flags.bit.fb_home = 1;
        mailbox.sensor_status.bit.fb_home = 1;
    } else {
        sensor_flags.bit.fb_home = 0;
        mailbox.sensor_status.bit.fb_home = 0;
    }
    if (p_chip_b_motor->control_2.bit.b6) {
        if (p_chip_b_motor->control_2.bit.b2) {
            if ((p_chip_b_motor->phase_2.b & 3) == 0) {
                fb_motor_position++;
            }
        } else {
            if ((p_chip_b_motor->phase_2.b & 3) == 0) {
                fb_motor_position--;
            }
        }
        if (fb_motor_position < 0x1dec || fb_motor_position > 0x4d22) {
            set_imask_exr(7);
            err_flags.bit.device = 1;
            err_saved_flags = err_flags.b;
            err_saved_code = err_code = ERR_FLATBED_MOTOR;
            motor_set_current(MOTOR_FB, MOTOR_CURRENT_OFF);
            fb_motor_request = 0;
            fb_motor_state = FB_MOTOR_STATE_START;
            set_imask_exr(fb_motor_saved_imask);
            return;
        }
    }
    if (fb_motor_home_sync && !sensor_flags.bit.fb_home) {
        fb_motor_home_sync = 0;
        fb_motor_position = 0x2000;
    }
    switch (fb_motor_ramp_state) {
    case RAMP_STATE_START:
    case RAMP_STATE_SPEED_UP:
        if (fb_motor_ramp_index != fb_motor_ramp_target) {
            fb_motor_ramp_index++;
            if (fb_motor_mode.b & FB_MOTOR_MODE_HALF_STEP) {
                p_chip_b_motor->period_2_high.f_0_8.f = ramp_fb_motor_half_high[fb_motor_ramp_index];
                p_chip_b_motor->period_2_low.f_0_8.f = ramp_fb_motor_half_low[fb_motor_ramp_index];
            } else {
                p_chip_b_motor->period_2_high.f_0_8.f = ramp_fb_motor_high[fb_motor_ramp_index];
                p_chip_b_motor->period_2_low.f_0_8.f = ramp_fb_motor_low[fb_motor_ramp_index];
            }
        } else {
            fb_motor_ramp_state = RAMP_STATE_CONSTANT;
            fb_motor_run_current_steps = 10;
        }
        break;
    case RAMP_STATE_SPEED_DOWN:
        if (fb_motor_ramp_index != fb_motor_ramp_target) {
            fb_motor_ramp_index--;
            if (fb_motor_mode.b & FB_MOTOR_MODE_HALF_STEP) {
                p_chip_b_motor->period_2_high.f_0_8.f = ramp_fb_motor_half_high[fb_motor_ramp_index];
                p_chip_b_motor->period_2_low.f_0_8.f = ramp_fb_motor_half_low[fb_motor_ramp_index];
            } else {
                p_chip_b_motor->period_2_high.f_0_8.f = ramp_fb_motor_high[fb_motor_ramp_index];
                p_chip_b_motor->period_2_low.f_0_8.f = ramp_fb_motor_low[fb_motor_ramp_index];
            }
        } else {
            fb_motor_ramp_state = RAMP_STATE_CONSTANT;
        }
        break;
    case RAMP_STATE_STOPPING:
    case 4:
        if (fb_motor_ramp_index != 0) {
            if (fb_motor_mode.b & FB_MOTOR_MODE_HALF_STEP) {
                p_chip_b_motor->period_2_high.f_0_8.f = ramp_fb_motor_half_high[fb_motor_ramp_index];
                p_chip_b_motor->period_2_low.f_0_8.f = ramp_fb_motor_half_low[fb_motor_ramp_index];
            } else {
                p_chip_b_motor->period_2_high.f_0_8.f = ramp_fb_motor_high[fb_motor_ramp_index];
                p_chip_b_motor->period_2_low.f_0_8.f = ramp_fb_motor_low[fb_motor_ramp_index];
            }
            if (fb_motor_ramp_index > 2) {
                fb_motor_ramp_index -= 2;
            } else {
                fb_motor_ramp_index = 0;
            }
        } else {
            set_imask_exr(7);
            motor_set_current(MOTOR_FB, MOTOR_CURRENT_OFF);
            set_imask_exr(fb_motor_saved_imask);
            mailbox.fb_motor_status.bit.moving = 0;
            fb_motor_ramp_state = RAMP_STATE_STOPPED;
        }
        break;
    }
    if (fb_motor_request) {
        switch (fb_motor_state) {
        case FB_MOTOR_STATE_START:
            mailbox.fb_motor_status.b = 0;
            fb_motor_step_count = 5;
            fb_motor_state = FB_MOTOR_STATE_CHECK_HOME;
            break;
        case FB_MOTOR_STATE_CHECK_HOME:
            if (fb_motor_step_count == 0) {
                fb_motor_scan_end = 0x1e28;
                if (!sensor_flags.bit.fb_home) {
                    fb_motor_position = 0x4d18;
                    fb_motor_state = FB_MOTOR_STATE_SEEK_HOME;
                } else {
                    fb_motor_position = 0x1df6;
                    fb_motor_state = FB_MOTOR_STATE_LEAVE_HOME;
                }
            } else {
                fb_motor_step_count--;
            }
            break;
        case FB_MOTOR_STATE_LEAVE_HOME:
            fb_motor_step_count = 0x300;
            fb_motor_state = FB_MOTOR_STATE_LEAVING_HOME;
            fb_motor_start_forward(fb_motor_ramp_end_class_0);
            break;
        case FB_MOTOR_STATE_LEAVING_HOME:
            if (!sensor_flags.bit.fb_home) {
                fb_motor_step_count = 0xc8;
                fb_motor_state = FB_MOTOR_STATE_PAST_HOME;
                fb_motor_blocked = 0;
            } else if (fb_motor_step_count == 0) {
                set_imask_exr(7);
                if (fb_motor_blocked) {
                    if (!p_chip_b->inputs.bit.b2 && tick_motor_fuse_count > 0x32) {
                        err_flags.bit.device = 1;
                        err_saved_flags = err_flags.b;
                        err_saved_code = err_code = ERR_MOTOR_FUSE;
                    } else {
                        err_flags.bit.document = 1;
                        err_saved_flags = err_flags.b;
                        err_saved_code = err_code = ERR_SHIPPING_LOCK;
                    }
                } else {
                    err_flags.bit.device = 1;
                    err_saved_flags = err_flags.b;
                    err_saved_code = err_code = ERR_FLATBED_MOTOR;
                }
                motor_set_current(MOTOR_FB, MOTOR_CURRENT_OFF);
                set_imask_exr(fb_motor_saved_imask);
                fb_motor_request = 0;
                fb_motor_state = FB_MOTOR_STATE_START;
            } else {
                fb_motor_step_count--;
            }
            break;
        case FB_MOTOR_STATE_PAST_HOME:
            if (fb_motor_step_count == 0) {
                fb_motor_state = FB_MOTOR_STATE_WAIT_STOPPED;
                fb_motor_stop();
            } else {
                fb_motor_step_count--;
            }
            break;
        case FB_MOTOR_STATE_WAIT_STOPPED:
            if (fb_motor_ramp_state == RAMP_STATE_STOPPED) {
                if (fb_motor_delay_ticks == 0) {
                    fb_motor_state = FB_MOTOR_STATE_SEEK_HOME;
                }
            } else {
                fb_motor_delay_ticks = fb_motor_run_pause_ticks;
            }
            break;
        case FB_MOTOR_STATE_SEEK_HOME:
            fb_motor_state = FB_MOTOR_STATE_SEEKING_HOME;
            fb_motor_start_reverse(fb_motor_ramp_end_class_0);
        case FB_MOTOR_STATE_SEEKING_HOME:
            if (sensor_flags.bit.fb_home) {
                fb_motor_step_count = 0x1d8 - (fb_motor_ramp_end_class_0 >> 1);
                fb_motor_step_count -= 0x1c;
                fb_motor_state = FB_MOTOR_STATE_HOME_OVERRUN;
                fb_motor_position = 0x2000;
            }
            break;
        case FB_MOTOR_STATE_HOME_OVERRUN:
            if (fb_motor_step_count == 0) {
                fb_motor_stop_at_home();
                fb_motor_state = FB_MOTOR_STATE_AT_HOME;
                if (fb_motor_request == FB_MOTOR_TEST) {
                    fb_motor_state = FB_MOTOR_STATE_TEST_PAUSE;
                    fb_motor_delay_ticks = fb_motor_pause_ticks;
                }
            } else {
                fb_motor_step_count--;
            }
            break;
        case FB_MOTOR_STATE_TEST_PAUSE:
            if (fb_motor_ramp_state == RAMP_STATE_STOPPED) {
                if (fb_motor_delay_ticks == 0) {
                    fb_motor_state = FB_MOTOR_STATE_AT_HOME;
                }
            } else {
                fb_motor_delay_ticks = fb_motor_run_pause_ticks;
            }
            break;
        case FB_MOTOR_STATE_AT_HOME:
            if (fb_motor_ramp_state == RAMP_STATE_STOPPED) {
                mailbox.fb_motor_status.bit.at_home = 1;
                if (fb_motor_request == FB_MOTOR_SCAN || fb_motor_request == FB_MOTOR_RUN_TEST || fb_motor_request == FB_MOTOR_TEST) {
                    fb_motor_stop_position = fb_motor_scan_end;
                    job_capture_flags.b = 0;
                    if (fb_motor_request == FB_MOTOR_TEST) {
                        fb_motor_done_position = FB_MOTOR_DONE_POSITION;
                    }
                    fb_motor_unused_limit = 0xffff;
                    fb_motor_home_sync = 1;
                    fb_motor_start_forward(fb_motor_scan_ramp_end);
                    fb_motor_state = FB_MOTOR_STATE_SCAN_START;
                } else {
                    fb_motor_request = 0;
                }
            }
            break;
        case FB_MOTOR_STATE_SCAN_START:
            if (fb_motor_ramp_state == RAMP_STATE_CONSTANT) {
                if (fb_motor_home_sync == 0 && fb_motor_position > fb_motor_done_position) {
                    fb_motor_done_position = 0xffff;
                    if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE || mailbox.cmd_code == MDC_CMD_FB_MOTOR_HOME) {
                        job_set_cmd_state(MDC_CMD_STATE_DONE);
                    }
                    mailbox.motion_flags.bit.front = 1;
                    mailbox.motion_flags.bit.at_top = 1;
                    mailbox.cmd_result = MDC_RESULT_FRONT_START;
                    fb_motor_state = FB_MOTOR_STATE_SCANNING;
                }
                if (fb_motor_position == FB_MOTOR_DONE_POSITION && sensor_flags.bit.fb_home && mailbox.cmd_code == MDC_CMD_SCAN_MOVE) {
                    set_imask_exr(7);
                    err_flags.bit.device = 1;
                    err_saved_flags = err_flags.b;
                    err_saved_code = err_code = ERR_FLATBED_MOTOR;
                    motor_set_current(MOTOR_FB, MOTOR_CURRENT_OFF);
                    fb_motor_request = 0;
                    fb_motor_state = FB_MOTOR_STATE_START;
                    fb_motor_blocked = 1;
                    set_imask_exr(fb_motor_saved_imask);
                    return;
                }
            }
            break;
        case FB_MOTOR_STATE_SCANNING:
            if (fb_motor_position > fb_motor_stop_position) {
                job_busy_flags.bit.scan_move = 0;
                fb_motor_state = FB_MOTOR_STATE_SCAN_STOP;
                fb_motor_stop();
            }
            if (job_capture_flags.bit.pause_requested) {
                job_capture_flags.bit.pause_requested = 0;
                mailbox.capture_flags.bit.paused = 1;
                fb_motor_state = FB_MOTOR_STATE_CAPTURE_STOP;
                fb_motor_stop();
            }
            break;
        case FB_MOTOR_STATE_SCAN_STOP:
            if (fb_motor_ramp_state == RAMP_STATE_STOPPED) {
                fb_motor_mode.bit.half_step = 0;
                mailbox.motion_flags.b = 0;
                fb_motor_state = FB_MOTOR_STATE_WAIT_STOPPED;
                fb_motor_delay_ticks = fb_motor_pause_ticks;
                lamp_adf_off_count = lamp_adf_off_limit;
                if (fb_motor_request != FB_MOTOR_TEST) {
                    fb_motor_request = FB_MOTOR_STOP;
                }
                if (main_fb_home_cmd == 2) {
                    if (sensor_flags.bit.adf_cover_open) {
                        err_flags.bit.document = 1;
                        err_saved_flags = err_flags.b;
                        err_saved_code = err_code = ERR_ADF_COVER_OPEN;
                        main_flags_1.bit.adf_cover_open = 1;
                    }
                    main_fb_home_cmd = 0;
                    cmd_test_mode = 0;
                }
            }
            break;
        case FB_MOTOR_STATE_CAPTURE_STOP:
            if (fb_motor_ramp_state == RAMP_STATE_STOPPED) {
                fb_motor_state = FB_MOTOR_STATE_CAPTURE_SETTLE;
                fb_motor_delay_ticks = fb_motor_pause_ticks;
            }
            break;
        case FB_MOTOR_STATE_CAPTURE_SETTLE:
            if (fb_motor_delay_ticks == 0) {
                fb_motor_state = FB_MOTOR_STATE_CAPTURE_BACK_UP;
                fb_motor_step_count = fb_motor_position - fb_motor_mark_position + 0x33c;
                fb_motor_start_reverse(fb_motor_ramp_end_class_5);
            }
            break;
        case FB_MOTOR_STATE_CAPTURE_BACK_UP:
            if (fb_motor_step_count == 0) {
                fb_motor_state = FB_MOTOR_STATE_CAPTURE_PAUSED;
                fb_motor_stop();
            } else {
                fb_motor_step_count--;
            }
            break;
        case FB_MOTOR_STATE_CAPTURE_PAUSED:
            if (fb_motor_ramp_state == RAMP_STATE_STOPPED) {
                util_put_be16(&mailbox.capture_steps_high, fb_motor_mark_position - fb_motor_position);
                mailbox.capture_flags.bit.steps_valid = 1;
                if (job_capture_flags.bit.resume_requested) {
                    job_capture_flags.bit.resume_requested = 0;
                    fb_motor_state = FB_MOTOR_STATE_CAPTURE_RESUME;
                    fb_motor_start_forward(fb_motor_scan_ramp_end);
                }
            }
            break;
        case FB_MOTOR_STATE_CAPTURE_RESUME:
            if (fb_motor_position > fb_motor_mark_position) {
                mailbox.capture_flags.bit.paused = 0;
                mailbox.capture_flags.bit.steps_valid = 0;
                fb_motor_state = FB_MOTOR_STATE_SCANNING;
            }
            break;
        }
    }
    if (fb_motor_run_current_steps) {
        fb_motor_run_current_steps--;
        if (fb_motor_run_current_steps == 0) {
            set_imask_exr(7);
            motor_set_current(MOTOR_FB, MOTOR_CURRENT_RUN);
            set_imask_exr(fb_motor_saved_imask);
        }
    }
}

void fb_motor_start_forward(unsigned short ramp_end)
{
    if (ramp_end != 0) {
        set_imask_exr(7);
        motor_set_current(MOTOR_FB, MOTOR_CURRENT_START);
        set_imask_exr(feed_saved_imask);
        fb_motor_ramp_target = ramp_end;
        fb_motor_ramp_state = RAMP_STATE_START;
    } else {
        p_chip_b_motor->period_2_high.f_0_8.f = 0xff;
        p_chip_b_motor->period_2_low.f_0_8.f = 0xff;
        fb_motor_ramp_target = 0;
        fb_motor_ramp_state = RAMP_STATE_CONSTANT;
    }
    mailbox.fb_motor_status.bit.moving = 1;
    if (fb_motor_mode.b & FB_MOTOR_MODE_HALF_STEP) {
        p_chip_b_motor->control_2.f_4_2.f = 2;
    } else {
        p_chip_b_motor->control_2.f_4_2.f = 1;
    }
    p_chip_b_motor->control_2.bit.b2 = 1;
    p_chip_b_motor->control_2.bit.b6 = 1;
    p_chip_b_motor->control_2.bit.b3 = 1;
}

void fb_motor_stop(void)
{
    if (fb_motor_ramp_index > 2) {
        set_imask_exr(7);
        motor_set_current(MOTOR_FB, MOTOR_CURRENT_START);
        set_imask_exr(feed_saved_imask);
        fb_motor_ramp_target = 0;
        fb_motor_ramp_state = RAMP_STATE_STOPPING;
    } else {
        p_chip_b_motor->control_2.bit.b6 = 0;
        p_chip_b_motor->control_2.bit.b3 = 0;
        fb_motor_ramp_target = 0;
        fb_motor_ramp_state = RAMP_STATE_STOPPED;
    }
}

void fb_motor_start_reverse(unsigned short ramp_end)
{
    if (ramp_end != 0) {
        set_imask_exr(7);
        motor_set_current(MOTOR_FB, MOTOR_CURRENT_START);
        set_imask_exr(feed_saved_imask);
        fb_motor_ramp_state = RAMP_STATE_START;
    } else {
        fb_motor_ramp_state = RAMP_STATE_CONSTANT;
    }
    mailbox.fb_motor_status.bit.moving = 1;
    fb_motor_ramp_target = ramp_end;
    p_chip_b_motor->control_2.f_4_2.f = 1;
    p_chip_b_motor->control_2.bit.b2 = 0;
    p_chip_b_motor->control_2.bit.b6 = 1;
    p_chip_b_motor->control_2.bit.b3 = 1;
}

void fb_motor_stop_at_home(void)
{
    if (fb_motor_ramp_index > 2) {
        set_imask_exr(7);
        motor_set_current(MOTOR_FB, MOTOR_CURRENT_START);
        set_imask_exr(feed_saved_imask);
        fb_motor_ramp_target = 0;
        fb_motor_ramp_state = RAMP_STATE_STOPPING;
    } else {
        p_chip_b_motor->control_2.bit.b6 = 0;
        p_chip_b_motor->control_2.bit.b3 = 0;
        fb_motor_ramp_target = 0;
        fb_motor_ramp_state = RAMP_STATE_STOPPED;
    }
}

void fb_motor_set_target(unsigned short ramp_end)
{
    if (fb_motor_ramp_index > ramp_end) {
        fb_motor_ramp_target = ramp_end;
        fb_motor_ramp_state = RAMP_STATE_SPEED_DOWN;
    } else if (fb_motor_ramp_index < ramp_end) {
        fb_motor_ramp_target = ramp_end;
        fb_motor_ramp_state = RAMP_STATE_SPEED_UP;
    }
}
