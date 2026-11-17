#include "mdc.h"

#pragma interrupt (isr_tgi1a)
void isr_tgi1a(void)
{
    unsigned char action;
    unsigned char ramp_state;
    union byte_view inputs;

    p_tpu->TSTR.BIT.CST1 = 0;
    pick_saved_imask = get_imask_exr();
    if (!p_tpu1->TSR.BIT.TGFA)
        return;
    p_tpu1->TSR.BIT.TGFA = 0;
    inputs.b = p_chip_a->inputs.b;
    set_imask_exr(pick_saved_imask);
    if (pick_ramp_state) {
        if (pick_edge_steps < 0xffff) pick_edge_steps++;
        if (pick_sensor_on_steps < 0xffff) pick_sensor_on_steps++;
    }
    if (!inputs.bit.b1) {
        if (pick_sensor_count < SENSOR_DEBOUNCE_MAX) pick_sensor_count++;
    } else {
        if (pick_sensor_count) pick_sensor_count--;
    }
    if (pick_sensor_count > SENSOR_DEBOUNCE_ON) {
        if (!pick_sensor_flags.bit.pick) {
            if (pick_next_sheet) {
                pick_next_sheet = 0;
                if (pick_continuous == 1 && job_pick_speed_class <= 1) {
                } else {
                    if (pick_edge_steps < pick_short_gap_slow_steps && mailbox.pick_ramp_steps_override == 0) {
                        pick_ramp_steps_next = pick_slow_steps;
                        if (pick_edge_steps < pick_short_gap_stop_steps) {
                            pick_short_gap_stopped = 1;
                            pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                        } else {
                            pick_action = MOTOR_ACTION_CHANGE_SPEED;
                        }
                    }
                }
            }
            pick_gap_steps[pick_gap_index] = pick_edge_steps;
            pick_edge_steps = 0;
            if (pick_gap_index < 17) pick_gap_index++;
            if (pick_ramp_state) pick_sensor_on_steps = pick_sensor_count;
            else pick_sensor_on_steps = 0;
            if (pick_df_armed) {
                pick_df_armed = 0;
                pick_df_active = 1;
                pick_df_steps = 0;
                pick_df_hits = 0;
            }
        }
        if (pick_df_active) {
            pick_df_steps++;
            if (pick_df_steps > 25) {
                if (sensor_df_active == 0) {
                    set_imask_exr(7);
                    sensor_df_on();
                    set_imask_exr(pick_saved_imask);
                    pick_df_on_count++;
                }
                if ((unsigned char)(p_ad->DRB >> 8) < 0xcc) pick_df_hits++;
            }
            if (pick_df_steps > 30) {
                pick_df_active = 0;
                if (pick_df_hits < 3) mailbox.separation_sheet_flags.bit.detected = 1;
                sensor_df_off();
            }
        }
        pick_sensor_flags.bit.pick = 1;
        mailbox.sensor_status.bit.pick = 1;
    } else {
        if (pick_sensor_flags.bit.pick) {
            pick_sheet_steps[pick_sheet_index] = pick_edge_steps;
            pick_edge_steps = 0;
            if (pick_sheet_index < 17) pick_sheet_index++;
        }
        pick_sensor_flags.bit.pick = 0;
        mailbox.sensor_status.bit.pick = 0;
    }
    ramp_state = pick_ramp_state;
    if (ramp_state) {
        if (++pick_step_count == 0) pick_step_count--;
    }
    switch (pick_ramp_state) {
    case RAMP_STATE_START:
    case RAMP_STATE_SPEED_UP:
        if (p_chip_b_motor->events.bit.b3) {
            p_chip_b_motor->events.b = 8;
            p_chip_b_motor->ramp_index_1.f_0_8.f -= 1;
            pick_ramp_state = RAMP_STATE_CONSTANT;
        }
        break;
    case RAMP_STATE_SPEED_DOWN:
        if (p_chip_b_motor->events.bit.b3) {
            p_chip_b_motor->events.b = 8;
            p_chip_b_motor->ramp_index_1.f_0_8.f += 1;
            pick_ramp_state = RAMP_STATE_CONSTANT;
        }
        break;
    case RAMP_STATE_STOPPING:
    case 4:
        if (p_chip_b_motor->events.bit.b3) {
            p_chip_b_motor->events.b = 8;
            set_imask_exr(7);
            motor_set_current(MOTOR_PICK, MOTOR_CURRENT_HOLD);
            set_imask_exr(pick_saved_imask);
            pick_ramp_state = RAMP_STATE_STOPPED;
        }
        break;
    }
    action = pick_action;
    if (job_capture_flags.bit.pause_active && (job_busy_flags.b & JOB_BUSY_PREPICK)) {
        job_busy_flags.b &= ~(JOB_BUSY_PICK | JOB_BUSY_PREPICK_ANY);
        job_feed_state = JOB_FEED_STATE_DONE;
        action = MOTOR_ACTION_STOP_KEEP_CURRENT;
        pick_state = PICK_STATE_STOP;
    }
    switch (pick_state) {
    case PICK_STATE_RESET:
        if (ramp_state) {
            set_imask_exr(7);
            motor_set_current(MOTOR_PICK, MOTOR_CURRENT_HOLD);
            set_imask_exr(pick_saved_imask);
            pick_ramp_state = ramp_state = RAMP_STATE_STOPPED;
        }
        p_chip_b_motor->period_1_high.f_0_8.f = 0xff;
        p_chip_b_motor->period_1_low.f_0_8.f = 0xff;
        pick_speed_select = 0;
        action = MOTOR_ACTION_OFF;
        pick_state = PICK_STATE_IDLE;
        break;
    case PICK_STATE_STOP:
        if (ramp_state == RAMP_STATE_STOPPED) {
            pick_speed_select = 0;
            if (main_arm_level_pending && pick_arm_pick_count && bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL && bw_motor_arm_request == 0) {
                main_arm_level_pending = 0;
                if (bw_motor_arm_mode == 0) bw_motor_arm_request = BW_MOTOR_ARM_NEXT_LEVEL;
                pick_arm_pick_count = 0;
            }
            pick_state = PICK_STATE_IDLE;
        }
        break;
    case PICK_STATE_START:
        if (ramp_state == RAMP_STATE_STOPPED) {
            if (sensor_flags.bit.pick) {
                if (sensor_flags.bit.top) {
                    pick_state = PICK_STATE_AT_TOP;
                } else {
                    pick_step_count = 0;
                    pick_flags_1.bit.b4 = 0;
                    pick_flags_2.bit.b4 = 0;
                    mailbox.multifeed_flags.bit.b4 = 0;
                    pick_speed_select = 1;
                    action = MOTOR_ACTION_START;
                    pick_sheet_at_start = 1;
                    pick_state = PICK_STATE_WAIT_PICK_SENSOR;
                }
            } else {
                if (mailbox.manual_feed_timeout && tick_manual_end_active) {
                    if (mailbox.motion_flags.b == 0 && feed_manual_empty && !(mailbox.page_end.b & MDC_PAGE_END_REQUEST)) {
                        feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                        bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                        feed_empty_seconds = feed_manual_timeout_limit + 1;
                        set_imask_exr(7);
                        sensor_flags.bit.no_paper = 1;
                        mailbox.sensor_status.bit.no_paper = 1;
                        set_imask_exr(pick_saved_imask);
                        if (!(job_stop_flags.w & 1)) {
                            err_flags.bit.document = 1;
                            err_code = ERR_NO_PAPER;
                        }
                        pick_state = PICK_STATE_ERROR;
                        job_busy_flags.bit.pick = 0;
                        break;
                    } else {
                        mailbox.manual_feed_end = 0;
                        tick_manual_end_active = 0;
                    }
                }
                pick_step_count = 0;
                pick_flags_1.bit.b4 = 0;
                pick_flags_2.bit.b4 = 0;
                mailbox.multifeed_flags.bit.b4 = 0;
                pick_speed_select = 1;
                action = MOTOR_ACTION_START;
                pick_sheet_at_start = 0;
                pick_state = PICK_STATE_WAIT_PICK_SENSOR;
            }
        } else if (ramp_state == RAMP_STATE_CONSTANT) {
            pick_sheet_at_start = 0;
            pick_state = PICK_STATE_WAIT_PICK_SENSOR;
        }
        break;
    case PICK_STATE_WAIT_PICK_SENSOR:
        if (sensor_flags.bit.pick) {
            pick_step_count = 0;
            pick_state = PICK_STATE_AT_PICK_SENSOR;
        } else if (pick_step_count >= pick_timeout_steps) {
            pick_retries = pick_retry_limit;
            action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            pick_state = PICK_STATE_RETRY;
        }
        break;
    case PICK_STATE_AT_PICK_SENSOR:
        if (sensor_flags.bit.pick) {
            pick_arm_pick_count++;
            p_p2->DR.BYTE = hw_p2_out.b |= HW_P2_ADF_LAMPS;
            if (feed_pick_edge_count == 0) {
                if (bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL && bw_motor_arm_request == 0) {
                    if (bw_motor_arm_mode) {
                        if (bw_motor_arm_lifted == 0) bw_motor_arm_request = BW_MOTOR_ARM_LIFT;
                    } else {
                        bw_motor_arm_request = BW_MOTOR_ARM_LIFT;
                    }
                    pick_arm_pick_count = 0;
                }
            } else {
                if (main_arm_seek_picks < pick_arm_pick_count && bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL && bw_motor_arm_request == 0) {
                    if (bw_motor_arm_mode == 0) bw_motor_arm_request = BW_MOTOR_ARM_SEEK_STEP;
                    pick_arm_pick_count = 0;
                }
                if (bw_motor_arm_mode && bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL && bw_motor_arm_request == 0 && bw_motor_arm_lifted == 0) {
                    bw_motor_arm_request = BW_MOTOR_ARM_LIFT;
                    pick_arm_pick_count = 0;
                }
            }
            pick_state = PICK_STATE_PICKED;
        } else {
            pick_state = PICK_STATE_WAIT_PICK_SENSOR;
        }
        break;
    case PICK_STATE_PICKED:
        if (pick_short_gap_stopped) {
            if (ramp_state == RAMP_STATE_STOPPED) {
                if (sensor_flags.bit.top) {
                    break;
                } else {
                    pick_short_gap_stopped = 0;
                    pick_state = PICK_STATE_START;
                    break;
                }
            } else {
                if (pick_action != MOTOR_ACTION_STOP_KEEP_CURRENT) pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            }
            break;
        }
        if (job_busy_flags.b & JOB_BUSY_PREPICK) job_busy_flags.bit.prepick_picked = 1;
        if (pick_df_active) break;
        if (err_flags.b == 0 && mailbox.cmd_code == MDC_CMD_FEED && mailbox.cmd_state == MDC_CMD_STATE_RUNNING && bw_motor_backing_request == 0 && err_flags.b == 0) {
            pick_ready_edge_steps = pick_edge_steps;
            NOPROTO(feed_set_cmd_state)(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_SHEET_READY;
            if (log_frozen == 0) {
                log_hook_begin(0);
                p_log_buf[log_index] = 0xf7;
                log_advance();
                p_log_buf[log_index] = job_state;
                log_advance();
                p_log_buf[log_index] = LOG_END;
                log_end();
            }
            pick_ready_edge_steps_logged = pick_edge_steps;
        }
        job_busy_flags.bit.pick = 0;
        pick_step_count = 0;
        if (pick_sheet_at_start == 0) pick_delay_steps = 0x32;
        else pick_delay_steps = 0;
        if (pick_continuous == 1) pick_state = PICK_STATE_CONTINUOUS_GAP;
        else pick_state = PICK_STATE_WAIT_TOP;
        break;
    case PICK_STATE_CONTINUOUS_GAP:
        if (pick_edge_steps > pick_continuous_gap_steps) {
            if (!(job_busy_flags.b & JOB_BUSY_SCAN_OR_EJECT) && feed_ramp_state == RAMP_STATE_STOPPED) feed_state = FEED_STATE_RESET;
            if (feed_first_start != 0 || (job_busy_flags.b & JOB_BUSY_SCAN_OR_EJECT) == 0 || mailbox.pick_ramp_steps_override != 0 || (mailbox.feed_debug_flags & 0x80)) {
                action = MOTOR_ACTION_STOP;
                pick_state = PICK_STATE_CONTINUOUS_RESTART;
                if (mailbox.sheet_count_high == 0 && mailbox.sheet_count_low == 0) feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                pick_gap_stop_count++;
            } else {
                action = MOTOR_ACTION_CHANGE_SPEED;
                pick_state = PICK_STATE_WAIT_TOP;
            }
        }
        break;
    case PICK_STATE_CONTINUOUS_RESTART:
        if (ramp_state == RAMP_STATE_STOPPED) {
            if (feed_ramp_state == RAMP_STATE_STOPPED) feed_ready_stop = 0;
            if ((job_busy_flags.b & JOB_BUSY_SCAN_OR_EJECT) && feed_ramp_state == RAMP_STATE_CONSTANT) {
                pick_speed_select = 2;
                action = MOTOR_ACTION_START;
                pick_state = PICK_STATE_WAIT_TOP;
            }
        }
        break;
    case PICK_STATE_WAIT_TOP:
        if (pick_delay_steps == 0) {
            if (sensor_flags.bit.top) {
                if (mailbox.manual_feed_timeout) {
                    tick_manual_end_active = 0;
                    sensor_flags.bit.no_paper = 0;
                    feed_empty_seconds = 0;
                }
                bw_motor_arm_idle_ticks = 0;
                if (pick_continuous == 1) {
                    if (feed_first_start) feed_first_start = 0;
                    pick_speed_select = 2;
                    action = MOTOR_ACTION_STOP;
                    pick_state = PICK_STATE_STOP;
                    if (pick_fast_next == 1) pick_ramp_steps_next = pick_ramp_steps_next_by_class[job_feed_speed_class];
                    else pick_ramp_steps_next = pick_ramp_steps_by_class[job_feed_speed_class];
                } else {
                    if (feed_first_start) {
                        feed_first_start = 0;
                    } else if (!(job_busy_flags.b & JOB_BUSY_SCAN_OR_EJECT)) {
                        action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                        pick_state = PICK_STATE_STOP_AT_TOP;
                        if ((mailbox.motion_flags.b & MDC_MOTION_SIDES) == 0) {
                            feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                            feed_ready_stop = 1;
                        }
                        pick_top_stop_count++;
                    } else {
                        if ((job_busy_flags.b & JOB_BUSY_EJECT) && (job_busy_flags.b & JOB_BUSY_PREPICK_PICKED)) {
                            action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                            pick_state = PICK_STATE_STOP_AT_TOP;
                            feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                            feed_ready_stop = 1;
                        } else {
                            pick_speed_select = 1;
                            action = MOTOR_ACTION_STOP;
                            pick_state = PICK_STATE_STOP;
                        }
                    }
                }
                if (pick_edge_steps > pick_arm_adjust_limit) main_arm_level_pending = 1;
            } else if (pick_step_count >= pick_top_timeout_steps) {
                pick_retries = pick_top_retry_limit;
                action = MOTOR_ACTION_STOP;
                pick_state = PICK_STATE_TOP_RETRY;
            }
        } else {
            pick_delay_steps--;
        }
        break;
    case PICK_STATE_RETRY:
        if (ramp_state == RAMP_STATE_STOPPED) {
            bw_motor_arm_idle_ticks = 0;
            if (sensor_flags.bit.no_paper && !sensor_flags.bit.pick) {
                if (mailbox.manual_feed_timeout && feed_empty_seconds > feed_manual_timeout_limit) {
                    set_imask_exr(7);
                    tick_manual_end_ticks = 0;
                    tick_manual_end_active = 1;
                    set_imask_exr(pick_saved_imask);
                }
                feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                feed_pick_edge_count = 0;
                err_flags.bit.document = 1;
                err_code = ERR_NO_PAPER;
                pick_state = PICK_STATE_ERROR;
                job_busy_flags.bit.pick = 0;
            } else {
                pick_wait_ticks = pick_retry_ticks;
                if (--pick_retries < 0) {
                    feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                    bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                    feed_pick_edge_count = 0;
                    err_flags.bit.document = 1;
                    err_code = ERR_PICK;
                    pick_state = PICK_STATE_ERROR;
                    job_busy_flags.bit.pick = 0;
                } else {
                    if (mailbox.manual_feed_timeout && feed_manual_empty) {
                        if (tick_manual_end_active) {
                            if (mailbox.motion_flags.b == 0 && !(mailbox.page_end.b & MDC_PAGE_END_REQUEST)) {
                                feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                                bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                                feed_empty_seconds = feed_manual_timeout_limit + 1;
                                feed_manual_empty = 0;
                                set_imask_exr(7);
                                sensor_flags.bit.no_paper = 1;
                                mailbox.sensor_status.bit.no_paper = 1;
                                set_imask_exr(pick_saved_imask);
                                if (!(job_stop_flags.w & 1)) {
                                    err_flags.bit.document = 1;
                                    err_code = ERR_NO_PAPER;
                                }
                                pick_state = PICK_STATE_ERROR;
                                job_busy_flags.bit.pick = 0;
                                break;
                            } else {
                                mailbox.manual_feed_end = 0;
                                tick_manual_end_active = 0;
                            }
                        } else {
                            if (feed_empty_seconds > pick_empty_stop_seconds && feed_ramp_state == RAMP_STATE_CONSTANT && (job_busy_flags.b & JOB_BUSY_SCAN_OR_EJECT) == 0) {
                                feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                                feed_ready_stop = 1;
                            }
                        }
                        pick_retries++;
                        pick_arm_pick_count = 0;
                        pick_state = PICK_STATE_RETRY_WAIT;
                        break;
                    } else {
                        if (bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL && bw_motor_arm_request == 0) {
                            if (feed_pick_edge_count == 0) {
                                bw_motor_arm_request = BW_MOTOR_ARM_LEAVE_RETRY;
                            } else if (pick_retry_limit >= 6 && pick_retries == pick_retry_limit + -5) {
                                bw_motor_arm_request = BW_MOTOR_ARM_REHOME_NEXT_LEVEL;
                            } else {
                                bw_motor_arm_request = BW_MOTOR_ARM_NEXT_LEVEL;
                            }
                            pick_arm_pick_count = 0;
                        }
                        pick_state = PICK_STATE_RETRY_WAIT;
                    }
                }
            }
        }
        break;
    case PICK_STATE_RETRY_WAIT:
        if (pick_wait_ticks == 0 && bw_motor_arm_request == 0) {
            if (feed_ramp_state != RAMP_STATE_CONSTANT && pick_continuous != 1) {
                feed_action = MOTOR_ACTION_START;
                pick_wait_ticks = pick_retry_ticks;
            } else {
                pick_step_count = 0;
                pick_speed_select = 0;
                pick_ramp_steps = pick_ramp_steps_slow;
                action = MOTOR_ACTION_START;
                pick_state = PICK_STATE_RETRY_PICK;
            }
        }
        break;
    case PICK_STATE_RETRY_PICK:
        if (sensor_flags.bit.pick) {
            pick_retries = 0;
            pick_step_count = 0;
            pick_state = PICK_STATE_AT_PICK_SENSOR;
        } else if (pick_step_count >= pick_timeout_steps) {
            action = MOTOR_ACTION_STOP;
            pick_state = PICK_STATE_RETRY;
        }
        break;
    case PICK_STATE_STOP_AT_TOP:
        if (ramp_state == RAMP_STATE_STOPPED && feed_ramp_state == RAMP_STATE_STOPPED) {
            if (main_arm_level_pending && pick_arm_pick_count && bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL && bw_motor_arm_request == 0) {
                main_arm_level_pending = 0;
                if (bw_motor_arm_mode == 0) bw_motor_arm_request = BW_MOTOR_ARM_NEXT_LEVEL;
                pick_arm_pick_count = 0;
            }
            pick_state = PICK_STATE_AT_TOP;
        }
        break;
    case PICK_STATE_AT_TOP:
        if ((job_busy_flags.b & JOB_BUSY_SCAN_MOVE) || ((job_busy_flags.b & JOB_BUSY_EJECT) && !(job_busy_flags.b & JOB_BUSY_PREPICK_PICKED))) {
            if (feed_ramp_state == RAMP_STATE_CONSTANT) {
            }
        }
        break;
    case PICK_STATE_TOP_RETRY:
        if (ramp_state == RAMP_STATE_STOPPED) {
            bw_motor_arm_idle_ticks = 0;
            if (sensor_flags.bit.no_paper && !sensor_flags.bit.pick) {
                feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                feed_pick_edge_count = 0;
                err_flags.bit.document = 1;
                err_code = ERR_NO_PAPER;
                pick_state = PICK_STATE_ERROR;
                job_busy_flags.bit.pick = 0;
            } else {
                pick_wait_ticks = pick_top_retry_ticks;
                if (--pick_retries < 0) {
                    feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                    bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                    feed_pick_edge_count = 0;
                    err_flags.bit.document = 1;
                    err_code = ERR_JAM;
                    pick_state = PICK_STATE_ERROR;
                    job_busy_flags.bit.pick = 0;
                } else {
                    if (bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL && bw_motor_arm_request == 0) {
                        if (pick_retries == 1) bw_motor_arm_request = BW_MOTOR_ARM_REHOME_NEXT_LEVEL;
                        else bw_motor_arm_request = BW_MOTOR_ARM_NEXT_LEVEL;
                        pick_arm_pick_count = 0;
                    }
                    pick_state = PICK_STATE_TOP_RETRY_WAIT;
                }
            }
        }
        break;
    case PICK_STATE_TOP_RETRY_WAIT:
        if (pick_wait_ticks == 0 && bw_motor_arm_request == 0) {
            if (feed_ramp_state != RAMP_STATE_CONSTANT) {
                feed_action = MOTOR_ACTION_START;
                pick_wait_ticks = pick_retry_ticks;
            } else {
                pick_step_count = 0;
                pick_speed_select = 0;
                pick_ramp_steps = pick_ramp_steps_slow;
                action = MOTOR_ACTION_START;
                pick_state = PICK_STATE_TOP_RETRY_FEED;
            }
        }
        break;
    case PICK_STATE_TOP_RETRY_FEED:
        if (sensor_flags.bit.top) {
            pick_delay_steps = 0;
            pick_state = PICK_STATE_WAIT_TOP;
        } else if (pick_step_count >= pick_top_timeout_steps) {
            action = MOTOR_ACTION_STOP;
            pick_state = PICK_STATE_TOP_RETRY;
        }
        break;
    }
    if (action) {
        switch (action) {
        case MOTOR_ACTION_OFF:
            set_imask_exr(7);
            motor_set_current(MOTOR_PICK, MOTOR_CURRENT_HOLD);
            set_imask_exr(pick_saved_imask);
            pick_current_mode = 0;
            if (ramp_state) pick_ramp_state = ramp_state = RAMP_STATE_STOPPED;
            action = MOTOR_ACTION_NONE;
            break;
        case MOTOR_ACTION_START:
            if (ramp_state == RAMP_STATE_STOPPED) {
                set_imask_exr(7);
                motor_set_current(MOTOR_PICK, MOTOR_CURRENT_START);
                set_imask_exr(pick_saved_imask);
                pick_current_mode = CURRENT_MODE_START;
                pick_start_ticks = 0x19;
                action = MOTOR_ACTION_START_RAMP;
            } else if (ramp_state == RAMP_STATE_CONSTANT) {
                action = MOTOR_ACTION_NONE;
            }
            break;
        case MOTOR_ACTION_START_RAMP:
            pick_start(pick_speed_select == 2 ? pick_ramp_steps_next : pick_ramp_steps);
            action = MOTOR_ACTION_NONE;
            break;
        case MOTOR_ACTION_STOP:
            if (ramp_state == RAMP_STATE_CONSTANT) {
                set_imask_exr(7);
                motor_set_current(MOTOR_PICK, MOTOR_CURRENT_START);
                set_imask_exr(pick_saved_imask);
                pick_current_mode = CURRENT_MODE_STOP;
                action = MOTOR_ACTION_STOP_RAMP;
            } else if (ramp_state == RAMP_STATE_STOPPED) {
                action = MOTOR_ACTION_NONE;
            }
            break;
        case MOTOR_ACTION_STOP_RAMP:
            pick_stop();
            action = MOTOR_ACTION_WAIT_STOPPED;
            break;
        case MOTOR_ACTION_STOP_KEEP_CURRENT:
            if (ramp_state == RAMP_STATE_CONSTANT) {
                set_imask_exr(7);
                motor_set_current(MOTOR_PICK, MOTOR_CURRENT_START);
                set_imask_exr(pick_saved_imask);
                pick_current_mode = CURRENT_MODE_STOP;
                action = MOTOR_ACTION_STOP_KEEP_CURRENT_RAMP;
            } else if (ramp_state == RAMP_STATE_STOPPED) {
                action = MOTOR_ACTION_NONE;
            }
            break;
        case MOTOR_ACTION_STOP_KEEP_CURRENT_RAMP:
            pick_stop_keep_current();
            action = MOTOR_ACTION_WAIT_STOPPED;
            break;
        case MOTOR_ACTION_WAIT_STOPPED:
            if (ramp_state == RAMP_STATE_STOPPED) action = MOTOR_ACTION_OFF;
            break;
        case MOTOR_ACTION_CHANGE_SPEED:
            if (ramp_state == RAMP_STATE_CONSTANT) {
                pick_change_speed(pick_ramp_steps, pick_ramp_steps_next);
                action = MOTOR_ACTION_NONE;
            }
            break;
        }
        pick_action = action;
    }
    if (pick_current_mode == CURRENT_MODE_START && ramp_state == RAMP_STATE_CONSTANT && pick_start_ticks == 0) {
        set_imask_exr(7);
        motor_set_current(MOTOR_PICK, pick_run_current);
        set_imask_exr(pick_saved_imask);
        pick_current_mode = CURRENT_MODE_RUN;
    }
    if (pick_current_mode == CURRENT_MODE_START && pick_boost_steps == 1) {
        set_imask_exr(7);
        motor_set_current(MOTOR_PICK, MOTOR_CURRENT_START);
        set_imask_exr(pick_saved_imask);
    }
    if (pick_boost_steps) pick_boost_steps--;
}

void pick_start(unsigned char steps)
{
    feed_jam_steps = 0;
    if (steps) {
        set_imask_exr(7);
        pick_hold_ticks = 0;
        if (pick_hold_current) motor_set_current(MOTOR_PICK, MOTOR_CURRENT_BOOST);
        else motor_set_current(MOTOR_PICK, MOTOR_CURRENT_START);
        set_imask_exr(pick_saved_imask);
        if (steps < 0x1d) {
            pick_short_gap_slow_steps = 0x40;
            pick_word_3 = 0x3c;
            pick_short_gap_stop_steps = 0x26;
        } else if (steps < 0x4d) {
            pick_short_gap_slow_steps = 0x3d;
            pick_word_3 = 0x3b;
            pick_short_gap_stop_steps = 0x26;
        } else {
            pick_short_gap_slow_steps = 0x37;
            pick_word_3 = 0x37;
            pick_short_gap_stop_steps = 0x26;
        }
        p_chip_b_motor->control_1.f_4_2.f = 1;
        p_chip_b_motor->ramp_steps_1.f_0_8.f = steps;
        p_chip_b_motor->control_1.bit.b2 = 1;
        p_chip_b_motor->ramp_index_1.f_0_8.f = 0;
        p_chip_b_motor->ramp_control_1.bit.b0 = 0;
        p_chip_b_motor->control_1.bit.b6 = 1;
        p_chip_b_motor->control_1.bit.b3 = 1;
        p_chip_b_motor->control_1.bit.b7 = 1;
        if (steps < 0x1d) pick_boost_steps = 5;
        else if (steps < 0x4d) pick_boost_steps = 4;
        else pick_boost_steps = 2;
        pick_ramp_state = RAMP_STATE_START;
    } else {
        p_chip_b_motor->control_1.f_4_2.f = 1;
        p_chip_b_motor->control_1.bit.b2 = 1;
        p_chip_b_motor->ramp_control_1.bit.b0 = 0;
        p_chip_b_motor->control_1.bit.b6 = 1;
        p_chip_b_motor->control_1.bit.b3 = 1;
        p_chip_b_motor->period_1_high.f_0_8.f = 0xff;
        p_chip_b_motor->period_1_low.f_0_8.f = 0xff;
        pick_ramp_state = RAMP_STATE_CONSTANT;
    }
}

void pick_stop(void)
{
    unsigned char index;

    index = p_chip_b_motor->ramp_index_1.f_0_8.f;
    if (index > 2) {
        set_imask_exr(7);
        motor_set_current(MOTOR_PICK, MOTOR_CURRENT_START);
        set_imask_exr(pick_saved_imask);
        p_chip_b_motor->ramp_index_1.f_0_8.f = pick_brake_entry[index];
        p_chip_b_motor->ramp_steps_1.f_0_8.f = 0x80 - pick_brake_entry[index];
        p_chip_b_motor->ramp_control_1.bit.b0 = 0;
        p_chip_b_motor->control_1.bit.b7 = 1;
        pick_ramp_state = RAMP_STATE_STOPPING;
    } else {
        p_chip_b_motor->control_1.bit.b6 = 0;
        p_chip_b_motor->control_1.bit.b3 = 0;
        pick_ramp_state = RAMP_STATE_STOPPED;
    }
    pick_next_sheet = 0;
}

void pick_stop_keep_current(void)
{
    unsigned char index;

    index = p_chip_b_motor->ramp_index_1.f_0_8.f;
    if (index > 2) {
        p_chip_b_motor->ramp_index_1.f_0_8.f = pick_brake_entry[index];
        p_chip_b_motor->ramp_steps_1.f_0_8.f = 0x80 - pick_brake_entry[index];
        p_chip_b_motor->ramp_control_1.bit.b0 = 0;
        p_chip_b_motor->control_1.bit.b7 = 1;
        pick_ramp_state = RAMP_STATE_STOPPING;
    } else {
        p_chip_b_motor->control_1.bit.b6 = 0;
        p_chip_b_motor->control_1.bit.b3 = 0;
        pick_ramp_state = RAMP_STATE_STOPPED;
    }
    pick_next_sheet = 0;
}

void pick_change_speed(unsigned char from_steps, unsigned char to_steps)
{
    if (from_steps > to_steps) {
        from_steps = from_steps + 1;
        p_chip_b_motor->ramp_steps_1.f_0_8.f = from_steps - to_steps;
        p_chip_b_motor->ramp_control_1.bit.b0 = 1;
        pick_ramp_state = RAMP_STATE_SPEED_DOWN;
        p_chip_b_motor->control_1.bit.b7 = 1;
    } else if (from_steps < to_steps) {
        p_chip_b_motor->ramp_steps_1.f_0_8.f = to_steps - from_steps;
        p_chip_b_motor->ramp_control_1.bit.b0 = 0;
        pick_ramp_state = RAMP_STATE_SPEED_UP;
        p_chip_b_motor->control_1.bit.b7 = 1;
    }
}
