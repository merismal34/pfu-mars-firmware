/* job.h: declarations of job.c, job_calibrate.c, job_page_start.c, job_end.c, job_settings.c,
   job_pipeline.c and 2 more files. */
#ifndef JOB_H
#define JOB_H
#include "common.h"

/* functions */
extern int job_run(void);
extern int job_run_next_page(void);
extern int job_check_source(void);
extern int job_check_feed_start(void);
extern int job_mark_sheet_fed(int from_capture_buf);
extern int job_report_page_to_link(void);
extern int job_wait_mdc_ready(void);
extern int job_wait_side_requested(void);
extern int job_wait_load_pending(void);
extern int job_switch_off_idle_heaters(void);
extern void job_check_warmup_idle(void);
extern int job_check_lamp(int from_lamp_time);
extern int job_run_calibration(void);
extern int job_calibrate(void);
extern int job_calibrate_levels();
extern void job_calibrate_levels_step_8();
extern int job_setup_sensor(void);
extern int job_start_page(void);
extern int job_wait_side_complete(int side);
extern int job_wait_motion_end(void);
extern int job_wait_page_event_80(int side);
extern int job_check_mdc_error(void);
extern unsigned char job_get_mdc_error(void);
extern int job_wait_link_page_lines(void);
extern int job_clear_page_record(void);
extern int job_wait_mdc_page_end(int unused_arg);
extern int job_end(void);
extern int job_hook_page_flag(void);
extern int job_abort(void);
extern void job_set_driver_function_confirm(unsigned char value, int record);
extern void job_set_prepick();
extern void job_set_sleep_timer(unsigned char value, int record);
extern void job_set_interlace_read(unsigned char value, int record);
extern void job_set_backing(unsigned char value, int record);
extern void job_set_double_feed(unsigned char value, int record);
extern void job_set_buffer_mode(unsigned char mode, unsigned char clear, short page_limit, int record);
extern void job_set_deskew_and_auto_width(unsigned char deskew, unsigned char auto_width, int record);
extern void job_set_auto_length(unsigned char value, int record);
extern void job_set_overscan(unsigned char value, int record);
extern void job_set_dropout_color(unsigned char, int);
extern void job_set_dropout_level(unsigned char level, unsigned short index, int record);
extern void job_set_device_controls(unsigned char value, int record);
extern void job_set_auto_power_off(char on);
extern void job_compute_power_off_limit(void);
extern void job_set_power_off_time(char off_time, char warning);
extern int job_check_date(unsigned char, unsigned char, unsigned char);
extern void job_apply_settings(void);
extern int job_prepare_pipeline(void);
extern int job_get_last_error(void);
extern int job_hook(void);
extern void job_prepare_side(int start);
extern void job_prepare_side_step_1(int side);
extern void job_apply_vdcc_debug_settings(void);
extern void job_load_vdcc_test_table(int side);
extern void job_wait_mdc_lines(void);
extern void job_wait_line_count(void);
extern char job_calibrate_levels_step_15(void);
extern void job_check_mdc_handshake_step_8(void);
extern int job_check_mdc_handshake(void);
extern int job_check_adf_page_start(void);
extern int job_check_lamp_at_page(int black);
extern void job_trace_add(unsigned long code);

#endif
