/* img.h: declarations of img_spline.c, img_spline_interp.c, img.c, img_fill.c. */
#ifndef IMG_H
#define IMG_H
#include "common.h"

/* img_side_mode.image_type.v bits: the image type img_decode_image_type derives from the window (exactly one bit; colour sets 14 and 13) */
#define IMG_TYPE_COLOR                          0x4000
#define IMG_TYPE_GRAY                           0x2000
#define IMG_TYPE_ERROR_DIFFUSION_SEPARATION_IPC 0x1000
#define IMG_TYPE_DITHER_SEPARATION_IPC          0x0800
#define IMG_TYPE_ERROR_DIFFUSION_SEPARATION     0x0400
#define IMG_TYPE_DITHER_SEPARATION              0x0200
#define IMG_TYPE_ERROR_DIFFUSION                0x0100
#define IMG_TYPE_DITHER                         0x0080
#define IMG_TYPE_DTC                            0x0040
#define IMG_TYPE_SDTC_IPC                       0x0020
#define IMG_TYPE_SDTC                           0x0010
#define IMG_TYPE_OUTLINE_IPC                    0x0008
#define IMG_TYPE_OUTLINE                        0x0004
#define IMG_TYPE_THRESHOLD                      0x0002
#define IMG_TYPE_THRESHOLD_IPC                  0x0001

struct win_binarize_view {
    char pad_00[2];
    struct { unsigned char :5, color:1, :2; } composition;
    char pad_03[5];
    char contrast;
    union view_scsi_initiator_bit gamma_pattern;
    char brightness_negated;
    char pad_0b[5];
    char lut_pattern;
};
/* img_side_mode.path_flags seen as its two bytes (a: bits 15-8, b: bits 7-0) */
struct img_path_flags {
    struct { unsigned char multi_level:1, :7; } hi;
    struct {
        unsigned char :1,
                      dither:1,
                      error_diffusion:1,
                      :5;
    } lo;
};
union img_halftone_select {
    unsigned short u;
    struct { unsigned char hi; char lo; } c;
    struct {
        unsigned char :1,
                      b6:1,
                      :6;
        unsigned char lo;
    } t;
    struct {
        unsigned char s:1,
                      f:3,
                      m:1,
                      c:3;
        unsigned char lo;
    } b;
    struct {
        unsigned short s:1,
                       f:3,
                       :1,
                       c:3,
                       lo:8;
    } w;
};
struct img_spline_point { int x; int y; };

/* functions */
extern void img_spline_prepare(struct img_spline_point *points, int count, double *moments);
extern short img_spline_eval(unsigned x, struct img_spline_point *points, unsigned count, double *moments);

/* functions */
extern void img_build_tone_curves_front(void);
extern void img_build_tone_curves_back(void);
extern int img_tone_points_unset(int side);
extern void img_fit_tone_curve(struct img_spline_point *in_points, int count, int side, int colour);
extern int img_tone_curve_in_use(int);
extern int img_tone_points_changed(int);
extern int img_tone_hook(int unused_arg);
extern void img_fit_tone_curve_ends(struct img_spline_point *in_points, int count, int side, int colour);
extern unsigned short img_setup_side(int side);
extern unsigned short img_compute_side_params(int side, int unused_arg);
extern void img_compute_window_size(int side, int unused_arg, int width_out, int length_out);
extern void img_edge_state_1000_d_step_1();
extern void img_load_pisces_preset_1(int side);
extern void img_load_pisces_preset_3(int side);
extern void img_align_width(int side, unsigned short *width, int flag);
extern void img_edge_state_04_step_1(int index, char *out);
extern void img_split_nibbles(int side, unsigned short flatbed_value, unsigned short adf_value, unsigned short *hi, unsigned short *lo);
extern void img_decode_window_mode(int side);
extern void img_decode_image_type(int side);
extern unsigned short img_select_resolutions(int side);
extern int img_check_resolution(int side);
extern int img_get_fixed_zoom(unsigned short *x_zoom_percent, unsigned short *y_zoom_percent);
extern int img_select_x_resolution(unsigned short *optical_dpi, unsigned short *zoom_permille, int side);
extern int img_select_y_resolution(unsigned short *optical_dpi, unsigned short *zoom_permille, int side);
extern unsigned short img_round_up_resolution(unsigned short dpi);
extern unsigned short img_optical_resolution(unsigned short dpi);
extern unsigned short img_optical_y_resolution(unsigned short dpi);
extern unsigned short img_compute_geometry(int side);
extern void img_compute_geometry_step_1(int side);
extern unsigned short img_compute_geometry_step_2(int side);
extern unsigned short img_compute_length_lines(int side);
extern void img_compute_geometry_step_5(int side);
extern unsigned short img_compute_geometry_step_6(int side);
extern unsigned short img_compute_geometry_step_7(int side);
extern unsigned short img_compute_width_px(int side, struct win_param_block *params, unsigned short *out);
extern void img_apply_y_dpi_minimum(int side, unsigned short *width);
extern void img_round_width(int side, unsigned short *width);
extern unsigned short img_window_length_to_lines(unsigned short *out, int length_inch1200, unsigned short dpi);
extern unsigned short img_custom_size_to_lines(unsigned short *out, unsigned short portrait_size, unsigned short landscape_size, unsigned short dpi);
extern unsigned short img_paper_size_lines(unsigned short *out, int want_length, unsigned short dpi);
extern unsigned short img_compute_center_offset(unsigned short *out, int side);
extern void img_compute_geometry_step_3(int side);
extern int img_setup_pisces_if_enabled(int side);
extern void img_setup_pisces(int side);
extern void img_setup_pisces_step_1(int side);
extern void img_setup_pisces_outline(int side);
extern void img_setup_pisces_emphasis(int side);
extern void img_load_threshold_regs(int side);
extern void img_setup_pisces_sdtc(int side);
extern void img_setup_pisces_halftone(int side);
extern unsigned char img_adjust_threshold(int side, unsigned char threshold);
extern char img_get_threshold_offset(int side);
extern void img_setup_vdcc_emphasis(int side);
extern int img_white_level_follow_on(int);
extern void img_setup_vdcc(int side);
extern void img_setup_vdcc_step_1(int side);
extern void img_setup_vdcc_step_2(int side);
extern void img_setup_vdcc_step_3(int side);
extern void img_set_low_res_timing(int side);
extern void img_setup_vdcc_step_5(int side);
extern void img_setup_vdcc_step_6(int side);
extern void img_setup_dropout_color(int side);
extern void img_setup_vdcc_step_8(int side);
extern void img_copy_auto_size_flag(int side);
extern void img_setup_vdcc_step_6_1_1(int side, short *cb_weight_out, short *cr_weight_out);
extern void img_hook_setup(int side);
extern unsigned short img_prepare_halftone(int side);
extern int img_check_halftone(int side, union img_halftone_select *select);
extern void img_select_halftone(int side, unsigned short *size, unsigned short *select);
extern char *img_halftone_source(union img_halftone_select *select);
extern void img_copy_halftone(char *p, char *dst, unsigned short size, union img_halftone_select *select);
extern void img_load_halftone(int side);
extern unsigned short img_prepare_gamma(int side);
extern void img_get_gamma_params(int side, struct img_gamma_key *gamma, struct win_binarize_view *window_binarize, struct img_path_flags *path_flags);
extern unsigned short img_default_download_gamma(struct img_gamma_key *gamma, int side);
extern int img_build_gamma(struct img_path_flags *path_flags, struct img_gamma_key *gamma, int side);
extern int img_gamma_out_of_date(struct img_path_flags *path_flags, struct img_gamma_key *gamma, int side);
extern int img_select_gamma_param(int side, struct img_path_flags *path_flags, struct img_gamma_key *gamma, unsigned char *param);
extern int img_tone_curves_need_rebuild(int side, char *back_loaded, char *front_loaded);
extern char *img_gamma_source(int side, struct img_path_flags *path_flags, struct img_gamma_key *gamma);
extern void img_build_gamma_table(unsigned char *tab, unsigned char *dst, struct img_gamma_key *gamma, int side);
extern int img_gamma_threshold(void *path_flags, struct img_gamma_key *gamma, int side);
extern short img_build_gamma_table_step_1(unsigned short *shift, unsigned char shift_param, unsigned char scale_param);
extern char img_build_gamma_table_step_5(unsigned char level, short shift, short scale);
extern unsigned int img_build_gamma_table_step_5_1(unsigned short exp_level, unsigned short shift, unsigned short scale);
extern int img_build_gamma_table_step_3(unsigned char *p, int size);
extern void img_build_gamma_table_step_4(int side, unsigned char *tab, int size, struct img_gamma_key *gamma);
extern void img_build_gamma_table_step_4_1(int side, char *out, unsigned index, unsigned char *tab, int size, struct img_gamma_key *gamma);
extern void img_build_gamma_table_step_4_2(unsigned char *luma_out, unsigned char *half_max_out, unsigned char *levels);
extern int img_mode_changed(int side);
extern int img_flags_changed(int side);
extern unsigned char img_negate_byte(char value);
extern void img_load_vdcc_tables(int side);
extern void img_write_vdcc_tables(int side);
extern void img_load_pisces_gamma(int side);
extern void img_write_vdcc_invert_tables(int side);
extern unsigned char img_get_fill_byte(int reverse_image, int invert_if_reverse);

/* constants */
extern const unsigned char img_default_tone_x_front[1];
extern const unsigned char img_default_tone_y_front[11];
extern const unsigned char img_default_tone_x_back[1];
extern const unsigned char img_default_tone_y_back[11];
extern const unsigned short img_resolution_codes[8];
extern const unsigned int img_paper_widths_inch1200[16];
extern const unsigned int img_paper_widths_landscape_inch1200[48];
extern const unsigned short img_paper_length_code_3[8];
extern const unsigned short img_paper_length_default[8];
extern const unsigned short img_paper_length_code_5[8];
extern const unsigned short img_paper_length_code_12[8];
extern const unsigned short img_paper_length_code_13[8];
extern const unsigned short img_paper_length_code_7[8];
extern const unsigned short img_paper_length_code_15[8];
extern const unsigned short img_paper_length_code_6[8];
extern const unsigned short img_paper_width_code_3[8];
extern const unsigned short img_paper_width_default[8];
extern const unsigned short img_paper_width_code_5[8];
extern const unsigned short img_paper_width_code_12[8];
extern const unsigned short img_paper_width_code_13[8];
extern const unsigned short img_paper_width_code_7[8];
extern const unsigned short img_paper_width_code_15[8];
extern const unsigned short img_paper_width_code_6[8];
extern const unsigned char img_halftone_pattern_map[64];
extern const unsigned char img_outline_levels[68];
extern const short img_x_offset_table[2][10];
extern const unsigned char img_variance_params[2];
extern const unsigned char img_variance_sdtc_w4[2];
extern const unsigned char img_variance_sdtc_w6[2];
extern const unsigned char img_variance_sdtc_mode[1];
extern const unsigned char img_variance_filter_level[57];
extern const unsigned char img_user_offset_steps[16];
extern const unsigned char img_resolution_codes_2[24];
extern const char img_halftone_patterns[4][8][64];
extern const char img_halftone_patterns_maint[5][8][64];
extern const short img_gamma_scale_table[256];
extern const short img_gamma_exp_table[256];
extern const char img_gamma_log_table[1024];
extern const char img_gamma_patterns[8][256];
extern const char img_gamma_patterns_1024[4][1024];
extern const unsigned short img_preset_1_ctrl_8[1];
extern const unsigned short img_preset_1_ctrl_10[1];
extern const unsigned short img_preset_1_word_0[1];
extern const unsigned short img_preset_1_word_1[1];
extern const unsigned short img_preset_1_word_2[1];
extern const unsigned short img_preset_1_word_3[1];
extern const unsigned short img_preset_1_word_4[1];
extern const unsigned short img_preset_1_word_5[1];
extern const unsigned short img_preset_1_word_6[1];
extern const unsigned short img_preset_1_word_7[1];
extern const unsigned short img_preset_1_word_8[1];
extern const unsigned short img_preset_1_word_9[1];
extern const unsigned short img_preset_1_word_10[1];
extern const unsigned short img_preset_1_word_11[1];
extern const unsigned short img_preset_1_word_12[1];
extern const unsigned short img_preset_1_word_13[1];
extern const unsigned short img_preset_1_word_14[1];
extern const unsigned short img_preset_3_ctrl_8[1];
extern const unsigned short img_preset_3_ctrl_10[1];
extern const unsigned short img_preset_3_word_0[1];
extern const unsigned short img_preset_3_word_1[1];
extern const unsigned short img_preset_3_word_2[1];
extern const unsigned short img_preset_3_word_3[1];
extern const unsigned short img_preset_3_word_4[1];
extern const unsigned short img_preset_3_word_5[1];
extern const unsigned short img_preset_3_word_6[1];
extern const unsigned short img_preset_3_word_7[1];
extern const unsigned short img_preset_3_word_8[1];
extern const unsigned short img_preset_3_word_9[1];
extern const unsigned short img_preset_3_word_10[1];
extern const unsigned short img_preset_3_word_11[1];
extern const unsigned short img_preset_3_word_12[1];
extern const unsigned short img_preset_3_word_13[1];
extern const unsigned short img_preset_3_word_14[1];
extern const unsigned short *img_paper_length_lines[16];
extern const unsigned short *img_paper_width_lines[16];

struct vdcc_reg_0b { char pad_00[11]; unsigned char hi:5, gamma_enable:3; };

#endif
