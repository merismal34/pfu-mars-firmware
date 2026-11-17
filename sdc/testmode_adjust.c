#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);

union byte_bit7_low7 {
    unsigned char c;
    struct { unsigned char b7:1, v:7; } f;
};
struct vdcc_gpio_regs_4_7 {
    char reg_100_103[4];
    union byte_bit7_low7 reg_104, reg_105;
    union UBF reg_106;
    union byte_bit7_low7 reg_107;
};

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


void testmode_0f(void)
{
    int unused28;
    struct page_record *scanning_page;
    int unused20;
    unsigned long j;
    unsigned short side;
    short pattern;
    unsigned short polls;
    unsigned char saved_1;
    unsigned char saved_2;
    unsigned char bits;
    short *word;

    scanning_page = &page_records[page_scanning.v];
    err_code_list[0] = 0;
    testmode_result_1[0] = 0;
    _builtin_set_imask(15);
    ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_select = 1;
    _builtin_set_imask(0);
    rt_delay_180k();
    rt_delay_180k();
    _builtin_set_imask(15);
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_105.f.b7 = 0;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_back_gpio[0])->reg_105.f.b7 = 0;
    if (p_vdcc_back_sensor->source_gate.f.b3) {
        err_code_list[0] |= 2;
    }
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    saved_1 = ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_107.f.v;
    saved_2 = ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_106.v;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_107.f.v = 2;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_106.v = 0;
    bits = ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_106.v & 18;
    if (bits & 2 || !(bits & 16)) {
        err_code_list[0] |= 64;
    }
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_106.v = 2;
    bits = ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_106.v & 18;
    if (!(bits & 2) || bits & 16) {
        err_code_list[0] |= 64;
    }
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_106.v = saved_2;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_107.f.v = saved_1;
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    saved_1 = ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_105.f.v;
    saved_2 = ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_104.f.v;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_105.f.v &= 0xc0;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_105.f.v |= 1;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_104.f.v = 0;
    bits = ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_104.f.v & 47;
    if (bits & 1 || !(bits & 8)) {
        err_code_list[0] |= 16;
    }
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_104.f.v = 1;
    bits = ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_104.f.v & 47;
    if (!(bits & 1) || bits & 8) {
        err_code_list[0] |= 16;
    }
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_104.f.v = saved_2;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_105.f.v = saved_1;
    _builtin_set_imask(0);
    if (err_code_list[0] == 0) {
        win_params.paper_size.f.custom_size = 1;
        win_params.paper_size.f.auto_length = 0;
        win_params.paper_width_inch1200.v = 0x2de0;
        win_params.paper_length_inch1200.v = 0x26c1;
        win_params.side[0].use_window_area = 1;
        win_params.side[1].use_window_area = 1;
        win_params.side[0].x_offset_inch1200 = 0;
        win_params.side[1].x_offset_inch1200 = 0;
        win_params.side[0].y_offset_inch1200 = 0;
        win_params.side[1].y_offset_inch1200 = 0;
        win_params.side[0].width_inch1200 = 0x800;
        win_params.side[1].width_inch1200 = 0x800;
        win_params.side[0].length_inch1200 = 0x40;
        win_params.side[1].length_inch1200 = 0x40;
        win_params.side[0].composition.f.multi_level = 1;
        win_params.side[0].composition.f.color = 1;
        win_params.side[1].composition.f.multi_level = 1;
        win_params.side[1].composition.f.color = 1;
        win_params.resolution_mode.v &= 0xfff8;
        win_params.resolution_mode.v |= 7;
        win_params.x_res_dpi.v = 600;
        win_params.y_res_dpi.v = 600;
        win_params.flags.f.adf = 1;
        win_sides.v = 2;
        _builtin_set_imask(15);
        if (hw_board_variant == 0) {
            ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.b11 = 1;
        }
        ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_reset = 1;
        if (hw_board_variant == 0) {
            ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.b11 = 0;
        }
        saved_1 = settings_test_flags[0];
        settings_test_flags[0] = 15;
        settings_main_offset_adf_front[0] = 0;
        settings_main_offset_adf_back[0] = 0;
        util_mem_clear(settings_tone_x_0, 18);
        saved_2 = hw_option_flags.v;
        hw_option_flags.f.b1 = 1;
        page_scanning.v = 0;
        scanning_page = &page_records[page_scanning.v];
        util_mem_clear(scanning_page, 0x88);
        job_buffer_mode_on = 0;
        scanning_page->flags.f.adf = 1;
        scanning_page->flags.f.scan_requested = 1;
        scanning_page->sides[0].flags.f.requested = 1;
        scanning_page->sides[1].flags.f.requested = 1;
        ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_106.f.b1 = 1;
        _builtin_set_imask(0);
        main_run_page_loop();
        if (page_records[page_scanning.v].flags.f.scan_end) {
        }
    }
    if (err_code_list[0] == 0) {
        for (side = 0; side < 2; side++) {
            _builtin_set_imask(15);
            p_pisces_ctrl[side]->ctrl.f.sdram_window = 1;
            p_pisces_ctrl[side]->reg_006 = p_pisces_capture[side]->base_64k.v;
            p_pisces_ctrl[side]->sdram_page = p_pisces_capture[side]->base_64k.v;
            _builtin_set_imask(0);
            if (side == 0) {
                word = p_pisces_front_sdram.h;
            } else {
                word = p_pisces_back_sdram.h;
            }
            j = 0;
            pattern = side * 2 + 1;
            while (j < 0x80) {
                *word = pattern;
                j++;
                pattern += 0x515;
                word++;
            }
            _builtin_set_imask(15);
            p_pisces_pass_source[side]->addr_high.v = 0;
            p_pisces_pass_source[side]->addr_low.v = 0;
            p_pisces_pass_source[side]->x_offset.v = 0;
            p_pisces_pass_source[side]->y_offset.v = 0;
            p_pisces_mode[side]->run.v = 0;
            p_pisces_mode[side]->run.f.b14 = 1;
            p_pisces_mode[side]->run.f.b8 = 1;
            p_pisces_mode[side]->run.f.b4 = 1;
            _builtin_set_imask(0);
            polls = 0;
            while (!p_pisces_pass_dest[side]->done.enable) {
                rt_delay_180k();
                if (++polls > 200) {
                }
            }
            p_pisces_ctrl[side]->sdram_page = p_pisces_pass_dest[side]->base_64k.v;
            if (side == 0) {
                word = p_pisces_front_sdram.h;
            } else {
                word = p_pisces_back_sdram.h;
            }
            j = 0;
            pattern = side * 2 + 1;
            while (j < 0x80) {
                if (*word != pattern) break;
                j++;
                pattern += 0x515;
                word++;
            }
        }
    }
    _builtin_set_imask(15);
    scsi_read_sides[0].flags.w = 0;
    scsi_read_sides[1].flags.w = 0;
    scsi_read_progress.v = 0;
    line_side_state[0].state_flags.v = 0;
    line_side_state[1].state_flags.v = 0;
    line_page_state[0].state_flags.v = 0;
    line_page_state[1].state_flags.v = 0;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_106.f.b1 = 0;
    hw_option_flags.v = saved_2;
    settings_test_flags[0] = saved_1;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_front_gpio[0])->reg_105.f.b7 = 1;
    ((struct vdcc_gpio_regs_4_7 *)p_vdcc_back_gpio[0])->reg_105.f.b7 = 1;
    if (hw_board_variant == 0) {
        ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.b11 = 1;
    }
    ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_reset = 1;
    p_pisces_ctrl[0]->ctrl.f.sdram_window = 0;
    p_pisces_ctrl[1]->ctrl.f.sdram_window = 0;
    ((volatile struct board_ctrl_reg_0c *)p_board_ctrl)->reg_0c.cga_select = 0;
    _builtin_set_imask(0);
    return;
}

void testmode_10(void)
{
    if (err_ultrasonic_error) {
        err_code_list[0] = 0xff;
    } else {
        err_code_list[0] = 0;
    }
}

short testmode_adjust_offset(void)
{
    maint_mode_3_state_0();
    maint_mode_3_state_1();
    if (err_code_list[0]) {
        return maint_mode_word.v &= 0x7fff;
    }
    maint_mode_3_state_2();
    maint_mode_3_state_3();
    if (err_code_list[0]) {
        return maint_mode_word.v &= 0x7fff;
    }
    maint_mode_3_set_settings(2);
    iic_eeprom_save_settings();
    maint_mode_word.v &= 0x7fff;
}

void testmode_adjust_magnification(void)
{
    maint_mode_2_state_0();
    maint_mode_2_state_1();
    if (err_code_list[0]) {
        maint_mode_word.v &= 0x7fff;
        return;
    }
    maint_mode_2_state_2();
    maint_mode_2_state_3();
    if (err_code_list[0]) {
        maint_mode_word.v &= 0x7fff;
        return;
    }
    maint_mode_2_set_settings(2);
    iic_eeprom_save_settings();
    maint_mode_word.v &= 0x7fff;
}

void testmode_adjust_white_level(void)
{
    maint_mode_4_state_0();
    maint_mode_4_state_1();
    if (err_code_list[0]) {
        maint_mode_word.v &= 0x7fff;
        return;
    }
    maint_mode_4_state_2();
    maint_mode_4_state_3();
    settings_test_flags[0] = maint_saved_value_6[0];
    if (err_code_list[0]) {
        maint_mode_word.v &= 0x7fff;
        return;
    }
    iic_eeprom_save_settings();
}

void testmode_adjust_ultrasonic(void)
{
    err_check_pending();
    maint_mode_8_state_1();
    if (err_code_list[0] == 1) {
        mdc_settings_8f[0] = maint_saved_value_0[0];
        return;
    }
    settings_ultrasonic_threshold[0] = mdc_settings_8f[0];
    iic_eeprom_save_settings();
}
