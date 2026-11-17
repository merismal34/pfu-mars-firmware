/* The line-interrupt and calibration file: lo code (section P) and hi code (`#pragma section H`,
   section PH) in five parts:
   H1 calib_load_white_long_page .. calib_hook (PH)   L1 line_handle_irq .. calib_mode_13_step_2 (P)   H2 calib_record_mark_sample (PH)
   L2 calib_wait_done .. line_run_long_page_step (P)    H3 line_end_page (PH) */
#include "fw.h"

/* ---- H1 (PH): the calibration start routines (calib_start_*). */
#pragma section H
/* Constants (section C). */
const unsigned char calib_white_factor_pairs[129][2] = {
    {0xcb, 0xa9}, {0xe0, 0x98}, {0xff, 0x82}, {0xd8, 0xa0},
    {0xd3, 0xa5}, {0xff, 0x84}, {0xc0, 0xb8}, {0xf0, 0x90},
    {0xff, 0x86}, {0xf6, 0x8d}, {0xdd, 0xa1}, {0xc0, 0xbc},
    {0xe0, 0xa0}, {0xf7, 0x8f}, {0xe3, 0x9f}, {0xdf, 0xa3},
    {0xc0, 0xc0}, {0xe2, 0xa2}, {0xcc, 0xb6}, {0xea, 0x9d},
    {0xd2, 0xb2}, {0xc4, 0xc0}, {0xcb, 0xba}, {0xe0, 0xa8},
    {0xf5, 0x98}, {0xfa, 0x95}, {0xef, 0x9e}, {0xd9, 0xb1},
    {0xf6, 0x9a}, {0xe4, 0xa9}, {0xf0, 0xa0}, {0xcc, 0xc0},
    {0xe9, 0xa7}, {0xde, 0xb1}, {0xe0, 0xb0}, {0xe2, 0xaf},
    {0xd0, 0xc0}, {0xd4, 0xbd}, {0xed, 0xa8}, {0xf8, 0xa0},
    {0xf5, 0xa3}, {0xd4, 0xc0}, {0xfc, 0x9f}, {0xdb, 0xbb},
    {0xcb, 0xcb}, {0xe0, 0xb8}, {0xf9, 0xa4}, {0xf1, 0xab},
    {0xed, 0xaf}, {0xf4, 0xaa}, {0xd8, 0xc3}, {0xdc, 0xc0},
    {0xdf, 0xbe}, {0xf0, 0xb0}, {0xd9, 0xc5}, {0xdd, 0xc2},
    {0xe0, 0xc0}, {0xd0, 0xd0}, {0xed, 0xb6}, {0xef, 0xb5},
    {0xe1, 0xc2}, {0xe4, 0xc0}, {0xda, 0xca}, {0xe9, 0xbd},
    {0xf8, 0xb1}, {0xf4, 0xb5}, {0xe8, 0xc0}, {0xe0, 0xc8},
    {0xe3, 0xc6}, {0xef, 0xbc}, {0xdd, 0xcd}, {0xec, 0xc0},
    {0xe5, 0xc7}, {0xe8, 0xc5}, {0xde, 0xcf}, {0xe3, 0xcb},
    {0xf0, 0xc0}, {0xea, 0xc6}, {0xe0, 0xd0}, {0xe5, 0xcc},
    {0xe9, 0xc9}, {0xf4, 0xc0}, {0xe2, 0xd1}, {0xf1, 0xc4},
    {0xea, 0xcb}, {0xdb, 0xda}, {0xf8, 0xc0}, {0xe9, 0xce},
    {0xfc, 0xbe}, {0xe0, 0xd8}, {0xe7, 0xd2}, {0xfc, 0xc0},
    {0xfe, 0xbf}, {0xe4, 0xd7}, {0xfc, 0xc2}, {0xfe, 0xc1},
    {0xf1, 0xcd}, {0xe7, 0xd7}, {0xfe, 0xc3}, {0xf0, 0xd0},
    {0xe0, 0xe0}, {0xf6, 0xcc}, {0xf9, 0xca}, {0xf3, 0xd0},
    {0xe8, 0xdb}, {0xee, 0xd6}, {0xfb, 0xcb}, {0xf5, 0xd1},
    {0xec, 0xda}, {0xfb, 0xcd}, {0xf4, 0xd4}, {0xe8, 0xe0},
    {0xef, 0xda}, {0xf4, 0xd6}, {0xf7, 0xd4}, {0xed, 0xde},
    {0xf3, 0xd9}, {0xf7, 0xd6}, {0xe3, 0xea}, {0xf2, 0xdc},
    {0xf6, 0xd9}, {0xfa, 0xd6}, {0xf0, 0xe0}, {0xf5, 0xdc},
    {0xfa, 0xd8}, {0xfd, 0xd6}, {0xf5, 0xde}, {0xf9, 0xdb},
    {0xd8, 0xfd}
};
const unsigned char calib_white_factor_single[65][2] = {
    {0x80, 0}, {0x7e, 0}, {0x7c, 0}, {0x7a, 0},
    {0x78, 0}, {0x76, 0}, {0x74, 0}, {0x72, 0},
    {0x70, 0}, {0x6e, 0}, {0x6c, 0}, {0x6a, 0},
    {0x68, 0}, {0x66, 0}, {0x64, 0}, {0x62, 0},
    {0x60, 0}, {0x5e, 0}, {0x5c, 0}, {0x5a, 0},
    {0x58, 0}, {0x56, 0}, {0x54, 0}, {0x52, 0},
    {0x50, 0}, {0x4e, 0}, {0x4c, 0}, {0x4a, 0},
    {0x48, 0}, {0x46, 0}, {0x44, 0}, {0x42, 0},
    {0x40, 0}, {0x3e, 0}, {0x3c, 0}, {0x3a, 0},
    {0x38, 0}, {0x36, 0}, {0x34, 0}, {0x32, 0},
    {0x30, 0}, {0x2e, 0}, {0x2c, 0}, {0x2a, 0},
    {0x28, 0}, {0x26, 0}, {0x24, 0}, {0x22, 0},
    {0x20, 0}, {0x1e, 0}, {0x1c, 0}, {0x1a, 0},
    {0x18, 0}, {0x16, 0}, {0x14, 0}, {0x12, 0},
    {0x10, 0}, {0x0e, 0}, {0x0c, 0}, {0x0a, 0},
    {8, 0}, {6, 0}, {4, 0}, {2, 0},
    {0, 0}
};

extern void _builtin_set_imask(int);

void calib_load_white_long_page(void)
{
    char sides;
    unsigned long factor;
    unsigned long scaled;
    int u16;
    int chip;
    unsigned short levels[3];
    unsigned char index;
    unsigned char index_2;

    if (settings_test_flags[0] & 16 || settings_test_flags[0] & 2) {
        return;
    }
    if (job_adf_sheet_count == 0) {
    } else {
        return;
    }
    if (win_params.flags.f.adf) {
        sides = 3;
    } else {
        sides = 1;
    }
    calib_white_level_fine[0][0] = 0;
    calib_white_level_fine[0][1] = 0;
    calib_white_level_fine[0][2] = 0;
    calib_white_level_fine[1][0] = 0;
    calib_white_level_fine[1][1] = 0;
    calib_white_level_fine[1][2] = 0;
    calib_white_level[0][0] = settings_white_level_adf_front_chip0[0];
    calib_white_level[0][1] = settings_white_level_adf_front_chip1[0];
    calib_white_level[0][2] = settings_white_level_adf_front_chip2[0];
    calib_white_level[1][0] = settings_white_level_adf_back_chip0[0];
    calib_white_level[1][1] = settings_white_level_adf_back_chip1[0];
    calib_white_level[1][2] = settings_white_level_adf_back_chip2[0];
    for (chip = 0; chip <= 2; chip++) {
        if (chip == 0) {
            index = 0xf8;
            index_2 = 0xef;
        } else if (chip == 1) {
            index = 0xf9;
            index_2 = 0xf0;
        } else {
            index = 0xfa;
            index_2 = 0xf1;
        }
        levels[chip] = settings[index] & 1;
        levels[chip] = levels[chip] << 8;
        levels[chip] += settings[index_2];
    }
    if (levels[0] >= levels[1]) {
        if (levels[0] >= levels[2]) {
            factor = 230 - (calib_black_level[0][0] >> 2);
            factor = factor * 10000;
            factor = factor / (120 - (calib_black_level[0][0] >> 2));
            scaled = (levels[0] + 128) * 100;
            scaled = scaled >> 8;
            factor = factor / scaled;
            for (chip = 0; chip <= 2; chip++) {
                scaled = (levels[chip] + 128) * 100;
                scaled = scaled >> 8;
                scaled = scaled * factor;
                scaled = scaled * 256 / 10000 - 128;
                levels[chip] = scaled;
            }
        } else {
            factor = 230 - (calib_black_level[0][2] >> 2);
            factor = factor * 10000;
            factor = factor / (120 - (calib_black_level[0][2] >> 2));
            scaled = (levels[2] + 128) * 100;
            scaled = scaled >> 8;
            factor = factor / scaled;
            for (chip = 0; chip <= 2; chip++) {
                scaled = (levels[chip] + 128) * 100;
                scaled = scaled >> 8;
                scaled = scaled * factor;
                scaled = scaled * 256 / 10000 - 128;
                levels[chip] = scaled;
            }
        }
    } else {
        if (levels[1] >= levels[2]) {
            factor = 230 - (calib_black_level[0][1] >> 2);
            factor = factor * 10000;
            factor = factor / (120 - (calib_black_level[0][1] >> 2));
            scaled = (levels[1] + 128) * 100;
            scaled = scaled >> 8;
            factor = factor / scaled;
            for (chip = 0; chip <= 2; chip++) {
                scaled = (levels[chip] + 128) * 100;
                scaled = scaled >> 8;
                scaled = scaled * factor;
                scaled = scaled * 256 / 10000 - 128;
                levels[chip] = scaled;
            }
        } else {
            factor = 230 - (calib_black_level[0][2] >> 2);
            factor = factor * 10000;
            factor = factor / (120 - (calib_black_level[0][2] >> 2));
            scaled = (levels[2] + 128) * 100;
            scaled = scaled >> 8;
            factor = factor / scaled;
            for (chip = 0; chip <= 2; chip++) {
                scaled = (levels[chip] + 128) * 100;
                scaled = scaled >> 8;
                scaled = scaled * factor;
                scaled = scaled * 256 / 10000 - 128;
                levels[chip] = scaled;
            }
        }
    }
    for (chip = 0; chip <= 2; chip++) {
        if (levels[chip] & 0x100) {
            index = levels[chip];
            if (index > 128) {
                index = 128;
            }
            calib_white_level[0][chip] = calib_white_factor_pairs[index][0];
            calib_white_level_fine[0][chip] = calib_white_factor_pairs[index][1];
        } else {
            calib_white_level[0][chip] = levels[chip];
            calib_white_level_fine[0][chip] = 0;
        }
    }
    calib_start_lamp_adjust(sides);
    calib_white_level_fine[0][0] = 0;
    calib_white_level_fine[0][1] = 0;
    calib_white_level_fine[0][2] = 0;
    calib_white_level_fine[1][0] = 0;
    calib_white_level_fine[1][1] = 0;
    calib_white_level_fine[1][2] = 0;
    calib_start_lamp_11(sides, 1);
    return;
}

char calib_start_offset(unsigned char sides)
{
    unsigned short x_start;
    unsigned short x_end;
    char error;

    error = 0;
    if (win_params.flags.f.adf) {
        x_start = 0x1e2c;
        x_end = 0x1e3c;
    } else {
        x_start = 0x1e78;
        x_end = x_start + 80;
    }
    if (img_side_params[0].ccd_300dpi_mode) {
        x_start >>= 1;
        x_end >>= 1;
    } else {
        if (win_params.flags.f.adf && !(settings_byte_0x1e[0] & 1) && img_side_params[0].y_res_dpi != 600 && img_side_params[0].y_res_dpi != 300) {
            x_start = x_start + 5;
            x_start -= 83;
        }
    }
    _builtin_set_imask(15);
    p_vdcc_measure[0]->x_start.hi = (unsigned char)(x_start >> 8);
    p_vdcc_measure[0]->x_start.lo = (unsigned char)x_start;
    p_vdcc_measure[0]->x_end.hi = (unsigned char)(x_end >> 8);
    p_vdcc_measure[0]->x_end.lo = (unsigned char)x_end;
    p_vdcc_measure[1]->x_start.hi = (unsigned char)(x_start >> 8);
    p_vdcc_measure[1]->x_start.lo = (unsigned char)x_start;
    p_vdcc_measure[1]->x_end.hi = (unsigned char)(x_end >> 8);
    p_vdcc_measure[1]->x_end.lo = (unsigned char)x_end;
    p_vdcc_measure[0]->select.f.mode = 2;
    p_vdcc_measure[1]->select.f.mode = 2;
    if (*p_vdcc_timing[0] > 0x1000) {
        p_vdcc_measure[0]->select.f.mode = 1;
        p_vdcc_measure[1]->select.f.mode = 1;
        if (calib_measure_mode_swap & 0x8000) {
            p_vdcc_measure[0]->select.f.mode = 2;
        }
        if (calib_measure_mode_swap & 0x80) {
            p_vdcc_measure[1]->select.f.mode = 2;
        }
    }
    if (img_side_params[0].ccd_300dpi_mode) {
        p_vdcc_front_sensor->source_gate.b.b2 = 0;
        p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
    }
    p_vdcc_measure[0]->reg_052.lo = 5;
    p_vdcc_measure[1]->reg_052.lo = 5;
    _builtin_set_imask(0);
    calib_offset_target_x4_front = calib_offset_target_front.v << 2;
    calib_offset_target_2_x4_front = calib_offset_target_2_front << 2;
    calib_offset_target_3_x4_front = calib_offset_target_3_front << 2;
    calib_offset_target_x4_back = calib_offset_target_back.v << 2;
    calib_offset_target_2_x4_back = calib_offset_target_2_back << 2;
    calib_offset_target_3_x4_back = calib_offset_target_3_back << 2;
    x_start = 0;
    if (sides & 1) {
        calib_offset_search_odd_front.l.v = 0xa0a0a;
        calib_offset_search_even_front.l.v = 0xa0a0a;
        calib_offset_odd_front.l.v = calib_offset_search_odd_front.l.v;
        calib_offset_even_front.l.v = calib_offset_search_even_front.l.v;
        x_start |= 21;
    }
    if (sides & 2) {
        calib_offset_search_odd_back.l.v = 0xa0a0a;
        calib_offset_search_even_back.l.v = 0xa0a0a;
        calib_offset_odd_back.l.v = calib_offset_search_odd_back.l.v;
        calib_offset_even_back.l.v = calib_offset_search_even_back.l.v;
        x_start |= 42;
    }
    calib_write_afe_offsets(x_start);
    _builtin_set_imask(15);
    calib_sides = sides;
    calib_running = 1;
    calib_started = 0;
    calib_mode = CALIB_MODE_OFFSET;
    calib_offset_done.w = 0x3f3f;
    if (sides & 1) {
        calib_offset_done.u.hi = 0;
    }
    if (sides & 2) {
        calib_offset_done.u.lo = 0;
    }
    calib_done_copy.w = calib_offset_done.w;
    _builtin_set_imask(0);
    if (calib_wait_done() == 1) {
        _builtin_set_imask(15);
        calib_done_copy.w = calib_offset_done.w;
        if (calib_done_copy.u.hi != 63) {
            error = ERR_OPTICAL_ADF_FRONT;
            if (!win_params.flags.f.adf) {
                error = ERR_OPTICAL_FLATBED;
            }
            if (win_sides.f.selection == 1) {
                error = 0;
            } else {
                if (img_side_params[0].y_res_dpi == 0xf0) {
                    if (calib_measure_mode_swap & 0x8000) {
                        calib_measure_mode_swap &= 0x7fff;
                    } else {
                        calib_measure_mode_swap |= 0x8000;
                    }
                }
                goto end;
            }
        }
        if (sides & 2 && calib_done_copy.u.lo != 63) {
            error = ERR_OPTICAL_ADF_BACK;
            if (!win_sides.f.selection) {
                error = 0;
            } else {
                if (win_params.flags.f.adf && img_side_params[0].y_res_dpi == 0xf0) {
                    if (calib_measure_mode_swap & 0x80) {
                        calib_measure_mode_swap &= 0xff7f;
                    } else {
                        calib_measure_mode_swap |= 0x80;
                    }
                }
                goto end;
            }
        }
        _builtin_set_imask(0);
    }
end:
    if (win_params.flags.f.adf) {
        if (mdc_is_adf_cover_open()) {
            error = ERR_ADF_COVER_OPEN;
        }
    }
    _builtin_set_imask(0);
    if (sides & 1) {
        calib_offset_saved_odd_front.l.v = calib_offset_result_odd_front.v;
        calib_offset_saved_even_front.l.v = calib_offset_result_even_front.v;
    }
    if (sides & 2) {
        calib_offset_saved_odd_back.l.v = calib_offset_result_odd_back.v;
        calib_offset_saved_even_back.l.v = calib_offset_result_even_back.v;
    }
    if (settings_byte_0x1d[0] & 16) {
        error = 0;
    }
    if (error && testmode_number.v == 48) {
        err_raise(error);
        error = 0;
    }
    calib_offset_bytes[0][0] = calib_offset_odd_front.b.chip0;
    calib_offset_bytes[0][1] = calib_offset_even_front.b.chip0;
    calib_offset_bytes[0][2] = calib_offset_odd_front.b.chip1;
    calib_offset_bytes[0][3] = calib_offset_even_front.b.chip1;
    calib_offset_bytes[0][4] = calib_offset_odd_front.b.chip2;
    calib_offset_bytes[0][5] = calib_offset_even_front.b.chip2;
    calib_offset_bytes_back[0][0] = calib_offset_odd_back.b.chip0;
    calib_offset_bytes_back[0][1] = calib_offset_even_back.b.chip0;
    calib_offset_bytes_back[0][2] = calib_offset_odd_back.b.chip1;
    calib_offset_bytes_back[0][3] = calib_offset_even_back.b.chip1;
    calib_offset_bytes_back[0][4] = calib_offset_odd_back.b.chip2;
    calib_offset_bytes_back[0][5] = calib_offset_even_back.b.chip2;
    return error;
}


unsigned char calib_start_gain(unsigned char sides)
{
    unsigned short x_start;
    unsigned short x_end;
    unsigned short gain;
    unsigned short setting;
    unsigned char error;

    error = 0;
    if (settings_byte_0x1e[0] & 0x10) {
        if (sides & 1) {
            setting = 400;
            if (win_params.flags.f.adf) {
                setting += 3;
            }
            x_start = settings[setting];
            if (x_start == 0) {
                x_start = 60;
            }
            x_end = settings_sub_offset_flatbed[setting];
            if (x_end == 0) {
                x_end = 60;
            }
            gain = settings_main_offset_adf_front[setting];
            if (gain == 0) {
                gain = 60;
            }
            afe_set_gain(0, 0, 0, x_start << 2);
            afe_set_gain(0, 0, 1, x_start << 2);
            afe_set_gain(0, 1, 0, x_end << 2);
            afe_set_gain(0, 1, 1, x_end << 2);
            afe_set_gain(0, 2, 0, gain << 2);
            afe_set_gain(0, 2, 1, gain << 2);
        }
        if (sides & 2) {
            setting = 406;
            x_start = settings[setting];
            if (x_start == 0) {
                x_start = 60;
            }
            x_end = settings_sub_offset_flatbed[setting];
            if (x_end == 0) {
                x_end = 60;
            }
            gain = settings_main_offset_adf_front[setting];
            if (gain == 0) {
                gain = 60;
            }
            afe_set_gain(1, 0, 0, x_start << 2);
            afe_set_gain(1, 0, 1, x_start << 2);
            afe_set_gain(1, 1, 0, x_end << 2);
            afe_set_gain(1, 1, 1, x_end << 2);
            afe_set_gain(1, 2, 0, gain << 2);
            afe_set_gain(1, 2, 1, gain << 2);
        }
        return error;
    }
    if (win_params.flags.f.adf) {
        x_start = 0x12f;
        x_end = 0x1db3;
    } else {
        x_start = 0x116;
        x_end = 0x1d9a;
    }
    if (img_side_params[0].ccd_300dpi_mode) {
        x_start >>= 1;
        x_end >>= 1;
    } else if (win_params.flags.f.adf && !(settings_byte_0x1e[0] & 1) && img_side_params[0].y_res_dpi != 600
               && img_side_params[0].y_res_dpi != 300) {
        x_start += 5;
        x_start -= 83;
    }
    _builtin_set_imask(15);
    p_vdcc_measure[0]->x_start.hi = (unsigned char)(x_start >> 8);
    p_vdcc_measure[0]->x_start.lo = (unsigned char)x_start;
    p_vdcc_measure[0]->x_end.hi = (unsigned char)(x_end >> 8);
    p_vdcc_measure[0]->x_end.lo = (unsigned char)x_end;
    p_vdcc_measure[1]->x_start.hi = (unsigned char)(x_start >> 8);
    p_vdcc_measure[1]->x_start.lo = (unsigned char)x_start;
    p_vdcc_measure[1]->x_end.hi = (unsigned char)(x_end >> 8);
    p_vdcc_measure[1]->x_end.lo = (unsigned char)x_end;
    p_vdcc_measure[0]->select.f.mode = 2;
    p_vdcc_measure[1]->select.f.mode = 2;
    if (*p_vdcc_timing[0] > 0x1000) {
        p_vdcc_measure[0]->select.f.mode = 1;
        p_vdcc_measure[1]->select.f.mode = 1;
        if (calib_measure_mode_swap & 0x8000) {
            p_vdcc_measure[0]->select.f.mode = 2;
        }
        if (calib_measure_mode_swap & 0x80) {
            p_vdcc_measure[1]->select.f.mode = 2;
        }
    }
    if (img_side_params[0].ccd_300dpi_mode) {
        p_vdcc_front_sensor->source_gate.b.b2 = 0;
        p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
    }
    p_vdcc_measure[0]->reg_052.lo = 11;
    p_vdcc_measure[1]->reg_052.lo = 11;
    _builtin_set_imask(0);
    calib_gain_target_x4_front = calib_gain_target_front << 2;
    calib_gain_target_2_x4_front = calib_gain_target_2_front << 2;
    calib_gain_target_x4_back = calib_gain_target_back << 2;
    calib_gain_target_2_x4_back = calib_gain_target_2_back << 2;
    p_vdcc_measure[0]->gain_target.lo = calib_gain_target_front;
    p_vdcc_measure[0]->gain_target.hi = 1;
    p_vdcc_measure[1]->gain_target.lo = calib_gain_target_back;
    p_vdcc_measure[1]->gain_target.hi = 1;
    p_vdcc_measure[0]->reg_053.hi = 15;
    p_vdcc_measure[1]->reg_053.hi = 15;
    x_start = 0;
    if (sides & 1) {
        calib_gain_search_odd_front.l.v = 0;
        calib_gain_search_even_front.l.v = 0;
        calib_gain_odd_front.l.v = calib_gain_search_odd_front.l.v;
        calib_gain_even_front.l.v = calib_gain_search_even_front.l.v;
        x_start |= 0x15;
    }
    if (sides & 2) {
        calib_gain_search_odd_back.l.v = 0;
        calib_gain_search_even_back.l.v = 0;
        calib_gain_odd_back.l.v = calib_gain_search_odd_back.l.v;
        calib_gain_even_back.l.v = calib_gain_search_even_back.l.v;
        x_start |= 0x2a;
    }
    calib_write_afe_gains(x_start);
    _builtin_set_imask(15);
    calib_sides = sides;
    calib_running = 1;
    calib_started = 0;
    calib_mode = CALIB_MODE_GAIN;
    calib_gain_done.w = 0x3f3f;
    if (sides & 1) {
        calib_gain_done.u.hi = 0;
    }
    if (sides & 2) {
        calib_gain_done.u.lo = 0;
    }
    calib_done_copy.w = calib_gain_done.w;
    _builtin_set_imask(0);
    if (calib_wait_done() == 1) {
        _builtin_set_imask(15);
        calib_done_copy.w = calib_gain_done.w;
        if (calib_done_copy.u.hi != 63) {
            error = ERR_OPTICAL_ADF_FRONT;
            if (!win_params.flags.f.adf) {
                error = ERR_OPTICAL_FLATBED;
            }
            if (win_sides.f.selection == 1) {
                error = 0;
            } else {
                goto end;
            }
        }
        if ((sides & 2) && calib_done_copy.u.lo != 63) {
            error = ERR_OPTICAL_ADF_BACK;
            if (win_sides.f.selection == 0) {
                error = 0;
            } else {
                goto end;
            }
        }
        _builtin_set_imask(0);
    }
end:
    if (win_params.flags.f.adf) {
        if (mdc_is_adf_cover_open()) {
            error = ERR_ADF_COVER_OPEN;
        }
    }
    _builtin_set_imask(0);
    if (sides & 1) {
        calib_gain_saved_odd_front.v = calib_gain_result_odd_front.v;
        calib_gain_saved_even_front.v = calib_gain_result_even_front.v;
    }
    if (sides & 2) {
        calib_gain_saved_odd_back.v = calib_gain_result_odd_back.v;
        calib_gain_saved_even_back.v = calib_gain_result_even_back.v;
    }
    if (settings_byte_0x1d[0] & 0x10) {
        error = 0;
    }
    if (error && testmode_number.v == 48) {
        err_raise(error);
        error = 0;
    }
    calib_gain_bytes[0].odd_chip0 = calib_gain_odd_front.b.chip0;
    calib_gain_bytes[0].even_chip0 = calib_gain_even_front.b.chip0;
    calib_gain_bytes[0].odd_chip1 = calib_gain_odd_front.b.chip1;
    calib_gain_bytes[0].even_chip1 = calib_gain_even_front.b.chip1;
    calib_gain_bytes[0].odd_chip2 = calib_gain_odd_front.b.chip2;
    calib_gain_bytes[0].even_chip2 = calib_gain_even_front.b.chip2;
    calib_gain_bytes[1].odd_chip0 = calib_gain_odd_back.b.chip0;
    calib_gain_bytes[1].even_chip0 = calib_gain_even_back.b.chip0;
    calib_gain_bytes[1].odd_chip1 = calib_gain_odd_back.b.chip1;
    calib_gain_bytes[1].even_chip1 = calib_gain_even_back.b.chip1;
    calib_gain_bytes[1].odd_chip2 = calib_gain_odd_back.b.chip2;
    calib_gain_bytes[1].even_chip2 = calib_gain_even_back.b.chip2;
    if (!win_params.flags.f.adf) {
        settings_gain_flatbed_chip0[0] = calib_gain_odd_front.b.chip0;
        settings_gain_flatbed_chip1[0] = calib_gain_odd_front.b.chip1;
        settings_gain_flatbed_chip2[0] = calib_gain_odd_front.b.chip2;
    } else {
        settings_gain_adf_front_chip0[0] = calib_gain_odd_front.b.chip0;
        settings_gain_adf_front_chip1[0] = calib_gain_odd_front.b.chip1;
        settings_gain_adf_front_chip2[0] = calib_gain_odd_front.b.chip2;
        settings_gain_adf_back_chip0[0] = calib_gain_odd_back.b.chip0;
        settings_gain_adf_back_chip1[0] = calib_gain_odd_back.b.chip1;
        settings_gain_adf_back_chip2[0] = calib_gain_odd_back.b.chip2;
    }
    return error;
}

struct vdcc_field_a_m { unsigned char :2, m:3, :3; };
struct vdcc_regs_calib {
    char reg_000_008[9];
    struct vdcc_field_a_m reg_009;
    char reg_00a;
    struct vdcc_field_a_m output_ctrl;
};
struct vdcc_correction_bytes {
    char pad_00[12];
    unsigned char black_chip0_a:8,
                  black_chip0_b:8,
                  black_chip1_a:8,
                  black_chip1_b:8,
                  black_chip2_a:8,
                  black_chip2_b:8;
};
struct vdcc_regs_calib_ptr { struct vdcc_regs_calib *v; };
struct vdcc_correction_calib_ptr { struct vdcc_correction_bytes *v; };

void calib_start_hook(int unused_arg)
{
}

void calib_start_apply(unsigned char sides)
{
    int r;

    _builtin_set_imask(15);
    ((struct vdcc_regs_calib_ptr *)p_vdcc_front)[0].v->reg_009.m = 7;
    ((struct vdcc_regs_calib_ptr *)p_vdcc_front)[0].v->output_ctrl.m = 7;
    ((struct vdcc_regs_calib_ptr *)p_vdcc_back)[0].v->reg_009.m = 7;
    ((struct vdcc_regs_calib_ptr *)p_vdcc_back)[0].v->output_ctrl.m = 7;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_front_correction)[0].v->black_chip0_a = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_front_correction)[0].v->black_chip0_b = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_front_correction)[0].v->black_chip1_a = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_front_correction)[0].v->black_chip1_b = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_front_correction)[0].v->black_chip2_a = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_front_correction)[0].v->black_chip2_b = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_back_correction)[0].v->black_chip0_a = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_back_correction)[0].v->black_chip0_b = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_back_correction)[0].v->black_chip1_a = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_back_correction)[0].v->black_chip1_b = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_back_correction)[0].v->black_chip2_a = 0;
    ((struct vdcc_correction_calib_ptr *)p_vdcc_back_correction)[0].v->black_chip2_b = 0;
    _builtin_set_imask(0);
    if (sides == 1) {
    } else {
        calib_apply_flag = 0;
    }
    _builtin_set_imask(15);
    calib_running = 1;
    calib_started = 0;
    calib_mode = CALIB_MODE_APPLY;
    _builtin_set_imask(0);
    if (sides != 4) {
        if (calib_wait_done() == 1) {
        }
    }
    return;
}


void calib_start_lamp_check(unsigned char sides)
{
    unsigned short x_start;
    unsigned short x_end;
    int r;

    if (win_params.flags.f.adf) {
        x_start = 0xe71;
    } else {
        x_start = 0xe5f;
    }
    x_end = x_start + 0x240;
    if (img_side_params[0].ccd_300dpi_mode) {
        x_start >>= 1;
        x_end >>= 1;
    }
    _builtin_set_imask(15);
    p_vdcc_measure[0]->x_start.hi = (unsigned char)(x_start >> 8);
    p_vdcc_measure[0]->x_start.lo = (unsigned char)x_start;
    p_vdcc_measure[0]->x_end.hi = (unsigned char)(x_end >> 8);
    p_vdcc_measure[0]->x_end.lo = (unsigned char)x_end;
    p_vdcc_measure[1]->x_start.hi = (unsigned char)(x_start >> 8);
    p_vdcc_measure[1]->x_start.lo = (unsigned char)x_start;
    p_vdcc_measure[1]->x_end.hi = (unsigned char)(x_end >> 8);
    p_vdcc_measure[1]->x_end.lo = (unsigned char)x_end;
    p_vdcc_measure[0]->select.f.mode = 3;
    p_vdcc_measure[1]->select.f.mode = 3;
    p_vdcc_measure[0]->reg_052.lo = 9;
    p_vdcc_measure[1]->reg_052.lo = 9;
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    calib_sides = sides;
    calib_running = 1;
    calib_started = 0;
    calib_mode = CALIB_MODE_LAMP_CHECK;
    _builtin_set_imask(0);
    if (calib_wait_done() == 1) {
    }
    return;
}

extern void _builtin_set_imask(int);

void calib_start_lamp_adjust(char sides)
{
    int unused;
    _builtin_set_imask(15);
    calib_lamp_adjust_first = 1;
    calib_running = 1;
    calib_started = 0;
    calib_mode = 13;
    _builtin_set_imask(0);
    if (calib_wait_done() == 1) {
    }
    _builtin_set_imask(15);
    calib_running = 1;
    calib_started = 0;
    calib_mode = CALIB_MODE_LAMP_ADJUST;
    _builtin_set_imask(0);
    if (calib_wait_done() == 1) {
    }
    _builtin_set_imask(15);
    calib_lamp_adjust_first = 0;
    calib_running = 1;
    calib_started = 0;
    calib_mode = 13;
    _builtin_set_imask(0);
    if (calib_wait_done() == 1) {
    }
    return;
}

void calib_start_lamp_11(char sides, int lamp_11_flag)
{
    calib_lamp_11_flag = lamp_11_flag;
    _builtin_set_imask(15);
    calib_running = 1;
    calib_started = 0;
    calib_mode = 11;
    _builtin_set_imask(0);
    if (calib_wait_done() == 1) {
    }
    return;
}


int calib_start_black(void)
{
    unsigned short x_start;
    unsigned short x_end;
    short avg;

    if (win_params.flags.f.adf) {
        x_start = 0x1e2c;
        x_end = 0x1e3c;
    } else {
        x_start = 0x1e78;
        x_end = x_start + 80;
    }
    if (img_side_params[0].ccd_300dpi_mode) {
        x_start >>= 1;
        x_end >>= 1;
    } else {
        if (win_params.flags.f.adf && !(settings_byte_0x1e[0] & 1) && img_side_params[0].y_res_dpi != 600 && img_side_params[0].y_res_dpi != 300) {
            x_start = x_start + 5;
            x_start -= 83;
        }
    }
    _builtin_set_imask(15);

    p_vdcc_measure[0]->x_start.hi = (unsigned char)(x_start >> 8);
    p_vdcc_measure[0]->x_start.lo = (unsigned char)x_start;
    p_vdcc_measure[0]->x_end.hi = (unsigned char)(x_end >> 8);
    p_vdcc_measure[0]->x_end.lo = (unsigned char)x_end;

    p_vdcc_measure[1]->x_start.hi = (unsigned char)(x_start >> 8);
    p_vdcc_measure[1]->x_start.lo = (unsigned char)x_start;
    p_vdcc_measure[1]->x_end.hi = (unsigned char)(x_end >> 8);
    p_vdcc_measure[1]->x_end.lo = (unsigned char)x_end;

    p_vdcc_measure[0]->select.f.mode = 2;
    p_vdcc_measure[1]->select.f.mode = 2;
    if (*p_vdcc_timing[0] > 0x1000) {
        p_vdcc_measure[0]->select.f.mode = 1;
        p_vdcc_measure[1]->select.f.mode = 1;
        if (calib_measure_mode_swap & 0x8000) {
            p_vdcc_measure[0]->select.f.mode = 2;
        }
        if (calib_measure_mode_swap & 0x80) {
            p_vdcc_measure[1]->select.f.mode = 2;
        }
    }
    if (img_side_params[0].ccd_300dpi_mode) {
        p_vdcc_front_sensor->source_gate.b.b2 = 0;
        p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
    }
    p_vdcc_measure[0]->reg_052.lo = 5;
    p_vdcc_measure[1]->reg_052.lo = 5;
    _builtin_set_imask(0);
    _builtin_set_imask(15);

    calib_black_pass1_front_chip0.w = 0;
    calib_black_pass1_front_chip1.w = 0;
    calib_black_pass1_front_chip2.w = 0;
    calib_black_pass1_back_chip0.w = 0;
    calib_black_pass1_back_chip1.w = 0;
    calib_black_pass1_back_chip2.w = 0;
    calib_black_pass2_front_chip0.w = 0;
    calib_black_pass2_front_chip1.w = 0;
    calib_black_pass2_front_chip2.w = 0;
    calib_black_pass2_back_chip0.w = 0;
    calib_black_pass2_back_chip1.w = 0;
    calib_black_pass2_back_chip2.w = 0;
    _builtin_set_imask(0);
    _builtin_set_imask(15);
    calib_running = 1;
    calib_started = 0;
    calib_mode = CALIB_MODE_BLACK;
    _builtin_set_imask(0);
    if (calib_wait_done() == 1) {
    }
    if ((settings_byte_0x1d[0] & 32)) {

        calib_black_pass1_front_chip0.w = calib_black_pass2_front_chip0.w = calib_black_pass1_front_chip1.w = calib_black_pass2_front_chip1.w = calib_black_pass1_front_chip2.w = calib_black_pass2_front_chip2.w = settings_fixed_black_front[0];
        calib_black_pass1_back_chip0.w = calib_black_pass2_back_chip0.w = calib_black_pass1_back_chip1.w = calib_black_pass2_back_chip1.w = calib_black_pass1_back_chip2.w = calib_black_pass2_back_chip2.w = settings_fixed_black_back[0];
    }
    _builtin_set_imask(15);
    if (!win_params.flags.f.adf) {
        if (calib_black_pass1_front_chip0.w >= settings_black_margin_flatbed_chip0[0]) {
            calib_black_pass1_front_chip0.w -= settings_black_margin_flatbed_chip0[0];
        } else {
            calib_black_pass1_front_chip0.w = 0;
        }
        if (calib_black_pass2_front_chip0.w >= settings_black_margin_flatbed_chip0[0]) {
            calib_black_pass2_front_chip0.w -= settings_black_margin_flatbed_chip0[0];
        } else {
            calib_black_pass2_front_chip0.w = 0;
        }
        if (calib_black_pass1_front_chip1.w >= settings_black_margin_flatbed_chip1[0]) {
            calib_black_pass1_front_chip1.w -= settings_black_margin_flatbed_chip1[0];
        } else {
            calib_black_pass1_front_chip1.w = 0;
        }
        if (calib_black_pass2_front_chip1.w >= settings_black_margin_flatbed_chip1[0]) {
            calib_black_pass2_front_chip1.w -= settings_black_margin_flatbed_chip1[0];
        } else {
            calib_black_pass2_front_chip1.w = 0;
        }
        if (calib_black_pass1_front_chip2.w >= settings_black_margin_flatbed_chip2[0]) {
            calib_black_pass1_front_chip2.w -= settings_black_margin_flatbed_chip2[0];
        } else {
            calib_black_pass1_front_chip2.w = 0;
        }
        if (calib_black_pass2_front_chip2.w >= settings_black_margin_flatbed_chip2[0]) {
            calib_black_pass2_front_chip2.w -= settings_black_margin_flatbed_chip2[0];
        } else {
            calib_black_pass2_front_chip2.w = 0;
        }
    } else {
        if (calib_black_pass1_front_chip0.w >= settings_black_margin_adf_front_chip0[0]) {
            calib_black_pass1_front_chip0.w -= settings_black_margin_adf_front_chip0[0];
        } else {
            calib_black_pass1_front_chip0.w = 0;
        }
        if (calib_black_pass2_front_chip0.w >= settings_black_margin_adf_front_chip0[0]) {
            calib_black_pass2_front_chip0.w -= settings_black_margin_adf_front_chip0[0];
        } else {
            calib_black_pass2_front_chip0.w = 0;
        }
        if (calib_black_pass1_front_chip1.w >= settings_black_margin_adf_front_chip1[0]) {
            calib_black_pass1_front_chip1.w -= settings_black_margin_adf_front_chip1[0];
        } else {
            calib_black_pass1_front_chip1.w = 0;
        }
        if (calib_black_pass2_front_chip1.w >= settings_black_margin_adf_front_chip1[0]) {
            calib_black_pass2_front_chip1.w -= settings_black_margin_adf_front_chip1[0];
        } else {
            calib_black_pass2_front_chip1.w = 0;
        }
        if (calib_black_pass1_front_chip2.w >= settings_black_margin_adf_front_chip2[0]) {
            calib_black_pass1_front_chip2.w -= settings_black_margin_adf_front_chip2[0];
        } else {
            calib_black_pass1_front_chip2.w = 0;
        }
        if (calib_black_pass2_front_chip2.w >= settings_black_margin_adf_front_chip2[0]) {
            calib_black_pass2_front_chip2.w -= settings_black_margin_adf_front_chip2[0];
        } else {
            calib_black_pass2_front_chip2.w = 0;
        }
        if (calib_black_pass1_back_chip0.w >= settings_black_margin_adf_back_chip0[0]) {
            calib_black_pass1_back_chip0.w -= settings_black_margin_adf_back_chip0[0];
        } else {
            calib_black_pass1_back_chip0.w = 0;
        }
        if (calib_black_pass2_back_chip0.w >= settings_black_margin_adf_back_chip0[0]) {
            calib_black_pass2_back_chip0.w -= settings_black_margin_adf_back_chip0[0];
        } else {
            calib_black_pass2_back_chip0.w = 0;
        }
        if (calib_black_pass1_back_chip1.w >= settings_black_margin_adf_back_chip1[0]) {
            calib_black_pass1_back_chip1.w -= settings_black_margin_adf_back_chip1[0];
        } else {
            calib_black_pass1_back_chip1.w = 0;
        }
        if (calib_black_pass2_back_chip1.w >= settings_black_margin_adf_back_chip1[0]) {
            calib_black_pass2_back_chip1.w -= settings_black_margin_adf_back_chip1[0];
        } else {
            calib_black_pass2_back_chip1.w = 0;
        }
        if (calib_black_pass1_back_chip2.w >= settings_black_margin_adf_back_chip2[0]) {
            calib_black_pass1_back_chip2.w -= settings_black_margin_adf_back_chip2[0];
        } else {
            calib_black_pass1_back_chip2.w = 0;
        }
        if (calib_black_pass2_back_chip2.w >= settings_black_margin_adf_back_chip2[0]) {
            calib_black_pass2_back_chip2.w -= settings_black_margin_adf_back_chip2[0];
        } else {
            calib_black_pass2_back_chip2.w = 0;
        }
    }
    if (img_side_params[0].ccd_300dpi_mode) {
        avg = (calib_black_pass1_front_chip0.w + calib_black_pass2_front_chip0.w) / 2;
        p_vdcc_front_correction[0]->black_chip0_a = (char)avg;
        p_vdcc_front_correction[0]->black_chip0_b = (char)avg;
        avg = (calib_black_pass1_front_chip1.w + calib_black_pass2_front_chip1.w) / 2;
        p_vdcc_front_correction[0]->black_chip1_a = (char)avg;
        p_vdcc_front_correction[0]->black_chip1_b = (char)avg;
        avg = (calib_black_pass1_front_chip2.w + calib_black_pass2_front_chip2.w) / 2;
        p_vdcc_front_correction[0]->black_chip2_a = (char)avg;
        p_vdcc_front_correction[0]->black_chip2_b = (char)avg;
        avg = (calib_black_pass1_back_chip0.w + calib_black_pass2_back_chip0.w) / 2;
        p_vdcc_back_correction[0]->black_chip0_a = (char)avg;
        p_vdcc_back_correction[0]->black_chip0_b = (char)avg;
        avg = (calib_black_pass1_back_chip1.w + calib_black_pass2_back_chip1.w) / 2;
        p_vdcc_back_correction[0]->black_chip1_a = (char)avg;
        p_vdcc_back_correction[0]->black_chip1_b = (char)avg;
        avg = (calib_black_pass1_back_chip2.w + calib_black_pass2_back_chip2.w) / 2;
        p_vdcc_back_correction[0]->black_chip2_a = (char)avg;
        p_vdcc_back_correction[0]->black_chip2_b = (char)avg;
        _builtin_set_imask(0);
    } else if (img_side_params[0].y_res_dpi == 240) {
        if (settings_byte_0x3b[0] & 2) {
            p_vdcc_front_correction[0]->black_chip0_a = calib_black_pass2_front_chip0.c.lo;
            p_vdcc_front_correction[0]->black_chip0_b = calib_black_pass1_front_chip0.c.lo;
            p_vdcc_front_correction[0]->black_chip1_a = calib_black_pass2_front_chip1.c.lo;
            p_vdcc_front_correction[0]->black_chip1_b = calib_black_pass1_front_chip1.c.lo;
            p_vdcc_front_correction[0]->black_chip2_a = calib_black_pass2_front_chip2.c.lo;
            p_vdcc_front_correction[0]->black_chip2_b = calib_black_pass1_front_chip2.c.lo;
            p_vdcc_back_correction[0]->black_chip0_a = calib_black_pass1_back_chip0.c.lo;
            p_vdcc_back_correction[0]->black_chip0_b = calib_black_pass2_back_chip0.c.lo;
            p_vdcc_back_correction[0]->black_chip1_a = calib_black_pass1_back_chip1.c.lo;
            p_vdcc_back_correction[0]->black_chip1_b = calib_black_pass2_back_chip1.c.lo;
            p_vdcc_back_correction[0]->black_chip2_a = calib_black_pass1_back_chip2.c.lo;
            p_vdcc_back_correction[0]->black_chip2_b = calib_black_pass2_back_chip2.c.lo;
        } else {
            p_vdcc_front_correction[0]->black_chip0_a = calib_black_pass1_front_chip0.c.lo;
            p_vdcc_front_correction[0]->black_chip0_b = calib_black_pass2_front_chip0.c.lo;
            p_vdcc_front_correction[0]->black_chip1_a = calib_black_pass1_front_chip1.c.lo;
            p_vdcc_front_correction[0]->black_chip1_b = calib_black_pass2_front_chip1.c.lo;
            p_vdcc_front_correction[0]->black_chip2_a = calib_black_pass1_front_chip2.c.lo;
            p_vdcc_front_correction[0]->black_chip2_b = calib_black_pass2_front_chip2.c.lo;
            p_vdcc_back_correction[0]->black_chip0_a = calib_black_pass2_back_chip0.c.lo;
            p_vdcc_back_correction[0]->black_chip0_b = calib_black_pass1_back_chip0.c.lo;
            p_vdcc_back_correction[0]->black_chip1_a = calib_black_pass2_back_chip1.c.lo;
            p_vdcc_back_correction[0]->black_chip1_b = calib_black_pass1_back_chip1.c.lo;
            p_vdcc_back_correction[0]->black_chip2_a = calib_black_pass2_back_chip2.c.lo;
            p_vdcc_back_correction[0]->black_chip2_b = calib_black_pass1_back_chip2.c.lo;
        }
        _builtin_set_imask(0);
    } else {
        if (settings_byte_0x3b[0] & 2) {
            p_vdcc_front_correction[0]->black_chip0_a = calib_black_pass1_front_chip0.c.lo;
            p_vdcc_front_correction[0]->black_chip0_b = calib_black_pass2_front_chip0.c.lo;
            p_vdcc_front_correction[0]->black_chip1_a = calib_black_pass1_front_chip1.c.lo;
            p_vdcc_front_correction[0]->black_chip1_b = calib_black_pass2_front_chip1.c.lo;
            p_vdcc_front_correction[0]->black_chip2_a = calib_black_pass1_front_chip2.c.lo;
            p_vdcc_front_correction[0]->black_chip2_b = calib_black_pass2_front_chip2.c.lo;
            p_vdcc_back_correction[0]->black_chip0_a = calib_black_pass1_back_chip0.c.lo;
            p_vdcc_back_correction[0]->black_chip0_b = calib_black_pass2_back_chip0.c.lo;
            p_vdcc_back_correction[0]->black_chip1_a = calib_black_pass1_back_chip1.c.lo;
            p_vdcc_back_correction[0]->black_chip1_b = calib_black_pass2_back_chip1.c.lo;
            p_vdcc_back_correction[0]->black_chip2_a = calib_black_pass1_back_chip2.c.lo;
            p_vdcc_back_correction[0]->black_chip2_b = calib_black_pass2_back_chip2.c.lo;
        } else {
            p_vdcc_front_correction[0]->black_chip0_a = calib_black_pass2_front_chip0.c.lo;
            p_vdcc_front_correction[0]->black_chip0_b = calib_black_pass1_front_chip0.c.lo;
            p_vdcc_front_correction[0]->black_chip1_a = calib_black_pass2_front_chip1.c.lo;
            p_vdcc_front_correction[0]->black_chip1_b = calib_black_pass1_front_chip1.c.lo;
            p_vdcc_front_correction[0]->black_chip2_a = calib_black_pass2_front_chip2.c.lo;
            p_vdcc_front_correction[0]->black_chip2_b = calib_black_pass1_front_chip2.c.lo;
            p_vdcc_back_correction[0]->black_chip0_a = calib_black_pass2_back_chip0.c.lo;
            p_vdcc_back_correction[0]->black_chip0_b = calib_black_pass1_back_chip0.c.lo;
            p_vdcc_back_correction[0]->black_chip1_a = calib_black_pass2_back_chip1.c.lo;
            p_vdcc_back_correction[0]->black_chip1_b = calib_black_pass1_back_chip1.c.lo;
            p_vdcc_back_correction[0]->black_chip2_a = calib_black_pass2_back_chip2.c.lo;
            p_vdcc_back_correction[0]->black_chip2_b = calib_black_pass1_back_chip2.c.lo;
        }
        _builtin_set_imask(0);
    }
    calib_black_level[0][0] = (calib_black_pass2_front_chip0.w + calib_black_pass1_front_chip0.w) >> 1;
    calib_black_level[0][1] = (calib_black_pass2_front_chip1.w + calib_black_pass1_front_chip1.w) >> 1;
    calib_black_level[0][2] = (calib_black_pass2_front_chip2.w + calib_black_pass1_front_chip2.w) >> 1;
    calib_black_level[1][0] = (calib_black_pass2_back_chip0.w + calib_black_pass1_back_chip0.w) >> 1;
    calib_black_level[1][1] = (calib_black_pass2_back_chip1.w + calib_black_pass1_back_chip1.w) >> 1;
    calib_black_level[1][2] = (calib_black_pass2_back_chip2.w + calib_black_pass1_back_chip2.w) >> 1;
    return;
}

extern void _builtin_set_imask(int);

struct vdcc_counter_reg_0e { short pad_00[7]; unsigned short step_count:16; };

void calib_hook(char unused_arg) {}
#pragma section
/* ---- L1 (P): the line interrupt (line_handle_irq, line_handle_side), the calibration sequence
   and engine, the step functions of calib_steps_offset .. calib_steps_mode_13. */

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);


/* line_handle_page_events's objects */


/* Uninitialised data (section B). */
int line_irq_imask;
unsigned short line_back_events_pending;
unsigned short line_back_delay_lines;
unsigned short line_back_count_20;
unsigned short line_back_count_100;
unsigned short line_front_length_lines;
unsigned long calib_follow_start_lines;

void line_handle_irq(void)
{
    line_irq_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (p_intc->ISR.BIT.IRQ0F) {
        p_intc->ISR.WORD = 127;
    }
    _builtin_set_imask(line_irq_imask);
    if (line_side_state[0].state_flags.v != 0) {
        line_handle_side(0);
    }
    if (line_side_state[1].state_flags.v != 0) {
        line_handle_side(1);
    }
    line_run_mdc_requests();
    line_irq_count++;
    line_side_state[0].line_count++;
    line_side_state[1].line_count++;
    if (calib_running != 0) {
        calib_run_step();
    }
    line_run_page_steps();
    line_count_mdc_waits();
}

void line_handle_side(int side)
{
    unsigned short r;
    unsigned short events;
    events = line_side_state[side].state_flags.v;
    if (events & 0xc0) {
        line_handle_page_events(side);
    }
    if (events & 0xc) {
        calib_handle_line(side);
    }
    if (events & 0x1000) {
        calib_record_mark_sample();
    }
    return;
}

void line_handle_page_events(int side)
{
    struct page_record *scanning_page;
    volatile struct line_side_record *state;
    struct line_timing *timing;
    unsigned short counter_steps;
    unsigned short lines_done;
    unsigned char capture_buf;
    unsigned short lines;
    unsigned short length_lines;

    scanning_page = &page_records[page_scanning.v];
    state = &line_side_state[side];
    timing = &line_page_timing[side];
    if (state->state_flags.v & 0x40) {
        _builtin_set_imask(15);
        state->state_flags.f.page_state = 2;
        if ((hw_hostif_flags.f.own_hostif && !(maint_mode_word.v & 0x5000))
            || ((maint_mode_word.v & 0x5000) && maint_mode_word.c.mode >= 1 && maint_mode_word.c.mode <= 4)) {
            line_page_state[side].state_flags.f.page_step_state = 3;
        }
        if (win_sides.f.selection == 0 && side == 1) {
            line_page_state[1].state_flags.f.page_step_state = 0;
        }
        if ((!win_params.side[side].white_level_follow.f.off && !hw_hostif_flags.f.own_hostif) || (settings_byte_0x1e[0] & 8)) {
            state->state_flags.f.follow_state = 3;
        }
        if (hw_hostif_flags.f.own_hostif && !win_params.side[side].white_level_follow.f.off && win_params.side[0].composition.f.color) {
            state->state_flags.f.follow_state = 3;
            if (job_long_page_flags & 0x80) {
                state->state_flags.f.follow_state = 0;
            }
        }
        if (state->state_flags.s.follow_state == 3) {
            if (mdc_adf_deskew_or_overscan_on()) {
                if (img_overscan_shift_on[side]) {
                    lines = img_overscan_margin.v + 0x179;
                    img_follow_start_lines[side].v = rt_divu32(lines * img_side_params[side].y_res_dpi, 600);
                } else {
                    calib_follow_start_lines = rt_divu32(img_side_params[side].y_res_dpi * 0x178, 600);
                }
            } else {
                calib_follow_start_lines = rt_divu32(img_side_params[side].y_res_dpi * 0x94, 600);
            }
        }
        timing->lines = 0;
        state->elapsed_lines = 0;
        state->top_edge_lines = 0;
        line_motion_stop_lines[side] = 0;
        if (mdc_mailbox.sensor_status.f.top) {
            state->state_flags.v |= 0x400;
            line_side_state[1].state_flags.v |= 0x400;
        }
        _builtin_set_imask(line_irq_imask);
        return;
    }
    _builtin_set_imask(15);
    if ((job_long_page_flags & 0x80) && (job_long_page_flags & 4)) {
        line_restart_capture(side);
        return;
    }
    _builtin_set_imask(line_irq_imask);
    if (state->elapsed_lines < 0xfff0) {
        state->elapsed_lines++;
    }
    _builtin_set_imask(15);
    if (side == 0) {
        if (state->state_flags.v & 0x800) {
            if (!(state->state_flags.v & 0x200)) {
                if (!mdc_mailbox.sensor_status.f.pick) {
                    state->state_flags.v |= 0x200;
                    line_side_state[1].state_flags.v |= 0x200;
                }
            }
            if (!(state->state_flags.v & 0x4000)) {
                if (mdc_adf_deskew_or_overscan_on() && hw_hostif_flags.f.own_hostif && win_params.flags.f.adf && (settings_overscan_control[0] & 0x80)) {
                    if (!mdc_mailbox.sensor_status.f.top) {
                        if (state->state_flags.v & 0x400) {
                            state->state_flags.v |= 0x4000;
                            line_side_state[1].state_flags.v |= 0x4000;
                        }
                    } else if (!(state->state_flags.v & 0x400)) {
                        state->state_flags.v |= 0x400;
                        line_side_state[1].state_flags.v |= 0x400;
                        if (img_overscan_shift_on[0]) {
                            capture_buf = page_side_state[0].capture_buf;
                            lines = img_overscan_shift[0].v;
                            lines = rt_divu32(lines * img_side_params[0].y_res_dpi, 600);
                            if (img_overscan_shift_negative[0]) {
                                if (p_vdcc_capture[0]->capture_ctrl.capture_run) {
                                    line_read_captured_lines(0);
                                    if (line_captured_lines[0].w >= lines) {
                                        lines = line_captured_lines[0].w - lines;
                                    } else {
                                        lines = 0;
                                    }
                                } else {
                                    lines = 0;
                                }
                            } else {
                                if (p_vdcc_capture[0]->capture_ctrl.capture_run) {
                                    line_read_captured_lines(0);
                                    lines = line_captured_lines[0].w + lines;
                                } else {
                                    lines = 0;
                                }
                            }
                            img_overscan_shift_lines[0][capture_buf] = lines;
                            img_follow_start_lines[0].v += img_overscan_shift_lines[0][capture_buf];
                        }
                        if (win_sides.f.selection) {
                            if (img_overscan_shift_on[1]) {
                                capture_buf = page_side_state[1].capture_buf;
                                lines = img_overscan_shift[1].v;
                                lines = rt_divu32(lines * img_side_params[1].y_res_dpi, 600);
                                if (img_overscan_shift_negative[1]) {
                                    if (p_vdcc_capture[1]->capture_ctrl.capture_run) {
                                        line_read_captured_lines(1);
                                        if (((volatile unsigned short *)line_captured_lines)[1] >= lines) {
                                            lines = ((volatile unsigned short *)line_captured_lines)[1] - lines;
                                        } else {
                                            lines = 0;
                                        }
                                    } else {
                                        lines = 0;
                                    }
                                } else {
                                    if (p_vdcc_capture[1]->capture_ctrl.capture_run) {
                                        line_read_captured_lines(1);
                                        lines = ((volatile unsigned short *)line_captured_lines)[1] + lines;
                                    } else {
                                        lines = 0;
                                    }
                                }
                                img_overscan_shift_lines[1][capture_buf] = lines;
                                img_follow_start_lines[1].v += img_overscan_shift_lines[1][capture_buf];
                            }
                        }
                    }
                } else if (!mdc_mailbox.sensor_status.f.top) {
                    state->state_flags.v |= 0x4000;
                    line_side_state[1].state_flags.v |= 0x4000;
                }
            }
            if (!(mdc_mailbox.motion_flags & MDC_MOTION_FRONT)) {
                state->top_edge_lines++;
                capture_buf = page_side_state[0].capture_buf;
                if (!(state->state_flags.v & 0x100)) {
                    state->state_flags.v |= 0x100;
                    if (line_page_state[0].state_flags.f.page_step_state && !p_vdcc_capture[0]->capture_ctrl.capture_run) {
                        line_page_state[0].state_flags.f.page_error = 1;
                        _builtin_set_imask(line_irq_imask);
                        return;
                    }
                    if (((volatile unsigned short (*)[22])edge_top_lines_front)[capture_buf][0] == 0xffff) {
                        if (1) ;
                        ((volatile unsigned short (*)[22])edge_top_lines_front)[capture_buf][0] = line_captured_lines[0].w;
                    }
                    if (!(line_back_events_pending & 8) && !(line_side_state[1].state_flags.v & 0x100)) {
                        line_back_events_pending |= 8;
                        line_back_count_100 = 0;
                        line_front_length_lines = 0xffff;
                    }
                }
                if (state->top_edge_lines > timing->event_20_line && !(state->state_flags.v & 0x20)) {
                    state->state_flags.v |= 0x20;
                    if (edge_event_20_lines_front[capture_buf][0] == 0xffff) {
                        if (1) ;
                        edge_event_20_lines_front[capture_buf][0] = line_captured_lines[0].w;
                    }
                    if (!(line_back_events_pending & 0x80) && !(line_side_state[1].state_flags.v & 0x20)) {
                        line_back_events_pending |= 0x80;
                        line_back_count_20 = 0;
                    }
                }
            }
        } else if (mdc_mailbox.motion_flags & MDC_MOTION_FRONT) {
            state->state_flags.v |= 0x800;
            line_motion_stop_lines[0] = 0;
            line_back_events_pending = 0;
            line_motion_flags |= 0x8000;
            line_motion_flags |= 0x800;
        }
    } else {
        if (state->state_flags.v & 0x800) {
            if (line_back_events_pending & 8) {
                ++line_back_count_100;
                if (line_back_count_100 > line_back_delay_lines) {
                    line_back_events_pending &= 0xfff7;
                    state->state_flags.v |= 0x100;
                    capture_buf = page_side_state[1].capture_buf;
                    edge_top_lines_back[capture_buf].w = ((volatile unsigned short (*)[22])edge_top_lines_front)[capture_buf][0];
                    if (img_overscan_shift_on[0]) {
                        edge_top_lines_back[capture_buf].w = edge_top_lines_back[capture_buf].w - img_overscan_shift_lines[0][capture_buf] + img_overscan_shift_lines[1][capture_buf];
                    }
                }
            }
            if (state->state_flags.v & 0x100) {
                state->top_edge_lines++;
            }
            if (line_back_events_pending & 0x80) {
                ++line_back_count_20;
                if (line_back_count_20 > line_back_delay_lines) {
                    line_back_events_pending &= 0xff7f;
                    state->state_flags.v |= 0x20;
                    capture_buf = page_side_state[1].capture_buf;
                    edge_event_20_lines_back[capture_buf].w = edge_event_20_lines_front[capture_buf][0];
                    if (img_overscan_shift_on[0]) {
                        edge_event_20_lines_back[capture_buf].w = edge_event_20_lines_back[capture_buf].w - img_overscan_shift_lines[0][capture_buf] + img_overscan_shift_lines[1][capture_buf];
                    }
                }
            }
        } else if (mdc_mailbox.motion_flags & MDC_MOTION_BACK) {
            state->state_flags.v |= 0x800;
            line_motion_stop_lines_back[0] = 0;
            line_motion_flags |= 0x8000;
            line_motion_flags |= 0x400;
            line_back_delay_lines = line_side_state[0].elapsed_lines;
        }
        if (win_sides.f.selection == 0) {
            _builtin_set_imask(line_irq_imask);
            return;
        }
    }
    _builtin_set_imask(line_irq_imask);
    if (state->elapsed_lines == timing->start_line) {
        if (win_sides.f.selection == 1) {
            line_start_capture(0);
        }
        line_start_capture(side);
    } else {
        timing->lines++;
    }
    if (!hw_hostif_flags.f.own_hostif) {
        if (win_params.flags.f.adf) {
            if (win_params.paper_size.f.auto_length) {
                if (state->state_flags.v & 0x100) {
                    if (side == 0) {
                        if (state->top_edge_lines >= timing->end_line) {
                            line_end_side(side);
                            if (line_front_length_lines == 0xffff) {
                                line_front_length_lines = state->elapsed_lines - timing->start_line;
                            }
                            return;
                        }
                    } else if (state->elapsed_lines > timing->start_line) {
                        length_lines = state->elapsed_lines - timing->start_line;
                        if (length_lines >= line_front_length_lines) {
                            line_end_side(side);
                            return;
                        }
                    }
                }
            } else if (state->state_flags.v & 0x100) {
                line_disable_capture(side);
            }
        }
        if (state->elapsed_lines < timing->start_line) {
            return;
        }
        if (--state->capture_lines == 0) {
            if (win_params.flags.f.adf && win_params.paper_size.f.auto_length) {
                state->capture_lines = 0xffff;
            } else {
                line_end_side(side);
            }
        }
    } else {
        if (job_long_page_flags & 0x80) {
            _builtin_set_imask(15);
            if ((job_long_page_flags & 1) && (jpeg_scan_flags.v & JPEG_SCAN_STOPPED)) {
                _builtin_set_imask(line_irq_imask);
                edge_end_long_page(side, line_irq_imask);
                if (win_sides.f.selection == 2) {
                    if (side == 0) {
                        edge_end_long_page(1, line_irq_imask);
                    } else {
                        edge_end_long_page(0, line_irq_imask);
                    }
                }
                _builtin_set_imask(15);
                job_long_page_flags &= 0x80;
                job_long_page_flags |= 2;
                tick_line_event_ticks.v = 0;
            } else if ((job_long_page_flags & 2) && (jpeg_scan_flags.v & JPEG_SCAN_STOPPED)) {
                while (1) {
                    lines_done = p_pisces_pass_dest[side]->lines_done;
                    if (lines_done == p_pisces_pass_dest[side]->lines_done) {
                        break;
                    }
                }
                if (lines_done >= scanning_page->sides[side].length_lines) {
                    line_page_state[side].state_flags.v = 0;
                    line_page_state[side].end_line = 0xffff;
                    line_long_page_state = 0;
                    line_stop_capture(side);
                    line_side_state[side].state_flags.v = 0;
                }
            }
            _builtin_set_imask(line_irq_imask);
        }
        if (jpeg_scan_flags.v & JPEG_SCAN_LONG_PAGE) {
            if (jpeg_scan_flags.v & JPEG_SCAN_PAUSE_WANTED) {
                jpeg_scan_flags.v &= JPEG_SCAN_LONG_PAGE;
                jpeg_scan_flags.v |= JPEG_SCAN_PAUSED;
                line_capture_wait_count.v = 0;
                if (win_params.flags.f.adf) {
                    line_gap_steps_left[0] = line_gap_steps_left_back[0] = line_gap_count_first;
                    line_gap_lines_left[0] = line_gap_lines_left_back[0] = line_gap_lines_first;
                    line_gap_step_ptr[0] = line_gap_step_ptr_back[0] = line_gap_table_first.v;
                    _builtin_set_imask(15);
                    p_vdcc_front_counter[0]->counter_ctrl.front_run = 0;
                    p_vdcc_front_counter[0]->counter_ctrl.front_clear = 1;
                    _builtin_set_imask(line_irq_imask);
                    while (p_vdcc_front_counter[0]->counter_ctrl.front_clear) {
                    }
                    _builtin_set_imask(15);
                    p_vdcc_front_counter[0]->counter_ctrl.front_run = 1;
                    _builtin_set_imask(line_irq_imask);
                    line_gap_counter[0] = line_gap_counter_back[0] = 0;
                }
                mdc_request_capture(0);
            } else if (jpeg_scan_flags.v & JPEG_SCAN_PAUSED) {
                if (jpeg_scan_flags.v & JPEG_SCAN_STOPPED) {
                    line_set_line_gate(side, 0);
                    return;
                }
                if (mdc_mailbox.capture_flags.v & MDC_CAPTURE_STEPS_VALID) {
                    if (!win_params.flags.f.adf) {
                        jpeg_scan_flags.v |= JPEG_SCAN_STOPPED;
                        line_capture_steps = mdc_mailbox.capture_steps_low.v | (mdc_mailbox.capture_steps_high << 8);
                        line_set_line_gate(side, 0);
                        return;
                    }
                    if (!line_count_down_gap(side)) {
                        line_set_line_gate(side, 0);
                        return;
                    }
                    if (line_gap_steps_left[side] == 0) {
                        line_gap_steps_left[0] = line_gap_steps_left_back[0] = 0;
                        jpeg_scan_flags.v |= JPEG_SCAN_STOPPED;
                    }
                } else if (mdc_mailbox.capture_flags.v & MDC_CAPTURE_PAUSED) {
                    if (win_params.flags.f.adf) {
                        if (!line_count_down_gap(side)) {
                            line_set_line_gate(side, 0);
                            return;
                        }
                        if (line_gap_steps_left[side] == 0) {
                            line_gap_steps_left[0] = line_gap_steps_left_back[0] = 0;
                            jpeg_scan_flags.v |= JPEG_SCAN_STOPPED;
                        }
                    } else {
                        line_set_line_gate(side, 0);
                        return;
                    }
                } else {
                    line_capture_wait_count.v++;
                    if (line_capture_wait_count.v >= 30) {
                        jpeg_scan_flags.v |= JPEG_SCAN_STOPPED;
                    }
                }
            } else if (jpeg_scan_flags.v & JPEG_SCAN_RESUMING) {
                if (win_params.flags.f.adf) {
                    if (!line_count_down_gap(side)) {
                        line_set_line_gate(side, 0);
                        if (line_gap_steps_left[side] == 0) {
                            line_gap_steps_left[0] = line_gap_steps_left_back[0] = 0;
                            jpeg_scan_flags.v &= JPEG_SCAN_LONG_PAGE;
                            _builtin_set_imask(15);
                            p_vdcc_front_counter[0]->counter_ctrl.front_run = 0;
                            _builtin_set_imask(line_irq_imask);
                        }
                        return;
                    }
                    if (line_gap_steps_left[side] == 0) {
                        line_gap_steps_left[0] = line_gap_steps_left_back[0] = 0;
                        jpeg_scan_flags.v &= JPEG_SCAN_LONG_PAGE;
                        _builtin_set_imask(15);
                        p_vdcc_front_counter[0]->counter_ctrl.front_run = 0;
                        _builtin_set_imask(line_irq_imask);
                    }
                } else {
                    _builtin_set_imask(15);
                    counter_steps = ((volatile struct vdcc_counter_regs *)p_vdcc_back_counter[0])->step_count;
                    _builtin_set_imask(line_irq_imask);
                    if (counter_steps >= 1) {
                        counter_steps -= 1;
                    } else {
                        counter_steps = 0;
                    }
                    line_capture_steps = mdc_mailbox.capture_steps_low.v | (mdc_mailbox.capture_steps_high << 8);
                    if (!(mdc_mailbox.capture_flags.v & (MDC_CAPTURE_PAUSED | MDC_CAPTURE_STEPS_VALID))) {
                        counter_steps++;
                    } else {
                        counter_steps++;
                        counter_steps--;
                    }
                    if (counter_steps < line_capture_steps) {
                        if (tick_capture_wait_ticks.v >= 0x9c4) {
                        } else {
                            line_set_line_gate(side, 0);
                            return;
                        }
                    }
                    jpeg_scan_flags.v &= JPEG_SCAN_LONG_PAGE;
                    _builtin_set_imask(15);
                    ((volatile struct vdcc_counter_regs *)p_vdcc_back_counter[0])->counter_ctrl.back_run = 0;
                    _builtin_set_imask(line_irq_imask);
                }
            }
            line_set_line_gate(side, 1);
        }
    }
}

void line_end_side(int side)
{
    if (!hw_hostif_flags.f.own_hostif && !(maint_mode_word.v & 0x4000)) {
        line_stop_capture(side);
    }
    if (!win_params.flags.f.adf) {
        mdc_request_stop();
    }
    _builtin_set_imask(15);
    line_side_state[side].capture_lines = 0;
    line_side_state[side].state_flags.v = 0;
    _builtin_set_imask(line_irq_imask);
}

void line_start_capture(int side)
{
    line_enable_capture(side);
    _builtin_set_imask(15);
    if (!p_pisces_mode[side]->run.f.capture_on) {
        p_pisces_mode[side]->run.f.capture_on = 1;
    }
    p_vdcc_capture[side]->capture_ctrl.b5 = 1;
    if ((job_long_page_flags & 0x80) && (job_long_page_flags & 4)) {
        p_vdcc_capture[side]->capture_ctrl.line_gate = 0;
    } else {
        p_vdcc_capture[side]->capture_ctrl.line_gate = 1;
    }
    p_vdcc_capture[side]->capture_ctrl.capture_run = 1;
    _builtin_set_imask(line_irq_imask);
}

void line_stop_capture(int side)
{
    p_vdcc_capture[side]->capture_ctrl.capture_run = 0;
}

void line_set_line_gate(int unused_side, int on)
{
    _builtin_set_imask(15);
    if (on) {
        p_vdcc_capture[0]->capture_ctrl.line_gate = 1;
        p_vdcc_capture[1]->capture_ctrl.line_gate = 1;
    } else {
        p_vdcc_capture[0]->capture_ctrl.line_gate = 0;
        p_vdcc_capture[1]->capture_ctrl.line_gate = 0;
    }
    _builtin_set_imask(line_irq_imask);
}

void line_enable_capture(int side)
{
    p_vdcc_capture[side]->capture_ctrl.output_enable = 1;
}

void line_disable_capture(int side)
{
    if (job_auto_size_flags.b6 || job_auto_size_flags.b7) {
        return;
    }
    if (settings_byte_0x34[0] & 1) {
        return;
    }
    if (!hw_hostif_flags.f.own_hostif) {
        p_vdcc_capture[side]->capture_ctrl.output_enable = 0;
    }
}

void line_run_mdc_requests(void)
{
    unsigned short busy;

    _builtin_set_imask(15);
    busy = mdc_busy.w;
    _builtin_set_imask(line_irq_imask);
    if (busy & 1) {
        line_send_feed();
    } else if (busy & 2) {
        line_send_scan_move();
    }
}

void line_send_feed(void)
{
    if (mdc_cmd_posted.f.posted) {
        if (!mdc_poll_result()) {
            _builtin_set_imask(line_irq_imask);
            return;
        }
        _builtin_set_imask(15);
        mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
        mdc_cmd_posted.f.posted = 0;
        mdc_busy.f.feed = 0;
        mdc_queued_cmd = 0;
        _builtin_set_imask(line_irq_imask);
        return;
    } else if (mdc_mailbox.cmd_state != MDC_CMD_STATE_IDLE) {
        _builtin_set_imask(15);
        if (mdc_mailbox.cmd_state == MDC_CMD_STATE_DONE) {
            mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
        }
        _builtin_set_imask(line_irq_imask);
        return;
    }
    mdc_send_command(MDC_CMD_FEED);
}

void line_send_scan_move(void)
{
    if (mdc_cmd_posted.f.posted) {
        if (!mdc_poll_result()) {
            return;
        }
        _builtin_set_imask(15);
        if (mdc_cmd_posted.c.last_result == MDC_RESULT_FRONT_START) {
            line_side_state[0].state_flags.f.page_state = 3;
        } else if (mdc_cmd_posted.c.last_result == MDC_RESULT_BACK_START) {
            line_side_state[1].state_flags.f.page_state = 3;
        } else if (mdc_cmd_posted.c.last_result == MDC_RESULT_ERROR || mdc_cmd_posted.c.last_result == MDC_RESULT_TIMEOUT) {
            mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
            mdc_cmd_posted.f.posted = 0;
            mdc_busy.f.scan_move = 0;
            mdc_queued_cmd = 0;
            _builtin_set_imask(line_irq_imask);
            return;
        }
        _builtin_set_imask(line_irq_imask);
        if (mdc_mailbox.cmd_state == MDC_CMD_STATE_DONE) {
            _builtin_set_imask(15);
            mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
            mdc_cmd_posted.f.posted = 0;
            mdc_busy.f.scan_move = 0;
            mdc_queued_cmd = 0;
            _builtin_set_imask(line_irq_imask);
            return;
        } else if (mdc_mailbox.cmd_state == MDC_CMD_STATE_ENDED) {
            _builtin_set_imask(line_irq_imask);
            return;
        } else {
            _builtin_set_imask(line_irq_imask);
            return;
        }
    } else if (mdc_mailbox.cmd_state != MDC_CMD_STATE_IDLE) {
        if (mdc_mailbox.cmd_state == MDC_CMD_STATE_DONE) {
            mdc_mailbox.cmd_state = MDC_CMD_STATE_IDLE;
        }
        _builtin_set_imask(line_irq_imask);
        return;
    } else {
        mdc_send_command(MDC_CMD_SCAN_MOVE);
    }
}

void line_count_mdc_waits(void)
{
    if (line_motion_flags & 0x800) {
        if (!(mdc_mailbox.motion_flags & MDC_MOTION_FRONT)) {
            if (!(line_motion_stop_lines[0] & 0x8000)) {
                _builtin_set_imask(15);
                line_motion_stop_lines[0]++;
                _builtin_set_imask(line_irq_imask);
            } else {
                _builtin_set_imask(15);
                line_motion_flags &= 0xf7ff;
                _builtin_set_imask(line_irq_imask);
            }
        }
    }
    if (line_motion_flags & 0x400) {
        if (!(mdc_mailbox.motion_flags & MDC_MOTION_BACK)) {
            if (!(line_motion_stop_lines_back[0] & 0x8000)) {
                _builtin_set_imask(15);
                line_motion_stop_lines_back[0]++;
                _builtin_set_imask(line_irq_imask);
            } else {
                _builtin_set_imask(15);
                line_motion_flags &= 0xfbff;
                _builtin_set_imask(line_irq_imask);
            }
        }
    }
    if (line_motion_flags & 0x8000) {
        if (!(mdc_mailbox.motion_flags & (MDC_MOTION_FRONT | MDC_MOTION_BACK | MDC_MOTION_BACK_PENDING))) {
            _builtin_set_imask(15);
            line_motion_flags &= 0x7fff;
            line_motion_flags |= 1;
            _builtin_set_imask(line_irq_imask);
        }
    }
}

void line_restart_capture(int side)
{
    struct line_side_record *state;
    struct line_timing *timing;

    state = &line_side_state[side];
    timing = &line_page_timing[side];
    if (win_sides.f.selection == 1) {
        line_start_capture(0);
    }
    line_start_capture(side);
    jpeg_resume_capture(line_irq_imask);
    job_long_page_flags &= 0x80;
    timing->start_line = 0;
    timing->lines = 0;
    state->elapsed_lines = 0;
    state->top_edge_lines = 0;
    if (img_side_params[0].y_res_dpi == 300) {
        line_gap_count_first = 27;
    }
}

extern void _builtin_set_imask(int);


void calib_handle_line(int side)
{
    int u1;
    int u2;
    int x;

    x = 0;
    if (img_overscan_shift_on[side]) {
        calib_follow_start_lines = img_follow_start_lines[side].v;
    }
    line_side_state[side].follow_lines++;
    if (line_page_timing[side].lines < calib_follow_start_lines) {
        line_side_state[side].follow_lines = 0;
    }
    _builtin_set_imask(15);
    if (line_page_timing[side].lines == calib_follow_start_lines) {
        line_side_state[side].follow_lines = 10;
        p_vdcc_front_correction[side]->reg_032.hi = 8;
        p_vdcc_front_correction[side]->reg_032.lo = 0;
        p_vdcc_front_correction[side]->reg_02c.hi = 8;
        p_vdcc_front_correction[side]->reg_02c.lo = 0;
        p_vdcc_front_correction[side]->reg_02e.hi = 7;
        p_vdcc_front_correction[side]->reg_02e.lo = 0xc3;
        p_vdcc_front_correction[side]->reg_030.hi = 2;
        p_vdcc_front_correction[side]->reg_030.lo = 0xcd;
        p_vdcc_measure[side]->reg_04c.hi = 4;
        p_vdcc_measure[side]->reg_04c.lo = 0;
        p_vdcc_measure[side]->reg_04e.hi = 2;
        p_vdcc_measure[side]->reg_04e.lo = 0;
        p_vdcc_front_correction[side]->mode_chip0 = p_vdcc_front_correction[side]->mode_chip1 = p_vdcc_front_correction[side]->mode_chip2 = CALIB_CORRECTION_ON;
    } else if (line_side_state[side].follow_lines == 16) {
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_0 = 16;
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_1 = 16;
        p_vdcc_front_correction[side]->follow_gain_2 = 16;
        p_vdcc_front_correction[side]->follow_gain_3 = 16;
        p_vdcc_front_correction[side]->follow_gain_4 = 4;
        p_vdcc_front_correction[side]->follow_gain_5 = 4;
        p_vdcc_front_correction[side]->follow_gain_7 = 4;
        p_vdcc_front_correction[side]->follow_gain_6 = 4;
        p_vdcc_front_correction[side]->follow_gain_9 = 4;
        p_vdcc_front_correction[side]->reg_015.b6 = 1;
        p_vdcc_front_correction[side]->reg_015.b5 = 1;
        p_vdcc_front_correction[side]->reg_015.b4 = 1;
        p_vdcc_front_correction[side]->reg_015.b2 = 1;
        p_vdcc_front_correction[side]->reg_015.b1 = 1;
        p_vdcc_front_correction[side]->reg_015.b0 = 1;
        p_vdcc_front_correction[side]->mode_chip0 = p_vdcc_front_correction[side]->mode_chip1 = p_vdcc_front_correction[side]->mode_chip2 = 0xd0;
    } else if (line_side_state[side].follow_lines == 43) {
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_0 = 4;
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_1 = 4;
        p_vdcc_front_correction[side]->follow_gain_2 = 4;
        p_vdcc_front_correction[side]->follow_gain_3 = 4;
        p_vdcc_front_correction[side]->follow_gain_4 = 4;
        p_vdcc_front_correction[side]->follow_gain_5 = 4;
        p_vdcc_front_correction[side]->follow_gain_7 = 4;
        p_vdcc_front_correction[side]->follow_gain_6 = 4;
        p_vdcc_front_correction[side]->follow_gain_9 = 4;
        p_vdcc_front_correction[side]->reg_015.b6 = 1;
        p_vdcc_front_correction[side]->reg_015.b5 = 1;
        p_vdcc_front_correction[side]->reg_015.b4 = 1;
        p_vdcc_front_correction[side]->reg_015.b2 = 1;
        p_vdcc_front_correction[side]->reg_015.b1 = 1;
        p_vdcc_front_correction[side]->reg_015.b0 = 1;
        p_vdcc_front_correction[side]->mode_chip0 = p_vdcc_front_correction[side]->mode_chip1 = p_vdcc_front_correction[side]->mode_chip2 = 0xd0;
    } else if (line_side_state[side].follow_lines == 54) {
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_0 = 32;
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_1 = 32;
        p_vdcc_front_correction[side]->follow_gain_2 = 16;
        p_vdcc_front_correction[side]->follow_gain_3 = 16;
        p_vdcc_front_correction[side]->follow_gain_4 = 1;
        p_vdcc_front_correction[side]->follow_gain_5 = 1;
        p_vdcc_front_correction[side]->follow_gain_7 = 1;
        p_vdcc_front_correction[side]->follow_gain_6 = 1;
        p_vdcc_front_correction[side]->follow_gain_9 = 1;
        p_vdcc_front_correction[side]->reg_015.b6 = 0;
        p_vdcc_front_correction[side]->reg_015.b5 = 0;
        p_vdcc_front_correction[side]->reg_015.b4 = 0;
        p_vdcc_front_correction[side]->reg_015.b2 = 1;
        p_vdcc_front_correction[side]->reg_015.b1 = 1;
        p_vdcc_front_correction[side]->reg_015.b0 = 0;
        p_vdcc_front_correction[side]->mode_chip0 = p_vdcc_front_correction[side]->mode_chip1 = p_vdcc_front_correction[side]->mode_chip2 = 0xd0;
    } else if (line_side_state[side].follow_lines == 56) {
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_0 = 16;
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_1 = 16;
        p_vdcc_front_correction[side]->follow_gain_2 = 8;
        p_vdcc_front_correction[side]->follow_gain_3 = 8;
        p_vdcc_front_correction[side]->follow_gain_4 = 1;
        p_vdcc_front_correction[side]->follow_gain_5 = 1;
        p_vdcc_front_correction[side]->follow_gain_7 = 1;
        p_vdcc_front_correction[side]->follow_gain_6 = 1;
        p_vdcc_front_correction[side]->follow_gain_9 = 1;
        p_vdcc_front_correction[side]->reg_015.b6 = 0;
        p_vdcc_front_correction[side]->reg_015.b5 = 0;
        p_vdcc_front_correction[side]->reg_015.b4 = 0;
        p_vdcc_front_correction[side]->reg_015.b2 = 1;
        p_vdcc_front_correction[side]->reg_015.b1 = 1;
        p_vdcc_front_correction[side]->reg_015.b0 = 0;
        p_vdcc_front_correction[side]->mode_chip0 = p_vdcc_front_correction[side]->mode_chip1 = p_vdcc_front_correction[side]->mode_chip2 = 0xd0;
    } else if (line_side_state[side].follow_lines == 58) {
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_0 = 8;
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_1 = 8;
        p_vdcc_front_correction[side]->follow_gain_2 = 2;
        p_vdcc_front_correction[side]->follow_gain_3 = 2;
        p_vdcc_front_correction[side]->follow_gain_4 = 1;
        p_vdcc_front_correction[side]->follow_gain_5 = 1;
        p_vdcc_front_correction[side]->follow_gain_7 = 1;
        p_vdcc_front_correction[side]->follow_gain_6 = 1;
        p_vdcc_front_correction[side]->follow_gain_9 = 1;
        p_vdcc_front_correction[side]->reg_015.b6 = 0;
        p_vdcc_front_correction[side]->reg_015.b5 = 0;
        p_vdcc_front_correction[side]->reg_015.b4 = 0;
        p_vdcc_front_correction[side]->reg_015.b2 = 1;
        p_vdcc_front_correction[side]->reg_015.b1 = 1;
        p_vdcc_front_correction[side]->reg_015.b0 = 0;
        p_vdcc_front_correction[side]->mode_chip0 = p_vdcc_front_correction[side]->mode_chip1 = p_vdcc_front_correction[side]->mode_chip2 = 0xd0;
    } else if (line_side_state[side].follow_lines == 60) {
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_0 = 4;
        p_vdcc_front_correction[side]->follow_gain_0_1.c.follow_gain_1 = 4;
        p_vdcc_front_correction[side]->follow_gain_2 = 1;
        p_vdcc_front_correction[side]->follow_gain_3 = 1;
        p_vdcc_front_correction[side]->follow_gain_4 = 1;
        p_vdcc_front_correction[side]->follow_gain_5 = 1;
        p_vdcc_front_correction[side]->follow_gain_7 = 1;
        p_vdcc_front_correction[side]->follow_gain_6 = 1;
        p_vdcc_front_correction[side]->follow_gain_9 = 1;
        p_vdcc_front_correction[side]->reg_015.b6 = 0;
        p_vdcc_front_correction[side]->reg_015.b5 = 0;
        p_vdcc_front_correction[side]->reg_015.b4 = 0;
        p_vdcc_front_correction[side]->reg_015.b2 = 1;
        p_vdcc_front_correction[side]->reg_015.b1 = 1;
        p_vdcc_front_correction[side]->reg_015.b0 = 0;
        p_vdcc_front_correction[side]->mode_chip0 = p_vdcc_front_correction[side]->mode_chip1 = p_vdcc_front_correction[side]->mode_chip2 = 0xd0;
    } else if (line_side_state[side].follow_lines == 55 || line_side_state[side].follow_lines == 57 || line_side_state[side].follow_lines == 59
               || line_side_state[side].follow_lines == 61 || (line_side_state[side].follow_lines >= 61 && line_side_state[side].follow_lines >= 85)) {
        p_vdcc_front_correction[side]->mode_chip0 = p_vdcc_front_correction[side]->mode_chip1 = p_vdcc_front_correction[side]->mode_chip2 = CALIB_CORRECTION_ON;
        if (line_side_state[side].follow_lines == 85) {
            line_side_state[side].follow_lines = 53;
        }
    }
    _builtin_set_imask(line_irq_imask);
    return;
}


char calib_run_sequence(void)
{
    int u1, u2, u3, u4, u5;
    unsigned short u6;
    unsigned char error;

    _builtin_set_imask(15);
    line_side_state[0].state_flags.v = 0;
    line_side_state[1].state_flags.v = 0;
    _builtin_set_imask(0);
    if ((settings_test_flags[0] & 0x10) || (settings_test_flags[0] & 2))
        return 0;
    if (calib_levels_deferred) {
        if (calib_levels_request) {
            _builtin_set_imask(15);
            calib_levels_due = 1;
            _builtin_set_imask(0);
            return 0;
        }
    }
    _builtin_set_imask(15);
    calib_levels_request = 0;
    _builtin_set_imask(0);
    afe_set_gain(0, 0, 0, 0x80);
    afe_set_gain(0, 0, 1, 0x80);
    afe_set_gain(0, 1, 0, 0x80);
    afe_set_gain(0, 1, 1, 0x80);
    afe_set_gain(0, 2, 0, 0x80);
    afe_set_gain(0, 2, 1, 0x80);
    afe_set_gain(1, 0, 0, 0x80);
    afe_set_gain(1, 0, 1, 0x80);
    afe_set_gain(1, 1, 0, 0x80);
    afe_set_gain(1, 1, 1, 0x80);
    afe_set_gain(1, 2, 0, 0x80);
    afe_set_gain(1, 2, 1, 0x80);
    calib_offset_target_front.v = 15;
    calib_offset_target_2_front = 50;
    calib_offset_target_3_front = 0;
    calib_offset_target_back.v = 15;
    calib_offset_target_2_back = 50;
    calib_offset_target_3_back = 0;
    calib_gain_target_front = 120;
    calib_gain_target_2_front = 100;
    calib_gain_target_back = 120;
    calib_gain_target_2_back = 100;
    if (!win_params.flags.f.adf) {
        if (calib_flatbed_high_target)
            calib_gain_target_front = 200;
        else
            calib_gain_target_front = 160;
        calib_gain_target_2_front = 100;
    }
    if (settings_offset_target_front[0] != 0)
        calib_offset_target_front.v = settings_offset_target_front[0];
    if (settings_offset_target_back[0] != 0)
        calib_offset_target_back.v = settings_offset_target_back[0];
    if (settings_gain_target_front[0] != 0)
        calib_gain_target_front = settings_gain_target_front[0];
    if (settings_gain_target_back[0] != 0)
        calib_gain_target_back = settings_gain_target_back[0];
    if (win_params.flags.f.adf) {
        error = calib_start_offset(3);
        if (win_sides.f.selection == 0 && error == 117)
            error = 0;
        if (error)
            return error;
        error = calib_start_gain(3);
        if (win_sides.f.selection == 0 && error == 117)
            error = 0;
        if (error)
            return error;
    } else {
        if ((error = calib_start_offset(1)) != 0)
            return error;
        if ((error = calib_start_gain(1)) != 0)
            return error;
    }
    if (img_side_params[0].ccd_300dpi_mode)
        p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 1;
    if (win_params.flags.f.adf)
        calib_start_apply(3);
    else
        calib_start_apply(1);
    calib_start_black();
    if (img_side_params[0].ccd_300dpi_mode)
        p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 1;
    return 0;
}

int calib_load_white_level()
{
    char sides;
    int u;
    int side;
    int chip;
    unsigned char index;

    if ((job_long_page_flags & 0x80) && win_white_level_follow != 0xc0) {
        calib_load_white_long_page();
        return;
    }
    if (settings_test_flags[0] & 16 || settings_test_flags[0] & 2) {
        return;
    }
    if (job_adf_sheet_count == 0) {
    } else {
        return;
    }
    if (win_params.flags.f.adf) {
        sides = 3;
    } else {
        sides = 1;
    }
    calib_white_level_fine[0][0] = 0;
    calib_white_level_fine[0][1] = 0;
    calib_white_level_fine[0][2] = 0;
    calib_white_level_fine[1][0] = 0;
    calib_white_level_fine[1][1] = 0;
    calib_white_level_fine[1][2] = 0;
    if (!win_params.flags.f.adf) {
        calib_white_level[0][0] = settings_white_level_flatbed_chip0[0];
        calib_white_level[0][1] = settings_white_level_flatbed_chip1[0];
        calib_white_level[0][2] = settings_white_level_flatbed_chip2[0];
        calib_white_level[1][0] = 0;
        calib_white_level[1][1] = 0;
        calib_white_level[1][2] = 0;
        for (chip = 0; chip <= 2; chip++) {
            if (chip == 0) {
                index = 0xf5;
            } else if (chip == 1) {
                index = 0xf6;
            } else {
                index = 0xf7;
            }
            if (settings[index] & 1) {
                index = calib_white_level[0][chip];
                if (index > 128) {
                    index = 128;
                }
                calib_white_level[0][chip] = calib_white_factor_pairs[index][0];
                calib_white_level_fine[0][chip] = calib_white_factor_pairs[index][1];
            } else if (settings[index] & 0x80) {
                index = calib_white_level[0][chip];
                if (index > 64) {
                    index = 64;
                }
                calib_white_level[0][chip] = calib_white_factor_single[index][0];
                calib_white_level_fine[0][chip] = calib_white_factor_single[index][1];
            }
        }
    } else {
        calib_white_level[0][0] = settings_white_level_adf_front_chip0[0];
        calib_white_level[0][1] = settings_white_level_adf_front_chip1[0];
        calib_white_level[0][2] = settings_white_level_adf_front_chip2[0];
        calib_white_level[1][0] = settings_white_level_adf_back_chip0[0];
        calib_white_level[1][1] = settings_white_level_adf_back_chip1[0];
        calib_white_level[1][2] = settings_white_level_adf_back_chip2[0];
        for (side = 0; side <= 1; side++) {
            for (chip = 0; chip <= 2; chip++) {
                if (side == 0) {
                    if (chip == 0) {
                        index = 0xf8;
                    } else if (chip == 1) {
                        index = 0xf9;
                    } else {
                        index = 0xfa;
                    }
                } else if (chip == 0) {
                    index = 0xfb;
                } else if (chip == 1) {
                    index = 0xfc;
                } else {
                    index = 0xfd;
                }
                if (settings[index] & 1) {
                    index = calib_white_level[side][chip];
                    if (index > 128) {
                        index = 128;
                    }
                    calib_white_level[side][chip] = calib_white_factor_pairs[index][0];
                    calib_white_level_fine[side][chip] = calib_white_factor_pairs[index][1];
                } else if (settings[index] & 0x80) {
                    index = calib_white_level[side][chip];
                    if (index > 64) {
                        index = 64;
                    }
                    calib_white_level[side][chip] = calib_white_factor_single[index][0];
                    calib_white_level_fine[side][chip] = calib_white_factor_single[index][1];
                }
            }
        }
    }
    calib_start_lamp_adjust(sides);
    calib_white_level_fine[0][0] = 0;
    calib_white_level_fine[0][1] = 0;
    calib_white_level_fine[0][2] = 0;
    calib_white_level_fine[1][0] = 0;
    calib_white_level_fine[1][1] = 0;
    calib_white_level_fine[1][2] = 0;
    calib_start_lamp_11(sides, 1);
    return;
}

int calib_check_lamp_exposure(void)
{
    unsigned long level;
    unsigned long ratio;
    unsigned long scaled_exposure;
    unsigned long target_level;
    unsigned long measured_level;
    int side;
    int chip;
    int drifted[2][3];
    int any_drifted;
    int brighter;
    int skip_back;
    unsigned short drift;
    unsigned short value;
    unsigned char setting;
    short u4;
    int error;

    any_drifted = 0;
    _builtin_set_imask(15);
    line_side_state[0].state_flags.v = 0;
    line_side_state[1].state_flags.v = 0;
    _builtin_set_imask(0);
    if ((settings_test_flags[0] & 0x10) || (settings_test_flags[0] & 2)) {
        _builtin_set_imask(15);
        calib_exposure_request = 0;
        _builtin_set_imask(0);
        return 0;
    }
    if (settings_calib_options[0] & 8) {
        _builtin_set_imask(15);
        calib_exposure_request = 0;
        _builtin_set_imask(0);
        return 0;
    }
    if ((job_long_page_flags & 0x80) && win_white_level_follow != 0xc0) {
        _builtin_set_imask(15);
        calib_exposure_request = 0;
        _builtin_set_imask(0);
        return 0;
    }
    if (calib_levels_deferred) {
        _builtin_set_imask(15);
        calib_exposure_request = 0;
        calib_levels_deferred = 0;
        _builtin_set_imask(0);
        return 0;
    }
    if (!job_adf_sheet_count) {
        _builtin_set_imask(15);
        calib_exposure_request = 1;
        _builtin_set_imask(0);
    }
    if (calib_extrapolate_request) {
        if (!calib_exposure_history_on)
            return 0;
        if ((error = calib_exposure_extrapolate()) != 0)
            return 0;
    } else {
        if (!calib_exposure_request)
            return 0;
        if (win_params.flags.f.adf && mdc_is_adf_cover_open()) {
            err_raise(ERR_ADF_COVER_OPEN);
            return 1;
        }
        if (job_black_backing && !mdc_run_backing(0))
            return 1;
        calib_exposure[0][0] = 0;
        calib_exposure[0][1] = 0;
        calib_exposure[0][2] = 0;
        calib_exposure[1][0] = 0;
        calib_exposure[1][1] = 0;
        calib_exposure[1][2] = 0;
        if (!job_adf_sheet_count && !calib_in_lamp_check) {
            calib_exposure_w4[0][0] = 0;
            calib_exposure_w4[0][1] = 0;
            calib_exposure_w4[0][2] = 0;
            calib_exposure_w4[1][0] = 0;
            calib_exposure_w4[1][1] = 0;
            calib_exposure_w4[1][2] = 0;
        }
        calib_start_lamp_check(3);
        _builtin_set_imask(15);
        calib_exposure_request = 0;
        _builtin_set_imask(0);
        if (!job_adf_sheet_count && !calib_in_lamp_check) {
            calib_exposure_calibrated[0][0] = calib_exposure_reference[0][0] = calib_exposure[0][0];
            calib_exposure_calibrated[0][1] = calib_exposure_reference[0][1] = calib_exposure[0][1];
            calib_exposure_calibrated[0][2] = calib_exposure_reference[0][2] = calib_exposure[0][2];
            calib_exposure_calibrated[1][0] = calib_exposure_reference[1][0] = calib_exposure[1][0];
            calib_exposure_calibrated[1][1] = calib_exposure_reference[1][1] = calib_exposure[1][1];
            calib_exposure_calibrated[1][2] = calib_exposure_reference[1][2] = calib_exposure[1][2];
            return 0;
        }
        if (calib_exposure_history_on)
            calib_exposure_history_add(calib_in_lamp_check);
    }
    drifted[0][0] = drifted[0][1] = drifted[0][2] = drifted[1][0] = drifted[1][1] = drifted[1][2] = 0;
    if (win_params.flags.f.adf && win_sides.f.selection == 0)
        skip_back = 1;
    else
        skip_back = 0;
    for (side = 0; side <= 1; side++) {
        for (chip = 0; chip <= 2; chip++) {
            if (calib_extrapolate_request) {
                any_drifted = 1;
                continue;
            }
            if (calib_exposure[side][chip] >= calib_exposure_reference[side][chip]) {
                drift = (unsigned short)calib_exposure[side][chip] - calib_exposure_reference[side][chip];
                brighter = 1;
            } else {
                drift = (unsigned short)calib_exposure_reference[side][chip] - calib_exposure[side][chip];
                brighter = 0;
            }
            if (side == 0)
                level = calib_exposure_reference[side][chip] - calib_offset_target_front.v * 4;
            else
                level = calib_exposure_reference[side][chip] - calib_offset_target_back.v * 4;
            level = level * 2;
            level = rt_divu16(level + 50, 100);
            value = level;
            if (drift >= value)
                drifted[side][chip] = 1;
            if (!win_params.flags.f.adf && side == 1)
                drifted[side][chip] = 0;
            if (skip_back && side == 1)
                drifted[side][chip] = 0;
            if (job_black_backing || calib_in_lamp_check)
                drifted[side][chip] = 1;
            if (drifted[side][chip] == 0) {
                calib_white_level[side][chip] = 0;
                continue;
            }
            any_drifted = 1;
        }
    }
    if (!any_drifted)
        return 0;
    calib_start_lamp_11(3, 0);
    for (side = 0; side <= 1; side++) {
        for (chip = 0; chip <= 2; chip++) {
            level = calib_exposure[side][chip] - calib_black_level[side][chip];
            level = level << 8;
            level = 100 * level;
            scaled_exposure = level;
            ratio = calib_exposure_calibrated[side][chip] - calib_black_level[side][chip];
            level = level / ratio;
            if (job_black_backing && calib_in_lamp_check && tick_lamp_on_ticks.v < 7500 && tick_awake_ticks.v < 7500) {
                if (calib_exposure_calibrated[side][1] > calib_exposure[side][1] + 1)
                    level = level - 400;
                else if (calib_exposure[side][1] > calib_exposure_calibrated[side][1] + 1)
                    level = level + 400;
                else {
                }
            } else if (job_black_backing && calib_lamp_relit && calib_warmup_ticks < 30000) {
                if (calib_lamp_cold) {
                    if (calib_warmup_ticks < 11000) {
                        if (calib_exposure_calibrated[side][1] > calib_exposure[side][1] + 1)
                            level = level - 500;
                        else if (calib_exposure[side][1] > calib_exposure_calibrated[side][1] + 1)
                            level = level + 200;
                    }
                } else {
                    if (calib_exposure_calibrated[side][1] > calib_exposure[side][1] + 1)
                        level = level - 500;
                    else if (calib_exposure[side][1] > calib_exposure_calibrated[side][1] + 1)
                        level = level + 500;
                    else if (calib_in_lamp_check)
                        level = level - 400;
                }
            }
            ratio <<= 8;
            ratio = scaled_exposure / ratio;
            if (!win_params.flags.f.adf) {
                if (chip == 0) {
                    value = settings_white_level_flatbed_chip0[0];
                    setting = 0xf5;
                } else if (chip == 1) {
                    value = settings_white_level_flatbed_chip1[0];
                    setting = 0xf6;
                } else {
                    value = settings_white_level_flatbed_chip2[0];
                    setting = 0xf7;
                }
                if (settings[setting] & 1)
                    value |= 0x100;
                else if (settings[setting] & 0x80) {
                }
                value = value + 0x80;
                if (calib_flatbed_high_target)
                    target_level = 200 - calib_offset_target_front.v;
                else
                    target_level = 160 - calib_offset_target_front.v;
                measured_level = calib_exposure_w4[side][chip] - calib_offset_target_front.v;
                target_level = value * ratio * target_level;
                target_level = target_level >> 8;
                target_level += calib_black_level[side][chip] * 100;
                measured_level = value * ratio * measured_level;
                measured_level = measured_level >> 8;
                measured_level += calib_black_level[side][chip] * 100;
                if (target_level >= 23000 && side == 0)
                    return 2;
                if (measured_level <= 6500 && side == 0)
                    return 2;
            } else {
                if (side == 0) {
                    if (chip == 0) {
                        value = settings_white_level_adf_front_chip0[0];
                        setting = 0xf8;
                    } else if (chip == 1) {
                        value = settings_white_level_adf_front_chip1[0];
                        setting = 0xf9;
                    } else {
                        value = settings_white_level_adf_front_chip2[0];
                        setting = 0xfa;
                    }
                    if (settings[setting] & 1)
                        value |= 0x100;
                    else if (settings[setting] & 0x80) {
                    }
                    value = value + 0x80;
                    target_level = 120 - calib_offset_target_front.v;
                    measured_level = calib_exposure_w4[side][chip] - calib_offset_target_front.v;
                } else {
                    if (chip == 0) {
                        value = settings_white_level_adf_back_chip0[0];
                        setting = 0xfb;
                    } else if (chip == 1) {
                        value = settings_white_level_adf_back_chip1[0];
                        setting = 0xfc;
                    } else {
                        value = settings_white_level_adf_back_chip2[0];
                        setting = 0xfd;
                    }
                    if (settings[setting] & 1)
                        value |= 0x100;
                    else if (settings[setting] & 0x80) {
                    }
                    value = value + 0x80;
                    target_level = 120 - calib_offset_target_back.v;
                    measured_level = calib_exposure_w4[side][chip] - calib_offset_target_back.v;
                }
                target_level = value * ratio * target_level;
                target_level = target_level >> 8;
                target_level += calib_black_level[side][chip] * 100;
                measured_level = value * ratio * measured_level;
                measured_level = measured_level >> 8;
                measured_level += calib_black_level[side][chip] * 100;
                if (target_level >= 23000) {
                    if (skip_back && side == 1) {
                    } else
                        return 2;
                }
                if (target_level <= 6500) {
                    if (skip_back && side == 1) {
                    } else
                        return 2;
                }
            }
            level = rt_divu32(level + 50, 100);
            if (!win_params.flags.f.adf && side == 1)
                level = 0x80;
            else if (skip_back && side == 1)
                level = 0x80;
            else {
                if (level <= 0x80)
                    return 2;
                else if (level >= 0x180)
                    return 2;
                else {
                    level -= 0x80;
                    if (1) ;
                    level = (unsigned char)level;
                }
            }
            calib_white_level[side][chip] = level;
            if (!calib_extrapolate_request)
                calib_exposure_reference[side][chip] = calib_exposure[side][chip];
        }
    }
    for (side = 0; side <= 1; side++) {
        if (calib_white_level[side][0] > calib_white_level[side][1]) {
            if (calib_white_level[side][0] > calib_white_level[side][2]) {
                if (calib_white_level[side][1] > calib_white_level[side][2]) {
                    if (calib_white_level[side][0] - calib_white_level[side][1] > 20)
                        calib_white_level[side][0] = calib_white_level[side][1];
                    if (calib_white_level[side][1] - calib_white_level[side][2] > 20)
                        calib_white_level[side][2] = calib_white_level[side][1];
                } else {
                    if (calib_white_level[side][0] - calib_white_level[side][2] > 20)
                        calib_white_level[side][0] = calib_white_level[side][2];
                    if (calib_white_level[side][2] - calib_white_level[side][1] > 20)
                        calib_white_level[side][1] = calib_white_level[side][2];
                }
            } else {
                if (calib_white_level[side][2] - calib_white_level[side][0] > 20)
                    calib_white_level[side][2] = calib_white_level[side][0];
                if (calib_white_level[side][0] - calib_white_level[side][1] > 20)
                    calib_white_level[side][1] = calib_white_level[side][0];
            }
        } else if (calib_white_level[side][1] > calib_white_level[side][2]) {
            if (calib_white_level[side][0] > calib_white_level[side][2]) {
                if (calib_white_level[side][1] - calib_white_level[side][0] > 20)
                    calib_white_level[side][1] = calib_white_level[side][0];
                if (calib_white_level[side][0] - calib_white_level[side][2] > 20)
                    calib_white_level[side][2] = calib_white_level[side][0];
            } else {
                if (calib_white_level[side][1] - calib_white_level[side][2] > 20)
                    calib_white_level[side][1] = calib_white_level[side][2];
                if (calib_white_level[side][2] - calib_white_level[side][0] > 20)
                    calib_white_level[side][0] = calib_white_level[side][2];
            }
        } else {
            if (calib_white_level[side][2] - calib_white_level[side][1] > 20)
                calib_white_level[side][2] = calib_white_level[side][1];
            if (calib_white_level[side][1] - calib_white_level[side][0] > 20)
                calib_white_level[side][0] = calib_white_level[side][1];
        }
    }
    calib_start_lamp_adjust(3);
    return 0;
}

void calib_exposure_history_add(int restart)
{
    if (restart) {
        calib_exposure_history_count = 0;
    } else {
        if (calib_exposure_history_count < 3)
            calib_exposure_history_count++;
        else
            return;
    }
    calib_exposure_history[calib_exposure_history_count].v[0][0] = calib_exposure[0][0];
    calib_exposure_history[calib_exposure_history_count].v[0][1] = calib_exposure[0][1];
    calib_exposure_history[calib_exposure_history_count].v[0][2] = calib_exposure[0][2];
    calib_exposure_history[calib_exposure_history_count].v[1][0] = calib_exposure[1][0];
    calib_exposure_history[calib_exposure_history_count].v[1][1] = calib_exposure[1][1];
    calib_exposure_history[calib_exposure_history_count].v[1][2] = calib_exposure[1][2];
    calib_exposure_history_time[calib_exposure_history_count] = calib_warmup_ticks / 50;
}

/* Extrapolates each falling exposure from the last two history entries
   (calib_exposure_history_add) to the present time; 1 when the history cannot be used. */
int calib_exposure_extrapolate(void)
{
    int side;
    int chip;
    unsigned int cur;
    unsigned int prev;
    unsigned int drop;
    int percent;
    unsigned int now;
    unsigned int time_last;
    unsigned int time_before;
    unsigned short last;

    if (calib_exposure_history_count == 0) {
        return 1;
    }
    last = calib_exposure_history_count;
    now = calib_warmup_ticks / 50;
    time_last = calib_exposure_history_time[last];
    time_before = calib_exposure_history_time[last - 1];
    if (now < time_last) {
        return 1;
    }
    if (time_last < time_before) {
        return 1;
    }
    if (calib_lamp_relit) {
        if (calib_warmup_ticks < 0x1964) {
            percent = 100;
        } else if (calib_warmup_ticks < 0x30d4) {
            percent = 70;
        } else {
            percent = 32;
        }
    } else {
        if (calib_warmup_ticks < 0x1964) {
            percent = 32;
        } else if (calib_warmup_ticks < 0x30d4) {
            percent = 80;
        } else {
            percent = 50;
        }
    }
    for (side = 0; !(side > 1); side++) {
        for (chip = 0; !(chip > 2); chip++) {
            cur = calib_exposure_history[last].v[side][chip];
            prev = calib_exposure_history[last - 1].v[side][chip];
            if (prev > cur) {
                drop = (prev - cur) * percent;
                drop = drop * (now - time_last);
                drop = drop / (time_last - time_before);
                drop = drop / 100;
                calib_exposure[side][chip] = cur - drop;
            } else {
                calib_exposure[side][chip] = cur;
            }
        }
    }
    return 0;
}

/* Constants (section C). */
void (*const calib_steps_offset[7])(void) = {
    calib_offset_step_0, calib_offset_step_1, calib_offset_step_2, calib_offset_step_3,
    calib_offset_step_4, calib_offset_step_5, calib_offset_step_6
};
void (*const calib_steps_gain[7])(void) = {
    calib_gain_step_0, calib_gain_step_1, calib_gain_step_2, calib_gain_step_3,
    calib_gain_step_4, calib_gain_step_5, calib_gain_step_6
};
void (*const calib_steps_black[4])(void) = {calib_next_step, calib_black_step_1, calib_black_step_2, calib_black_step_3};
void (*const calib_steps_apply[5])(void) = {
    calib_apply_step_0, calib_apply_step_1, calib_apply_step_2, calib_apply_step_3,
    calib_apply_step_4
};
void (*const calib_steps_lamp_check[3])(void) = {calib_next_step, calib_lamp_check_step_1, calib_lamp_check_step_2};
void (*const calib_steps_lamp_adjust[8])(void) = {
    calib_next_step, calib_lamp_adjust_step_1, calib_lamp_adjust_step_2, calib_lamp_adjust_step_3,
    calib_lamp_adjust_step_4, calib_lamp_adjust_step_5, calib_lamp_adjust_step_6, calib_lamp_adjust_step_7
};
void (*const calib_steps_lamp_11[3])(void) = {calib_next_step, calib_lamp_11_step_1, calib_lamp_11_step_2};
void (*const calib_steps_mode_12[3])(void) = {calib_next_step, calib_mode_12_step_1, calib_mode_12_step_2};
void (*const calib_steps_mode_13[3])(void) = {calib_next_step, calib_mode_13_step_1, calib_mode_13_step_2};

void calib_run_step(void)
{
    if (calib_started == 0) {
        calib_started = 1;
        calib_step = 0;
        calib_line_count = 0;
    }
    calib_step_result = CALIB_STEP_STAY;
    switch (calib_mode) {
    case CALIB_MODE_APPLY: calib_steps_apply[calib_step](); break;
    case CALIB_MODE_BLACK: calib_steps_black[calib_step](); break;
    case CALIB_MODE_OFFSET: calib_steps_offset[calib_step](); break;
    case CALIB_MODE_GAIN: calib_steps_gain[calib_step](); break;
    case CALIB_MODE_LAMP_CHECK: calib_steps_lamp_check[calib_step](); break;
    case CALIB_MODE_LAMP_ADJUST: calib_steps_lamp_adjust[calib_step](); break;
    case 11: calib_steps_lamp_11[calib_step](); break;
    case 12: calib_steps_mode_12[calib_step](); break;
    case 13: calib_steps_mode_13[calib_step](); break;
    }
    switch (calib_step_result) {
    case CALIB_STEP_STAY: break;
    case CALIB_STEP_NEXT: break;
    case CALIB_STEP_DONE:
        calib_running = 0;
        calib_started = 0;
        break;
    }
    return;
}

void calib_next_step(void)
{
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_offset_step_0(void)
{
    calib_line_count = 1;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_offset_step_1(void)
{
    short chip_bits;

    if (calib_line_count != 0) {
        calib_line_count--;
        if (calib_line_count == 0) {
            calib_step_result = CALIB_STEP_NEXT;
            calib_step++;
        }
    } else {
        if (calib_offset_done.front.odd_chip0 == 0) {
            if (calib_offset_search_odd_front.b.chip0 < 250u) {
                calib_offset_search_odd_front.f.chip0 = calib_offset_search_odd_front.b.chip0 + 1;
            } else {
                calib_offset_done.front.odd_chip0 = 1;
            }
        }
        if (calib_offset_done.front.odd_chip1 == 0) {
            if (calib_offset_search_odd_front.b.chip1 < 250u) {
                calib_offset_search_odd_front.f.chip1 = calib_offset_search_odd_front.b.chip1 + 1;
            } else {
                calib_offset_done.front.odd_chip1 = 1;
            }
        }
        if (calib_offset_done.front.odd_chip2 == 0) {
            if (calib_offset_search_odd_front.b.chip2 < 250u) {
                calib_offset_search_odd_front.f.chip2 = calib_offset_search_odd_front.b.chip2 + 1;
            } else {
                calib_offset_done.front.odd_chip2 = 1;
            }
        }
        if (calib_offset_done.back.odd_chip0 == 0) {
            if (calib_offset_search_odd_back.b.chip0 < 250u) {
                calib_offset_search_odd_back.f.chip0 = calib_offset_search_odd_back.b.chip0 + 1;
            } else {
                calib_offset_done.back.odd_chip0 = 1;
            }
        }
        if (calib_offset_done.back.odd_chip1 == 0) {
            if (calib_offset_search_odd_back.b.chip1 < 250u) {
                calib_offset_search_odd_back.f.chip1 = calib_offset_search_odd_back.b.chip1 + 1;
            } else {
                calib_offset_done.back.odd_chip1 = 1;
            }
        }
        if (calib_offset_done.back.odd_chip2 == 0) {
            if (calib_offset_search_odd_back.b.chip2 < 250u) {
                calib_offset_search_odd_back.f.chip2 = calib_offset_search_odd_back.b.chip2 + 1;
            } else {
                calib_offset_done.back.odd_chip2 = 1;
            }
        }
        chip_bits = 0;
        if ((calib_sides & 1)) {
            calib_offset_odd_front.l.v = calib_offset_search_odd_front.l.v;
            chip_bits |= 21;
        }
        if ((calib_sides & 2)) {
            calib_offset_odd_back.l.v = calib_offset_search_odd_back.l.v;
            chip_bits |= 42;
        }
        calib_write_afe_offsets(chip_bits);
        calib_line_count = 1;
        calib_step_result = CALIB_STEP_STAY;
    }
    return;
}

extern void _builtin_set_imask(int);


void calib_offset_step_2(void)
{
    char all_done;

    _builtin_set_imask(15);
    if (!calib_offset_done.front.odd_chip0) {
        p_vdcc_measure[0]->select.s.sel = 0;
        if ((unsigned)((p_vdcc_measure[0]->level)) > calib_offset_target_x4_front) {
            calib_offset_done.front.odd_chip0 = 1;
        }
    }
    if (!calib_offset_done.front.odd_chip1) {
        p_vdcc_measure[0]->select.s.sel = 1;
        if ((unsigned)((p_vdcc_measure[0]->level)) > calib_offset_target_x4_front) {
            calib_offset_done.front.odd_chip1 = 1;
        }
    }
    if (calib_offset_done.front.odd_chip2 == 0) {
        p_vdcc_measure[0]->select.s.sel = 2;
        if ((unsigned)((p_vdcc_measure[0]->level)) > calib_offset_target_x4_front) {
            calib_offset_done.front.odd_chip2 = 1;
        }
    }
    _builtin_set_imask(line_irq_imask);
    _builtin_set_imask(15);
    if (!calib_offset_done.back.odd_chip0) {
        p_vdcc_measure[1]->select.s.sel = 0;
        if ((unsigned)((p_vdcc_measure[1]->level)) > calib_offset_target_x4_back) {
            calib_offset_done.back.odd_chip0 = 1;
        }
    }
    if (!calib_offset_done.back.odd_chip1) {
        p_vdcc_measure[1]->select.s.sel = 1;
        if ((unsigned)((p_vdcc_measure[1]->level)) > calib_offset_target_x4_back) {
            calib_offset_done.back.odd_chip1 = 1;
        }
    }
    if (calib_offset_done.back.odd_chip2 == 0) {
        p_vdcc_measure[1]->select.s.sel = 2;
        if ((unsigned)((p_vdcc_measure[1]->level)) > calib_offset_target_x4_back) {
            calib_offset_done.back.odd_chip2 = 1;
        }
    }
    _builtin_set_imask(line_irq_imask);
    all_done = 0;
    if (calib_sides == 1) {
        if (calib_offset_done.u.hi == 7) {
            all_done = 1;
        }
    } else if (calib_sides == 2) {
        if (calib_offset_done.u.lo == 7) {
            all_done = 1;
        }
    } else if (calib_sides == 3) {
        if (calib_offset_done.w == 0x707) {
            all_done = 1;
        }
    }
    if (all_done == 0) {
        calib_step--;
    } else {
        if (calib_offset_search_odd_front.b.chip0 > 10u) {
            calib_offset_search_even_front.f.chip0 = calib_offset_search_odd_front.b.chip0 - 10;
        } else {
            calib_offset_search_even_front.b.chip0 = 0;
        }
        if (calib_offset_search_odd_front.b.chip1 > 10u) {
            calib_offset_search_even_front.f.chip1 = calib_offset_search_odd_front.b.chip1 - 10;
        } else {
            calib_offset_search_even_front.b.chip1 = 0;
        }
        if (calib_offset_search_odd_front.b.chip2 > 10u) {
            calib_offset_search_even_front.f.chip2 = calib_offset_search_odd_front.b.chip2 - 10;
        } else {
            calib_offset_search_even_front.b.chip2 = 0;
        }
        if (calib_offset_search_odd_back.b.chip0 > 10u) {
            calib_offset_search_even_back.f.chip0 = calib_offset_search_odd_back.b.chip0 - 10;
        } else {
            calib_offset_search_even_back.b.chip0 = 0;
        }
        if (calib_offset_search_odd_back.b.chip1 > 10u) {
            calib_offset_search_even_back.f.chip1 = calib_offset_search_odd_back.b.chip1 - 10;
        } else {
            calib_offset_search_even_back.b.chip1 = 0;
        }
        if (calib_offset_search_odd_back.b.chip2 > 10u) {
            calib_offset_search_even_back.f.chip2 = calib_offset_search_odd_back.b.chip2 - 10;
        } else {
            calib_offset_search_even_back.b.chip2 = 0;
        }
        _builtin_set_imask(15);
        p_vdcc_measure[0]->select.f.mode = 1;
        p_vdcc_measure[1]->select.f.mode = 1;
        if (*p_vdcc_timing[0] > 0x1000) {
            p_vdcc_measure[0]->select.f.mode = 2;
            p_vdcc_measure[1]->select.f.mode = 2;
            if (calib_measure_mode_swap & 0x8000) {
                p_vdcc_measure[0]->select.f.mode = 1;
            }
            if (calib_measure_mode_swap & 0x80) {
                p_vdcc_measure[1]->select.f.mode = 1;
            }
        }
        if (img_side_params[0].ccd_300dpi_mode != 0) {
            p_vdcc_front_sensor->source_gate.b.b2 = 1;
            p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
        }
        _builtin_set_imask(line_irq_imask);
        calib_step++;
        calib_line_count = 0;
    }
    calib_step_result = CALIB_STEP_NEXT;
}


void calib_offset_step_3(void)
{
    short chip_bits;

    if (calib_line_count != 0) {
        calib_line_count--;
        if (calib_line_count == 0) {
            calib_step_result = CALIB_STEP_NEXT;
            calib_step++;
        }
    } else {
        if (calib_offset_done.front.even_chip0 == 0) {
            if (calib_offset_search_even_front.b.chip0 < 250u) {
                calib_offset_search_even_front.f.chip0 = calib_offset_search_even_front.b.chip0 + 1;
            } else {
                calib_offset_done.front.even_chip0 = 1;
            }
        }
        if (calib_offset_done.front.even_chip1 == 0) {
            if (calib_offset_search_even_front.b.chip1 < 250u) {
                calib_offset_search_even_front.f.chip1 = calib_offset_search_even_front.b.chip1 + 1;
            } else {
                calib_offset_done.front.even_chip1 = 1;
            }
        }
        if (calib_offset_done.front.even_chip2 == 0) {
            if (calib_offset_search_even_front.b.chip2 < 250u) {
                calib_offset_search_even_front.f.chip2 = calib_offset_search_even_front.b.chip2 + 1;
            } else {
                calib_offset_done.front.even_chip2 = 1;
            }
        }
        if (calib_offset_done.back.even_chip0 == 0) {
            if (calib_offset_search_even_back.b.chip0 < 250u) {
                calib_offset_search_even_back.f.chip0 = calib_offset_search_even_back.b.chip0 + 1;
            } else {
                calib_offset_done.back.even_chip0 = 1;
            }
        }
        if (calib_offset_done.back.even_chip1 == 0) {
            if (calib_offset_search_even_back.b.chip1 < 250u) {
                calib_offset_search_even_back.f.chip1 = calib_offset_search_even_back.b.chip1 + 1;
            } else {
                calib_offset_done.back.even_chip1 = 1;
            }
        }
        if (calib_offset_done.back.even_chip2 == 0) {
            if (calib_offset_search_even_back.b.chip2 < 250u) {
                calib_offset_search_even_back.f.chip2 = calib_offset_search_even_back.b.chip2 + 1;
            } else {
                calib_offset_done.back.even_chip2 = 1;
            }
        }
        chip_bits = 0;
        if ((calib_sides & 1)) {
            calib_offset_even_front.l.v = calib_offset_search_even_front.l.v;
            chip_bits |= 0x540;
        }
        if ((calib_sides & 2)) {
            calib_offset_even_back.l.v = calib_offset_search_even_back.l.v;
            chip_bits |= 0xa80;
        }
        calib_write_afe_offsets(chip_bits);
        calib_line_count = 1;
        calib_step_result = CALIB_STEP_STAY;
    }
    return;
}

extern void _builtin_set_imask(int);


void calib_offset_step_4(void)
{
    char all_done;
    _builtin_set_imask(15);
    if (calib_offset_done.front.even_chip0 == 0) {
        p_vdcc_measure[0]->select.s.sel = 0;
        if (p_vdcc_measure[0]->level > calib_offset_target_x4_front) {
            calib_offset_done.front.even_chip0 = 1;
        }
    }
    if (calib_offset_done.front.even_chip1 == 0) {
        p_vdcc_measure[0]->select.s.sel = 1;
        if (p_vdcc_measure[0]->level > calib_offset_target_x4_front) {
            calib_offset_done.front.even_chip1 = 1;
        }
    }
    if (calib_offset_done.front.even_chip2 == 0) {
        p_vdcc_measure[0]->select.s.sel = 2;
        if (p_vdcc_measure[0]->level > calib_offset_target_x4_front) {
            calib_offset_done.front.even_chip2 = 1;
        }
    }
    _builtin_set_imask(line_irq_imask);
    _builtin_set_imask(15);
    if (calib_offset_done.back.even_chip0 == 0) {
        p_vdcc_measure[1]->select.s.sel = 0;
        if (p_vdcc_measure[1]->level > calib_offset_target_x4_back) {
            calib_offset_done.back.even_chip0 = 1;
        }
    }
    if (calib_offset_done.back.even_chip1 == 0) {
        p_vdcc_measure[1]->select.s.sel = 1;
        if (p_vdcc_measure[1]->level > calib_offset_target_x4_back) {
            calib_offset_done.back.even_chip1 = 1;
        }
    }
    if (calib_offset_done.back.even_chip2 == 0) {
        p_vdcc_measure[1]->select.s.sel = 2;
        if (p_vdcc_measure[1]->level > calib_offset_target_x4_back) {
            calib_offset_done.back.even_chip2 = 1;
        }
    }
    _builtin_set_imask(line_irq_imask);
    all_done = 0;
    if (calib_sides == 1) {
        if (calib_offset_done.u.hi == 63) {
            all_done = 1;
        }
    } else if (calib_sides == 2) {
        if (calib_offset_done.u.lo == 63) {
            all_done = 1;
        }
    } else if (calib_sides == 3) {
        if (calib_offset_done.w == 0x3f3f) {
            all_done = 1;
        }
    }
    if (all_done == 0) {
        calib_step--;
    } else {
        calib_step++;
        calib_line_count = 2;
    }
    calib_step_result = CALIB_STEP_NEXT;
}

void calib_offset_step_5(void)
{
    if (calib_line_count != 0) {
        calib_line_count--;
        calib_step_result = CALIB_STEP_STAY;
        return;
    } else {
    calib_busy = 0;
    _builtin_set_imask(15);
    if (calib_offset_search_odd_front.b.chip0 >= 250u) {
        calib_busy = 1;
        calib_offset_done.front.odd_chip0 = 0;
    }
    if (calib_offset_search_odd_front.b.chip1 >= 250u) {
        calib_busy = 1;
        calib_offset_done.front.odd_chip1 = 0;
    }
    if (calib_offset_search_odd_front.b.chip2 >= 250u) {
        calib_busy = 1;
        calib_offset_done.front.odd_chip2 = 0;
    }
    if (calib_offset_search_even_front.b.chip0 >= 250u) {
        calib_busy = 1;
        calib_offset_done.front.even_chip0 = 0;
    }
    if (calib_offset_search_even_front.b.chip1 >= 250u) {
        calib_busy = 1;
        calib_offset_done.front.even_chip1 = 0;
    }
    if (calib_offset_search_even_front.b.chip2 >= 250u) {
        calib_busy = 1;
        calib_offset_done.front.even_chip2 = 0;
    }
    if (calib_offset_search_odd_back.b.chip0 >= 250u) {
        calib_busy = 1;
        calib_offset_done.back.odd_chip0 = 0;
    }
    if (calib_offset_search_odd_back.b.chip1 >= 250u) {
        calib_busy = 1;
        calib_offset_done.back.odd_chip1 = 0;
    }
    if (calib_offset_search_odd_back.b.chip2 >= 250u) {
        calib_busy = 1;
        calib_offset_done.back.odd_chip2 = 0;
    }
    if (calib_offset_search_even_back.b.chip0 >= 250u) {
        calib_busy = 1;
        calib_offset_done.back.even_chip0 = 0;
    }
    if (calib_offset_search_even_back.b.chip1 >= 250u) {
        calib_busy = 1;
        calib_offset_done.back.even_chip1 = 0;
    }
    if (calib_offset_search_even_back.b.chip2 >= 250u) {
        calib_busy = 1;
        calib_offset_done.back.even_chip2 = 0;
    }
    _builtin_set_imask(line_irq_imask);
    calib_step++;
    calib_step_result = CALIB_STEP_NEXT;
    }
    return;
}


void calib_offset_step_6(void)
{
    if (calib_sides & 1) {
        calib_offset_result_odd_front.v = calib_offset_search_odd_front.l.v;
        calib_offset_result_even_front.v = calib_offset_search_even_front.l.v;
    }
    if (calib_sides & 2) {
        calib_offset_result_odd_back.v = calib_offset_search_odd_back.l.v;
        calib_offset_result_even_back.v = calib_offset_search_even_back.l.v;
    }
    calib_step_result = CALIB_STEP_DONE;
    return;
}

void calib_gain_step_0(void)
{
    calib_line_count = 1;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}


void calib_gain_step_1(void)
{
    short chip_bits;

    if (calib_line_count != 0) {
        calib_line_count--;
        if (calib_line_count == 0) {
            calib_step_result = CALIB_STEP_NEXT;
            calib_step++;
        }
    } else {
        if (calib_gain_done.front.odd_chip0 == 0) {
            if (calib_gain_search_odd_front.b.chip0 < 250u) {
                calib_gain_search_odd_front.f.chip0 = calib_gain_search_odd_front.b.chip0 + 1;
            } else {
                calib_gain_done.front.odd_chip0 = 1;
            }
        }
        if (calib_gain_done.front.odd_chip1 == 0) {
            if (calib_gain_search_odd_front.b.chip1 < 250u) {
                calib_gain_search_odd_front.f.chip1 = calib_gain_search_odd_front.b.chip1 + 1;
            } else {
                calib_gain_done.front.odd_chip1 = 1;
            }
        }
        if (calib_gain_done.front.odd_chip2 == 0) {
            if (calib_gain_search_odd_front.b.chip2 < 250u) {
                calib_gain_search_odd_front.f.chip2 = calib_gain_search_odd_front.b.chip2 + 1;
            } else {
                calib_gain_done.front.odd_chip2 = 1;
            }
        }
        if (calib_gain_done.back.odd_chip0 == 0) {
            if (calib_gain_search_odd_back.b.chip0 < 250u) {
                calib_gain_search_odd_back.f.chip0 = calib_gain_search_odd_back.b.chip0 + 1;
            } else {
                calib_gain_done.back.odd_chip0 = 1;
            }
        }
        if (calib_gain_done.back.odd_chip1 == 0) {
            if (calib_gain_search_odd_back.b.chip1 < 250u) {
                calib_gain_search_odd_back.f.chip1 = calib_gain_search_odd_back.b.chip1 + 1;
            } else {
                calib_gain_done.back.odd_chip1 = 1;
            }
        }
        if (calib_gain_done.back.odd_chip2 == 0) {
            if (calib_gain_search_odd_back.b.chip2 < 250u) {
                calib_gain_search_odd_back.f.chip2 = calib_gain_search_odd_back.b.chip2 + 1;
            } else {
                calib_gain_done.back.odd_chip2 = 1;
            }
        }
        chip_bits = 0;
        if ((calib_sides & 1)) {
            calib_gain_odd_front.l.v = calib_gain_search_odd_front.l.v;
            chip_bits |= 0x15;
        }
        if ((calib_sides & 2)) {
            calib_gain_odd_back.l.v = calib_gain_search_odd_back.l.v;
            chip_bits |= 0x2a;
        }
        calib_write_afe_gains(chip_bits);
        calib_line_count = 1;
        calib_step_result = CALIB_STEP_STAY;
    }
    return;
}

extern void _builtin_set_imask(int);


void calib_gain_step_2(void)
{
    char all_done;

    if (!calib_gain_done.front.odd_chip0) {
        if (p_vdcc_front[0]->measure_status.s2.status == 2) {
            calib_gain_done.front.odd_chip0 = 1;
        }
    }
    if (!calib_gain_done.front.odd_chip1) {
        if (p_vdcc_front[0]->measure_status.s4.status == 2) {
            calib_gain_done.front.odd_chip1 = 1;
        }
    }
    if (calib_gain_done.front.odd_chip2 == 0) {
        if (p_vdcc_front[0]->measure_status.s6.status == 2) {
            calib_gain_done.front.odd_chip2 = 1;
        }
    }
    if (!calib_gain_done.back.odd_chip0) {
        if (p_vdcc_back[0]->measure_status.s2.status == 2) {
            calib_gain_done.back.odd_chip0 = 1;
        }
    }
    if (!calib_gain_done.back.odd_chip1) {
        if (p_vdcc_back[0]->measure_status.s4.status == 2) {
            calib_gain_done.back.odd_chip1 = 1;
        }
    }
    if (calib_gain_done.back.odd_chip2 == 0) {
        if (p_vdcc_back[0]->measure_status.s6.status == 2) {
            calib_gain_done.back.odd_chip2 = 1;
        }
    }
    all_done = 0;
    if (calib_sides == 1) {
        if (calib_gain_done.u.hi == 7) {
            all_done = 1;
        }
    } else if (calib_sides == 2) {
        if (calib_gain_done.u.lo == 7) {
            all_done = 1;
        }
    } else if (calib_sides == 3) {
        if (calib_gain_done.w == 0x707) {
            all_done = 1;
        }
    }
    if (all_done == 0) {
        calib_step--;
    } else {
        if (calib_gain_search_odd_front.b.chip0 > 10u) {
            calib_gain_search_even_front.f.chip0 = calib_gain_search_odd_front.b.chip0 - 10;
        } else {
            calib_gain_search_even_front.b.chip0 = 0;
        }
        if (calib_gain_search_odd_front.b.chip1 > 10u) {
            calib_gain_search_even_front.f.chip1 = calib_gain_search_odd_front.b.chip1 - 10;
        } else {
            calib_gain_search_even_front.b.chip1 = 0;
        }
        if (calib_gain_search_odd_front.b.chip2 > 10u) {
            calib_gain_search_even_front.f.chip2 = calib_gain_search_odd_front.b.chip2 - 10;
        } else {
            calib_gain_search_even_front.b.chip2 = 0;
        }
        if (calib_gain_search_odd_back.b.chip0 > 10u) {
            calib_gain_search_even_back.f.chip0 = calib_gain_search_odd_back.b.chip0 - 10;
        } else {
            calib_gain_search_even_back.b.chip0 = 0;
        }
        if (calib_gain_search_odd_back.b.chip1 > 10u) {
            calib_gain_search_even_back.f.chip1 = calib_gain_search_odd_back.b.chip1 - 10;
        } else {
            calib_gain_search_even_back.b.chip1 = 0;
        }
        if (calib_gain_search_odd_back.b.chip2 > 10u) {
            calib_gain_search_even_back.f.chip2 = calib_gain_search_odd_back.b.chip2 - 10;
        } else {
            calib_gain_search_even_back.b.chip2 = 0;
        }
        _builtin_set_imask(15);
        p_vdcc_measure[0]->select.f.mode = 1;
        p_vdcc_measure[1]->select.f.mode = 1;
        if (*p_vdcc_timing[0] > 0x1000) {
            p_vdcc_measure[0]->select.f.mode = 2;
            p_vdcc_measure[1]->select.f.mode = 2;
            if (calib_measure_mode_swap & 0x8000) {
                p_vdcc_measure[0]->select.f.mode = 1;
            }
            if (calib_measure_mode_swap & 0x80) {
                p_vdcc_measure[1]->select.f.mode = 1;
            }
        }
        if (img_side_params[0].ccd_300dpi_mode != 0) {
            p_vdcc_front_sensor->source_gate.b.b2 = 1;
            p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
        }
        _builtin_set_imask(line_irq_imask);
        calib_step++;
        calib_line_count = 0;
    }
    calib_step_result = CALIB_STEP_NEXT;
}


void calib_gain_step_3(void)
{
    short chip_bits;

    if (calib_line_count != 0) {
        calib_line_count--;
        if (calib_line_count == 0) {
            calib_step_result = CALIB_STEP_NEXT;
            calib_step++;
        }
    } else {
        if (calib_gain_done.front.even_chip0 == 0) {
            if (calib_gain_search_even_front.b.chip0 < 250u) {
                calib_gain_search_even_front.f.chip0 = calib_gain_search_even_front.b.chip0 + 1;
            } else {
                calib_gain_done.front.even_chip0 = 1;
            }
        }
        if (calib_gain_done.front.even_chip1 == 0) {
            if (calib_gain_search_even_front.b.chip1 < 250u) {
                calib_gain_search_even_front.f.chip1 = calib_gain_search_even_front.b.chip1 + 1;
            } else {
                calib_gain_done.front.even_chip1 = 1;
            }
        }
        if (calib_gain_done.front.even_chip2 == 0) {
            if (calib_gain_search_even_front.b.chip2 < 250u) {
                calib_gain_search_even_front.f.chip2 = calib_gain_search_even_front.b.chip2 + 1;
            } else {
                calib_gain_done.front.even_chip2 = 1;
            }
        }
        if (calib_gain_done.back.even_chip0 == 0) {
            if (calib_gain_search_even_back.b.chip0 < 250u) {
                calib_gain_search_even_back.f.chip0 = calib_gain_search_even_back.b.chip0 + 1;
            } else {
                calib_gain_done.back.even_chip0 = 1;
            }
        }
        if (calib_gain_done.back.even_chip1 == 0) {
            if (calib_gain_search_even_back.b.chip1 < 250u) {
                calib_gain_search_even_back.f.chip1 = calib_gain_search_even_back.b.chip1 + 1;
            } else {
                calib_gain_done.back.even_chip1 = 1;
            }
        }
        if (calib_gain_done.back.even_chip2 == 0) {
            if (calib_gain_search_even_back.b.chip2 < 250u) {
                calib_gain_search_even_back.f.chip2 = calib_gain_search_even_back.b.chip2 + 1;
            } else {
                calib_gain_done.back.even_chip2 = 1;
            }
        }
        chip_bits = 0;
        if ((calib_sides & 1)) {
            calib_gain_even_front.l.v = calib_gain_search_even_front.l.v;
            chip_bits |= 0x540;
        }
        if ((calib_sides & 2)) {
            calib_gain_even_back.l.v = calib_gain_search_even_back.l.v;
            chip_bits |= 0xa80;
        }
        calib_write_afe_gains(chip_bits);
        calib_line_count = 1;
        calib_step_result = CALIB_STEP_STAY;
    }
    return;
}

extern void _builtin_set_imask(int);
extern void _builtin_set_cr(int);
extern int _builtin_get_cr(void);
#define RESTORE() _builtin_set_cr((line_irq_imask & 15) << 4 | _builtin_get_cr() & ~0xf0)

void calib_gain_step_4(void)
{
    char all_done;

    if (!calib_gain_done.front.even_chip0) {
        if (p_vdcc_front[0]->measure_status.s2.status == 2) {
            calib_gain_done.front.even_chip0 = 1;
        }
    }
    if (!calib_gain_done.front.even_chip1) {
        if (p_vdcc_front[0]->measure_status.s4.status == 2) {
            calib_gain_done.front.even_chip1 = 1;
        }
    }
    if (!calib_gain_done.front.even_chip2) {
        if (p_vdcc_front[0]->measure_status.s6.status == 2) {
            calib_gain_done.front.even_chip2 = 1;
        }
    }
    if (!calib_gain_done.back.even_chip0) {
        if (p_vdcc_back[0]->measure_status.s2.status == 2) {
            calib_gain_done.back.even_chip0 = 1;
        }
    }
    if (!calib_gain_done.back.even_chip1) {
        if (p_vdcc_back[0]->measure_status.s4.status == 2) {
            calib_gain_done.back.even_chip1 = 1;
        }
    }
    if (!calib_gain_done.back.even_chip2) {
        if (p_vdcc_back[0]->measure_status.s6.status == 2) {
            calib_gain_done.back.even_chip2 = 1;
        }
    }
    all_done = 0;
    if (calib_sides == 1) {
        if (calib_gain_done.u.hi == 63) {
            all_done = 1;
        }
    } else if (calib_sides == 2) {
        if (calib_gain_done.u.lo == 63) {
            all_done = 1;
        }
    } else if (calib_sides == 3) {
        if (calib_gain_done.w == 0x3f3f) {
            all_done = 1;
        }
    }
    if (all_done == 0) {
        calib_step--;
    } else {
        calib_step++;
        calib_line_count = 2;
    }
    calib_step_result = CALIB_STEP_NEXT;
}

void calib_gain_step_5(void)
{
    if (calib_line_count != 0) {
        calib_line_count--;
        calib_step_result = CALIB_STEP_STAY;
        return;
    } else {
    calib_busy = 0;
    if (calib_gain_odd_front.b.chip0 >= 250u) {
        calib_busy = 1;
        calib_gain_done.front.odd_chip0 = 0;
    }
    if (calib_gain_odd_front.b.chip1 >= 250u) {
        calib_busy = 1;
        calib_gain_done.front.odd_chip1 = 0;
    }
    if (calib_gain_odd_front.b.chip2 >= 250u) {
        calib_busy = 1;
        calib_gain_done.front.odd_chip2 = 0;
    }
    if (calib_gain_even_front.b.chip0 >= 250u) {
        calib_busy = 1;
        calib_gain_done.front.even_chip0 = 0;
    }
    if (calib_gain_even_front.b.chip1 >= 250u) {
        calib_busy = 1;
        calib_gain_done.front.even_chip1 = 0;
    }
    if (calib_gain_even_front.b.chip2 >= 250u) {
        calib_busy = 1;
        calib_gain_done.front.even_chip2 = 0;
    }
    if (calib_gain_odd_back.b.chip0 >= 250u) {
        calib_busy = 1;
        calib_gain_done.back.odd_chip0 = 0;
    }
    if (calib_gain_odd_back.b.chip1 >= 250u) {
        calib_busy = 1;
        calib_gain_done.back.odd_chip1 = 0;
    }
    if (calib_gain_odd_back.b.chip2 >= 250u) {
        calib_busy = 1;
        calib_gain_done.back.odd_chip2 = 0;
    }
    if (calib_gain_even_back.b.chip0 >= 250u) {
        calib_busy = 1;
        calib_gain_done.back.even_chip0 = 0;
    }
    if (calib_gain_even_back.b.chip1 >= 250u) {
        calib_busy = 1;
        calib_gain_done.back.even_chip1 = 0;
    }
    if (calib_gain_even_back.b.chip2 >= 250u) {
        calib_busy = 1;
        calib_gain_done.back.even_chip2 = 0;
    }
    calib_step++;
    calib_step_result = CALIB_STEP_NEXT;
    }
    return;
}

void calib_gain_step_6(void)
{
    if (calib_sides & 1) {
        calib_gain_result_odd_front.v = calib_gain_search_odd_front.l.v;
        calib_gain_result_even_front.v = calib_gain_search_even_front.l.v;
    }
    if (calib_sides & 2) {
        calib_gain_result_odd_back.v = calib_gain_search_odd_back.l.v;
        calib_gain_result_even_back.v = calib_gain_search_even_back.l.v;
    }
    calib_step_result = CALIB_STEP_DONE;
    return;
}

void calib_black_step_1(void)
{
    if (calib_line_count != 0) {
        calib_line_count--;
        _builtin_set_imask(15);
        p_vdcc_measure[0]->select.s.sel = 0;
        calib_black_pass1_front_chip0.w += p_vdcc_measure[0]->level;
        p_vdcc_measure[0]->select.s.sel = 1;
        calib_black_pass1_front_chip1.w += p_vdcc_measure[0]->level;
        p_vdcc_measure[0]->select.s.sel = 2;
        calib_black_pass1_front_chip2.w += p_vdcc_measure[0]->level;
        p_vdcc_measure[1]->select.s.sel = 0;
        calib_black_pass1_back_chip0.w += p_vdcc_measure[1]->level;
        p_vdcc_measure[1]->select.s.sel = 1;
        calib_black_pass1_back_chip1.w += p_vdcc_measure[1]->level;
        p_vdcc_measure[1]->select.s.sel = 2;
        calib_black_pass1_back_chip2.w += p_vdcc_measure[1]->level;
        _builtin_set_imask(line_irq_imask);
        if (calib_line_count == 0) {
            calib_line_count = 8;
            calib_step_result = CALIB_STEP_NEXT;
            calib_step++;
            _builtin_set_imask(15);
            p_vdcc_measure[0]->select.f.mode = 1;
            p_vdcc_measure[1]->select.f.mode = 1;
            if (*p_vdcc_timing[0] > 0x1000) {
                p_vdcc_measure[0]->select.f.mode = 2;
                p_vdcc_measure[1]->select.f.mode = 2;
                if (calib_measure_mode_swap & 0x8000) {
                    p_vdcc_measure[0]->select.f.mode = 1;
                }
                if (calib_measure_mode_swap & 0x80) {
                    p_vdcc_measure[1]->select.f.mode = 1;
                }
            }
            if (img_side_params[0].ccd_300dpi_mode != 0) {
                p_vdcc_front_sensor->source_gate.b.b2 = 1;
                p_vdcc_front_sensor->source_gate.b.ccd_300dpi_mode = 0;
            }
            _builtin_set_imask(line_irq_imask);
        }
    } else {
        calib_line_count = 8;
        calib_step_result = CALIB_STEP_STAY;
    }
    return;
}

void calib_black_step_2(void)
{
    if (calib_line_count != 0) {
        calib_line_count--;
        _builtin_set_imask(15);
        p_vdcc_measure[0]->select.s.sel = 0;
        calib_black_pass2_front_chip0.w += p_vdcc_measure[0]->level;
        p_vdcc_measure[0]->select.s.sel = 1;
        calib_black_pass2_front_chip1.w += p_vdcc_measure[0]->level;
        p_vdcc_measure[0]->select.s.sel = 2;
        calib_black_pass2_front_chip2.w += p_vdcc_measure[0]->level;
        p_vdcc_measure[1]->select.s.sel = 0;
        calib_black_pass2_back_chip0.w += p_vdcc_measure[1]->level;
        p_vdcc_measure[1]->select.s.sel = 1;
        calib_black_pass2_back_chip1.w += p_vdcc_measure[1]->level;
        p_vdcc_measure[1]->select.s.sel = 2;
        calib_black_pass2_back_chip2.w += p_vdcc_measure[1]->level;
        _builtin_set_imask(line_irq_imask);
        if (calib_line_count == 0) {
            calib_step_result = CALIB_STEP_NEXT;
            calib_step++;
        }
    }
    return;
}

void calib_black_step_3(void)
{
    calib_black_pass1_front_chip0.w >>= 3;
    calib_black_pass2_front_chip0.w >>= 3;
    calib_black_pass1_front_chip1.w >>= 3;
    calib_black_pass2_front_chip1.w >>= 3;
    calib_black_pass1_front_chip2.w >>= 3;
    calib_black_pass2_front_chip2.w >>= 3;
    calib_black_pass1_back_chip0.w >>= 3;
    calib_black_pass2_back_chip0.w >>= 3;
    calib_black_pass1_back_chip1.w >>= 3;
    calib_black_pass2_back_chip1.w >>= 3;
    calib_black_pass1_back_chip2.w >>= 3;
    calib_black_pass2_back_chip2.w >>= 3;
    calib_black_pass1_front_chip0.w &= 0xff;
    calib_black_pass2_front_chip0.w &= 0xff;
    calib_black_pass1_front_chip1.w &= 0xff;
    calib_black_pass2_front_chip1.w &= 0xff;
    calib_black_pass1_front_chip2.w &= 0xff;
    calib_black_pass2_front_chip2.w &= 0xff;
    calib_black_pass1_back_chip0.w &= 0xff;
    calib_black_pass2_back_chip0.w &= 0xff;
    calib_black_pass1_back_chip1.w &= 0xff;
    calib_black_pass2_back_chip1.w &= 0xff;
    calib_black_pass1_back_chip2.w &= 0xff;
    calib_black_pass2_back_chip2.w &= 0xff;
    calib_busy = 0;
    calib_step_result = CALIB_STEP_DONE;
    return;
}

void calib_apply_step_0(void)
{
    _builtin_set_imask(15);
    p_vdcc_front_correction[0]->mode_chip0 = p_vdcc_front_correction[0]->mode_chip1 = p_vdcc_front_correction[0]->mode_chip2 = 80;
    p_vdcc_back_correction[0]->mode_chip0 = p_vdcc_back_correction[0]->mode_chip1 = p_vdcc_back_correction[0]->mode_chip2 = 80;
    _builtin_set_imask(line_irq_imask);
    calib_step_result = CALIB_STEP_NEXT;
    calib_line_count = 2;
    calib_step++;
    return;
}

void calib_apply_step_1(void)
{
    if (calib_line_count != 0) {
        calib_line_count--;
        if (calib_line_count == 0) {
            _builtin_set_imask(15);
            p_vdcc_front[0]->events.c = 0xe0;
            p_vdcc_back[0]->events.c = 0xe0;
            _builtin_set_imask(line_irq_imask);
            calib_step_result = CALIB_STEP_NEXT;
            calib_step++;
        }
    }
    return;
}

void calib_apply_step_2(void)
{
    _builtin_set_imask(15);
    p_vdcc_front_correction[0]->reg_012 = 0x80;
    p_vdcc_back_correction[0]->reg_012 = 0x80;
    p_vdcc_front_correction[0]->mode_chip0 = p_vdcc_front_correction[0]->mode_chip1 = p_vdcc_front_correction[0]->mode_chip2 = 87;
    p_vdcc_back_correction[0]->mode_chip0 = p_vdcc_back_correction[0]->mode_chip1 = p_vdcc_back_correction[0]->mode_chip2 = 87;
    _builtin_set_imask(line_irq_imask);
    calib_line_count = 100;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_apply_step_3(void)
{
    if (calib_line_count != 0) {
        calib_line_count--;
        calib_step_result = CALIB_STEP_STAY;
        if (p_vdcc_front[0]->events.b.correction_done_7 && p_vdcc_front[0]->events.b.correction_done_6 && p_vdcc_front[0]->events.b.correction_done_5
            && p_vdcc_back[0]->events.b.correction_done_7 && p_vdcc_back[0]->events.b.correction_done_6 && p_vdcc_back[0]->events.b.correction_done_5) {
            calib_line_count = 0;
        }
        return;
    }
    _builtin_set_imask(15);
    p_vdcc_front[0]->events.c = 0xe0;
    p_vdcc_back[0]->events.c = 0xe0;
    _builtin_set_imask(line_irq_imask);
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_apply_step_4(void)
{
    calib_busy = 0;
    calib_step_result = CALIB_STEP_DONE;
    return;
}

void calib_lamp_check_step_1(void)
{
    if (calib_line_count != 0) {
        calib_line_count--;
        _builtin_set_imask(15);
        p_vdcc_measure[0]->select.s.sel = 0;
        calib_exposure[0][0] += p_vdcc_measure[0]->level;
        p_vdcc_measure[0]->select.s.sel = 1;
        ((unsigned long *)calib_exposure)[1] += p_vdcc_measure[0]->level;
        p_vdcc_measure[0]->select.s.sel = 2;
        ((unsigned long *)calib_exposure)[2] += p_vdcc_measure[0]->level;
        p_vdcc_measure[1]->select.s.sel = 0;
        calib_exposure_back[0].chip0 += p_vdcc_measure[1]->level;
        p_vdcc_measure[1]->select.s.sel = 1;
        ((unsigned long *)calib_exposure_back)[1] += p_vdcc_measure[1]->level;
        p_vdcc_measure[1]->select.s.sel = 2;
        ((unsigned long *)calib_exposure_back)[2] += p_vdcc_measure[1]->level;
        if (job_adf_sheet_count == 0 && calib_in_lamp_check == 0) {
            p_vdcc_measure[0]->select.s.sel = 0;
            calib_exposure_w4[0][0] += p_vdcc_measure[0]->level_2;
            p_vdcc_measure[0]->select.s.sel = 1;
            calib_exposure_w4[0][1] += p_vdcc_measure[0]->level_2;
            p_vdcc_measure[0]->select.s.sel = 2;
            calib_exposure_w4[0][2] += p_vdcc_measure[0]->level_2;
            p_vdcc_measure[1]->select.s.sel = 0;
            calib_exposure_w4_back[0].chip0 += p_vdcc_measure[1]->level_2;
            p_vdcc_measure[1]->select.s.sel = 1;
            ((unsigned long *)calib_exposure_w4_back)[1] += p_vdcc_measure[1]->level_2;
            p_vdcc_measure[1]->select.s.sel = 2;
            ((unsigned long *)calib_exposure_w4_back)[2] += p_vdcc_measure[1]->level_2;
        }
        _builtin_set_imask(line_irq_imask);
        if (calib_line_count == 0) {
            calib_line_count = 1;
            calib_step_result = CALIB_STEP_NEXT;
            calib_step++;
        }
    } else {
        calib_line_count = 16;
        calib_step_result = CALIB_STEP_STAY;
    }
    return;
}

void calib_lamp_check_step_2(void)
{
    calib_exposure[0][0] = calib_exposure[0][0] >> 4;
    calib_exposure[0][1] = calib_exposure[0][1] >> 4;
    calib_exposure[0][2] = calib_exposure[0][2] >> 4;
    calib_exposure_back[0].chip0 = calib_exposure_back[0].chip0 >> 4;
    calib_exposure_back[0].chip1 = calib_exposure_back[0].chip1 >> 4;
    calib_exposure_back[0].chip2 = calib_exposure_back[0].chip2 >> 4;
    if (job_adf_sheet_count == 0 && *(long *)&calib_in_lamp_check == 0) {
        calib_exposure_w4[0][0] = calib_exposure_w4[0][0] >> 6;
        calib_exposure_w4[0][1] = calib_exposure_w4[0][1] >> 6;
        calib_exposure_w4[0][2] = calib_exposure_w4[0][2] >> 6;
        calib_exposure_w4_back[0].chip0 = calib_exposure_w4_back[0].chip0 >> 6;
        calib_exposure_w4_back[0].chip1 = calib_exposure_w4_back[0].chip1 >> 6;
        calib_exposure_w4_back[0].chip2 = calib_exposure_w4_back[0].chip2 >> 6;
    }
    calib_busy = 0;
    calib_step_result = CALIB_STEP_DONE;
    return;
}


void calib_lamp_adjust_step_1(void)
{
    _builtin_set_imask(15);
    if (calib_white_level[0][0] == 0 || (unsigned char)calib_white_level[0][0] == 0x80) {
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_front_correction[0]->white_factor = calib_white_level[0][0];
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_LOAD_WHITE;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    }
    if (calib_white_level_back[0] == 0 || (unsigned char)calib_white_level_back[0] == 0x80) {
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_back_correction[0]->white_factor = calib_white_level_back[0];
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_LOAD_WHITE;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    }
    _builtin_set_imask(line_irq_imask);
    calib_line_count = 1;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}
void calib_lamp_adjust_step_2(void)
{
    _builtin_set_imask(15);
    if (((char *)calib_white_level)[1] == 0 || (unsigned char)((char *)calib_white_level)[1] == 0x80) {
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_front_correction[0]->white_factor = ((char *)calib_white_level)[1];
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_LOAD_WHITE;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    }
    if (calib_white_level_back_1[0] == 0 || (unsigned char)calib_white_level_back_1[0] == 0x80) {
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_back_correction[0]->white_factor = calib_white_level_back_1[0];
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_LOAD_WHITE;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    }
    _builtin_set_imask(line_irq_imask);
    calib_line_count = 1;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_lamp_adjust_step_3(void)
{
    _builtin_set_imask(15);
    if (((char *)calib_white_level)[2] == 0 || (unsigned char)((char *)calib_white_level)[2] == 0x80) {
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_front_correction[0]->white_factor = ((char *)calib_white_level)[2];
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_LOAD_WHITE;
    }
    if (calib_white_level_back_2[0] == 0 || (unsigned char)calib_white_level_back_2[0] == 0x80) {
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_back_correction[0]->white_factor = calib_white_level_back_2[0];
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_LOAD_WHITE;
    }
    _builtin_set_imask(line_irq_imask);
    if (!(0 != (0 != calib_white_level_fine[0][0])) && !((0 != calib_white_level_fine[0][1]) != 0) && calib_white_level_fine[0][2] == 0 && *(volatile char *)calib_white_level_fine_back == 0 && *(volatile char *)calib_white_level_fine_back_1 == 0 && *(volatile char *)calib_white_level_fine_back_2 == 0) calib_step += 3;
    calib_line_count = 1;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_lamp_adjust_step_4(void)
{
    _builtin_set_imask(15);
    if (calib_white_level_fine[0][0] == 0) {
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_front_correction[0]->white_factor = calib_white_level_fine[0][0];
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_LOAD_WHITE;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    }
    if (calib_white_level_fine_back[0] == 0) {
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_back_correction[0]->white_factor = calib_white_level_fine_back[0];
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_LOAD_WHITE;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    }
    _builtin_set_imask(line_irq_imask);
    calib_line_count = 1;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_lamp_adjust_step_5(void)
{
    _builtin_set_imask(15);
    if (((char *)calib_white_level_fine)[1] == 0) {
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_front_correction[0]->white_factor = ((char *)calib_white_level_fine)[1];
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_LOAD_WHITE;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    }
    if (calib_white_level_fine_back_1[0] == 0) {
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_back_correction[0]->white_factor = calib_white_level_fine_back_1[0];
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_LOAD_WHITE;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    }
    _builtin_set_imask(line_irq_imask);
    calib_line_count = 1;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_lamp_adjust_step_6(void)
{
    _builtin_set_imask(15);
    if (((char *)calib_white_level_fine)[2] == 0) {
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_front_correction[0]->white_factor = ((char *)calib_white_level_fine)[2];
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_LOAD_WHITE;
    }
    if (calib_white_level_fine_back_2[0] == 0) {
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    } else {
        p_vdcc_back_correction[0]->white_factor = calib_white_level_fine_back_2[0];
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_LOAD_WHITE;
    }
    _builtin_set_imask(line_irq_imask);
    calib_line_count = 1;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_lamp_adjust_step_7(void)
{
    _builtin_set_imask(15);
    p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
    p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
    p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
    p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
    p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    _builtin_set_imask(line_irq_imask);
    calib_busy = 0;
    calib_step_result = CALIB_STEP_DONE;
    return;
}

void calib_lamp_11_step_1(void)
{
    unsigned char mode;
    if (calib_lamp_11_flag != 0) {
        mode = 0x60;
    } else {
        mode = 0x70;
    }
    _builtin_set_imask(15);
    p_vdcc_front_correction[0]->mode_chip0 = mode;
    p_vdcc_front_correction[0]->mode_chip1 = mode;
    p_vdcc_front_correction[0]->mode_chip2 = mode;
    p_vdcc_back_correction[0]->mode_chip0 = mode;
    p_vdcc_back_correction[0]->mode_chip1 = mode;
    p_vdcc_back_correction[0]->mode_chip2 = mode;
    _builtin_set_imask(line_irq_imask);
    calib_line_count = 100;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_lamp_11_step_2(void)
{
    if (calib_line_count != 0) {
        calib_line_count--;
        calib_step_result = CALIB_STEP_STAY;
        if (((struct vdcc_reg_06 *)p_vdcc_front[0])->b3 && ((struct vdcc_reg_06 *)p_vdcc_front[0])->b2 && ((struct vdcc_reg_06 *)p_vdcc_front[0])->b1
            && ((struct vdcc_reg_06 *)p_vdcc_back[0])->b3 && ((struct vdcc_reg_06 *)p_vdcc_back[0])->b2 && ((struct vdcc_reg_06 *)p_vdcc_back[0])->b1) {
            calib_line_count = 0;
        }
        return;
    }
    {
        _builtin_set_imask(15);
        p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
        p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
        _builtin_set_imask(line_irq_imask);
        calib_busy = 0;
        calib_step_result = CALIB_STEP_DONE;
    }
    return;
}

void calib_mode_12_step_1(void)
{
    if (calib_line_count != 0) {
        calib_line_count--;
        _builtin_set_imask(15);
        p_vdcc_measure[0]->select.s.sel = 0;
        ((unsigned int *)calib_mode_12_level_front)[0] += p_vdcc_measure[0]->level;
        p_vdcc_measure[0]->select.s.sel = 1;
        ((unsigned int *)calib_mode_12_level_front)[1] += p_vdcc_measure[0]->level;
        p_vdcc_measure[0]->select.s.sel = 2;
        ((unsigned int *)calib_mode_12_level_front)[2] += p_vdcc_measure[0]->level;
        p_vdcc_measure[1]->select.s.sel = 0;
        ((unsigned int *)calib_mode_12_level_back)[0] += p_vdcc_measure[1]->level;
        p_vdcc_measure[1]->select.s.sel = 1;
        ((unsigned int *)calib_mode_12_level_back)[1] += p_vdcc_measure[1]->level;
        p_vdcc_measure[1]->select.s.sel = 2;
        ((unsigned int *)calib_mode_12_level_back)[2] += p_vdcc_measure[1]->level;
        _builtin_set_imask(line_irq_imask);
        if (calib_line_count == 0) {
            calib_line_count = 1;
            calib_step_result = CALIB_STEP_NEXT;
            calib_step++;
        }
    } else {
        calib_line_count = 16;
        calib_step_result = CALIB_STEP_STAY;
    }
    return;
}

void calib_mode_12_step_2(void)
{
    calib_mode_12_level_front[0].chip0 = calib_mode_12_level_front[0].chip0 >> 4;
    calib_mode_12_level_front[0].chip1 = calib_mode_12_level_front[0].chip1 >> 4;
    calib_mode_12_level_front[0].chip2 = calib_mode_12_level_front[0].chip2 >> 4;
    calib_mode_12_level_back[0].chip0 = calib_mode_12_level_back[0].chip0 >> 4;
    calib_mode_12_level_back[0].chip1 = calib_mode_12_level_back[0].chip1 >> 4;
    calib_mode_12_level_back[0].chip2 = calib_mode_12_level_back[0].chip2 >> 4;
    calib_busy = 0;
    calib_step_result = CALIB_STEP_DONE;
    return;
}

void calib_mode_13_step_1(void)
{
    unsigned char mode;
    if (calib_lamp_adjust_first != 0) {
        mode = 0x80;
    } else {
        mode = 0xa0;
    }
    _builtin_set_imask(15);
    p_vdcc_front_correction[0]->mode_chip0 = mode;
    p_vdcc_front_correction[0]->mode_chip1 = mode;
    p_vdcc_front_correction[0]->mode_chip2 = mode;
    p_vdcc_back_correction[0]->mode_chip0 = mode;
    p_vdcc_back_correction[0]->mode_chip1 = mode;
    p_vdcc_back_correction[0]->mode_chip2 = mode;
    _builtin_set_imask(line_irq_imask);
    calib_line_count = 1;
    calib_step_result = CALIB_STEP_NEXT;
    calib_step++;
    return;
}

void calib_mode_13_step_2(void)
{
    _builtin_set_imask(15);
    calib_line_count = 0;
    p_vdcc_front_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
    p_vdcc_front_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
    p_vdcc_front_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    p_vdcc_back_correction[0]->mode_chip0 = CALIB_CORRECTION_ON;
    p_vdcc_back_correction[0]->mode_chip1 = CALIB_CORRECTION_ON;
    p_vdcc_back_correction[0]->mode_chip2 = CALIB_CORRECTION_ON;
    _builtin_set_imask(line_irq_imask);
    calib_busy = 0;
    calib_step_result = CALIB_STEP_DONE;
    return;
}
#undef RESTORE

/* ---- H2 (PH): calib_record_mark_sample (run by line_handle_side in L1). */
#pragma section H
void calib_record_mark_sample(void)
{
    unsigned short counter;
    unsigned short limit;
    counter = ((struct vdcc_counter_reg_0e *)p_vdcc_back_counter[0])->step_count;
    if (!mdc_mailbox.sensor_status.f.fb_home && selftest_mark_start == 0) {
        selftest_mark_start = ((struct vdcc_counter_reg_0e *)p_vdcc_back_counter[0])->step_count;
    }
    if (selftest_mark_start == 0) {
        limit = 0xffff;
    } else if (settings_byte_0x35[0] & 1) {
        limit = selftest_mark_start + 106;
    } else {
        limit = selftest_mark_start + 59;
    }
    if ((unsigned)counter > (unsigned)limit) {
        _builtin_set_imask(15);
        line_side_state[0].state_flags.f.mark_sampling = 0;
        _builtin_set_imask(line_irq_imask);
    } else {
        edge_left_front[selftest_profile_count] = ((struct vdcc_monitor_pair *)p_vdcc_front_monitor[0])->b;
        edge_right_front[selftest_profile_count] = counter;
        if (selftest_profile_count < 0x578) selftest_profile_count++;
    }
}
#pragma section
/* ---- L2 (P): calib_wait_done, the gain writers, the per-line page steps. */

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

int calib_wait_done(void)
{
    do {
        if (calib_wait_abort != 0) {
            _builtin_set_imask(15);
            *(volatile unsigned short *)&calib_started = 0;
            *(volatile unsigned short *)&calib_busy = 0;
            *(volatile unsigned short *)&calib_mode = CALIB_MODE_APPLY;
            *(volatile unsigned short *)&calib_running = 0;
            calib_wait_count[0] = 0;
            _builtin_set_imask(0);
            return 1;
        }
    } while (calib_running);
    if (calib_busy != 0) {
        return 1;
    }
    return 0;
}

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

void calib_write_afe_offsets(unsigned short chip_bits)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (win_params.flags.f.adf == 0) {
        p_vdcc_afe[0]->ctrl.f.adf_select = 0;
    } else {
        p_vdcc_afe[0]->ctrl.f.adf_select = 1;
    }
    _builtin_set_imask(saved_imask);
    if (chip_bits & 0x1) {
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_OFFSET_ODD);
        p_vdcc_afe[0]->data_low = calib_offset_odd_front.b.chip0;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    if (chip_bits & 0x4) {
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_OFFSET_ODD);
        p_vdcc_afe[0]->data_low = calib_offset_odd_front.b.chip1;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    if (chip_bits & 0x10) {
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_OFFSET_ODD);
        p_vdcc_afe[0]->data_low = calib_offset_odd_front.b.chip2;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    if (chip_bits & 0x40) {
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_OFFSET_EVEN);
        p_vdcc_afe[0]->data_low = calib_offset_even_front.b.chip0;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    if (chip_bits & 0x100) {
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_OFFSET_EVEN);
        p_vdcc_afe[0]->data_low = calib_offset_even_front.b.chip1;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    if (chip_bits & 0x400) {
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_OFFSET_EVEN);
        p_vdcc_afe[0]->data_low = calib_offset_even_front.b.chip2;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    _builtin_set_imask(15);
    p_vdcc_afe[1]->ctrl.f.adf_select = 0;
    _builtin_set_imask(saved_imask);
    if (chip_bits & 0x2) {
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_OFFSET_ODD);
        p_vdcc_afe[1]->data_low = calib_offset_odd_back.b.chip0;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
    if (chip_bits & 0x8) {
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_OFFSET_ODD);
        p_vdcc_afe[1]->data_low = calib_offset_odd_back.b.chip1;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
    if (chip_bits & 0x20) {
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_OFFSET_ODD);
        p_vdcc_afe[1]->data_low = calib_offset_odd_back.b.chip2;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
    if (chip_bits & 0x80) {
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_OFFSET_EVEN);
        p_vdcc_afe[1]->data_low = calib_offset_even_back.b.chip0;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
    if (chip_bits & 0x200) {
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_OFFSET_EVEN);
        p_vdcc_afe[1]->data_low = calib_offset_even_back.b.chip1;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
    if (chip_bits & 0x800) {
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_OFFSET_EVEN);
        p_vdcc_afe[1]->data_low = calib_offset_even_back.b.chip2;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
}

void calib_write_afe_gains(unsigned short chip_bits)
{
    unsigned short word;
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (win_params.flags.f.adf == 0) {
        p_vdcc_afe[0]->ctrl.f.adf_select = 0;
    } else {
        p_vdcc_afe[0]->ctrl.f.adf_select = 1;
    }
    _builtin_set_imask(saved_imask);
    if (chip_bits & 0x1) {
        word = calib_gain_odd_front.b.chip0;
        word <<= 2;
        word |= 0xc00;
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = word >> 8;
        p_vdcc_afe[0]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    if (chip_bits & 0x4) {
        word = calib_gain_odd_front.b.chip1;
        word <<= 2;
        word |= 0x4c00;
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = word >> 8;
        p_vdcc_afe[0]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    if (chip_bits & 0x10) {
        word = calib_gain_odd_front.b.chip2;
        word <<= 2;
        word |= 0xcc00;
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = word >> 8;
        p_vdcc_afe[0]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    if (chip_bits & 0x40) {
        word = calib_gain_even_front.b.chip0;
        word <<= 2;
        word |= 0x800;
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = word >> 8;
        p_vdcc_afe[0]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    if (chip_bits & 0x100) {
        word = calib_gain_even_front.b.chip1;
        word <<= 2;
        word |= 0x4800;
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = word >> 8;
        p_vdcc_afe[0]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    if (chip_bits & 0x400) {
        word = calib_gain_even_front.b.chip2;
        word <<= 2;
        word |= 0xc800;
        _builtin_set_imask(15);
        p_vdcc_afe[0]->data_high = word >> 8;
        p_vdcc_afe[0]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(0);
    }
    _builtin_set_imask(15);
    p_vdcc_afe[1]->ctrl.f.adf_select = 0;
    _builtin_set_imask(saved_imask);
    if (chip_bits & 0x2) {
        word = calib_gain_odd_back.b.chip0;
        word <<= 2;
        word |= 0xc00;
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = word >> 8;
        p_vdcc_afe[1]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
    if (chip_bits & 0x8) {
        word = calib_gain_odd_back.b.chip1;
        word <<= 2;
        word |= 0x4c00;
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = word >> 8;
        p_vdcc_afe[1]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
    if (chip_bits & 0x20) {
        word = calib_gain_odd_back.b.chip2;
        word <<= 2;
        word |= 0xcc00;
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = word >> 8;
        p_vdcc_afe[1]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
    if (chip_bits & 0x80) {
        word = calib_gain_even_back.b.chip0;
        word <<= 2;
        word |= 0x800;
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = word >> 8;
        p_vdcc_afe[1]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
    if (chip_bits & 0x200) {
        word = calib_gain_even_back.b.chip1;
        word <<= 2;
        word |= 0x4800;
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = word >> 8;
        p_vdcc_afe[1]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
    if (chip_bits & 0x800) {
        word = calib_gain_even_back.b.chip2;
        word <<= 2;
        word |= 0xc800;
        _builtin_set_imask(15);
        p_vdcc_afe[1]->data_high = word >> 8;
        p_vdcc_afe[1]->data_low = word;
        _builtin_set_imask(saved_imask);
        afe_wait_write(1);
    }
}

#define job_auto_size_flags job_auto_size_flags_word
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

/* Uninitialised data (section B). */
int line_page_step_imask;

int line_count_down_gap(int side)
{
    unsigned short counter;
    _builtin_set_imask(15);
    counter = p_vdcc_front_counter[0]->line_count;
    _builtin_set_imask(line_irq_imask);
    if (jpeg_scan_flags.v & JPEG_SCAN_RESUMING) if (counter <= 1) {
        if (tick_capture_wait_ticks.v >= 0x5dc) {
            line_gap_steps_left[side] = 0;
            return 1;
        }
        return 0;
    }
    *(unsigned short *)(line_gap_lines_left + side) -= 1;
    if (!(*(unsigned short *)(line_gap_lines_left + side) < 1))
        return 0;
    *(unsigned short *)(line_gap_counter + side) = counter;
    if (line_gap_steps_left[side]) {
        *(unsigned short *)(line_gap_lines_left + side) += *(*(unsigned char **)(line_gap_step_ptr + side))++;
        line_gap_steps_left[side]--;
    } else {
        *(unsigned short *)(line_gap_lines_left + side) = 1;
        if (line_gap_steps_left[0] == 0 && line_gap_steps_left_back[0] == 0) {
        }
    }
    return 1;
}

void line_run_page_steps(void)
{
    line_page_step_imask = _builtin_get_imask();
    if (line_page_state[0].state_flags.v & 12) {
        line_run_page_step(0);
    }
    if (line_page_state[1].state_flags.v & 12) {
        line_run_page_step(1);
    }
}


void line_run_page_step(int side)
{
    unsigned char capture_buf;
    unsigned short end_line;
    unsigned short lines;
    int last_side;
    struct edge_record *edge;
    struct page_record *scanning_page;
    if (p_vdcc_capture[side]->capture_ctrl.capture_run == 0) {
        return;
    }
    line_read_captured_lines(side);
    lines = line_captured_lines[side].w;
    if (job_long_page_flags & 0x80) {
        line_run_long_page_step(side, lines);
        if (jpeg_scan_flags.v & (JPEG_SCAN_PAUSE_WANTED | JPEG_SCAN_PAUSED)) {
            return;
        }
        _builtin_set_imask(15);
        if (!(job_long_page_flags & 2) && !(job_long_page_flags & 1)) {
            if (win_sides.f.selection == 1) {
                last_side = 1;
            } else {
                last_side = 0;
            }
            scanning_page = &page_records[page_scanning.v];
            if (scanning_page->sides[last_side].size_final == 1 || (line_side_state[0].state_flags.v & 0x4000) != 0) {
            } else {
                if (line_captured_lines[last_side].w > img_side_params[last_side].long_page_limit_lines) {
                    job_long_page_flags |= 1;
                    jpeg_scan_flags.v |= JPEG_SCAN_PAUSE_WANTED;
                }
            }
        } else {
            if ((job_long_page_flags & 1) || (job_long_page_flags & 2)) {
                _builtin_set_imask(line_page_step_imask);
                return;
            }
        }
        _builtin_set_imask(line_page_step_imask);
    }
    if (line_page_state[side].auto_size) {
        capture_buf = page_side_state[side].capture_buf;
        edge = &edge_side_records[side].e[capture_buf];
        if (edge->state < EDGE_STATE_SKEW_DIRECTION) {
            edge_run_line_step(side, line_page_step_imask, edge);
            if (edge->state == EDGE_STATE_SKEW_DIRECTION) {
                end_line = lines + 16;
                if (line_page_state[side].end_line > end_line) {
                    line_page_state[side].end_line = end_line;
                }
            }
        }
    }
    line_run_page_step_part_4(side);
    if (lines >= line_page_state[side].end_line || lines >= img_side_params[side].scan_length_lines) {
        line_end_page(side);
    }
}

void line_read_captured_lines(int side)
{
    while (1) {
        _builtin_set_imask(15);
        line_captured_lines[side].w = p_pisces_capture[side]->lines_done;
        if (line_captured_lines[side].w == p_pisces_capture[side]->lines_done) {
            break;
        }
        _builtin_set_imask(line_page_step_imask);
    }
}
extern unsigned short job_auto_size_flags;

void line_run_page_step_part_4(int side)
{
    unsigned char capture_buf;
    if (line_page_state[side].end_line != 0xffff) {
        return;
    }
    if (job_auto_size_flags & 0xc000) {
        if (line_side_state[side].state_flags.v & 0x20) {
            capture_buf = page_side_state[side].capture_buf;
            line_page_state[side].end_line = edge_side_records[side].e[capture_buf].event_20_lines + (short)line_page_state[side].top_offset_lines + 16;
        }
    } else {
        if (line_side_state[side].state_flags.v & 0x100) {
            capture_buf = page_side_state[side].capture_buf;
            line_page_state[side].end_line = edge_side_records[side].e[capture_buf].top_lines + (short)line_page_state[side].top_offset_lines + img_top_filler_lines + 16;
        }
    }
}


void line_run_long_page_step(int side, unsigned short lines)
{
    unsigned short value;
    unsigned int level;
    switch (line_long_page_state) {
    case 0x80:
        value = rt_divu16(img_side_params[side].x_res_dpi * 1180, 600);
        value >>= 1;
        p_pisces_edge[side]->ctrl.g = 7;
        p_pisces_edge[side]->x_start.v = img_side_params[side].capture_width_px / 2 - (short)value;
        p_pisces_edge[side]->x_end.v = (short)value + img_side_params[side].capture_width_px / 2;
        line_long_page_level_sum = 0;
        line_long_page_level_count = 0;
        line_long_page_level_index = 0;
        line_long_page_state = 1;
        break;
    case 1:
        if (job_sheet_count == 0) {
            value = rt_divu16(img_side_params[side].x_res_dpi * 94, 600);
            if (value > lines) {
                break;
            }
        }
        if (lines < line_long_page_sample_lines) {
            level = p_pisces_edge[side]->level;
            level = (unsigned char)(level >> 8);
            line_long_page_level_sum += level;
            line_long_page_level_count++;
            break;
        }
        line_long_page_sample_lines = 0;
        line_long_page_state = 0;
        break;
    default:
        value = rt_divu16(img_side_params[side].long_page_limit_lines, 100);
        if (lines % value == 0) {
            value = p_pisces_edge[side]->level;
            value = (unsigned char)(value >> 8);
            edge_left_back[line_long_page_level_index] = value;
            line_long_page_level_index++;
        }
        break;
    }
}
#undef job_auto_size_flags

/* ---- H3 (PH): line_end_page. */
#pragma section H
#define line_captured_lines line_captured_lines_words

struct view_line_captured_lines { unsigned short w[2]; };
extern struct view_line_captured_lines line_captured_lines;

void line_end_page(int side)
{
    unsigned char capture_buf;
    unsigned short top_lines;
    unsigned short limit;
    struct edge_record *edge;

    _builtin_set_imask(15);
    if ((maint_mode_word.c.mode == MAINT_MODE_SUB_MAGNIFICATION || maint_mode_word.c.mode == MAINT_MODE_OFFSET || testmode_number.v == 33 || testmode_number.v == 32)
        ) {
        if (line_captured_lines.w[side] >= img_side_params[side].scan_length_lines) {
        } else {
            _builtin_set_imask(line_page_step_imask);
            return;
        }
    }
    capture_buf = page_side_state[side].capture_buf;
    if (side == 0 && hw_hostif_flags.f.own_hostif && (unsigned short)edge_side_records[side].e[capture_buf].top_lines != 0xffff) {
        if (img_overscan_shift_on[0]) {
            if (((unsigned short (*)[22])edge_top_lines_front)[capture_buf][0] > img_overscan_shift_lines[0][capture_buf]) {
                top_lines = ((unsigned short (*)[22])edge_top_lines_front)[capture_buf][0] - img_overscan_shift_lines[0][capture_buf];
            } else {
                top_lines = 0;
            }
            if (img_crop_area[0].y_start_lines > top_lines) {
                line_page_state[0].state_flags.v = 0;
                line_page_state[0].state_flags.f.page_error = 1;
                _builtin_set_imask(line_page_step_imask);
                return;
            }
        } else {
            if (img_crop_area[0].y_start_lines > ((unsigned short (*)[22])edge_top_lines_front)[capture_buf][0]) {
                line_page_state[0].state_flags.v = 0;
                line_page_state[0].state_flags.f.page_error = 1;
                _builtin_set_imask(line_page_step_imask);
                return;
            }
        }
        if (win_sides.f.selection) {
            if (img_overscan_shift_on[1]) {
                if (edge_top_lines_back[capture_buf].w > img_overscan_shift_lines[1][capture_buf]) {
                    top_lines = edge_top_lines_back[capture_buf].w - img_overscan_shift_lines[1][capture_buf];
                } else {
                    top_lines = 0;
                }
                if (img_crop_area[1].y_start_lines > top_lines) {
                    line_page_state[0].state_flags.v = 0;
                    line_page_state[0].state_flags.f.page_error = 1;
                    _builtin_set_imask(line_page_step_imask);
                    return;
                }
            } else {
                if (img_crop_area[1].y_start_lines > ((unsigned short (*)[22])edge_top_lines_front)[capture_buf][0]) {
                    line_page_state[0].state_flags.v = 0;
                    line_page_state[0].state_flags.f.page_error = 1;
                    _builtin_set_imask(line_page_step_imask);
                    return;
                }
            }
        }
    }
    page_side_state[side].buf[capture_buf].flags |= 0x40;
    line_page_state[side].state_flags.v = 0;
    line_page_state[side].end_line = 0xffff;
    line_long_page_state = 0;
    line_stop_capture(side);
    edge_side_records[side].e[capture_buf].page_end_lines = line_captured_lines.w[side];
    if ((unsigned short)edge_side_records[side].e[capture_buf].top_lines == 0xffff) {
        edge_side_records[side].e[capture_buf].top_lines = line_captured_lines.w[side];
    }
    if ((unsigned short)edge_side_records[side].e[capture_buf].event_20_lines == 0xffff) {
        edge_side_records[side].e[capture_buf].event_20_lines = line_captured_lines.w[side];
    }
    edge = &edge_side_records[side].e[capture_buf];
    if (job_auto_size_flags.b7) {
        edge->deskew_end_lines = line_captured_lines.w[side];
        p_pisces_ctrl[side]->ctrl.f.sdram_window = 0;
        if (win_long_length) {
            top_lines = ((unsigned short (*)[22])edge_top_lines_front)[capture_buf][0];
            limit = img_side_params[side].y_res_dpi * 18;
            if (top_lines > limit) {
                limit = 0xffff;
            } else {
                limit = 0;
            }
            if (limit) {
                edge->state = EDGE_STATE_FALLBACK;
                _builtin_set_imask(line_page_step_imask);
                return;
            }
        }
        if (edge->search_failed) {
            edge->state = EDGE_STATE_FALLBACK;
            _builtin_set_imask(line_page_step_imask);
            return;
        }
        if (edge->top_corner_line == 0) {
            edge->state = EDGE_STATE_FALLBACK;
            settings_edge_fail_code[0] = 48;
        } else if (edge->state < EDGE_STATE_SKEW_DIRECTION || edge->bottom_line == 0 || edge->bottom_corner_line == 0) {
            edge->state = EDGE_STATE_SKEW_DIRECTION;
            if (!edge->bottom_line) {
                if (job_auto_size_flags.b2) {
                    edge->state = EDGE_STATE_FALLBACK;
                    _builtin_set_imask(line_page_step_imask);
                    settings_edge_fail_code[0] = 54;
                    return;
                }
                edge->bottom_line = line_captured_lines.w[side];
            }
            if (!edge->bottom_corner_line) {
                edge->bottom_corner_line = edge->bottom_line;
            } else {
                top_lines = (edge->bottom_line + edge->top_line) / 2;
                if (edge->bottom_corner_line < top_lines) {
                    edge->bottom_corner_line = top_lines;
                }
            }
            if (!edge->bottom_corner_line) {
                edge->bottom_corner_line = edge->bottom_line;
            }
        }
    } else {
        edge->state = 0x100;
    }
    _builtin_set_imask(line_page_step_imask);
    if (!win_params.flags.f.adf) {
        mdc_request_stop();
    }
}
#undef line_captured_lines
#pragma section
