#define panel_set_display_mode panel_set_display_mode_hdr
#include "fw.h"
#undef panel_set_display_mode
#pragma section H

void panel_set_display_mode(unsigned char, unsigned char);


#define SETBC(v) t = (v); if (panel_current.glyph_shown != t) { if (iic_panel_set_digit(t)) panel_current.glyph_shown = t; }

/* Constants (section C). */
/* 7-segment glyph per index (panel_current.chars): bit 0 d, 1 e, 2 g, 3 f, 4 a, 5 b, 6 c; 0 = lit, bit 7 unused.
   0-15 hex digits, 16 H, 17 J, 18 L, 19 P, 20 U, 21 c, 22 o, 23 all off, 24 blank, 25/26 upper/lower half,
   27-34 maintenance mode pictures, 35-161 every segment combination, 162 upper o, 163 -, 164 _ */
const unsigned char panel_glyphs[1] = {4};
const char panel_glyph_1_pattern[1] = {0x1f};
const char panel_glyph_2_pattern[1] = {0x48};
const char panel_glyph_3_pattern[1] = {0xa};
const char panel_glyph_4_pattern[1] = {0x13};
const char panel_glyph_5_pattern[1] = {0x22};
const char panel_glyph_6_pattern[2] = {0x20, 0xf};
const char panel_glyph_8_pattern[1] = {0};
const char panel_glyph_9_pattern[3] = {2, 1, 0x30};
const char panel_glyph_c_pattern[2] = {0x64, 0x18};
const char panel_glyph_e_pattern[1] = {0x60};
const unsigned char panel_glyph_f_pattern[1] = {0x61};
const char panel_glyph_h_pattern[3] = {0x11, 0x1c, 0x74};
const unsigned char panel_glyph_p_pattern[3] = {0x41, 0x14, 0x78};
const char panel_glyph_small_o_pattern[2] = {0x38, -1};
const unsigned char panel_glyph_blank_pattern[1] = {0x7f};
const unsigned char panel_glyph_send_to_pattern[1] = {0x47};
const unsigned char panel_glyph_scan_pattern[136] = "9okjbB@\x08(w}_?uW7]=\x1fU5\x17\x1d\x15{sy[;qS3Y9\x1bQ1\x13\x19\x11~v|^>tV6\\<\x1eT4\x16\x1c\x14zrxZ:pR2X8\x1aP0\x12\x18\x10ogmO/eG'M-\x0f" "E%\x07\r\x05kciK+aC#I)\x0b" "A!\x03\t\x01nflN.dF&L,\x0e" "D$\x06\x0c\x04jbhJ*`B\"H(\n@ \x02\x08\x10";
const unsigned char panel_glyph_upper_o_pattern[5] = "C{~o@";
const unsigned char panel_hex_glyphs[1] = {0};
const unsigned char panel_hex_glyph_1[1] = {1};
const unsigned char panel_hex_glyph_2[1] = {2};
const unsigned char panel_hex_glyph_3[1] = {3};
const unsigned char panel_hex_glyph_4[1] = {4};
const unsigned char panel_hex_glyph_5[1] = {5};
const unsigned char panel_hex_glyph_6[1] = {6};
const unsigned char panel_hex_glyph_7[1] = {7};
const unsigned char panel_hex_glyph_8[1] = {8};
const unsigned char panel_hex_glyph_9[7] = {9, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
/* function number 0 is shown as C */
const unsigned char panel_function_glyphs[10] = {0x0c, 1, 2, 3, 4, 5, 6, 7, 8, 9};
/* sensor test segments (service manual 6.1.2, T11): f Empty, e ADF cover, b TOP, c multifeed (unused), g pick,
   d document cover, a DF sensor; then a copy of panel_glyphs[35..161] to look the pattern up */
const unsigned char panel_sensor_segments[1] = {8};
const unsigned char panel_segment_adf_cover[1] = {2};
const unsigned char panel_segment_top_sensor[2] = {0x20, 0x40};
const unsigned char panel_segment_pick_sensor[1] = {4};
const unsigned char panel_segment_document_cover[1] = {1};
const unsigned char panel_segment_df_sensor[128] = "\x10w}_?uW7]=\x1fU5\x17\x1d\x15{sy[;qS3Y9\x1bQ1\x13\x19\x11~v|^>tV6\\<\x1eT4\x16\x1c\x14zrxZ:pR2X8\x1aP0\x12\x18\x10ogmO/eG'M-\x0f" "E%\x07\r\x05kciK+aC#I)\x0b" "A!\x03\t\x01nflN.dF&L,\x0e" "D$\x06\x0c\x04jbhJ*`B\"H(\n@ \x02\x08\x10";
/* error screen T26: error n (1-7) lights one segment; bytes 7-24 are not read */
const unsigned char panel_maint_error_glyphs[25] = {
    0xa5, 0x23, 0xa3, 0x25, 0x24, 0xa4, 0x26, 9, 8, 8, 7, 7, 6, 5, 4, 3,
    3, 2, 2, 1, 1, 0, 0, 0, 0
};

/* Every 8 ms after button_poll; the first case that applies draws: PF1 sequence, setup mode, multifeed blink,
   power-saving blank, start-up screens, button acknowledgement, Function press, else the current screen. */
void panel_run(void)
{
    unsigned char t;
    int blinking;
    if (power_pf1_sequence_step > 1)
        return;
    if (panel_error_code_shown) {
        panel_step_sequence();
        return;
    }
    panel_check_setup_mode();
    if (panel_setup_shown) {
        if ((unsigned)tick_panel_polls.v > (unsigned)panel_setup_blink_polls) {
            tick_panel_polls.v = 0;
            if (panel_setup_blink_phase.v == 0) {
                iic_panel_set_digit(panel_glyph_blank_pattern[0]);
                panel_setup_blink_phase.v = 1;
            } else if (panel_setup_blink_phase.v) {
                iic_panel_set_port0(PANEL_LED_POWER);
                panel_setup_blink_phase.v = 0;
            }
        }
        return;
    }
    if (!power_state.f.saving) {
        blinking = panel_run_multifeed_blink();
        if (blinking)
            return;
    }
    if (!power_state.f.leave_request && (power_state.f.saving || power_state.f.unused) && !err_mdc_alarm && !power_mdc_restart_state.f.restart_running && !mdc_fan_alarm.b6) {
        panel_current.display_flags.b6 = 1;
        t = panel_glyph_blank_pattern[0];
        if (panel_current.glyph_shown != t && iic_panel_set_digit(t)) {
            panel_current.glyph_shown = t;
            if (!power_state.f.saving)
                return;
        }
        return;
    }
    if (selftest_startup_done == 0) {
        panel_current.display_flags.b6 = 1;
        if (panel_current.startup_error_request.f.b7) {
            if (panel_step_sequence()) {
                panel_current.startup_error_request.f.b7 = 0;
                panel_current.startup_error_shown.f.b7 = 1;
            }
            return;
        }
        if (panel_current.startup_error_request.f.b6) {
            if (panel_step_sequence()) {
                panel_current.startup_error_request.f.b6 = 0;
                panel_current.startup_error_shown.f.b6 = 1;
            }
            return;
        }
        if (!(settings_byte_0x7f[0] & 128) && panel_current.startup_error_request.f.b5) {
            if (panel_step_sequence()) {
                panel_current.startup_error_request.f.b5 = 0;
                panel_current.startup_error_shown.f.b5 = 1;
            }
            return;
        }
        if (panel_current.startup_error_request.f.b4) {
            if (panel_step_sequence()) {
                panel_current.startup_error_request.f.b4 = 0;
                panel_current.startup_error_shown.f.b4 = 1;
            }
            return;
        }
        if (panel_current.startup_error_request.f.b0) {
            SETBC(panel_glyphs[0]);
            return;
        }
        if (panel_current.startup_error_request.f.b3) {
            SETBC(panel_glyph_f_pattern[0]);
            return;
        }
        SETBC(panel_glyph_p_pattern[0]);
        return;
    }
    {
        if (panel_current.page_activity && !(maint_mode_word.v & 0x5000)) {
            SETBC(panel_glyphs[panel_function_glyphs[panel_current.function_number]]);
            return;
        }
        if (testmode_number.v == 3 || testmode_number.v == 37) {
            if (panel_current.char_count <= 1)
                return;
            else
                panel_step_sequence();
            return;
        } else
        if (!(maint_mode_word.v & 0x5000)) {
        if (!(err_display_flags.v & 64)) {
        if (panel_current.button_scan_latch && panel_current.button_send_to_latch) {
            SETBC(panel_glyph_scan_pattern[0] & panel_glyph_send_to_pattern[0]);
            return;
        }
        if (panel_current.button_scan_latch) {
            SETBC(panel_glyph_scan_pattern[0]);
            return;
        }
        if (panel_current.button_send_to_latch) {
            SETBC(panel_glyph_send_to_pattern[0]);
            return;
        }
        if (panel_current.button_function_request) {
            panel_current.button_function_request = 0;
            if (++panel_current.function_number > 9)
                panel_current.function_number = 0;
            SETBC(panel_glyphs[panel_function_glyphs[panel_current.function_number]]);
            panel_current.display_flags.b6 = 1;
            panel_set_display_mode(0, PANEL_MODE_FUNCTION);
            return;
        }
        if (panel_current.button_function_back_request) {
            panel_current.button_function_back_request = 0;
            if (panel_current.function_number == 0)
                panel_current.function_number = 9;
            else
                panel_current.function_number--;
            SETBC(panel_glyphs[panel_function_glyphs[panel_current.function_number]]);
            panel_current.display_flags.b6 = 1;
            panel_set_display_mode(0, PANEL_MODE_FUNCTION);
            return;
        }
        }
        }
        if (panel_current.char_count <= 1) {
            SETBC(panel_glyphs[panel_current.chars[0]]);
        } else
            panel_step_sequence();
    }
}

/* Shows chars[char_index] for char_polls polls each, then blank for pause_polls, and repeats; 1 once repeat_limit runs are done. */
int panel_step_sequence(void)
{
    unsigned char t;
    int finished;
    finished = 0;
    if (panel_current.repeat_limit && panel_current.repeat_count >= panel_current.repeat_limit)
        return 1;
    if (panel_current.char_index >= panel_current.char_count) {
        t = panel_glyph_blank_pattern[0];
    } else {
        t = panel_glyphs[panel_current.chars[panel_current.char_index]];
    }
    SETBC(t);
    if (panel_current.char_index < panel_current.char_count) {
        if ((unsigned)tick_panel_polls.v > (unsigned)panel_current.char_polls) {
            if (++panel_current.char_index >= panel_current.char_count && !panel_current.pause_polls) {
                panel_current.char_index = 0;
                panel_current.repeat_count++;
                if (panel_current.repeat_count >= panel_current.repeat_limit)
                    finished = 1;
            }
            tick_panel_polls.v = 0;
        }
    } else if (panel_current.char_index == panel_current.char_count) {
        if ((unsigned)tick_panel_polls.v > (unsigned)panel_current.pause_polls) {
            panel_current.char_index = 0;
            tick_panel_polls.v = 0;
            panel_current.repeat_count++;
            if (panel_current.repeat_count >= panel_current.repeat_limit)
                finished = 1;
        }
    }
    return 0;
}

/* display_main 6 error, 8 maintenance mode screens, 10 maintenance states, 12 read-outs, else 2 (normal); display_sub picks the screen. */
void panel_set_display_mode(unsigned char display_sub, unsigned char display_main)
{
    unsigned char old_sub, old_main;
    if (panel_current.display_main == display_main && panel_current.display_sub == display_sub) {
        if (!panel_current.display_flags.b6 && panel_current.display_main != PANEL_MODE_MAINT)
            return;
    }
    panel_current.display_flags.b6 = 0;
    old_main = panel_current.display_main;
    old_sub = panel_current.display_sub;
    panel_current.display_main = display_main;
    panel_current.display_sub = display_sub;
    switch (display_main) {
    case PANEL_MODE_ERROR:
        if (!panel_show_error()) {
            panel_current.display_main = old_main;
            panel_current.display_sub = old_sub;
        }
        iic_panel_set_port0(PANEL_LED_POWER | PANEL_LED_CHECK);  /* Power and Check LED */
        break;
    case PANEL_MODE_MAINT:
        panel_maint_mode_screens[display_sub]();
        if (maint_mode_word.c.mode != MAINT_MODE_EMULATION)
            iic_panel_set_port0(PANEL_LED_POWER);
        break;
    case PANEL_MODE_MAINT_STATE:
        panel_maint_state_screens[display_sub]();
        break;
    case PANEL_MODE_MAINT_INFO:
        panel_maint_info_screens[display_sub]();
        break;
    case PANEL_MODE_FUNCTION:
    default:
        panel_function_screens[display_sub]();
        iic_panel_set_port0(PANEL_LED_POWER);  /* Power LED only */
        break;
    }
}

void panel_show_function_number(void)
{
    panel_current.char_count = 0;
    panel_current.chars[0] = panel_function_glyphs[panel_current.function_number];
    panel_current.display_flags.b7 = 0;
}

void panel_show_mode_2_1(void)
{
    panel_current.char_count = 2;
    panel_current.chars[0] = 0;
    panel_current.chars[1] = PANEL_GLYPH_BLANK;
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = 126;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
    panel_current.display_flags.b6 = 1;
    panel_current.display_sub = 0;
}

void panel_show_mode_2_2(void)
{
}

void panel_show_mode_2_3(void)
{
}

void panel_show_mode_2_4(void)
{
}

void panel_show_mode_2_5(void)
{
}

void panel_show_mode_2_6(void)
{
}

void panel_show_mode_2_7(void)
{
}

/* Error display (main mode 6): chars[0] the letter (10 A, 12 C, 14 E, 15 F, 16 H, 17 J, 18 L, 20 U), chars[2] the
   digit from panel_hex_glyphs (0) .. panel_hex_glyph_9 (9); err_display_codes gives the sub-mode per error code. */
int panel_show_error(void)
{
    if (panel_current.display_sub == PANEL_ERROR_U0) {
        panel_current.chars[0] = PANEL_GLYPH_U;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyphs[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_J1) {
        panel_current.chars[0] = PANEL_GLYPH_J;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_1[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        if (err_last_code.s.v == ERR_JAM_DIRTY_ROLLERS)
            panel_current.chars[2] = panel_hex_glyph_9[0];
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_J2) {
        panel_current.chars[0] = PANEL_GLYPH_J;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_2[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_UNUSED_3) {
    }
    else if (panel_current.display_sub == PANEL_ERROR_U4) {
        panel_current.chars[0] = PANEL_GLYPH_U;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        if (hw_option_flags.f.b0)
            panel_current.chars[2] = panel_hex_glyph_5[0];
        else
            panel_current.chars[2] = panel_hex_glyph_4[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_NONE) {
    }
    else if (panel_current.display_sub == PANEL_ERROR_U6) {
        panel_current.chars[0] = PANEL_GLYPH_U;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_6[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_U7) {
        panel_current.chars[0] = PANEL_GLYPH_U;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_7[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_E0) {
        panel_current.chars[0] = PANEL_GLYPH_E;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyphs[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_E1) {
        panel_current.chars[0] = PANEL_GLYPH_E;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_1[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_E2) {
        panel_current.chars[0] = PANEL_GLYPH_E;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_2[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_E3) {
        panel_current.chars[0] = PANEL_GLYPH_E;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_3[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_H1) {
        panel_current.chars[0] = PANEL_GLYPH_H;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_1[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_H7) {
        panel_current.chars[0] = PANEL_GLYPH_H;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_7[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_E6) {
        panel_current.chars[0] = PANEL_GLYPH_E;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_6[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_E7) {
        panel_current.chars[0] = PANEL_GLYPH_E;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_7[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_E8) {
        panel_current.chars[0] = PANEL_GLYPH_E;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_8[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_E9) {
        panel_current.chars[0] = PANEL_GLYPH_E;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_9[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_A) {
        panel_current.chars[0] = PANEL_GLYPH_A;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        if (err_last_code.s.v == ERR_IMPRINTER_CONTROL_BOARD)
            panel_current.chars[2] = panel_hex_glyphs[0];
        else if (err_last_code.s.v == ERR_IMPRINTER_TIMEOUT)
            panel_current.chars[2] = panel_hex_glyph_1[0];
        else if (err_last_code.s.v == ERR_IMPRINTER_HEAD)
            panel_current.chars[2] = panel_hex_glyph_2[0];
        else if (err_last_code.s.v == ERR_IMPRINTER_EEPROM)
            panel_current.chars[2] = panel_hex_glyph_3[0];
        else if (err_last_code.s.v == ERR_IMPRINTER_ROM)
            panel_current.chars[2] = panel_hex_glyph_4[0];
        else
            panel_current.chars[2] = panel_hex_glyphs[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_C0) {
        panel_current.chars[0] = PANEL_GLYPH_C;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyphs[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_NONE_INTERFACE) {
    }
    else if (panel_current.display_sub == PANEL_ERROR_F2_F4) {
        panel_current.chars[0] = PANEL_GLYPH_F;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        if (err_last_code.s.v == ERR_PICK_ARM)
            panel_current.chars[2] = panel_hex_glyph_2[0];
        else
            panel_current.chars[2] = panel_hex_glyph_4[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_NONE_FLASH) {
    }
    else if (panel_current.display_sub == PANEL_ERROR_F6) {
        panel_current.chars[0] = PANEL_GLYPH_F;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_6[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_HEATER) { /* F7/F8/F9 heater error: 0x94 flatbed, 0x92 ADF front, 0x93 ADF back */
        panel_current.chars[0] = PANEL_GLYPH_F;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        if (err_last_code.s.v == ERR_HEATER_FLATBED)
            panel_current.chars[2] = panel_hex_glyph_7[0];
        else if (err_last_code.s.v == ERR_HEATER_ADF_FRONT)
            panel_current.chars[2] = panel_hex_glyph_8[0];
        else if (err_last_code.s.v == ERR_HEATER_ADF_BACK)
            panel_current.chars[2] = panel_hex_glyph_9[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_E9_2) {
        panel_current.chars[0] = PANEL_GLYPH_E;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_9[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_NONE_IPC) {
    }
    else if (panel_current.display_sub == PANEL_ERROR_H6) {
        panel_current.chars[0] = PANEL_GLYPH_H;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_6[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_SENSOR) { /* L6 ultrasonic sensor (error 2), L9 OMR sensor (error 3) */
        panel_current.chars[0] = PANEL_GLYPH_L;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        if (err_last_code.s.v == ERR_ULTRASONIC_SENSOR)
            panel_current.chars[2] = panel_hex_glyph_6[0];
        else if (err_last_code.s.v == ERR_DF_SENSOR)
            panel_current.chars[2] = panel_hex_glyph_9[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_C0_2) {
        panel_current.chars[0] = PANEL_GLYPH_C;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyphs[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_C8) {
        panel_current.chars[0] = PANEL_GLYPH_C;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_8[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    else if (panel_current.display_sub == PANEL_ERROR_H5) { /* H5 CGA board error */
        panel_current.chars[0] = PANEL_GLYPH_H;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.chars[2] = panel_hex_glyph_5[0];
        panel_current.chars[3] = PANEL_GLYPH_BLANK;
        panel_current.chars[4] = PANEL_GLYPH_BLANK;
        panel_current.char_count = 5;
        panel_current.char_polls = PANEL_CHAR_POLLS;
    }
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    switch (panel_current.display_sub) {
    case PANEL_ERROR_E6:
    case PANEL_ERROR_E7:
    case PANEL_ERROR_E8:
    case PANEL_ERROR_E9:
    case PANEL_ERROR_E9_2:
        panel_current.repeat_limit = 3;
        break;
    default:
        panel_current.repeat_limit = 0;
        break;
    }
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
    return 1;
}

void panel_show_maint_mode_1(void)
{
    unsigned char glyph;
    unsigned char i;
    switch (maint_screen) {
    case MAINT_SCREEN_SELECT:
        panel_current.char_count = 0;
        panel_current.chars[0] = PANEL_GLYPH_MAINT_MODE_1;
        panel_current.display_flags.b7 = 0;
        break;
    case MAINT_SCREEN_ITEM:
        glyph = win_params.resolution_mode.c.lo & 15;
        panel_current.char_count = 0;
        panel_current.chars[0] = panel_hex_glyphs[glyph];
        panel_current.display_flags.b7 = 0;
        break;
    case MAINT_SCREEN_ADJUSTED:
        panel_current.char_count = 0;
        panel_current.display_flags.b7 = 0;
        glyph = 0;
        if (!mdc_mailbox.sensor_status.f.no_paper)
            glyph |= panel_sensor_segments[0];
        if (mdc_mailbox.sensor_status.f.adf_cover_open)
            glyph |= panel_segment_adf_cover[0];
        if (mdc_mailbox.sensor_status.f.top)
            glyph |= panel_segment_top_sensor[0];
        if (mdc_mailbox.sensor_status.f.pick)
            glyph |= panel_segment_pick_sensor[0];
        if (mdc_mailbox.sensor_status.f.document_cover_open)
            glyph |= panel_segment_document_cover[0];
        if (mdc_mailbox.sensor_status_2.f.df)
            glyph |= panel_segment_df_sensor[0];
        glyph = ~(glyph + 128);
        for (i = 7; i < 134; i++) {
            if (glyph == panel_sensor_segments[i])
                break;
        }
        if (i >= 134)
            glyph = PANEL_GLYPH_BLANK;
        else
            glyph = i + 28;
        panel_current.chars[0] = glyph;
        break;
    case MAINT_SCREEN_CONFIRM:
        if (panel_maint_p_flag)
            panel_current.chars[0] = PANEL_GLYPH_P;
        else
            panel_current.chars[0] = PANEL_GLYPH_DASH;
        panel_current.char_count = 0;
        panel_current.display_flags.b7 = 0;
        break;
    case MAINT_SCREEN_RUNNING:
        panel_show_maint_item_blinking();
        break;
    default:
        panel_show_maint_screen();
        break;
    }
}

void panel_show_maint_mode_2(void)
{
    int x;
    switch (maint_screen) {
    case MAINT_SCREEN_SELECT:
        panel_current.char_count = 0;
        panel_current.chars[0] = 28;
        panel_current.display_flags.b7 = 0;
        break;
    default:
        panel_show_maint_screen();
        break;
    }
}

void panel_show_maint_mode_3(void)
{
    switch (maint_screen) {
    case MAINT_SCREEN_SELECT:
        panel_current.char_count = 0;
        panel_current.chars[0] = 29;
        panel_current.display_flags.b7 = 0;
        break;
    default:
        panel_show_maint_screen();
        break;
    }
}

void panel_show_maint_mode_4(void)
{
    switch (maint_screen) {
    case MAINT_SCREEN_SELECT:
        panel_current.char_count = 0;
        panel_current.chars[0] = 30;
        panel_current.display_flags.b7 = 0;
        break;
    default:
        panel_show_maint_screen();
        break;
    }
}

void panel_show_maint_mode_5(void)
{
    int x;
    switch (maint_screen) {
    case MAINT_SCREEN_SELECT:
        panel_current.char_count = 0;
        panel_current.chars[0] = 31;
        panel_current.display_flags.b7 = 0;
        break;
    default:
        panel_show_maint_screen();
        break;
    }
}

void panel_show_maint_mode_6(void)
{
    switch (maint_screen) {
    case MAINT_SCREEN_SELECT:
        panel_current.char_count = 0;
        panel_current.chars[0] = 32;
        panel_current.display_flags.b7 = 0;
        break;
    case MAINT_SCREEN_ITEM:
        panel_current.char_count = 0;
        panel_current.chars[0] = panel_hex_glyphs[maint_item.v];
        panel_current.display_flags.b7 = 0;
        break;
    default:
        panel_show_maint_screen();
        break;
    }
}

void panel_show_maint_mode_7(void)
{
    switch (maint_screen) {
    case MAINT_SCREEN_SELECT:
        panel_current.char_count = 0;
        panel_current.chars[0] = 33;
        panel_current.display_flags.b7 = 0;
        break;
    default:
        panel_show_maint_screen();
        break;
    }
}

void panel_show_maint_mode_8(void)
{
    panel_current.char_count = 0;
    panel_current.chars[0] = 34;
    panel_current.display_flags.b7 = 0;
}

void panel_show_maint_mode_10(void)
{
    switch (maint_screen) {
    case MAINT_SCREEN_SELECT:
        panel_current.char_count = 0;
        panel_current.chars[0] = PANEL_GLYPH_A;
        panel_current.display_flags.b7 = 0;
        break;
    case MAINT_SCREEN_ITEM:
        panel_current.char_count = 0;
        panel_current.chars[0] = panel_hex_glyphs[maint_item.v];
        panel_current.display_flags.b7 = 0;
        break;
    case MAINT_SCREEN_ADJUSTED:
        panel_show_emulation_model();
        break;
    default:
        panel_show_maint_screen();
        break;
    }
}

void panel_show_maint_mode_0(void)
{
    switch (maint_screen) {
    case MAINT_SCREEN_SELECT:
        panel_current.char_count = 0;
        panel_current.chars[0] = PANEL_GLYPH_B;
        panel_current.display_flags.b7 = 0;
        break;
    case MAINT_SCREEN_ITEM:
        panel_current.char_count = 2;
        panel_current.chars[0] = PANEL_GLYPH_B;
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.display_flags.b7 = 0;
        panel_current.char_index = 0;
        panel_current.repeat_count = 0;
        panel_current.repeat_limit = 0;
        panel_current.char_polls = PANEL_CHAR_POLLS;
        panel_current.pause_polls = 0;
        tick_panel_polls.v = 0;
        break;
    case MAINT_SCREEN_ADJUSTED:
        break;
    default:
        panel_show_maint_screen();
        break;
    }
}

void panel_show_maint_screen(void)
{
    switch (maint_screen) {
    case MAINT_SCREEN_ITEM:
        panel_current.char_count = 0;
        panel_current.chars[0] = panel_hex_glyphs[maint_item.v];
        panel_current.display_flags.b7 = 0;
        break;
    case MAINT_SCREEN_ADJUSTED:
        panel_show_maint_adjusted();
        break;
    case MAINT_SCREEN_CONFIRM:
        panel_show_maint_confirm();
        break;
    case MAINT_SCREEN_WRITING:
        panel_show_maint_writing();
        break;
    case MAINT_SCREEN_WRITTEN:
        panel_show_maint_written();
        break;
    case MAINT_SCREEN_FAILED:
        panel_show_maint_failed();
        break;
    case MAINT_SCREEN_RUNNING:
        panel_show_maint_item_blinking();
        break;
    case MAINT_SCREEN_ERROR_1:
    case 0xe2:
    case 0xe3:
    case 0xe4:
    case 0xe5:
    case 0xe6:
    case 0xe7:
        panel_current.char_count = 2;
        panel_current.chars[0] = panel_maint_error_glyphs[(maint_screen & 7) - 1];
        panel_current.chars[1] = PANEL_GLYPH_BLANK;
        panel_current.display_flags.b7 = 0;
        panel_current.char_index = 0;
        panel_current.repeat_count = 0;
        panel_current.repeat_limit = 0;
        panel_current.char_polls = PANEL_CHAR_POLLS;
        panel_current.pause_polls = 0;
        tick_panel_polls.v = 0;
        break;
    case MAINT_SCREEN_SAVE_EEPROM:
        panel_current.char_count = 0;
        panel_current.chars[0] = PANEL_GLYPH_H;
        panel_current.display_flags.b7 = 0;
        break;
    default:
        panel_show_maint_screen();
        break;
    }
}

void panel_show_maint_mode_number_blinking(void)
{
    panel_current.char_count = 2;
    panel_current.chars[0] = panel_hex_glyphs[maint_mode_word.c.mode];
    panel_current.chars[1] = PANEL_GLYPH_BLANK;
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = 25;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
}

void panel_show_maint_item_blinking(void)
{
    panel_current.char_count = 2;
    panel_current.chars[0] = panel_hex_glyphs[maint_item.v];
    panel_current.chars[1] = PANEL_GLYPH_BLANK;
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = PANEL_CHAR_POLLS;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
}

void panel_show_maint_failed(void)
{
    panel_current.char_count = 0;
    panel_current.chars[0] = PANEL_GLYPH_SMALL_C;
    panel_current.display_flags.b7 = 0;
}

void panel_show_maint_adjusted(void)
{
    panel_current.char_count = 0;
    panel_current.chars[0] = PANEL_GLYPH_SMALL_O;
    panel_current.display_flags.b7 = 0;
}

void panel_show_maint_confirm(void)
{
    panel_current.char_count = 2;
    panel_current.chars[0] = PANEL_GLYPH_SMALL_O;
    panel_current.chars[1] = PANEL_GLYPH_BLANK;
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = PANEL_CHAR_POLLS;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
}

void panel_show_maint_writing(void)
{
    panel_current.char_count = 0;
    panel_current.chars[0] = PANEL_GLYPH_L;
    panel_current.display_flags.b7 = 0;
}

void panel_show_maint_written(void)
{
    panel_current.char_count = 0;
    panel_current.chars[0] = PANEL_GLYPH_UPPER_O;
    panel_current.display_flags.b7 = 0;
}

void panel_show_firmware_versions(void)
{
    unsigned char j, pos, ch, tens;
    panel_current.char_count = 21;
    util_mem_set(&panel_current.chars[1], PANEL_GLYPH_BLANK, 20);
    panel_current.chars[0] = PANEL_GLYPH_DASH;
    panel_current.chars[10] = 82;
    j = 0;
    pos = 1;
    while (j < 3) {
        ch = scsi_revision_tail[j];
        if (j == 0) {
            if (ch >= 48 && ch <= 57)
                ch -= 48;
            else if (ch >= 65 && ch <= 90)
                ch += 192;
            else
                ch = 0;
            tens = 0;
            for (tens = 0; ch > 9; ) {
                ch -= 10;
                tens++;
            }
            panel_current.chars[pos] = panel_hex_glyphs[tens];
            pos += 2;
            panel_current.chars[pos] = panel_hex_glyphs[ch];
        } else {
            panel_current.chars[pos] = ch + 208;
        }
        j++;
        pos += 2;
    }
    j = 0;
    pos = 11;
    while (j < 3) {
        ch = mdc_version[(j + 1) * 2];
        if (j == 0) {
            if (ch >= 48 && ch <= 57)
                ch -= 48;
            else if (ch >= 65 && ch <= 90)
                ch += 192;
            else
                ch = 0;
            tens = 0;
            for (tens = 0; ch > 9; ) {
                ch -= 10;
                tens++;
            }
            panel_current.chars[pos] = panel_hex_glyphs[tens];
            pos += 2;
            panel_current.chars[pos] = panel_hex_glyphs[ch];
        } else {
            panel_current.chars[pos] = ch + 208;
        }
        j++;
        pos += 2;
    }
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = PANEL_CHAR_POLLS;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
}

void panel_show_first_read_date(void)
{
    unsigned char digit, value, base, ux;
    unsigned char *p;
    int u2;
    int i;
    panel_current.char_count = 15;
    panel_current.chars[0] = PANEL_GLYPH_DASH;
    util_mem_set(&panel_current.chars[1], PANEL_GLYPH_BLANK, 14);
    base = 10;
    p = &panel_current.chars[1];
    for (i = 0; i < 3; i++) {
        if (i == 0)
            value = settings_first_read_year[0];
        else if (i == 1)
            value = settings_first_read_month[0];
        else
            value = settings_first_read_day[0];
        digit = rt_divu16(value, base);
        *p++ = panel_hex_glyphs[digit];
        p++;
        digit = value;
        while (digit >= base)
            digit -= base;
        *p++ = panel_hex_glyphs[digit];
        p++;
    }
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = PANEL_CHAR_POLLS;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
}

void panel_show_adf_count(void)
{
    panel_show_counter((settings_adf_count.c[1] << 8) | settings_adf_count.c[0], settings_adf_count_low[0]);
}

void panel_show_flatbed_count(void)
{
    panel_show_counter((settings_flatbed_count.c[1] << 8) | settings_flatbed_count.c[0], settings_flatbed_count_low[0]);
}

void panel_show_pick_roller_count(void)
{
    panel_show_counter((settings_pick_roller_count.c[1] << 8) | settings_pick_roller_count.c[0], settings_adf_count_low[0]);
}

void panel_show_brake_roller_count(void)
{
    panel_show_counter((settings_brake_roller_count.c[1] << 8) | settings_brake_roller_count.c[0], settings_adf_count_low[0]);
}

void panel_show_adf_count_2(void)
{
    panel_show_counter((settings_adf_count.c[1] << 8) | settings_adf_count.c[0], settings_adf_count_low[0]);
}

void panel_show_mode_12_7(void)
{
}

void panel_show_mode_12_8(void)
{
}

void panel_show_error_history(void)
{
    unsigned char j, pos, nibble;
    panel_current.char_count = 16;
    util_mem_set(&panel_current.chars[1], PANEL_GLYPH_BLANK, 15);
    j = 0;
    pos = 0;
    while (j < 3) {
        if (j == 0)
            panel_current.chars[pos] = PANEL_GLYPH_DASH;
        else
            panel_current.chars[pos] = PANEL_GLYPH_UNDERSCORE;
        pos++;
        nibble = *panel_error_history_entries[j];
        nibble >>= 4;
        panel_current.chars[pos] = panel_hex_glyphs[nibble];
        pos += 2;
        nibble = *panel_error_history_entries[j];
        nibble &= 15;
        panel_current.chars[pos] = panel_hex_glyphs[nibble];
        j++;
        pos += 2;
    }
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = PANEL_CHAR_POLLS;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
}

void panel_show_counter(unsigned short count, unsigned char low_count)
{
    int digit;
    unsigned sheets;
    unsigned divisor;
    unsigned char *p;
    int i;
    panel_current.char_count = 19;
    panel_current.chars[0] = PANEL_GLYPH_DASH;
    util_mem_set(&panel_current.chars[1], PANEL_GLYPH_BLANK, 18);
    sheets = count * 500 + low_count * 10;
    if (sheets < 500)
        sheets = 0;
    divisor = 10000000;
    p = &panel_current.chars[1];
    for (i = 1; i <= 8; i++) {
        digit = rt_divu32(sheets, divisor);
        *p++ = panel_hex_glyphs[(unsigned char)digit];
        p++;
        while (sheets >= divisor)
            sheets -= divisor;
        divisor = rt_divu32(divisor, 10);
    }
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = PANEL_CHAR_POLLS;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
}

void panel_show_emulation_model(void)
{
    int unused_frame;
    panel_current.char_count = 10;
    panel_current.chars[0] = PANEL_GLYPH_DASH;
    util_mem_set(&panel_current.chars[1], PANEL_GLYPH_BLANK, 11);
    switch (maint_item.v) {
    case 1:
        panel_current.chars[1] = 5;
        panel_current.chars[3] = 7;
        panel_current.chars[5] = 5;
        panel_current.chars[7] = 0;
        break;
    case 2:
        panel_current.chars[1] = 4;
        panel_current.chars[3] = 7;
        panel_current.chars[5] = 5;
        panel_current.chars[7] = 0;
        break;
    case 3:
        panel_current.chars[1] = 4;
        panel_current.chars[3] = 6;
        panel_current.chars[5] = 4;
        panel_current.chars[7] = 0;
        break;
    default:
        panel_current.chars[1] = 6;
        panel_current.chars[3] = 7;
        panel_current.chars[5] = 7;
        panel_current.chars[7] = 0;
        break;
    }
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = PANEL_CHAR_POLLS;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
}

/* button_bits = raw buttons (b0 Scan/Stop, b3 Function): both held 375 polls (3 s) save the panel and show the last
   error code in hex; either released 250 polls (2 s) restores the saved state. */
void panel_check_error_code_buttons(struct Bits button_bits)
{
    if (panel_error_code_shown != 0 && (!button_bits.b3 || !button_bits.b0)) {
        panel_error_code_polls.v += 1;
        if (panel_error_code_polls.v > PANEL_ERROR_CODE_RELEASE_POLLS) {
            panel_current = panel_saved;
            panel_current.glyph_shown += 1;
            panel_error_code_shown = 0;
        }
    } else if (panel_error_code_shown == 0 && button_bits.b3 && button_bits.b0) {
        panel_error_code_polls.v += 1;
        if (panel_error_code_polls.v > PANEL_ERROR_CODE_HOLD_POLLS) {
            panel_saved = panel_current;
            panel_error_code_shown = 1;
            panel_error_code_polls.v = 0;
            panel_show_error_code();
        }
    } else if (panel_error_code_shown == 0) {
        panel_error_code_polls.v = 0;
    }
}

void panel_show_error_code(void)
{
    unsigned char code;
    code = 0;
    panel_current.chars[0] = PANEL_GLYPH_SMALL_O;
    if (err_mdc_alarm != 0) {
        code = 233;
        panel_current.chars[0] = PANEL_GLYPH_UPPER_O;
    } else if (mdc_fan_alarm.b6 != 0) {
        code = 236;
        panel_current.chars[0] = PANEL_GLYPH_UPPER_O;
    } else if (mdc_mailbox.error_code.v != 0) {
        code = mdc_mailbox.error_code.v;
    } else if (err_last_code.s.v) {
        code = err_last_code.s.v;
        panel_current.chars[0] = PANEL_GLYPH_UPPER_O;
    }
    if (selftest_memory_error_code.v != 0)
        code = selftest_memory_error_code.v;
    panel_current.chars[1] = panel_hex_glyphs[(code & 240) >> 4];
    panel_current.chars[2] = panel_hex_glyphs[code & 15];
    panel_current.chars[3] = PANEL_GLYPH_BLANK;
    panel_current.char_count = 4;
    panel_current.display_flags.b7 = 0;
    panel_current.char_index = 0;
    panel_current.repeat_count = 0;
    panel_current.repeat_limit = 0;
    panel_current.char_polls = PANEL_CHAR_POLLS;
    panel_current.pause_polls = 0;
    tick_panel_polls.v = 0;
}

void panel_check_setup_mode(void)
{
    if (panel_setup_shown != 0 && scsi_setup_mode == 0) {
        panel_current.glyph_shown += 1;
        panel_setup_shown = 0;
    } else if (panel_setup_shown == 0 && scsi_setup_mode != 0) {
        panel_setup_shown = 1;
        panel_setup_blink_polls = 63;
        tick_panel_polls.v = 0;
    }
}

/* The function number blinking after a multifeed (operator's guide pp. 83-84): digit 75 polls, blank
   panel_multifeed_blink_polls (150 slow, 31 fast = bypass armed); returns 1 while it runs. */
int panel_run_multifeed_blink(void)
{
    char glyph;
    short digit_polls;
    int running;
    running = 0;
    /* bit 15 start request, bit 7 running, bit 8 wait for Scan/Stop, bit 6 digit shown, bit 3 patterns deleted */
    if ((panel_multifeed_blink_state.v & 128) != 0 && (panel_multifeed_blink_state.v & 32768) == 0) {
        panel_current.glyph_shown += 1;
        panel_multifeed_blink_state.v &= 65151;
    } else if ((panel_multifeed_blink_state.v & 128) == 0 && (panel_multifeed_blink_state.v & 32768) != 0) {
        panel_multifeed_blink_state.v |= 128;
        if (!hw_hostif_flags.f.own_hostif)
            panel_multifeed_blink_state.v |= 256;
        panel_multifeed_blink_polls.v = PANEL_MULTIFEED_BLINK_SLOW;
        panel_multifeed_blink_age_polls.v = 0;
        panel_current.glyph_shown += 1;
    }
    if ((panel_multifeed_blink_state.v & 128) != 0) {
        running = 1;
        panel_multifeed_blink_age_polls.v += 1;
        if ((panel_multifeed_blink_state.v & 8) != 0) {
            panel_patterns_cleared_polls.v += 1;
            if (panel_patterns_cleared_polls.v < 250) {
                iic_panel_set_digit(panel_glyph_upper_o_pattern[0]);
            } else {
                panel_multifeed_blink_state.v &= 65527;
            }
        } else if ((panel_multifeed_blink_state.v & 64) != 0) {
            if ((unsigned)tick_panel_polls.v > (unsigned)(short)panel_multifeed_blink_polls.v) {
                tick_panel_polls.v = 0;
                iic_panel_set_digit(panel_glyph_blank_pattern[0]);
                panel_multifeed_blink_state.v &= 65471;
            }
        } else {
            if (panel_multifeed_blink_polls.v == PANEL_MULTIFEED_BLINK_FAST)
                digit_polls = 31;
            else
                digit_polls = 75;
            if ((unsigned)tick_panel_polls.v > (unsigned)digit_polls) {
                tick_panel_polls.v = 0;
                glyph = panel_glyphs[(unsigned char)panel_function_glyphs[panel_current.function_number]];
                iic_panel_set_digit(glyph);
                panel_multifeed_blink_state.v |= 64;
            }
        }
    }
    return running;
}
/* Initialised data (section D). */

void (*panel_function_screens[8])(void) = {
    panel_show_function_number, panel_show_mode_2_1, panel_show_mode_2_2, panel_show_mode_2_3,
    panel_show_mode_2_4, panel_show_mode_2_5, panel_show_mode_2_6, panel_show_mode_2_7
};
void (*panel_maint_mode_screens[12])(void) = {
    panel_show_maint_mode_0, panel_show_maint_mode_1, panel_show_maint_mode_2, panel_show_maint_mode_3,
    panel_show_maint_mode_4, panel_show_maint_mode_5, panel_show_maint_mode_6, panel_show_maint_mode_7,
    panel_show_maint_mode_8, panel_show_maint_mode_1, panel_show_maint_mode_10, panel_show_maint_mode_0
};
void (*panel_maint_info_screens[10])(void) = {
    panel_show_firmware_versions, panel_show_first_read_date, panel_show_adf_count, panel_show_flatbed_count,
    panel_show_pick_roller_count, panel_show_brake_roller_count, panel_show_adf_count_2, panel_show_mode_12_7,
    panel_show_mode_12_8, panel_show_error_history
};
void (*panel_maint_state_screens[7])(void) = {
    panel_show_maint_mode_number_blinking, panel_show_maint_failed, panel_show_maint_adjusted, panel_show_maint_item_blinking,
    panel_show_maint_confirm, panel_show_maint_writing, panel_show_maint_written
};
unsigned char * panel_error_history_entries[3] = {
    settings_error_history_index + 1,
    settings_error_history_index + 2,
    settings_error_history_index + 3
};

