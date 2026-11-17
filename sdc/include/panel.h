/* panel.h: declarations of panel.c, panel_expander_init.c, panel_expander_init_dark.c,
   panel_expander_reset.c, panel_expander_port1.c, panel_expander_update.c. */
#ifndef PANEL_H
#define PANEL_H
#include "common.h"

/* panel_current.display_sub in PANEL_MODE_ERROR: the code the panel shows (err_display_codes; the operator's guide's error codes; PANEL_ERROR_NONE*: nothing shown) */
#define PANEL_ERROR_U0             0x00
#define PANEL_ERROR_J1             0x01
#define PANEL_ERROR_J2             0x02
#define PANEL_ERROR_UNUSED_3       0x03
#define PANEL_ERROR_U4             0x04
#define PANEL_ERROR_NONE           0x05
#define PANEL_ERROR_E0             0x10
#define PANEL_ERROR_E1             0x11
#define PANEL_ERROR_E2             0x12
#define PANEL_ERROR_E3             0x13
#define PANEL_ERROR_E6             0x16
#define PANEL_ERROR_E7             0x17
#define PANEL_ERROR_E8             0x18
#define PANEL_ERROR_E9             0x19
#define PANEL_ERROR_U6             0x06
#define PANEL_ERROR_U7             0x07
#define PANEL_ERROR_H1             0x14
#define PANEL_ERROR_H7             0x15
#define PANEL_ERROR_A              0x1A
#define PANEL_ERROR_C0             0x1C
#define PANEL_ERROR_NONE_INTERFACE 0x1D
#define PANEL_ERROR_F2_F4          0x1F
#define PANEL_ERROR_NONE_FLASH     0x20
#define PANEL_ERROR_F6             0x21
#define PANEL_ERROR_HEATER         0x22
#define PANEL_ERROR_E9_2           0x25
#define PANEL_ERROR_NONE_IPC       0x26
#define PANEL_ERROR_H6             0x27
#define PANEL_ERROR_SENSOR         0x28
#define PANEL_ERROR_C0_2           0x29
#define PANEL_ERROR_C8             0x2A
#define PANEL_ERROR_H5             0x2B
#define PANEL_ERROR_NONE_PAUSED    0x32
/* panel_current.display_main: the panel's display mode (panel_set_display_mode's second argument) */
#define PANEL_MODE_FUNCTION    2
#define PANEL_MODE_ERROR       6
#define PANEL_MODE_MAINT       8
#define PANEL_MODE_MAINT_STATE 10
#define PANEL_MODE_MAINT_INFO  12
/* panel_current.chars[]: glyph numbers of the panel's 7-segment font (0-9 are the digits) */
#define PANEL_GLYPH_A                10
#define PANEL_GLYPH_B                11
#define PANEL_GLYPH_C                12
#define PANEL_GLYPH_E                14
#define PANEL_GLYPH_F                15
#define PANEL_GLYPH_H                16
#define PANEL_GLYPH_J                17
#define PANEL_GLYPH_L                18
#define PANEL_GLYPH_P                19
#define PANEL_GLYPH_U                20
#define PANEL_GLYPH_SMALL_C          21
#define PANEL_GLYPH_SMALL_O          22
#define PANEL_GLYPH_BLANK            24
#define PANEL_GLYPH_MAINT_MODE_1     27
#define PANEL_GLYPH_SEGMENT_PATTERNS 35
#define PANEL_GLYPH_UPPER_O          162
#define PANEL_GLYPH_DASH             163
#define PANEL_GLYPH_UNDERSCORE       164
/* iic_panel_set_port0: the LED bits of expander port 0 */
#define PANEL_LED_POWER 0x10
#define PANEL_LED_CHECK 0x20
/* panel poll counts (one poll per panel tick) */
#define PANEL_ERROR_CODE_HOLD_POLLS 375
#define PANEL_ERROR_CODE_RELEASE_POLLS 250
#define PANEL_CHAR_POLLS               63
#define PANEL_MULTIFEED_BLINK_SLOW     150
#define PANEL_MULTIFEED_BLINK_FAST     31

/* functions */
extern void panel_run(void);
extern int panel_step_sequence(void);
extern void panel_set_display_mode(int, int);
extern void panel_show_function_number(void);
extern void panel_show_mode_2_1(void);
extern void panel_show_mode_2_2(void);
extern void panel_show_mode_2_3(void);
extern void panel_show_mode_2_4(void);
extern void panel_show_mode_2_5(void);
extern void panel_show_mode_2_6(void);
extern void panel_show_mode_2_7(void);
extern int panel_show_error(void);
extern void panel_show_maint_mode_1(void);
extern void panel_show_maint_mode_2(void);
extern void panel_show_maint_mode_3(void);
extern void panel_show_maint_mode_4(void);
extern void panel_show_maint_mode_5(void);
extern void panel_show_maint_mode_6(void);
extern void panel_show_maint_mode_7(void);
extern void panel_show_maint_mode_8(void);
extern void panel_show_maint_mode_10(void);
extern void panel_show_maint_mode_0(void);
extern void panel_show_maint_screen(void);
extern void panel_show_maint_mode_number_blinking(void);
extern void panel_show_maint_item_blinking(void);
extern void panel_show_maint_failed(void);
extern void panel_show_maint_adjusted(void);
extern void panel_show_maint_confirm(void);
extern void panel_show_maint_writing(void);
extern void panel_show_maint_written(void);
extern void panel_show_firmware_versions(void);
extern void panel_show_first_read_date(void);
extern void panel_show_adf_count(void);
extern void panel_show_flatbed_count(void);
extern void panel_show_pick_roller_count(void);
extern void panel_show_brake_roller_count(void);
extern void panel_show_adf_count_2(void);
extern void panel_show_mode_12_7(void);
extern void panel_show_mode_12_8(void);
extern void panel_show_error_history(void);
extern void panel_show_counter(unsigned short count, unsigned char low_count);
extern void panel_show_emulation_model(void);
extern void panel_check_error_code_buttons(struct Bits);
extern void panel_show_error_code(void);
extern void panel_check_setup_mode(void);
extern int panel_run_multifeed_blink(void);
extern void panel_init_expander(void);
extern void panel_init_expander_dark(void);
extern int panel_expander_reset_outputs(void);
extern int panel_expander_clear_port1(void);
extern int panel_update_expander(void);

/* constants */
extern const unsigned char panel_glyphs[1];
extern const char panel_glyph_1_pattern[1];
extern const char panel_glyph_2_pattern[1];
extern const char panel_glyph_3_pattern[1];
extern const char panel_glyph_4_pattern[1];
extern const char panel_glyph_5_pattern[1];
extern const char panel_glyph_6_pattern[2];
extern const char panel_glyph_8_pattern[];
extern const char panel_glyph_9_pattern[3];
extern const char panel_glyph_c_pattern[2];
extern const char panel_glyph_e_pattern[1];
extern const unsigned char panel_glyph_f_pattern[1];
extern const char panel_glyph_h_pattern[3];
extern const unsigned char panel_glyph_p_pattern[3];
extern const char panel_glyph_small_o_pattern[2];
extern const unsigned char panel_glyph_blank_pattern[1];
extern const unsigned char panel_glyph_send_to_pattern[1];
extern const unsigned char panel_glyph_scan_pattern[136];
extern const unsigned char panel_glyph_upper_o_pattern[5];
extern const unsigned char panel_hex_glyphs[1];
extern const unsigned char panel_hex_glyph_1[1];
extern const unsigned char panel_hex_glyph_2[1];
extern const unsigned char panel_hex_glyph_3[1];
extern const unsigned char panel_hex_glyph_4[1];
extern const unsigned char panel_hex_glyph_5[1];
extern const unsigned char panel_hex_glyph_6[1];
extern const unsigned char panel_hex_glyph_7[1];
extern const unsigned char panel_hex_glyph_8[1];
extern const unsigned char panel_hex_glyph_9[7];
extern const unsigned char panel_function_glyphs[10];
extern const unsigned char panel_sensor_segments[1];
extern const unsigned char panel_segment_adf_cover[1];
extern const unsigned char panel_segment_top_sensor[2];
extern const unsigned char panel_segment_pick_sensor[1];
extern const unsigned char panel_segment_document_cover[1];
extern const unsigned char panel_segment_df_sensor[128];
extern const unsigned char panel_maint_error_glyphs[25];
extern void (*panel_function_screens[8]) (void);
extern void (*panel_maint_mode_screens[12]) (void);
extern void (*panel_maint_info_screens[10]) (void);
extern void (*panel_maint_state_screens[7]) (void);
extern unsigned char *panel_error_history_entries[3];

#endif
