#include "fw.h"
#pragma section H

extern void _builtin_set_imask(int);

void main_task_wake_requests(void)
{
    int done;

    done = 0;
    if (hw_hostif_flags.f.own_hostif && job_aborted && !scsi_mdc_log_pending && scsi_resume_flags.f.resume_pending) {
        if (!page_size_wanted && !page_size_ready && !page_size_reply_pending) {
            if ((jpeg_scan_flags.v & JPEG_SCAN_PAUSED) && !power_state.f.saving) {
                if (job_long_page_flags & 128) {
                } else {
                    mdc_request_capture(1);
                    done = 1;
                }
            }
            isr_request_command_layer(0);
            if (done) {
                _builtin_set_imask(15);
                tick_wait_count = 0;
                _builtin_set_imask(0);
                while (tick_wait_count < 5000)
                    ;
                mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
            }
        }
    }
    if (scsi_mdc_log_request) {
        scsi_mdc_log_request = 0;
        if (power_state.f.saving) {
            power_state.f.leave_request = 1;
            power_leave_save();
        }
        if (scsi_diag_request == SCSI_DIAG_REQUEST_GET_DEV_LOG_DAT) {
            mdc_run_dump_log();
            scsi_mdc_log_pending = 0;
        }
        _builtin_set_imask(0);
        isr_request_command_layer(0);
        _builtin_set_imask(0);
    }
    if (main_settings_request) {
        _builtin_set_imask(15);
        main_settings_request = 0;
        _builtin_set_imask(0);
        if (power_state.f.saving) {
            power_state.f.leave_request = 1;
            power_leave_save();
        }
        mdc_run_command(MDC_CMD_CLEAR_OVERLAP_PATTERNS);
    }
}


struct vdcc_regs_04_05 {
    char pad_00[4];
    volatile struct vdcc_mdc_event_bits mdc_event;
    volatile unsigned char events;
};


struct uart_reg_mcr { char pad_00[9]; volatile struct Bits mcr; };

void main_task_start_updater(void)
{
    void (*updater)();
    unsigned char sync_mode;
    unsigned char ultra;

    if (mdc_motor_test_state.w || line_side_state[0].state_flags.v || line_side_state[1].state_flags.v) {
        return;
    }
    _builtin_set_imask(15);
    if (scsi_update_request & 64) {
        _builtin_set_imask(0);
        isr_request_command_layer(0);
        _builtin_set_imask(0);
    }
    if (scsi_update_request & 128) {
        _builtin_set_imask(0);
        sync_mode = ((struct scsi_regs_dummy *)scsi_ctrl)->syncmode;
        if (((struct scsi_regs_dummy *)scsi_ctrl)->scsimode0.b4) {
            ultra = 1;
        } else {
            ultra = 0;
        }
        if (hw_hostif_flags.f.usb == 0) {
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct scsi_regs_dummy *)scsi_ctrl)->reset = 7;
            _builtin_set_gbr(0);
            ((struct scsi_regs_dummy *)scsi_ctrl)->scsimode0.b7 = 0;
            _builtin_set_gbr(0);
            ((struct scsi_regs_dummy *)scsi_ctrl)->scsimode0.b6 = 1;
            _builtin_set_imask(0);
        }
        if (!power_state.f.saving) {
            power_enter_save(0);
        } else {
            p_board_ctrl->reg_02.hostif_idle = 0;
            p_board_ctrl->reg_02.standby = 0;
            p_board_ctrl->reg_04.hostif_idle_enable = 0;
        }
        _builtin_set_imask(15);
        ((struct vdcc_regs_04_05 *)p_vdcc_front[0])->events = 0xfd;
        ((struct vdcc_regs_04_05 *)p_vdcc_front[0])->mdc_event.mdc_result_ready = 1;
        ((struct vdcc_regs_04_05 *)p_vdcc_back[0])->events = 0xfd;
        ((struct vdcc_regs_04_05 *)p_vdcc_back[0])->mdc_event.mdc_result_ready = 1;
        p_pisces_edge[0]->ctrl.enable = 0;
        p_pisces_edge[1]->ctrl.enable = 0;
        p_pisces_ctrl[0]->reg_00a.v = 0;
        p_pisces_ctrl[0]->event_enable.f.b4 = 0;
        p_pisces_ctrl[0]->reg_00a.f.b4 = 1;
        p_pisces_ctrl[0]->event_enable.f.b4 = 1;
        p_pisces_ctrl[1]->reg_00a.v = 0;
        p_pisces_ctrl[1]->event_enable.f.b4 = 0;
        p_pisces[0]->n.b0 = 1;
        p_pisces[1]->n.b0 = 1;
        hw_enable_vdcc(0);
        isr_stop_cmt1();
        p_intc->IPRA.WORD = 0;
        p_intc->IPRB.WORD = 0;
        p_intc->IPRC.WORD = 0;
        p_intc->IPRD.WORD = 0;
        p_intc->IPRE.WORD = 0;
        p_intc->IPRF.WORD = 0;
        p_intc->IPRG.WORD = 0;
        p_intc->IPRH.WORD = 0;
        p_intc->IPRI.WORD = 0;
        p_intc->IPRJ.WORD = 0;
        *(*(volatile unsigned char **)&p_mtu) = 0;
        p_cmt->CMSTR.WORD = 0;
        if (p_cmt0->CMCSR.BIT.CMF) {
            p_cmt0->CMCSR.BIT.CMF = 0;
        }
        if (p_cmt1->CMCSR.BIT.CMF) {
            p_cmt1->CMCSR.BIT.CMF = 0;
        }
        if (p_mtu2->TSR.BIT.TGFA) {
            p_mtu2->TSR.BIT.TGFA = 0;
        }
        if (p_mtu3->TSR.BIT.TGFA) {
            p_mtu3->TSR.BIT.TGFA = 0;
        }
        if (p_mtu4->TSR.BIT.TGFA) {
            p_mtu4->TSR.BIT.TGFA = 0;
        }
        p_cmt0->CMCSR.BIT.CMIE = 0;
        p_cmt1->CMCSR.BIT.CMIE = 0;
        p_mtu1->TIER.BIT.TGIEA = 0;
        p_mtu2->TIER.BIT.TGIEA = 0;
        p_mtu3->TIER.BIT.TGIEA = 0;
        p_mtu4->TIER.BIT.TGIEA = 0;
        p_cmt0->CMCNT = 0;
        p_cmt1->CMCNT = 0;
        p_mtu1->TCNT = 0;
        p_mtu2->TCNT = 0;
        p_mtu3->TCNT = 0;
        p_mtu4->TCNT = 0;
        p_sci1->SCR.BIT.RE = 0;
        p_sci1->SCR.BIT.RIE = 0;
        p_sci0->SCR.BIT.RE = 0;
        p_sci0->SCR.BIT.RIE = 0;
        p_intc->IPRA.WORD = 0;
        p_sci1->SCR.BIT.RE = 0;
        p_sci1->SCR.BIT.RIE = 0;
        p_sci0->SCR.BIT.RE = 0;
        p_sci0->SCR.BIT.RIE = 0;
        ((struct uart_reg_mcr *)p_uart[0])->mcr.b0 = 0;
        updater = boot_run_updater;
        if (hw_hostif_flags.f.usb) {
            updater(0, 0, 0);
        } else {
            updater(1, sync_mode, ultra);
        }
    }
    _builtin_set_imask(0);
}
void main_task_download(void)
{
    int x;

    if (!(flash_download_session & 128)) {
        return;
    }
    if (power_state.f.saving) {
        power_state.f.leave_request = 1;
        power_leave_save();
    }
    if (flash_download_pending == 0) {
        return;
    }
    _builtin_set_imask(15);
    flash_download_pending = 0;
    _builtin_set_imask(0);
    flash_mdc_run_download();
    _builtin_set_imask(15);
    scsi_resume_flags.f.resume_pending = 1;
    _builtin_set_imask(0);
    isr_request_command_layer(0);
    _builtin_set_imask(0);
}


void main_task_clear_error_display(void)
{
    int x;
    int ultrasonic_multifeed;
    unsigned short wait_ticks;

    if (mdc_motor_test_state.w != 0) {
        return;
    }
    if ((*(char *)&err_display_flags & 64) == 0 && (*(char *)&err_display_flags & 128) == 0) {
        return;
    }
    if (mdc_mailbox.sensor_status.f.adf_cover_open || mdc_mailbox.sensor_status.f.top || mdc_mailbox.sensor_status.f.pick) {
        if (mdc_mailbox.sensor_status.f.adf_cover_open) {
            _builtin_set_imask(15);
            main_error_cover_opened = 1;
            _builtin_set_imask(0);
        }
        tick_error_ticks.v = 0;
        return;
    }
    if ((mdc_mailbox.error_code.v == ERR_MULTIFEED_ULTRASONIC || err_last_code.s.v == ERR_MULTIFEED_ULTRASONIC) && testmode_number.v == 0 && (maint_mode_word.v & 0x4000) == 0) {
        ultrasonic_multifeed = 1;
        if (!hw_hostif_flags.f.own_hostif) {
            _builtin_set_imask(15);
            panel_multifeed_blink_state.v = 0x100;
            panel_multifeed_blink_age_polls.v = 0;
            _builtin_set_imask(0);
        }
    } else {
        ultrasonic_multifeed = 0;
    }
    _builtin_set_imask(15);
    tick_wait_count = 0;
    _builtin_set_imask(0);
    if (power_mdc_restart_state.f.timeout_error) {
        wait_ticks = 5000;
    } else {
        wait_ticks = 1500;
    }
    while (tick_wait_count < wait_ticks) {
        if (panel_current.display_main == PANEL_MODE_ERROR && panel_current.display_sub == PANEL_ERROR_J2 && !main_error_cover_opened) {
            _builtin_set_imask(15);
            panel_multifeed_blink_state.v = 0;
            _builtin_set_imask(0);
            return;
        }
        if ((maint_mode_word.v & 0x4000) && maint_mode_word.c.mode == MAINT_MODE_WHITE_LEVEL) {
            return;
        }
        if (mdc_mailbox.sensor_status.f.adf_cover_open || mdc_mailbox.sensor_status.f.top || mdc_mailbox.sensor_status.f.pick) {
            tick_error_ticks.v = 0;
            _builtin_set_imask(15);
            panel_multifeed_blink_state.v = 0;
            _builtin_set_imask(0);
            return;
        }
    }
    _builtin_set_imask(15);
    err_display_flags.b.b6 = 0;
    err_display_flags.b.b2 = 0;
    main_error_cover_opened = 0;
    err_display_flags.b.b7 = 0;
    power_mdc_restart_state.f.timeout_error = 0;
    _builtin_set_imask(0);
    main_restore_display_mode();
    if (ultrasonic_multifeed) {
        _builtin_set_imask(15);
        panel_multifeed_blink_state.v |= 0x8000;
        _builtin_set_imask(0);
    }
}
void main_task_restore_display(void)
{
    if (err_display_flags.b.b6 || err_display_flags.b.b5) {
        return;
    }
    if (panel_current.display_main != PANEL_MODE_ERROR || panel_current.display_sub != PANEL_ERROR_U4) {
        return;
    }
    tick_wait_count = 0;
    while (tick_wait_count < 50) {
        if (mdc_mailbox.sensor_status.f.adf_cover_open) {
            return;
        }
    }
    main_restore_display_mode();
}
void main_task_retry_mdc_error(void)
{
    struct page_record *scanning_page;

    scanning_page = &page_records[page_scanning.v];
    if (scanning_page->flags.v & PAGE_FLAGS_ACTIVE) {
        return;
    }
    if (flash_download_session & 128) {
        return;
    }
    if ((mdc_mailbox.error_code.v == ERR_MULTIFEED_ULTRASONIC || err_last_code.s.v == ERR_MULTIFEED_ULTRASONIC || mdc_mailbox.error_code.v == ERR_MULTIFEED_LENGTH || err_last_code.s.v == ERR_MULTIFEED_LENGTH) && button_state.b.b6 && main_multifeed_retry_count < 3) {
        if ((mdc_mailbox.error_code.v == ERR_MULTIFEED_ULTRASONIC || err_last_code.s.v == ERR_MULTIFEED_ULTRASONIC) && testmode_number.v == 0 && (maint_mode_word.v & 0x4000) == 0) {
            _builtin_set_imask(15);
            panel_multifeed_blink_state.v |= 0x8000;
            _builtin_set_imask(0);
        }
        mdc_run_eject();
        main_multifeed_retry_count++;
    }
}
void main_task_turn_rollers(void)
{
    struct page_record *scanning_page;

    scanning_page = &page_records[page_scanning.v];
    if (maint_mode_word.v & 0x4000) {
        return;
    }
    if (mdc_motor_test_flags.f.rollers_turned) {
        if (!button_state.b.b7 && !button_state.b.b6) {
            mdc_motor_test_flags.f.rollers_turned = 0;
        }
        if (mdc_motor_test_state.w) {
            if (82 <= tick_motor_test_ticks.v) {
                mdc_end_motor_test(mdc_motor_test_motor.v, MDC_MOTOR_TEST_SPEED_CLASS);
            }
        }
        panel_current.roller_turn_request = 0;
        return;
    }
    if (panel_current.roller_turn_request == 0 || mdc_motor_test_state.w || (scanning_page->flags.v & PAGE_FLAGS_ACTIVE) != 0) {
        if (mdc_motor_test_state.w) {
            if (82 <= tick_motor_test_ticks.v) {
                mdc_end_motor_test(mdc_motor_test_motor.v, MDC_MOTOR_TEST_SPEED_CLASS);
            }
        }
        return;
    }
    mdc_motor_test_motor.v = MDC_MOTOR_TEST_PICK_AND_FEED;
    mdc_run_motor_test(mdc_motor_test_motor.v, MDC_MOTOR_TEST_SPEED_CLASS);
    panel_current.roller_turn_request = 0;
    tick_motor_test_ticks.v = 0;
    mdc_motor_test_flags.f.rollers_turned = 1;
}

void main_task_multifeed_buttons(void)
{
    int held_long;

    if (!(panel_multifeed_blink_state.v & 128)) {
        return;
    }
    if (button_state.b.b4) {
        _builtin_set_imask(15);
        tick_wait_count = 0;
        _builtin_set_imask(0);
        held_long = 0;
        while (button_state.b.b4) {
            if (tick_wait_count > 1000) {
                held_long = 1;
                break;
            }
        }
        if (held_long && (settings_intelligent_multifeed[0] & 4)) {
            _builtin_set_imask(15);
            panel_multifeed_blink_state.v |= 8;
            panel_patterns_cleared_polls.v = 0;
            _builtin_set_imask(0);
            mdc_run_command(MDC_CMD_CLEAR_OVERLAP_PATTERNS);
            mdc_run_read_overlap_patterns();
            util_mem_clear(iic_eeprom_area2, 0x88);
            iic_eeprom_save_area2_range(0, 0x88);
        } else {
            _builtin_set_imask(15);
            if (panel_multifeed_blink_polls.v == PANEL_MULTIFEED_BLINK_FAST) {
                panel_multifeed_blink_polls.v = PANEL_MULTIFEED_BLINK_SLOW;
            } else {
                panel_multifeed_blink_polls.v = PANEL_MULTIFEED_BLINK_FAST;
            }
            _builtin_set_imask(0);
        }
        _builtin_set_imask(15);
        tick_wait_count = 0;
        _builtin_set_imask(0);
        while (tick_wait_count < 500 || button_state.b.b4) {
        }
    }
    if (panel_multifeed_blink_state.v & 0x100) {
        if (button_state.b.b7 || panel_multifeed_blink_age_polls.v > 3750) {
            _builtin_set_imask(15);
            panel_multifeed_blink_state.v &= 0xfeff;
            _builtin_set_imask(0);
        }
    }
}
void main_task_idle_lamp_off(void)
{
    struct page_record *scanning_page;
    int x;
    int y;

    scanning_page = &page_records[page_scanning.v];
    if (mdc_mailbox.sensor_status.f.adf_cover_open) {
        return;
    }
    if (mdc_mailbox.error_code.v) {
        return;
    }
    if (flash_download_session & 128) {
        return;
    }
    if ((job_long_page_flags & 128) && (job_long_page_flags & 2)) {
        return;
    }
    if ((scanning_page->flags.v & PAGE_FLAGS_ACTIVE) == 0) {
        _builtin_set_imask(0);
        if (tick_idle_seconds.v + 60 >= power_save_seconds.v) {
            if ((mdc_mailbox.lamp_status & MDC_LAMP_STATUS_ALL) != MDC_LAMP_STATUS_ALL) {
                mdc_run_lamps(7);
            }
            return;
        }
        if (tick_lamp_off_ticks > 5000 && mdc_mailbox.lamp_off == 0) {
            win_params.flags.f.adf = 1;
            afe_select_source();
            power_restart_sensor_path(0);
        }
    }
}
void main_task_idle_park(void)
{
    if (main_park_pending == 0 || (unsigned int)tick_idle_seconds.v < 30) {
        return;
    }
    if (tick_page_idle_seconds.v < 30) {
        return;
    }
    main_park_pending = 0;
    if (job_long_page_flags & 128) {
        return;
    }
    if (mdc_mailbox.backing_position == MDC_BACKING_BLACK && !mdc_mailbox.sensor_status.f.adf_cover_open) {
        mdc_run_backing(0);
    }
}
void main_clear_error_display(int show_function_screen)
{
    int x;

    err_display_flags.b.b6 = 0;
    err_display_flags.b.b5 = 0;
    power_mdc_restart_state.f.timeout_error = 0;
    if (show_function_screen) {
        panel_set_display_mode(0, PANEL_MODE_FUNCTION);
    }
}
void main_task_apply_link_settings(void)
{
    struct page_record *scanning_page;
    unsigned char *src;
    unsigned char *dst;
    unsigned short count;
    unsigned short i;
    unsigned short cell;

    scanning_page = &page_records[page_scanning.v];
    if (!iic_busy.b6) {
        return;
    }
    _builtin_set_imask(15);
    iic_eeprom_cell.v = iic_pending_offset;
    src = iic_pending_settings + iic_eeprom_cell.v;
    dst = settings + iic_eeprom_cell.v;
    count = iic_pending_len;
    i = 0;
    while (1) {
        dst[i] = src[i];
        cell = iic_eeprom_cell.v;
        cell++;
        if (cell >= 512) {
            break;
        } else {
            iic_eeprom_cell.v = cell;
        }
        if (++i >= count) {
            break;
        }
    }
    for (count = 1; count < iic_pending_len; count++) {
        *dst++ = *src++;
        if (iic_eeprom_cell.v + 1 >= 512) {
            break;
        }
        iic_eeprom_cell.v++;
    }
    iic_busy.b6 = 0;
    job_apply_settings();
    _builtin_set_imask(15);
    if (!hw_hostif_flags.f.own_hostif) {
        link_reply.c.code = 2;
        link_reply.f.b7 = 1;
        scanning_page->flags.v &= ~PAGE_FLAGS_REPLY_PENDING;
        isr_request_command_layer(2);
    }
    _builtin_set_imask(0);
}
void main_task_save_settings(void)
{
    struct page_record *scanning_page;

    scanning_page = &page_records[page_scanning.v];
    if (!iic_busy.b5) {
        return;
    }
    iic_eeprom_save_settings();
    _builtin_set_imask(15);
    iic_busy.b5 = 0;
    if (!hw_hostif_flags.f.own_hostif) {
        link_reply.c.code = 2;
        link_reply.f.b7 = 1;
        scanning_page->flags.v &= ~PAGE_FLAGS_REPLY_PENDING;
        isr_request_command_layer(2);
    }
    _builtin_set_imask(0);
}
void main_task_save_area2(void)
{
    struct page_record *scanning_page;

    scanning_page = &page_records[page_scanning.v];
    if (!iic_busy.b4) {
        return;
    }
    iic_eeprom_save_area2();
    _builtin_set_imask(15);
    iic_busy.b4 = 0;
    if (!hw_hostif_flags.f.own_hostif) {
        link_reply.c.code = 2;
        link_reply.f.b7 = 1;
        scanning_page->flags.v &= ~PAGE_FLAGS_REPLY_PENDING;
        isr_request_command_layer(2);
    }
    _builtin_set_imask(0);
}
void main_restore_display_mode(void)
{
    unsigned char display_sub;
    unsigned char display_main;

    if ((maint_mode_word.v & 0x4000) == 0) {
        if ((maint_mode_word.v & 0x1000) == 0) {
            display_sub = 0;
            display_main = 2;
        } else {
            display_sub = 0;
            display_main = 8;
        }
    } else {
        display_sub = maint_mode_word.c.mode;
        display_main = 8;
    }
    panel_set_display_mode(display_sub, display_main);
}
extern int _builtin_get_imask(void);

void power_task_power_save(void)
{
    struct page_record *scanning_page;
    int saved_imask;
    unsigned char request;
    scanning_page = &page_records[page_scanning.v];
    if (tick_page_idle_seconds.v < 35) {
        return;
    }
    if (mdc_motor_test_state.w || (line_side_state[0].state_flags.v & 0xfedf) || (line_side_state[1].state_flags.v & 0xfedf) || (maint_mode_word.v & 0x5000)) {
        if (line_side_state[1].state_flags.v != 0x4000 || hw_hostif_flags.f.selected) {
            if (tick_idle_seconds.v + 70 > power_save_seconds.v) {
                saved_imask = _builtin_get_imask();
                _builtin_set_imask(15);
                if (power_save_seconds.v > 70) {
                    tick_idle_seconds.v = power_save_seconds.v - 70;
                }
                _builtin_set_imask(saved_imask);
            }
            return;
        }
    }
    if ((job_long_page_flags & 0x80) && (jpeg_scan_flags.v & JPEG_SCAN_PAUSED) && !power_state.f.saving && mdc_mailbox.sensor_status.f.top) {
        return;
    }
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    request = power_state.f.enter_request | power_state.f.leave_request;
    if (power_state.f.enter_request || tick_idle_seconds.v >= power_save_seconds.v) {
        if (power_state.f.saving) {
            if (power_state.f.leave_request) {
                power_leave_save();
            } else {
                power_state.f.enter_request = 0;
            }
        } else {
            power_enter_save(1);
        }
    } else {
        if (power_state.f.leave_request) {
            power_leave_save();
        }
    }
    _builtin_set_imask(saved_imask);
    if (!hw_hostif_flags.f.own_hostif) {
        if ((scanning_page->flags.v & PAGE_FLAGS_REPLY_PENDING) && request) {
            _builtin_set_imask(15);
            link_reply.c.code = 2;
            scanning_page->flags.v &= ~PAGE_FLAGS_REPLY_PENDING;
            link_reply.f.b7 = 1;
            isr_request_command_layer(2);
            _builtin_set_imask(saved_imask);
        }
    }
    _builtin_set_imask(saved_imask);
}


/* Leaves power saving: board and port bits, host interface and LSI set-up, the MDC shared RAM
   written back from edge_left_front; clears power_state. */
void power_leave_save(void)
{
    int x;
    unsigned char saved_win_flags;

    if (err_mdc_alarm) {
        power_state.f.leave_request = 0;
        iic_panel_set_port0(PANEL_LED_POWER | PANEL_LED_CHECK);
        return;
    }
    _builtin_set_imask(15);
    hw_restart_flags.f.leaving_power_save = 1;
    mdc_fan_alarm.b6 = 0;
    log_add_record(5, 0, 0, 0);
    p_board_ctrl->reg_0a.power_enable_8 = 1;
    p_board_ctrl->reg_0a.power_enable_9 = 1;
    rt_delay_60k();
    p_board_ctrl->reg_06.mdc_enable = 1;
    rt_delay_180k();
    if (hw_hostif_flags.f.own_hostif) {
        p_board_ctrl->reg_02.hostif_idle = 0;
        p_board_ctrl->reg_02.standby = 0;
        p_board_ctrl->reg_04.hostif_idle_enable = 0;
        ((struct usb_regs_dummy *)usb_ctrl)->main_int_enb.b0 = 0;
    }
    _builtin_set_imask(0);
    hw_init_ports();
    if (panel_current.display_main == PANEL_MODE_ERROR)
        iic_panel_set_port0(PANEL_LED_POWER | PANEL_LED_CHECK);
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.selected && hw_hostif_flags.f.usb)
        hw_switch_to_usb();
    p_board_ctrl->reg_0e.ccd_300dpi_mode = power_saved_sensor_bits.b0;
    p_vdcc_front_sensor->source_gate.w.b10 = power_saved_sensor_bits.b2;
    p_vdcc_front_sensor->source_gate.w.ccd_300dpi_mode = power_saved_sensor_bits.b1;
    _builtin_set_imask(0);
    if (hw_hostif_flags.f.own_hostif && (edge_left_front[0] != 0 || power_image_startup_state[0] != 0))
        util_mem_copy_to_odd((void *)&mdc_mailbox.handshake_high, edge_left_front, 0x400);
    mdc_run_power_save(0);
    power_state.f.saving = 0;
    hw_enable_lsi_events();
    panel_set_display_mode(panel_current.display_sub, panel_current.display_main);
    p_intc->IPRA.WORD |= 0xe00;
    _builtin_set_imask(0);
    mdc_wait_pb9_low();
    _builtin_set_imask(15);
    tick_mdc_start_ticks = 0;
    job_calibrated_since_mdc_start = 0;
    _builtin_set_imask(0);
    saved_win_flags = win_params.flags.v;
    win_params.flags.f.adf = 1;
    afe_select_source();
    win_params.flags.v = saved_win_flags;
    power_restart_sensor_path(1);
    _builtin_set_imask(15);
    power_state.v = 0;
    tick_idle_seconds.v = 0;
    hw_restart_flags.f.leaving_power_save = 0;
    mdc_fan_alarm.b7 = 1;
    power_off_idle_seconds = 0;
    power_off_flags.f.idle_restarted = 1;
    _builtin_set_imask(0);
    mdc_check_fan_alarm();
    if (power_woken_by_button) {
        _builtin_set_imask(15);
        power_woken_by_button = 0;
        _builtin_set_imask(0);
        if ((settings_byte_0x7f[0] & 1) && hw_hostif_flags.f.own_hostif) {
            {
                _builtin_set_imask(15);
                tick_wait_count = 0;
                _builtin_set_imask(0);
                while (tick_wait_count < 10000) {
                    if (page_records[0].flags.f.load_pending || page_records[0].sides[0].flags.f.requested || (job_manual_feed_flags.w & 0x8000))
                        break;
                    if (job_aborted && scsi_resume_flags.f.resume_pending)
                        break;
                    if (tick_heater_monitor == 0) {
                        saved_win_flags = win_params.flags.v;
                        win_params.flags.f.adf = 0;
                        afe_select_source();
                        win_params.flags.v = saved_win_flags;
                        break;
                    }
                }
            }
        }
    }
}


struct power_down_reg_08 { char pad_00[8]; volatile unsigned int mstcr; };

/* Enters power saving (power_state bit 5): MDC command 0x30, LSIs and ports off, the MDC shared
   RAM (0x400 odd bytes) saved in edge_left_front for power_leave_save. */
void power_enter_save(int full)
{
    int i;
    int x;
    int saved_imask;

    log_add_record(5, 1, 0, 0);
    _builtin_set_imask(15);
    power_state.f.saving = 1;
    power_state.f.enter_request = 0;
    tick_hopper_was_empty = 0;
    mdc_fan_alarm.b7 = 0;
    tick_pf6_low_reported = 1;
    tick_pf6_count = 0;
    mdc_mailbox.wake_flags &= 0xfe;
    _builtin_set_imask(0);
    hook_power_save_1();
    hook_power_save_2();
    mdc_run_power_save(1);
    if (err_mdc_alarm == 0 && !mdc_fan_alarm.b6)
        iic_panel_set_port0(PANEL_LED_POWER);
    _builtin_set_imask(15);
    hw_port_e->DRL.BIT.B7 = 0;
    hw_port_e->DRL.BIT.B10 = 0;
    hw_port_a->DR.BIT.B0 = 0;
    hw_port_e->DRL.BIT.B2 = 0;
    hw_port_a->DR.BIT.B0 = 1;
    p_vdcc_front_sensor->source_gate.w.flatbed_ccd_off = 1;
    p_vdcc_front_sensor->source_gate.w.adf_ccd_off = 1;
    hw_port_a->DR.BIT.B1 = 1;
    hw_port_e->DRL.BIT.B8 = 1;
    _builtin_set_imask(0);
    if (full == 0) {
        for (i = 0; i < 1200; i++) {
            rt_delay_60k();
            if (!((volatile struct st_pf *)hw_port_f)->DR.BIT.B7) {
                rt_delay_60k();
                if (!((volatile struct st_pf *)hw_port_f)->DR.BIT.B7)
                    break;
            }
        }
        return;
    }
    _builtin_set_imask(15);
    tick_wait_count = 0;
    _builtin_set_imask(0);
    jpeg_sync_buffer_pointer(0, saved_imask = _builtin_get_imask());
    jpeg_sync_buffer_pointer(1, saved_imask);
    while (1) {
        if (tick_wait_count >= 2500)
            break;
    }
    hw_port_e->DRL.BIT.B9 = 0;
    job_adf_sheet_count = 0;
    job_sheet_count = 0;
    util_mem_clear(img_loaded_tables[0].gamma_loaded_mono, 4);
    util_mem_clear(img_loaded_tables[1].gamma_loaded_mono, 4);
    util_mem_clear(&img_loaded_tables[0].halftone_loaded, 8);
    util_mem_clear(&img_loaded_tables[1].halftone_loaded, 8);
    util_mem_clear(img_loaded_tables[0].gamma_loaded_color, 4);
    util_mem_clear(img_loaded_tables[1].gamma_loaded_color, 4);
    jpeg_loaded_state[0].loaded_compression = jpeg_loaded_state[0].compression = 0;
    jpeg_loaded_state[1].loaded_compression = jpeg_loaded_state[1].compression = 0;
    _builtin_set_imask(15);
    job_calib_valid = 0;
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    p_pisces_sdram_ctrl[0]->ctrl.b.b5 = 1;
    p_pisces_sdram_ctrl[1]->ctrl.b.b5 = 1;
    p_pisces_sdram_ctrl[0]->ctrl.b.b3 = 0;
    p_pisces_sdram_ctrl[1]->ctrl.b.b3 = 0;
    rt_delay_24k();
    p_pisces[0]->n.b0 = 1;
    p_pisces[1]->n.b0 = 1;
    _builtin_set_imask(0);
    p_vdcc_front_sensor->source_gate.w.b10 = 0;
    p_vdcc_front_sensor->source_gate.w.ccd_300dpi_mode = 0;
    hw_enable_vdcc(0);
    if (hw_board_variant != 0 && !(settings_byte_0x48[0] & 2)) {
        if (hw_hostif_flags.f.selected) {
            if (hw_hostif_flags.f.usb) {
                _builtin_set_imask(15);
                p_board_ctrl->reg_0c.b11 = 1;
                _builtin_set_imask(0);
                for (i = 0; i <= 20; i++)
                    rt_delay_60k();
                _builtin_set_imask(15);
                p_board_ctrl->reg_08.b11 = 1;
                /* the controller is off now: its register writes go to RAM until the wake-up */
                (*(struct scsi_regs_dummy **)&scsi_ctrl) = p_scsi_ctrl_dummy;
                ((struct scsi_regs_dummy *)scsi_ctrl)->mainint = 0;
                _builtin_set_imask(0);
            } else {
                _builtin_set_imask(15);
                hw_port_b->DR.BIT.B7 = 1;
                _builtin_set_imask(0);
                for (i = 0; i <= 20; i++)
                    rt_delay_60k();
                _builtin_set_imask(15);
                hw_port_e->DRL.BIT.B12 = 1;
                (*(struct usb_regs_dummy **)&usb_ctrl) = p_usb_ctrl_dummy;
                ((struct usb_regs_dummy *)usb_ctrl)->main_int_stat = 0;
                _builtin_set_imask(0);
            }
        } else if (hw_hostif_flags.f.owner_decided && hw_cga_board_present.f.present) {
            _builtin_set_imask(15);
            p_board_ctrl->reg_0c.b11 = 1;
            hw_port_b->DR.BIT.B7 = 1;
            _builtin_set_imask(0);
            for (i = 0; i <= 20; i++)
                rt_delay_60k();
            _builtin_set_imask(15);
            p_board_ctrl->reg_08.b11 = 1;
            (*(struct scsi_regs_dummy **)&scsi_ctrl) = p_scsi_ctrl_dummy;
            ((struct scsi_regs_dummy *)scsi_ctrl)->mainint = 0;
            hw_port_e->DRL.BIT.B12 = 1;
            (*(struct usb_regs_dummy **)&usb_ctrl) = p_usb_ctrl_dummy;
            ((struct usb_regs_dummy *)usb_ctrl)->main_int_stat = 0;
            _builtin_set_imask(0);
        }
    }
    if (hw_hostif_flags.f.own_hostif) {
        _builtin_set_imask(15);
        p_intc->IPRA.WORD &= 15;
        _builtin_set_imask(0);
        _builtin_set_imask(15);
        ((struct usb_regs_dummy *)usb_ctrl)->main_int_enb.b0 = 1;
        _builtin_set_imask(0);
        util_mem_copy_from_odd((int)edge_left_front, (int)&mdc_mailbox.handshake_high, 0x400);
        _builtin_set_imask(15);
        p_board_ctrl->reg_02.hostif_idle = 1;
        p_board_ctrl->reg_02.standby = 1;
        p_board_ctrl->reg_04.hostif_idle_enable = 1;
        p_board_ctrl->reg_0a.power_enable_8 = 0;
        p_board_ctrl->reg_0a.power_enable_9 = 0;
        _builtin_set_imask(0);
    } else {
        util_mem_copy_from_odd((int)edge_left_front, (int)&mdc_mailbox.handshake_high, 0x400);
        _builtin_set_imask(15);
        p_board_ctrl->reg_0a.power_enable_8 = 0;
        _builtin_set_imask(0);
    }
    _builtin_set_imask(15);
    p_board_ctrl->reg_06.mdc_enable = 0;
    power_saved_sensor_bits.b0 = p_board_ctrl->reg_0e.ccd_300dpi_mode;
    power_saved_sensor_bits.b2 = p_vdcc_front_sensor->source_gate.w.b10;
    power_saved_sensor_bits.b1 = p_vdcc_front_sensor->source_gate.w.ccd_300dpi_mode;
    p_board_ctrl->reg_0e.ccd_300dpi_mode = 0;
    if (hw_board_variant != 0 && (settings_byte_0x48[0] & 4))
        ((struct power_down_reg_08 *)p_sbycr)->mstcr = 0xf31dc0fd;
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    tick_wait_count = 0;
    _builtin_set_imask(0);
    while (1) {
        if (tick_wait_count >= 1500)
            break;
    }
}

/* Auto power off (service manual appendix 3): power_off_idle_seconds counts idle seconds; warning bit at
   power_off_warning_seconds, power_off (power off) at power_off_limit_seconds. */
void power_task_auto_off(void)
{
    int switch_enabled;

    switch_enabled = hw_power_switch_enabled();
    if ((settings_emulation[0] & 0x1c) || !(settings_power_off_enable[0] & 0x80) || (settings_power_off_time[0] & 0x80) || switch_enabled == 0 || hw_board_variant == 0
        || (maint_mode_word.v & 0x5000))
        return;
    _builtin_set_imask(15);
    if (power_off_flags.f.idle_restarted == 1) {
        if (power_off_idle_seconds >= power_off_warning_seconds.v && power_off_flags.f.warning_due == 0)
            power_off_flags.f.warning_due = 1;
    }
    _builtin_set_imask(0);
    if (tick_page_idle_seconds.v < 5)
        return;
    if (!power_off_flags.f.enabled)
        return;
    if (power_off_idle_seconds >= power_off_limit_seconds.v) {
        _builtin_set_imask(15);
        power_off_flags.f.requested = 1;
        power_off();
        power_off_idle_seconds -= 60;
        _builtin_set_imask(0);
    }
}

void power_task_mdc_restart(void)
{
    _builtin_set_imask(15);
    tick_wait_count = 0;
    _builtin_set_imask(0);
    while (1) {
        if (tick_wait_count >= 2500)
            break;
    }
    power_mdc_restart_state.f.restart_running = 1;
    if (power_enter_save_checked() == 0) {
        _builtin_set_imask(15);
        power_mdc_timeout_count = 100;
        power_mdc_restart_request = 0;
        power_mdc_restart_state.f.retry_state = 1;
        _builtin_set_imask(0);
        return;
    }
    power_leave_save();
    power_mdc_restart_state.f.restart_running = 0;
    power_mdc_restart_request = 0;
    err_display_flags.b.b6 = 1;
}

int power_enter_save_checked(void)
{
    int x;
    int y;
    int saved_imask;

    _builtin_set_imask(15);
    power_state.f.saving = 1;
    power_state.f.enter_request = 0;
    tick_hopper_was_empty = 0;
    mdc_fan_alarm.b7 = 0;
    _builtin_set_imask(0);
    log_add_record(5, 1, 1, 0);
    hook_power_save_1();
    hook_power_save_2();
    if (mdc_run_power_save_timeout() == 0) {
        _builtin_set_imask(15);
        power_state.f.saving = 0;
        mdc_fan_alarm.b7 = 1;
        _builtin_set_imask(0);
        return 0;
    }
    _builtin_set_imask(15);
    hw_port_e->DRL.BIT.B7 = 0;
    hw_port_e->DRL.BIT.B10 = 0;
    hw_port_a->DR.BIT.B0 = 0;
    hw_port_e->DRL.BIT.B2 = 0;
    hw_port_a->DR.BIT.B0 = 1;
    p_vdcc_front_sensor->source_gate.w.flatbed_ccd_off = 1;
    p_vdcc_front_sensor->source_gate.w.adf_ccd_off = 1;
    hw_port_a->DR.BIT.B1 = 1;
    hw_port_e->DRL.BIT.B8 = 1;
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    tick_wait_count = 0;
    _builtin_set_imask(0);
    jpeg_sync_buffer_pointer(0, saved_imask = _builtin_get_imask());
    jpeg_sync_buffer_pointer(1, saved_imask);
    while (1) {
        if (tick_wait_count >= 2500)
            break;
    }
    hw_port_e->DRL.BIT.B9 = 0;
    job_adf_sheet_count = 0;
    job_sheet_count = 0;
    util_mem_clear(img_loaded_tables[0].gamma_loaded_mono, 4);
    util_mem_clear(img_loaded_tables[1].gamma_loaded_mono, 4);
    util_mem_clear(&img_loaded_tables[0].halftone_loaded, 8);
    util_mem_clear(&img_loaded_tables[1].halftone_loaded, 8);
    util_mem_clear(img_loaded_tables[0].gamma_loaded_color, 4);
    util_mem_clear(img_loaded_tables[1].gamma_loaded_color, 4);
    jpeg_loaded_state[0].loaded_compression = jpeg_loaded_state[0].compression = 0;
    jpeg_loaded_state[1].loaded_compression = jpeg_loaded_state[1].compression = 0;
    _builtin_set_imask(15);
    p_pisces_sdram_ctrl[0]->ctrl.b.b5 = 1;
    p_pisces_sdram_ctrl[1]->ctrl.b.b5 = 1;
    p_pisces_sdram_ctrl[0]->ctrl.b.b3 = 0;
    p_pisces_sdram_ctrl[1]->ctrl.b.b3 = 0;
    rt_delay_24k();
    p_pisces[0]->n.b0 = 1;
    p_pisces[1]->n.b0 = 1;
    _builtin_set_imask(0);
    p_vdcc_front_sensor->source_gate.w.b10 = 0;
    p_vdcc_front_sensor->source_gate.w.ccd_300dpi_mode = 0;
    hw_enable_vdcc(0);
    if (hw_hostif_flags.f.own_hostif) {
        _builtin_set_imask(15);
        p_intc->IPRA.WORD &= 15;
        _builtin_set_imask(0);
        util_mem_copy_from_odd((int)edge_left_front, (int)&mdc_mailbox.handshake_high, 0x400);
        _builtin_set_imask(15);
        p_board_ctrl->reg_02.hostif_idle = 1;
        p_board_ctrl->reg_02.standby = 1;
        p_board_ctrl->reg_04.hostif_idle_enable = 1;
        p_board_ctrl->reg_0a.power_enable_8 = 0;
        p_board_ctrl->reg_0a.power_enable_9 = 0;
        _builtin_set_imask(0);
    } else {
        _builtin_set_imask(15);
        p_board_ctrl->reg_0a.power_enable_8 = 0;
        _builtin_set_imask(0);
    }
    _builtin_set_imask(15);
    p_board_ctrl->reg_06.mdc_enable = 0;
    power_saved_sensor_bits.b0 = p_board_ctrl->reg_0e.ccd_300dpi_mode;
    power_saved_sensor_bits.b2 = p_vdcc_front_sensor->source_gate.w.b10;
    power_saved_sensor_bits.b1 = p_vdcc_front_sensor->source_gate.w.ccd_300dpi_mode;
    p_board_ctrl->reg_0e.ccd_300dpi_mode = 0;
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    tick_wait_count = 0;
    _builtin_set_imask(0);
    while (1) {
        if (tick_wait_count >= 1500)
            break;
    }
    return 1;
}


void power_task_mdc_retry(void)
{
    struct page_record *scanning_page;
    struct page_record *current_page;
    int ok;
    int x;

    scanning_page = &page_records[page_scanning.v];
    current_page = &page_records[page_current.v];
    ok = 1;
    if (power_mdc_restart_state.f.retry_state != 1)
        return;
    if (!power_mdc_restart_state.f.retry_allowed)
        return;
    if ((scanning_page->flags.v & PAGE_FLAGS_ACTIVE) || (current_page->flags.v & PAGE_FLAGS_FED_OR_SCANNED))
        return;
    if (mdc_request_log_state() == 0)
        ok = 0;
    _builtin_set_imask(15);
    tick_wait_count = 0;
    _builtin_set_imask(0);
    while (1) {
        if (tick_wait_count >= 500)
            break;
    }
    if (ok) {
        power_copy_mdc_log(0);
        power_dump_state(3);
        _builtin_set_imask(15);
        power_mdc_restart_state.f.retry_state = 0;
        power_mdc_restart_state.f.retry_allowed = 1;
        _builtin_set_imask(0);
        power_enter_save(1);
        power_leave_save();
        power_mdc_restart_state.f.restart_running = 0;
    } else {
        power_mdc_restart_state.f.restart_running = 0;
        power_mdc_restart_state.f.retry_state = 2;
        err_raise(ERR_MDC_TIMEOUT);
        power_copy_mdc_log(1);
        power_dump_state(3);
        _builtin_set_imask(15);
        power_mdc_restart_state.f.retry_allowed = 0;
        _builtin_set_imask(0);
    }
}

int power_copy_mdc_log(unsigned char after_timeout)
{
    unsigned char *buf;
    unsigned short i;
    unsigned short end;
    unsigned short window_offset;
    unsigned short start;
    unsigned short len;

    start = 0;
    len = 0x200;
    mdc_run_dump_log();
    if (mdc_cmd_posted.c.last_result == MDC_RESULT_TIMEOUT)
        return 0;
    window_offset = start * 2;
    if (after_timeout) {
        buf = power_mdc_dump_1;
        power_mdc_restart_state.f.timeout_log_copied = 1;
    } else {
        buf = power_mdc_dump_0;
        power_mdc_restart_state.f.log_copied = 1;
    }
    util_mem_copy_from_odd((int)buf, (int)&mdc_back_window[0x200 + window_offset] + 1, len);
    _builtin_set_imask(15);
    i = window_offset + 0x200;
    if (i < 0x200)
        i = 0x200;
    end = len * 2 + window_offset + 0x200;
    if (end > 0x600)
        end = 0x600;
    for (; (unsigned)i < (unsigned)end; i++)
        mdc_back_window[i] = 0;
    _builtin_set_imask(0);
    return 1;
}
