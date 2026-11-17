#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
struct vdcc_timing_regs_power {
    unsigned short line_period;
    char reg_072_077[6];
    struct { unsigned short p:2, v:14; } reg_078, pixels_per_line;
    char reg_07c;
    struct { unsigned char p:6, lo:2; } reg_07d;
    unsigned short reg_07e, reg_080;
    char reg_082_083[2];
    unsigned short reg_084;
};

struct vdcc_monitor_reg_00 { unsigned char hi:4, lo:4; };
struct vdcc_monitor_reg_01 { unsigned char p:2, mode:2, lo:4; };
struct vdcc_monitor_view {
    struct vdcc_monitor_reg_00 reg_060;
    struct vdcc_monitor_reg_01 reg_061;
    unsigned short level_left, level_middle, level_right, position_left, position_middle, position_right;
};

void power_restart_sensor_path(int skip_unit_power)
{
    int i;
    union { unsigned short w[2]; unsigned char b[4]; } range;

    if (mdc_mailbox.lamp_off != 0) {
        if (mdc_mailbox.sensor_status.f.adf_cover_open) {
            mdc_request_heater_off(3);
            return;
        }
        mdc_run_lamps(7);
        _builtin_set_imask(15);
        tick_wait_count = 0;
        _builtin_set_imask(0);
        while (tick_lamp_on_ticks.v < 250) {
            if (tick_wait_count > 1000) {
                return;
            }
        }
    }
    _builtin_set_imask(15);
    tick_lamp_off_ticks = 0;
    _builtin_set_imask(0);
    if (skip_unit_power == 0) {
        mdc_run_unit_power(0x80);
    }
    range.w[1] = 0xe71;
    range.w[0] = range.w[1] + 0x240;
    for (i = 0; i <= 1; i++) {
        _builtin_set_imask(15);
        ((struct vdcc_timing_regs_power *)p_vdcc_timing[i])->line_period = 0x1382;
        ((struct vdcc_timing_regs_power *)p_vdcc_timing[i])->reg_078.v = 4;
        ((struct vdcc_timing_regs_power *)p_vdcc_timing[i])->pixels_per_line.v = 0x1f2c;
        ((struct vdcc_timing_regs_power *)p_vdcc_timing[i])->reg_07d.lo = 0;
        ((struct vdcc_timing_regs_power *)p_vdcc_timing[i])->reg_07e = 0;
        ((struct vdcc_timing_regs_power *)p_vdcc_timing[i])->reg_080 = 0;
        ((struct vdcc_timing_regs_power *)p_vdcc_timing[i])->reg_084 = 0xca;
        p_vdcc_measure[i]->x_start.hi = (unsigned char)(range.w[1] >> 8);
        p_vdcc_measure[i]->x_start.lo = range.b[3];
        p_vdcc_measure[i]->x_end.hi = (unsigned char)(range.w[0] >> 8);
        p_vdcc_measure[i]->x_end.lo = range.b[1];
        p_vdcc_measure[i]->select.f.mode = 3;
        p_vdcc_measure[i]->reg_052.lo = 9;
        p_vdcc_measure[i]->select.s.sel = 1;
        _builtin_set_imask(0);
    }
    _builtin_set_imask(15);
    p_board_ctrl->reg_0e.ccd_300dpi_mode = 0;
    p_vdcc_front_sensor->source_gate.b.b2 = 0;
    p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
    _builtin_set_imask(0);
    testmode_wait_lines(3);
    for (i = 0; i <= 1; i++) {
        _builtin_set_imask(15);
        afe_write_config(i);
        afe_set_gain(i, 0, 0, 0x80);
        afe_set_gain(i, 0, 1, 0x80);
        afe_set_gain(i, 1, 0, 0x80);
        afe_set_gain(i, 1, 1, 0x80);
        afe_set_gain(i, 2, 0, 0x80);
        afe_set_gain(i, 2, 1, 0x80);
        _builtin_set_imask(0);
    }
    _builtin_set_imask(15);
    tick_heater_rounds = 0;
    tick_heater_monitor_ticks = 0;
    util_mem_clear(tick_heater_level_history_front, 10);
    util_mem_clear(tick_heater_level_history_back, 10);
    tick_heater_lamp_temperature[0] = mdc_mailbox.lamp_temperature_front;
    tick_heater_lamp_temperature[1] = mdc_mailbox.lamp_temperature_back;
    for (i = 0; i <= 1; i++) {
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[i])->reg_061.mode = 2;
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[i])->reg_061.lo = 8;
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[i])->reg_060.lo = 8;
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[i])->reg_060.hi = 8;
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[i])->position_left = 0x5dc;
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[i])->position_middle = 0xd40;
        ((struct vdcc_monitor_view *)p_vdcc_front_monitor[i])->position_right = 0x15d0;
    }
    tick_heater_monitor = 0x8080;
    _builtin_set_imask(0);
}

extern void _builtin_set_gbr(void *);
extern int _builtin_get_imask(void);
struct usb_regs_power {
    char reg_00_07[17];
    char main_int_enb;
    char pad_12;
    char sie_int_enb;
    char pad_14;
    char reg_0a;
    char reg_0b_0f[11];
    struct Bits chip_reset;
    char reg_11_1e[29];
    char usb_bank;
};

struct scsi_regs_power {
    char pad_00[15];
    char reset;
    char pad_10[3];
    struct Bits scsimode0;
};


/* Power off (auto power off, power button): saves the changed settings image, MDC command 0x62, stops the
   hardware; on board variant 1 (hw_board_variant) holds PB8 low until the power goes, else restarts. */
void power_off(void)
{
    int i;
    int changed_bytes;
    int saved_imask;
    int unused8;
    struct page_record *scanning_page;
    struct page_record *current_page;

    scanning_page = (struct page_record *)page_records + page_scanning.v;
    current_page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    if (maint_save_eeprom_mode.b7) {
        return;
    }
    if (power_off_running) {
        return;
    }
    _builtin_set_imask(15);
    power_off_running = 1;
    _builtin_set_imask(saved_imask);
    if (testmode_number.v == 3 || testmode_number.v == 37) {
        _builtin_set_imask(15);
        power_off_running = 0;
        _builtin_set_imask(saved_imask);
        return;
    }
    if (!power_state.f.saving) {
        jpeg_sync_buffer_pointer(0, saved_imask);
        jpeg_sync_buffer_pointer(1, saved_imask);
    }
    _builtin_set_imask(15);
    if (!iic_busy.b7) {
        for (i = 0, changed_bytes = 0; i < 0x200; i++) {
            if (settings_saved[i] != settings[i]) {
                changed_bytes++;
            }
        }
        if (testmode_number.v != 0 || (maint_mode_word.v & 0x4000) != 0) {
            if (settings_test_flags[0] != settings_saved_test_flags[0]) {
                changed_bytes = 1000;
            }
        }
        settings_test_flags[0] = 0;
        if (settings_byte_0x58[0]) {
            changed_bytes = 1000;
        }
        if (changed_bytes < 60) {
            iic_eeprom_save_settings();
            rt_delay_180k();
        }
    }
    if ((scanning_page->flags.v & PAGE_FLAGS_ACTIVE) || (current_page->flags.v & PAGE_FLAGS_FED_OR_SCANNED)) {
        if (!(maint_mode_word.v & 0x5000) && main_testmode_started == 0) {
            power_dump_state(2);
        }
    } else {
        if (!power_state.f.saving && mdc_mailbox.backing_position == MDC_BACKING_BLACK && mdc_mailbox.cmd_state == MDC_CMD_STATE_IDLE && !mdc_mailbox.sensor_status.f.adf_cover_open) {
            mdc_mailbox.cmd_code = MDC_CMD_BACKING_WHITE;  /* MDC command 0x62: backing to its position, posted directly */
            mdc_mailbox.cmd_state = MDC_CMD_STATE_POSTED;
            for (i = 0; i < 40; i++) {
                rt_delay_180k();
                if (mdc_mailbox.cmd_state == MDC_CMD_STATE_DONE) {
                    break;
                }
            }
        }
    }
    p_board_ctrl->reg_0a.power_enable_8 = 1;
    p_board_ctrl->reg_0a.power_enable_9 = 1;
    rt_delay_60k();
    p_board_ctrl->reg_06.mdc_enable = 1;
    rt_delay_180k();
    p_board_ctrl->reg_02.hostif_idle = 0;
    p_board_ctrl->reg_02.standby = 0;
    p_board_ctrl->reg_04.hostif_idle_enable = 0;
    hw_hostif_flags.v = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_power *)usb_ctrl)->chip_reset.b0 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_power *)usb_ctrl)->main_int_enb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_power *)usb_ctrl)->sie_int_enb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_power *)usb_ctrl)->reg_0a = 0;
    _builtin_set_gbr(0);
    ((struct scsi_regs_power *)scsi_ctrl)->reset = 7;
    _builtin_set_gbr(0);
    ((struct scsi_regs_power *)scsi_ctrl)->scsimode0.b7 = 0;
    _builtin_set_gbr(0);
    ((struct scsi_regs_power *)scsi_ctrl)->scsimode0.b6 = 1;
    if (hw_board_variant == 0) {
        p_board_ctrl->reg_0c.b11 = 1;
    }
    p_board_ctrl->reg_0c.cga_reset = 1;
    p_board_ctrl->reg_0c.cga_select = 0;
    p_board_ctrl->reg_08.b10 = 1;
    if (hw_board_variant == 0) {
        p_board_ctrl->reg_08.b11 = 1;
    }
    p_board_ctrl->reg_0e.ccd_300dpi_mode = 0;
    p_vdcc_front_sensor->source_gate.b.b2 = 0;
    p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
    hw_port_e->DRL.BIT.B3 = 1;
    hw_port_e->DRL.BIT.B7 = 0;
    hw_port_e->DRL.BIT.B10 = 0;
    hw_port_a->DR.BIT.B0 = 0;
    hw_port_e->DRL.BIT.B2 = 0;
    hw_port_a->DR.BIT.B0 = 1;
    p_vdcc_front_sensor->source_gate.b.flatbed_ccd_off = 1;
    p_vdcc_front_sensor->source_gate.b.adf_ccd_off = 1;
    hw_port_a->DR.BIT.B1 = 1;
    hw_port_e->DRL.BIT.B8 = 1;
    if (hw_board_variant == 0) {
        job_adf_sheet_count = 0;
        util_mem_clear(img_loaded_tables[0].gamma_loaded_mono, 4);
        util_mem_clear(img_loaded_tables[1].gamma_loaded_mono, 4);
        util_mem_clear(&img_loaded_tables[0].halftone_loaded, 8);
        util_mem_clear(&img_loaded_tables[1].halftone_loaded, 8);
        util_mem_clear(img_loaded_tables[0].gamma_loaded_color, 4);
        util_mem_clear(img_loaded_tables[1].gamma_loaded_color, 4);
        jpeg_loaded_state[0].loaded_compression = jpeg_loaded_state[0].compression = 0;
        jpeg_loaded_state[1].loaded_compression = jpeg_loaded_state[1].compression = 0;
        p_board_ctrl->reg_06.b8 = 0;
        for (i = 0; i < 8; i++) {
            rt_delay_60k();
        }
        p_board_ctrl->reg_06.b8 = 1;
    }
    p_pisces_sdram_ctrl[0]->ctrl.b.b5 = 1;
    p_pisces_sdram_ctrl[1]->ctrl.b.b5 = 1;
    p_pisces_sdram_ctrl[0]->ctrl.b.b3 = 0;
    p_pisces_sdram_ctrl[1]->ctrl.b.b3 = 0;
    rt_delay_24k();
    p_pisces[0]->n.b0 = 1;
    p_pisces[1]->n.b0 = 1;
    hw_enable_vdcc(0);
    if (hw_hostif_flags.f.own_hostif) {
        p_intc->IPRA.WORD &= 0xf;
        util_mem_copy_from_odd((int)edge_left_front, (int)&mdc_mailbox.handshake_high, 0x400);
        p_board_ctrl->reg_02.hostif_idle = 1;
        p_board_ctrl->reg_02.standby = 1;
        p_board_ctrl->reg_04.hostif_idle_enable = 1;
    }
    p_mtu1->TIER.BIT.TGIEA = 0;
    p_mtu2->TIER.BIT.TGIEA = 0;
    p_mtu3->TIER.BIT.TGIEA = 0;
    p_mtu4->TIER.BIT.TGIEA = 0;
    p_intc->IPRA.WORD = 0;
    p_intc->IPRG.WORD = 0;
    p_intc->IPRD.WORD = 0xa0;
    p_intc->IPRE.WORD = 0;
    p_board_ctrl->reg_02.hostif_idle = 1;
    p_board_ctrl->reg_02.standby = 1;
    p_board_ctrl->reg_04.hostif_idle_enable = 1;
    p_board_ctrl->reg_06.mdc_enable = 0;
    p_board_ctrl->reg_0a.power_enable_8 = 0;
    p_board_ctrl->reg_0a.power_enable_9 = 0;
    hw_port_e->DRL.BIT.B9 = 0;
    for (i = 0; i < 26; i++) {
        rt_delay_180k();
    }
    panel_expander_clear_port1();
    if (hw_board_variant) {
        for (;;) {
            hw_port_a->DR.BIT.B8 = 0;
        }
    } else {
        start_reset();
    }
}

void power_check_button(void)
{
    int switch_enabled;

    switch_enabled = 0;
    switch_enabled = hw_power_switch_enabled();
    if (switch_enabled && button_state.b.b3 && button_power_latch.v) {
        power_off();
    }
}

/* Hidden sequence: five level changes of PF1, each 1-3 s after the last, with the buttons of
   button_state bits 4/6/7 held; then state dump (power_dump_state(0)) and power off. */
void power_check_pf1_sequence(void)
{
    unsigned short i;

    if (!button_state.b.b4 || !button_state.b.b6 || !button_state.b.b7) {
        if (power_pf1_sequence_step && ((unsigned int)power_pf1_sequence_seconds) > 10) {
            power_pf1_sequence_step = 0;
        }
        return;
    }
    if (power_pf1_sequence_step == 0 && hw_port_f->b1) {
        power_pf1_sequence_step = 1;
        power_pf1_sequence_seconds = 0;
    }
    if (power_pf1_sequence_step == 0) {
        return;
    }
    if (power_pf1_sequence_step == 1 && !hw_port_f->b1) {
        if (((unsigned int)power_pf1_sequence_seconds) > 1) {
            power_pf1_sequence_step = 2;
            power_pf1_sequence_seconds = 0;
            if (!power_state.f.saving && !power_state.f.enter_request) {
                iic_panel_set_digit(panel_glyph_2_pattern[0]);
            }
        }
    }
    if (power_pf1_sequence_step == 2 && hw_port_f->b1) {
        if (((unsigned int)power_pf1_sequence_seconds) > 1) {
            power_pf1_sequence_step = 3;
            power_pf1_sequence_seconds = 0;
            if (!power_state.f.saving && !power_state.f.enter_request) {
                iic_panel_set_digit(panel_glyph_3_pattern[0]);
            }
        }
    }
    if (power_pf1_sequence_step == 3 && !hw_port_f->b1) {
        if (((unsigned int)power_pf1_sequence_seconds) > 1) {
            power_pf1_sequence_step = 4;
            power_pf1_sequence_seconds = 0;
            if (!power_state.f.saving && !power_state.f.enter_request) {
                iic_panel_set_digit(panel_glyph_4_pattern[0]);
            }
        }
    }
    if (power_pf1_sequence_step == 4 && hw_port_f->b1) {
        if (((unsigned int)power_pf1_sequence_seconds) > 1) {
            power_pf1_sequence_step = 5;
            power_pf1_sequence_seconds = 0;
            if (!power_state.f.saving && !power_state.f.enter_request) {
                iic_panel_set_digit(panel_glyph_5_pattern[0]);
            }
        }
    }
    if (((unsigned int)power_pf1_sequence_seconds) > 3) {
        power_pf1_sequence_step = 0;
        if (!power_state.f.saving && !power_state.f.enter_request) {
            iic_panel_set_digit(panel_glyph_1_pattern[0]);
        }
        return;
    }
    if (power_pf1_sequence_step < 5) {
        return;
    }
    if (power_pf1_sequence_step == 5) {
        while (1) {
            if (!hw_port_f->b1) {
                break;
            }
            rt_delay_60k();
        }
        power_pf1_sequence_step = 0;
        iic_panel_set_digit(panel_glyph_6_pattern[0]);
        power_dump_state(0);
        iic_panel_set_digit(panel_glyph_small_o_pattern[0]);
        for (i = 0; i < 200; i++) {
            rt_delay_60k();
        }
        power_dump_inhibit = 0x80;
        power_off();
    }
}


/* State dump into the memory block (section BM; once, while power_dump_inhibit is 0); reason: 0 PF1 sequence, 2 power off during a page, 3 MDC restart. Written to flash 0x878000 when the boot header's dump option is > 1. */
void power_dump_state(unsigned char reason)
{
    int count;
    char *dst;
    char *src;
    int saved_imask;
    unsigned char *dump_option;

    saved_imask = _builtin_get_imask();
    if (power_dump_inhibit == 0) {
        _builtin_set_imask(15);
        p_board_ctrl->reg_0a.power_enable_8 = 1;
        p_board_ctrl->reg_0a.power_enable_9 = 1;
        rt_delay_60k();
        p_board_ctrl->reg_06.mdc_enable = 1;
        rt_delay_180k();
        p_board_ctrl->reg_02.hostif_idle = 0;
        p_board_ctrl->reg_02.standby = 0;
        p_board_ctrl->reg_04.hostif_idle_enable = 0;
        power_dump_buf[0] = log_clock_set;
        power_dump_year[0] = log_clock_year;
        power_dump_month[0] = log_clock_month.v;
        power_dump_day[0] = log_clock_day.v;
        power_dump_hour = log_clock_hour.v;
        power_dump_minute = log_clock_min.v;
        power_dump_second = log_clock_sec.v;
        power_dump_reason = reason;
        util_mem_copy(power_dump_revision, scsi_revision, 4);
        power_dump_mdc_version_0[0] = mdc_version[0];
        power_dump_mdc_version_1[0] = mdc_version_1[0];
        power_dump_mdc_version_2[0] = mdc_version_2[0];
        power_dump_mdc_version_3[0] = mdc_version_3[0];
        util_mem_copy(power_dump_line_side_state, line_side_state, 44);
        power_dump_line_motion_flags = line_motion_flags;
        power_dump_page_buf_count = page_buf_count.v;
        power_dump_page_scanning = page_scanning.v;
        power_dump_page_current = page_current.v;
        power_dump_page_scanned_count = page_scanned_count.v;
        power_dump_page_sent_count = page_sent_count.v;
        util_mem_copy(power_dump_page_scanning_record, &page_records[page_scanning.v], 0x88);
        util_mem_copy(power_dump_page_current_record, &page_records[page_current.v], 0x88);
        util_mem_copy(power_dump_last_page_size, scsi_last_page_size, 36);
        util_mem_copy(power_dump_page_side_front, page_side_state, 48);
        util_mem_copy(power_dump_page_side_back, &page_side_state[1], 48);
        util_mem_copy(power_dump_capture_record_front, &page_side_records[page_side_state[0].capture_buf], 0x88);
        util_mem_copy(power_dump_capture_record_back, &page_side_records[page_side_state[1].capture_buf], 0x88);
        util_mem_copy(power_dump_mdc_cmd_log, mdc_cmd_log, 96);
        util_mem_copy(power_dump_mdc_cmd_results, mdc_cmd_result_log, 96);
        power_dump_mdc_cmd_log_index = mdc_cmd_log_index;
        util_mem_copy(power_dump_log_events, log_events, 0x200);
        util_mem_copy(power_dump_settings, settings, 0x200);
        util_mem_copy(power_dump_mailbox, &mdc_mailbox, 0x800);
        util_mem_copy(power_dump_pisces_front, (void *)p_pisces[0], 0x200);
        util_mem_copy(power_dump_pisces_back, (void *)p_pisces[1], 0x200);
        util_mem_copy(power_dump_page_size_wanted, &page_size_wanted, 32);
        power_dump_scsi_status = scsi_status;
        power_dump_scsi_end_state = scsi_end_state;
        power_dump_xfer_state_front = xfer_state_front;
        power_dump_xfer_state_back = xfer_state_back;
        power_dump_scsi_param_len = scsi_param_len.v;
        power_dump_scsi_xfer_len = scsi_xfer_len.v;
        power_dump_sense_information = scsi_sense_information.v;
        power_dump_usb_bulk_len = usb_bulk_len;
        if (reason != 3) {
            power_dump_usb_ep0_stage = usb_ep0_stage;
            power_dump_usb_bulk_state = usb_bulk_state;
            power_dump_usb_bulk_out_state = usb_bulk_out_state;
            power_dump_usb_unused_state_1 = usb_unused_state_1;
            power_dump_usb_transfer_phase = usb_transfer_phase;
            power_dump_usb_unused_state_2 = usb_unused_state_2;
            power_dump_usb_ep0_event = usb_ep0_event;
            power_dump_usb_epa_event = usb_epa_event;
            power_dump_usb_epb_event = usb_epb_event;
            power_dump_usb_ep0_max_packet = (unsigned char)usb_ep0_max_packet;
            power_dump_usb_epa_max_packet = usb_epa_max_packet.v;
            power_dump_usb_epb_max_packet = usb_epb_max_packet;
            p_pisces_hostif[1]->ctrl.f.b0 = 1;
            p_pisces_hostif[1]->ctrl.f.b9 = 0;
            p_pisces_hostif[0]->ctrl.f.b0 = 1;
            p_pisces_hostif[0]->ctrl.f.b9 = 0;
            p_pisces_ctrl[0]->ctrl.f.hostif_stop = 1;
            ((struct usb_regs_power *)usb_ctrl)->usb_bank = 0;
            for (count = 0x82, dst = power_dump_usb_bank_0, src = (char *)((struct usb_regs_power *)usb_ctrl); count != 0; count--) {
                *dst++ = *src++;
            }
            ((struct usb_regs_power *)usb_ctrl)->usb_bank = 1;
            for (count = 0x82, dst = power_dump_usb_bank_1, src = (char *)((struct usb_regs_power *)usb_ctrl); count != 0; count--) {
                *dst++ = *src++;
            }
            ((struct usb_regs_power *)usb_ctrl)->usb_bank = 2;
            for (count = 0x82, dst = power_dump_usb_bank_2, src = (char *)((struct usb_regs_power *)usb_ctrl); count != 0; count--) {
                *dst++ = *src++;
            }
        }
        power_dump_mdc_restart_state = power_mdc_restart_state.v;
        power_dump_marker = 0xaaaaaaaa;
        dump_option = (unsigned char *)0x40f;   /* the boot header's block version, read as the dump option (section HEADER + 0xf of the other link): a number that must follow the link */
        if (*dump_option > 1) {
            flash_save_dump();
            for (count = 0; count < 200; count++) {
                rt_delay_60k();
            }
        }
        _builtin_set_imask(saved_imask);
    }
}
