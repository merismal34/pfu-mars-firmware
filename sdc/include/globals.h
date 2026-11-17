/* globals.h: the variables of the globals modules (lo B, hi B, the kept state B3, the memory block
   BM, on-chip RAM: data files without an owner source file), in their definition order and with their
   types, so that the externs lay out B as the definitions do; one object with two types has an
   #ifndef FW_DEFINITIONS .. #else block (globals.c defines it). */
#ifndef GLOBALS_H
#define GLOBALS_H
#include "common.h"

/* types: bitfield bytes (b7 is the most significant bit) and register views */
/* usb_ep0_int_status, usb_epa_int_status, usb_epb_int_status: bytes tested as bits and stored whole */
union UBF { unsigned char v; struct Bits f; };

/* button_state: a byte stored whole and tested as bits */
union byte_bits_c { unsigned char c; struct Bits b; };

/* types: bitfield bytes (b7 is the most significant bit) and register views */
union view_win_sides {
    unsigned char v;
    struct { unsigned char link_given:1, :5, selection:2; } f;
};
/* img_loaded_tables[2]: 24-byte records per side */
struct img_table_state {
    unsigned short halftone_built;
    unsigned short pad_02;
    unsigned short halftone_loaded;
    unsigned short pad_06;
    struct img_gamma_key gamma_built_mono;
    unsigned char gamma_loaded_mono[4];
    struct img_gamma_key gamma_built_color;
    unsigned char gamma_loaded_color[4];
};

/* one-member structs */
struct cell_edge_c0_runs { unsigned short v; };
struct cell_panel_multifeed_blink_state { unsigned short v; };
struct cell_panel_multifeed_blink_polls { unsigned short v; };
struct cell_page_scanning { int v; };
struct cell_tick_hostif_wait_ticks { unsigned short v; };
struct cell_tick_capture_wait_ticks { unsigned short v; };
struct cell_tick_motor_test_ticks { unsigned short v; };
struct cell_tick_idle_seconds { int v; };
struct cell_power_off_limit_seconds { unsigned int v; };
struct cell_power_off_warning_seconds { unsigned int v; };
union view_scsi_read_side {
    int v;
    struct { unsigned char pad_00[3]; unsigned char lo; } c;
    volatile int s;
};
struct cell_scsi_read_window_id { unsigned char v; };
struct cell_scsi_sense_owner { char v; };
struct cell_scsi_reply_message { unsigned char v; };
struct cell_scsi_disconnected_initiator { char v; };
struct cell_scsi_message_retries { unsigned char v; };
union view_scsi_param_len {
    volatile unsigned int v;
    struct { unsigned char pad_00[2]; unsigned short lo; } c;
};
union view_scsi_xfer_len {
    unsigned int v;
    struct { unsigned char pad_00[3]; unsigned char lo; } c;
};
struct cell_maint_pisces_side { int v; };
struct cell_maint_adjust_target { unsigned char v; };
struct cell_maint_state { unsigned char v; };
struct cell_maint_item { unsigned char v; };
struct cell_testmode_number { unsigned char v; };
struct cell_testmode_param_1 { unsigned char v; };
struct cell_testmode_adjust_mode { unsigned char v; };
struct maint_flag_mode { unsigned char pad_00; volatile unsigned char mode; };
union view_maint_mode_word {
    volatile unsigned short v;
    struct maint_flag_mode c;
};
union view_uart_frame_len {
    unsigned short v;
    struct { unsigned char pad_00; char lo; } c;
};
struct cell_uart_baud_index { unsigned char v; };

struct cell_calib_offset_target_front { unsigned char v; };
struct cell_calib_offset_target_back { unsigned char v; };
struct cell_page_current { int v; };
struct cell_page_scanned_count { volatile int v; };
union view_mdc_motor_test_state {
    unsigned short w;
    struct { unsigned char hi; struct Bits lo; } by;
};

struct job_mode_record {
    unsigned char job_separation[1], lamp_off_minutes, prepick, measurement_unit, auto_length, auto_width, sleep_timer, interlace_read, pad_08, backing, double_feed, deskew, overscan, dropout_color;
    unsigned char dropout_levels[6];
    unsigned char device_controls, buffer_mode[1], buffer_clear, pad_17;
    unsigned short page_count_limit, max_burst_size, unit_divisor;
    unsigned char buffer_byte_6, pad_1f;
    union { unsigned char b[1]; unsigned char c; } driver_function_confirm;
    char pad_21[3];
};
struct edge_top_work {
    short start_width_px;
    unsigned short min_width_px;
    short max_width_px;
    short confirm_count;
    unsigned short confirm_limit;
};
struct view_jpeg_setup_flags { struct Bits b; char pad_01[7]; };
struct edge_level_work {
    short dark_count;
    short line_count;
    char pad_04[2];
    short watch_lines;
    union {
        unsigned short v;
        struct { char hi; char lo; } c;
    } level_sum;
    char dark_level;
    char level_40;
    char half_peak_40;
    char level_64;
    char half_peak_64;
    char pad_0f[1];
};
struct edge_span { unsigned short left; unsigned short right; };
struct edge_width_work {
    unsigned short settle_lines;
    short min_width_px;
    short max_width_px;
    char pad_06[2];
    unsigned int width_sum;
    short width_count;
    unsigned short bottom_search_line;
    short narrow_count;
    unsigned short edge_inset_px;
    short lost_count;
    unsigned short lost_limit;
    short present_width_px;
    short scan_length_lines;
    char pad_1c[4];
};
struct edge_top_fit {
    int run, rise, left_intercept, right_intercept, top_intercept;
};
struct edge_corner_set {
    int left_x_px, left_line, right_x_px, right_line, left_lower;
};
union byte_bits { unsigned char v; struct Bits b; };
/* img_mode, img_mode_applied: 12 bytes per side; +0 and +2 flag words (bits of the high and the low byte), +4 word (+4 bits 0-2, +5 byte), +8 word (halftone code), +10 word (+11 byte) */
struct img_side_mode {
    union {
        unsigned short v;
        struct {
            unsigned short :1,
                           color:1,
                           gray:1,
                           error_diffusion_separation_ipc:1,
                           dither_separation_ipc:1,
                           error_diffusion_separation:1,
                           dither_separation:1,
                           error_diffusion:1,
                           dither:1,
                           dtc:1,
                           sdtc_ipc:1,
                           sdtc:1,
                           outline_ipc:1,
                           outline:1,
                           threshold:1,
                           threshold_ipc:1;
        } f;
    } image_type;
    union img_path_flags_word {
        unsigned short v;
        struct {
            unsigned short multi_level:1,
                           dtc:1,
                           sdtc:1,
                           outline:1,
                           area_separation:1,
                           ipc_path:1,
                           b9:1,
                           reverse_image:1,
                           emphasis:1,
                           dither:1,
                           error_diffusion:1,
                           :2,
                           jpeg2_output:1,
                           jpeg_output:1,
                           adf:1;
        } f;
    } path_flags;
    union {
        unsigned short w;
        struct { unsigned char p:5, variance:3; unsigned char threshold; } f;
    } binarize;
    char pad_06[2];
    union {
        unsigned short v;
        struct { char hi; char lo; } c;
        struct {
            unsigned char downloaded:1,
                          b6:1,
                          :3,
                          pattern:3;
        } f;
    } halftone;
    union {
        unsigned short v;
        struct { unsigned char hi, lo; } c;
    } emphasis;
};
struct job_color_dropout { int v; unsigned char c[4]; int w; };
struct img_offset_pair { short main_offset; short sub_offset; };
struct calib_gain_set {
    unsigned short odd_chip0, even_chip0, odd_chip1, even_chip1, odd_chip2, even_chip2;
};
struct calib_afe_value { unsigned long v:32; };
struct page_record_side {
    union {
        int v;
        struct {
            unsigned int :27;
            unsigned int data_ready:1,
                         requested:1,
                         scanned:1,
                         last_chunk:1;
            unsigned int :1;
        } f;
    } flags;
    unsigned short width_px;
    unsigned short length_lines;
    unsigned int image_bytes;
    unsigned int image_bytes_sent;
    unsigned int jpeg_bytes;
    unsigned int jpeg_bytes_sent;
    short paper_width_px;
    short paper_length_lines;
    short width_unaligned_px;
    unsigned short length_unpadded_lines;
    int size_final;
    int output_lines_set;
    int l40;
    int jpeg_done;
};
struct page_record {
    union {
        int v;
        struct {
            unsigned int :2;
            unsigned int power_pending:1;
            unsigned int :1;
            unsigned int load_pending:1,
                         read_pending:1;
            unsigned int :3;
            unsigned int adf:1,
                         scan_requested:1;
            unsigned int :3;
            unsigned int sheet_fed:1,
                         scanned:1,
                         sent:1;
            unsigned int :14;
            unsigned int scan_end:1;
        } f;
    } flags;
    struct page_record_side sides[2];
    unsigned int buf_offset;
    int driver_crop;
    char tail[4];
    union {
        unsigned short v;
        struct {
            char p[1];
            struct {
                unsigned char hardware_error:1,
                              medium_error:1,
                              illegal_request:1;
                unsigned char :5;
            } f;
        } c;
    } error_flags;
    unsigned short hardware_error_bits;
    union {
        unsigned short v;
        struct { unsigned char hi; unsigned char lo; } c;
    } hardware_error_reported;
    union { short s; unsigned short v; } medium_error_bits;
    union {
        unsigned short v;
        struct { unsigned char hi; unsigned char lo; } c;
    } medium_error_reported;
    short illegal_request_bits;
    union {
        unsigned short v;
        struct { unsigned char hi; unsigned char lo; } c;
    } illegal_request_reported;
    unsigned char error_kind;
    char pad_7f[5];
    union {
        unsigned char v;
        struct { unsigned char length:1, ultrasonic:1; unsigned char :6; } f;
    } multifeed_flags;
    char pad_85[3];
};
struct page_size_record {
    int size_final;
    short length_lines, width_px, width_unaligned_px, length_unpadded_lines;
};
struct view_scsi_defect_transfer_bits { unsigned char defect_area:1, :7; };
struct cell_u8 { unsigned char v; };
struct cell_s8 { char v; };
struct cell_u16 { unsigned short v; };
struct cell_scsi_reselect_retries { unsigned char v; };
struct cell_scsi_sense_information { int v; };
struct cell_page_stall_count { unsigned short w; };
/* line_captured_lines: per-side words read as one-member structs (value first) */
struct cell_line_captured_lines { unsigned short w; };
struct maint_edge_samples { unsigned short edges[8]; };
struct u16_pair { unsigned short level_sum, b; };
struct jpeg_loaded_record {
    unsigned short compression;
    unsigned short loaded_compression;
    int color;
    int previous_color;
};
struct cell_dirt_window_ptr { unsigned short *p; };
struct log_record {
    unsigned char minutes;
    unsigned char sec;
    unsigned char type;
    unsigned char code;
    unsigned char count;
    unsigned char detail;
};
struct edge_fit_record {
    int sum_xx, sum_x, sum_xy, count, sum_y, min_x, max_x;
    unsigned short min_y, max_y, sample_limit;
    char pad_22[2];
    int sample_count, right_side;
};
struct edge_sum_record {
    int sum_x;
    int count;
    int sum_y;
    int sample_count;
};
struct cell_edge_substate_range_low { unsigned short v; };
struct cell_edge_search_result { unsigned short v; };
struct cell_edge_search_start_px { volatile short v; };
struct cell_edge_search_steps { short v; };
struct cell_edge_search_level { unsigned short v; };
struct cell_edge_search_min_left { short v; };
struct cell_edge_b0_last_width { int v; };
struct cell_edge_c0_steps { unsigned short v; };
struct cell_panel_setup_blink_phase { int v; };
struct cell_panel_error_code_polls { unsigned short v; };
struct cell_page_capture_bytes { unsigned int v; };
struct cell_scsi_win_long_length_lines { unsigned short v; };
struct cell_scsi_diag_reply_len { unsigned short v; };
struct cell_maint_white_level_side { int v; };
struct cell_maint_white_level_target { unsigned char v; };
struct cell_err_code_count { unsigned char v; };
struct cell_mdc_end_heater_warmup_bits { char v; };
struct cell_tick_count { unsigned short v; };
struct cell_tick_panel_polls { short v; };
struct cell_tick_hopper_paper_ticks { unsigned short v; };
struct cell_tick_hopper_empty_ticks { unsigned short v; };
struct cell_tick_document_cover_open_ticks { unsigned short v; };
struct cell_tick_count_last_second { unsigned short v; };
struct cell_tick_seconds { unsigned short v; };
struct cell_scsi_imprinter_text_len { unsigned char v; };
struct cell_console_tx_state { unsigned char v; };
struct cell_console_line_length { unsigned char v; };
struct cell_console_tx_ptr { unsigned long v; };
struct cell_console_dump_end { unsigned long v; };
struct cell_mdc_motor_test_motor { char v; };
struct cell_dirt_flat_count { unsigned short v; };
struct cell_dirt_dip_limit_by_port { int v; };
struct cell_job_adf_sheets_mod_10000 { unsigned int v; };
struct calib_chip_values { unsigned long chip0, chip1, chip2; };
struct calib_chip_levels { unsigned int chip0, chip1, chip2; };
union word_hi_lo_bits {
    unsigned short w;
    struct { unsigned short hi:8, lo:8; } b;
    struct { char hi, lo; } c;
};
union calib_afe_bytes {
    struct calib_afe_value l;
    struct {
        unsigned char :8,
                      chip0:8,
                      chip1:8,
                      chip2:8;
    } b;
    struct {
        unsigned long :8,
                      chip0:8,
                      chip1:8,
                      chip2:8;
    } f;
};
struct cell_calib_exposure_history { unsigned int v[2][3]; };
struct cell_line_capture_wait_count { unsigned short v; };
struct double_feed_bits {
    unsigned char :1,
                  length_report_only:1,
                  ultrasonic_report_only:1,
                  length_check:1,
                  ultrasonic_check:1,
                  :1,
                  length_tolerance:2;
};
struct edge_geometry {
    int dx_px, edge_length_px;
    unsigned short width_px, source_lines, length_source_lines, length_lines, skew_angle, x_zoom_permille, y_zoom_permille, bottom_margin_lines, paper_width_px, paper_length_lines, width_unaligned_px, length_unpadded_lines;
};
struct flash_block_entry {
    unsigned short version;
    unsigned short sum;
    int address;
    unsigned char size_kb;
    unsigned char attribute;
    char pad_0a[6];
};
struct flash_download_target {
    char pad_00[5];
    unsigned char block_count;
    unsigned char state;
    unsigned char block;
    char pad_08[4];
    int write_len;
    int read_pending;
    int read_len;
    int write_addr;
    int read_addr;
    char pad_20[4];
    unsigned char *data_ptr;
    struct flash_block_entry blocks[12];
    char pad_e8[12];
    unsigned char progress_count;
    char pad_f5;
    unsigned char mdc_firmware_version[4];
    char pad_fa[2];
};
/* calib_offset_done, calib_gain_done, calib_done_copy: two calibration bytes as one word; front = bits of the high byte (+0), back = bits of the low byte (+1) */
union calib_done_bits {
    unsigned short w;
    struct {
        unsigned short :2,
                       even_chip0:1,
                       even_chip1:1,
                       even_chip2:1,
                       odd_chip0:1,
                       odd_chip1:1,
                       odd_chip2:1,
                       lo:8;
    } front;
    struct {
        unsigned short hi:8,
                       :2,
                       even_chip0:1,
                       even_chip1:1,
                       even_chip2:1,
                       odd_chip0:1,
                       odd_chip1:1,
                       odd_chip2:1;
    } back;
    struct { unsigned short hi:8, lo:8; } u;
};
struct edge_side_word { unsigned short w; char p[42]; };
union view_job_double_feed_flags {
    unsigned char c;
    struct double_feed_bits b;
};
union view_scsi_imprinter_style {
    unsigned char v;
    struct {
        unsigned char hi:3;
        unsigned char bold:1;
        unsigned char font:2;
        unsigned char direction:2;
    } f;
};
struct view_uart_irq_status {
    char p[4];
    union {
        unsigned char v;
        struct Bits b;
        struct { unsigned char p:4, f:3, q:1; } f;
    } iir;
    union byte_bits lsr;
};
struct img_side {
    unsigned short width_px;
    unsigned short output_length_lines;
    unsigned short x_res_dpi;
    unsigned short y_res_dpi;
    unsigned short capture_width_600dpi;
    unsigned short scan_length_600dpi;
    unsigned short capture_width_px;
    unsigned short scan_length_lines;
    unsigned short capture_length_lines;
    unsigned short x_offset_600dpi;
    unsigned short x_offset_px;
    unsigned short x_zoom_permille;
    unsigned short y_zoom_permille;
    unsigned short clip_width_px;
    unsigned short x_offset_adjust;
    unsigned short x_origin_600dpi;
    unsigned short long_page_limit_lines;
    unsigned short pad_22;
    volatile int ccd_300dpi_mode;
};
struct cell_s32 { int v; };
struct img_halftone_download_entry {
    int v;
    unsigned char pad_04[2];
    unsigned char width;
    unsigned char height;
};
struct page_buf {
    union {
        int v;
        struct { short pad_00; short lo; } s;
    } base_64k;
    unsigned char flags;
    char pad_05[3];
};
struct page_side {
    unsigned short events;
    unsigned char last_buf;
    unsigned char capture_buf;
    unsigned char output_buf;
    unsigned char start_buf;
    unsigned char queued_buf;
    unsigned char queued_count;
    char last_page_buf;
    char pad_09;
    unsigned short captured_count;
    unsigned short output_count;
    char pad_0e[2];
    int uncropped;
    int last_page_forced;
    struct page_buf buf[3];
};
union scsi_win_value {
    unsigned int v;
    struct { unsigned char pad_00[2]; unsigned short lo; } c;
};
struct mdc_flag_result { unsigned char flags; unsigned char last_result; };
union view_mdc_cmd_posted {
    struct {
        unsigned short :6;
        unsigned short end_requested:1,
                       posted:1;
        unsigned short :8;
    } f;
    struct mdc_flag_result c;
};
struct line_timing {
    unsigned long lines;
    unsigned short start_base_line;
    unsigned short start_line;
    unsigned short lead_lines;
    unsigned short end_line;
    unsigned short event_20_line;
    char pad_0e[2];
};
struct scsi_regs {
    char pad_00;
    union byte_bits mainint;
    char pad_02;
    union byte_bits scsiint1;
    char pad_04;
    union byte_bits scsiint2;
    char pad_06[9];
    unsigned char reset;
    char pad_10[3];
    union byte_bits scsimode0;
    char pad_14;
    union byte_bits scsimode1;
    char pad_16;
    union byte_bits scsictl;
    char pad_18[3];
    union {
        unsigned char v;
        struct { unsigned char h:4, l:4; } f;
    } syncmode;
    char pad_1c;
    unsigned char ownid;
    char pad_1e;
    union {
        unsigned char v;
        struct {
            unsigned char p:1,
                          initiator_id:3,
                          q:1,
                          reselect_id:3;
        } f;
    } sdid;
    char pad_20;
    unsigned char seltime;
    char pad_22;
    struct Bits fifoctl;
    char pad_24;
    unsigned char fifodata;
    char pad_26;
    unsigned char ndmasize;
    char pad_28;
    unsigned char command;
    char pad_2a[5];
    union byte_bits dmactl;
    char pad_30[3];
    unsigned char dtbc2;
    char pad_34;
    unsigned char dtbc1;
    char pad_36;
    unsigned char dtbc0;
    char pad_38;
    union byte_bits config0;
    char pad_3a;
    unsigned char config1;
};
struct usb_regs {
    char pad_00;
    union byte_bits main_int_stat;
    char pad_02;
    unsigned char sie_int_stat;
    char pad_04;
    unsigned char reg_02;
    char pad_06;
    union byte_bits ep0_int_stat;
    char pad_08;
    unsigned char epa_int_stat;
    char pad_0a;
    unsigned char epb_int_stat;
    char pad_0c;
    unsigned char reg_06;
    char reg_07[3];
    unsigned char main_int_enb;
    char pad_12;
    unsigned char sie_int_enb;
    char pad_14;
    unsigned char reg_0a;
    char reg_0b[3];
    unsigned char epa_int_enb;
    char reg_0d_0f[7];
    union byte_bits chip_reset;
    char reg_11[3];
    union byte_bits pm_control;
    char pad_26;
    unsigned char usb_control;
    char pad_28;
    struct Bits usb_status;
    char pad_2a;
    unsigned char xcvr_control;
    char pad_2c;
    unsigned char usb_test;
    char pad_2e;
    struct Bits reg_17;
    char reg_18_1e[15];
    unsigned char usb_bank;
    char pad_40;
    unsigned char reg_20;
    char reg_21[3];
    unsigned char reg_22;
    char pad_46;
    unsigned char reg_23;
    char pad_48;
    unsigned char reg_24;
    char pad_4a;
    unsigned char reg_25;
    char reg_26_29[9];
    struct Bits ep0_control_in;
    char pad_56;
    struct Bits ep0_control_out;
    char reg_2c_2f[9];
    unsigned char reg_30;
    char pad_62;
    unsigned char reg_31;
    char pad_64;
    unsigned char reg_32;
    char pad_66;
    unsigned char reg_33;
    char pad_68;
    unsigned char reg_34;
    char pad_6a;
    unsigned char reg_35;
    char pad_6c;
    unsigned char reg_36;
    char pad_6e;
    unsigned char reg_37;
};
struct log_event {
    int seconds;
    unsigned char type;
    char code;
    char count;
    char z;
};
union line_side_flags {
    unsigned short v;
    struct {
        unsigned char :3;
        unsigned char mark_sampling:1;
        unsigned char :4;
        unsigned char page_state:2;
        unsigned char :2;
        unsigned char follow_state:2;
        unsigned char :2;
    } f;
    /* the same two fields in a 16-bit unit */
    struct {
        unsigned short :8;
        unsigned short page_state:2;
        unsigned short :2;
        unsigned short follow_state:2;
        unsigned short :2;
    } s;
};
struct line_side_record {
    union line_side_flags state_flags;
    unsigned short line_count;
    unsigned short elapsed_lines;
    unsigned short capture_lines;
    unsigned short top_edge_lines;
    unsigned short stop_target_lines;
    unsigned short follow_lines;
    char pad_0e[6];
};
union line_page_flags {
    unsigned short v;
    /* a 16-bit unit */
    struct {
        unsigned short :7;
        unsigned short page_error:1;
        unsigned short :4;
        unsigned short page_step_state:2;
        unsigned short :2;
    } f;
};
struct line_page_side {
    union line_page_flags state_flags;
    unsigned short top_offset_lines;
    unsigned short end_line;
    unsigned short pad_06;
    int auto_size;
};
struct img_crop_rect {
    unsigned short width_px;
    unsigned short length_lines;
    unsigned short x_start_px;
    unsigned short y_start_lines;
    unsigned short x_offset_px;
    unsigned short y_offset_lines;
};
struct edge_record_set { struct edge_record e[3]; };
struct cell_iic_panel_digit { unsigned char v; };
struct cell_scsi_win_current_end { unsigned char *v; };
union view_job_readtime_ticks {
    int v;
    struct { unsigned char pad_00[2]; short lo; } c;
};
struct cell_tick_flatbed_lamp_on_ticks { unsigned int v; };
struct cell_tick_readtime_ticks { int v; };
struct cell_scsi_imprinter_counter_step { unsigned char v; };

/* variables */
extern struct edge_level_work edge_level_search[];
extern struct edge_top_work edge_top_search[];
extern struct edge_span edge_line[];
extern struct edge_width_work edge_width_track[];
extern struct edge_top_fit edge_top_line[];
extern struct edge_corner_set edge_top_corners[];
extern struct edge_geometry edge_page_geometry[];
extern unsigned short edge_fit_saved_a;
extern unsigned short edge_fit_saved_b;
extern int edge_fit_first;
extern struct cell_u16 edge_fit_margin;
union view_edge_fit_limit { struct cell_u16 s; short v; };
extern union view_edge_fit_limit edge_fit_limit;
extern unsigned short edge_substate_range_high;
extern struct cell_edge_substate_range_low edge_substate_range_low;
extern unsigned short edge_substate_upper;
extern unsigned short edge_substate_lower;
extern unsigned short edge_source_lines_extra;
extern unsigned char edge_pad_1[];
extern unsigned short edge_line_step;
extern unsigned short edge_fit_rows[];
extern unsigned short edge_fit_row0_b[];
extern unsigned short edge_fit_row1_a[];
extern unsigned short edge_fit_row1_b[];
extern int edge_fit_done;
extern int edge_corners_result;
extern int edge_substate_found;
extern unsigned short edge_last_line[];
extern short edge_top_confirm_lines[];
extern int edge_top_corner_x;
extern unsigned char edge_unused_1[];
extern struct cell_edge_search_result edge_search_result;
extern struct cell_edge_search_start_px edge_search_start_px;
extern struct cell_edge_search_steps edge_search_steps;
extern unsigned short edge_search_offsets[];
extern struct cell_edge_search_level edge_search_level;
extern struct cell_edge_search_min_left edge_search_min_left;
extern unsigned short edge_search_hits;
extern unsigned short edge_right_limit_hits;
extern volatile unsigned short edge_right_limit;
struct cell_edge_flatbed_limit_lines { unsigned short v; };
extern struct cell_edge_flatbed_limit_lines edge_flatbed_limit_lines;
extern unsigned char edge_b0_state;
extern volatile unsigned short edge_b0_index;
extern unsigned short edge_b0_start_index;
struct cell_edge_b0_runs { unsigned short v; };
extern struct cell_edge_b0_runs edge_b0_runs;
extern volatile unsigned short edge_b0_steps;
extern unsigned char edge_b0_flag;
extern struct cell_edge_b0_last_width edge_b0_last_width;
extern int edge_b0_falling;
extern unsigned char edge_c0_state;
extern unsigned short edge_c0_index;
extern unsigned short edge_c0_start_index;
extern struct cell_edge_c0_runs edge_c0_runs;
extern struct cell_edge_c0_steps edge_c0_steps;
extern unsigned char edge_c0_retry;
extern struct win_param_block *img_window;
extern char *img_gamma_buf;
extern struct img_gamma_key *img_gamma_entry;
extern unsigned short *img_halftone_loaded;
extern char img_edge_levels[];
extern char img_edge_level_1[];
extern char img_edge_level_2[];
extern char img_edge_level_3[];
extern unsigned short img_deskew_margin[];
extern unsigned short img_top_distance_lines[];
extern unsigned short img_back_delay_lines;
extern int scsi_saved_imask;
/* RAM copies of the port pointers p_pa, p_pb, p_pe, p_pf (hw_port_f: see below) */
extern volatile struct st_pa *hw_port_a;
extern volatile struct st_pb *hw_port_b;
extern volatile struct st_pe *hw_port_e;
extern struct Bits *hw_port_f;
extern char iic_panel_port0;
extern struct cell_iic_panel_digit iic_panel_digit;
extern int panel_update_pending;
extern char hw_scsi_ctrl_dummy[];
extern int scsi_wait_selection_pending;
extern int scsi_command_received;
extern union byte_bits scsi_mainint_status;
extern union byte_bits scsi_scsiint1_status;
extern union byte_bits scsi_scsiint2_status;
extern unsigned char scsi_scsictl_status;
extern unsigned char scsi_last_ctrl_command;
extern unsigned char scsi_unused_1[];
extern struct view_uart_irq_status uart_irq_status[];
extern unsigned char uart_unused_1[];
extern void *hw_vdcc_back_gpio_copy;
union view_link_reply_11_value {
    unsigned short v;
    struct { unsigned char pad_00; volatile unsigned char lo; } c;
};
extern union view_link_reply_11_value link_reply_11_value;
extern unsigned char selftest_startup_buttons;
extern volatile unsigned char selftest_startup_sensors;
extern int selftest_startup_done;
extern int selftest_passed;
extern int job_calibrated_since_mdc_start;
struct view_power_state_bits {
    unsigned char enter_request:1,
                  leave_request:1,
                  saving:1,
                  :2,
                  unused:1,
                  :2;
};
union view_power_state { unsigned char v; struct view_power_state_bits f; };
extern union view_power_state power_state;
extern union byte_bits_c button_state;
extern unsigned char job_adf_sheet_count;
extern unsigned char selftest_settings_0x49_value;
extern int calib_levels_request;
extern int calib_levels_due;
extern int calib_exposure_request;
extern int calib_exposure_due;
extern int calib_in_lamp_check;
extern int calib_extrapolate_request;
extern int calib_extrapolate_due;
extern int calib_warmup_idle;
extern volatile int job_sheet_count;
struct cell_button_power_latch { unsigned char v; };
extern struct cell_button_power_latch button_power_latch;
extern unsigned char power_off_running;
extern unsigned char job_pause_requested;
struct cell_job_pause_page_count { unsigned char v; };
extern struct cell_job_pause_page_count job_pause_page_count;
extern struct Bits button_send_to_wait_release;
struct view_power_off_flags_bits {
    unsigned char enabled:1,
                  requested:1,
                  idle_restarted:1,
                  warning_due:1,
                  :4;
};
union view_power_off_flags {
    unsigned char v;
    struct view_power_off_flags_bits f;
};
extern union view_power_off_flags power_off_flags;
extern char job_applied_power_off_time;
extern char job_applied_power_off_warning;
extern int job_applied_power_off_enable;
struct cell_page_fed_count { unsigned short v; };
extern struct cell_page_fed_count page_fed_count;
extern unsigned char page_pad_1[];
extern struct img_side_mode img_mode[];
extern struct img_side_mode img_mode_applied[];
extern struct img_side img_side_params[];
extern struct cell_s32 img_gamma_inverted[];
extern struct job_color_dropout job_dropout[];
extern struct job_color_dropout job_dropout_previous[];
extern union view_job_double_feed_flags job_double_feed_flags;
extern int scsi_setup_mode;
extern unsigned char job_double_feed_default;
extern int job_prepick_default;
extern int job_dropout_default[];
extern int scsi_imprinter_error;
extern unsigned char scsi_imprinter_error_byte_1[];
extern unsigned char scsi_imprinter_error_byte_2[];
extern unsigned char scsi_imprinter_error_byte_3[];
extern unsigned char scsi_imprinter_error_byte_4[];
extern unsigned char img_overscan_on;
struct cell_img_overscan_margin { short v; };
extern struct cell_img_overscan_margin img_overscan_margin;
extern unsigned char img_overscan_shift_on[2];
extern unsigned char img_overscan_shift_negative[2];
struct cell_img_overscan_shift { short v; };
extern struct cell_img_overscan_shift img_overscan_shift[2];
extern unsigned short img_overscan_shift_lines[2][3];
struct cell_img_follow_start_lines { unsigned long v; };
extern struct cell_img_follow_start_lines img_follow_start_lines[2];
extern struct Bits link_imprinter_flags;
extern unsigned long scsi_imprinter_counter_initial;
extern struct img_offset_pair img_fine_offsets[];
struct cell_job_sensor_sides { unsigned char v; };
extern struct cell_job_sensor_sides job_sensor_sides;
extern unsigned char job_pad_1[];
extern struct Bits power_saved_sensor_bits;
struct panel_state {
    unsigned char display_main;
    unsigned char display_sub;
    struct Bits display_flags;
    char pad_03;
    unsigned char saved_main, saved_sub;
    char pad_06[2];
    volatile unsigned char glyph_shown;
    unsigned char char_count;
    unsigned char repeat_limit;
    union UBF startup_error_request;
    union UBF startup_error_shown;
    char pad_0d;
    short char_polls;
    short pause_polls;
    unsigned char chars[30];
    unsigned int page_activity;
    int unread_word_52;
    unsigned char char_index;
    unsigned char repeat_count;
    unsigned char button_power_press_polls, button_power_release_polls, button_scan_press_polls, button_scan_release_polls, button_send_to_press_polls, button_send_to_release_polls;
    int button_function_request;
    unsigned char button_function_press_polls;
    unsigned char button_function_release_polls;
    char pad_46[2];
    int button_function_back_request;
    unsigned char function_number;
    char pad_4d;
    unsigned short button_power_press_latch_polls, button_power_press_unlatch_polls;
    char pad_52[2];
    int button_power_press_latch;
    unsigned short button_scan_latch_polls, button_scan_unlatch_polls;
    int button_scan_latch;
    unsigned short button_send_to_latch_polls, button_send_to_unlatch_polls;
    int button_send_to_latch;
    int roller_turn_request;
};
extern struct panel_state panel_current;
extern int panel_setup_shown;
extern struct cell_panel_setup_blink_phase panel_setup_blink_phase;
extern short panel_setup_blink_polls;
extern struct panel_state panel_saved;
extern int panel_error_code_shown;
extern struct cell_panel_error_code_polls panel_error_code_polls;
struct cell_selftest_memory_error_code { unsigned char v; };
extern struct cell_selftest_memory_error_code selftest_memory_error_code;
extern struct cell_panel_multifeed_blink_state panel_multifeed_blink_state;
extern struct cell_panel_multifeed_blink_polls panel_multifeed_blink_polls;
extern struct cell_u16 panel_patterns_cleared_polls;
extern struct cell_u16 panel_multifeed_blink_age_polls;
extern volatile unsigned short job_buffer_feed_flags;
extern int main_settings_request;
extern int scsi_object_position_done;
extern int job_driver_function_confirm;
extern unsigned short job_button_pause_flags;
extern unsigned short selftest_mark_start;
extern unsigned short selftest_profile_count;
extern unsigned char selftest_mark_offset;
extern unsigned short calib_started;
extern unsigned short calib_busy;
extern unsigned short calib_mode;
extern unsigned short calib_running;
extern unsigned short calib_wait_count[];
extern unsigned short calib_line_count;
extern unsigned short calib_step;
extern unsigned short calib_step_result;
extern unsigned short calib_apply_flag;
extern unsigned char calib_unused_1[];
extern unsigned short calib_sides;
extern union calib_afe_bytes calib_offset_odd_front;
extern union calib_afe_bytes calib_offset_even_front;
extern union calib_afe_bytes calib_offset_odd_back;
extern union calib_afe_bytes calib_offset_even_back;
extern struct calib_afe_value calib_offset_result_odd_front;
extern struct calib_afe_value calib_offset_result_even_front;
extern struct calib_afe_value calib_offset_result_odd_back;
extern struct calib_afe_value calib_offset_result_even_back;
extern union calib_afe_bytes calib_offset_search_odd_front;
extern union calib_afe_bytes calib_offset_search_even_front;
extern union calib_afe_bytes calib_offset_search_odd_back;
extern union calib_afe_bytes calib_offset_search_even_back;
extern unsigned char calib_gain_target_front;
extern unsigned char calib_gain_target_2_front;
extern struct cell_calib_offset_target_front calib_offset_target_front;
extern unsigned char calib_offset_target_2_front;
extern unsigned char calib_offset_target_3_front;
extern volatile unsigned short calib_gain_target_x4_front;
extern volatile unsigned short calib_gain_target_2_x4_front;
extern volatile unsigned short calib_offset_target_x4_front;
extern volatile unsigned short calib_offset_target_2_x4_front;
extern volatile unsigned short calib_offset_target_3_x4_front;
extern unsigned char calib_gain_target_back;
extern unsigned char calib_gain_target_2_back;
extern struct cell_calib_offset_target_back calib_offset_target_back;
extern unsigned char calib_offset_target_2_back;
extern unsigned char calib_offset_target_3_back;
extern volatile unsigned short calib_gain_target_x4_back;
extern volatile unsigned short calib_gain_target_2_x4_back;
extern volatile unsigned short calib_offset_target_x4_back;
extern volatile unsigned short calib_offset_target_2_x4_back;
extern volatile unsigned short calib_offset_target_3_x4_back;
extern union calib_afe_bytes calib_gain_odd_front;
extern union calib_afe_bytes calib_gain_even_front;
extern union calib_afe_bytes calib_gain_odd_back;
extern union calib_afe_bytes calib_gain_even_back;
extern struct calib_afe_value calib_gain_result_odd_front;
extern struct calib_afe_value calib_gain_result_even_front;
extern struct calib_afe_value calib_gain_result_odd_back;
extern struct calib_afe_value calib_gain_result_even_back;
extern union calib_afe_bytes calib_gain_search_odd_front;
extern union calib_afe_bytes calib_gain_search_even_front;
extern union calib_afe_bytes calib_gain_search_odd_back;
extern union calib_afe_bytes calib_gain_search_even_back;
extern unsigned char calib_unused_2[];
extern volatile union word_hi_lo_bits calib_black_pass1_front_chip0;
extern volatile union word_hi_lo_bits calib_black_pass1_front_chip1;
extern volatile union word_hi_lo_bits calib_black_pass1_front_chip2;
extern volatile union word_hi_lo_bits calib_black_pass2_front_chip0;
extern volatile union word_hi_lo_bits calib_black_pass2_front_chip1;
extern volatile union word_hi_lo_bits calib_black_pass2_front_chip2;
extern volatile union word_hi_lo_bits calib_black_pass1_back_chip0;
extern volatile union word_hi_lo_bits calib_black_pass1_back_chip1;
extern volatile union word_hi_lo_bits calib_black_pass1_back_chip2;
extern volatile union word_hi_lo_bits calib_black_pass2_back_chip0;
extern volatile union word_hi_lo_bits calib_black_pass2_back_chip1;
extern volatile union word_hi_lo_bits calib_black_pass2_back_chip2;
extern union calib_done_bits calib_offset_done;
extern union calib_done_bits calib_gain_done;
extern union calib_done_bits calib_done_copy;
/* calib_exposure_calibrated..calib_black_level: tables with rows of 3 */
extern unsigned int calib_exposure_calibrated[][3];
extern unsigned char calib_exposure_calibrated_1_0[];
extern unsigned char calib_exposure_calibrated_1_1[];
extern unsigned char calib_exposure_calibrated_1_2[];
extern unsigned int calib_exposure_w4[][3];
extern struct calib_chip_values calib_exposure_w4_back[];
extern unsigned int calib_exposure_reference[][3];
extern unsigned char calib_exposure_reference_1_0[];
extern unsigned char calib_exposure_reference_1_1[];
extern unsigned char calib_exposure_reference_1_2[];
extern unsigned int calib_exposure[][3];
extern struct calib_chip_values calib_exposure_back[];
extern unsigned short calib_offset_bytes[][6];
extern unsigned short calib_offset_bytes_back[][6];
extern struct calib_gain_set calib_gain_bytes[];
extern unsigned char calib_gain_bytes_back[];
extern unsigned short calib_black_level[][3];
extern unsigned char calib_white_level[][3];
extern char calib_white_level_back[];
extern char calib_white_level_back_1[];
extern char calib_white_level_back_2[];
extern unsigned char calib_white_level_fine[][3];
extern char calib_white_level_fine_back[];
extern char calib_white_level_fine_back_1[];
extern char calib_white_level_fine_back_2[];
extern struct calib_chip_levels calib_mode_12_level_front[];
extern struct calib_chip_levels calib_mode_12_level_back[];
extern int calib_lamp_adjust_first;
extern unsigned char calib_unused_3[];
extern union calib_afe_bytes calib_offset_saved_odd_front;
extern union calib_afe_bytes calib_offset_saved_even_front;
extern union calib_afe_bytes calib_offset_saved_odd_back;
extern union calib_afe_bytes calib_offset_saved_even_back;
extern unsigned char calib_unused_4[];
extern struct calib_afe_value calib_gain_saved_odd_front;
extern struct calib_afe_value calib_gain_saved_even_front;
extern struct calib_afe_value calib_gain_saved_odd_back;
extern struct calib_afe_value calib_gain_saved_even_back;
extern unsigned char calib_unused_5[];
extern int calib_lamp_11_flag;
extern unsigned short calib_wait_abort;
extern int calib_flatbed_high_target;
extern int calib_exposure_history_on;
extern unsigned short calib_exposure_history_count;
extern unsigned short calib_exposure_history_time[];
extern struct cell_calib_exposure_history calib_exposure_history[];
extern int job_early_warmup;
extern unsigned short calib_measure_mode_swap;
extern unsigned short img_top_filler_lines;
extern unsigned short edge_left_front[1400];
extern unsigned short edge_right_front[];
extern unsigned short edge_left_back[];
extern unsigned short edge_right_back[];
struct edge_point { unsigned short corner_line; unsigned short bottom_line; };
extern struct edge_point edge_bottom_point[2];
extern unsigned char iic_eeprom_bad_cells;
extern unsigned char iic_eeprom_load_passes;
extern struct Bits iic_busy;
extern volatile unsigned short iic_pending_len;
extern volatile unsigned short iic_pending_offset;
extern unsigned char iic_pending_settings[];
extern unsigned char iic_pending_power_off_time[];
extern unsigned char iic_pending_power_off_warning[];
extern unsigned char iic_pending_upper_half[];
extern struct img_table_state img_loaded_tables[];
extern struct img_halftone_download_entry img_halftone_downloads[];
extern unsigned char img_gamma_loaded[];
extern unsigned char img_lut_loaded[];
extern char img_lut_tables[][3072];
extern char img_gamma_tables[][3072];
extern char img_halftone_matrix[][2048];
extern unsigned char img_download_halftone[][1024];
extern unsigned char img_download_gamma[][1024];
extern unsigned char img_download_lut[][3072];
extern int scsi_send_qualifier;
extern int img_vdcc_tables_loaded;
extern struct flash_download_target flash_download_targets[];
extern int flash_download_target_1_byte_0[];
extern int flash_download_target_2_byte_0[];
extern int flash_download_pending;
extern unsigned char flash_download_session;
extern unsigned char flash_unused_1[];
extern char flash_download_buf[];
extern unsigned char flash_download_block_info[][16];
extern unsigned char flash_pad_1[];

/* variables */
struct hw_device_error_bits {
    unsigned char panel_expander:1,
                  eeprom:1,
                  scsi_interface:1,
                  image_memory:1,
                  cga_board:1,
                  :3;
};
union hw_device_error_flags {
    unsigned char v;
    struct hw_device_error_bits f;
};
extern union hw_device_error_flags err_pending_flags;
extern int err_shipping_lock_pending;
extern int err_mdc_alarm;
struct cell_page_buf_count { int v; };
extern struct cell_page_buf_count page_buf_count;
extern struct cell_page_scanning page_scanning;
extern struct cell_page_current page_current;
extern struct cell_page_scanned_count page_scanned_count;
struct cell_page_sent_count { volatile int v; };
extern struct cell_page_sent_count page_sent_count;
extern int page_record_ring;
struct cell_page_buf_bytes { unsigned int v; };
extern struct cell_page_buf_bytes page_buf_bytes;
extern struct cell_page_capture_bytes page_capture_bytes;
extern int page_buf_limit_bytes;
extern struct page_record page_records[];
extern struct page_size_record scsi_last_page_size[];
extern int job_aborted;
extern int scsi_page_read_done;
extern int scsi_last_page_driver_crop;
extern int job_buffer_mode_on;
extern struct page_side page_side_state[];
extern struct page_record page_side_records[];
extern struct Bits win_scanning_order;
extern unsigned char win_unused_2[];
struct cell_err_last_code { unsigned char v; };
union view_err_last_code {
    struct cell_err_last_code s;
    struct { char v; } c;
};
extern union view_err_last_code err_last_code;
extern volatile unsigned char err_feed_params_pending;
extern unsigned char job_feed_starting;
extern unsigned char job_separation_motion_wait;
extern int job_prepick_on;
extern int job_interlace_read;
extern volatile int job_black_backing;
extern int job_keep_backing;
extern struct Bits mdc_fan_alarm;
extern unsigned char mdc_pad_1[];
struct view_link_page_info_flags_bits { unsigned char computing:1, :7; };
extern struct view_link_page_info_flags_bits link_page_info_flags;
extern unsigned char link_pad_1[];
extern struct Bits job_device_controls;
extern unsigned char mdc_cmd_log[];
extern unsigned char mdc_cmd_result_log[];
extern unsigned char mdc_df_test_result_log[];
extern int mdc_cmd_log_index;
extern unsigned char mdc_unused_4[];
extern unsigned char scsi_param_buf[];
extern unsigned char scsi_param_byte_1[];
extern unsigned char scsi_param_byte_2[];
extern unsigned char scsi_param_byte_3[];
extern unsigned char scsi_param_byte_4[];
extern unsigned char scsi_param_byte_5[];
extern unsigned char scsi_param_byte_6[];
extern unsigned char scsi_param_byte_7[];
extern unsigned char scsi_param_byte_8[];
extern unsigned char scsi_param_byte_10[];
extern unsigned char scsi_param_byte_11[];
extern unsigned char scsi_param_byte_12[];
extern unsigned char scsi_param_byte_16[];
extern unsigned char scsi_param_byte_17[];
extern unsigned char scsi_param_byte_18[];
extern unsigned char scsi_param_byte_19[];
extern unsigned char scsi_param_byte_22[];
extern unsigned short link_tx_frame[];
union view_link_tx_byte_2 { char c; char a[2]; };
extern union view_link_tx_byte_2 link_tx_byte_2;
extern unsigned char link_tx_byte_4[];
extern unsigned char link_tx_byte_5[];
extern unsigned short link_tx_byte_6[];
extern unsigned short link_tx_byte_8[];
extern unsigned short link_tx_byte_10[];
extern unsigned char link_tx_byte_12[];
extern unsigned char link_tx_byte_13[];
extern unsigned char link_tx_byte_14[];
extern unsigned char link_tx_byte_15[];
extern unsigned char link_tx_byte_16[];
extern unsigned char link_tx_byte_17[];
extern unsigned char link_tx_byte_18[];
extern unsigned char link_tx_byte_19[];
extern unsigned char link_tx_byte_20[];
extern unsigned char link_tx_byte_21[];
extern char link_tx_byte_22[];
extern unsigned char link_tx_byte_23[];
extern unsigned char link_tx_byte_24[];
extern unsigned char link_tx_byte_25[];
extern unsigned char link_tx_byte_26[];
extern unsigned char link_tx_byte_27[];
extern unsigned char link_tx_byte_43[];
extern unsigned char link_tx_byte_50[];
extern unsigned char link_tx_byte_51[];
extern unsigned char link_tx_byte_52[];
extern unsigned char link_tx_byte_54[];
extern unsigned char link_tx_byte_56[];
extern unsigned char link_tx_byte_57[];
extern unsigned char link_tx_byte_61[];
extern unsigned char link_tx_byte_64[];
extern unsigned char link_tx_byte_66[];
extern unsigned char link_tx_byte_71[];
extern unsigned char link_tx_byte_72[];
extern unsigned char link_tx_byte_73[];
extern unsigned char link_tx_byte_77[];
extern unsigned char link_tx_byte_81[];
extern unsigned char link_tx_byte_82[];
extern unsigned char link_tx_byte_84[];
extern unsigned char link_tx_byte_85[];
extern unsigned char link_tx_byte_88[];
extern unsigned char link_tx_byte_89[];
extern unsigned char link_tx_byte_90[];
extern unsigned char link_tx_byte_91[];
extern unsigned char link_tx_byte_120[];
extern unsigned char link_tx_byte_130[];
extern unsigned char link_tx_byte_131[];
extern unsigned char link_tx_byte_179[];
extern unsigned char link_tx_byte_180[];
extern unsigned char link_tx_byte_182[];
extern unsigned char link_tx_byte_184[];
extern unsigned char link_tx_byte_185[];
extern unsigned char link_tx_byte_189[];
extern unsigned char link_tx_byte_199[];
extern unsigned short scsi_win_saved_descriptors[];
extern unsigned short scsi_win_saved_back_descriptor[];
extern unsigned short scsi_win_descriptor_len;
extern unsigned char scsi_pad_1[];
extern unsigned char scsi_cdb[];
extern unsigned char scsi_cdb_byte_1[];
extern unsigned char scsi_cdb_byte_2[];
extern unsigned char scsi_cdb_byte_3[];
extern unsigned char scsi_cdb_byte_4[];
extern unsigned char scsi_cdb_byte_5[];
extern unsigned char scsi_cdb_byte_6[];
extern struct cell_u8 scsi_cdb_byte_7[];
extern struct cell_u8 scsi_cdb_byte_8[];
extern unsigned char scsi_cdb_byte_9[];
extern unsigned char scsi_reply_buf[];
extern unsigned char scsi_reply_byte_1[];
extern unsigned char scsi_reply_byte_2[];
extern unsigned char scsi_reply_byte_3[];
extern unsigned char scsi_reply_byte_4[];
extern unsigned char scsi_reply_byte_5[];
extern unsigned char scsi_reply_byte_6[];
extern unsigned char scsi_reply_byte_7[];
extern unsigned char scsi_reply_byte_8[];
extern unsigned char scsi_reply_byte_9[];
extern unsigned char scsi_reply_byte_10[];
extern unsigned char scsi_reply_byte_11[];
extern unsigned char scsi_reply_byte_12[];
extern unsigned char scsi_reply_byte_13[];
extern unsigned char scsi_reply_byte_14[];
extern unsigned char scsi_reply_byte_15[];
extern unsigned char scsi_reply_byte_16[];
extern unsigned char scsi_reply_byte_17[];
extern unsigned char scsi_reply_byte_18[];
extern unsigned char scsi_reply_byte_19[];
extern unsigned char scsi_reply_byte_20[];
extern unsigned char scsi_reply_byte_21[];
extern unsigned char scsi_reply_byte_22[];
extern unsigned char scsi_reply_byte_23[];
extern unsigned char scsi_reply_byte_24[];
extern unsigned char scsi_reply_byte_25[];
extern unsigned char scsi_reply_byte_28[];
extern unsigned char scsi_reply_byte_29[];
extern unsigned char scsi_reply_byte_30[];
extern unsigned char scsi_reply_byte_31[];
extern unsigned char scsi_reply_byte_32[];
extern unsigned char scsi_reply_byte_33[];
extern unsigned short scsi_reply_byte_34[];
extern unsigned char scsi_reply_byte_36[];
extern unsigned char scsi_reply_byte_37[];
extern unsigned char scsi_reply_byte_38[];
extern unsigned char scsi_reply_byte_39[];
extern unsigned char scsi_reply_byte_40[];
extern unsigned char scsi_reply_byte_41[];
extern unsigned char scsi_reply_byte_42[];
extern unsigned char scsi_reply_byte_43[];
extern unsigned char scsi_reply_byte_44[];
extern unsigned char scsi_reply_byte_45[];
extern unsigned char scsi_reply_byte_46[];
extern unsigned char scsi_reply_byte_47[];
extern unsigned char scsi_reply_byte_48[];
extern unsigned char scsi_reply_byte_49[];
extern unsigned char scsi_reply_byte_86[];
extern unsigned char scsi_reply_byte_87[];
extern unsigned short scsi_reply_byte_88[];
extern unsigned char scsi_reply_byte_90[];
extern unsigned short scsi_reply_byte_92[];
extern unsigned char scsi_reply_byte_98[];
extern unsigned char scsi_reply_byte_100[];
extern unsigned char scsi_reply_byte_101[];
extern unsigned char scsi_reply_byte_102[];
extern unsigned char scsi_reply_byte_103[];
extern unsigned char scsi_reply_byte_111[];
extern unsigned char scsi_reply_byte_112[];
extern unsigned char scsi_reply_byte_115[];
extern unsigned char scsi_reply_byte_118[];
extern unsigned char scsi_reply_byte_128[];
extern unsigned char scsi_reply_byte_129[];
extern unsigned char scsi_reply_byte_177[];
extern unsigned char scsi_reply_byte_178[];
extern unsigned char scsi_reply_byte_180[];
extern unsigned char scsi_reply_byte_182[];
extern unsigned char scsi_reply_byte_183[];
extern unsigned char scsi_reply_byte_187[];
extern unsigned char scsi_reply_byte_190[];
extern unsigned char scsi_reply_byte_192[];
extern unsigned char scsi_reply_byte_197[];
extern unsigned char scsi_reply_byte_198[];
extern unsigned char scsi_reply_byte_199[];
extern unsigned char scsi_reply_byte_203[];
extern unsigned char scsi_reply_byte_207[];
extern unsigned char scsi_reply_byte_208[];
extern unsigned char scsi_reply_byte_210[];
extern unsigned char scsi_reply_byte_211[];
extern unsigned char scsi_reply_byte_214[];
extern unsigned char scsi_reply_byte_215[];
extern unsigned char scsi_reply_byte_216[];
extern unsigned char scsi_reply_byte_217[];
extern unsigned char scsi_sense_data[];
extern unsigned char scsi_sync_params[];
extern unsigned char scsi_sync_offsets[];
extern union view_scsi_initiator_bit scsi_initiator_bit;
extern struct cell_scsi_sense_owner scsi_sense_owner;
extern struct cell_s8 scsi_reserved_by;
extern unsigned char scsi_identify_message;
extern unsigned char scsi_ua_reported;
extern unsigned char scsi_mode_changed;
extern unsigned char scsi_sync_initiators;
extern char scsi_status_byte;
extern unsigned char scsi_message_byte;
extern struct cell_scsi_reply_message scsi_reply_message;
extern char scsi_ext_message[];
extern char scsi_extended_message_len[];
extern unsigned char scsi_extended_message_code[];
extern unsigned char scsi_sdtr_period[];
extern unsigned char scsi_sdtr_offset[];
extern unsigned char scsi_disconnect_allowed;
extern struct cell_scsi_disconnected_initiator scsi_disconnected_initiator;
extern unsigned char scsi_reselect_active;
extern struct cell_scsi_reselect_retries scsi_reselect_retries;
extern struct cell_scsi_message_retries scsi_message_retries;
extern union view_scsi_param_len scsi_param_len;
extern union view_scsi_xfer_len scsi_xfer_len;
extern struct cell_scsi_sense_information scsi_sense_information;
extern int scsi_reselect_failed;
extern int scsi_deferred;
struct view_scsi_read_sides {
    union {
        unsigned short w;
        struct { char pad_00; union UBF lo; } b;
    } flags;
    unsigned short pad_02;
    int window_set;
    int data_due;
};
extern struct view_scsi_read_sides scsi_read_sides[2];
extern int scsi_read_check_value;
extern int xfer_last_chunk;
extern int scsi_scan_requested;
extern unsigned char scsi_unused_2[];
extern int main_page_loop_phase;
extern int xfer_chunk_max;
extern int xfer_chunk_max_2;
extern volatile int xfer_chunk_bytes;
extern int win_long_length;
extern unsigned char *scsi_win_current_descriptor;
extern struct cell_scsi_win_current_end scsi_win_current_end;
extern unsigned short scsi_win_size_rounding;
extern struct cell_scsi_win_long_length_lines scsi_win_long_length_lines;
extern union view_scsi_read_side scsi_read_side;
extern int win_extra_long_length;
extern struct cell_scsi_read_window_id scsi_read_window_id;
extern unsigned char job_read_8_byte_3;
extern unsigned char scsi_unusual_paper_state;
extern unsigned char scsi_pad_2[];
extern union UBF scsi_read_progress;
extern unsigned char scsi_pad_3[];
extern struct job_mode_record job_mode_params[];
extern unsigned char scsi_diag_request;
extern struct cell_u8 scsi_testmode_result_state;
extern struct cell_u16 scsi_diag_ram_addr_hi;
extern struct cell_u16 scsi_diag_ram_addr_lo;
extern struct cell_scsi_diag_reply_len scsi_diag_reply_len;
extern int scsi_first_read_date_stored;
extern struct cell_u16 link_memory_addr_high;
extern unsigned short link_memory_addr_low;
struct cell_scsi_diag_data_len { unsigned short v; };
extern struct cell_scsi_diag_data_len scsi_diag_data_len;
extern int scsi_mdc_log_request;
extern unsigned char scsi_sop_put_valid;
extern char scsi_mdc_log_pending;
extern unsigned short scsi_imprinter_slot;
extern unsigned short scsi_dev_log_selector;
extern unsigned short scsi_dev_log_offset;
extern unsigned short scsi_dev_log_chunk_size;
extern struct cell_page_stall_count page_stall_count[];
struct cell_page_stall_limit { unsigned short v; };
extern struct cell_page_stall_limit page_stall_limit[];
struct view_scsi_resume_flags_bits {
    unsigned char resume_pending:1,
                  page_ended:1,
                  :6;
};
union view_scsi_resume_flags {
    unsigned char v;
    struct view_scsi_resume_flags_bits f;
};
extern union view_scsi_resume_flags scsi_resume_flags;
extern unsigned char scsi_unused_3[];
extern char scsi_status;
extern unsigned char scsi_end_state;
extern unsigned char scsi_cmd_state;
extern unsigned char xfer_state_front;
extern unsigned char xfer_state_back;
extern int page_size_wanted;
extern int page_size_ready;
extern int page_size_reply_pending;
extern int page_size_side;
extern unsigned char scsi_read_pixel_size_flag;
extern unsigned short page_size_paper_width_px;
extern unsigned short page_size_paper_length_lines;
extern int page_size_driver_crop;
extern unsigned short page_size_word_24;
extern unsigned short page_size_word_28;
extern char scsi_message_state;
extern struct cell_s32 scsi_param_valid;
extern unsigned char *scsi_win_descriptor_addr;
extern unsigned int scsi_win_descriptor_end;
extern volatile short scsi_field_pointer;
extern unsigned char scsi_win_descriptor_id;
extern int scsi_win_res_valid;
extern unsigned short scsi_win_res_dpi;
extern int scsi_win_inch1200_valid;
extern union scsi_win_value scsi_win_inch1200;
extern int scsi_win_px16_valid;
extern union scsi_win_value scsi_win_px16;
extern unsigned char scsi_unused_4[];
extern unsigned short job_scan_complete_wait;
extern int err_multifeed_reported;
extern unsigned char err_multifeed_code;
extern unsigned short job_buffer_option;
extern char hw_usb_ctrl_dummy[];
extern unsigned char usb_ep0_stage;
extern unsigned char usb_bulk_state;
extern unsigned char usb_bulk_out_state;
extern unsigned char usb_unused_state_1;
extern unsigned char usb_transfer_phase;
struct view_usb_wake_events_bits {
    unsigned char transfer_armed:1,
                  :1,
                  attach_raised:1,
                  vbus_changed:1,
                  non_j:1,
                  :1,
                  dma_started:1,
                  :1;
};
union view_usb_wake_events {
    unsigned char v;
    struct view_usb_wake_events_bits f;
};
extern union view_usb_wake_events usb_wake_events;
struct view_usb_event_flags_bits {
    unsigned char attach:1,
                  detach:1,
                  bus_reset:1,
                  suspend:1,
                  :4;
};
union view_usb_event_flags {
    unsigned char v;
    struct view_usb_event_flags_bits f;
};
extern union view_usb_event_flags usb_event_flags;
extern union UBF usb_main_status;
extern union UBF usb_main_int_read;
/* usb_sie_status: the SIE interrupt bits collected from the controller's register 0x01 (S1R72003 SIEIntStat) */
struct view_usb_sie_status_bits {
    unsigned char vbus_changed:1,
                  non_j:1,
                  bus_reset:1,
                  suspend:1,
                  :1,
                  chirp_done:1,
                  restore_done:1,
                  :1;
};
union view_usb_sie_status {
    unsigned char v;
    struct view_usb_sie_status_bits f;
};
extern union view_usb_sie_status usb_sie_status;
extern union UBF usb_sie_int_read;
extern union UBF usb_ep_int_status;
extern union UBF usb_ep_int_read;
extern union UBF usb_ep0_int_status;
extern union UBF usb_ep0_int_read;
extern union UBF usb_epa_int_status;
extern union UBF usb_epa_int_read;
extern union UBF usb_epb_int_status;
extern union UBF usb_epb_int_read;
extern unsigned char usb_unused_1[];
extern unsigned char usb_device_state;
extern unsigned char usb_unused_2[];
extern char usb_epa_address[];
extern char usb_epb_address[];
extern char usb_epc_address[];
extern struct Bits usb_ctrl_flags;
extern unsigned char *usb_ep0_data_ptr;
extern unsigned short usb_ep0_wlength;
extern unsigned short usb_ep0_done_len;
extern unsigned short usb_ep0_data_len;
extern unsigned char usb_bulk_wait_count;
extern unsigned char usb_ep0_reply_buf[];
extern unsigned char usb_ep0_reply_byte_1[];
extern unsigned char usb_ep0_reply_byte_2[];
extern unsigned char usb_ep0_reply_byte_3[];
extern unsigned char usb_ep0_reply_byte_10[];
extern unsigned char usb_ep0_reply_byte_11[];
extern unsigned char usb_ep0_event;
extern unsigned char usb_epa_event;
extern unsigned char usb_epb_event;
extern unsigned char usb_address;
extern unsigned short usb_ep0_max_packet;
union view_usb_epa_max_packet {
    unsigned short v;
    struct { unsigned char pad_00; unsigned char lo; } c;
};
extern union view_usb_epa_max_packet usb_epa_max_packet;
extern unsigned short usb_epb_max_packet;
extern unsigned char usb_alternate_setting;
extern unsigned char usb_unused_state_2;
extern unsigned char usb_unused_3[];
extern unsigned int usb_bulk_len;
extern unsigned usb_read_chunk_len;
extern unsigned int usb_read_len;
extern unsigned char usb_unused_4[];
extern unsigned char usb_read_byte_unused;
/* usb_transfer_flags: the state of the bulk transfer */
struct view_usb_transfer_flags_bits {
    unsigned char done:1,
                  :5,
                  pending:1,
                  pisces_in:1;
};
extern struct view_usb_transfer_flags_bits usb_transfer_flags;
extern unsigned char usb_unused_5[];
extern unsigned char usb_irq_code;
extern unsigned char *usb_bulk_in_ptr;
extern char *usb_bulk_out_ptr;
extern int usb_bulk_sending;
extern int usb_suspend_entered;
extern int usb_suspended;
extern unsigned short usb_suspend_ticks;
extern unsigned char usb_last_main_int;
extern unsigned char usb_last_sie_int;
extern unsigned char usb_last_ep_int;
extern unsigned char usb_last_ep0_int;
extern unsigned char usb_last_epa_int;
extern unsigned char usb_last_epb_int;
extern union view_maint_mode_word maint_mode_word;
extern unsigned char maint_screen;
extern char maint_sensor_test;
extern struct cell_maint_state maint_state;
extern struct cell_maint_item maint_item;
extern struct Bits maint_save_eeprom_mode;
extern unsigned char maint_adjust_error;
extern short maint_start_flag;
extern unsigned char err_maint_error_count_1;
extern unsigned char err_maint_error_count_2;
extern unsigned char err_maint_error_count_3;
extern unsigned char err_maint_error_count_4;
extern struct cell_maint_pisces_side maint_pisces_side;
extern struct cell_maint_adjust_target maint_adjust_target;
extern struct maint_edge_samples maint_top_edges;
extern struct maint_edge_samples maint_bottom_edges;
extern struct maint_edge_samples maint_left_edges;
extern struct maint_edge_samples maint_left_edges_far;
extern struct cell_maint_white_level_side maint_white_level_side;
extern struct cell_maint_white_level_target maint_white_level_target;
extern struct cell_u16 maint_white_level_result[];
extern unsigned char maint_white_level_result_1[];
extern unsigned char maint_white_level_result_2[];
extern unsigned char maint_saved_value_0[];
extern unsigned char maint_saved_value_1[];
extern unsigned char maint_saved_value_2[];
extern unsigned char maint_saved_value_3[];
extern unsigned char maint_saved_value_4[];
extern unsigned char maint_saved_value_5[];
extern unsigned char maint_saved_value_6[];
extern unsigned char maint_saved_value_7[];
union view_scsi_testmode_reply_len {
    unsigned short v;
    struct { unsigned char pad_00; unsigned char lo; } c;
};
extern union view_scsi_testmode_reply_len scsi_testmode_reply_len;
extern unsigned char err_code_list[];
extern unsigned char testmode_result_1[];
extern unsigned char testmode_result_2[];
extern unsigned char testmode_result_3[];
extern unsigned char testmode_result_4[];
extern unsigned char testmode_result_5[];
extern unsigned char testmode_result_6[];
extern unsigned char testmode_result_7[];
extern unsigned char testmode_result_8[];
extern unsigned char testmode_result_9[];
extern unsigned char testmode_result_10[];
extern unsigned char testmode_result_11[];
extern struct cell_testmode_number testmode_number;
extern struct cell_testmode_param_1 testmode_param_1;
extern char testmode_param_2;
extern struct cell_testmode_adjust_mode testmode_adjust_mode;
extern int testmode_param_absent;
extern int err_ultrasonic_error;
extern int scsi_diag_started;
extern int testmode_feed_test;
extern int mdc_lamp_cover_override;
extern int maint_pick_test;
extern struct cell_err_code_count err_code_count;
extern int main_testmode_started;
extern unsigned char main_unused_1[];
struct cell_job_readtime_pages { unsigned short v; };
extern struct cell_job_readtime_pages job_readtime_pages;
extern union view_job_readtime_ticks job_readtime_ticks;
extern unsigned char mdc_lamps_bits;
extern struct cell_mdc_end_heater_warmup_bits mdc_end_heater_warmup_bits;
extern int tick_hopper_was_empty;
extern union byte_bits err_display_flags;
extern volatile unsigned short tick_wait_count;
extern struct cell_tick_count tick_count;
extern unsigned int calib_warmup_ticks;
extern struct cell_tick_idle_seconds tick_idle_seconds;
struct cell_power_save_seconds { unsigned long v; };
extern struct cell_power_save_seconds power_save_seconds;
extern struct cell_tick_flatbed_lamp_on_ticks tick_flatbed_lamp_on_ticks;
extern struct cell_tick_panel_polls tick_panel_polls;
extern unsigned char tick_unused_1[];
extern unsigned short tick_adf_closed_ticks;
extern struct cell_tick_hopper_paper_ticks tick_hopper_paper_ticks;
extern struct cell_tick_hopper_empty_ticks tick_hopper_empty_ticks;
extern struct cell_tick_document_cover_open_ticks tick_document_cover_open_ticks;
extern unsigned short tick_document_cover_closed_ticks;
struct cell_tick_page_wait_ticks { unsigned short v; };
extern struct cell_tick_page_wait_ticks tick_page_wait_ticks;
extern int tick_document_cover_event;
extern int calib_levels_deferred;
extern int button_error_reset_pending;
extern int button_wake_press;
extern struct cell_tick_count_last_second tick_count_last_second;
extern struct cell_tick_seconds tick_seconds;
extern struct cell_tick_hostif_wait_ticks tick_hostif_wait_ticks;
struct cell_tick_idle_seconds_capped { unsigned short v; };
extern struct cell_tick_idle_seconds_capped tick_idle_seconds_capped;
extern struct cell_tick_capture_wait_ticks tick_capture_wait_ticks;
extern struct cell_tick_motor_test_ticks tick_motor_test_ticks;
extern unsigned short tick_mdc_start_ticks;
struct cell_tick_error_ticks { unsigned short v; };
extern struct cell_tick_error_ticks tick_error_ticks;
extern unsigned char tick_unused_2[];
extern struct cell_tick_readtime_ticks tick_readtime_ticks;
extern unsigned short job_elapsed_seconds;
struct cell_tick_line_event_ticks { unsigned short v; };
extern struct cell_tick_line_event_ticks tick_line_event_ticks;
union view_job_manual_feed_flags { unsigned short w; };
extern union view_job_manual_feed_flags job_manual_feed_flags;
struct cell_tick_manual_feed_seconds { unsigned short v; };
extern struct cell_tick_manual_feed_seconds tick_manual_feed_seconds;
struct cell_tick_lamp_on_ticks { unsigned int v; };
extern struct cell_tick_lamp_on_ticks tick_lamp_on_ticks;
struct cell_tick_awake_ticks { unsigned int v; };
extern struct cell_tick_awake_ticks tick_awake_ticks;
extern unsigned int tick_sheet_gap_ticks;
extern unsigned int tick_relight_ticks;
extern unsigned char button_scan_released;
extern struct cell_power_off_limit_seconds power_off_limit_seconds;
extern struct cell_power_off_warning_seconds power_off_warning_seconds;
struct cell_tick_page_idle_seconds { unsigned int v; };
extern struct cell_tick_page_idle_seconds tick_page_idle_seconds;
extern int power_woken_by_button;
extern int tick_pf6_low_reported;
extern unsigned short tick_pf6_count;
extern unsigned short tick_flatbed_home_ticks;
extern unsigned int tick_calib_age_ticks;
extern unsigned char tick_unused_3[];
union view_uart_timeout_mode {
    unsigned short v;
    struct {
        unsigned short q:13,
                       :1,
                       ack_timeout:1,
                       frame_timeout:1;
    } g;
};
extern union view_uart_timeout_mode uart_timeout_mode;
extern unsigned char uart_unused_2[];
extern struct Bits uart_timeout_flags;
extern unsigned char uart_timeout_retries;
extern unsigned long tick_heater_monitor_ticks;
extern unsigned short tick_heater_monitor;
extern unsigned short tick_heater_sample_count;
extern unsigned short tick_heater_rounds;
extern unsigned short tick_heater_level_sums[];
extern unsigned short tick_heater_level_sum_back[];
extern unsigned short tick_heater_level_history_front[];
extern unsigned short tick_heater_level_history_back[];
extern unsigned short tick_heater_lamp_temperature[];
extern unsigned short tick_lamp_off_ticks;
extern int tick_heater_monitor_changed[];
extern int tick_heater_monitor_changed_back[];
extern unsigned short tick_heater_monitor_history[2][2][5];
extern struct u16_pair tick_heater_monitor_sums[];
extern struct u16_pair tick_heater_monitor_sum_back[];
extern int tick_keep_warm_seen;
extern int calib_lamp_relit;
extern int job_keep_warm_relit;
extern int calib_lamp_cold;
extern unsigned short job_calib_valid;
extern char link_window_front_params[];
extern unsigned char link_window_back_params[];
extern char link_window_mode_params[];
extern unsigned char link_z_params[];
extern unsigned char link_side_back_params[];
extern unsigned char *uart_tx_ptr;
extern short uart_rx_count;
extern union view_uart_frame_len uart_frame_len;
union view_uart_rx_status {
    unsigned short w;
    struct Bits b;
    unsigned char c;
    struct {
        unsigned short hi:8,
                       :4,
                       frame_overflow:1,
                       framing_error:1,
                       overrun:1,
                       parity_error:1;
    } g;
};
extern union view_uart_rx_status uart_rx_status;
extern struct Bits uart_pending_events;
extern unsigned char uart_rx_retries;
extern struct Bits link_start_scan_flag;
extern unsigned char img_link_scan_mode;
extern unsigned char img_pad_1[];
extern unsigned char link_global_param_low[];
extern unsigned char link_global_param_high[];
struct link_flag_code { unsigned char pad_00; unsigned char code; };
union view_link_reply {
    short w;
    unsigned char b;
    struct Bits f;
    struct link_flag_code c;
};
extern union view_link_reply link_reply;
extern unsigned char link_unused_1[];
extern unsigned short job_link_abort_flags;
extern int jpeg_qtables_downloaded;
extern unsigned char jpeg_download_qtables[];
extern unsigned char jpeg_download_qtable_chroma[];
extern struct jpeg_loaded_record jpeg_loaded_state[2];
extern unsigned char jpeg_unused_1[];
extern struct view_jpeg_setup_flags jpeg_setup_flags[];
extern unsigned char jpeg_unused_2[];
extern union UBF scsi_imprinter_print;
extern unsigned char scsi_unused_5[];
extern union view_scsi_imprinter_style scsi_imprinter_style;
extern union UBF scsi_imprinter_flags;
union view_scsi_imprinter_counter {
    unsigned long v;
    struct { unsigned char pad_00[3]; unsigned char lo; } c;
};
extern union view_scsi_imprinter_counter scsi_imprinter_counter;
extern struct cell_scsi_imprinter_counter_step scsi_imprinter_counter_step;
extern unsigned char scsi_imprinter_text[];
extern struct cell_scsi_imprinter_text_len scsi_imprinter_text_len;
extern unsigned char scsi_imprinter_print_len;
extern unsigned char scsi_unused_6[];
extern int panel_maint_p_flag;
extern struct Bits link_unused_flags;
extern unsigned char link_unused_2[];
extern unsigned char console_packet_in[];
extern unsigned char console_packet_in_cmd[];
extern unsigned char console_packet_in_arg_0[];
extern unsigned char console_packet_in_arg_1[];
extern unsigned char console_packet_in_arg_2[];
extern unsigned char console_packet_in_arg_3[];
extern unsigned char console_packet_in_arg_4[];
extern unsigned char console_packet_in_arg_5[];
extern unsigned char console_packet_in_arg_6[];
extern unsigned char console_packet_out[];
extern unsigned char console_packet_out_len[];
extern unsigned char console_packet_out_data_0[];
extern unsigned char console_packet_out_data_1[];
extern unsigned char console_packet_out_data_2[];
extern unsigned char console_packet_out_data_3[];
extern unsigned char console_packet_out_data_4[];
extern unsigned char console_packet_out_data_5[];
extern unsigned char console_packet_out_data_6[];
extern unsigned char console_packet_out_data_7[];
extern unsigned short console_packet_out_index;
extern unsigned short console_packet_in_count;
extern struct cell_console_tx_state console_tx_state;
extern unsigned char console_tx_state_saved;
extern unsigned char console_tx_continue;
extern struct cell_console_line_length console_line_length;
extern struct cell_console_tx_ptr console_tx_ptr;
extern unsigned long console_tx_spare;
extern volatile unsigned long console_tx_segment;
extern unsigned long console_segment;
extern volatile unsigned long console_offset;
extern struct cell_console_dump_end console_dump_end;
extern unsigned char console_store_value;
extern unsigned char console_store_active;
extern unsigned char console_tx_count;
extern unsigned char console_tx_index;
extern unsigned char console_line[];
extern unsigned char console_tx_line[];
extern unsigned char console_tx_line_ascii[];
extern unsigned char console_tx_line_crlf[];
extern int console_text_mode;
extern int console_rx_byte_count;
union view_mdc_busy {
    short w;
    struct {
        unsigned short :7;
        unsigned short power_save:1;
        unsigned short :3;
        unsigned short queued:1,
                       unused:1,
                       eject:1,
                       scan_move:1,
                       feed:1;
    } f;
};
extern union view_mdc_busy mdc_busy;
extern unsigned char mdc_unused_5[];
/* mdc_cmd_posted: globals.c defines the bytes it is, at an odd address; its users see a union with a 16-bit bit-field unit */
#ifndef FW_DEFINITIONS
extern union view_mdc_cmd_posted mdc_cmd_posted;
#else
extern unsigned char mdc_cmd_posted[];
#endif
extern unsigned char mdc_queued_cmd;
extern unsigned char mdc_queued_params[];
extern unsigned char mdc_queued_param_byte_1[];
extern unsigned char mdc_queued_param_byte_2[];
extern unsigned char mdc_queued_param_byte_4[];
extern int mdc_wait_ticks;
extern union view_mdc_motor_test_state mdc_motor_test_state;
extern unsigned char mdc_unused_6[];
struct view_mdc_motor_test_flags_bits {
    unsigned char flatbed_stop_requested:1,
                  maint_started:1,
                  :5,
                  rollers_turned:1;
};
union view_mdc_motor_test_flags {
    unsigned char v;
    struct view_mdc_motor_test_flags_bits f;
};
extern union view_mdc_motor_test_flags mdc_motor_test_flags;
extern unsigned char mdc_unused_7[];
extern volatile union UBF jpeg_scan_flags;
extern struct cell_mdc_motor_test_motor mdc_motor_test_motor;
struct cell_job_dirty_roller_flags { unsigned short v; };
extern struct cell_job_dirty_roller_flags job_dirty_roller_flags;
extern int job_page_end_at_pick_sensor;
extern int mdc_capture_pause_sent;
extern int mdc_capture_resume_repeat;
extern struct line_timing line_page_timing[];
extern unsigned char job_long_page_flags;
extern unsigned char win_white_level_follow;
extern int line_long_page_level_sum;
extern int line_long_page_level_count;
extern unsigned short line_long_page_sample_lines;
extern unsigned short line_long_page_level_index;
extern unsigned char img_long_page_level;
extern unsigned char line_long_page_state;
extern unsigned char line_pad_1[];
extern volatile unsigned char job_separation_sheet_flags;
extern unsigned char job_feed_backing_request;
extern unsigned char job_pad_2[];
extern unsigned short img_offset_table_index;
extern unsigned short main_error_cover_opened;
extern unsigned char main_unused_2[];
extern int line_gap_step_ptr[];
extern int line_gap_step_ptr_back[];
union view_line_gap_table_first { int v; unsigned char *p; };
extern union view_line_gap_table_first line_gap_table_first;
union view_line_gap_table_next { int v; unsigned char *p; };
extern union view_line_gap_table_next line_gap_table_next;
extern char line_gap_steps_left[];
extern char line_gap_steps_left_back[];
extern unsigned char line_gap_count_first;
extern unsigned char line_gap_count_next;
extern unsigned short line_gap_lines_left[];
extern unsigned short line_gap_lines_left_back[];
extern unsigned short line_gap_lines_first;
extern unsigned short line_gap_lines_next;
extern unsigned short line_gap_counter[];
extern unsigned short line_gap_counter_back[];
extern unsigned short line_capture_steps;
extern struct cell_line_capture_wait_count line_capture_wait_count;
extern unsigned char line_unused_3[];
extern unsigned short jpeg_restart_counter;
extern unsigned char jpeg_pad_1[];
extern unsigned short page_poll_count;
extern unsigned short main_multifeed_retry_count;
extern unsigned char scsi_update_request;
union view_flash_status {
    unsigned short v;
    struct { unsigned char pad_00; unsigned char lo; } c;
};
extern union view_flash_status flash_status;
extern struct cell_s8 hw_restart_scsi_syncmode;
extern struct cell_s8 hw_restart_scsi_ultra;
extern int flash_cmd_addrs_set;
extern struct cell_u16 flash_len;
extern unsigned char *volatile flash_dst;
extern unsigned char *flash_src;
extern unsigned char flash_backup_header[];
extern unsigned char flash_backup_header_tail[];
extern unsigned char scsi_diag_update_mode;
extern unsigned short *flash_cmd_addr1;
extern unsigned short *flash_cmd_addr2;
extern unsigned short *flash_cmd_addr3;
extern unsigned short *flash_hi_cmd_addr1;
extern unsigned short *flash_hi_cmd_addr2;
extern unsigned short *flash_hi_cmd_addr3;
extern unsigned short dirt_patched_count;
extern volatile unsigned short dirt_candidate_count;
extern struct cell_dirt_flat_count dirt_flat_count;
extern unsigned short dirt_flat_low_count;
extern unsigned short dirt_run_count;
union view_dirt_dip_limit {
    int v;
    struct { unsigned char pad_00[2]; unsigned short lo; } c;
};
extern union view_dirt_dip_limit dirt_dip_limit;
extern volatile int dirt_flat_limit;
extern volatile int dirt_flat_limit_low;
extern int dirt_defects_present;
extern struct cell_u16 dirt_window_max;
union view_dirt_half_width {
    unsigned short v;
    struct { unsigned char pad_00; unsigned char lo; } c;
};
extern union view_dirt_half_width dirt_half_width;
extern struct cell_dirt_window_ptr dirt_window_ptr;
extern unsigned short dirt_depth_scaled;
extern unsigned short dirt_depth_limit;
extern struct cell_u16 dirt_line_average[];
extern struct cell_u16 dirt_depth_by_port[];
extern struct cell_dirt_dip_limit_by_port dirt_dip_limit_by_port[];
extern struct view_scsi_defect_transfer_bits scsi_defect_transfer;
extern unsigned char scsi_unused_7[];
extern char scsi_param_buf_large[];
extern unsigned short flash_defect_lists[];
extern unsigned char flash_settings_copy[];
extern int main_park_pending;
extern struct scsi_regs *scsi_ctrl;
extern int power_dump_inhibit;
extern int log_seconds;
extern unsigned char log_clock_set;
extern unsigned char log_clock_year;
struct cell_log_clock_month { unsigned char v; };
extern struct cell_log_clock_month log_clock_month;
struct cell_log_clock_day { unsigned char v; };
extern struct cell_log_clock_day log_clock_day;
struct cell_log_clock_hour { unsigned char v; };
extern struct cell_log_clock_hour log_clock_hour;
struct cell_log_clock_min { unsigned char v; };
extern struct cell_log_clock_min log_clock_min;
struct cell_log_clock_sec { unsigned char v; };
extern struct cell_log_clock_sec log_clock_sec;
extern unsigned char log_unused_1[];
extern unsigned char log_start_day;
extern unsigned char log_start_hour;
extern struct log_record log_records[];
extern unsigned short log_record_wrap_index;
extern struct cell_u16 log_record_index;
extern struct cell_u16 log_record_last;
extern struct Bits log_flags;
extern volatile unsigned char log_record_minutes;
extern unsigned char settings[];
extern unsigned char settings_sub_offset_flatbed[];
extern unsigned char settings_main_offset_adf_front[];
extern unsigned char settings_sub_offset_adf_front[];
extern unsigned char settings_main_offset_adf_back[];
extern unsigned char settings_sub_offset_adf_back[];
extern unsigned char settings_mark_flags[];
extern unsigned char settings_sub_magnification_adf[];
extern unsigned char settings_sub_magnification_flatbed[];
extern unsigned char settings_window_mode[];
extern unsigned char settings_cga_scsi_id[];
extern unsigned char settings_emulation[];
extern unsigned char settings_byte_0x17[];
extern unsigned char settings_power_off_enable[];
extern unsigned char settings_byte_0x1d[];
extern unsigned char settings_byte_0x1e[];
extern unsigned char settings_byte_0x23[];
extern unsigned char settings_adf_count_low[];
extern unsigned char settings_flatbed_count_low[];
extern unsigned char settings_adf_count_ones[];
extern unsigned char settings_flatbed_count_ones[];
/* the 16-bit settings counters (stored byte-swapped): one type */
union counter_word { unsigned short w[1]; unsigned char c[2]; };
extern union counter_word settings_flatbed_count;
extern union counter_word settings_adf_count;
extern unsigned char settings_cleaning_count[];
extern unsigned char settings_byte_0x32[];
extern unsigned char settings_byte_0x33[];
extern unsigned char settings_byte_0x34[];
extern unsigned char settings_byte_0x35[];
extern unsigned char settings_mark_result[];
extern char settings_edge_fail_code[];
extern unsigned char settings_byte_0x3a[];
extern unsigned char settings_byte_0x3b[];
extern union counter_word settings_pick_roller_count;
extern union counter_word settings_brake_roller_count;
extern unsigned char settings_byte_0x40[];
extern unsigned char settings_byte_0x48[];
extern unsigned char settings_byte_0x49[];
extern unsigned char settings_uart_baud[];
extern unsigned char settings_calib_options[];
extern unsigned char settings_test_flags[];
extern unsigned char settings_test_pattern[];
extern unsigned char settings_byte_0x53[];
extern unsigned char settings_byte_0x54[];
extern unsigned char settings_byte_0x55[];
extern unsigned char settings_power_off_time[];
extern unsigned char settings_power_off_warning[];
extern unsigned char settings_byte_0x58[];
extern unsigned char settings_first_read_year[];
extern unsigned char settings_first_read_month[];
extern unsigned char settings_first_read_day[];
extern unsigned char settings_device_id_1[];
extern unsigned char settings_device_id_2[];
extern unsigned char settings_device_id_3[];
extern unsigned char settings_byte_0x70[];
extern unsigned char settings_overscan_control[];
extern unsigned char settings_log_source[];
extern unsigned char settings_byte_0x7e[];
extern unsigned char settings_byte_0x7f[];
extern unsigned char settings_block_0x80[];
extern unsigned char settings_df_sensor_range[];
extern unsigned char settings_df_sensor_level[];
extern unsigned char settings_ultrasonic_threshold[];
extern unsigned char settings_sheet_gap_extension[];
extern unsigned char settings_byte_0x9a[];
extern unsigned char settings_non_rectangular[];
extern unsigned char settings_power_switch[];
extern struct Bits settings_setup_flags;
extern unsigned char settings_autocrop_boundary[];
extern unsigned char settings_cleaning_cycle[];
extern unsigned char settings_pick_roller_alarm[];
extern unsigned char settings_brake_roller_alarm[];
extern unsigned char settings_byte_0xca[];
extern unsigned char settings_intelligent_multifeed[];
extern unsigned char settings_byte_0xcd[];
extern unsigned char settings_manual_feed_timeout[];
union view_settings_multifeed_check {
    unsigned char c;
    struct { unsigned char p:6, lo:2; } f;
};
extern union view_settings_multifeed_check settings_multifeed_check;
extern unsigned char settings_long_page_enable[];
extern unsigned char settings_scan_options[];
extern unsigned char settings_dropout_codes[];
extern unsigned char settings_filler_flatbed_top_bottom[];
extern unsigned char settings_filler_flatbed_right_left[];
extern unsigned char settings_filler_adf_top_bottom[];
extern unsigned char settings_filler_adf_right_left[];
extern unsigned char settings_user_offset_adf_front[];
extern unsigned char settings_user_offset_adf_back[];
extern unsigned char settings_user_offset_flatbed[];
extern unsigned char settings_byte_0xdf[];
extern unsigned char settings_threshold_flatbed_default[];
extern unsigned char settings_threshold_flatbed_channel_1[];
extern unsigned char settings_threshold_flatbed_channel_2[];
extern unsigned char settings_threshold_flatbed_no_dropout[];
extern unsigned char settings_threshold_adf_front_default[];
extern unsigned char settings_threshold_adf_front_channel_1[];
extern unsigned char settings_threshold_adf_front_channel_2[];
extern unsigned char settings_threshold_adf_front_no_dropout[];
extern unsigned char settings_threshold_adf_back_default[];
extern unsigned char settings_threshold_adf_back_channel_1[];
extern unsigned char settings_threshold_adf_back_channel_2[];
extern unsigned char settings_threshold_adf_back_no_dropout[];
extern unsigned char settings_white_level_flatbed_chip0[];
extern unsigned char settings_white_level_flatbed_chip1[];
extern unsigned char settings_white_level_flatbed_chip2[];
extern unsigned char settings_white_level_adf_front_chip0[];
extern unsigned char settings_white_level_adf_front_chip1[];
extern unsigned char settings_white_level_adf_front_chip2[];
extern unsigned char settings_white_level_adf_back_chip0[];
extern unsigned char settings_white_level_adf_back_chip1[];
extern unsigned char settings_white_level_adf_back_chip2[];
extern unsigned char settings_white_level_high_flatbed_chip0[];
extern unsigned char settings_white_level_high_flatbed_chip1[];
extern unsigned char settings_white_level_high_flatbed_chip2[];
extern unsigned char settings_white_level_high_adf_front_chip0[];
extern unsigned char settings_white_level_high_adf_front_chip1[];
extern unsigned char settings_white_level_high_adf_front_chip2[];
extern unsigned char settings_white_level_high_adf_back_chip0[];
extern unsigned char settings_white_level_high_adf_back_chip1[];
extern unsigned char settings_white_level_high_adf_back_chip2[];
extern unsigned char settings_upper_half[];
extern unsigned char settings_tone_x_0[];
extern unsigned char settings_tone_x_1[];
extern unsigned char settings_tone_x_2[];
extern unsigned char settings_tone_front_0[];
extern unsigned char settings_tone_front_1[];
extern unsigned char settings_tone_front_2[];
extern unsigned char settings_tone_back_0[];
extern unsigned char settings_tone_back_1[];
extern unsigned char settings_tone_back_2[];
extern unsigned char settings_lamp_timer_default[];
extern unsigned char settings_sleep_timer_default[];
extern unsigned char settings_mode_page_defaults[];
extern unsigned char settings_scsi_fault_count[];
extern unsigned char settings_error_history_index[];
extern char settings_block_0x180[];
extern unsigned char settings_error_count[];
extern unsigned char settings_byte_0x18f[];
extern unsigned char settings_gain_target_front[];
extern unsigned char settings_gain_target_back[];
extern unsigned char settings_offset_target_front[];
extern unsigned char settings_offset_target_back[];
extern unsigned char settings_fixed_black_front[];
extern unsigned char settings_fixed_black_back[];
extern unsigned char settings_gain_flatbed_chip0[];
extern unsigned char settings_gain_flatbed_chip1[];
extern unsigned char settings_gain_flatbed_chip2[];
extern unsigned char settings_gain_adf_front_chip0[];
extern unsigned char settings_gain_adf_front_chip1[];
extern unsigned char settings_gain_adf_front_chip2[];
extern unsigned char settings_gain_adf_back_chip0[];
extern unsigned char settings_gain_adf_back_chip1[];
extern unsigned char settings_gain_adf_back_chip2[];
extern unsigned char settings_black_margin_flatbed_chip0[];
extern unsigned char settings_black_margin_flatbed_chip1[];
extern unsigned char settings_black_margin_flatbed_chip2[];
extern unsigned char settings_black_margin_adf_front_chip0[];
extern unsigned char settings_black_margin_adf_front_chip1[];
extern unsigned char settings_black_margin_adf_front_chip2[];
extern unsigned char settings_black_margin_adf_back_chip0[];
extern unsigned char settings_black_margin_adf_back_chip1[];
extern unsigned char settings_black_margin_adf_back_chip2[];
extern unsigned char settings_link_text[];
extern unsigned char settings_saved[];
extern unsigned char settings_saved_test_flags[];
extern unsigned char iic_eeprom_area2[];
extern unsigned char iic_eeprom_area2_byte_1[];
extern unsigned char iic_eeprom_area2_byte_2[];
extern unsigned char iic_eeprom_area2_saved[];
extern char iic_eeprom_area2_saved_byte_1[];
extern unsigned char img_tone_x_built[];
extern unsigned char img_tone_front_built[];
extern unsigned char img_tone_back_built[];
extern unsigned char img_tone_curves[][3][1024];
/* hw_hostif_flags and its saved copy hw_cga_hostif_flags: which host interface drives the scan */
struct view_hw_hostif_flags_bits {
    unsigned char owner_decided:1,
                  :3,
                  selected:1,
                  cga_test_scan:1,
                  own_hostif:1,
                  usb:1;
};
union view_hw_hostif_flags {
    volatile unsigned char v;
    volatile struct view_hw_hostif_flags_bits f;
};
extern union view_hw_hostif_flags hw_hostif_flags;
union view_hw_cga_hostif_flags {
    unsigned char v;
    struct view_hw_hostif_flags_bits f;
};
extern union view_hw_cga_hostif_flags hw_cga_hostif_flags;
extern union UBF hw_option_flags;
extern unsigned char hw_scsi_own_id;
extern volatile char scsi_own_id_bit;
extern union hw_device_error_flags hw_device_errors;
extern struct cell_u16 iic_eeprom_cell;
extern int iic_eeprom_error;
extern int hw_after_update;
extern int selftest_mdc_in_boot;
union view_main_adf_sheets_pending {
    unsigned short v;
    struct { unsigned short v; } s;
};
extern union view_main_adf_sheets_pending main_adf_sheets_pending;
struct cell_main_flatbed_sheets_pending { unsigned short v; };
extern struct cell_main_flatbed_sheets_pending main_flatbed_sheets_pending;
extern unsigned char main_unused_3[];
extern struct Bits main_settings_dirty;
union view_job_adf_sheet_ones {
    unsigned short v;
    struct { unsigned char pad_00; unsigned char lo; } c;
};
extern union view_job_adf_sheet_ones job_adf_sheet_ones;
extern unsigned char job_unused_1[];
union view_job_flatbed_sheet_ones {
    unsigned short v;
    struct { unsigned char pad_00; unsigned char lo; } c;
};
extern volatile union view_job_flatbed_sheet_ones job_flatbed_sheet_ones;
extern unsigned char job_unused_2[];
extern struct cell_job_adf_sheets_mod_10000 job_adf_sheets_mod_10000;
extern unsigned int img_sdram_bytes[];
struct view_hw_restart_flags_bits {
    unsigned char host_reset:1,
                  leaving_power_save:1,
                  unused:1,
                  usb_restart:1,
                  :4;
};
union view_hw_restart_flags {
    signed char v;
    struct view_hw_restart_flags_bits f;
};
extern union view_hw_restart_flags hw_restart_flags;
extern struct Bits main_startup_flags;
struct view_hw_cga_board_present_bits {
    unsigned char present:1,
                  link_scan_started:1,
                  link_identified:1,
                  :5;
};
union view_hw_cga_board_present {
    unsigned char v;
    struct view_hw_cga_board_present_bits f;
    signed char c[1];
};
extern union view_hw_cga_board_present hw_cga_board_present;
extern unsigned char hw_cga_board_type;
extern struct cell_uart_baud_index uart_baud_index;
extern unsigned char uart_baud_index_new;
extern unsigned char job_pick_error_count;
extern volatile unsigned short main_cleaning_sheets;
extern unsigned char power_mdc_restart_request;
extern unsigned char power_mdc_timeout_count;
union view_power_mdc_restart_state {
    unsigned char v;
    struct {
        unsigned char :1;
        unsigned char timeout_log_copied:1;
        unsigned char log_copied:1;
        unsigned char restart_running:1;
        unsigned char timeout_error:1;
        unsigned char retry_allowed:1;
        unsigned char retry_state:2;
    } f;
};
extern union view_power_mdc_restart_state power_mdc_restart_state;
extern int power_off_idle_seconds;
extern int hw_board_variant;
extern int usb_restart_pending;
extern struct usb_regs *usb_ctrl;
extern unsigned char power_dump_buf[];
extern unsigned char power_dump_year[];
extern unsigned char power_dump_month[];
extern unsigned char power_dump_day[];
extern unsigned char power_dump_revision[];
extern unsigned char power_dump_mdc_version_0[];
extern unsigned char power_dump_mdc_version_1[];
extern unsigned char power_dump_mdc_version_2[];
extern unsigned char power_dump_mdc_version_3[];
extern unsigned char power_dump_reason;
extern unsigned char power_dump_hour;
extern unsigned char power_dump_minute;
extern unsigned char power_dump_second;
extern unsigned char power_dump_log_events[];
extern struct log_event log_events[];
extern int log_event_index;
extern int power_pf1_sequence_seconds;
extern unsigned power_pf1_sequence_step;
extern unsigned char power_unused_1[];
extern unsigned char power_dump_settings[];
extern unsigned char power_dump_mdc_restart_state;
extern unsigned short job_trace_index;
extern unsigned long job_trace_ring[];
extern unsigned char power_dump_line_side_state[];
extern unsigned short power_dump_line_motion_flags;
extern unsigned char power_dump_unused_1[];
extern int power_dump_page_buf_count;
extern int power_dump_page_scanning;
extern int power_dump_page_current;
extern int power_dump_page_scanned_count;
extern int power_dump_page_sent_count;
extern unsigned char power_dump_unused_2[];
extern unsigned char power_dump_page_scanning_record[];
extern unsigned char power_dump_page_current_record[];
extern unsigned char power_dump_last_page_size[];
extern unsigned char power_dump_page_side_front[];
extern unsigned char power_dump_page_side_back[];
extern unsigned char power_dump_capture_record_front[];
extern unsigned char power_dump_capture_record_back[];
extern unsigned char power_dump_mdc_cmd_log[];
extern unsigned char power_dump_mdc_cmd_results[];
extern int power_dump_mdc_cmd_log_index;
extern unsigned char power_dump_unused_3[];
extern unsigned char power_dump_mailbox[];
extern unsigned char power_dump_page_size_wanted[];
extern unsigned char power_dump_scsi_status;
extern unsigned char power_dump_scsi_end_state;
extern unsigned char power_dump_xfer_state_front;
extern unsigned char power_dump_xfer_state_back;
extern int power_dump_scsi_param_len;
extern int power_dump_scsi_xfer_len;
extern int power_dump_sense_information;
extern int power_dump_usb_bulk_len;
extern char power_dump_usb_bank_0[];
extern char power_dump_usb_bank_1[];
extern char power_dump_usb_bank_2[];
extern unsigned char usb_error_registers[];
extern unsigned char power_dump_usb_ep0_stage;
extern unsigned char power_dump_usb_bulk_state;
extern unsigned char power_dump_usb_bulk_out_state;
extern unsigned char power_dump_usb_unused_state_1;
extern unsigned char power_dump_usb_transfer_phase;
extern unsigned char power_dump_usb_unused_state_2;
extern unsigned char power_dump_usb_ep0_event;
extern unsigned char power_dump_usb_epa_event;
extern unsigned char power_dump_usb_epb_event;
extern unsigned char power_dump_usb_ep0_max_packet;
extern unsigned short power_dump_usb_epa_max_packet;
extern unsigned short power_dump_usb_epb_max_packet;
extern unsigned char usb_error_unused_1[];
extern unsigned char usb_error_ep0_stage;
extern unsigned char usb_error_bulk_state;
extern unsigned char usb_error_bulk_out_state;
extern unsigned char usb_error_unused_state_1;
extern unsigned char usb_error_transfer_phase;
extern unsigned char usb_error_unused_state_2;
extern unsigned char usb_error_ep0_event;
extern unsigned char usb_error_epa_event;
extern unsigned char usb_error_epb_event;
extern unsigned char usb_error_ep0_max_packet;
extern unsigned short usb_error_epa_max_packet;
extern unsigned short usb_error_epb_max_packet;
extern int usb_error_code;
extern unsigned char usb_error_main_int;
extern unsigned char usb_error_sie_int;
extern unsigned char usb_error_ep_int;
extern unsigned char usb_error_ep0_int;
extern unsigned char usb_error_epa_int;
extern unsigned char usb_error_epb_int;
extern unsigned char usb_error_unused_2[];
extern unsigned char power_dump_pisces_front[];
extern unsigned char power_dump_pisces_back[];
extern unsigned int power_dump_marker;
extern unsigned char power_mdc_dump_0[];
extern unsigned char power_mdc_dump_1[];

/* I/O (on-chip registers) */
extern struct win_param_block win_params;
extern unsigned char win_unused_1[];
extern union view_win_sides win_sides;
extern struct line_side_record line_side_state[2];
extern volatile unsigned short line_irq_count;
extern unsigned char line_unused_1[];
extern volatile unsigned short line_motion_stop_lines[];
extern volatile unsigned short line_motion_stop_lines_back[];
extern unsigned short line_motion_flags;
extern unsigned char line_unused_2[];
extern struct line_page_side line_page_state[2];
extern volatile struct Bits job_auto_size_flags;
extern struct img_crop_rect img_crop_area[];
extern unsigned char img_crop_area_back[];
extern struct cell_line_captured_lines line_captured_lines[];
extern struct edge_record_set edge_side_records[];
extern volatile unsigned short edge_top_lines_front[][22];
extern volatile unsigned short edge_event_20_lines_front[][22];
extern volatile struct edge_side_word edge_top_lines_back[];
extern volatile struct edge_side_word edge_event_20_lines_back[];
extern struct edge_fit_record edge_fit_params[2];
extern unsigned short edge_samples[];
extern unsigned short edge_sample_values[];
extern unsigned short edge_sample_pos[];
extern unsigned short edge_samples_taken[];
extern unsigned short edge_fit_state[];
extern struct edge_sum_record edge_side_line_sums[];
extern unsigned short edge_substate[];

#endif
