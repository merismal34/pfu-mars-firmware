#include "mdc.h"

void feed_handle_event(void)
{
    unsigned char state;
    unsigned char action;
    unsigned char ramp_state;
    union byte_view inputs;
    unsigned short u1;
    unsigned short u2;
    unsigned short count;
    unsigned short *edge_word;

    feed_saved_imask = get_imask_exr();
    inputs.b = p_chip_a->inputs.b;
    set_imask_exr(feed_saved_imask);
    if (feed_top_steps < 0xffff && feed_ramp_state)
        feed_top_steps++;
    if (inputs.bit.b3) {
        if (!feed_top_window_flags.bit.open) {
            if (sensor_top_count < sensor_top_count_limit)
                sensor_top_count++;
        } else {
            if (feed_top_window_on_steps < 0xffff)
                feed_top_window_on_steps++;
        }
    } else {
        if (sensor_top_count)
            sensor_top_count--;
        if (feed_top_window_flags.bit.open && feed_top_window_steps == 0) {
            feed_top_window_flags.w &= ~(FEED_TOP_WINDOW_OPEN | FEED_TOP_WINDOW_TRAILING_EDGE);
            util_put_be16(&mailbox.top_window_on_steps_high, feed_top_window_on_steps);
        }
    }
    if (feed_top_window_steps != 0 && feed_top_window_flags.bit.open) {
        feed_top_window_steps--;
        if (feed_top_window_steps == 0) {
            feed_top_window_on_saved = feed_top_window_on_steps;
            if (feed_top_window_on_steps == 0) {
                feed_top_window_flags.w &= ~(FEED_TOP_WINDOW_OPEN | FEED_TOP_WINDOW_TRAILING_EDGE);
                util_put_be16(&mailbox.top_window_on_steps_high, feed_top_window_on_steps);
            }
        }
    } else if (feed_top_window_flags.bit.open && feed_top_window_steps == 0 && (feed_top_window_flags.w & FEED_TOP_WINDOW_TRAILING_EDGE) && feed_top_window_on_steps > 0x40) {
        feed_top_window_flags.w &= ~(FEED_TOP_WINDOW_OPEN | FEED_TOP_WINDOW_TRAILING_EDGE);
        feed_top_window_flags.bit.gap_correction = 1;
        util_put_be16(&mailbox.top_window_on_steps_high, feed_top_window_on_steps);
    }
    if (feed_ramp_state) {
        if (inputs.bit.b3)
            job_top_clear_steps = 0;
        else if (job_top_clear_steps < 0xfffffff0UL)
            job_top_clear_steps++;
    }
    if (sensor_top_count > sensor_top_threshold) {
        if (feed_in_transport) {
            if ((feed_pick_start_flags.w == 0 && feed_top_steps >= job_front_start_steps && mailbox.cmd_code == MDC_CMD_SCAN_MOVE && mailbox.cmd_state == MDC_CMD_STATE_RUNNING && feed_ramp_stopped == 0)
                || ((feed_pick_start_flags.w & (FEED_PICK_START_ENABLED | FEED_PICK_START_FRONT)) == (FEED_PICK_START_ENABLED | FEED_PICK_START_FRONT) && feed_top_steps >= job_front_start_steps && feed_ramp_stopped == 0)) {
                job_front_reached = 1;
                if (feed_pick_start_flags.w != 0) {
                    feed_pick_start_flags.byte.l &= ~FEED_PICK_START_FRONT;
                    feed_pick_start_countdown = 0;
                } else {
                    if ((job_scan_start_flags.w & JOB_SCAN_START_TOP_COVERED) && feed_top_steps > job_front_start_steps + job_late_start_margin) {
                        feed_front_late_steps = feed_top_steps - job_front_start_steps;
                        job_scan_start_flags.byte.l &= ~JOB_SCAN_START_TOP_COVERED;
                    } else {
                        feed_front_late_steps = 0;
                        job_scan_start_flags.byte.l &= ~JOB_SCAN_START_TOP_COVERED;
                    }
                    job_capture_flags.b = 0;
                    mailbox.motion_flags.b |= MDC_MOTION_FRONT;
                    feed_gap_timeout = 0;
                    if (mailbox.source & MDC_SOURCE_ADF) {
                        if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE) {
                            feed_set_cmd_state(MDC_CMD_STATE_ENDED);
                            feed_gap_timeout = FEED_GAP_TIMEOUT_EVENTS;
                        }
                    } else {
                        job_busy_flags.bit.scan_move = 0;
                        if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE)
                            feed_set_cmd_state(MDC_CMD_STATE_DONE);
                    }
                    if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE) {
                        if (err_flags.b == 0)
                            mailbox.cmd_result = MDC_RESULT_FRONT_START;
                        else
                            mailbox.cmd_result = MDC_RESULT_ERROR;
                    }
                    if (log_frozen == 0) {
                        log_hook_begin(0);
                        set_imask_exr(7);
                        p_log_buf[log_index] = LOG_FRONT_START;
                        log_advance();
                        p_log_buf[log_index] = job_state;
                        log_advance();
                        p_log_buf[log_index] = LOG_END;
                        set_imask_exr(feed_saved_imask);
                        log_end();
                    }
                }
            } else {
                if ((feed_pick_start_flags.w == 0 && feed_top_steps >= job_back_start_steps + feed_front_late_steps && !(mailbox.motion_flags.b & MDC_MOTION_BACK) && feed_ramp_stopped == 0)
                    || ((feed_pick_start_flags.w & (FEED_PICK_START_ENABLED | FEED_PICK_START_BACK)) == (FEED_PICK_START_ENABLED | FEED_PICK_START_BACK) && feed_top_steps >= job_back_start_steps && feed_ramp_stopped == 0)) {
                    if (feed_pick_start_flags.w != 0) {
                        feed_pick_start_flags.byte.l &= ~FEED_PICK_START_BACK;
                    } else {
                        mailbox.motion_flags.b |= (MDC_MOTION_BACK | MDC_MOTION_BACK_PENDING);
                        feed_front_late_steps_saved = feed_front_late_steps;
                        feed_front_late_steps = 0;
                        if (mailbox.source & MDC_SOURCE_ADF) {
                            job_busy_flags.bit.scan_move = 0;
                            if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE) {
                                if (mailbox.cmd_state == MDC_CMD_STATE_RUNNING || !(mailbox.motion_flags.b & MDC_MOTION_FRONT)) {
                                    mailbox.sheet_gap_error_count++;
                                    err_flags.b |= ERR_FLAG_DOCUMENT;
                                    err_code = ERR_SHEET_GAP;
                                }
                                feed_set_cmd_state(MDC_CMD_STATE_DONE);
                                if (err_flags.b == 0)
                                    mailbox.cmd_result = MDC_RESULT_BACK_START;
                                else
                                    mailbox.cmd_result = MDC_RESULT_ERROR;
                                feed_gap_timeout = 0;
                            }
                        }
                    }
                }
            }
        }
    } else {
        if (feed_top_steps > sensor_top_threshold) {
            if (sensor_top_count > (unsigned char)(sensor_top_threshold - feed_event_divider.byte.l))
                sensor_top_count -= feed_event_divider.w - feed_event_count;
        }
        feed_top_steps = 0;
    }
    if (feed_gap_timeout == 0) {
    } else {
        feed_gap_timeout--;
        if (feed_gap_timeout == 1 && feed_ramp_state == RAMP_STATE_STOPPED && !(job_capture_flags.bit.pause_active) && mailbox.cmd_code == MDC_CMD_SCAN_MOVE
            && (mailbox.cmd_state == MDC_CMD_STATE_ENDED || mailbox.cmd_state == MDC_CMD_STATE_RUNNING)) {
            err_flags.b |= ERR_FLAG_DOCUMENT;
            err_code = ERR_SHEET_GAP;
            mailbox.cmd_result = MDC_RESULT_ERROR;
        }
    }
    if (feed_event_count < feed_event_divider.w) {
        feed_event_count++;
        if (feed_event_count == feed_event_divider.w)
            feed_read_sensors(inputs);
        return;
    }
    feed_event_count = 0;
    if (feed_byte_6 & 1) {
        if (feed_mode.b & FEED_MODE_SKIP_NEXT_EVENT) {
            feed_mode.b &= ~FEED_MODE_SKIP_NEXT_EVENT;
            return;
        }
        feed_mode.b |= FEED_MODE_SKIP_NEXT_EVENT;
    }
    if (feed_wait_steps)
        feed_wait_steps--;
    if ((ramp_state = feed_ramp_state) != 0) {
        sensor_mux_interval = TICKS_PER_SECOND;
        lamp_heater_interval = TICKS_PER_SECOND;
    } else {
        sensor_mux_interval = 50;
        lamp_heater_interval = 50;
    }
    switch (feed_ramp_state) {
    case RAMP_STATE_START:
    case RAMP_STATE_SPEED_UP:
        if (p_chip_a_motor->events.bit.b3) {
            feed_ramp_stopped = 0;
            if (p_chip_a_motor->ramp_steps_1.b)
                break;
            p_chip_a_motor->events.b = 8;
            p_chip_a_motor->ramp_index_1.f_0_8.f -= 1;
            feed_ramp_state = RAMP_STATE_CONSTANT;
            feed_speed_tracking = 1;
            if (ramp_feed_restore_pending) {
                ramp_feed_restore_pending = 0;
                ramp_feed_restore_step = 1;
            }
        }
        break;
    case RAMP_STATE_SPEED_DOWN:
        if (p_chip_a_motor->events.bit.b3) {
            feed_ramp_stopped = 0;
            p_chip_a_motor->events.b = 8;
            p_chip_a_motor->ramp_index_1.f_0_8.f += 1;
            feed_ramp_state = RAMP_STATE_CONSTANT;
            feed_speed_tracking = 1;
        }
        break;
    case RAMP_STATE_STOPPING:
    case 4:
        if (p_chip_a_motor->events.bit.b3) {
            feed_ramp_stopped = 1;
            p_chip_a_motor->events.b = 8;
            set_imask_exr(7);
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_OFF);
            set_imask_exr(feed_saved_imask);
            feed_ready_stop = 0;
            feed_ramp_state = RAMP_STATE_STOPPED;
            feed_speed_tracking = 0;
        }
        break;
    }
    if (feed_speed_tracking && feed_ramp_state == RAMP_STATE_CONSTANT) {
        if (feed_speed_step > feed_speed_step_target) {
            feed_speed_step--;
            p_chip_a_motor->period_1_high.f_0_8.f = ramp_feed_periods[feed_speed_step].a;
            p_chip_a_motor->period_1_low.f_0_8.f = ramp_feed_periods[feed_speed_step].b;
        }
        if (feed_speed_step < feed_speed_step_target) {
            feed_speed_step++;
            p_chip_a_motor->period_1_high.f_0_8.f = ramp_feed_periods[feed_speed_step].a;
            p_chip_a_motor->period_1_low.f_0_8.f = ramp_feed_periods[feed_speed_step].b;
        }
    }
    feed_check_ultrasonic();
    if (ramp_feed_restore_step)
        ramp_restore_feed();
    action = feed_action;
    switch (state = feed_state) {
    case FEED_STATE_RESET:
        if (ramp_state) {
            set_imask_exr(7);
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_OFF);
            set_imask_exr(feed_saved_imask);
            if (feed_byte_6 & 1) {
                p_chip_a_motor->control_1.f_4_2.f = 1;
                feed_mode.b &= ~FEED_MODE_SKIP_NEXT_EVENT;
            }
            feed_ramp_state = ramp_state = RAMP_STATE_STOPPED;
        }
        if (p_chip_a_motor->control_1.bit.b3 && p_chip_a_motor->current_1[7] == 0)
            p_chip_a_motor->control_1.bit.b3 = 0;
        if (!sensor_flags.bit.pick)
            feed_pick_edge_count = 0;
        p_chip_a_motor->period_1_high.f_0_8.f = 0xff;
        p_chip_a_motor->period_1_low.f_0_8.f = 0xff;
        action = MOTOR_ACTION_OFF;
        state = FEED_STATE_IDLE;
        break;
    case FEED_STATE_START:
        if (action == MOTOR_ACTION_NONE) {
            if (feed_ramp_steps == feed_ramp_steps_next)
                action = MOTOR_ACTION_START;
            else
                action = MOTOR_ACTION_CHANGE_SPEED;
            state = FEED_STATE_WAIT_SHEET;
        }
        break;
    case FEED_STATE_WAIT_SHEET:
        if (feed_sheets_in_path != 0 || (mailbox.motion_flags.b & MDC_MOTION_FRONT)) {
            if (feed_sheets_in_path == 0)
                mailbox.path_empty_count++;
            state = FEED_STATE_TRANSPORT;
        } else if (mailbox.long_page == MDC_LONG_PAGE_ON && !sensor_flags.bit.top && !sensor_flags.bit.pick && sensor_flags.bit.no_paper) {
            state = FEED_STATE_STOP;
            if (mailbox.cmd_state == MDC_CMD_STATE_POSTED && mailbox.cmd_code == MDC_CMD_SCAN_MOVE)
                feed_set_cmd_state(MDC_CMD_STATE_RUNNING);
            err_flags.b |= ERR_FLAG_DOCUMENT;
            err_code = ERR_JAM;
            feed_pick_edge_state = 0xff;
        }
        break;
    case FEED_STATE_TRANSPORT:
        if (!sensor_flags.bit.top && pick_ramp_state == RAMP_STATE_STOPPED) {
            if (!sensor_flags.bit.pick)
                feed_sheets_in_path = 0;
            feed_wait_steps = job_eject_steps;
            if ((job_last_page_flags & 1) && !sensor_flags.bit.no_paper)
                feed_wait_steps = feed_last_page_wait_steps;
            state = FEED_STATE_EJECT;
        }
        if (mailbox.motion_flags.b & MDC_MOTION_SIDES) {
            if (job_capture_flags.bit.pause_requested) {
                state = FEED_STATE_CAPTURE_STOP;
                action = MOTOR_ACTION_STOP_KEEP_CURRENT_RAMP;
                pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            }
        } else if (job_capture_flags.bit.pause_requested && job_top_clear_steps < 0x37bUL) {
            state = FEED_STATE_CAPTURE_STOP;
            action = MOTOR_ACTION_STOP_KEEP_CURRENT_RAMP;
            pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
        }
        if (feed_gap_steps > feed_speed_step_window_low && feed_gap_steps < feed_speed_step_window_high && feed_speed_step_phase != 1) {
            feed_speed_step_target = 4;
            feed_speed_step_phase = 1;
        }
        break;
    case FEED_STATE_EJECT:
        if (job_capture_flags.bit.pause_requested) {
            if (mailbox.motion_flags.b & MDC_MOTION_SIDES) {
                state = FEED_STATE_CAPTURE_STOP;
                action = MOTOR_ACTION_STOP_KEEP_CURRENT_RAMP;
                pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            } else if (job_top_clear_steps < 0x37bUL) {
                state = FEED_STATE_CAPTURE_STOP;
                action = MOTOR_ACTION_STOP_KEEP_CURRENT_RAMP;
                pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            }
        }
        if (feed_wait_steps == 0) {
            if (pick_state == PICK_STATE_WAIT_TOP || pick_state == PICK_STATE_TOP_RETRY || pick_state == PICK_STATE_TOP_RETRY_WAIT || pick_state == PICK_STATE_TOP_RETRY_FEED) {
                state = FEED_STATE_TRANSPORT;
            } else if (pick_state == PICK_STATE_CONTINUOUS_GAP || pick_state == PICK_STATE_CONTINUOUS_RESTART) {
                state = FEED_STATE_TRANSPORT;
                if (!(job_busy_flags.b & JOB_BUSY_SCAN_OR_EJECT) && ramp_state == RAMP_STATE_CONSTANT) {
                    action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                    feed_ready_stop = 1;
                }
            } else {
                if (!sensor_flags.bit.pick)
                    feed_pick_edge_count = 0;
                state = FEED_STATE_STOP;
            }
        } else {
            if (sensor_flags.bit.top || pick_ramp_state != RAMP_STATE_STOPPED) {
                state = FEED_STATE_TRANSPORT;
            } else if (job_busy_flags.b & JOB_BUSY_PICK) {
                state = FEED_STATE_TRANSPORT;
            } else if (feed_ramp_state == RAMP_STATE_STOPPED) {
                if (!sensor_flags.bit.pick)
                    feed_pick_edge_count = 0;
                state = FEED_STATE_STOP;
                if (pick_state == PICK_STATE_CONTINUOUS_GAP || pick_state == PICK_STATE_CONTINUOUS_RESTART)
                    state = FEED_STATE_TRANSPORT;
            }
        }
        break;
    case FEED_STATE_STOP:
        feed_wait_ticks = 0x64;
        action = MOTOR_ACTION_STOP_KEEP_CURRENT;
        state = FEED_STATE_WAIT_STOPPED;
        break;
    case FEED_STATE_WAIT_STOPPED:
        if (feed_wait_ticks == 0 && action == MOTOR_ACTION_NONE) {
            action = FEED_ACTION_ARM_HOME;
            state = FEED_STATE_IDLE;
        }
        break;
    case FEED_STATE_ULTRASONIC_IDLE:
        multifeed_check.flags.w = 0;
        multifeed_check.flags.bit.b14 = 0;
        multifeed_check.sample_count = 0;
        state = FEED_STATE_ULTRASONIC_START;
        break;
    case FEED_STATE_ULTRASONIC_PAPER:
        multifeed_check.flags.w = 0;
        multifeed_check.flags.bit.b14 = 1;
        multifeed_check.sample_count = 0;
        state = FEED_STATE_ULTRASONIC_START;
        break;
    case FEED_STATE_ULTRASONIC_START:
        multifeed_check.sample_countdown = 0;
        multifeed_check.sample_total = 0;
        multifeed_check.flags.bit.b9 = 0;
        multifeed_check.flags.bit.b15 = 1;
        state = FEED_STATE_ULTRASONIC_SAMPLE;
        break;
    case FEED_STATE_ULTRASONIC_SAMPLE:
        if (multifeed_check.flags.bit.b15) {
            if (multifeed_check.sample_countdown-- == 0) {
                if (multifeed_check.flags.bit.b14) {
                    if (multifeed_check.sample_count < ULTRASONIC_SAMPLES) {
                        multifeed_check.sample_countdown = multifeed_check.sample_interval;
                        p_chip_a_motor->period_1_high.f_0_8.f = 1;
                        p_chip_a_motor->period_1_low.f_0_8.f = 0xf7;
                        sensor_set_ultrasonic_sampling(1);
                    } else {
                        multifeed_check.flags.bit.b15 = 0;
                        multifeed_check.paper_level = multifeed_check.sum;
                        multifeed_check.sum = 0;
                        state = FEED_STATE_ULTRASONIC_DONE;
                    }
                } else {
                    if (multifeed_check.sample_count < ULTRASONIC_SAMPLES) {
                        multifeed_check.sample_countdown = multifeed_check.sample_interval;
                        p_chip_a_motor->period_1_high.f_0_8.f = 1;
                        p_chip_a_motor->period_1_low.f_0_8.f = 0xf7;
                        sensor_set_ultrasonic_sampling(0);
                    } else {
                        multifeed_check.flags.bit.b15 = 0;
                        multifeed_check.idle_level = multifeed_check.sum;
                        multifeed_check.sum = 0;
                        state = FEED_STATE_ULTRASONIC_DONE;
                    }
                }
            }
        }
        break;
    case FEED_STATE_CAPTURE_STOP:
        if (feed_ramp_state == RAMP_STATE_STOPPED) {
            state = FEED_STATE_CAPTURE_SETTLE;
            feed_wait_ticks = 0x64;
        }
        break;
    case FEED_STATE_CAPTURE_SETTLE:
        if (feed_wait_ticks == 0) {
            util_put_be16(&mailbox.capture_steps_high, 0);
            mailbox.capture_flags.b |= MDC_CAPTURE_STEPS_VALID;
            state = FEED_STATE_CAPTURE_PAUSED;
        }
        break;
    case FEED_STATE_CAPTURE_PAUSED:
        if (job_capture_flags.bit.resume_requested) {
            job_capture_flags.bit.resume_requested = 0;
            state = FEED_STATE_CAPTURE_RESUME;
            action = MOTOR_ACTION_START;
        }
        break;
    case FEED_STATE_CAPTURE_RESUME:
        if (ramp_state == RAMP_STATE_CONSTANT) {
            mailbox.capture_flags.b &= ~MDC_CAPTURE_PAUSED;
            mailbox.capture_flags.b &= ~MDC_CAPTURE_STEPS_VALID;
            state = FEED_STATE_TRANSPORT;
        }
        break;
    }
    feed_state = state;
    if (feed_pick_start_flags.w)
        feed_check_pick_start();
    if ((state == FEED_STATE_TRANSPORT || state == FEED_STATE_EJECT || state == FEED_STATE_CAPTURE_STOP || state == FEED_STATE_CAPTURE_SETTLE || state == FEED_STATE_CAPTURE_PAUSED || state == FEED_STATE_CAPTURE_RESUME) && feed_ramp_state) {
        edge_word = feed_edge_records[0].edge;
        feed_in_transport = 1;
        if (!sensor_flags.bit.top) {
            if ((count = *edge_word) != 0) {
                *edge_word = ++count;
            }
        }
        edge_word++;
        if (!sensor_flags.bit.pick) {
        }
        edge_word++;
        if (sensor_flags.bit.top) {
        }
        edge_word++;
        edge_word++;
        if (feed_sheet_active[0]) {
            if (feed_sheet_counters[0] == job_front_end_steps) {
                set_imask_exr(7);
                if (mailbox.long_page == MDC_LONG_PAGE_ON) {
                    if (sensor_flags.bit.top || sensor_flags.bit.pick || !sensor_flags.bit.no_paper)
                        set_imask_exr(7);
                    else
                        mailbox.motion_flags.b &= ~MDC_MOTION_FRONT;
                } else {
                    mailbox.motion_flags.b &= ~MDC_MOTION_FRONT;
                }
                set_imask_exr(feed_saved_imask);
            } else if (feed_sheet_counters[0] == job_back_end_steps) {
                if (mailbox.long_page == MDC_LONG_PAGE_ON && (mailbox.motion_flags.b & MDC_MOTION_FRONT)) {
                } else {
                    mailbox.motion_flags.b &= ~MDC_MOTION_BACK;
                    mailbox.motion_flags.b &= ~MDC_MOTION_BACK_PENDING;
                    feed_sheet_active[0] = 0;
                    if (feed_speed_step_phase == 2 || feed_speed_step_phase == 1) {
                        feed_speed_step_target = 0;
                        feed_speed_step_phase = 0;
                    }
                    log_error_word.w++;
                    log_hook_begin(0);
                    set_imask_exr(7);
                    p_log_buf[log_index] = LOG_SHEET_END;
                    log_advance();
                    p_log_buf[log_index] = log_error_word.byte.l;
                    log_advance();
                    if (log_frozen == 0) {
                        p_log_buf[log_index] = job_state;
                        log_advance();
                    }
                    p_log_buf[log_index] = LOG_END;
                    set_imask_exr(feed_saved_imask);
                    log_end();
                }
            }
        }
        if (feed_sheet_active[1]) {
            if (feed_sheet_counters[1] == job_front_end_steps) {
                set_imask_exr(7);
                if (mailbox.long_page == MDC_LONG_PAGE_ON) {
                    if (sensor_flags.bit.top || sensor_flags.bit.pick || !sensor_flags.bit.no_paper)
                        set_imask_exr(7);
                    else
                        mailbox.motion_flags.b &= ~MDC_MOTION_FRONT;
                } else {
                    mailbox.motion_flags.b &= ~MDC_MOTION_FRONT;
                }
                set_imask_exr(feed_saved_imask);
            } else if (feed_sheet_counters[1] == job_back_end_steps) {
                if (mailbox.long_page == MDC_LONG_PAGE_ON && (mailbox.motion_flags.b & MDC_MOTION_FRONT)) {
                } else {
                    mailbox.motion_flags.b &= ~MDC_MOTION_BACK;
                    mailbox.motion_flags.b &= ~MDC_MOTION_BACK_PENDING;
                    feed_sheet_active[1] = 0;
                    if (feed_speed_step_phase == 2 || feed_speed_step_phase == 1) {
                        feed_speed_step_target = 0;
                        feed_speed_step_phase = 0;
                    }
                    log_error_word.w++;
                    log_hook_begin(0);
                    set_imask_exr(7);
                    p_log_buf[log_index] = LOG_SHEET_END;
                    log_advance();
                    p_log_buf[log_index] = log_error_word.byte.l;
                    log_advance();
                    if (log_frozen == 0) {
                        p_log_buf[log_index] = job_state;
                        log_advance();
                    }
                    p_log_buf[log_index] = LOG_END;
                    set_imask_exr(feed_saved_imask);
                    log_end();
                }
            }
        }
    } else {
        if (state == FEED_STATE_WAIT_SHEET)
            feed_in_transport = 1;
        else
            feed_in_transport = 0;
    }
    if (action) {
        switch (action) {
        case MOTOR_ACTION_OFF:
            set_imask_exr(7);
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_OFF);
            set_imask_exr(feed_saved_imask);
            feed_current_mode = 0;
            if (ramp_state) {
                feed_ramp_state = ramp_state = RAMP_STATE_STOPPED;
            }
            action = MOTOR_ACTION_NONE;
            break;
        case MOTOR_ACTION_START:
            if (ramp_state == RAMP_STATE_STOPPED) {
                set_imask_exr(7);
                motor_set_current(MOTOR_FEED, MOTOR_CURRENT_START);
                set_imask_exr(feed_saved_imask);
                feed_current_mode = CURRENT_MODE_START;
                feed_start_ticks = 0x19;
                action = MOTOR_ACTION_START_RAMP;
            } else if (ramp_state == RAMP_STATE_CONSTANT) {
                action = MOTOR_ACTION_NONE;
            }
            break;
        case MOTOR_ACTION_START_RAMP:
            if (feed_start_ticks == 0) {
                feed_start(feed_ramp_steps);
                action = MOTOR_ACTION_NONE;
            }
            break;
        case MOTOR_ACTION_STOP:
            if (ramp_state == RAMP_STATE_CONSTANT) {
                set_imask_exr(7);
                motor_set_current(MOTOR_FEED, MOTOR_CURRENT_START);
                set_imask_exr(feed_saved_imask);
                feed_current_mode = CURRENT_MODE_STOP;
                action = MOTOR_ACTION_STOP_RAMP;
            } else if (ramp_state == RAMP_STATE_STOPPED) {
                action = MOTOR_ACTION_NONE;
            }
            break;
        case MOTOR_ACTION_STOP_RAMP:
            feed_stop();
            action = MOTOR_ACTION_WAIT_STOPPED;
            break;
        case MOTOR_ACTION_STOP_KEEP_CURRENT:
            if (ramp_state == RAMP_STATE_CONSTANT) {
                set_imask_exr(7);
                motor_set_current(MOTOR_FEED, MOTOR_CURRENT_START);
                set_imask_exr(feed_saved_imask);
                feed_current_mode = CURRENT_MODE_STOP;
                action = MOTOR_ACTION_STOP_KEEP_CURRENT_RAMP;
            } else if (ramp_state == RAMP_STATE_STOPPED) {
                action = MOTOR_ACTION_NONE;
            }
            break;
        case MOTOR_ACTION_STOP_KEEP_CURRENT_RAMP:
            feed_stop_keep_current();
            feed_ramp_stopped = 1;
            if (job_capture_flags.bit.pause_requested) {
                job_capture_flags.bit.pause_requested = 0;
                mailbox.capture_flags.b |= MDC_CAPTURE_PAUSED;
            }
            action = MOTOR_ACTION_WAIT_STOPPED;
            break;
        case MOTOR_ACTION_WAIT_STOPPED:
            if (ramp_state == RAMP_STATE_STOPPED)
                action = MOTOR_ACTION_OFF;
            break;
        case MOTOR_ACTION_CHANGE_SPEED:
            set_imask_exr(7);
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_START);
            set_imask_exr(feed_saved_imask);
            feed_current_mode = CURRENT_MODE_START;
            action = FEED_ACTION_CHANGE_SPEED_RAMP;
            break;
        case FEED_ACTION_CHANGE_SPEED_RAMP:
            if (ramp_state == RAMP_STATE_CONSTANT) {
                feed_change_speed(feed_ramp_steps, feed_ramp_steps_next);
                feed_ramp_steps = feed_ramp_steps_next;
                action = MOTOR_ACTION_NONE;
            } else if (ramp_state == RAMP_STATE_STOPPED) {
                feed_start(feed_ramp_steps_next);
                feed_ramp_steps = feed_ramp_steps_next;
                action = MOTOR_ACTION_NONE;
            }
            break;
        case FEED_ACTION_ARM_HOME:
            if (sensor_flags.bit.adf_cover_open) {
                action = MOTOR_ACTION_OFF;
            } else if (!sensor_flags.bit.top && !sensor_flags.bit.pick && sensor_flags.bit.no_paper) {
                bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                action = FEED_ACTION_WAIT_ARM_HOME;
            } else {
                bw_motor_arm_idle_ticks = 0x9c4;
                action = MOTOR_ACTION_NONE;
            }
            lamp_adf_off_count = lamp_adf_off_limit;
            break;
        case FEED_ACTION_WAIT_ARM_HOME:
            if (bw_motor_arm_request == 0)
                action = MOTOR_ACTION_OFF;
            break;
        }
        feed_action = action;
    }
    if (ramp_state) {
        switch (feed_pick_edge_state) {
        case 0:
            if (sensor_flags.bit.pick && ramp_state == RAMP_STATE_CONSTANT) {
                feed_edge_records[++feed_pick_edge_count % FEED_EDGE_RECORD_COUNT].edge[0] = 1;
                if (mailbox.sheet_count_low == 0xff) {
                    mailbox.sheet_count_low = 0;
                    mailbox.sheet_count_high++;
                } else {
                    mailbox.sheet_count_low++;
                }
                feed_sheets_in_path++;
                feed_pick_edge_state = 2;
            }
            break;
        case 2:
            if (!sensor_flags.bit.pick) {
                feed_edge_records[(feed_pick_edge_count - 1) % FEED_EDGE_RECORD_COUNT].edge[1] = 1;
                feed_pick_edge_state = 0;
            }
            break;
        }
        switch (feed_top_edge_state) {
        case 0:
            if (sensor_flags.bit.top) {
                mailbox.motion_flags.b |= MDC_MOTION_AT_TOP;
                feed_edge_records[++feed_top_edge_count % FEED_EDGE_RECORD_COUNT].edge[2] = 1;
                feed_top_edge_state = 2;
            }
            break;
        case 2:
            if (!sensor_flags.bit.top) {
                if (feed_sheets_in_path)
                    feed_sheets_in_path--;
                feed_edge_records[(feed_top_edge_count - 1) % FEED_EDGE_RECORD_COUNT].edge[3] = 1;
                mailbox.motion_flags.b &= ~MDC_MOTION_AT_TOP;
                feed_top_edge_state = 0;
            }
            break;
        }
    }
    if (feed_current_mode == CURRENT_MODE_START && ramp_state == RAMP_STATE_CONSTANT && feed_start_ticks == 0) {
        set_imask_exr(7);
        motor_set_current(MOTOR_FEED, feed_run_current);
        set_imask_exr(feed_saved_imask);
        feed_current_mode = CURRENT_MODE_RUN;
    }
    if (feed_auto_scan == FEED_AUTO_SCAN_POST_FEED) {
        if (err_flags.b != 0 || sensor_flags.bit.adf_cover_open) {
            mailbox.cmd_code = MDC_CMD_CLEAR_ERRORS;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            feed_auto_scan = FEED_AUTO_SCAN_OFF;
        } else if (mailbox.cmd_state == MDC_CMD_STATE_DONE) {
            if (mailbox.capture_flags.b & MDC_CAPTURE_PAUSED) {
                if (mailbox.motion_flags.b == 0) {
                    set_imask_exr(7);
                    mailbox.cmd_code = MDC_CMD_FEED;
                    job_set_cmd_state(MDC_CMD_STATE_POSTED);
                    feed_auto_scan = FEED_AUTO_SCAN_POST_SCAN_MOVE;
                    set_imask_exr(feed_saved_imask);
                }
            } else {
                set_imask_exr(7);
                mailbox.cmd_code = MDC_CMD_FEED;
                job_set_cmd_state(MDC_CMD_STATE_POSTED);
                feed_auto_scan = FEED_AUTO_SCAN_POST_SCAN_MOVE;
                set_imask_exr(feed_saved_imask);
            }
        }
    } else if (feed_auto_scan == FEED_AUTO_SCAN_POST_SCAN_MOVE) {
        if (err_flags.b != 0 || sensor_flags.bit.adf_cover_open) {
            mailbox.cmd_code = MDC_CMD_CLEAR_ERRORS;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            feed_auto_scan = FEED_AUTO_SCAN_OFF;
        } else if (mailbox.cmd_state == MDC_CMD_STATE_DONE && mailbox.cmd_result == MDC_RESULT_SHEET_READY) {
            set_imask_exr(7);
            mailbox.cmd_code = MDC_CMD_SCAN_MOVE;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            feed_auto_scan = FEED_AUTO_SCAN_POST_FEED;
            set_imask_exr(feed_saved_imask);
        }
    }
    if (err_history_last != mailbox.error_code) {
        err_history_last = mailbox.error_code;
        if (mailbox.error_code != 0) {
            if (err_history_index > 0x13)
                err_history_index = 0;
            err_history[err_history_index++] = mailbox.error_code;
        }
    }
}

void feed_read_sensors(union byte_view inputs)
{
    if (feed_ramp_state) {
        if (job_pick_gap_steps < 0xffff) {
            job_pick_gap_steps++;
        }
        if (feed_pick_on_steps < 0xffff) {
            feed_pick_on_steps++;
        }
        if (feed_pick_off_steps < 0xffff) {
            feed_pick_off_steps++;
        }
    }
    if (!inputs.bit.b1) {
        if (sensor_pick_count < SENSOR_DEBOUNCE_MAX) {
            sensor_pick_count++;
        }
    } else {
        if (sensor_pick_count != 0) {
            sensor_pick_count--;
        }
    }
    if (sensor_pick_count > SENSOR_DEBOUNCE_ON) {
        if (!sensor_flags.bit.pick) {
            job_pick_gap_steps = 0;
            if (feed_ramp_state) {
                feed_pick_on_steps = sensor_pick_count;
            } else {
                feed_pick_on_steps = 0;
            }
            if (pick_sensor_on_steps > feed_ultrasonic_limit) {
                if (pick_ramp_state) {
                    pick_sensor_on_steps = pick_sensor_count;
                } else {
                    pick_sensor_on_steps = 0;
                }
            }
        }
        sensor_flags.bit.pick = 1;
        mailbox.sensor_status.bit.pick = 1;
    } else {
        if (sensor_flags.bit.pick) {
            job_pick_gap_steps = 0;
            if (feed_speed_step_phase == 1) {
                feed_speed_step_target = 8;
                feed_speed_step_phase = 2;
            }
            if (feed_ramp_state) {
                feed_pick_off_steps = 5 - sensor_pick_count;
            } else {
                feed_pick_off_steps = 0;
            }
        }
        sensor_flags.bit.pick = 0;
        mailbox.sensor_status.bit.pick = 0;
    }
    if (feed_ramp_state) {
        if (feed_jam_steps < feed_jam_limit) {
            feed_jam_steps++;
            /* no TOP edge for too long: eject jam */
            if (feed_jam_steps == feed_jam_limit) {
                if (feed_jam_steps != FEED_JAM_DISABLED) {
                    if (mailbox.long_page == MDC_LONG_PAGE_ON) {
                    } else {
                        err_flags.bit.document = 1;
                        err_code = ERR_EJECT_JAM;
                    }
                    feed_jam_steps = 0;
                }
            }
        }
        if (feed_gap_steps != 0xffff) {
            feed_gap_steps++;
        }
    }
    if (feed_top_edge_steps < 0xffff) {
        if (feed_ramp_state) {
            feed_top_edge_steps++;
            feed_sheet_counters[0]++;
            feed_sheet_counters[1]++;
        }
    }
    if (sensor_top_count > sensor_top_threshold) {
        if (!sensor_flags.bit.top) {
            multifeed_check.overlap_reference = feed_pick_on_steps;
            sensor_top_count = sensor_top_threshold;
            if (feed_gap_index < 0x11) {
                feed_gap_index++;
            }
            if (sensor_df_test) {
                mailbox.df_test_result.bit.top_edge = 1;
            }
            if (feed_top_edge_steps <= 0xffe9) {
                feed_top_edge_steps += 0x16;
            }
            if (feed_clearing_path) {
            } else {
                if (feed_top_window_flags.w & FEED_TOP_WINDOW_GAP_CORRECTION) {
                    feed_top_window_flags.bit.gap_correction = 0;
                    feed_gap_correction_steps = feed_top_window_on_steps / 3;
                    if (feed_top_edge_steps > feed_gap_correction_steps) {
                        feed_top_edge_steps -= feed_gap_correction_steps;
                    } else {
                        feed_top_edge_steps = 1;
                    }
                }
                /* the gap to the previous sheet is too short */
                if (feed_top_edge_steps < FEED_MIN_SHEET_GAP_STEPS && feed_ramp_state) {
                    err_flags.bit.document = 1;
                    err_code = ERR_SHEET_GAP;
                }
            }
            feed_gap_history[feed_gap_index] = feed_top_edge_steps;
            feed_top_edge_steps = 0;
            feed_jam_steps = 0;
            feed_gap_steps = 0;
        } else {
            if (feed_fast_ramp == 1) {
                if (!(feed_top_window_flags.w & FEED_TOP_WINDOW_ARMED)) {
                    if (sensor_top_count == sensor_top_count_limit) {
                        feed_top_window_flags.bit.armed = 1;
                    }
                } else {
                    if (sensor_top_count < feed_top_off_threshold) {
                        if (!feed_top_window_flags.bit.open) {
                            feed_top_window_flags.bit.open = 1;
                            feed_top_window_flags.w &= ~(FEED_TOP_WINDOW_GAP_CORRECTION | FEED_TOP_WINDOW_TRAILING_EDGE | FEED_TOP_WINDOW_ARMED);
                            feed_gap_correction_steps = 0;
                            feed_top_window_on_steps = 0;
                            feed_top_window_steps = 0x68;
                        }
                    }
                }
            }
        }
        /* longer than any sheet: eject jam */
        if (feed_max_sheet_length != 0xffff && feed_top_edge_steps > feed_max_sheet_length) {
            if (mailbox.long_page == MDC_LONG_PAGE_ON) {
            } else {
                err_flags.bit.document = 1;
                err_code = ERR_EJECT_JAM;
            }
            feed_top_edge_steps = 0;
        }
        sensor_flags.bit.top = 1;
        mailbox.sensor_status.bit.top = 1;
        mailbox.motion_flags.bit.at_top = 1;
    } else {
        if ((sensor_flags.bit.adf_cover_open || sensor_cover_count) && (mailbox.motion_flags.b & MDC_MOTION_FRONT) && feed_ramp_state && !feed_clearing_path) {
            if (sensor_flags.bit.top && mailbox.cover_open_top_count < 0xff) {
                mailbox.cover_open_top_count++;
            }
        } else {
            if (sensor_flags.bit.top) {
                if (feed_length_index < 0x11) {
                    feed_length_index++;
                }
                if (!(mailbox.feed_options.b & MDC_FEED_LENGTH_CHECK)) {
                    feed_length_index = 0;
                }
                feed_length_history[feed_length_index] = feed_top_edge_steps;
                util_put_be16(&mailbox.sheet_length_high, feed_top_edge_steps);
                if ((mailbox.feed_options.b & MDC_FEED_LENGTH_CHECK) && feed_ramp_state && !feed_clearing_path && feed_length_history[1] > FEED_MIN_LENGTH_CHECK_STEPS
                    && (feed_top_edge_steps > feed_length_history[1] + job_length_tolerance || feed_top_edge_steps < feed_length_history[1] - job_length_tolerance)) {
                    if (mailbox.feed_options.b & MDC_FEED_MULTIFEED_REPORT_ONLY) {
                        mailbox.multifeed_flags.bit.length = 1;
                    } else {
                        mailbox.multifeed_flags.bit.length = 1;
                        err_flags.bit.document = 1;
                        err_code = ERR_MULTIFEED_LENGTH;
                        feed_pick_edge_state = 0xff;
                    }
                }
                log_hook_begin(0);
                set_imask_exr(7);
                p_log_buf[log_index] = LOG_SHEET_LENGTH;
                log_advance();
                p_log_buf[log_index] = (unsigned char)(feed_top_edge_steps >> 8);
                log_advance();
                p_log_buf[log_index] = (unsigned char)feed_top_edge_steps;
                log_advance();
                p_log_buf[log_index] = LOG_END;
                set_imask_exr(feed_saved_imask);
                log_end();
                feed_top_edge_steps = 0;
                feed_jam_steps = 0;
                if (job_sheet_slot) {
                    job_sheet_slot = 0;
                } else {
                    job_sheet_slot = 1;
                }
                feed_sheet_active[job_sheet_slot] = 1;
                feed_sheet_counters[job_sheet_slot] = 0;
                if (feed_top_window_flags.bit.open) {
                    feed_top_window_flags.bit.trailing_edge = 1;
                }
                if (!feed_clearing_path && !sensor_flags.bit.adf_cover_open) {
                    if (mailbox.motion_flags.b & MDC_MOTION_FRONT) {
                        if (mailbox.source & MDC_SOURCE_ADF) {
                            if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE && mailbox.cmd_state == MDC_CMD_STATE_ENDED) {
                                if (mailbox.motion_flags.b & MDC_MOTION_BACK) {
                                    mailbox.gap_error_both_count++;
                                } else {
                                    mailbox.gap_error_front_count++;
                                }
                                err_flags.bit.document = 1;
                                err_code = ERR_SHEET_GAP;
                            }
                        }
                    } else if ((mailbox.cmd_code == MDC_CMD_FEED || mailbox.cmd_code == MDC_CMD_SCAN_MOVE || (mailbox.motion_flags.b & MDC_MOTION_BACK)) && feed_ramp_state) {
                        mailbox.gap_error_stopped_count++;
                        err_flags.bit.document = 1;
                        err_code = ERR_SHEET_GAP;
                    }
                }
            }
            sensor_flags.bit.top = 0;
            mailbox.sensor_status.bit.top = 0;
            mailbox.motion_flags.bit.at_top = 0;
        }
    }
    if (mailbox.manual_feed_timeout) {
        if (inputs.bit.b4) {
            if (sensor_empty_count < SENSOR_DEBOUNCE_MAX) {
                sensor_empty_count++;
            }
            if (feed_manual_empty_count < SENSOR_DEBOUNCE_MAX) {
                feed_manual_empty_count++;
            }
        } else {
            if (sensor_empty_count) {
                sensor_empty_count--;
            }
            if (feed_manual_empty_count) {
                feed_manual_empty_count--;
            }
        }
        if (sensor_empty_count > SENSOR_DEBOUNCE_ON) {
            feed_manual_timeout_limit = feed_manual_timeout_seconds[mailbox.manual_feed_timeout];
            if (feed_empty_seconds > feed_manual_timeout_limit) {
                sensor_flags.bit.no_paper = 1;
                mailbox.sensor_status.bit.no_paper = 1;
            }
        } else {
            sensor_flags.bit.no_paper = 0;
            mailbox.sensor_status.bit.no_paper = 0;
        }
        if (feed_manual_empty_count > SENSOR_DEBOUNCE_ON) {
            feed_manual_empty = 1;
            mailbox.sensor_status_2.bit.manual_feed_empty = 1;
        } else {
            feed_manual_empty = 0;
            mailbox.sensor_status_2.bit.manual_feed_empty = 0;
        }
    } else {
        if (inputs.bit.b4) {
            if (sensor_empty_count < SENSOR_DEBOUNCE_MAX) {
                sensor_empty_count++;
            }
        } else {
            if (sensor_empty_count) {
                sensor_empty_count--;
            }
        }
        if (sensor_empty_count > SENSOR_DEBOUNCE_ON) {
            sensor_flags.bit.no_paper = 1;
            mailbox.sensor_status.bit.no_paper = 1;
        } else {
            sensor_flags.bit.no_paper = 0;
            mailbox.sensor_status.bit.no_paper = 0;
        }
    }
}

void feed_check_ultrasonic(void)
{
    struct ultrasonic_check *check;
    struct ultrasonic_check *check_2;
    short i;
    unsigned short position;
    unsigned short sample_flags;
    unsigned short overlap_index;
    int res;

    check = &multifeed_check;
    check_2 = &multifeed_check;
    if (feed_clearing_path) {
    } else {
        if (feed_ramp_state) {
            if (!sensor_flags.bit.top) {
                check->overlap_pending = 0;
            } else {
                if (check->positions[0]) {
                    if (check->overlap_reference > feed_ultrasonic_sample_offset) {
                        check->overlap_reference -= feed_ultrasonic_sample_offset;
                    } else {
                        check->overlap_reference = 0;
                    }
                    for (i = 0; i <= check->position_count; i++) {
                        if (check->ends[i] <= check->overlap_reference + 0x14) {
                            continue;
                        }
                        check->overlap_pending = 1;
                        if (check->positions[i] > check->overlap_reference) {
                            feed_overlap_starts[i] = check->positions[i] - check->overlap_reference;
                            check->overlap_reference = check->positions[i];
                        } else {
                            feed_overlap_starts[i] = 1;
                        }
                        if (check->ends[i] != 0xffff) {
                            feed_overlap_lengths[i] = check->ends[i] - check->overlap_reference;
                            check->overlap_count++;
                        } else {
                            check->overlap_pending = 0xff;
                            break;
                        }
                    }
                    check->positions[0] = 0;
                    check->position_count = 0;
                }
            }
            if (!(pick_flags_1.b & 4) && check->in_area) {
                if (check->sample_countdown-- == 0) {
                    set_imask_exr(7);
                    if (!sensor_flags.bit.top) {
                        position = pick_sensor_on_steps * 100 / 46;
                        if (position >= feed_ultrasonic_sample_limit) {
                            position = feed_ultrasonic_sample_limit - 1;
                        }
                    } else if (mailbox.speed_class_b < 4) {
                        position = (feed_top_steps + 2) / 3 + feed_ultrasonic_sample_offset;
                    } else {
                        position = (feed_top_steps + 3) / 6 + feed_ultrasonic_sample_offset;
                    }
                    sample_flags = check->overflow_flags.w;
                    set_imask_exr(feed_saved_imask);
                    overlap_index = check->overlap_count;
                    if (sensor_flags.bit.top && (check->low_count > check->low_limit || check->overlap_pending)) {
                        pick_flags_1.bit.ultrasonic_multifeed = 1;
                        if (mailbox.feed_options.b & MDC_FEED_ULTRASONIC_CHECK) {
                            if (mailbox.feed_options.b & MDC_FEED_MULTIFEED_REPORT_ONLY) {
                                feed_multifeed_flags.bit.ultrasonic = 1;
                            } else {
                                res = feed_multifeed_can_pass();
                                if (res == 0) {
                                    feed_multifeed_flags.bit.ultrasonic = 1;
                                    mailbox.multifeed_flags.b |= feed_multifeed_flags.b;
                                    err_flags.bit.document = 1;
                                    err_code = ERR_MULTIFEED_ULTRASONIC;
                                    feed_pick_edge_state = 0xff;
                                } else {
                                    if (!(job_overlap_check_flags.w & JOB_OVERLAP_CHECK_FOUND)) {
                                        job_overlap_check_flags.bit.found = 1;
                                    }
                                    pick_flags_1.bit.ultrasonic_multifeed = 0;
                                    if (overlap_index != 0xff && feed_overlap_starts[overlap_index] == 0 && check->overlap_pending == 0) {
                                        feed_overlap_starts[overlap_index] = position;
                                    } else {
                                        check->overlap_pending = 0;
                                    }
                                }
                            }
                        }
                        if (!(job_overlap_check_flags.w & JOB_OVERLAP_CHECK_FOUND)) {
                            sensor_stop_ultrasonic();
                            feed_ultrasonic_running = 0;
                            check_2->in_area = 0;
                            multifeed_check.flags.bit.b8 = 0;
                            return;
                        } else {
                            multifeed_check.sample_countdown = multifeed_check.sample_interval;
                            sensor_set_ultrasonic_sampling(1);
                        }
                    } else {
                        multifeed_check.sample_countdown = multifeed_check.sample_interval;
                        if (sample_flags & 1) {
                            check->overflow_flags.w &= 0xfffe;
                            if (overlap_index == 0xff) {
                            } else {
                                if (position > feed_overlap_starts[overlap_index]) {
                                    position -= feed_overlap_starts[overlap_index];
                                } else {
                                    position = 1;
                                }
                                feed_overlap_lengths[overlap_index] = position;
                                overlap_index++;
                                if (overlap_index >= 4) {
                                    overlap_index = 0xff;
                                }
                                check->overlap_count = overlap_index;
                            }
                        }
                        sensor_set_ultrasonic_sampling(1);
                    }
                }
            }
            if (sensor_flags.bit.pick && pick_sensor_on_steps < feed_ultrasonic_limit) {
                feed_pick_on_steps = pick_sensor_on_steps * 100 / 46;
            }
            if (pick_sensor_on_steps >= feed_ultrasonic_limit && feed_pick_on_steps == feed_ultrasonic_check_count_a) {
                feed_start_ultrasonic_check();
                feed_ultrasonic_starts_a++;
            } else if (sensor_flags.bit.pick && feed_ultrasonic_running == 0 && !(pick_flags_1.b & 4)
                       && pick_sensor_on_steps < feed_ultrasonic_limit && pick_sensor_on_steps >= feed_ultrasonic_limit_low) {
                feed_start_ultrasonic_check();
                feed_ultrasonic_starts_b++;
            }
            if (feed_ultrasonic_running) {
                res = feed_in_check_area();
                if (res == 0) {
                    check->in_area = 0;
                } else if (check->flags.bit.b8 && check->in_area == 0) {
                    check->in_area = 1;
                    check->sample_count = 0;
                    check->low_count = 0;
                    check->w32 = 0;
                }
            }
            if (feed_pick_off_steps == feed_ultrasonic_check_count_b - 8
                && (feed_ultrasonic_running || (mailbox.feed_options.b & MDC_FEED_MULTIFEED_REPORT_ONLY) || !(mailbox.feed_options.b & MDC_FEED_ULTRASONIC_CHECK))) {
                {
                    if (job_overlap_check_flags.w & JOB_OVERLAP_CHECK_FOUND) {
                        if (check->low_count > check->low_limit || check->overlap_pending) {
                            if (overlap_index == 0xff) {
                            } else {
                                if (position > feed_overlap_starts[overlap_index]) {
                                    position -= feed_overlap_starts[overlap_index];
                                } else {
                                    position = 1;
                                }
                                feed_overlap_lengths[overlap_index] = position;
                                overlap_index++;
                                if (overlap_index >= 4) {
                                    overlap_index = 0xff;
                                }
                                check->overlap_count = overlap_index;
                            }
                        }
                        feed_overlap_match_state = OVERLAP_MATCH_START;
                        res = feed_match_overlap();
                        if (res == 0) {
                            job_overlap_check_flags.bit.unmatched = 1;
                            feed_overlap_match_state = OVERLAP_MATCH_START;
                            err_flags.bit.document = 1;
                            err_code = ERR_MULTIFEED_ULTRASONIC;
                            feed_pick_edge_state = 0xff;
                            job_overlap_check_flags.bit.found = 0;
                        } else if (res == 1) {
                            pick_flags_1.bit.ultrasonic_multifeed = 0;
                            job_overlap_check_flags.bit.unmatched = 0;
                            feed_overlap_match_state = OVERLAP_MATCH_START;
                            job_overlap_check_flags.bit.found = 0;
                        } else {
                            job_overlap_check_flags.bit.unmatched = 1;
                        }
                    }
                    feed_ultrasonic_running = 0;
                    multifeed_check.w44 = multifeed_check.sample_count;
                    check_2->in_area = 0;
                    check_2->sample_count = 0;
                    check_2->sample_countdown = 0;
                    check_2->low_count = 0;
                    check_2->sum = 0;
                    check_2->w36 = 0;
                    check_2->w34 = 0;
                    check_2->w32 = 0;
                    check_2->flags.bit.b8 = 1;
                    sensor_stop_ultrasonic();
                    pick_flags_1.bit.ultrasonic_multifeed = 0;
                    if (feed_overlap_hold_sheets && --feed_overlap_hold_sheets == 0) {
                        for (i = 0; i < 4; i++) {
                            feed_overlap_starts_saved[i] = 0;
                            feed_overlap_starts[i] = 0;
                            feed_overlap_lengths_saved[i] = 0;
                            feed_overlap_lengths[i] = 0;
                        }
                    }
                    if (feed_overlap_bypass && --feed_overlap_bypass == 0) {
                        for (i = 0; i < 4; i++) {
                            feed_overlap_starts_saved[i] = 0;
                            feed_overlap_starts[i] = 0;
                            feed_overlap_lengths_saved[i] = 0;
                            feed_overlap_lengths[i] = 0;
                        }
                    }
                    mailbox.multifeed_flags.b |= feed_multifeed_flags.b;
                }
            } else {
                if (sensor_flags.bit.top && feed_pick_off_steps > feed_ultrasonic_check_count_b - 8 && (job_overlap_check_flags.w & JOB_OVERLAP_CHECK_FOUND) && feed_overlap_match_state) {
                    res = feed_match_overlap();
                    if (res == 0) {
                        job_overlap_check_flags.bit.unmatched = 1;
                        feed_overlap_match_state = OVERLAP_MATCH_START;
                        err_flags.bit.document = 1;
                        err_code = ERR_MULTIFEED_ULTRASONIC;
                        feed_pick_edge_state = 0xff;
                        job_overlap_check_flags.bit.found = 0;
                    } else if (res == 1) {
                        pick_flags_1.bit.ultrasonic_multifeed = 0;
                        job_overlap_check_flags.bit.unmatched = 0;
                        feed_overlap_match_state = OVERLAP_MATCH_START;
                        job_overlap_check_flags.bit.found = 0;
                        return;
                    }
                }
            }
        }
    }
}

int feed_multifeed_can_pass(void)
{
    unsigned char overlap_mode;
    int skip;

    skip = 0;
    overlap_mode = job_overlap_flags.b & OVERLAP_MODE_MASK;
    if (overlap_mode == OVERLAP_MODE_MANUAL) {
        if (feed_overlap_bypass == 0) {
        } else {
            skip = 1;
        }
    } else {
        skip = 1;
    }
    return skip;
}

int feed_match_overlap(void)
{
    unsigned char overlap_mode;
    int res;
    short pattern;
    short entry;
    unsigned short length_low;
    unsigned short length_high;
    unsigned short length;
    unsigned short start_low;
    unsigned short start_high;
    unsigned short start;
    unsigned short matches;

    res = 0;
    overlap_mode = job_overlap_flags.b & OVERLAP_MODE_MASK;
    if (overlap_mode == OVERLAP_MODE_MANUAL) {
        if (feed_overlap_bypass) {
            res = 1;
        }
    } else {
        if (overlap_mode == OVERLAP_MODE_AUTO_2) {
            res = 2;
            switch (feed_overlap_match_state) {
            case OVERLAP_MATCH_START:
                feed_count_overlaps();
                feed_find_max_overlap_length();
                feed_overlap_match_state = OVERLAP_MATCH_SEARCHING;
                feed_overlap_search_index = 0;
                break;
            case OVERLAP_MATCH_SEARCHING:
                for (pattern = feed_overlap_search_index; pattern < OVERLAP_PATTERN_COUNT; pattern++) {
                    for (entry = 0; entry < 4; entry++) {
                        length_high = job_overlap_patterns_b[pattern][entry];
                        if (length_high == 0) {
                            break;
                        }
                        if ((unsigned long)length_high + OVERLAP_MATCH_TOLERANCE <= 0xffff) {
                            length_low = length_high + OVERLAP_MATCH_TOLERANCE;
                        } else {
                            length_low = 0xffff;
                        }
                        if (feed_overlap_max_length <= length_low) {
                            res = 1;
                            feed_overlap_match_state = OVERLAP_MATCH_FOUND;
                            return res;
                        }
                    }
                    if (entry == 0 && length_high == 0) {
                        res = 0;
                        feed_overlap_match_state = OVERLAP_MATCH_NONE;
                        break;
                    } else {
                        feed_overlap_search_index++;
                        if (feed_overlap_search_index >= OVERLAP_PATTERN_COUNT) {
                            res = 0;
                            feed_overlap_match_state = OVERLAP_MATCH_NONE;
                        }
                        break;
                    }
                }
            }
        } else {
            res = 2;
            switch (feed_overlap_match_state) {
            case OVERLAP_MATCH_START:
                feed_count_overlaps();
                feed_overlap_match_state = OVERLAP_MATCH_SEARCHING;
                feed_overlap_search_index = 0;
                break;
            case OVERLAP_MATCH_SEARCHING:
                for (pattern = feed_overlap_search_index; pattern < OVERLAP_PATTERN_COUNT; pattern++) {
                    matches = 0;
                    for (entry = 0; entry < 4; entry++) {
                        length = job_overlap_patterns_b[pattern][entry];
                        if (length == 0) {
                            break;
                        }
                        if (length >= OVERLAP_MATCH_TOLERANCE) {
                            length_low = length - OVERLAP_MATCH_TOLERANCE;
                        } else {
                            length_low = 0;
                        }
                        if ((unsigned long)length + OVERLAP_MATCH_TOLERANCE <= 0xffff) {
                            length_high = length + OVERLAP_MATCH_TOLERANCE;
                        } else {
                            length_high = 0xffff;
                        }
                        if (feed_overlap_lengths[entry] >= length_low && feed_overlap_lengths[entry] <= length_high) {
                            start = job_overlap_patterns_a[pattern][entry];
                            if (start >= OVERLAP_MATCH_TOLERANCE) {
                                start_low = start - OVERLAP_MATCH_TOLERANCE;
                            } else {
                                start_low = 0;
                            }
                            if ((unsigned long)start + OVERLAP_MATCH_TOLERANCE <= 0xffff) {
                                start_high = start + OVERLAP_MATCH_TOLERANCE;
                            } else {
                                start_high = 0xffff;
                            }
                            if (feed_overlap_starts[entry] >= start_low && feed_overlap_starts[entry] <= start_high) {
                                matches++;
                            }
                        }
                    }
                    if (entry == 0 && length == 0) {
                        res = 0;
                        feed_overlap_match_state = OVERLAP_MATCH_NONE;
                        break;
                    } else {
                        feed_overlap_search_index++;
                        if (feed_overlap_count == matches && entry == matches) {
                            res = 1;
                            feed_overlap_match_state = OVERLAP_MATCH_FOUND;
                            break;
                        } else if (feed_overlap_search_index >= OVERLAP_PATTERN_COUNT) {
                            res = 0;
                            feed_overlap_match_state = OVERLAP_MATCH_NONE;
                            break;
                        } else {
                            break;
                        }
                    }
                }
                break;
            }
        }
        if (res == 0) {
            for (entry = 0; entry < 4; entry++) {
                feed_overlap_starts_saved[entry] = feed_overlap_starts[entry];
                feed_overlap_lengths_saved[entry] = feed_overlap_lengths[entry];
            }
            switch (overlap_mode) {
            case 3:
                feed_overlap_hold_sheets = 2;
                break;
            case 5:
                job_overlap_flags.bit.unmatched_mode_5 = 1;
                break;
            case 4:
                job_overlap_flags.bit.unmatched_saved = 1;
                break;
            }
        }
    }
    return res;
}

void feed_count_overlaps(void)
{
    short i;

    for (i = 0; i < 4; i++) {
        if (feed_overlap_starts[i] == 0) {
            break;
        }
    }
    feed_overlap_count = i;
}

void feed_find_max_overlap_length(void)
{
    short i;

    feed_overlap_max_length = feed_overlap_lengths[0];
    for (i = 1; i < 4; i++) {
        if (feed_overlap_lengths[i] == 0) {
            break;
        }
        if (feed_overlap_max_length < feed_overlap_lengths[i]) {
            feed_overlap_max_length = feed_overlap_lengths[i];
        }
    }
}

void feed_start_ultrasonic_check(void)
{
    short i;
    struct ultrasonic_check *check;

    check = &multifeed_check;
    feed_multifeed_flags.b = 0;
    feed_ultrasonic_running = 1;
    for (i = 0; i < 4; i++) {
        feed_overlap_starts[i] = 0;
        feed_overlap_lengths[i] = 0;
        multifeed_check.positions[i] = 0;
        multifeed_check.ends[i] = 0;
    }
    check->overlap_count = 0;
    check->overflow_flags.w = 0;
    multifeed_check.position_count = 0;
    multifeed_check.overlap_pending = 0;
    sensor_set_ultrasonic_sampling(1);
}

int feed_in_check_area(void)
{
    struct ultrasonic_check *check;
    unsigned short position;

    check = &multifeed_check;
    if (sensor_flags.bit.top) {
        if (mailbox.speed_class_b < 4) {
            position = (feed_top_steps + 2) / 3 + feed_ultrasonic_sample_offset;
        } else {
            position = (feed_top_steps + 3) / 6 + feed_ultrasonic_sample_offset;
        }
    }
    if (check->area_mode) {
        if (check->detect_start != check->detect_end) {
            if (!sensor_flags.bit.top) {
                if (check->detect_start < feed_ultrasonic_sample_offset) {
                    return 1;
                }
                return 0;
            } else {
                if (position < check->detect_start) {
                    return 0;
                }
                if (position > check->detect_end) {
                    return 0;
                }
            }
        }
    } else {
        if (check->area_start != check->area_end) {
            if (!sensor_flags.bit.top) {
                if (check->area_end < feed_ultrasonic_sample_offset) {
                    return 1;
                } else {
                    return 0;
                }
            } else {
                if (position < check->area_end && position >= check->area_start) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void feed_check_pick_start(void)
{
    if (sensor_flags.bit.pick && feed_ramp_state) {
        if (feed_pick_on_steps >= job_front_pick_start_steps && mailbox.cmd_code == MDC_CMD_SCAN_MOVE && mailbox.cmd_state == MDC_CMD_STATE_RUNNING && feed_ramp_stopped == 0) {
            feed_pick_start_flags.bit.front_started = 1;
            job_capture_flags.b = 0;
            feed_gap_timeout = 0;
            feed_pick_start_countdown = feed_pick_start_timeout;
            mailbox.motion_flags.bit.front = 1;
            if (mailbox.source & MDC_SOURCE_ADF) {
                if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE) {
                    feed_set_cmd_state(MDC_CMD_STATE_ENDED);
                    feed_gap_timeout = FEED_GAP_TIMEOUT_EVENTS;
                }
            } else {
                job_busy_flags.bit.scan_move = 0;
                if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE) {
                    feed_set_cmd_state(MDC_CMD_STATE_DONE);
                }
            }
            if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE) {
                if (err_flags.b == 0) {
                    mailbox.cmd_result = MDC_RESULT_FRONT_START;
                } else {
                    mailbox.cmd_result = MDC_RESULT_ERROR;
                }
            }
            if (log_frozen == 0) {
                log_hook_begin(0);
                set_imask_exr(7);
                p_log_buf[log_index] = 0xf8;
                log_advance();
                p_log_buf[log_index] = job_state;
                log_advance();
                p_log_buf[log_index] = LOG_END;
                set_imask_exr(feed_saved_imask);
                log_end();
            }
        } else if (feed_pick_on_steps >= job_back_pick_start_steps && !(mailbox.motion_flags.b & MDC_MOTION_BACK) && feed_ramp_stopped == 0) {
            if ((mailbox.source & MDC_SOURCE_ADF) && err_flags.b == 0) {
                job_busy_flags.bit.scan_move = 0;
                if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE) {
                    if (mailbox.cmd_state == MDC_CMD_STATE_RUNNING || !(mailbox.motion_flags.b & MDC_MOTION_FRONT)) {
                        mailbox.sheet_gap_error_count++;
                        err_flags.bit.document = 1;
                        err_code = ERR_SHEET_GAP;
                    }
                    feed_set_cmd_state(MDC_CMD_STATE_DONE);
                    if (err_flags.b == 0) {
                        mailbox.cmd_result = MDC_RESULT_BACK_START;
                    } else {
                        mailbox.cmd_result = MDC_RESULT_ERROR;
                    }
                    feed_gap_timeout = 0;
                }
            }
            if (err_flags.b == 0) {
                feed_pick_start_flags.bit.back_started = 1;
                mailbox.motion_flags.b |= (MDC_MOTION_BACK | MDC_MOTION_BACK_PENDING);
            }
        }
    }
    if (feed_pick_start_countdown) {
        feed_pick_start_countdown--;
        if (feed_pick_start_countdown == 1 && !sensor_flags.bit.top && (feed_pick_start_flags.w & (FEED_PICK_START_FRONT | FEED_PICK_START_BACK))) {
            err_flags.bit.document = 1;
            err_code = ERR_JAM;
            feed_pick_edge_state = 0xff;
            feed_pick_on_steps = 0;
        }
    }
}

void feed_start(unsigned char steps)
{
    unsigned short jam_limit;

    feed_ready_stop = 0;
    feed_speed_tracking = 0;
    if (feed_first_start) {
        feed_first_start = 0;
        feed_ready_stop = 1;
    }
    if ((jam_limit = util_get_be16(&mailbox.jam_limit_high))) {
        feed_jam_limit = jam_limit;
    } else {
        feed_jam_limit = FEED_JAM_LIMIT_DEFAULT;
    }
    if (steps) {
        set_imask_exr(7);
        motor_set_current(MOTOR_FEED, MOTOR_CURRENT_START);
        set_imask_exr(feed_saved_imask);
        if (job_capture_flags.bit.pause_active) {
            if (mailbox.long_page == MDC_LONG_PAGE_ON && mailbox.speed_class_b == 3) {
            } else {
                steps = 0x7a;
            }
            ramp_build_feed_gentle();
        } else {
            ramp_build_feed();
        }
        ramp_load_feed_speed_steps(job_feed_speed_class);
        if (feed_mode.b & FEED_MODE_HALF_STEP) {
            p_chip_a_motor->ramp_steps_1.f_0_8.f = steps;
            p_chip_a_motor->control_1.f_4_2.f = 2;
        } else {
            p_chip_a_motor->ramp_steps_1.f_0_8.f = steps;
            p_chip_a_motor->control_1.f_4_2.f = 1;
        }
        if (steps > 0x32) {
            p_chip_a_motor->reg_02.f_0_4.f = 1;
            if (feed_event_divider.w != 2) {
                feed_top_steps = feed_top_steps >> 1;
            }
            feed_event_divider.w = 2;
            sensor_top_count_limit = 0x7f;
            sensor_top_count_limit = sensor_top_count_limit >> 1;
            if (FEED_TOP_LIMIT_ADD) {
                sensor_top_count_limit += 0x15;
            } else {
                sensor_top_count_limit = 0x15;
            }
            sensor_top_threshold = 9;
            feed_top_off_threshold = 0x4b;
        } else {
            p_chip_a_motor->reg_02.f_0_4.f = 0;
            if (feed_event_divider.w != 5) {
                feed_top_steps += feed_top_steps;
            }
            feed_event_divider.w = 5;
            sensor_top_count_limit = 0x7f;
            if (FEED_TOP_LIMIT_ADD) {
                sensor_top_count_limit += 0x2a;
            } else {
                sensor_top_count_limit = 0x2a;
            }
            sensor_top_threshold = 0x12;
            feed_top_off_threshold = 0x97;
        }
        p_chip_a_motor->control_1.bit.b2 = 1;
        p_chip_a_motor->ramp_index_1.f_0_8.f = 0;
        p_chip_a_motor->ramp_control_1.bit.b0 = 0;
        p_chip_a_motor->control_1.bit.b6 = 1;
        p_chip_a_motor->control_1.bit.b3 = 1;
        p_chip_a_motor->control_1.bit.b7 = 1;
        feed_ramp_state = RAMP_STATE_START;
    } else {
        if (feed_mode.b & FEED_MODE_HALF_STEP) {
            p_chip_a_motor->control_1.f_4_2.f = 2;
        } else {
            p_chip_a_motor->control_1.f_4_2.f = 1;
        }
        p_chip_a_motor->control_1.bit.b2 = 1;
        p_chip_a_motor->ramp_control_1.bit.b0 = 0;
        p_chip_a_motor->control_1.bit.b6 = 1;
        p_chip_a_motor->control_1.bit.b3 = 1;
        p_chip_a_motor->period_1_high.f_0_8.f = 0xff;
        p_chip_a_motor->period_1_low.f_0_8.f = 0xff;
        feed_ramp_state = RAMP_STATE_CONSTANT;
    }
}

void feed_stop(void)
{
    unsigned char index;

    feed_speed_tracking = 0;
    index = p_chip_a_motor->ramp_index_1.b;
    if (feed_fast_ramp == 1) {
        if (index > 2) {
            index -= 2;
            set_imask_exr(7);
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_START);
            set_imask_exr(feed_saved_imask);
            p_chip_a_motor->ramp_steps_1.f_0_8.f = index;
            p_chip_a_motor->ramp_control_1.bit.b0 = 1;
            p_chip_a_motor->control_1.bit.b7 = 1;
            feed_ramp_state = RAMP_STATE_STOPPING;
        } else {
            p_chip_a_motor->control_1.bit.b6 = 0;
            p_chip_a_motor->control_1.bit.b3 = 0;
            feed_ramp_state = RAMP_STATE_STOPPED;
        }
    } else {
        if (index > 0x6e) {
            set_imask_exr(7);
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_START);
            set_imask_exr(feed_saved_imask);
            p_chip_a_motor->ramp_steps_1.f_0_8.f = 0xa4 - index;
            p_chip_a_motor->ramp_control_1.bit.b0 = 0;
            p_chip_a_motor->control_1.bit.b7 = 1;
            feed_ramp_state = RAMP_STATE_STOPPING;
        } else if (index > 2) {
            index -= 2;
            set_imask_exr(7);
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_START);
            set_imask_exr(feed_saved_imask);
            p_chip_a_motor->ramp_steps_1.f_0_8.f = index;
            p_chip_a_motor->ramp_control_1.bit.b0 = 1;
            p_chip_a_motor->control_1.bit.b7 = 1;
            feed_ramp_state = RAMP_STATE_STOPPING;
        } else {
            p_chip_a_motor->control_1.bit.b6 = 0;
            p_chip_a_motor->control_1.bit.b3 = 0;
            feed_ramp_state = RAMP_STATE_STOPPED;
        }
    }
}

void feed_stop_keep_current(void)
{
    unsigned char index;

    feed_speed_tracking = 0;
    index = p_chip_a_motor->ramp_index_1.b;
    if (feed_fast_ramp == 1) {
        if (index > 2) {
            if (pick_continuous != 1 && feed_ready_stop && (job_feed_speed_class == 3 || job_feed_speed_class == 2)) {
                p_chip_a_motor->ramp_steps_1.f_0_8.f = 0xa4 - index;
                p_chip_a_motor->ramp_control_1.bit.b0 = 0;
                p_chip_a_motor->control_1.bit.b7 = 1;
            } else {
                index -= 2;
                p_chip_a_motor->ramp_steps_1.f_0_8.f = index;
                p_chip_a_motor->ramp_control_1.bit.b0 = 1;
                p_chip_a_motor->control_1.bit.b7 = 1;
            }
            feed_ramp_state = RAMP_STATE_STOPPING;
        } else {
            p_chip_a_motor->control_1.bit.b6 = 0;
            p_chip_a_motor->control_1.bit.b3 = 0;
            feed_ramp_state = RAMP_STATE_STOPPED;
        }
    } else {
        if (index > 0x6e) {
            p_chip_a_motor->ramp_steps_1.f_0_8.f = 0xa4 - index;
            p_chip_a_motor->ramp_control_1.bit.b0 = 0;
            p_chip_a_motor->control_1.bit.b7 = 1;
            feed_ramp_state = RAMP_STATE_STOPPING;
        } else if (index > 2) {
            index -= 2;
            p_chip_a_motor->ramp_steps_1.f_0_8.f = index;
            p_chip_a_motor->ramp_control_1.bit.b0 = 1;
            p_chip_a_motor->control_1.bit.b7 = 1;
            feed_ramp_state = RAMP_STATE_STOPPING;
        } else {
            p_chip_a_motor->control_1.bit.b6 = 0;
            p_chip_a_motor->control_1.bit.b3 = 0;
            feed_ramp_state = RAMP_STATE_STOPPED;
        }
    }
}

void feed_change_speed(unsigned char from_steps, unsigned char to_steps)
{
    unsigned char index;

    feed_speed_tracking = 0;
    ramp_load_feed_speed_steps(mailbox.speed_class_b);
    index = p_chip_a_motor->ramp_index_1.b;
    if (index != from_steps) {
        feed_speed_change_misses++;
    } else if (from_steps > to_steps) {
        p_chip_a_motor->ramp_steps_1.f_0_8.f = from_steps - to_steps;
        p_chip_a_motor->ramp_control_1.bit.b0 = 1;
        feed_ramp_state = RAMP_STATE_SPEED_DOWN;
        p_chip_a_motor->control_1.bit.b7 = 1;
    } else if (from_steps < to_steps) {
        p_chip_a_motor->ramp_steps_1.f_0_8.f = to_steps - from_steps;
        p_chip_a_motor->ramp_control_1.bit.b0 = 0;
        feed_ramp_state = RAMP_STATE_SPEED_UP;
        p_chip_a_motor->control_1.bit.b7 = 1;
    }
}

void feed_set_cmd_state(unsigned char state)
{
    if (job_state_history_last != mailbox.cmd_state) {
        job_state_history_last = mailbox.cmd_state;
        if (job_state_history_index > 0x31) {
            job_state_history_index = 0;
        }
        job_state_history[job_state_history_index++] = mailbox.cmd_state;
    }
    mailbox.cmd_state = state;
    if (job_state_history_last != mailbox.cmd_state) {
        job_state_history_last = mailbox.cmd_state;
        if (job_state_history_index > 0x31) {
            job_state_history_index = 0;
        }
        job_state_history[job_state_history_index++] = mailbox.cmd_state;
    }
}
