/* hw_selftest.h: declarations of hw_selftest.c.
   */
#ifndef HW_SELFTEST_H
#define HW_SELFTEST_H
#include "common.h"

/* p_pisces_input_op->input_op: the Pisces input operation */
#define PISCES_INPUT_PASS   0
#define PISCES_INPUT_INVERT 1
#define PISCES_INPUT_ZERO   2
#define PISCES_INPUT_ONES   3

struct pisces_filter_coef_bits {
    unsigned short p:6,
                   mode:2,
                   q:3,
                   f:1,
                   g:1,
                   l:3;
};
union pisces_filter_coef {
    unsigned short w;
    struct pisces_filter_coef_bits f;
};
struct vdcc_monitor_words {
    unsigned short reg_060, level_left, level_middle, level_right;
};
union pisces_filter_w0 {
    unsigned short w;
    struct {
        unsigned short p:13,
                       b2:1,
                       b1:1,
                       b0:1;
    } f;
};
union pisces_filter_word {
    unsigned short w;
    struct { unsigned short h:8, l:8; } f;
};
union pisces_filter_c16 {
    unsigned short w;
    struct { unsigned short p:6, mode:2, l:8; } f;
};
struct pisces_filter_regs {
    union pisces_filter_w0 reg_170;
    union pisces_filter_word reg_172, reg_174, reg_176;
    union pisces_filter_coef channel_0, channel_1, channel_2, channel_3;
    union pisces_filter_c16 variance_level;
};
struct vdcc_emphasis_regs {
    unsigned short reg_0b0, reg_0b2, reg_0b4;
    unsigned char reg_0b6, reg_0b7;
    unsigned short reg_0b8;
};
struct scsi_regs_dummy {
    char pad_00;
    unsigned char mainint;
    char pad_02[13];
    unsigned char reset;
    char pad_10[3];
    struct Bits scsimode0;
    char pad_14[7];
    unsigned char syncmode;
};
struct usb_regs_dummy {
    char pad_00;
    unsigned char main_int_stat;
    char pad_02[15];
    struct Bits main_int_enb;
};
union pisces_binarize_modes {
    unsigned short w;
    struct {
        unsigned short p:8,
                       a:2,
                       b:2,
                       c:2,
                       d:2;
    } f;
};
struct pisces_binarize_regs {
    union pisces_binarize_modes modes;
    struct { unsigned short h:8, l:8; } threshold;
    struct { unsigned short p:6, m:2, l:8; } reg_194;
    unsigned short reg_196;
    union {
        struct { unsigned short p:2, m:2, :12; } f;
        struct { unsigned short p:6, a:5, b:5; } h;
    } halftone_size;
};
struct pisces_kernel_regs { unsigned short w[15]; };
/* VDCC3 +0x90 (capture_ctrl): b0 output enable, b2 line gate, b3 capture run */
struct vdcc_capture_ctrl_bits {
    unsigned char low_res_mode:1,
                  :1,
                  b5:1,
                  b4:1,
                  capture_run:1,
                  line_gate:1,
                  margin_window:1,
                  output_enable:1;
};
struct vdcc_capture_regs {
    struct vdcc_capture_ctrl_bits capture_ctrl;
    union {
        struct Bits b;
        struct { unsigned char apply:1, dropout_channel:3, x_res_code:4; } f;
    } x_res_ctrl;
    char reg_092;
    struct Bits d;
    struct {
        unsigned char b7:1,
                      m:3,
                      b3:1,
                      l:3;
    } e;
    struct {
        unsigned char :1,
                      b6:1,
                      m:2,
                      b3:1,
                      l:3;
    } f;
    char reg_096_097[2];
    struct { unsigned short p:2, v:14; } window_start, reg_09a;
    struct { unsigned short p:3, v:13; } reg_09c;
    struct { unsigned short p:2, v:14; } reg_09e;
    unsigned char margin_start_px, reg_0a1;
    struct { unsigned short p:2, v:14; } margin_end_px, reg_0a4, reg_0a6;
};
/* VDCC3 +0x03 (irq_enable): b0 the line interrupt */
struct vdcc_irq_enable_bits { unsigned char :7, line_irq_enable:1; };
/* VDCC3 +0x04 (mdc_event): b0 the MDC's result doorbell */
struct vdcc_mdc_event_bits { unsigned char :7, mdc_result_ready:1; };
/* VDCC3 +0x05 (events): b0 the line event, written 1 to clear */
struct vdcc_events_bits {
    unsigned char correction_done_7:1,
                  correction_done_6:1,
                  correction_done_5:1,
                  :4,
                  line_event:1;
};
struct vdcc_regs {
    struct Bits r0;
    struct {
        unsigned char chip_enable:2,
                      :2,
                      uart_reset:1,
                      b2:1,
                      :1,
                      soft_reset:1;
    } reset_ctrl;
    struct Bits r2;
    union { unsigned char v; struct vdcc_irq_enable_bits f; } irq_enable;
    struct vdcc_mdc_event_bits mdc_event;
    union { unsigned char c; struct vdcc_events_bits b; } events;
    char pad_06;
    union {
        struct { unsigned char p:2, status:2, :4; } s2;
        struct { unsigned char p:4, status:2, :2; } s4;
        struct { unsigned char p:6, status:2; } s6;
    } measure_status;
    struct { unsigned char :2, emphasis_enable:3, l:3; } emphasis_ctrl;
    struct { unsigned char :2, m:3, l:3; } r9;
    char pad_0a;
    struct {
        unsigned char output_enable:1,
                      :1,
                      m:3,
                      gamma_enable:3;
    } output_ctrl;
};
struct uart_lcr {
    unsigned char dlab:1,
                  set_break:1,
                  stick_parity:1,
                  even_parity:1,
                  parity_enable:1,
                  two_stop_bits:1,
                  word_length:2;
};
struct vdcc_gpio_reg_05 { char p[5]; struct Bits reg_105; };
/* VDCC3 +0xCA as bits: b0, b1 the two CCD-board gates of the front chip (1 = off), b3 the CCD readout mode of ADF 300 dpi */
struct vdcc_source_gate_bits {
    unsigned char :4,
                  ccd_300dpi_mode:1,
                  b2:1,
                  adf_ccd_off:1,
                  flatbed_ccd_off:1;
};
struct vdcc_sensor_regs {
    char p[9];
    struct Bits reg_0c9;
    union {
        struct vdcc_source_gate_bits b;
        unsigned char c;
        struct { unsigned char p:4, b3:1, lo:3; } f;
        struct {
            unsigned short p:4,
                           ccd_300dpi_mode:1,
                           b10:1,
                           adf_ccd_off:1,
                           flatbed_ccd_off:1,
                           :8;
        } w;
        struct { unsigned short p:12, lo:4; } wl;
        struct {
            unsigned char pad_00;
            struct { unsigned char h:4, l:4; } n11;
        } n;
    } source_gate;
};
union pisces_global_reg {
    unsigned short w;
    struct { unsigned short :15, b0:1; } n;
    struct {
        unsigned short p:8,
                       a:2,
                       b:2,
                       c:3,
                       d:1;
    } m;
};
/* p_vdcc_front_correction, p_vdcc_back_correction: the front / back VDCC3 register block at 0x220010 / 0x230010 */
struct vdcc_correction_regs {
    unsigned char mode_chip1, mode_chip0, reg_012, mode_chip2, reg_014;
    struct Bits reg_015;
    unsigned char reg_016;
    unsigned char white_factor:8;
    char reg_018_01b[4];
    unsigned char black_chip0_a:8,
                  black_chip0_b:8,
                  black_chip1_a:8,
                  black_chip1_b:8,
                  black_chip2_a:8,
                  black_chip2_b:8;
    union {
        short w;
        struct { unsigned char follow_gain_0:8, follow_gain_1:8; } c;
    } follow_gain_0_1;
    unsigned char follow_gain_2:8,
                  follow_gain_3:8,
                  follow_gain_4:8,
                  follow_gain_5:8,
                  follow_gain_6:8,
                  follow_gain_7:8,
                  follow_gain_8:8,
                  follow_gain_9:8;
    struct {
        unsigned short :4,
                       hi:4,
                       lo:8;
    } reg_02c, reg_02e, reg_030, reg_032;
};
/* Pisces +0x1A0 (p_pisces_jpeg_ctrl) */
struct pisces_jpeg_ctrl_bits { unsigned short :14, b1:1, b0:1; };
/* board control +0x02 as bits: b9 hostif_idle (the host interface idle: isr_irq3 clears it at entry, usb_handle_irq
   sets it when nothing is pending, usb_bulk clears it for a transfer), b11 standby (1 in power save) */
struct board_ctrl_word_02 {
    unsigned short :4,
                   standby:1,
                   :1,
                   hostif_idle:1,
                   :9;
};
/* board control +0x04 as bits: b9 hostif_idle_enable (the enable of +0x02 b9) */
struct board_ctrl_word_04 { unsigned short :6, hostif_idle_enable:1, :9; };
/* board control +0x06 as bits: b10 mdc_enable (0 in standby, set after +0x0A b8 / b9 on wake-up; b8, b9 set by
   hw_init_bus only, not named) */
struct board_ctrl_word_06 {
    unsigned short :5,
                   mdc_enable:1,
                   b9:1,
                   b8:1,
                   :8;
};
/* board control +0x08 as bits: b8 .. b11 set by hw_init_bus and power_enter_save; no reading yet */
struct board_ctrl_word_08 {
    unsigned short :4,
                   b11:1,
                   b10:1,
                   b9:1,
                   b8:1,
                   :8;
};
/* board control +0x0A as bits: b8 power_enable_8, b9 power_enable_9 (the first lines set on wake-up, 0 in standby)
   */
struct board_ctrl_word_0a {
    unsigned short :6,
                   power_enable_9:1,
                   power_enable_8:1,
                   :8;
};
/* board control +0x0C as bits: b8 cga_select, b10 cga_reset (the CGA board probe, selftest_probe_cga_board); b11
   set by power_enter_save on board variant != 0, not named */
struct board_ctrl_word_0c {
    unsigned short :4,
                   b11:1,
                   cga_reset:1,
                   :1,
                   cga_select:1,
                   :8;
};
/* board control +0x0E as bits: b8 ccd_300dpi_mode (with VDCC3 +0xCA b3, job_setup_sensor; saved over power save)
   */
struct board_ctrl_word_0e { unsigned short :7, ccd_300dpi_mode:1, :8; };
struct board_ctrl_regs {
    struct { unsigned short p:5, code:3, q:8; } variant;
    struct board_ctrl_word_02 reg_02;
    struct board_ctrl_word_04 reg_04;
    struct board_ctrl_word_06 reg_06;
    struct board_ctrl_word_08 reg_08;
    struct board_ctrl_word_0a reg_0a;
    struct board_ctrl_word_0c reg_0c;
    struct board_ctrl_word_0e reg_0e;
};
struct board_ctrl_reg_04 {
    short p[2];
    struct { unsigned short :6, hostif_idle_enable:1, :9; } reg_04;
};
struct vdcc_gpio_regs {
    char reg_100_104[5];
    struct Bits reg_105;
    unsigned char reg_106;
    struct { unsigned char :1, l:7; } reg_107;
};
struct uart_mcr {
    unsigned char p:4,
                  out2:1,
                  out1:1,
                  rts:1,
                  dtr:1;
};
struct uart_regs {
    char pad_00;
    unsigned char rbr_thr;
    char pad_02;
    unsigned char ier;
    char pad_04;
    unsigned char iir_fcr;
    char pad_06;
    struct uart_lcr lcr;
    char pad_08;
    struct uart_mcr mcr;
    char pad_0a;
    unsigned char lsr;
};
struct pisces_block_ctrl { unsigned short p:15, enable:1; };
struct pisces_ctrl_bits {
    unsigned short :6,
                   f8:2,
                   :3,
                   sdram_window:1,
                   :1,
                   hostif_stop:1,
                   :1,
                   jpeg_access:1;
};
/* Pisces +0x008 / +0x00A / +0x00C: the event enable, +0x00A and the event status words (bits 7-0) */
struct pisces_event_bits {
    unsigned short :8,
                   b7:1,
                   b6:1,
                   b5:1,
                   b4:1,
                   b3:1,
                   b2:1,
                   b1:1,
                   b0:1;
};
struct pisces_ctrl_regs {
    union { unsigned short v; struct pisces_ctrl_bits f; } ctrl;
    unsigned short sdram_page, reg_006;
    union {
        unsigned short v;
        struct pisces_event_bits f;
    } event_enable, reg_00a, event_status;
};
struct pisces_run_bits {
    unsigned short :1,
                   b14:1,
                   :1,
                   b12:1,
                   :1,
                   output_on:1,
                   :1,
                   b8:1,
                   :3,
                   b4:1,
                   :3,
                   capture_on:1;
};
struct pisces_format_bits {
    unsigned short f14:2,
                   output_format:2,
                   :1,
                   output_color:1,
                   :1,
                   b8:1,
                   b7:1,
                   f5:2,
                   b4:1,
                   :1,
                   b2:1,
                   f0:2;
};
struct pisces_output_mode_bits {
    unsigned short f14:2,
                   f12:2,
                   :2,
                   f8:2,
                   :8;
};
struct pisces_output_format_bits {
    unsigned short :2,
                   f12:2,
                   :1,
                   output_jpeg:1,
                   :1,
                   binary_out:1,
                   :8;
};
struct pisces_mode_regs {
    union { unsigned short v; struct pisces_run_bits f; } run;
    union { unsigned short v; struct pisces_format_bits f; } format;
    union { unsigned short v; struct pisces_output_mode_bits f; } reg_014;
    union {
        unsigned short v;
        struct pisces_output_format_bits f;
    } output_format;
};
struct link_global_params {
    struct { unsigned char sides_given:1, side_selection:2, :5; } side_flags;
    unsigned char prepick;
    struct { unsigned char param_high:4, param_low:4; } byte_2;
    struct {
        unsigned char double_feed_given:1,
                      double_feed_continue:1,
                      :1,
                      double_feed_thickness:1,
                      double_feed_length:4;
    } double_feed_flags;
    char spare_4, reserved_5;
    struct {
        unsigned char backing_given:1,
                      backing:3,
                      auto_size_mode:2,
                      page_end_mode:2;
    } paper_flags;
    char spare_7, spare_8;
    struct { unsigned char :5, scan_mode:3; } scan_flags;
    unsigned char reserved_10;
};
struct jpeg_regs {
    unsigned char reg_00_02[3];
    struct Bits reg_03;
    char reg_04_1e[27];
    unsigned char reg_1f;
};
struct pisces_zoom_permille { unsigned short :4, v:12; };
struct field_13_bits { unsigned short :3, v:13; };
struct pisces_fill_value { unsigned short :8, lo:8; };
struct pisces_gray_row_regs {
    short y_weight;
    union {
        short v;
        struct { unsigned short p:6, s:1, v:9; } f;
    } cb_weight, cr_weight;
};
struct link_side_view {
    char pad_00[2];
    struct {
        unsigned char :1,
                      color_mode:2,
                      dropout_color:3,
                      c:2;
    } color_bits;
    char pad_03[3];
    unsigned char color_contrast, color_brightness;
};
struct pisces_zoom_regs {
    struct pisces_zoom_permille x_zoom_permille, y_zoom_permille;
    struct field_13_bits reg_144;
    short reg_146;
    struct field_13_bits width_px;
    short length_lines;
};
struct pisces_hostif_regs {
    unsigned short count_low, count_high;
    union {
        unsigned short v;
        struct {
            unsigned short :4,
                           b11:1,
                           b10:1,
                           b9:1,
                           b8:1,
                           a:2,
                           b:2,
                           :2,
                           b1:1,
                           b0:1;
        } f;
    } ctrl;
};
struct pisces_input_op_regs { unsigned short p:14, input_op:2; };
/* Pisces +0x120: the SDRAM controller's control word */
struct pisces_sdram_ctrl_bits {
    unsigned short :10,
                   b5:1,
                   :1,
                   b3:1,
                   :2,
                   b0:1;
};
struct pisces_sdram_ctrl_regs {
    union { unsigned short v; struct pisces_sdram_ctrl_bits b; } ctrl;
    struct {
        unsigned short p:2,
                       f12:2,
                       :1,
                       b10:1,
                       :1,
                       b8:1,
                       :1,
                       b6:1,
                       :1,
                       b4:1,
                       :1,
                       b2:1,
                       f0:2;
    } reg_122;
    unsigned short reg_124;
};
struct pisces_edge_regs {
    struct {
        unsigned short :5,
                       g:3,
                       :2,
                       f:2,
                       :3,
                       enable:1;
    } ctrl;
    struct field_13_bits x_start, x_end;
    unsigned short level;
    short pad_08[2];
    struct {
        unsigned short lo:8,
                       hi:8;
    } threshold_1, threshold_2, threshold_3;
    unsigned short left_edge, right_edge;
};
struct pisces_capture_regs {
    struct pisces_block_ctrl ctrl;
    union { unsigned short v; struct field_13_bits f; } base_64k;
    struct field_13_bits size_64k, stride;
    struct { unsigned short v:16; } lines;
    struct { unsigned short p:11, f:5; } reg_04a;
    unsigned short lines_done;
};
/* VDCC3 +0x10B (the AFE serial port's ctrl): b4 selects the ADF board, b0 busy */
struct vdcc_afe_ctrl_bits {
    unsigned char :3,
                  adf_select:1,
                  b3:1,
                  b2:1,
                  b1:1,
                  busy:1;
};
/* VDCC3 +0x10F: b7 enables the AFE serial port */
struct vdcc_afe_port_ctrl_bits { unsigned char port_enable:1, :7; };
struct vdcc_afe_regs {
    struct { unsigned char h:4, :1, l:3; } format;
    unsigned char reg_109;
    char reg_10a;
    union { unsigned char v; struct vdcc_afe_ctrl_bits f; } ctrl;
    unsigned char data_high, data_low;
    char reg_10e;
    struct vdcc_afe_port_ctrl_bits port_ctrl;
};
struct pisces_sdram_pixel { char pad_00, b; };
union pisces_sdram_ptr {
    struct pisces_sdram_pixel *s;
    short *h;
    char *p;
    int v;
};
struct pisces_table_set {
    unsigned short *gamma, *t4, *t8, *t12;
};
struct vdcc_measure_coord { unsigned short :2, hi:6, lo:8; };
struct vdcc_measure_regs {
    char reg_040;
    union {
        struct { unsigned char hi:5, sel:3; } s;
        struct { unsigned char :2, mode:2, rest:4; } f;
    } select;
    char reg_042_043[2];
    unsigned short level_2, level;
    struct vdcc_measure_coord x_start, x_end, reg_04c, reg_04e;
    struct { unsigned short hi:8, lo:8; } gain_target;
    struct { unsigned char hi:4, lo:4; } reg_052, reg_053;
};
/* VDCC3 +0xDB: the counters' clear (self-clearing) and run bits, b1/b0 on the front chip, b5/b4 on the back chip */
struct vdcc_counter_ctrl_bits {
    unsigned char :2,
                  back_clear:1,
                  back_run:1,
                  :2,
                  front_clear:1,
                  front_run:1;
};
struct vdcc_counter_regs {
    char pad_00[11];
    struct vdcc_counter_ctrl_bits counter_ctrl;
    unsigned short line_count:16;
    unsigned short step_count:16;
};
struct vdcc_bank_regs {
    unsigned char :1,
                  bank:3,
                  b:4;
    unsigned char test_pattern;
};
struct word_field { unsigned short v:16; };
struct pisces_pass_source_regs {
    struct pisces_block_ctrl ctrl;
    union { unsigned short v; struct word_field f; } addr_low;
    union { unsigned short v; struct field_13_bits f; } addr_high;
    union {
        unsigned short v;
        struct { unsigned short p:1, v:15; } f;
    } x_offset;
    union { unsigned short v; struct word_field f; } y_offset;
    struct field_13_bits base_64k, ring_64k, stride;
    struct word_field lines;
    struct field_13_bits width;
    struct word_field length;
    struct { unsigned short p:11, f:5; } reg_076;
    short reg_078_07a[2];
    struct { unsigned short hi:8, v:8; } fill;
    struct pisces_fill_value fill_cb, fill_cr;
    struct { unsigned short left_lower:1, p:15; } skew_on;
    short skew_0, skew_1;
};
struct pisces_pass_dest_regs {
    struct pisces_block_ctrl ctrl;
    union { unsigned short v; struct field_13_bits f; } base_64k;
    struct field_13_bits size_64k;
    struct { unsigned short p:2, v:14; } width_px;
    union { short v; struct word_field f; } length_lines;
    struct { unsigned short p:11, f:5; } reg_09a;
    unsigned short lines_done;
    struct pisces_block_ctrl done;
    struct { unsigned short b15:1, p:15; } reg_0a0;
    struct { unsigned short p:2, v:14; } x_start, x_end;
    short y_start;
    unsigned short y_end;
    struct { unsigned short hi:8, v:8; } fill;
    struct pisces_fill_value fill_cb, fill_cr;
};
struct pisces_output_regs {
    struct { unsigned short :15, enable:1; } ctrl;
    struct { unsigned short v:16; } addr_low;
    struct { unsigned short p:3, v:13; } addr_high;
    struct { unsigned short :1, v:15; } reg_0c6;
    struct { unsigned short v:16; } reg_0c8;
    unsigned short base_64k, size_64k, width, lines, width_px, reg_0d4, output_gate, lines_done;
    struct pisces_block_ctrl reg_0da;
    struct { unsigned short hi:8, v:8; } fill;
    struct { unsigned short hi:8, lo:8; } fill_cb, fill_cr;
};
struct pisces_kernel_ctrl_regs {
    unsigned short reg_160;
    unsigned short width_px;
    struct {
        unsigned short b15:1,
                       :9,
                       mid:2,
                       :1,
                       lo:3;
    } reg_164;
    struct pisces_fill_value reg_166;
    union {
        unsigned short w;
        struct { unsigned short :13, lo:3; } f;
    } reg_168;
    unsigned short reg_16a;
};
struct link_window_params_record {
    union {
        unsigned char c;
        struct {
            unsigned char b7:1,
                          fixed_length:1,
                          source_given:1,
                          adf:1,
                          resolution_given:1,
                          resolution_code:3;
        } f;
    } scan_flags;
    union {
        unsigned char c;
        struct {
            unsigned char :1,
                          white_level_follow_given:1,
                          white_level_follow_off:1,
                          :5;
        } f;
        struct { unsigned char p:3, binarize_mode:2, level_code:3; } g;
    } follow_level;
    union {
        unsigned char c;
        struct {
            unsigned char halftone_downloaded:1,
                          :3,
                          halftone_given:1,
                          halftone_on:1,
                          multi_level:1,
                          long_page:1;
        } f;
    } image_flags;
    unsigned char dtc_options_hi, dtc_options_lo;
    union {
        unsigned char c;
        struct {
            unsigned char paper_mode:2,
                          use_window_area:1,
                          paper_code:5;
        } f;
    } paper;
    short paper_width_inch400, paper_length_inch400, x_offset_inch400, width_inch400;
    unsigned short y_offset_inch400, length_inch400;
    char mode_byte;
    union {
        unsigned char c;
        struct {
            unsigned char gamma_downloaded:1,
                          :5,
                          dpi_given:1,
                          level_given:1;
        } f;
    } gamma_flags;
    unsigned char level;
    unsigned char contrast;
    short x_res_dpi, y_res_dpi;
};
struct link_window_mode_view { char pad_00[4]; unsigned char mode_byte; };

/* functions */
extern void hw_restart(void);
extern void hw_startup_sequence(void);
extern void hw_init(void);
extern int hw_init_ports(void);
extern void hw_init_bus(void);
extern void hw_stop_wdt(void);
extern void hw_clear_dtc(void);
extern void hw_stop_dmac(void);
extern void hw_init_sci(void);
extern void hw_set_sci1_baud(void);
extern void hw_init_mtu(void);
extern void hw_init_cmt(void);
extern void hw_init_ad(void);
extern void hw_init_intc(int param);
extern void hw_enable_lsi_events(void);
extern void hw_init_ram_sections(void);
extern void hw_init_ram_set3(void);
extern void hw_set_vbr(void);
extern int hw_power_switch_enabled(void);
extern int hw_wait_mdc_ready(void);
extern unsigned char hw_check_mdc_signature(void);
extern int hw_init_vdcc(void);
extern void hw_enable_vdcc(int on);
extern void hw_init_afe_port(int side, int on);
extern int hw_init_pisces(void);
extern char selftest_vdcc_memories(void);
extern int hw_fill_verify_16(unsigned short *area, unsigned count, unsigned char value);
extern void hw_init_uart(void);
extern void hw_check_update_signature(void);
extern int hw_init_scsi_ctrl(void);
extern int hw_read_scsi_id(void);
extern void hw_unused_1(void);
extern int hw_init_usb_ctrl(void);
extern void hw_select_hostif(void);
extern int hw_switch_to_usb(void);
extern short selftest_pisces_indirect(void);
extern short selftest_pisces_indirect_ram(int side);
extern int selftest_pisces_rams(void);
extern unsigned int selftest_fill_verify();
extern unsigned int selftest_fill_verify_stride(short *area, short value);

/* functions */
extern int selftest_run_power_on(void);
extern void selftest_show_error(unsigned char code);
extern void selftest_load_settings(void);
extern void selftest_probe_cga_board(void);
extern void selftest_wait_pf7_high(void);
extern void selftest_clear_option_flags(void);
extern void selftest_set_mode_defaults(void);
extern void selftest_wait_power_button(void);
extern void selftest_enable_uart_rx(void);
extern void selftest_restart_uart(void);
extern void selftest_select_maint_mode(void);
extern void selftest_init_window(void);
extern unsigned char hw_window_mode_default(void);
extern void selftest_init_tone_curves(void);
extern void selftest_run_startup(void);
extern void selftest_finish_startup(void);
extern int selftest_vdcc_small_rams(void);
extern unsigned short selftest_vdcc_bank_rams();
extern unsigned short selftest_vdcc_bank_select();
extern char selftest_map_error(unsigned short result);
extern unsigned char selftest_measure_mark(void);

/* constants */
extern const unsigned char scsi_revision[1];
extern const unsigned char scsi_revision_tail[3];
extern const unsigned char hw_scsi_id_table[];
extern const unsigned char hw_window_mode_codes[8];
extern void *p_unused_after_iic;
extern unsigned short *p_vdcc_front_gamma_0;
extern unsigned short *p_vdcc_front_gamma_1;
extern unsigned short *p_vdcc_front_gamma_2;
extern struct pisces_table_set p_pisces_front_tables;
extern unsigned short *p_pisces_front_halftone;
extern short *p_pisces_front_ram;
extern unsigned short *p_vdcc_back_gamma_0;
extern unsigned short *p_vdcc_back_gamma_1;
extern unsigned short *p_vdcc_back_gamma_2;
extern struct pisces_table_set p_pisces_back_tables;
extern unsigned short *p_pisces_back_halftone;
extern short *p_pisces_back_ram;
extern unsigned short *p_vdcc_front_ref_0[1];
extern unsigned short *p_vdcc_front_ref_1[1];
extern unsigned short *p_vdcc_front_ref_2[1];
extern unsigned short *p_vdcc_back_ref_0[1];
extern unsigned short *p_vdcc_back_ref_1[1];
extern unsigned short *p_vdcc_back_ref_2[1];
extern unsigned short *p_vdcc_front_ref_byte_0[1];
extern unsigned short *p_vdcc_front_ref_byte_1[1];
extern unsigned short *p_vdcc_front_ref_byte_2[1];
extern unsigned short *p_vdcc_back_ref_byte_0[1];
extern unsigned short *p_vdcc_back_ref_byte_1[1];
extern unsigned short *p_vdcc_back_ref_byte_2[1];
extern unsigned short *p_vdcc_front_test_table;
extern unsigned short *p_vdcc_back_test_table;
extern union pisces_sdram_ptr p_pisces_front_sdram;
extern union pisces_sdram_ptr p_pisces_back_sdram;
extern volatile struct board_ctrl_regs *p_board_ctrl;
extern struct scsi_regs_dummy *p_scsi_ctrl_dummy;
extern void *p_scsi_ctrl;
extern struct vdcc_regs *p_vdcc_front[1];
extern struct vdcc_regs *p_vdcc_back[1];
extern struct vdcc_correction_regs *p_vdcc_front_correction[1];
extern struct vdcc_correction_regs *p_vdcc_back_correction[1];
extern struct vdcc_measure_regs *p_vdcc_measure[2];
extern struct vdcc_monitor_words *p_vdcc_front_monitor[1];
extern struct vdcc_monitor_words *p_vdcc_back_monitor[1];
extern unsigned short *p_vdcc_timing[];

/* peripheral blocks: the ROM pointer table 0x3edb0-0x3ee3c, Renesas' structs from
   7144s.h. volatile as in the header's own address macros: no function compiles differently without it.
   Not header types (declared in the files): p_mtu (TSTR at 0xffff8240), p_ad (both A/D units),
   p_sbycr (a long at +8). No user yet: p_mtu34, p_pc, p_pd, p_flash, p_ubc,
   p_dmac0..p_dmac3. */
extern struct vdcc_capture_regs *p_vdcc_capture[2];
extern struct vdcc_emphasis_regs *p_vdcc_emphasis[2];
extern struct vdcc_sensor_regs *p_vdcc_front_sensor;
extern struct vdcc_sensor_regs *p_vdcc_back_sensor;
extern volatile struct vdcc_counter_regs *p_vdcc_front_counter[1];
extern volatile struct vdcc_counter_regs *p_vdcc_back_counter[1];
extern struct vdcc_bank_regs *p_vdcc_front_bank[1];
extern struct vdcc_bank_regs *p_vdcc_back_bank[1];
extern struct vdcc_gpio_regs *p_vdcc_front_gpio[1];
extern void *p_vdcc_back_gpio[1];
extern struct vdcc_afe_regs *p_vdcc_afe[2];
extern struct uart_regs *p_uart[12];
extern struct usb_regs_dummy *p_usb_ctrl_dummy;
extern void *p_usb_ctrl;
extern volatile union pisces_global_reg *p_pisces[2];
extern volatile struct pisces_ctrl_regs *p_pisces_ctrl[2];
extern volatile struct pisces_mode_regs *p_pisces_mode[2];
extern unsigned short (*p_pisces_ycc_matrix[2])[3];
extern volatile struct pisces_capture_regs *p_pisces_capture[2];
extern volatile struct pisces_pass_source_regs *p_pisces_pass_source[2];
extern volatile struct pisces_pass_dest_regs *p_pisces_pass_dest[2];
extern volatile struct pisces_output_regs *p_pisces_output[2];
extern unsigned short (*p_pisces_rgb_matrix[2])[3];
extern volatile struct pisces_hostif_regs *p_pisces_hostif[2];
extern volatile struct pisces_sdram_ctrl_regs *p_pisces_sdram_ctrl[2];
extern volatile struct pisces_input_op_regs *p_pisces_input_op[2];
extern volatile struct pisces_zoom_regs *p_pisces_zoom[2];
extern volatile struct pisces_gray_row_regs *p_pisces_gray_row[2];
extern volatile struct pisces_kernel_ctrl_regs *p_pisces_kernel_ctrl[2];
extern volatile struct pisces_filter_regs *p_pisces_filter[2];
extern volatile struct pisces_binarize_regs *p_pisces_binarize[2];
extern volatile struct pisces_jpeg_ctrl_bits *p_pisces_jpeg_ctrl[2];
extern volatile struct pisces_edge_regs *p_pisces_edge[2];
extern volatile struct pisces_kernel_regs *p_pisces_kernel[2];
extern struct link_window_params_record *link_window_params_ptrs[1];
extern void *link_window_back_ptr;
extern struct link_window_mode_view *link_window_mode_ptrs[1];
extern unsigned char *link_z_params_ptrs[1];
extern struct link_global_params *link_global_params_ptr;
extern struct link_side_view *link_side_params_ptrs[4];
extern volatile struct jpeg_regs *p_jpeg[2];
extern unsigned short *p_jpeg_front_qtables[4];
extern unsigned short *p_jpeg_back_qtables[4];
extern unsigned short *p_jpeg_front_huffman[8];
extern unsigned short *p_jpeg_back_huffman[8];

/* variables */
extern int selftest_power_button_seen;
extern int selftest_uart_restart_pending;

/* I/O */
extern volatile unsigned char mdc_version[];
extern volatile unsigned char mdc_signature_gap_1[];
extern volatile unsigned char mdc_signature_gap_2[];
extern volatile unsigned char mdc_signature_gap_3[];
extern volatile unsigned char mdc_signature_gap_4[];
extern unsigned char mdc_settings_8d[];
extern unsigned char mdc_settings_8e[];
extern volatile unsigned char mdc_back_window_overlap_patterns;

#endif
