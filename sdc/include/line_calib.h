/* line_calib.h: declarations of line_calib.c.
   */
#ifndef LINE_CALIB_H
#define LINE_CALIB_H
#include "common.h"

/* calib_mode: the calibration in progress (switch in calib_run_step: the step table) */
#define CALIB_MODE_APPLY       0
#define CALIB_MODE_BLACK       1
#define CALIB_MODE_OFFSET      2
#define CALIB_MODE_GAIN        3
#define CALIB_MODE_LAMP_CHECK  9
#define CALIB_MODE_LAMP_ADJUST 10
/* calib_step_result: what a calibration step reports to calib_run_step */
#define CALIB_STEP_STAY 0
#define CALIB_STEP_NEXT 1
#define CALIB_STEP_DONE 0xFF
/* vdcc_correction_regs.mode_chip0/_chip1/_chip2: the VDCC's shading correction mode */
#define CALIB_CORRECTION_RAW        0x30
#define CALIB_CORRECTION_LOAD_WHITE 0x90
#define CALIB_CORRECTION_ON         0xC0

struct vdcc_reg_06 {
    char pad_00[6];
    unsigned char :4,
                  b3:1,
                  b2:1,
                  b1:1,
                  :1;
};

/* functions */
extern void line_handle_irq(void);
extern void line_handle_side(int side);
extern void line_handle_page_events(int side);
extern void line_end_side(int side);
extern void line_start_capture(int side);
extern void line_stop_capture(int side);
extern void line_set_line_gate(int unused_side, int on);
extern void line_enable_capture(int side);
extern void line_disable_capture(int side);
extern void line_run_mdc_requests(void);
extern void line_send_feed(void);
extern void line_send_scan_move(void);
extern void line_count_mdc_waits(void);
extern void line_restart_capture(int side);
extern void calib_handle_line(int side);
extern char calib_run_sequence(void);
extern int calib_load_white_level();
extern int calib_check_lamp_exposure(void);
extern void calib_exposure_history_add(int restart);
extern int calib_exposure_extrapolate(void);
extern void calib_run_step(void);
extern void calib_next_step(void);
extern void calib_offset_step_0(void);
extern void calib_offset_step_1(void);
extern void calib_offset_step_2(void);
extern void calib_offset_step_3(void);
extern void calib_offset_step_4(void);
extern void calib_offset_step_5(void);
extern void calib_offset_step_6(void);
extern void calib_gain_step_0(void);
extern void calib_gain_step_1(void);
extern void calib_gain_step_2(void);
extern void calib_gain_step_3(void);
extern void calib_gain_step_4(void);
extern void calib_gain_step_5(void);
extern void calib_gain_step_6(void);
extern void calib_black_step_1(void);
extern void calib_black_step_2(void);
extern void calib_black_step_3(void);
extern void calib_apply_step_0(void);
extern void calib_apply_step_1(void);
extern void calib_apply_step_2(void);
extern void calib_apply_step_3(void);
extern void calib_apply_step_4(void);
extern void calib_lamp_check_step_1(void);
extern void calib_lamp_check_step_2(void);
extern void calib_lamp_adjust_step_1(void);
extern void calib_lamp_adjust_step_2(void);
extern void calib_lamp_adjust_step_3(void);
extern void calib_lamp_adjust_step_4(void);
extern void calib_lamp_adjust_step_5(void);
extern void calib_lamp_adjust_step_6(void);
extern void calib_lamp_adjust_step_7(void);
extern void calib_lamp_11_step_1(void);
extern void calib_lamp_11_step_2(void);
extern void calib_mode_12_step_1(void);
extern void calib_mode_12_step_2(void);
extern void calib_mode_13_step_1(void);
extern void calib_mode_13_step_2(void);
extern int calib_wait_done(void);
extern void calib_write_afe_offsets(unsigned short);
extern void calib_write_afe_gains(unsigned short);
extern int line_count_down_gap(int side);
extern void line_run_page_steps(void);
extern void line_run_page_step(int side);
extern void line_read_captured_lines(int side);
extern void line_run_page_step_part_4(int side);
extern void line_run_long_page_step(int side, unsigned short lines);

/* functions */
extern void calib_load_white_long_page(void);
extern char calib_start_offset(unsigned char);
extern unsigned char calib_start_gain(unsigned char);
extern void calib_start_hook(int unused_arg);
extern void calib_start_apply(unsigned char);
extern void calib_start_lamp_check(unsigned char);
extern void calib_start_lamp_adjust(char);
extern void calib_start_lamp_11(char, int);
extern int calib_start_black(void);
extern void calib_hook(char unused_arg);
extern void calib_record_mark_sample(void);
extern void line_end_page(int);

/* constants */
extern void (*const calib_steps_offset[7]) (void);
extern void (*const calib_steps_gain[7]) (void);
extern void (*const calib_steps_black[4]) (void);
extern void (*const calib_steps_apply[5]) (void);
extern void (*const calib_steps_lamp_check[3]) (void);
extern void (*const calib_steps_lamp_adjust[8]) (void);
extern void (*const calib_steps_lamp_11[3]) (void);
extern void (*const calib_steps_mode_12[3]) (void);
extern void (*const calib_steps_mode_13[3]) (void);

/* constants */
extern const unsigned char calib_white_factor_pairs[129][2];
extern const unsigned char calib_white_factor_single[65][2];

/* variables */
extern int line_irq_imask;
extern unsigned short line_back_events_pending;
extern unsigned short line_back_delay_lines;
extern unsigned short line_back_count_20;
extern unsigned short line_back_count_100;
extern unsigned short line_front_length_lines;
extern unsigned long calib_follow_start_lines;
extern int line_page_step_imask;

struct vdcc_monitor_pair { short pad_00; short b; };

#endif
