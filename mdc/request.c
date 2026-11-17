#include "mdc.h"

#pragma interrupt (isr_tgi2a)
void isr_tgi2a(void)
{
    unsigned long l16;
    unsigned short log_position;

    p_tpu->TSTR.BIT.CST2 = 0;
    request_saved_imask = get_imask_exr();
    if (!p_tpu2->TSR.BIT.TGFA) {
        return;
    }
    {
        p_tpu2->TSR.BIT.TGFA = 0;
        set_imask_exr(request_saved_imask);
        if (mailbox.request == MDC_REQUEST_REINIT_FAST || mailbox.request == MDC_REQUEST_REINIT_WAKE) {
            if (mailbox.request == MDC_REQUEST_REINIT_FAST) {
                mailbox.mechanics_init_mode = 5;
                main_init_state = 4;
            } else if (mailbox.request == MDC_REQUEST_REINIT_WAKE) {
                mailbox.mechanics_init_mode = 6;
                main_init_state = 4;
            }
        } else if (mailbox.request == MDC_REQUEST_MOTORS_OFF || mailbox.request == MDC_REQUEST_RESTART) {
            motor_set_current(MOTOR_FEED, MOTOR_CURRENT_OFF);
            motor_set_current(MOTOR_PICK, MOTOR_CURRENT_OFF);
            motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_OFF);
            motor_set_current(MOTOR_FB, MOTOR_CURRENT_OFF);
            p_p2->DR.BYTE = hw_p2_out.b = 0;
        } else if (mailbox.request == MDC_REQUEST_CAPTURE_PAUSE) {
            set_imask_exr(7);
            if (mailbox.motion_flags.b & MDC_MOTION_SIDES) {
                job_capture_flags.bit.pause_requested = 1;
                job_capture_flags.bit.pause_active = 1;
                fb_motor_mark_position = fb_motor_position;
            } else if (job_top_clear_steps < 0x37b) {
                job_capture_flags.bit.pause_requested = 1;
                job_capture_flags.bit.pause_active = 1;
                fb_motor_mark_position = fb_motor_position;
            } else {
                job_capture_flags.bit.pause_active = 0;
                mailbox.capture_flags.bit.paused = 1;
                mailbox.capture_flags.bit.steps_valid = 1;
            }
            set_imask_exr(request_saved_imask);
        } else if (mailbox.request == MDC_REQUEST_CAPTURE_RESUME) {
            set_imask_exr(7);
            if (job_capture_flags.bit.pause_active) {
                if (mailbox.motion_flags.b & MDC_MOTION_SIDES) {
                    job_capture_flags.bit.resume_requested = 1;
                } else if (feed_state == FEED_STATE_CAPTURE_STOP || feed_state == FEED_STATE_CAPTURE_SETTLE || feed_state == FEED_STATE_CAPTURE_PAUSED || fb_motor_state == FB_MOTOR_STATE_CAPTURE_STOP || fb_motor_state == FB_MOTOR_STATE_CAPTURE_SETTLE || fb_motor_state == FB_MOTOR_STATE_CAPTURE_BACK_UP || fb_motor_state == FB_MOTOR_STATE_CAPTURE_PAUSED) {
                    job_capture_flags.bit.resume_requested = 1;
                } else if (job_top_clear_steps < 0x37b) {
                    job_capture_flags.bit.resume_requested = 1;
                } else {
                    job_capture_flags.b = 0;
                    mailbox.capture_flags.b = 0;
                }
            } else {
                if (feed_state == FEED_STATE_CAPTURE_STOP || feed_state == FEED_STATE_CAPTURE_SETTLE || feed_state == FEED_STATE_CAPTURE_PAUSED || fb_motor_state == FB_MOTOR_STATE_CAPTURE_STOP || fb_motor_state == FB_MOTOR_STATE_CAPTURE_SETTLE || fb_motor_state == FB_MOTOR_STATE_CAPTURE_BACK_UP || fb_motor_state == FB_MOTOR_STATE_CAPTURE_PAUSED) {
                    job_capture_flags.bit.resume_requested = 1;
                } else {
                    mailbox.capture_flags.b = 0;
                }
            }
            set_imask_exr(request_saved_imask);
        } else if (mailbox.request == MDC_REQUEST_STOP) {
            fb_motor_stop_position = FB_MOTOR_DONE_POSITION;
            if (mailbox.manual_feed_timeout) {
                set_imask_exr(7);
                tick_manual_end_ticks = 0;
                tick_manual_end_active = 1;
                job_stop_flags.bit.manual_feed_stop = 1;
                set_imask_exr(request_saved_imask);
            }
            job_stop_flags.w |= 0xc0;
        } else if (mailbox.request == MDC_REQUEST_ABORT) {
            fb_motor_stop_position = FB_MOTOR_DONE_POSITION;
            err_flags.bit.document = 1;
            err_code = ERR_STOPPED;
        } else if (mailbox.request == MDC_REQUEST_HEATER_FRONT_OFF) {
            hw_p2_out.bit.front_heater = 0;
            p_p2->DR.BYTE = hw_p2_out.b;
            lamp_unit_front_on = 0;
        } else if (mailbox.request == MDC_REQUEST_HEATER_BACK_OFF) {
            hw_p2_out.bit.back_heater = 0;
            p_p2->DR.BYTE = hw_p2_out.b;
            lamp_unit_back_on = 0;
        } else if (mailbox.request == MDC_REQUEST_HEATERS_OFF) {
            p_p2->DR.BYTE = (hw_p2_out.b &= ~HW_P2_ADF_HEATERS);
            lamp_unit_front_on = 0;
            lamp_unit_back_on = 0;
        } else if (mailbox.request == MDC_REQUEST_LOG_STATE) {
            mailbox.dump_count++;
            log_hook_begin(0);
            log_position = log_index;
            set_imask_exr(7);
            p_log_buf[log_index] = 0xdd;
            log_advance();
            p_log_buf[log_index] = 0xdd;
            log_advance();
            p_log_buf[log_index] = (unsigned char)(log_position >> 8);
            log_advance();
            p_log_buf[log_index] = (unsigned char)log_position;
            log_advance();
        p_log_buf[log_index] = job_state;
        log_advance();
        p_log_buf[log_index] = job_busy_flags.b;
        log_advance();
        p_log_buf[log_index] = job_feed_state;
        log_advance();
        p_log_buf[log_index] = err_state;
        log_advance();
        p_log_buf[log_index] = err_code;
        log_advance();
        p_log_buf[log_index] = err_saved_code;
        log_advance();
        p_log_buf[log_index] = mailbox.error_code;
        log_advance();
        p_log_buf[log_index] = mailbox.dump_count;
        log_advance();
        p_log_buf[log_index] = mailbox.motion_flags.b;
        log_advance();
        p_log_buf[log_index] = mailbox.cmd_state;
        log_advance();
        p_log_buf[log_index] = mailbox.cmd_code;
        log_advance();
        p_log_buf[log_index] = mailbox.cmd_result;
        log_advance();
        p_log_buf[log_index] = LOG_END;
            set_imask_exr(request_saved_imask);
            log_end();
            job_state = JOB_STATE_RESET;
            err_state = 0;
            err_flags.b = 0;
            err_code = 0;
            err_saved_flags = 0;
            err_saved_code = 0;
            err_deferred = 0;
            mailbox.error_flags.b = 0;
            mailbox.error_code = 0;
        }
        if (mailbox.request == MDC_REQUEST_RESTART) {
            set_imask_exr(7);
            set_imask_exr(7);
            set_imask_exr(7);
            p_p2->DR.BYTE = hw_p2_out.b = 0;
            p_p1->DR.BIT.B6 = 0;
            NOPROTO(motor_enable_chips)(0);
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
            set_imask_exr(7);
            set_imask_exr(7);
            set_imask_exr(7);
            mailbox.handshake_high = 0;
            mailbox.handshake_low = 0;
            mailbox.request = 0;
            (*(void (**)(void))0)();
        }
        lamp_off_count = lamp_off_limit;
        if (err_flags.b == 0) {
            mailbox.request = 0;
        } else {
            mailbox.request = 0;
        }
    }
}
