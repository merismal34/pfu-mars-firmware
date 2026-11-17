#include "mdc.h"

void job_run(void)
{
    unsigned char pad[10];
    unsigned short last_page_flags;
    short bytes;
    unsigned short *edge_word;

    set_imask_exr(7);
    if (job_state_history_last != mailbox.cmd_state) {
        job_state_history_last = mailbox.cmd_state;
        if (job_state_history_index > 0x31)
            job_state_history_index = 0;
        job_state_history[job_state_history_index++] = mailbox.cmd_state;
    }
    if (job_cmd_history_last != mailbox.cmd_code) {
        job_cmd_history_last = mailbox.cmd_code;
        if (job_cmd_history_index > 0x31)
            job_cmd_history_index = 0;
        job_cmd_history[job_cmd_history_index++] = mailbox.cmd_code;
    }
    set_imask_exr(0);
    job_restore_backing();
    switch (job_state) {
    case JOB_STATE_RESET:
        bytes = 8;
        edge_word = feed_edge_records[0].edge;
        while (bytes != 0) {
            *edge_word++ = 0;
            bytes -= 2;
        }
        feed_sheets_in_path = 0;
        mailbox.motion_flags.b = 0;
        job_feed_state = JOB_FEED_STATE_IDLE;
        job_scan_state = JOB_SCAN_STATE_IDLE;
        job_eject_state = JOB_EJECT_STATE_IDLE;
        job_busy_flags.b = 0;
        mailbox.sheet_count_high = 0;
        mailbox.sheet_count_low = 0;
        job_state = JOB_STATE_IDLE;
        break;
    case JOB_STATE_SHEET_END:
        if ((mailbox.page_end.b & MDC_PAGE_END_REQUEST) && !(mailbox.page_end.b & MDC_PAGE_END_ACCEPTED) && !sensor_flags.bit.no_paper) {
            mailbox.page_end.bit.accepted = 1;
            if (mailbox.manual_feed_timeout) {
                mailbox.manual_feed_end = 0;
                tick_manual_end_active = 0;
            }
        }
        if (job_page_limit == job_page_count || (mailbox.page_end.b & MDC_PAGE_END_REQUEST)) {
            mailbox.job_trace_code = 0xf1;
            if (feed_state == FEED_STATE_IDLE) {
                if ((mailbox.page_end.b & (MDC_PAGE_END_REQUEST | MDC_PAGE_END_ACCEPTED)) == (MDC_PAGE_END_REQUEST | MDC_PAGE_END_ACCEPTED) && sensor_flags.bit.no_paper) {
                    err_flags.bit.document = 1;
                    err_code = ERR_NO_PAPER;
                }
                util_log_add_error_word();
                job_state = JOB_STATE_RESET;
                if (job_page_limit) {
                    if (job_wake_flags.bit.wake_event) {
                        tick_flatbed_lamp_delay = 0;
                        job_wake_flags.bit.flatbed_lamp_pending = 1;
                    }
                } else {
                    if (job_stop_flags.bit.requested) {
                        if (job_wake_flags.bit.wake_event) {
                            tick_flatbed_lamp_delay = 0;
                            job_wake_flags.bit.flatbed_lamp_pending = 1;
                        }
                    }
                }
                mailbox.job_trace_code = 0xfe;
                break;
            } else {
                if (job_pick_gap_extend == 0 && !job_capture_flags.bit.pause_active)
                    break;
            }
        }
        set_imask_exr(7);
        if (sensor_flags.bit.top) {
            if (sensor_flags.bit.pick) {
                if (feed_ramp_state) {
                    set_imask_exr(0);
                    mailbox.job_trace_code = 0x10;
                    break;
                }
            }
        } else {
            if (sensor_flags.bit.pick && feed_top_edge_steps < 0xf0 && feed_ramp_state) {
                set_imask_exr(0);
                mailbox.job_trace_code = 0x11;
                break;
            }
        }
        if (feed_gap_steps < 0x1b1 && feed_ramp_state) {
            set_imask_exr(0);
            mailbox.job_trace_code = 0x12;
            break;
        }
        if (!sensor_flags.bit.pick) {
            if (job_pick_gap_steps < job_pick_gap_min && feed_ramp_state) {
                set_imask_exr(0);
                mailbox.job_trace_code = 0x20;
                break;
            }
            if (job_pick_gap_extend != 0) {
                if (job_pick_gap_steps < job_pick_gap_extended) {
                    if (feed_ramp_state) {
                        set_imask_exr(0);
                        mailbox.job_trace_code = 0x21;
                        break;
                    }
                } else {
                    job_pick_gap_extend = 0;
                }
            }
            if ((job_overlap_check_flags.w & JOB_OVERLAP_CHECK_ACTIVE) && feed_ramp_state) {
                set_imask_exr(0);
                mailbox.job_trace_code = 0x30;
                break;
            }
            if ((job_last_page_flags & 1) && feed_ramp_state) {
                set_imask_exr(0);
                mailbox.job_trace_code = 0x40;
                job_state = JOB_STATE_IDLE;
                break;
            }
            if ((mailbox.feed_options.b & MDC_FEED_JOB_SEPARATION) && (mailbox.separation_sheet_flags.b & MDC_SEPARATION_SHEET_DETECTED) && feed_ultrasonic_running && feed_ramp_state) {
                set_imask_exr(0);
                mailbox.job_trace_code = 0x50;
                break;
            }
        }
        if (job_capture_flags.bit.pause_active) {
            if (mailbox.motion_flags.b & MDC_MOTION_SIDES) {
                if (mailbox.cmd_state == MDC_CMD_STATE_POSTED) {
                    lamp_off_count = lamp_off_limit;
                    if (mailbox.long_page == MDC_LONG_PAGE_ON && (job_top_clear_steps < 0x37b || (job_top_clear_steps >= 0x37b && (sensor_flags.bit.top || sensor_flags.bit.pick)))) {
                    } else {
                        mailbox.motion_flags.bit.front = 0;
                        mailbox.motion_flags.bit.back = 0;
                        mailbox.motion_flags.bit.back_pending = 0;
                    }
                }
                set_imask_exr(0);
                mailbox.job_trace_code = 0x60;
                break;
            } else if (job_top_clear_steps < 0x37b) {
                if (mailbox.cmd_state == MDC_CMD_STATE_POSTED) {
                    lamp_off_count = lamp_off_limit;
                    if (mailbox.long_page == MDC_LONG_PAGE_ON) {
                    } else {
                        mailbox.motion_flags.bit.front = 0;
                        mailbox.motion_flags.bit.back = 0;
                        mailbox.motion_flags.bit.back_pending = 0;
                    }
                }
                set_imask_exr(0);
                mailbox.job_trace_code = 0x61;
                break;
            }
        }
        set_imask_exr(0);
        if (job_page_limit == job_page_count || (mailbox.page_end.b & MDC_PAGE_END_REQUEST)) {
            mailbox.job_trace_code = 0xf0;
            if (feed_state == FEED_STATE_IDLE) {
                if ((mailbox.page_end.b & (MDC_PAGE_END_REQUEST | MDC_PAGE_END_ACCEPTED)) == (MDC_PAGE_END_REQUEST | MDC_PAGE_END_ACCEPTED) && sensor_flags.bit.no_paper) {
                    err_flags.bit.document = 1;
                    err_code = ERR_NO_PAPER;
                }
                util_log_add_error_word();
                job_state = JOB_STATE_RESET;
                mailbox.job_trace_code = 0xff;
            }
            break;
        } else {
            if (feed_state == FEED_STATE_IDLE) {
                if (job_feed_state == JOB_FEED_STATE_DONE && job_wake_flags.bit.wake_event) {
                    tick_flatbed_lamp_delay = 500;
                    job_wake_flags.bit.flatbed_lamp_pending = 1;
                }
                job_state = JOB_STATE_IDLE;
            } else if (job_feed_state == JOB_FEED_STATE_PREPICK && !job_capture_flags.bit.pause_active && !sensor_flags.bit.pick && feed_top_edge_steps > 0x1b1) {
                lamp_adf_off_count = lamp_adf_off_limit;
                lamp_adf_off_count = 0x78;
                util_put_be16(&mailbox.job_page_limit_high, job_page_limit);
                util_put_be16(&mailbox.job_page_count_high, job_page_count);
                mailbox.page_count_limit_copy_high = mailbox.page_count_limit_high;
                mailbox.page_count_limit_copy_low = mailbox.page_count_limit_low;
                job_busy_flags.bit.prepick = 1;
                pick_next_sheet = 1;
                job_feed_state = JOB_FEED_STATE_PREPARE;
                job_state = JOB_STATE_IDLE;
            }
        }
    case JOB_STATE_IDLE:
        if (job_state == JOB_STATE_IDLE && (mailbox.page_end.b & MDC_PAGE_END_REQUEST) && err_code == 0 && (mailbox.page_end.b & MDC_PAGE_END_ACCEPTED)) {
            if (sensor_flags.bit.no_paper) {
                err_flags.bit.document = 1;
                err_code = ERR_NO_PAPER;
            } else {
                mailbox.page_end.bit.done = 1;
            }
        }
        if (mailbox.cmd_state != MDC_CMD_STATE_POSTED) {
            if (job_stop_flags.bit.requested && feed_ramp_state == RAMP_STATE_STOPPED && !job_capture_flags.bit.pause_active && job_feed_state == JOB_FEED_STATE_SHEET_READY && (mailbox.feed_options.b & MDC_FEED_PREPICK) &&
                job_wake_flags.bit.wake_event && !p_p1->DR.BIT.B6) {
                tick_flatbed_lamp_delay = 0;
                job_wake_flags.bit.flatbed_lamp_pending = 1;
            }
            break;
        }
        lamp_off_count = lamp_off_limit;
        if (mailbox.cmd_code == MDC_CMD_FEED && !(mailbox.feed_debug_flags & 8)) {
            if (mailbox.feed_debug_flags & 0x20) {
                if (mailbox.feed_debug_flags & 0x10) {
                    set_imask_exr(7);
                    while (1)
                        ;
                } else {
                    job_state = JOB_STATE_FEEDING;
                    break;
                }
            }
            if (sensor_flags.bit.adf_cover_open) {
                if (err_code == 0 && tick_adf_cover_state == 2)
                    tick_adf_cover_state = 0;
                break;
            }
            {
                job_front_reached = 0;
                if (job_stop_flags.bit.requested) {
                    job_page_count = 0;
                    if (job_feed_state == JOB_FEED_STATE_SHEET_READY)
                        job_page_count++;
                    if (mailbox.page_count_limit_high || mailbox.page_count_limit_low)
                        job_page_limit = mailbox.page_count_limit_low | (mailbox.page_count_limit_high << 8);
                    else
                        job_page_limit = 0xffff;
                    mailbox.job_trace_code = 1;
                }
                job_stop_flags.w = 0;
                if (mailbox.lamp_off_minutes)
                    lamp_adf_off_limit = mailbox.lamp_off_minutes;
                else
                    lamp_adf_off_limit = 0x3c;
                job_wake_flags.bit.flatbed_lamp_pending = 0;
                p_p1->DR.BIT.B6 = 0;
                lamp_adf_off_count = lamp_adf_off_limit;
                lamp_adf_off_count = 0x78;
                p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_ADF_LAMPS);
                if (job_feed_state == JOB_FEED_STATE_SHEET_READY) {
                    job_busy_flags.b &= ~JOB_BUSY_PREPICK_ANY;
                    if (sensor_flags.bit.pick) {
                        CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
                        p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_ADF_LAMPS);
                        if (err_flags.b == 0 && err_flags.b == 0) {
                            CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_DONE);
                            mailbox.cmd_result = MDC_RESULT_SHEET_READY;
                        }
                        if (log_frozen == 0) {
                            log_hook_begin(0xfa);
                            p_log_buf[log_index] = 0xf7;
                            log_advance();
                            p_log_buf[log_index] = job_state;
                            log_advance();
                            p_log_buf[log_index] = LOG_END;
                            log_end();
                        }
                        job_state = JOB_STATE_SHEET_READY;
                    } else {
                        CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
                        if (mailbox.sheet_count_high == 0 && mailbox.sheet_count_low == 0) {
                            mailbox.multifeed_flags_imprinter = 0;
                            feed_pick_edge_count = 0;
                        }
                        pick_next_sheet = 1;
                        job_feed_state = JOB_FEED_STATE_PREPARE;
                        job_state = JOB_STATE_FEEDING;
                    }
                } else if (job_busy_flags.b & JOB_BUSY_PREPICK) {
                    set_imask_exr(7);
                    job_busy_flags.b &= ~JOB_BUSY_PREPICK_ANY;
                    set_imask_exr(0);
                    CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
                    job_state = JOB_STATE_FEEDING;
                } else if (job_feed_state == JOB_FEED_STATE_IDLE) {
                    CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
                    job_busy_flags.b &= ~JOB_BUSY_PREPICK_ANY;
                    if (mailbox.sheet_count_high == 0 && mailbox.sheet_count_low == 0) {
                        mailbox.multifeed_flags_imprinter = 0;
                        feed_pick_edge_count = 0;
                        job_sheet_slot = 0;
                        feed_sheet_active[0] = 0;
                        feed_sheet_active[1] = 0;
                        job_feed_slot = 0;
                        job_feed_slot_flags[0] = 0;
                        job_feed_slot_flags[1] = 0;
                        job_page_count = 0;
                        if (mailbox.page_count_limit_high || mailbox.page_count_limit_low)
                            job_page_limit = mailbox.page_count_limit_low | (mailbox.page_count_limit_high << 8);
                        else
                            job_page_limit = 0xffff;
                        mailbox.job_trace_code = 0;
                        job_capture_flags.b = 0;
                    }
                    main_clear_path_pending = 0;
                    pick_next_sheet = 1;
                    job_feed_state = JOB_FEED_STATE_PREPARE;
                    job_state = JOB_STATE_FEEDING;
                } else if (job_feed_state == JOB_FEED_STATE_PREPICK || job_feed_state == JOB_FEED_STATE_DONE) {
                    if (!job_capture_flags.bit.pause_active) {
                        lamp_adf_off_count = lamp_adf_off_limit;
                        lamp_adf_off_count = 0x78;
                        CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
                        job_busy_flags.b &= ~JOB_BUSY_PREPICK_ANY;
                        pick_next_sheet = 1;
                        job_feed_state = JOB_FEED_STATE_PREPARE;
                        job_state = JOB_STATE_FEEDING;
                    } else {
                        if (!(mailbox.motion_flags.b & MDC_MOTION_SIDES)) {
                            if (job_top_clear_steps < 0x37b) {
                            } else {
                                job_capture_flags.bit.pause_active = 0;
                            }
                        }
                    }
                }
                if (job_feed_slot)
                    job_feed_slot = 0;
                else
                    job_feed_slot = 1;
                job_feed_slot_flags[job_feed_slot] &= 0xfffc;
            }
        } else if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE && (mailbox.source & MDC_SOURCE_FLATBED)) {
            set_imask_exr(7);
            p_p2->DR.BYTE = (hw_p2_out.b &= ~HW_P2_ADF_LAMPS);
            p_chip_a_motor->control_1.bit.b3 = 0;
            tick_warmup_hold = 0;
            set_imask_exr(0);
            CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
            job_busy_flags.bit.scan_move = 1;
            job_fb_motor_state = JOB_FB_MOTOR_STATE_START;
            job_state = JOB_STATE_FLATBED;
            job_wake_flags.bit.wake_event = 0;
        } else if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE) {
            set_imask_exr(7);
            p_p2->DR.BIT.B5 = 0;
            p_p2->DR.BIT.B6 = 0;
            lamp_heater_front_warmup = 0;
            lamp_heater_back_warmup = 0;
            hw_p2_out.b &= ~HW_P2_ADF_HEATERS;
            tick_warmup_hold = 0;
            if (feed_ready_stop == 0) {
                job_busy_flags.bit.scan_move = 1;
                set_imask_exr(0);
                CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
                mailbox.multifeed_flags.b = 0;
                last_page_flags = mailbox.last_page;
                if (mailbox.pick_ramp_steps_override)
                    last_page_flags |= 0x80;
                last_page_flags |= mailbox.settings_90;
                job_last_page_flags = last_page_flags;
                if (last_page_flags & 0x80)
                    job_last_page = 1;
                else
                    job_last_page = 0;
                job_stop_flags.w = 0;
                job_scan_state = JOB_SCAN_STATE_START;
                job_busy_flags.b &= ~JOB_BUSY_PREPICK_ANY;
                job_state = JOB_STATE_SCAN_START;
            }
            set_imask_exr(0);
        } else if (mailbox.cmd_code == MDC_CMD_FB_MOTOR_HOME) {
            if (main_fb_home_cmd != 2)
                main_fb_home_cmd = 1;
            CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
            job_busy_flags.bit.scan_move = 1;
            job_fb_motor_state = JOB_FB_MOTOR_STATE_START;
            job_busy_flags.b &= ~JOB_BUSY_PREPICK_ANY;
            job_state = JOB_STATE_FLATBED;
        } else if (mailbox.cmd_code == MDC_CMD_DOWNLOAD_START) {
            CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
            util_flash_enter_download();
            job_state = JOB_STATE_IDLE;
        } else {
            if (mailbox.cmd_code == MDC_CMD_FEED && (mailbox.feed_debug_flags & 8))
                break;
            CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
            cmd_phase = CMD_PHASE_START;
            do {
                cmd_dispatch();
            } while (cmd_phase && err_flags.b == 0);
        }
        break;
    case JOB_STATE_FEEDING:
        if (job_feed_state == JOB_FEED_STATE_SHEET_READY) {
            if (mailbox.cmd_state == MDC_CMD_STATE_RUNNING && (mailbox.feed_backing_request == 0x80 || mailbox.feed_backing_request == 0x81)) {
                while (bw_motor_backing_request != 0 && err_flags.b == 0) {
                    if (cmd_timeout == 0) {
                        bw_motor_record_backing(0x88);
                        err_flags.bit.device = 1;
                        err_saved_flags = err_flags.b;
                        err_saved_code = err_code = ERR_BACKING;
                        break;
                    }
                }
                if (err_flags.b == 0) {
                    CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_DONE);
                    mailbox.cmd_result = MDC_RESULT_SHEET_READY;
                }
                if (log_frozen == 0) {
                    log_hook_begin(0xfa);
                    p_log_buf[log_index] = 0xf7;
                    log_advance();
                    p_log_buf[log_index] = job_state;
                    log_advance();
                    p_log_buf[log_index] = LOG_END;
                    log_end();
                }
            } else if (mailbox.cmd_state == MDC_CMD_STATE_RUNNING && err_flags.b == 0) {
                if (err_flags.b == 0) {
                    CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_DONE);
                    mailbox.cmd_result = MDC_RESULT_SHEET_READY;
                }
                if (log_frozen == 0) {
                    log_hook_begin(0xfa);
                    p_log_buf[log_index] = 0xf7;
                    log_advance();
                    p_log_buf[log_index] = job_state;
                    log_advance();
                    p_log_buf[log_index] = LOG_END;
                    log_end();
                }
            }
            job_hold_ticks = 15000UL;
            job_state = JOB_STATE_SHEET_READY;
        }
        break;
    case JOB_STATE_SHEET_READY:
        if (mailbox.cmd_state == MDC_CMD_STATE_POSTED) {
            lamp_off_count = lamp_off_limit;
            if (mailbox.cmd_code == MDC_CMD_FEED) {
                job_busy_flags.b &= ~JOB_BUSY_PREPICK_ANY;
                job_stop_flags.w = 0;
                p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_ADF_LAMPS);
                if (sensor_flags.bit.pick) {
                    CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
                    if (err_flags.b == 0 && err_flags.b == 0) {
                        CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_DONE);
                        mailbox.cmd_result = MDC_RESULT_SHEET_READY;
                    }
                    if (log_frozen == 0) {
                        log_hook_begin(0xfa);
                        p_log_buf[log_index] = 0xf7;
                        log_advance();
                        p_log_buf[log_index] = job_state;
                        log_advance();
                        p_log_buf[log_index] = LOG_END;
                        log_end();
                    }
                } else {
                    CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
                    if (mailbox.sheet_count_high == 0 && mailbox.sheet_count_low == 0) {
                        mailbox.multifeed_flags_imprinter = 0;
                        feed_pick_edge_count = 0;
                    }
                    pick_next_sheet = 1;
                    job_feed_state = JOB_FEED_STATE_PREPARE;
                    job_state = JOB_STATE_FEEDING;
                }
            } else if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE && (mailbox.source & MDC_SOURCE_FLATBED)) {
                job_state = JOB_STATE_IDLE;
            } else if (mailbox.cmd_code == MDC_CMD_SCAN_MOVE) {
                set_imask_exr(7);
                p_p2->DR.BIT.B5 = 0;
                p_p2->DR.BIT.B6 = 0;
                lamp_heater_front_warmup = 0;
                lamp_heater_back_warmup = 0;
                hw_p2_out.b &= ~HW_P2_ADF_HEATERS;
                tick_warmup_hold = 0;
                if (feed_ready_stop == 0) {
                    job_busy_flags.bit.scan_move = 1;
                    set_imask_exr(0);
                    CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
                    mailbox.multifeed_flags.b = 0;
                    last_page_flags = mailbox.last_page;
                    if (mailbox.pick_ramp_steps_override)
                        last_page_flags |= 0x80;
                    last_page_flags |= mailbox.settings_90;
                    job_last_page_flags = last_page_flags;
                    if (last_page_flags & 0x80)
                        job_last_page = 1;
                    else
                        job_last_page = 0;
                    job_stop_flags.w = 0;
                    job_scan_state = JOB_SCAN_STATE_START;
                    job_busy_flags.b &= ~JOB_BUSY_PREPICK_ANY;
                    job_state = JOB_STATE_SCAN_START;
                }
                set_imask_exr(0);
            } else {
                CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_RUNNING);
                cmd_phase = CMD_PHASE_START;
                do {
                    cmd_dispatch();
                } while (cmd_phase && err_flags.b == 0);
            }
        } else {
            if (job_stop_flags.bit.requested && feed_ramp_state == RAMP_STATE_STOPPED && !job_capture_flags.bit.pause_active && job_feed_state == JOB_FEED_STATE_SHEET_READY && job_wake_flags.bit.wake_event &&
                !p_p1->DR.BIT.B6) {
                tick_flatbed_lamp_delay = 0;
                job_wake_flags.bit.flatbed_lamp_pending = 1;
            }
        }
        break;
    case JOB_STATE_SCAN_START:
    case JOB_STATE_SCAN_RESTART:
        if (job_scan_state == JOB_SCAN_STATE_IDLE) {
            job_state = JOB_STATE_SCANNING;
            job_scan_seconds = 0;
        } else if (feed_ramp_state == RAMP_STATE_STOPPED && job_state == JOB_STATE_SCAN_START) {
            job_scan_state = JOB_SCAN_STATE_START;
            job_state = JOB_STATE_SCAN_RESTART;
        }
        break;
    case JOB_STATE_SCANNING:
        if (!(job_busy_flags.b & JOB_BUSY_SCAN_MOVE))
            job_state = JOB_STATE_SHEET_END;
        if (feed_ramp_state == RAMP_STATE_STOPPED && !job_capture_flags.bit.pause_active) {
            if (job_scan_seconds > 0xf && job_front_reached == 0 && !sensor_flags.bit.top) {
                err_flags.bit.document = 1;
                err_code = ERR_JAM;
            }
        } else {
            if (job_scan_seconds > 0x19 && job_front_reached == 0 && !sensor_flags.bit.top) {
                err_flags.bit.document = 1;
                err_code = ERR_JAM;
            }
        }
        if (job_scan_seconds > 0x11 && feed_ramp_state == RAMP_STATE_STOPPED && !job_capture_flags.bit.pause_active) {
            err_flags.bit.document = 1;
            err_code = ERR_SHEET_GAP;
        }
        break;
    case JOB_STATE_EJECT:
        job_run_eject();
        if (job_eject_state == JOB_EJECT_STATE_IDLE) {
            if (err_flags.b == 0) {
                CALL_AS(void, job_set_cmd_state, (unsigned char))(MDC_CMD_STATE_DONE);
                mailbox.cmd_result = MDC_RESULT_OK;
            }
            job_state = JOB_STATE_IDLE;
        }
        break;
    case JOB_STATE_FLATBED:
        if (!(job_busy_flags.b & JOB_BUSY_SCAN_MOVE) && fb_motor_state == FB_MOTOR_STATE_AT_HOME)
            job_state = JOB_STATE_IDLE;
        break;
    }
    job_run_feed();
    if (job_scan_state)
        job_run_adf_scan();
    if (job_fb_motor_state)
        job_run_fb_motor();
}

void job_run_feed(void)
{
    unsigned char pad1[4];
    unsigned short last_page_flags;
    unsigned char retries;
    unsigned char pad2[3];

    retries = mailbox.settings_c5 & 0xf;
    switch (job_feed_state) {
    case JOB_FEED_STATE_PREPARE:
        mailbox.separation_sheet_flags.b = 0;
        bw_motor_arm_idle_ticks = 0;
        if (multifeed_check.adjust_state == 2) {
            multifeed_check.adjust_state = 1;
            NOPROTO(cmd_adjust_ultrasonic)(1);
            cmd_adjust_df_sensor();
        }
        if (mailbox.df_adjust_flags & MDC_DF_ADJUST_ULTRASONIC_OK) {
            set_imask_exr(7);
            if (feed_pick_edge_count == 0) {
                feed_ultrasonic_running = 0;
                feed_ultrasonic_starts_a = 0;
                feed_ultrasonic_starts_b = 0;
                job_df_on_count = 0;
                pick_df_on_count = 0;
                if (!multifeed_check.flags.bit.b8) {
                    sensor_stop_ultrasonic();
                    pick_flags_1.bit.ultrasonic_multifeed = 0;
                    feed_multifeed_flags.b = 0;
                }
                if (!multifeed_check.flags.bit.b8) {
                    multifeed_check.in_area = 0;
                    multifeed_check.sample_count = 0;
                    multifeed_check.sample_countdown = 0;
                    multifeed_check.low_count = 0;
                    multifeed_check.sum = 0;
                    multifeed_check.w36 = 0;
                    multifeed_check.w34 = 0;
                    multifeed_check.w32 = 0;
                    multifeed_check.flags.bit.b8 = 1;
                }
                set_imask_exr(0);
                job_set_multifeed_area();
                job_set_overlap_mode();
                log_error_armed = 1;
                log_error_word.w = 0;
            }
            set_imask_exr(0);
        } else {
            err_flags.bit.device = 1;
            err_code = ERR_ULTRASONIC_SENSOR;
        }
        if (mailbox.feed_options.b & MDC_FEED_LENGTH_CHECK)
            job_length_tolerance = (unsigned char)job_length_margin_by_option[mailbox.feed_options.b & MDC_FEED_LENGTH_TOLERANCE];
        if (mailbox.settings_b3 == 0) {
        } else if (mailbox.settings_b3 <= 0x32) {
            job_length_tolerance = (unsigned char)job_length_margin_by_setting[mailbox.settings_b3];
        } else {
            job_length_tolerance = (unsigned char)job_length_margin_by_setting[10];
        }
        pick_df_active = 0;
        mailbox.separation_sheet_flags.bit.detected = 0;
        if (mailbox.feed_options.b & MDC_FEED_JOB_SEPARATION) {
            if (mailbox.df_adjust_flags & MDC_DF_ADJUST_DF_OK) {
                set_imask_exr(7);
                pick_df_armed = 1;
                if (feed_ultrasonic_running == 0) {
                    sensor_df_on();
                    job_df_on_count++;
                }
                set_imask_exr(0);
            } else {
                err_flags.bit.device = 1;
                err_code = ERR_DF_SENSOR;
            }
        } else {
            pick_df_armed = 0;
            sensor_df_active = 0;
        }
        if (mailbox.speed_class_a > 4) {
            pick_slow_steps = pick_slow_steps_class_5_a;
            job_pick_slow_steps_b = pick_slow_steps_class_5_b;
        } else if (mailbox.speed_class_a > 3) {
            pick_slow_steps = pick_slow_steps_class_4_a;
            job_pick_slow_steps_b = pick_slow_steps_class_4_b;
        } else {
            pick_slow_steps = pick_slow_steps_a;
            job_pick_slow_steps_b = pick_slow_steps_b;
        }
        if (mailbox.speed_class_a > 4) {
            motor_current_tables[1][2][0] = 0;
            motor_current_tables[1][2][4] = 0x52;
            motor_current_tables[1][2][7] = 0x40;
            motor_current_tables[1][7][0] = 0;
            motor_current_tables[1][7][4] = 0x22;
            motor_current_tables[1][7][7] = 0x40;
        } else if (mailbox.speed_class_a > 3) {
            motor_current_tables[1][2][0] = 0;
            motor_current_tables[1][2][4] = 0x54;
            motor_current_tables[1][2][7] = 0x78;
            motor_current_tables[1][7][0] = 0;
            motor_current_tables[1][7][4] = 0x37;
            motor_current_tables[1][7][7] = 0x5d;
        } else {
            motor_current_tables[1][2][0] = 0;
            motor_current_tables[1][2][4] = 0x98;
            motor_current_tables[1][2][7] = 0x98;
            motor_current_tables[1][7][0] = 0;
            motor_current_tables[1][7][4] = 0x4b;
            motor_current_tables[1][7][7] = 0x7a;
        }
        if (feed_pick_edge_count == 0) {
            if (sensor_flags.bit.pick | sensor_flags.bit.top) {
                err_flags.bit.document = 1;
                err_code = ERR_EJECT_JAM;
                job_feed_state = JOB_FEED_STATE_ERROR;
                break;
            }
            if (mailbox.manual_feed_timeout) {
                mailbox.manual_feed_end = 0;
                tick_manual_end_ticks = 0;
                tick_manual_end_active = 0;
                sensor_flags.bit.no_paper = 0;
                feed_empty_seconds = 0;
                pick_empty_stop_seconds = 0x1e;
                job_feed_state = JOB_FEED_STATE_ARM_TO_LEVEL;
            } else if (job_eject_ticks != 0) {
                if (sensor_flags.bit.no_paper) {
                    err_flags.bit.document = 1;
                    err_code = ERR_NO_PAPER;
                    job_feed_state = JOB_FEED_STATE_ERROR;
                    job_eject_ticks = 0x9c4;
                } else {
                    job_feed_state = JOB_FEED_STATE_ARM_TO_LEVEL;
                    job_eject_ticks = 0;
                }
            } else {
                job_feed_state = JOB_FEED_STATE_ARM_TO_LEVEL;
                job_eject_ticks = 0x9c4;
            }
            pick_gap_stop_count = 0;
            pick_top_stop_count = 0;
            mailbox.cover_open_top_count = 0;
        } else {
            if (mailbox.speed_class_a > 5)
                mailbox.speed_class_a = 5;
            job_pick_speed_class = mailbox.speed_class_a;
            job_feed_last_page = job_last_page;
            if (job_feed_last_page == 1)
                pick_continuous = 1;
            else
                pick_continuous = pick_continuous_by_class[job_pick_speed_class];
            pick_continuous_gap_steps = pick_gap_steps_by_class[job_pick_speed_class];
            pick_ramp_steps = pick_ramp_steps_by_class[job_pick_speed_class];
            if (mailbox.pick_ramp_steps_override) {
                if (mailbox.pick_ramp_steps_override < 2) {
                    pick_ramp_steps = 2;
                    job_pick_speed_class = 5;
                } else if (mailbox.pick_ramp_steps_override > 0x4d) {
                    pick_ramp_steps = 0x4d;
                    job_pick_speed_class = 1;
                } else {
                    pick_ramp_steps = mailbox.pick_ramp_steps_override;
                    if (pick_ramp_steps <= 0xc)
                        job_pick_speed_class = 5;
                    else if (pick_ramp_steps <= 0x1d)
                        job_pick_speed_class = 4;
                    else
                        job_pick_speed_class = 1;
                }
            }
            if (job_pick_speed_class == 5)
                pick_run_current = MOTOR_CURRENT_RUN_CLASS_5;
            else if (job_pick_speed_class == 4)
                pick_run_current = MOTOR_CURRENT_RUN_CLASS_4;
            else
                pick_run_current = MOTOR_CURRENT_RUN;
            if (retries == 0)
                pick_retry_limit = 6;
            else if (retries > 8)
                pick_retry_limit = 8;
            else
                pick_retry_limit = retries;
            pick_timeout_steps = main_pick_timeout_steps;
            pick_top_retry_limit = 6;
            pick_top_timeout_steps = pick_top_timeout;
            if (bw_motor_arm_request == 0 && bw_motor_arm_state != BW_MOTOR_ARM_STATE_AT_LEVEL) {
                bw_motor_arm_request = BW_MOTOR_ARM_TO_LEVEL;
                job_feed_state = JOB_FEED_STATE_WAIT_ARM;
            } else {
                job_feed_state = JOB_FEED_STATE_PICK;
            }
        }
        break;
    case JOB_FEED_STATE_WAIT_ARM:
        if (bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL)
            job_feed_state = JOB_FEED_STATE_PICK;
        break;
    case JOB_FEED_STATE_ARM_TO_LEVEL:
        bw_motor_arm_idle_ticks = 0;
        if (bw_motor_arm_request == 0 && bw_motor_arm_state != BW_MOTOR_ARM_STATE_AT_LEVEL) {
            util_bw_motor_load_arm_settings();
            bw_motor_arm_request = BW_MOTOR_ARM_TO_LEVEL;
            job_feed_state = JOB_FEED_STATE_START;
        } else if (bw_motor_arm_request == 0 && bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL) {
            job_feed_state = JOB_FEED_STATE_START;
        }
        break;
    case JOB_FEED_STATE_START:
        if (bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_LEVEL) {
            if (sensor_flags.bit.no_paper) {
                err_flags.bit.document = 1;
                err_code = ERR_NO_PAPER;
                job_feed_state = JOB_FEED_STATE_ERROR;
                bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                break;
            }
            job_eject_ticks = 0;
            if (mailbox.speed_class_a > 5)
                mailbox.speed_class_a = 5;
            job_pick_speed_class = mailbox.speed_class_a;
            last_page_flags = mailbox.last_page;
            if (mailbox.pick_ramp_steps_override)
                last_page_flags |= 0x80;
            last_page_flags |= mailbox.settings_90;
            job_last_page_flags = last_page_flags;
            if (last_page_flags & 0x80) {
                job_feed_last_page = 1;
                job_last_page = 1;
            } else {
                job_feed_last_page = 0;
                job_last_page = 0;
            }
            if (job_feed_last_page == 1)
                pick_continuous = 1;
            else
                pick_continuous = pick_continuous_by_class[job_pick_speed_class];
            pick_continuous_gap_steps = pick_gap_steps_by_class[1];
            pick_ramp_steps = pick_ramp_steps_slow;
            pick_run_current = MOTOR_CURRENT_RUN;
            if (mailbox.pick_ramp_steps_override) {
                if (mailbox.pick_ramp_steps_override < 2)
                    pick_ramp_steps = 2;
                else if (mailbox.pick_ramp_steps_override > 0x4d)
                    pick_ramp_steps = 0x4d;
                else
                    pick_ramp_steps = mailbox.pick_ramp_steps_override;
            }
            p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_ADF_LAMPS);
            feed_pick_edge_state = 4;
            feed_top_edge_state = 4;
            if (mailbox.speed_class_b > 5)
                mailbox.speed_class_b = 5;
            feed_ramp_steps = feed_ramp_steps_by_class[mailbox.speed_class_b];
            feed_ramp_steps_next = feed_ramp_steps_by_class[mailbox.speed_class_b];
            feed_run_current = MOTOR_CURRENT_RUN;
            set_imask_exr(7);
            job_first_speed_class = 0;
            feed_ramp_steps = feed_ramp_steps_default;
            if (pick_continuous == 1) {
                if (mailbox.sheet_count_high == 0 && mailbox.sheet_count_low == 0) {
                    feed_action = MOTOR_ACTION_START;
                    job_first_speed_class = 1;
                    if (mailbox.long_page == MDC_LONG_PAGE_ON)
                        job_first_speed_class = 3;
                    if (mailbox.feed_debug_flags & 7) {
                        job_first_speed_class = mailbox.feed_debug_flags & 7;
                        if (job_first_speed_class > 5)
                            job_first_speed_class = 5;
                    }
                    job_select_fast_ramps(job_first_speed_class);
                    if (feed_fast_ramp == 1)
                        feed_ramp_steps = feed_fast_ramp_steps[job_first_speed_class];
                    else
                        feed_ramp_steps = feed_ramp_steps_by_class[job_first_speed_class];
                    feed_ramp_steps_next = feed_ramp_steps;
                    if (job_first_speed_class == 5)
                        feed_run_current = MOTOR_CURRENT_RUN_CLASS_5;
                    else if (job_first_speed_class == 4)
                        feed_run_current = MOTOR_CURRENT_RUN_CLASS_4;
                    else if (job_first_speed_class == 3)
                        feed_run_current = MOTOR_CURRENT_RUN;
                    else if (job_first_speed_class == 2)
                        feed_run_current = MOTOR_CURRENT_RUN;
                    else
                        feed_run_current = MOTOR_CURRENT_RUN;
                } else {
                    feed_top_edge_steps = 0x1f4;
                    p_chip_a_motor->period_1_high.f_0_8.f = 4;
                    p_chip_a_motor->period_1_low.f_0_8.f = 0xfd;
                }
            } else {
                feed_action = MOTOR_ACTION_START;
            }
            job_feed_speed_class = 0;
            feed_first_start = 1;
            set_imask_exr(0);
            pick_sheet_index = 0;
            pick_gap_index = 0;
            if (job_length_history_reset)
                feed_length_index = 0;
            job_length_history_reset = 0;
            feed_top_window_flags.w = 0;
            feed_gap_index = 0;
            pick_arm_pick_count = 0;
            if (retries == 0)
                pick_retry_limit = 6;
            else if (retries > 8)
                pick_retry_limit = 8;
            else
                pick_retry_limit = retries;
            pick_timeout_steps = main_pick_timeout_first_steps;
            pick_top_retry_limit = 6;
            pick_top_timeout_steps = pick_top_timeout;
            job_feed_state = JOB_FEED_STATE_PICK;
        }
        if (bw_motor_arm_request == 0 && bw_motor_arm_state != BW_MOTOR_ARM_STATE_AT_LEVEL)
            bw_motor_arm_request = BW_MOTOR_ARM_TO_LEVEL;
        break;
    case JOB_FEED_STATE_PICK:
        if (sensor_flags.bit.no_paper) {
            if (mailbox.cmd_code == MDC_CMD_FEED || (bw_motor_arm_request == 0 && bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_HOME)) {
                bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                job_feed_state = JOB_FEED_STATE_ARM_HOME_EMPTY;
            }
        } else {
            if (feed_ramp_state != RAMP_STATE_CONSTANT) {
                if (feed_ramp_state == RAMP_STATE_STOPPED) {
                    if (pick_continuous == 1) {
                    } else {
                        feed_action = MOTOR_ACTION_START;
                    }
                } else {
                    break;
                }
            }
            if (multifeed_check.flags.bit.b8)
                multifeed_check.paper_level_adjusted = 0x52;
            if (mailbox.feed_backing_request == 0x80) {
                set_imask_exr(7);
                bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
                set_imask_exr(0);
                cmd_timeout = CMD_TIMEOUT_TICKS;
            } else if (mailbox.feed_backing_request == 0x81) {
                set_imask_exr(7);
                bw_motor_backing_request = BW_MOTOR_BACKING_BLACK;
                set_imask_exr(0);
                cmd_timeout = CMD_TIMEOUT_TICKS;
            }
            if (log_frozen == 0) {
                log_hook_begin(0xfa);
                p_log_buf[log_index] = 0xf6;
                log_advance();
                p_log_buf[log_index] = job_state;
                log_advance();
                p_log_buf[log_index] = LOG_END;
                log_end();
            }
            job_busy_flags.bit.pick = 1;
            bw_motor_arm_idle_ticks = 0;
            pick_state = PICK_STATE_START;
            job_feed_state = JOB_FEED_STATE_PICKING;
        }
        break;
    case JOB_FEED_STATE_PICKING:
        if (!(job_busy_flags.b & JOB_BUSY_PICK)) {
            job_feed_state = JOB_FEED_STATE_SHEET_READY;
            job_page_count++;
            if (mailbox.manual_feed_timeout && tick_manual_end_active == 0)
                feed_empty_seconds = 0;
        }
        break;
    case JOB_FEED_STATE_SHEET_READY:
    case JOB_FEED_STATE_SHEET_READY_2:
        if (sensor_flags.bit.pick && (job_busy_flags.b & JOB_BUSY_SCAN_OR_EJECT) && !(mailbox.source & MDC_SOURCE_FLATBED))
            job_feed_state = JOB_FEED_STATE_WAIT_SHEET_CLEAR;
        break;
    case JOB_FEED_STATE_WAIT_SHEET_CLEAR:
        if (!sensor_flags.bit.pick || !sensor_flags.bit.top) {
            if (!(mailbox.non_rectangular & 1) && (mailbox.feed_options.b & MDC_FEED_PREPICK)) {
                if (!sensor_flags.bit.no_paper)
                    job_feed_state = JOB_FEED_STATE_PREPICK;
                else
                    job_feed_state = JOB_FEED_STATE_IDLE;
            } else {
                job_feed_state = JOB_FEED_STATE_DONE;
            }
        }
        break;
    case JOB_FEED_STATE_PREPICK:
    case JOB_FEED_STATE_DONE:
        break;
    case JOB_FEED_STATE_ARM_HOME_EMPTY:
        bw_motor_arm_idle_ticks = 0;
        if (bw_motor_arm_request == 0 && bw_motor_arm_state == BW_MOTOR_ARM_STATE_AT_HOME) {
            if (sensor_flags.bit.no_paper) {
                err_flags.bit.document = 1;
                err_code = ERR_NO_PAPER;
                job_feed_state = JOB_FEED_STATE_ERROR;
            } else {
                bw_motor_arm_request = BW_MOTOR_ARM_TO_LEVEL;
                job_feed_state = JOB_FEED_STATE_START;
            }
        }
        break;
    }
}

void job_run_adf_scan(void)
{
    unsigned char temperature;
    unsigned short max_length;
    unsigned char paper_option;
    unsigned short pad_offset;
    short bytes;
    unsigned short *edge_word;
    unsigned char shorten;
    unsigned short correction_steps;

    switch (job_scan_state) {
    case JOB_SCAN_STATE_START:
        if (mailbox.speed_class_a > 4) {
            pick_slow_steps = pick_slow_steps_class_5_a;
            job_pick_slow_steps_b = pick_slow_steps_class_5_b;
        } else if (mailbox.speed_class_a > 3) {
            pick_slow_steps = pick_slow_steps_class_4_a;
            job_pick_slow_steps_b = pick_slow_steps_class_4_b;
        } else {
            pick_slow_steps = pick_slow_steps_a;
            job_pick_slow_steps_b = pick_slow_steps_b;
        }
        if (feed_sheets_in_path == 0 || feed_ramp_state == RAMP_STATE_STOPPED) {
            bytes = 8;
            edge_word = feed_edge_records[0].edge;
            while (bytes != 0) {
                *edge_word++ = 0;
                bytes -= 2;
            }
            feed_pick_edge_count = 0;
            feed_top_edge_count = 0;
            feed_pick_edge_state = 0;
            feed_top_edge_state = 0;
            max_length = util_get_be16(&mailbox.max_sheet_length_high);
            if (max_length != 0)
                feed_max_sheet_length = max_length;
            else
                feed_max_sheet_length = 0x13ec;
            job_front_start_steps = util_get_be16(&mailbox.top_to_front_distance_high);
            job_front_end_steps = job_front_start_steps - job_read_lead_distance;
            if (mailbox.overscan_control & 0x80) {
                feed_pick_start_flags.w |= FEED_PICK_START_ENABLED;
                feed_pick_start_flags.w &= ~(FEED_PICK_START_FRONT | FEED_PICK_START_BACK);
                job_front_pick_start_steps = job_pick_to_front_distance;
                if (job_front_pick_start_steps < (mailbox.lead_front_low | (mailbox.lead_front_high << 8)))
                    job_front_pick_start_steps = 0;
                else
                    job_front_pick_start_steps -= (mailbox.lead_front_low | (mailbox.lead_front_high << 8));
            } else {
                feed_pick_start_flags.w = 0;
            }
            if (mailbox.lead_front_high == 0) {
                if (job_front_start_steps < mailbox.lead_front_low)
                    job_front_start_steps = 0;
                else
                    job_front_start_steps -= mailbox.lead_front_low;
                if (mailbox.end_lead_front_low == 0 && mailbox.end_lead_front_high == 0) {
                    if (job_front_end_steps < mailbox.lead_front_low)
                        job_front_end_steps = 0;
                    else
                        job_front_end_steps -= mailbox.lead_front_low;
                } else if (mailbox.end_lead_front_high == 0) {
                    if (job_front_end_steps < mailbox.end_lead_front_low)
                        job_front_end_steps = 0;
                    else
                        job_front_end_steps -= mailbox.end_lead_front_low;
                } else {
                    if (job_front_end_steps < (mailbox.end_lead_front_low | (mailbox.end_lead_front_high << 8)))
                        job_front_end_steps = 0;
                    else
                        job_front_end_steps -= (mailbox.end_lead_front_low | (mailbox.end_lead_front_high << 8));
                }
            } else {
                if (job_front_start_steps < (mailbox.lead_front_low | (mailbox.lead_front_high << 8)))
                    job_front_start_steps = 0;
                else
                    job_front_start_steps -= (mailbox.lead_front_low | (mailbox.lead_front_high << 8));
                if (mailbox.end_lead_front_low == 0 && mailbox.end_lead_front_high == 0) {
                    if (job_front_end_steps < (mailbox.lead_front_low | (mailbox.lead_front_high << 8)))
                        job_front_end_steps = 0;
                    else
                        job_front_end_steps -= (mailbox.lead_front_low | (mailbox.lead_front_high << 8));
                } else if (mailbox.end_lead_front_high == 0) {
                    if (job_front_end_steps < mailbox.end_lead_front_low)
                        job_front_end_steps = 0;
                    else
                        job_front_end_steps -= mailbox.end_lead_front_low;
                } else {
                    if (job_front_end_steps < (mailbox.end_lead_front_low | (mailbox.end_lead_front_high << 8)))
                        job_front_end_steps = 0;
                    else
                        job_front_end_steps -= (mailbox.end_lead_front_low | (mailbox.end_lead_front_high << 8));
                }
            }
            max_length = 0x7f;
            max_length += job_read_lead_margin;
            if (job_front_end_steps < max_length)
                job_front_end_steps = 0;
            else
                job_front_end_steps -= max_length;
            paper_option = mailbox.settings_80 & 0xe0;
            if (paper_option == 0x20 || paper_option == 0x80)
                pad_offset = 0x1d9;
            else
                pad_offset = 0;
            job_pad_offset = pad_offset;
            job_eject_steps = job_eject_count;
            if (mailbox.non_rectangular & 1) {
                job_pad_offset = job_pad_extra_length;
                job_eject_steps = job_eject_count_pad;
            }
            job_front_end_steps += job_pad_offset;
            job_front_end_steps /= 6;
            job_back_start_steps = util_get_be16(&mailbox.top_to_back_distance_high);
            job_back_end_steps = job_back_start_steps - job_read_lead_distance;
            if (feed_pick_start_flags.w != 0) {
                job_back_pick_start_steps = job_pick_to_back_distance;
                if (job_back_pick_start_steps < (mailbox.lead_back_low | (mailbox.lead_back_high << 8)))
                    job_back_pick_start_steps = 0;
                else
                    job_back_pick_start_steps -= (mailbox.lead_back_low | (mailbox.lead_back_high << 8));
            }
            if (mailbox.lead_back_high == 0) {
                if (job_back_start_steps < mailbox.lead_back_low)
                    job_back_start_steps = 0;
                else
                    job_back_start_steps -= mailbox.lead_back_low;
                if (mailbox.end_lead_back_low == 0 && mailbox.end_lead_back_high == 0) {
                    if (job_back_end_steps < mailbox.lead_back_low)
                        job_back_end_steps = 0;
                    else
                        job_back_end_steps -= mailbox.lead_back_low;
                } else {
                    if (job_back_end_steps < (mailbox.end_lead_back_low | (mailbox.end_lead_back_high << 8)))
                        job_back_end_steps = 0;
                    else
                        job_back_end_steps -= (mailbox.end_lead_back_low | (mailbox.end_lead_back_high << 8));
                }
            } else {
                if (job_back_start_steps < (mailbox.lead_back_low | (mailbox.lead_back_high << 8)))
                    job_back_start_steps = 0;
                else
                    job_back_start_steps -= (mailbox.lead_back_low | (mailbox.lead_back_high << 8));
                if (mailbox.end_lead_back_low == 0 && mailbox.end_lead_back_high == 0) {
                    if (job_back_end_steps < (mailbox.lead_back_low | (mailbox.lead_back_high << 8)))
                        job_back_end_steps = 0;
                    else
                        job_back_end_steps -= (mailbox.lead_back_low | (mailbox.lead_back_high << 8));
                } else {
                    if (job_back_end_steps < (mailbox.end_lead_back_low | (mailbox.end_lead_back_high << 8)))
                        job_back_end_steps = 0;
                    else
                        job_back_end_steps -= (mailbox.end_lead_back_low | (mailbox.end_lead_back_high << 8));
                }
            }
            max_length = 0x7f;
            max_length += job_read_lead_margin;
            if (job_back_end_steps < max_length)
                job_back_end_steps = 0;
            else
                job_back_end_steps -= max_length;
            job_back_end_steps += job_pad_offset;
            job_back_end_steps /= 6;
            if (!(mailbox.settings_af & 4)) {
                temperature = sensor_temperature_b;
                if (temperature > 0x3e) {
                    shorten = 1;
                    correction_steps = 0x17;
                } else if (temperature > 0x39) {
                    shorten = 1;
                    correction_steps = 0x15;
                } else if (temperature > 0x34) {
                    shorten = 1;
                    correction_steps = 0x12;
                } else if (temperature > 0x2f) {
                    shorten = 1;
                    correction_steps = 0xe;
                } else if (temperature > 0x2a) {
                    shorten = 1;
                    correction_steps = 0x9;
                } else if (temperature > 0x25) {
                    shorten = 1;
                    correction_steps = 0x3;
                } else if (temperature > 0x20) {
                    shorten = 0;
                    correction_steps = 0x5;
                } else if (temperature > 0x1b) {
                    shorten = 0;
                    correction_steps = 0xc;
                } else if (temperature > 0x16) {
                    shorten = 0;
                    correction_steps = 0x12;
                } else if (temperature > 0x11) {
                    shorten = 0;
                    correction_steps = 0x18;
                } else if (temperature > 0xc) {
                    shorten = 0;
                    correction_steps = 0x1c;
                } else if (temperature > 0x7) {
                    shorten = 0;
                    correction_steps = 0x20;
                } else if (temperature > 0x2) {
                    shorten = 0;
                    correction_steps = 0x22;
                } else {
                    shorten = 0;
                    correction_steps = 0x24;
                }
                if (shorten) {
                    if (job_front_start_steps < correction_steps)
                        job_front_start_steps = 0;
                    else
                        job_front_start_steps -= correction_steps;
                    if (job_back_start_steps < correction_steps)
                        job_back_start_steps = 0;
                    else
                        job_back_start_steps -= correction_steps;
                } else {
                    job_front_start_steps += correction_steps;
                    job_back_start_steps += correction_steps;
                }
            }
            if (!(mailbox.settings_af & 8) && mailbox.speed_class_b < 4) {
                job_front_start_steps += 0x13;
                job_back_start_steps += 0x13;
            }
            if (sensor_flags.bit.top) {
                job_scan_start_flags.bit.top_covered = 1;
                job_late_start_margin = 0;
            } else {
                job_scan_start_flags.bit.top_covered = 0;
                job_late_start_margin = 0;
            }
            if (mailbox.speed_class_b < 4) {
                job_front_start_steps >>= 1;
                job_back_start_steps >>= 1;
                job_late_start_margin >>= 1;
            }
            if (feed_pick_start_flags.w != 0) {
                job_front_pick_start_steps /= 6;
                job_back_pick_start_steps /= 6;
                if (mailbox.speed_class_b > 4)
                    feed_pick_start_timeout = job_pick_start_timeout_slow;
                else
                    feed_pick_start_timeout = job_pick_start_timeout;
            }
            if (mailbox.speed_class_b > 4)
                mailbox.speed_class_b = 5;
            if (mailbox.speed_class_b > 4)
                job_pick_gap_min = 0x28;
            else if (mailbox.speed_class_b > 3)
                job_pick_gap_min = 0x21;
            else
                job_pick_gap_min = 0x37;
            if ((paper_option == 0x20 || paper_option == 0x80) && !(mailbox.non_rectangular & 1))
                job_pick_gap_min += job_pad_offset / 6;
            job_select_fast_ramps(mailbox.speed_class_b);
            set_imask_exr(7);
            if (job_feed_speed_class == mailbox.speed_class_b) {
                feed_mode.bit.half_step = 0;
                if (feed_fast_ramp == 1)
                    feed_ramp_steps = feed_fast_ramp_steps[job_feed_speed_class];
                else
                    feed_ramp_steps = feed_ramp_steps_by_class[job_feed_speed_class];
                feed_ramp_steps_next = feed_ramp_steps;
                if (pick_fast_next == 1)
                    pick_ramp_steps_next = pick_ramp_steps_next_by_class[job_feed_speed_class];
                else
                    pick_ramp_steps_next = pick_ramp_steps_by_class[job_feed_speed_class];
            } else {
                job_feed_speed_class = mailbox.speed_class_b;
                if (feed_fast_ramp == 1)
                    feed_ramp_steps_next = feed_fast_ramp_steps[job_feed_speed_class];
                else
                    feed_ramp_steps_next = feed_ramp_steps_by_class[job_feed_speed_class];
                if (job_feed_speed_class == 5) {
                    feed_run_current = MOTOR_CURRENT_RUN_CLASS_5;
                    pick_run_current = MOTOR_CURRENT_RUN_CLASS_5;
                } else if (job_feed_speed_class == 4) {
                    feed_run_current = MOTOR_CURRENT_RUN_CLASS_4;
                    pick_run_current = MOTOR_CURRENT_RUN_CLASS_4;
                } else if (job_feed_speed_class == 3) {
                    feed_run_current = MOTOR_CURRENT_RUN;
                    pick_run_current = MOTOR_CURRENT_RUN;
                } else if (job_feed_speed_class == 2) {
                    feed_run_current = MOTOR_CURRENT_RUN;
                    pick_run_current = MOTOR_CURRENT_RUN;
                } else {
                    feed_run_current = MOTOR_CURRENT_RUN;
                    pick_run_current = MOTOR_CURRENT_RUN;
                }
                if (pick_fast_next == 1)
                    pick_ramp_steps_next = pick_ramp_steps_next_by_class[job_feed_speed_class];
                else
                    pick_ramp_steps_next = pick_ramp_steps_by_class[job_feed_speed_class];
            }
            feed_state = FEED_STATE_START;
            set_imask_exr(0);
            job_scan_state = JOB_SCAN_STATE_WAIT_SPEED;
        } else {
            if (job_feed_speed_class == mailbox.speed_class_b) {
                if ((mailbox.byte_0x3c6 & 0x20) && (mailbox.backing_position & 1) && bw_motor_backing_request != BW_MOTOR_BACKING_TEST) {
                    set_imask_exr(7);
                    bw_motor_backing_request = BW_MOTOR_BACKING_BLACK;
                    set_imask_exr(0);
                }
                job_scan_state = JOB_SCAN_STATE_WAIT_SPEED;
                if (feed_ramp_state != RAMP_STATE_CONSTANT) {
                    if (mailbox.speed_class_b > 5)
                        mailbox.speed_class_b = 5;
                    job_select_fast_ramps(mailbox.speed_class_b);
                    set_imask_exr(7);
                    if (job_feed_speed_class == mailbox.speed_class_b) {
                        feed_mode.bit.half_step = 0;
                        if (feed_fast_ramp == 1)
                            feed_ramp_steps = feed_fast_ramp_steps[job_feed_speed_class];
                        else
                            feed_ramp_steps = feed_ramp_steps_by_class[job_feed_speed_class];
                        feed_ramp_steps_next = feed_ramp_steps;
                        if (pick_fast_next == 1)
                            pick_ramp_steps_next = pick_ramp_steps_next_by_class[job_feed_speed_class];
                        else
                            pick_ramp_steps_next = pick_ramp_steps_by_class[job_feed_speed_class];
                    } else {
                        job_feed_speed_class = mailbox.speed_class_b;
                        if (feed_fast_ramp == 1)
                            feed_ramp_steps_next = feed_fast_ramp_steps[job_feed_speed_class];
                        else
                            feed_ramp_steps_next = feed_ramp_steps_by_class[job_feed_speed_class];
                        if (job_feed_speed_class == 5) {
                            feed_run_current = MOTOR_CURRENT_RUN_CLASS_5;
                            pick_run_current = MOTOR_CURRENT_RUN_CLASS_5;
                        } else if (job_feed_speed_class == 4) {
                            feed_run_current = MOTOR_CURRENT_RUN_CLASS_4;
                            pick_run_current = MOTOR_CURRENT_RUN_CLASS_4;
                        } else {
                            feed_run_current = MOTOR_CURRENT_RUN;
                            pick_run_current = MOTOR_CURRENT_RUN;
                        }
                        if (pick_fast_next == 1)
                            pick_ramp_steps_next = pick_ramp_steps_next_by_class[job_feed_speed_class];
                        else
                            pick_ramp_steps_next = pick_ramp_steps_by_class[job_feed_speed_class];
                    }
                    feed_state = FEED_STATE_START;
                    set_imask_exr(0);
                    job_scan_state = JOB_SCAN_STATE_WAIT_SPEED;
                }
            } else {
                if (job_feed_speed_class != mailbox.speed_class_b) {
                    job_feed_speed_class = mailbox.speed_class_b;
                    job_select_fast_ramps(job_feed_speed_class);
                    if (feed_fast_ramp == 1)
                        feed_ramp_steps_next = feed_fast_ramp_steps[job_feed_speed_class];
                    else
                        feed_ramp_steps_next = feed_ramp_steps_by_class[job_feed_speed_class];
                    if (job_feed_speed_class == 5) {
                        feed_run_current = MOTOR_CURRENT_RUN_CLASS_5;
                        pick_run_current = MOTOR_CURRENT_RUN_CLASS_5;
                    } else if (job_feed_speed_class == 4) {
                        feed_run_current = MOTOR_CURRENT_RUN_CLASS_4;
                        pick_run_current = MOTOR_CURRENT_RUN_CLASS_4;
                    } else if (job_feed_speed_class == 3) {
                        feed_run_current = MOTOR_CURRENT_RUN;
                        pick_run_current = MOTOR_CURRENT_RUN;
                    } else if (job_feed_speed_class == 2) {
                        feed_run_current = MOTOR_CURRENT_RUN;
                        pick_run_current = MOTOR_CURRENT_RUN;
                    } else {
                        feed_run_current = MOTOR_CURRENT_RUN;
                        pick_run_current = MOTOR_CURRENT_RUN;
                    }
                    if (pick_fast_next == 1)
                        pick_ramp_steps_next = pick_ramp_steps_next_by_class[job_feed_speed_class];
                    else
                        pick_ramp_steps_next = pick_ramp_steps_by_class[job_feed_speed_class];
                    feed_state = FEED_STATE_START;
                    set_imask_exr(0);
                    job_scan_state = JOB_SCAN_STATE_WAIT_SPEED;
                }
            }
        }
        if (mailbox.gap_extension_flags & 1) {
            mailbox.gap_extension_flags = 0;
            job_pick_gap_extend = 1;
            job_pick_gap_extended = mailbox.gap_extension;
            job_pick_gap_extended += job_pick_gap_min;
        } else if (mailbox.settings_96 != 0 && mailbox.gap_extension == 0) {
            job_pick_gap_extend = 1;
            job_pick_gap_extended = mailbox.settings_96;
            job_pick_gap_extended += job_pick_gap_min;
        }
        break;
    case JOB_SCAN_STATE_RESTART_STOPPED:
        if (feed_ramp_state == RAMP_STATE_STOPPED) {
            set_imask_exr(7);
            feed_mode.bit.half_step = 0;
            job_feed_speed_class = mailbox.speed_class_b;
            job_select_fast_ramps(job_feed_speed_class);
            if (feed_fast_ramp == 1)
                feed_ramp_steps = feed_fast_ramp_steps[job_feed_speed_class];
            else
                feed_ramp_steps = feed_ramp_steps_by_class[job_feed_speed_class];
            feed_ramp_steps_next = feed_ramp_steps;
            if (pick_fast_next == 1)
                pick_ramp_steps_next = pick_ramp_steps_next_by_class[job_feed_speed_class];
            else
                pick_ramp_steps_next = pick_ramp_steps_by_class[job_feed_speed_class];
            feed_state = FEED_STATE_START;
            set_imask_exr(0);
            job_scan_state = JOB_SCAN_STATE_WAIT_SPEED;
        }
        break;
    case JOB_SCAN_STATE_WAIT_PATH_EMPTY:
        if (feed_sheets_in_path == 0)
            job_scan_state = JOB_SCAN_STATE_WAIT_SPEED;
        break;
    case JOB_SCAN_STATE_WAIT_SPEED:
        if (feed_ramp_state == RAMP_STATE_CONSTANT && bw_motor_backing_request == 0) {
            if (mailbox.lamp_off_minutes)
                lamp_adf_off_limit = mailbox.lamp_off_minutes;
            else
                lamp_adf_off_limit = 0x3c;
            lamp_adf_off_count = 0;
            p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_ADF_LAMPS);
            if (feed_gap_index == 0) {
                job_wait_ticks = cmd_settle_ticks;
                job_scan_state = JOB_SCAN_STATE_SETTLE;
            } else {
                job_scan_state = JOB_SCAN_STATE_DONE;
            }
        }
        break;
    case JOB_SCAN_STATE_SETTLE:
        if (job_wait_ticks == 0) {
            set_imask_exr(7);
            motor_set_current(MOTOR_FEED, feed_run_current);
            set_imask_exr(0);
            feed_current_mode = CURRENT_MODE_RUN;
            job_scan_state = JOB_SCAN_STATE_DONE;
        }
        break;
    case JOB_SCAN_STATE_DONE:
        job_scan_state = JOB_SCAN_STATE_IDLE;
        break;
    }
}

void job_run_fb_motor(void)
{
    unsigned char pad[4];

    switch (job_fb_motor_state) {
    case JOB_FB_MOTOR_STATE_START:
        ramp_build_fb_motor();
        if (fb_motor_blocked) {
            util_fb_motor_home();
            if (fb_motor_blocked) {
                job_fb_motor_state = JOB_FB_MOTOR_STATE_IDLE;
                break;
            }
        }
        fb_motor_done_position = FB_MOTOR_DONE_POSITION;
        fb_motor_done_position -= mailbox.lead_flatbed_low;
        if (mailbox.settings_ad) {
            if (mailbox.settings_ad & 0x80)
                fb_motor_done_position -= (unsigned char)(mailbox.settings_ad & 0x7f);
            else
                fb_motor_done_position += mailbox.settings_ad;
        }
        fb_motor_scan_end = util_get_be16(&mailbox.flatbed_scan_length_high);
        if (fb_motor_scan_end != 0 && fb_motor_scan_end < 0x2aa1)
            fb_motor_scan_end += FB_MOTOR_DONE_POSITION;
        else
            fb_motor_scan_end = 0x4b8d;
        if (main_fb_home_cmd) {
            if (main_fb_home_cmd != 2)
                main_fb_home_cmd = 0;
            fb_motor_done_position = FB_MOTOR_DONE_POSITION;
            fb_motor_scan_end = 0x22c4;
        }
        if (mailbox.speed_class_b > 6)
            mailbox.speed_class_b = 6;
        job_fb_motor_speed_class = mailbox.speed_class_b;
        set_imask_exr(7);
        fb_motor_scan_ramp_end = fb_motor_ramp_end_by_class[job_fb_motor_speed_class];
        fb_motor_mode.bit.half_step = 0;
        if (mailbox.lamp_off_minutes)
            lamp_adf_off_limit = mailbox.lamp_off_minutes;
        else
            lamp_adf_off_limit = 0x3c;
        lamp_adf_off_count = 0;
        fb_motor_request = FB_MOTOR_SCAN;
        set_imask_exr(0);
        job_fb_motor_state = JOB_FB_MOTOR_STATE_WAIT_HOME;
        break;
    case JOB_FB_MOTOR_STATE_WAIT_HOME:
        if (fb_motor_state == FB_MOTOR_STATE_AT_HOME) {
            job_fb_motor_state = JOB_FB_MOTOR_STATE_IDLE;
            break;
        }
    }
}

void job_hook(unsigned long unused_arg, unsigned char unused_arg_2)
{
}

unsigned short job_scale_1000_254(unsigned short value)
{
    unsigned long product;

    product = value;
    product = product * 1000;
    product = product / 0xfe;
    return product;
}

void job_run_eject(void)
{
    switch (job_eject_state) {
    case JOB_EJECT_STATE_START:
        cmd_timeout = CMD_TIMEOUT_TICKS;
        if (job_busy_flags.b & JOB_BUSY_PREPICK) {
            if (!sensor_flags.bit.pick && !sensor_flags.bit.top) {
                job_eject_state = JOB_EJECT_STATE_IDLE;
            } else {
                if (feed_ramp_state == RAMP_STATE_STOPPED) {
                    feed_ramp_steps_next = feed_ramp_steps_default;
                    feed_state = FEED_STATE_START;
                }
                job_eject_state = JOB_EJECT_STATE_WAIT_FEED_END;
            }
        } else {
            if (!sensor_flags.bit.pick && !sensor_flags.bit.top) {
                job_eject_state = JOB_EJECT_STATE_IDLE;
            } else {
                if (feed_ramp_state == RAMP_STATE_STOPPED) {
                    feed_ramp_steps_next = feed_ramp_steps_default;
                    feed_state = FEED_STATE_START;
                }
                job_busy_flags.bit.eject = 1;
                job_eject_state = JOB_EJECT_STATE_PICK_ASSIST;
            }
        }
        break;
    case JOB_EJECT_STATE_PICK_ASSIST:
        if (feed_ramp_state == RAMP_STATE_CONSTANT) {
            if (!(job_busy_flags.b & JOB_BUSY_PREPICK) && sensor_flags.bit.pick) {
                pick_ramp_steps = pick_ramp_steps_slow;
                pick_action = MOTOR_ACTION_START;
            }
            job_eject_state = JOB_EJECT_STATE_WAIT_FEED_END;
        }
        break;
    case JOB_EJECT_STATE_WAIT_FEED_END:
        if (!(job_busy_flags.b & JOB_BUSY_PREPICK) && !sensor_flags.bit.pick && pick_ramp_state == RAMP_STATE_CONSTANT && pick_action == MOTOR_ACTION_NONE) {
            pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
        }
        if (feed_state == FEED_STATE_IDLE) {
            if (!(job_busy_flags.b & JOB_BUSY_PREPICK)) {
                pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            }
            job_eject_state = JOB_EJECT_STATE_WAIT_STOPPED;
        } else if (feed_state == FEED_STATE_WAIT_SHEET && cmd_timeout == 0) {
            if (!(job_busy_flags.b & JOB_BUSY_PREPICK)) {
                pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
            }
            feed_state = FEED_STATE_RESET;
            job_eject_state = JOB_EJECT_STATE_WAIT_STOPPED;
        }
        break;
    case JOB_EJECT_STATE_WAIT_STOPPED:
        if (feed_ramp_state == RAMP_STATE_STOPPED && pick_ramp_state == RAMP_STATE_STOPPED) {
            job_busy_flags.bit.eject = 0;
            job_eject_state = JOB_EJECT_STATE_IDLE;
        }
        break;
    }
}

void job_set_multifeed_area(void)
{
    unsigned long area_start;
    unsigned long area_length;
    unsigned char area_mode;

    area_start = ((unsigned long)mailbox.settings_bb * 0x3ef << 1) >> 8;
    area_length = ((unsigned long)mailbox.settings_be * 0x3ef << 1) >> 8;
    if (mailbox.settings_c0 & 0x20) {
        area_mode = 1;
    } else {
        area_mode = 0;
    }
    job_set_multifeed_area_steps(0, area_mode, area_start, area_length);
}

void job_set_multifeed_area_steps(unsigned short unused_arg, unsigned char area_mode, unsigned long area_start, unsigned long area_length)
{
    multifeed_check.area_mode = area_mode;
    if (area_start + area_length <= 0xc) {
        area_start = 0;
        area_length = 0;
    } else {
        area_start -= 0xc;
        if (area_length) {
            area_length += area_start;
            if (area_length & 0x80000000UL) {
                area_length = 1;
            } else if (area_length > 0x10000UL) {
                area_length = 0xffff;
            }
            if (area_start & 0x80000000UL) {
                area_start = 0;
            } else if (area_start > area_length) {
                area_start = area_length - 1;
            }
        } else {
            area_start = 0;
        }
    }
    if (area_start > 0xffffUL) {
        area_start = 0xfffe;
    }
    multifeed_check.area_start = area_start;
    multifeed_check.detect_start = area_start;
    if (area_length == 0) {
        multifeed_check.area_start = 0;
        multifeed_check.area_end = 0;
        multifeed_check.detect_start = 0;
        multifeed_check.detect_end = 0;
    } else if (area_mode) {
        multifeed_check.detect_end = area_length;
    } else {
        multifeed_check.area_end = area_length;
    }
}

void job_set_overlap_mode(void)
{
    unsigned char requested_mode;
    unsigned char overlap_mode;
    unsigned char overlap_options;
    short i;

    requested_mode = mailbox.settings_cb & OVERLAP_SETTINGS_MODE_MASK;
    overlap_mode = job_overlap_flags.b & OVERLAP_MODE_MASK;
    overlap_options = job_overlap_flags.b & OVERLAP_OPTIONS_MASK;
    if (requested_mode == 2) {
        requested_mode = 0;
    }
    if (requested_mode == OVERLAP_MODE_MANUAL || requested_mode != overlap_mode) {
        overlap_options &= ~(OVERLAP_UNMATCHED_SAVED | OVERLAP_UNMATCHED_MODE_5);
        feed_overlap_hold_sheets = 0;
        for (i = 0; i < 4; i++) {
            feed_overlap_starts_saved[i] = 0;
            feed_overlap_lengths_saved[i] = 0;
        }
    }
    if (requested_mode != overlap_mode && !(mailbox.settings_cb & OVERLAP_SETTINGS_KEEP_PATTERNS)) {
        cmd_clear_overlap_patterns();
    }
    if (requested_mode == OVERLAP_MODE_MANUAL) {
        if (mailbox.multifeed_bypass & 0x80) {
            feed_overlap_bypass = 1;
        } else {
            feed_overlap_bypass = 0;
        }
    } else if (requested_mode == OVERLAP_MODE_AUTO_1 || requested_mode == OVERLAP_MODE_AUTO_2) {
        if ((mailbox.multifeed_bypass & 0x80) && feed_overlap_lengths_saved[0]) {
            job_store_overlap_pattern();
        }
    }
    job_overlap_check_flags.w &= ~JOB_OVERLAP_CHECK_ACTIVE;
    feed_overlap_match_state = OVERLAP_MATCH_START;
    for (i = 0; i < 4; i++) {
        feed_overlap_starts[i] = 0;
        feed_overlap_lengths[i] = 0;
    }
    job_overlap_flags.b = requested_mode | overlap_options;
}

void job_store_overlap_pattern(void)
{
    short i;

    for (i = 0; i < 4; i++) {
        job_overlap_patterns_b[job_overlap_pattern_count][i] = feed_overlap_lengths_saved[i];
        job_overlap_patterns_a[job_overlap_pattern_count][i] = feed_overlap_starts_saved[i];
    }
    job_overlap_pattern_count++;
    if (job_overlap_pattern_count >= OVERLAP_PATTERN_COUNT) {
        job_overlap_pattern_count = 0;
    }
}

void job_select_fast_ramps(unsigned char speed_class)
{
    feed_fast_ramp = feed_fast_ramp_by_class[speed_class];
    pick_fast_next = pick_fast_next_by_class[speed_class];
    if (mailbox.long_page == MDC_LONG_PAGE_ON && speed_class == 3) {
        feed_fast_ramp = 0;
        pick_fast_next = 0;
    }
}

void job_set_cmd_state(unsigned char state)
{
    set_imask_exr(7);
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
    set_imask_exr(0);
}

void job_restore_backing(void)
{
    if (feed_ramp_state || bw_motor_backing_request || (mailbox.capture_flags.b & MDC_CAPTURE_PAUSED)) {
        return;
    }
    bw_motor_backing_quiet = 0;
    if (job_stop_flags.w & 0x40) {
        if (err_code == 0) {
            if (bw_motor_backing_request == 0 && bw_motor_backing_state != 0 && bw_motor_backing_state != 5) {
                set_imask_exr(7);
                bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
                bw_motor_backing_quiet = 1;
                set_imask_exr(0);
            }
            job_stop_flags.bit.restore_backing = 0;
            job_page_limit = 0;
            job_page_count = 0;
        }
    }
}
