#include "mdc.h"

void util_delay(void)
{
    short i;

    for (i = 0; i < 100; i++) {
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
    }
}

void util_flash_enter_download(void)
{
    unsigned char *src;
    unsigned char *dst;
    unsigned short count;

    set_imask_exr(7);
    p_p2->DR.BYTE = hw_p2_out.b = 0;
    p_p1->DR.BIT.B6 = 0;
    motor_enable_chips(0);
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
    src = flash_section_rom;
    dst = flash_section_start;
    count = 0x800;
    do {
        *dst++ = *src++;
    } while (--count);
    flash_run_download();
}

void util_lamp_clear_timers(void)
{
    lamp_heater_flatbed_warmup = 1;
    lamp_heater_front_warmup = 1;
    lamp_heater_back_warmup = 1;
    lamp_heater_control = 1;
}

void util_bw_motor_load_arm_settings(void)
{
    unsigned char sub;
    unsigned char mode;

    mode = mailbox.settings_29 & 3;
    sub = mailbox.settings_28 & 0xf;
    bw_motor_arm_default_steps = bw_motor_arm_base_steps + bw_motor_arm_level_7;
    bw_motor_arm_default_position = bw_motor_arm_level_7;
    util_arm_lift_position = bw_motor_arm_level_1;
    if (mode != 0) {
        if (mode == 1) {
            bw_motor_arm_default_steps = bw_motor_arm_mode_1_steps;
            bw_motor_arm_default_position = bw_motor_arm_level_0;
            util_arm_lift_position = 0xffff;
        } else if (mode == 2) {
            if (sub == 0 || sub <= 9) {
                bw_motor_arm_default_steps = bw_motor_arm_base_steps + bw_motor_arm_level_8;
                bw_motor_arm_default_position = bw_motor_arm_level_8;
            } else if (sub == 10) {
                bw_motor_arm_default_steps = bw_motor_arm_base_steps + bw_motor_arm_level_9;
                bw_motor_arm_default_position = bw_motor_arm_level_9;
            } else if (sub == 11) {
                bw_motor_arm_default_steps = bw_motor_arm_base_steps + bw_motor_arm_level_10;
                bw_motor_arm_default_position = bw_motor_arm_level_10;
            } else if (sub == 12) {
                bw_motor_arm_default_steps = bw_motor_arm_base_steps + bw_motor_arm_level_11;
                bw_motor_arm_default_position = bw_motor_arm_level_11;
            } else if (sub == 13) {
                bw_motor_arm_default_steps = bw_motor_arm_base_steps + bw_motor_arm_level_12;
                bw_motor_arm_default_position = bw_motor_arm_level_12;
            } else {
                bw_motor_arm_default_steps = bw_motor_arm_base_steps + bw_motor_arm_level_12;
                bw_motor_arm_default_position = bw_motor_arm_level_12;
            }
            util_arm_lift_position = 0xffff;
        }
    }
    if (mode != 2 || (mailbox.settings_d4 & 1)) {
        bw_motor_arm_mode = 1;
    } else {
        bw_motor_arm_mode = 0;
    }
}

void util_overlap_clear_after_error(void)
{
    unsigned char overlap_mode;
    unsigned char overlap_options;
    short i;

    overlap_mode = job_overlap_flags.b & OVERLAP_MODE_MASK;
    overlap_options = job_overlap_flags.b & OVERLAP_OPTIONS_MASK;
    overlap_options &= ~OVERLAP_UNMATCHED_MODE_5;
    feed_overlap_hold_sheets = 0;
    if (overlap_mode != OVERLAP_MODE_AUTO_1 && overlap_mode != OVERLAP_MODE_AUTO_2) {
        overlap_options &= ~OVERLAP_UNMATCHED_SAVED;
        for (i = 0; i < 4; i++) {
            feed_overlap_starts_saved[i] = 0;
            feed_overlap_lengths_saved[i] = 0;
        }
    }
    job_overlap_flags.b = overlap_mode | overlap_options;
}

void util_log_init(void)
{
    log_index = 0x100;
    util_put_be16(&mailbox.log_size_high, log_index);
    log_index = 0x10;
}

void util_log_add_error_word(void)
{
    unsigned char unused[10];

    log_error_armed = 0;
    if (log_error_word.w != 0) {
        log_hook_begin(0xfa);
        set_imask_exr(7);
        p_log_buf[log_index] = 0xf0;
        log_advance_255();
        p_log_buf[log_index] = (unsigned char)(log_error_word.w >> 8);
        log_advance_255();
        p_log_buf[log_index] = (unsigned char)log_error_word.w;
        log_advance_255();
        p_log_buf[log_index] = LOG_END;
        log_advance_255();
        p_log_buf[log_index] = LOG_END;
        log_advance_255();
        set_imask_exr(0);
        log_end();
    }
}

void util_backing_to_home(void)
{
    mailbox.backing_position = MDC_BACKING_BLACK;
    bw_motor_backing_state = 0;
    bw_motor_backing_request = BW_MOTOR_BACKING_WHITE;
    cmd_timeout = CMD_TIMEOUT_TICKS;
    while (bw_motor_backing_state != 5 && err_flags.b == 0) {
        if (cmd_timeout == 0) {
            bw_motor_record_backing(0x84);
            err_flags.bit.device = 1;
            err_saved_flags = err_flags.b;
            err_saved_code = err_code = ERR_BACKING;
            break;
        }
    }
}

void util_fb_motor_home(void)
{
    mailbox.fb_motor_status.b = 0;
    fb_motor_request = FB_MOTOR_HOME;
    fb_motor_state = FB_MOTOR_STATE_START;
    cmd_timeout = CMD_TIMEOUT_TICKS;
    fb_motor_ramp_index = 0;
    main_fb_home_cmd = 0;
    while (fb_motor_request != 0) {
        if (cmd_timeout == 0) {
            err_flags.bit.device = 1;
            err_saved_flags = err_flags.b;
            err_saved_code = err_code = ERR_FLATBED_MOTOR;
            break;
        }
    }
}

void util_feed_clear_path(void)
{
    unsigned char st;
    unsigned char saved;
    short imask;

    st = 0;
    imask = get_imask_exr();
    if (!sensor_flags.bit.adf_cover_open) {
        p_p2->DR.BYTE = (hw_p2_out.b |= HW_P2_ADF_LAMPS);
        saved = hw_p2_out.b;
        p_p2->DR.BYTE = (hw_p2_out.b &= ~HW_P2_HEATERS);
    } else {
        saved = hw_p2_out.b;
    }
    set_imask_exr(7);
    feed_clearing_path = 1;
    set_imask_exr(imask);
    while (err_flags.b == 0 && st != 9) {
        switch (st) {
        case 0:
            if (job_busy_flags.b & JOB_BUSY_PREPICK_PICKED) {
                st = 9;
                break;
            }
            if (mailbox.mechanics_init_mode == 5 || mailbox.mechanics_init_mode == 6) {
                st = 7;
                break;
            }
            if (main_init_after_wake != 0) {
                main_init_after_wake = 0;
                if (sensor_flags.bit.top || sensor_flags.bit.pick) {
                    st = 9;
                } else {
                    st = 7;
                }
                break;
            } else {
                cmd_timeout = 0x5dc;
                feed_pick_edge_state = 4;
                feed_top_edge_state = 4;
                if (sensor_flags.bit.top || sensor_flags.bit.pick || main_clear_path_pending != 0) {
                    if (feed_ramp_state != RAMP_STATE_CONSTANT) {
                        feed_fast_ramp = 0;
                        feed_speed_step = 0;
                        feed_speed_step_target = 0;
                        feed_speed_step_phase = 0;
                        job_capture_flags.b = 0;
                        feed_ramp_steps = feed_ramp_steps_default;
                        feed_action = MOTOR_ACTION_START;
                    }
                    st = 1;
                } else {
                    st = 7;
                }
                main_clear_path_pending = 0;
                break;
            }
        case 1:
            if (cmd_timeout == 0) {
                feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                st = 6;
            } else if (feed_ramp_state == RAMP_STATE_CONSTANT && !sensor_flags.bit.top) {
                if (sensor_flags.bit.pick) {
                    cmd_timeout = 0x5dc;
                    st = 2;
                    feed_top_edge_steps = 0x1f4;
                    pick_ramp_steps = pick_ramp_steps_slow;
                    pick_action = MOTOR_ACTION_START;
                } else {
                    cmd_timeout = 0xfa;
                    st = 4;
                }
            }
            break;
        case 2:
        case 3:
            if (cmd_timeout == 0) {
                pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                st = 6;
            } else if (sensor_flags.bit.top) {
                pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                st = 4;
            }
            break;
        case 4:
        case 5:
            if (cmd_timeout == 0) {
                if (st == 4 && sensor_flags.bit.top) {
                    cmd_timeout = 0x5dc;
                    st = 5;
                } else {
                    feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
                    st = 6;
                }
            }
            break;
        case 6:
            if (feed_ramp_state == RAMP_STATE_STOPPED) {
                if (sensor_flags.bit.pick || sensor_flags.bit.top) {
                    if (!(err_flags.b & ERR_FLAG_DEVICE)) {
                        err_flags.bit.document = 1;
                        err_code = ERR_EJECT_JAM;
                    }
                }
                feed_pick_edge_state = 0;
                feed_top_edge_state = 0;
                st = 7;
            }
            break;
        case 7:
            if ((mailbox.mechanics_init_mode == 5 || mailbox.mechanics_init_mode == 6) && sensor_flags.bit.pick_arm) {
                st = 9;
                break;
            } else {
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_START;
                bw_motor_arm_request = BW_MOTOR_ARM_HOME;
                st = 8;
                break;
            }
        case 8:
            if (bw_motor_arm_request == 0) {
                st = 9;
            }
            break;
        }
    }
    if (pick_ramp_state != RAMP_STATE_STOPPED) {
        cmd_timeout = 0x12c;
        while (cmd_timeout != 0)
            ;
        pick_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
        while (pick_ramp_state != RAMP_STATE_STOPPED)
            ;
    }
    if (feed_ramp_state != RAMP_STATE_STOPPED) {
        cmd_timeout = 0x5dc;
        while (cmd_timeout != 0)
            ;
        feed_action = MOTOR_ACTION_STOP_KEEP_CURRENT;
        while (feed_ramp_state != RAMP_STATE_STOPPED)
            ;
    }
    set_imask_exr(7);
    feed_clearing_path = 0;
    set_imask_exr(imask);
    hw_p2_out.b = saved;
    p_p2->DR.BYTE = hw_p2_out.b;
}

void util_mem_clear(unsigned char *p, unsigned long count)
{
    while (count--) {
        *p = 0;
        p++;
    }
    return;
}

void util_mem_copy(unsigned char *dst, unsigned char *src, unsigned long count)
{
    while (count--) {
        *dst = *src;
        src++;
        dst++;
    }
    return;
}

int util_mem_equal(unsigned char *left, unsigned char *right, unsigned long count)
{
    while (count--) {
        if (*left != *right)
            return 0;
        left++;
        right++;
    }
    return 1;
}

int util_mem_equal_count(unsigned char *left, unsigned char *right, unsigned long count, unsigned long *equal_count)
{
    *equal_count = 0;
    while (count--) {
        if (*left != *right)
            return 0;
        left++;
        right++;
        (*equal_count)++;
    }
    return 1;
}

int util_mem_check_fill(unsigned char *p, unsigned char value, unsigned long count, unsigned long *fill_count)
{
    *fill_count = 0;
    while (count--) {
        if (*p != value)
            return 0;
        p++;
        (*fill_count)++;
    }
    return 1;
}

void util_mem_fill(unsigned char *p, unsigned char value, unsigned long count)
{
    while (count--) {
        *p = value;
        p++;
    }
    return;
}

void util_mem_fill_short(unsigned char *p, short count, unsigned char value)
{
    short i;

    for (i = 0; i < count; i++) {
        *p++ = value;
    }
    return;
}

void util_put_be16(unsigned char *p, unsigned short value)
{
    unsigned short high;

    high = value;
    *p++ = high >> 8;
    *p = value;
    return;
}

unsigned short util_get_be16(unsigned char *p)
{
    unsigned short high;
    unsigned short low;

    high = *p++;
    low = *p;
    high = high << 8;
    low = low & 0xff;
    high += low;
    return high;
}
