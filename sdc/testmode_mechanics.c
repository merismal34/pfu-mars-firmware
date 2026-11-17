#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);


struct board_ctrl_reg_0c {
    char pad_00[12];
    struct {
        unsigned short p:4,
                       b11:1,
                       cga_reset:1,
                       :1,
                       cga_select:1,
                       :8;
    } reg_0c;
};
union byte_bit7_low7 {
    unsigned char c;
    struct { unsigned char b7:1, v:7; } f;
};
struct vdcc_gpio_regs_4_5 { char reg_100_103[4]; union byte_bit7_low7 reg_104, reg_105; };

void testmode_panel_buttons(void)
{
    unsigned char button_bits;

    if (scsi_resume_flags.f.resume_pending)
        isr_request_command_layer(0);
    _builtin_set_imask(15);
    panel_current.display_main = PANEL_MODE_MAINT;
    panel_current.char_count = 11;
    panel_current.chars[0] = PANEL_GLYPH_BLANK;
    panel_current.chars[1] = 0;
    panel_current.chars[2] = 1;
    panel_current.chars[3] = 2;
    panel_current.chars[4] = 3;
    panel_current.chars[5] = 4;
    panel_current.chars[6] = 5;
    panel_current.chars[7] = 6;
    panel_current.chars[8] = 7;
    panel_current.chars[9] = 8;
    panel_current.chars[10] = 9;
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = 50;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
    _builtin_set_imask(0);
    for (;;) {
        _builtin_set_imask(0);
        if (testmode_param_absent == 0) {
            _builtin_set_imask(15);
            testmode_number.v = 0;
            scsi_testmode_result_state.v = 1;
            _builtin_set_imask(0);
            panel_set_display_mode(0, PANEL_MODE_FUNCTION);
            return;
        }
        button_bits = 0;
        if (button_state.b.b4)
            button_bits |= 0x80;
        if (button_state.b.b6)
            button_bits |= 0x40;
        if (button_state.b.b7)
            button_bits |= 0x20;
        if (button_state.b.b3)
            button_bits |= 0x10;
        _builtin_set_imask(15);
        err_code_list[0] = button_bits;
        _builtin_set_imask(0);
    }
}


void testmode_26(void)
{
    unsigned char motor;
    int running;

    if (scsi_resume_flags.f.resume_pending)
        isr_request_command_layer(0);
    if (testmode_param_1.v == 3) {
        motor = MDC_MOTOR_TEST_NONE;
    } else if (testmode_param_1.v == 2) {
        motor = MDC_MOTOR_TEST_PICK_ARM;
    } else if (testmode_param_1.v == 1) {
        motor = MDC_MOTOR_TEST_FEED;
    } else {
        motor = MDC_MOTOR_TEST_PICK;
    }
    running = 0;
    err_check_pending();
    for (;;) {
        _builtin_set_imask(0);
        if (testmode_param_absent == 0) {
            if (motor != MDC_MOTOR_TEST_NONE)
                mdc_end_motor_test(motor, MDC_MOTOR_TEST_SPEED_CLASS);
            _builtin_set_imask(15);
            testmode_number.v = 0;
            scsi_testmode_result_state.v = 1;
            _builtin_set_imask(0);
            return;
        }
        if (motor != MDC_MOTOR_TEST_NONE) {
            if (running == 0) {
                mdc_run_motor_test(motor, MDC_MOTOR_TEST_SPEED_CLASS);
                running = 1;
            }
        } else {
            mdc_run_backing(running);
            tick_wait_count = 0;
            while (tick_wait_count < 150)
                ;
            if (running == 1) {
                running = 0;
            } else {
                running = 1;
            }
        }
    }
}

void testmode_27(void)
{
    unsigned char sensor_bits;
    unsigned char mode;

    maint_sensor_test = 1;
    win_params.flags.f.adf = 1;
    afe_select_source();
    err_check_pending();
    if (testmode_param_1.v == 2) {
        mdc_run_command_flag(MDC_CMD_ULTRASONIC_MONITOR_ON, 1);
    } else {
        mdc_run_command_flag(MDC_CMD_DF_TEST_ON, 1);
    }
    mode = testmode_param_1.v;
    sensor_bits = 0;
    if (mdc_mailbox.sensor_status.f.pick) sensor_bits |= 0x80;
    if (mdc_mailbox.sensor_status.f.top) sensor_bits |= 0x40;
    if (mdc_mailbox.sensor_status_2.f.ultrasonic) sensor_bits |= 0x20;
    if (mdc_mailbox.sensor_status_2.f.df) sensor_bits |= 0x10;
    if (mdc_mailbox.sensor_status.f.backing) sensor_bits |= 8;
    if (mdc_mailbox.sensor_status.f.pick_arm) sensor_bits |= 4;
    if (mdc_mailbox.sensor_status.f.adf_cover_open) sensor_bits |= 2;
    if (mdc_mailbox.sensor_status.f.no_paper) sensor_bits |= 1;
    _builtin_set_imask(15);
    err_code_list[0] = sensor_bits;
    _builtin_set_imask(0);
    sensor_bits = 0;
    if (mdc_mailbox.sensor_status.f.document_cover_open) sensor_bits |= 0x80;
    _builtin_set_imask(15);
    testmode_result_1[0] = sensor_bits;
    _builtin_set_imask(0);
    if (scsi_resume_flags.f.resume_pending)
        isr_request_command_layer(0);
    while (1) {
        _builtin_set_imask(0);
        if (testmode_param_absent == 0) {
            if (mode == 2) {
                mdc_run_command_flag(MDC_CMD_ULTRASONIC_MONITOR_OFF, 0);
            } else {
                mdc_run_command_flag(MDC_CMD_DF_TEST_OFF, 0);
            }
            maint_sensor_test = 0;
            _builtin_set_imask(15);
            testmode_number.v = 0;
            scsi_testmode_result_state.v = 1;
            _builtin_set_imask(0);
            return;
        }
        sensor_bits = 0;
        if (mdc_mailbox.sensor_status.f.pick) sensor_bits |= 0x80;
        if (mdc_mailbox.sensor_status.f.top) sensor_bits |= 0x40;
        if (mdc_mailbox.sensor_status_2.f.ultrasonic) sensor_bits |= 0x20;
        if (mdc_mailbox.sensor_status_2.f.df) sensor_bits |= 0x10;
        if (mdc_mailbox.sensor_status.f.backing) sensor_bits |= 8;
        if (mdc_mailbox.sensor_status.f.pick_arm) sensor_bits |= 4;
        if (mdc_mailbox.sensor_status.f.adf_cover_open) sensor_bits |= 2;
        if (mdc_mailbox.sensor_status.f.no_paper) sensor_bits |= 1;
        _builtin_set_imask(15);
        err_code_list[0] = sensor_bits;
        _builtin_set_imask(0);
        sensor_bits = 0;
        if (mdc_mailbox.sensor_status.f.document_cover_open) sensor_bits |= 0x80;
        _builtin_set_imask(15);
        testmode_result_1[0] = sensor_bits;
        _builtin_set_imask(0);
    }
}

void testmode_28(void)
{
    struct page_record *scanning_page;
    int u;

    scanning_page = &page_records[page_scanning.v];
    if (scsi_resume_flags.f.resume_pending)
        isr_request_command_layer(0);
    testmode_feed_test = 1;
    err_code_list[0] = 0;
    testmode_result_1[0] = 0;
    win_params.paper_size.f.auto_length = 1;
    win_params.side[0].use_window_area = win_params.side[1].use_window_area = 1;
    win_params.side[0].y_offset_inch1200 = win_params.side[1].y_offset_inch1200 = 0;
    win_params.side[0].length_inch1200 = win_params.side[1].length_inch1200 = 0x4d82;
    win_params.side[0].width_inch1200 = win_params.side[1].width_inch1200 = 100;
    win_params.x_res_dpi.v = win_params.y_res_dpi.v = 200;
    if (!mdc_mailbox.sensor_status.f.no_paper && !mdc_mailbox.sensor_status.f.adf_cover_open) {
        win_params.flags.f.adf = 1;
    } else {
        win_params.flags.f.adf = 0;
    }
    job_buffer_mode_on = 0;
    job_page_end_at_pick_sensor = 1;
    err_check_pending();
    while (1) {
        if (testmode_param_absent == 0) {
            testmode_feed_test = 0;
            _builtin_set_imask(15);
            testmode_number.v = 0;
            scsi_testmode_result_state.v = 0;
            _builtin_set_imask(0);
            return;
        }
        if (err_code_list[0] || testmode_result_1[0]) {
            scsi_testmode_result_state.v = 1;
            break;
        }
        util_mem_clear(scanning_page, 0x88);
        if (win_params.flags.f.adf) {
            scanning_page->flags.f.adf = 1;
            win_sides.f.selection = 2;
        } else {
            win_sides.f.selection = 0;
        }
        scanning_page->flags.f.scan_requested = 1;
        scanning_page->sides[0].flags.f.requested = 1;
        main_run_page_loop();
    }
    testmode_feed_test = 0;
}

void testmode_lamp(void)
{
    int lamp_on;
    lamp_on = 1;
    if (scsi_resume_flags.f.resume_pending) {
        isr_request_command_layer(0);
    }
    mdc_lamp_cover_override = 1;
    err_check_pending();
    while (1) {
        _builtin_set_imask(0);
        if (testmode_param_absent == 0) {
            mdc_lamp_cover_override = 0;
            mdc_run_lamps(0);
            _builtin_set_imask(15);
            testmode_number.v = 0;
            scsi_testmode_result_state.v = 1;
            _builtin_set_imask(0);
            return;
        }
        if (lamp_on == 0) {
            mdc_run_lamps(7);
            lamp_on = 1;
        } else {
            mdc_run_lamps(0);
            lamp_on = 0;
        }
        tick_wait_count = 0;
        while (tick_wait_count < 250) {
        }
    }    mdc_lamp_cover_override = 0;
}

void testmode_2a(void)
{
    struct page_record *scanning_page;
    unsigned char saved_1;
    unsigned char saved_2;
    unsigned char saved_sensor;
    unsigned char bits;
    short u2;
    char main_offset_back;
    char main_offset_front;

    err_code_list[0] = 0;
    testmode_result_1[0] = 0;
    _builtin_set_imask(15);
    ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_select = 1;
    _builtin_set_imask(0);
    rt_delay_180k();
    rt_delay_180k();
    if (hw_board_variant == 0) {
        ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.b11 = 1;
    }
    ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_reset = 1;
    ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_reset = 0;
    _builtin_set_imask(15);
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_105.f.b7 = 0;
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_back_gpio[0])->reg_105.f.b7 = 0;
    if (!p_vdcc_back_sensor->source_gate.f.b3) {
        err_code_list[0] |= 2;
    }
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    saved_1 = ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_105.f.v;
    saved_2 = ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_104.f.v;
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_105.f.v &= 0xc0;
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_105.f.v |= 7;
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_104.f.v = 0;
    bits = ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_104.f.v & 63;
    if (bits & 1 || !(bits & 8)) {
        err_code_list[0] |= 16;
    }
    if (bits & 2 || !(bits & 16)) {
        err_code_list[0] |= 32;
    }
    if (bits & 4 || !(bits & 32)) {
        err_code_list[0] |= 8;
    }
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_104.f.v = 1;
    bits = ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_104.f.v & 63;
    if (!(bits & 1) || bits & 8) {
        err_code_list[0] |= 16;
    }
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_104.f.v = 2;
    bits = ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_104.f.v & 63;
    if (!(bits & 2) || bits & 16) {
        err_code_list[0] |= 32;
    }
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_104.f.v = 4;
    bits = ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_104.f.v & 63;
    if (!(bits & 4) || bits & 32) {
        err_code_list[0] |= 8;
    }
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_104.f.v = saved_2;
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_105.f.v = saved_1;
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    saved_sensor = p_vdcc_front_sensor->source_gate.n.n11.l;
    p_vdcc_front_sensor->source_gate.n.n11.l = 15;
    hw_port_e->DRL.BIT.B15 = 0;
    saved_2 = 0;
    for (saved_1 = 0; saved_1 < 8; saved_1++) {
        bits = p_vdcc_back_sensor->source_gate.f.lo;
        switch (bits) {
        case 0:
            saved_2 |= 1;
            break;
        case 1:
            saved_2 |= 2;
            break;
        case 2:
            saved_2 |= 4;
            break;
        case 3:
            saved_2 |= 8;
            break;
        case 4:
            saved_2 |= 16;
            break;
        case 5:
            saved_2 |= 32;
            break;
        case 6:
            saved_2 |= 64;
            break;
        case 7:
            saved_2 |= 128;
            break;
        }
        hw_port_e->DRL.BIT.B15 = 1;
        hw_port_e->DRL.BIT.B15 = 0;
    }
    if (saved_2 != 0xff) {
        err_code_list[0] |= 4;
    }
    p_vdcc_front_sensor->source_gate.wl.lo = saved_sensor;
    _builtin_set_imask(0);
    if (hw_port_e->DRL.BIT.B14) {
        err_code_list[0] |= 1;
    } else if (err_code_list[0] == 0) {
        win_params.paper_size.f.custom_size = 1;
        win_params.paper_size.f.auto_length = 0;
        win_params.paper_width_inch1200.v = 0x2de0;
        win_params.paper_length_inch1200.v = 0x26c1;
        win_params.side[0].use_window_area = 1;
        win_params.side[1].use_window_area = 1;
        win_params.side[0].x_offset_inch1200 = 0;
        win_params.side[1].x_offset_inch1200 = 0xe0;
        win_params.side[0].y_offset_inch1200 = 0;
        win_params.side[1].y_offset_inch1200 = 0;
        win_params.side[0].width_inch1200 = 0x762;
        win_params.side[1].width_inch1200 = 0x762;
        win_params.side[0].length_inch1200 = 0x1275;
        win_params.side[1].length_inch1200 = 0x1275;
        win_params.side[0].composition.f.multi_level = 1;
        win_params.side[0].composition.f.color = 1;
        win_params.side[1].composition.f.multi_level = 1;
        win_params.side[1].composition.f.color = 1;
        win_params.resolution_mode.v &= 0xfff8;
        win_params.resolution_mode.v |= 5;
        win_params.x_res_dpi.v = 0x96;
        win_params.y_res_dpi.v = 0x96;
        win_params.flags.f.adf = 1;
        win_sides.v = 2;
        _builtin_set_imask(15);
        saved_1 = settings_test_flags[0];
        settings_test_flags[0] = 15;
        main_offset_front = settings_main_offset_adf_front[0];
        main_offset_back = settings_main_offset_adf_back[0];
        settings_main_offset_adf_front[0] = 0;
        settings_main_offset_adf_back[0] = 0;
        saved_2 = hw_hostif_flags.v;
        hw_hostif_flags.v = 0;
        hw_hostif_flags.f.cga_test_scan = 1;
        img_link_scan_mode = 2;
        saved_sensor = p_vdcc_front_sensor->source_gate.n.n11.l;
        p_vdcc_front_sensor->source_gate.n.n11.l = 15;
        hw_port_e->DRL.BIT.B15 = 1;
        page_scanning.v = 0;
        scanning_page = &page_records[page_scanning.v];
        util_mem_clear(scanning_page, 0x88);
        job_buffer_mode_on = 0;
        scanning_page->flags.f.adf = 1;
        scanning_page->flags.f.scan_requested = 1;
        scanning_page->sides[0].flags.f.requested = 1;
        scanning_page->sides[1].flags.f.requested = 1;
        _builtin_set_imask(0);
        main_run_page_loop();
        _builtin_set_imask(15);
        tick_wait_count = 0;
        _builtin_set_imask(0);
        while (1) {
            if (p_vdcc_capture[0]->capture_ctrl.capture_run == 0 && p_vdcc_capture[1]->capture_ctrl.capture_run == 0) {
                break;
            }
            if (tick_wait_count > 2500) {
                _builtin_set_imask(15);
                p_vdcc_capture[0]->capture_ctrl.capture_run = 0;
                p_vdcc_capture[1]->capture_ctrl.capture_run = 0;
                _builtin_set_imask(0);
                break;
            }
        }
        if (hw_port_e->DRL.BIT.B4) {
            err_code_list[0] |= 64;
        }
        _builtin_set_imask(15);
        hw_hostif_flags.f.cga_test_scan = 0;
        img_link_scan_mode = 3;
        page_scanning.v = 0;
        scanning_page = &page_records[page_scanning.v];
        util_mem_clear(scanning_page, 0x88);
        job_buffer_mode_on = 0;
        scanning_page->flags.f.adf = 1;
        scanning_page->flags.f.scan_requested = 1;
        scanning_page->sides[0].flags.f.requested = 1;
        scanning_page->sides[1].flags.f.requested = 1;
        _builtin_set_imask(0);
        main_run_page_loop();
        _builtin_set_imask(15);
        tick_wait_count = 0;
        _builtin_set_imask(0);
        while (1) {
            if (p_vdcc_capture[0]->capture_ctrl.capture_run == 0 && p_vdcc_capture[1]->capture_ctrl.capture_run == 0) {
                break;
            }
            if (tick_wait_count > 2500) {
                _builtin_set_imask(15);
                p_vdcc_capture[0]->capture_ctrl.capture_run = 0;
                p_vdcc_capture[1]->capture_ctrl.capture_run = 0;
                _builtin_set_imask(0);
                break;
            }
        }
        if (!hw_port_e->DRL.BIT.B14) {
            err_code_list[0] |= 129;
        }
        _builtin_set_imask(15);
        hw_port_e->DRL.BIT.B15 = 0;
        p_vdcc_front_sensor->source_gate.wl.lo = saved_sensor;
        hw_hostif_flags.v = saved_2;
        settings_test_flags[0] = saved_1;
        settings_main_offset_adf_front[0] = main_offset_front;
        settings_main_offset_adf_back[0] = main_offset_back;
        _builtin_set_imask(0);
    }
    _builtin_set_imask(15);
    scsi_read_sides[0].flags.w = 0;
    scsi_read_sides[1].flags.w = 0;
    scsi_read_progress.v = 0;
    line_side_state[0].state_flags.v = 0;
    line_side_state[1].state_flags.v = 0;
    line_page_state[0].state_flags.v = 0;
    line_page_state[1].state_flags.v = 0;
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_front_gpio[0])->reg_105.f.b7 = 1;
    ((struct vdcc_gpio_regs_4_5 *)p_vdcc_back_gpio[0])->reg_105.f.b7 = 1;
    if (hw_board_variant == 0) {
        ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.b11 = 1;
    }
    ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_reset = 1;
    ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_select = 0;
    _builtin_set_imask(0);
    return;
}
