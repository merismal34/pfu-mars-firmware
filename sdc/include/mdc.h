/* mdc.h: declarations of mdc_command.c, mdc_feed.c, mdc_run.c, mdc_request.c, mdc_paper_path.c.
   */
#ifndef MDC_H
#define MDC_H
#include "common.h"

/* MDC command codes (mdc_mailbox.cmd_code; the MDC's cmd_dispatch / job_run; some are never sent by the SDC) */
#define MDC_CMD_READ_MEMORY            0x06
#define MDC_CMD_WRITE_MEMORY           0x07
#define MDC_CMD_BW_MOTOR_CYCLE         0x10
#define MDC_CMD_LAMPS                  0x11
#define MDC_CMD_MOTOR_TEST             0x12
#define MDC_CMD_ADJUST_DF_SENSOR       0x14
#define MDC_CMD_PICK_SHEET             0x16
#define MDC_CMD_ADJUST_ULTRASONIC      0x17
#define MDC_CMD_NO_OPERATION           0x18
#define MDC_CMD_IMPRINTER_INK             0x19
#define MDC_CMD_BACKING_TEST           0x1B
#define MDC_CMD_DF_TEST_ON             0x1C
#define MDC_CMD_DF_TEST_OFF            0x1D
#define MDC_CMD_ULTRASONIC_MONITOR_ON  0x1E
#define MDC_CMD_ULTRASONIC_MONITOR_OFF 0x1F
#define MDC_CMD_POWER_SAVE             0x30
#define MDC_CMD_END_HEATER_WARMUP      0x38
#define MDC_CMD_UNIT_POWER             0x39
#define MDC_CMD_WARMUP_ON              0x40
#define MDC_CMD_WARMUP_OFF             0x41
#define MDC_CMD_CLEAR_ERRORS           0x48
#define MDC_CMD_EJECT_ALIAS            0x60
#define MDC_CMD_BACKING_WHITE          0x62
#define MDC_CMD_BACKING_BLACK          0x63
#define MDC_CMD_BACKING_HOME           0x64
#define MDC_CMD_BACKING_REVERSE        0x65
#define MDC_CMD_FB_MOTOR_HOME          0x66
#define MDC_CMD_PICK_ARM_MOVE          0x68
#define MDC_CMD_PICK_ARM_HOME          0x69
#define MDC_CMD_CLEAR_OVERLAP_PATTERNS 0x6E
#define MDC_CMD_FEED_RUN_TEST          0x70
#define MDC_CMD_BW_MOTOR_RUN_TEST      0x71
#define MDC_CMD_FEED_FLATBED_RUN_TEST  0x72
#define MDC_CMD_READ_OVERLAP_PATTERNS  0x75
#define MDC_CMD_WRITE_OVERLAP_PATTERNS 0x76
#define MDC_CMD_DUMP_LOG               0x79
#define MDC_CMD_FEED                   0x81
#define MDC_CMD_SCAN_MOVE              0x82
#define MDC_CMD_EJECT                  0x90
#define MDC_CMD_DOWNLOAD_START         0xC0
#define MDC_CMD_DOWNLOAD_END           0xC1
/* mdc_mailbox.sensor_status bits (the MDC's sensor flags) */
#define MDC_SENSOR_FB_HOME             0x01
#define MDC_SENSOR_DOCUMENT_COVER_OPEN 0x02
#define MDC_SENSOR_ADF_COVER_OPEN      0x04
#define MDC_SENSOR_NO_PAPER            0x08
#define MDC_SENSOR_TOP                 0x10
#define MDC_SENSOR_BACKING             0x20
#define MDC_SENSOR_PICK                0x40
#define MDC_SENSOR_PICK_ARM            0x80
/* mdc_mailbox.lamp_status bits (the MDC's lamp status; _ADF_BOTH / _ALL: SDC-side combinations) */
#define MDC_LAMP_STATUS_ADF_FRONT 0x80
#define MDC_LAMP_STATUS_ADF_BACK  0x40
#define MDC_LAMP_STATUS_FLATBED   0x20
#define MDC_LAMP_STATUS_KEEP_WARM 0x02
#define MDC_LAMP_STATUS_ADF_BOTH  0xC0
#define MDC_LAMP_STATUS_ALL       0xE0
/* mdc_mailbox.capture_flags bits (the MDC's capture pause) */
#define MDC_CAPTURE_PAUSED      0x01
#define MDC_CAPTURE_STEPS_VALID 0x02
/* mdc_mailbox.page_end bits (stop at the sheet end: this side requests, the MDC answers) */
#define MDC_PAGE_END_REQUEST  0x01
#define MDC_PAGE_END_DONE     0x02
#define MDC_PAGE_END_ACCEPTED 0x04
/* mdc_mailbox.separation_sheet_flags, .warmup_output, .df_test_result bits */
#define MDC_SEPARATION_SHEET_DETECTED 0x80
#define MDC_WARMUP_OUTPUT_ON          0x01
#define MDC_DF_TEST_RESULT_TOP_EDGE   0x80
/* mdc_mailbox.cover_open_event values (_ADF_2: bit 0 too, never set by the MDC) */
#define MDC_COVER_OPEN_EVENT_ADF   0x80
#define MDC_COVER_OPEN_EVENT_ADF_2 0x81
/* mdc_mailbox.cmd_end: ends the running continuous command (the MDC clears it) */
#define MDC_CMD_END 0x80
/* mdc_mailbox.long_page: long-page mode (no jam / length errors on the MDC) */
#define MDC_LONG_PAGE_ON 0x80
/* mdc_mailbox.df_adjust_flags bits (the MDC's adjustment results) */
#define MDC_DF_ADJUST_ULTRASONIC_OK 0x01
#define MDC_DF_ADJUST_DF_OK         0x02
/* mdc_mailbox.feed_options bits (mdc_set_feed_params) */
#define MDC_FEED_PREPICK               0x80
#define MDC_FEED_MULTIFEED_REPORT_ONLY 0x40
#define MDC_FEED_ULTRASONIC_CHECK      0x10
#define MDC_FEED_LENGTH_CHECK          0x08
#define MDC_FEED_JOB_SEPARATION        0x04
#define MDC_FEED_LENGTH_TOLERANCE      0x03
#define MDC_FEED_LENGTH_TOLERANCE_10MM 0x01
#define MDC_FEED_LENGTH_TOLERANCE_15MM 0x02
#define MDC_FEED_LENGTH_TOLERANCE_20MM 0x03
/* command MDC_CMD_MOTOR_TEST, parameter byte 0: the motor (MDC_MOTOR_TEST_NONE: testmode_26's "no motor");
   MDC_MOTOR_TEST_SPEED_CLASS: parameter byte 2, the speed class of the fixed-speed callers */
#define MDC_MOTOR_TEST_FEED          0
#define MDC_MOTOR_TEST_PICK          1
#define MDC_MOTOR_TEST_PICK_ARM      2
#define MDC_MOTOR_TEST_BACKING       3
#define MDC_MOTOR_TEST_FB_MOTOR      4
#define MDC_MOTOR_TEST_PICK_AND_FEED 0x10
#define MDC_MOTOR_TEST_SPEED_CLASS   5
#define MDC_MOTOR_TEST_NONE          0xFF
/* mdc_mailbox.multifeed_flags / page_record.multifeed_flags bits */
#define MDC_MULTIFEED_ULTRASONIC 0x04
#define MDC_MULTIFEED_LENGTH     0x08
/* mdc_mailbox.backing_position */
#define MDC_BACKING_BLACK     0
#define MDC_BACKING_WHITE     1
#define MDC_BACKING_AT_SENSOR 2
#define MDC_BACKING_MOVING    3
/* mdc_mailbox.cmd_state: the command handshake */
#define MDC_CMD_STATE_IDLE    0
#define MDC_CMD_STATE_POSTED  2
#define MDC_CMD_STATE_RUNNING 4
#define MDC_CMD_STATE_ENDED   6
#define MDC_CMD_STATE_DONE    8
/* mdc_mailbox.cmd_result and its posted copy's last_result: the command's result (TIMEOUT and PENDING are the SDC's own) */
#define MDC_RESULT_OK          0
#define MDC_RESULT_ERROR       2
#define MDC_RESULT_SHEET_READY 0x10
#define MDC_RESULT_FRONT_START 0x12
#define MDC_RESULT_BACK_START  0x14
#define MDC_RESULT_TIMEOUT     0xF0
#define MDC_RESULT_PENDING     0xFF
/* mdc_mailbox.source: the scan source */
#define MDC_SOURCE_FLATBED 0x20
#define MDC_SOURCE_ADF     0x40
/* mdc_mailbox.motion_flags bits */
#define MDC_MOTION_FRONT        0x01
#define MDC_MOTION_BACK         0x02
#define MDC_MOTION_AT_TOP       0x04
#define MDC_MOTION_BACK_PENDING 0x08
#define MDC_MOTION_SIDES        0x03
/* mdc_mailbox.request: the immediate requests (mdc_send_request; the MDC's isr_tgi2a) */
#define MDC_REQUEST_REINIT_FAST      0x31
#define MDC_REQUEST_REINIT_WAKE      0x32
#define MDC_REQUEST_RESTART          0x40
#define MDC_REQUEST_CAPTURE_PAUSE    0x44
#define MDC_REQUEST_CAPTURE_RESUME   0x45
#define MDC_REQUEST_MOTORS_OFF       0x50
#define MDC_REQUEST_STOP             0x60
#define MDC_REQUEST_ABORT            0x61
#define MDC_REQUEST_LOG_STATE        0x62
#define MDC_REQUEST_HEATER_FRONT_OFF 0x71
#define MDC_REQUEST_HEATER_BACK_OFF  0x72
#define MDC_REQUEST_HEATERS_OFF      0x73
/* mdc_mailbox.startup_request / startup_state (selftest_run_power_on) */
#define MDC_STARTUP_POWER_ON          0
#define MDC_STARTUP_INIT_MECHANICS    2
#define MDC_STARTUP_RUN_4             4
#define MDC_STARTUP_RUN_6             6
#define MDC_STARTUP_RESTART_MECHANICS 8
#define MDC_STARTUP_HOLD              0x0C
#define MDC_STARTUP_HOLD_2            0x0D
#define MDC_STARTUP_HOLD_3            0x0E
#define MDC_STARTUP_RUN               0x10
/* the power-on handshake word, handshake_high << 8 | handshake_low */
#define MDC_HANDSHAKE_MDC_UP     0x5555
#define MDC_HANDSHAKE_SDC_ANSWER 0xAAAA
#define MDC_HANDSHAKE_DONE       0x3377
/* mdc_mailbox.download_cmd: the MDC firmware download steps */
#define MDC_DOWNLOAD_WRITE      0x80
#define MDC_DOWNLOAD_READ       0x88
#define MDC_DOWNLOAD_INFO_TABLE 0xA0
/* other mailbox values (selftest_run_power_on) */
#define MDC_VERSION_BOOT   0x42
#define MDC_OPTION_PRESENT 0x80

struct mailbox_last_page_cell { unsigned char v:8; };
struct mailbox_top_to_front_distance_high_cell { unsigned char v; };
struct mailbox_top_to_front_distance_low_cell { unsigned char v; };
struct mailbox_top_to_back_distance_high_cell { unsigned char v; };
struct mailbox_top_to_back_distance_low_cell { unsigned char v; };
struct mailbox_handshake_low_cell { volatile unsigned char v; };
struct mailbox_capture_steps_low_cell { unsigned char v; };
struct mailbox_capture_flags_cell { unsigned char v; };
struct mailbox_page_count_limit_high_cell { unsigned char v:8; };
struct mailbox_page_count_limit_low_cell { unsigned char v:8; };
struct mailbox_multifeed_bypass_cell { unsigned char f:8; };
struct mailbox_end_lead_front_low_cell { unsigned char v:8; };
struct mailbox_end_lead_front_high_cell { unsigned char v:8; };

/* functions */
extern void mdc_poll(void);
extern void mdc_send_eject(void);
extern void mdc_run_flatbed_scan_move(void);
extern void mdc_send_df_test(int on);
extern void mdc_clear_busy_unused(void);
extern void mdc_send_queued_command(void);
extern void mdc_send_power_save(void);
extern void mdc_send_command(unsigned char cmd);
extern int mdc_poll_result(void);
extern int mdc_run_feed(void);
extern void mdc_run_eject(void);
extern void mdc_set_scan_params(void);
extern void mdc_run_command(unsigned char);
extern void mdc_run_power_save(int on);
extern int mdc_run_power_save_timeout(void);
extern int mdc_run_backing(int);
extern void mdc_run_command_flag(unsigned char, int);
extern int mdc_run_lamps(unsigned char);
extern int mdc_run_unit_power(unsigned char);
extern int mdc_run_warmup_on(void);
extern int mdc_run_warmup_off(void);
extern int mdc_run_end_heater_warmup(char);
extern void mdc_run_motor_test(char, char);
extern void mdc_end_motor_test(char, char);
extern void mdc_run_pick_arm(int, int);
extern void mdc_request_capture(int);
extern void mdc_request_stop(void);
extern void mdc_request_abort(void);
extern void mdc_request_heater_off(unsigned char heaters);
extern void mdc_request_restart(void);
extern int mdc_request_log_state(void);
extern void mdc_send_request(unsigned char request);
extern void mdc_wake_board(void);
extern void mdc_run_read_overlap_patterns(void);
extern void mdc_run_write_overlap_patterns(void);
extern void mdc_run_dump_log(void);
extern int mdc_wait_request_taken(void);
extern int mdc_wait_pb9_low(void);
extern void mdc_set_feed_params(void);
extern int mdc_is_paper_in_hopper(void);
extern int mdc_is_adf_cover_open(void);
extern void mdc_check_fan_alarm(void);
extern void mdc_select_scan_tables(void);
extern int mdc_adf_deskew_or_overscan_on(void);
extern int mdc_unused_1(void);
extern void mdc_unused_2(void);
extern void mdc_unused_3(void);
extern int mdc_is_paper_path_clear(void);

/* constants */
extern const unsigned char mdc_gap_counts_one_side[8];
extern const unsigned char mdc_gap_counts_both_sides[24];
extern const unsigned char mdc_gap_lines_next_one_side[104];
extern const unsigned char mdc_gap_lines_next_both_sides[104];
extern const unsigned char mdc_gap_lines_first_one_side[32];
extern const unsigned char mdc_gap_lines_first_both_sides[32];
extern const unsigned char mdc_gap_lines_next_long_page[90];
extern const unsigned char mdc_gap_lines_first_long_page[32];
extern const unsigned char mdc_gap_lines_unused_1[24];
extern const unsigned char mdc_gap_lines_unused_2[24];
extern const unsigned char mdc_gap_lines_unused_3[24];
extern const unsigned char mdc_gap_lines_unused_4[24];
extern const unsigned char mdc_unused_word_table[18];
extern unsigned char *mdc_gap_tables_one_side[8];
extern unsigned char *mdc_gap_tables_both_sides[24];

/* variables */
extern int mdc_saved_imask;

/* I/O */
extern unsigned char mdc_cmd_params[];

struct mailbox_error_code_cell { volatile unsigned char v; };
/* mailbox sensor_status / sensor_status_2: the MDC's sensor bytes (the MDC tree's bit names, its shared.h) */
struct mailbox_sensor_status_bits {
    unsigned char pick_arm:1,
                  pick:1,
                  backing:1,
                  top:1,
                  no_paper:1,
                  adf_cover_open:1,
                  document_cover_open:1,
                  fb_home:1;
};
union mailbox_sensor_status_cell {
    volatile unsigned char v;
    volatile struct mailbox_sensor_status_bits f;
};
struct mailbox_sensor_status_2_bits {
    unsigned char df:1,
                  ultrasonic:1,
                  :5,
                  manual_feed_empty:1;
};
union mailbox_sensor_status_2_cell {
    volatile unsigned char v;
    volatile struct mailbox_sensor_status_2_bits f;
};
struct mailbox_area {
    char pad_000[1];
    volatile unsigned char handshake_high;
    char pad_002[1];
    struct mailbox_handshake_low_cell handshake_low;
    char pad_004[1];
    volatile unsigned char startup_state;
    char pad_006[1];
    volatile unsigned char startup_request;
    char pad_008[75];
    volatile unsigned char download_cmd;
    char pad_054[1];
    volatile unsigned char download_len_high;
    char pad_056[1];
    volatile unsigned char download_len_low;
    char pad_058[1];
    volatile unsigned char download_offset_3;
    char pad_05a[1];
    volatile unsigned char download_offset_2;
    char pad_05c[1];
    volatile unsigned char download_offset_1;
    char pad_05e[1];
    volatile unsigned char download_offset_0;
    char pad_060[1];
    volatile unsigned char option_present;
    char pad_062[3];
    volatile unsigned char board_variant;
    char pad_066[263];
    volatile unsigned char overscan_control;
    char pad_16e[117];
    volatile unsigned char non_rectangular;
    char pad_1e4[59];
    volatile unsigned char manual_feed_timeout;
    char pad_220[615];
    union mailbox_sensor_status_cell sensor_status;
    char pad_488[1];
    union mailbox_sensor_status_2_cell sensor_status_2;
    char pad_48a[1];
    volatile unsigned char multifeed_flags;
    char pad_48c[7];
    struct mailbox_error_code_cell error_code;
    char pad_494[3];
    volatile unsigned char separation_sheet_flags;
    char pad_498[13];
    volatile unsigned char lamp_status;
    char pad_4a6[15];
    struct mailbox_top_to_front_distance_high_cell top_to_front_distance_high;
    char pad_4b6[1];
    struct mailbox_top_to_front_distance_low_cell top_to_front_distance_low;
    char pad_4b8[1];
    struct mailbox_top_to_back_distance_high_cell top_to_back_distance_high;
    char p4ba[1];
    struct mailbox_top_to_back_distance_low_cell top_to_back_distance_low;
    char p4bc[5];
    volatile unsigned char backing_position;
    char pad_4c2[5];
    volatile unsigned char warmup_output;
    char pad_4c8[1];
    volatile unsigned char lamp_temperature_front;
    char p4ca[1];
    volatile unsigned char lamp_temperature_back;
    char p4cc[1];
    volatile unsigned char lamp_temperature_flatbed;
    char p4ce[1];
    volatile struct mailbox_capture_flags_cell capture_flags;
    char pad_4d0[1];
    volatile unsigned char byte_0x368;
    char pad_4d2[1];
    volatile unsigned char lamp_temperature;
    char pad_4d4[3];
    volatile unsigned char lamp_temperature_b;
    char pad_4d8[7];
    volatile char lamp_off;
    char pad_4e0[13];
    volatile unsigned char capture_steps_high;
    char p4ee[1];
    volatile struct mailbox_capture_steps_low_cell capture_steps_low;
    char pad_4f0[1];
    volatile unsigned char df_adjust_flags;
    char pad_4f2[1];
    volatile unsigned char cover_open_event;
    char pad_4f4[141];
    volatile unsigned char source;
    char pad_582[1];
    volatile unsigned char speed_class_a;
    char pad_584[1];
    volatile unsigned char speed_class_b;
    char pad_586[1];
    volatile unsigned char feed_options;
    char pad_588[1];
    volatile unsigned char max_sheet_length_high;
    char pad_58a[1];
    volatile unsigned char max_sheet_length_low;
    char pad_58c[3];
    volatile char feed_backing_request;
    char pad_590[1];
    volatile unsigned char long_page;
    char pad_592[1];
    volatile unsigned char lamp_off_minutes;
    char pad_594[1];
    volatile unsigned char lead_front_low;
    char pad_596[1];
    volatile unsigned char lead_back_low;
    char pad_598[1];
    volatile unsigned char lead_flatbed_low;
    char pad_59a[13];
    unsigned char jam_limit_high;
    char pad_5a8[1];
    unsigned char jam_limit_low;
    char p5aa[1];
    volatile unsigned char gap_extension;
    char p5ac[5];
    volatile unsigned char lead_front_high;
    char pad_5b2[1];
    volatile unsigned char lead_back_high;
    char pad_5b4[1];
    volatile unsigned char lead_flatbed_high;
    char pad_5b6[1];
    struct mailbox_last_page_cell last_page;
    char pad_5b8[1];
    struct mailbox_page_count_limit_high_cell page_count_limit_high;
    char p5ba[1];
    struct mailbox_page_count_limit_low_cell page_count_limit_low;
    char p5bc[1];
    volatile struct mailbox_multifeed_bypass_cell multifeed_bypass;
    char p5be[3];
    volatile struct mailbox_end_lead_front_low_cell end_lead_front_low;
    char pad_5c2[1];
    volatile struct mailbox_end_lead_front_high_cell end_lead_front_high;
    char pad_5c4[1];
    volatile unsigned char end_lead_back_low;
    char pad_5c6[1];
    volatile unsigned char end_lead_back_high;
    char pad_5c8[1];
    volatile unsigned char wake_flags;
    char p5ca[1];
    volatile unsigned char page_end;
    char p5cc[213];
    unsigned char cmd_end;
    char pad_6a2[1];
    volatile unsigned char manual_feed_end;
    char pad_6a4[317];
    volatile unsigned char motion_flags;
    char pad_7e2[1];
    volatile unsigned char cmd_state;
    char pad_7e4[1];
    volatile unsigned char cmd_code;
    char pad_7e6[1];
    volatile unsigned char gap_extension_flags;
    char pad_7e8[19];
    volatile unsigned char df_test_result;
    char p7fc[1];
    volatile unsigned char cmd_result;
    char p7fe[1];
    volatile unsigned char request;
};
extern struct mailbox_area mdc_mailbox;

#endif
