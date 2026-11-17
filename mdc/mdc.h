/* The application: the declarations shared by its source files. */
#ifndef MDC_H
#define MDC_H
#include <machine.h>
#include "io.h"
#include "shared.h"

/* Requests the SDC raises in mailbox.request (served by isr_tgi2a) */
#define MDC_REQUEST_REINIT_FAST      0x31
#define MDC_REQUEST_REINIT_WAKE      0x32
#define MDC_REQUEST_RESTART          0x40
#define MDC_REQUEST_CAPTURE_PAUSE    0x44
#define MDC_REQUEST_CAPTURE_RESUME      0x45
#define MDC_REQUEST_MOTORS_OFF       0x50
#define MDC_REQUEST_STOP             0x60
#define MDC_REQUEST_ABORT            0x61
#define MDC_REQUEST_LOG_STATE        0x62
#define MDC_REQUEST_HEATER_FRONT_OFF 0x71
#define MDC_REQUEST_HEATER_BACK_OFF  0x72
#define MDC_REQUEST_HEATERS_OFF      0x73

/* cmd_phase: the steps of cmd_dispatch */
#define CMD_PHASE_IDLE    0
#define CMD_PHASE_START   2
#define CMD_PHASE_RUNNING 4
#define CMD_PHASE_DONE    6

/* mailbox.cmd_end: the SDC ends a continuous command */
#define MDC_CMD_END 0x80

/* mailbox.backing_position: where the backing plate is */
#define MDC_BACKING_BLACK     0
#define MDC_BACKING_WHITE     1
#define MDC_BACKING_AT_SENSOR 2
#define MDC_BACKING_MOVING    3

/* Command MDC_CMD_MOTOR_TEST, parameter byte 0: the motor to run */
#define MDC_MOTOR_TEST_FEED          0
#define MDC_MOTOR_TEST_PICK          1
#define MDC_MOTOR_TEST_PICK_ARM      2
#define MDC_MOTOR_TEST_BACKING       3
#define MDC_MOTOR_TEST_FB_MOTOR      4
#define MDC_MOTOR_TEST_PICK_AND_FEED 0x10

/* mailbox.feed_options bits */
#define MDC_FEED_PREPICK               0x80
#define MDC_FEED_MULTIFEED_REPORT_ONLY 0x40
#define MDC_FEED_ULTRASONIC_CHECK      0x10
#define MDC_FEED_LENGTH_CHECK          0x08
#define MDC_FEED_JOB_SEPARATION        0x04
#define MDC_FEED_LENGTH_TOLERANCE      0x03
#define MDC_FEED_LENGTH_TOLERANCE_10MM 0x01
#define MDC_FEED_LENGTH_TOLERANCE_15MM 0x02
#define MDC_FEED_LENGTH_TOLERANCE_20MM 0x03

/* mailbox.multifeed_flags bits (the code uses the bit views) */
#define MDC_MULTIFEED_ULTRASONIC 0x04
#define MDC_MULTIFEED_LENGTH     0x08

/* mailbox.sensor_status bits (the code uses the bit views) */
#define MDC_SENSOR_FB_HOME             0x01
#define MDC_SENSOR_DOCUMENT_COVER_OPEN 0x02
#define MDC_SENSOR_ADF_COVER_OPEN      0x04
#define MDC_SENSOR_NO_PAPER            0x08
#define MDC_SENSOR_TOP                 0x10
#define MDC_SENSOR_BACKING             0x20
#define MDC_SENSOR_PICK                0x40
#define MDC_SENSOR_PICK_ARM            0x80

/* mailbox.lamp_status bits (the code uses the bit views; the SDC tests the masks) */
#define MDC_LAMP_STATUS_ADF_FRONT 0x80
#define MDC_LAMP_STATUS_ADF_BACK  0x40
#define MDC_LAMP_STATUS_FLATBED   0x20
#define MDC_LAMP_STATUS_KEEP_WARM 0x02

/* mailbox.source bits: the paper source of MDC_CMD_SCAN_MOVE */
#define MDC_SOURCE_FLATBED 0x20
#define MDC_SOURCE_ADF     0x40

/* mailbox.motion_flags bits: where the sheet is */
#define MDC_MOTION_FRONT        0x01
#define MDC_MOTION_BACK         0x02
#define MDC_MOTION_AT_TOP       0x04
#define MDC_MOTION_BACK_PENDING 0x08
#define MDC_MOTION_SIDES        0x03

/* mailbox.long_page: the long-page mode */
#define MDC_LONG_PAGE_ON 0x80

/* mailbox.df_adjust_flags: results of the sensor adjustments */
#define MDC_DF_ADJUST_ULTRASONIC_OK 0x01
#define MDC_DF_ADJUST_DF_OK         0x02

/* mailbox.capture_flags bits: the capture pause (requests 0x44 / 0x45) */
#define MDC_CAPTURE_PAUSED      0x01
#define MDC_CAPTURE_STEPS_VALID 0x02

/* mailbox.page_end bits: the SDC's request to end the pages and the MDC's answers */
#define MDC_PAGE_END_REQUEST 0x01
#define MDC_PAGE_END_DONE    0x02
#define MDC_PAGE_END_ACCEPTED 0x04

/* mailbox.separation_sheet_flags bits */
#define MDC_SEPARATION_SHEET_DETECTED 0x80

/* mailbox.df_test_result bits (the code uses the bit view; the SDC tests the mask) */
#define MDC_DF_TEST_RESULT_TOP_EDGE 0x80

/* mailbox.cover_open_event bits (the code uses the bit view; the SDC compares the byte) */
#define MDC_COVER_OPEN_EVENT_ADF 0x80

/* mailbox.warmup_output bits: the image of P2.1 */
#define MDC_WARMUP_OUTPUT_ON 0x01

/* mailbox.imprinter_flags bits */
#define MDC_IMPRINTER_NOT_INSTALLED 0x08

/* err_flags bits: the class of the pending error */
#define ERR_FLAG_DOCUMENT 0x08
#define ERR_FLAG_DEVICE   0x10

/* Error codes in err_code, err_saved_code and mailbox.error_code (the SDC's names) */
#define ERR_ULTRASONIC_SENSOR    0x02
#define ERR_DF_SENSOR            0x03
#define ERR_SHIPPING_LOCK        0x10
#define ERR_NO_PAPER             0x20
#define ERR_JAM                  0x31
#define ERR_EJECT_JAM            0x36
#define ERR_PICK_SENSOR_JAM      0x3A
#define ERR_ADF_COVER_OPEN       0x40
#define ERR_PICK                 0x50
#define ERR_MULTIFEED_LENGTH     0x56
#define ERR_SHEET_GAP            0x57
#define ERR_MULTIFEED_ULTRASONIC 0x58
#define ERR_STOPPED              0x78
#define ERR_MOTOR_FUSE           0x81
#define ERR_LAMP_FUSE            0x84
#define ERR_LAMP_FUSE_2          0x85
#define ERR_FLATBED_MOTOR        0x87
#define ERR_CGA_BOARD            0x8A
#define ERR_BACKING              0xC2
#define ERR_PICK_ARM             0xC4
#define ERR_VDCC                 0xE9

/* feed_action / pick_action: what the motor interrupt does next */
#define MOTOR_ACTION_NONE                   0
#define MOTOR_ACTION_OFF                    1
#define MOTOR_ACTION_START                  3
#define MOTOR_ACTION_START_RAMP             4
#define MOTOR_ACTION_STOP                   5
#define MOTOR_ACTION_STOP_RAMP              6
#define MOTOR_ACTION_STOP_KEEP_CURRENT      7
#define MOTOR_ACTION_STOP_KEEP_CURRENT_RAMP 8
#define MOTOR_ACTION_WAIT_STOPPED           9
#define MOTOR_ACTION_CHANGE_SPEED           10
#define FEED_ACTION_CHANGE_SPEED_RAMP       11
#define FEED_ACTION_ARM_HOME                12
#define FEED_ACTION_WAIT_ARM_HOME           13

/* feed_ramp_state, pick_ramp_state, fb_motor_ramp_state: the state of a speed ramp */
#define RAMP_STATE_STOPPED    0
#define RAMP_STATE_START      1
#define RAMP_STATE_CONSTANT   2
#define RAMP_STATE_STOPPING   3
#define RAMP_STATE_SPEED_UP   5
#define RAMP_STATE_SPEED_DOWN 6

/* feed_current_mode / pick_current_mode: the current phase of a motor */
#define CURRENT_MODE_RUN   0x43
#define CURRENT_MODE_STOP  0x44
#define CURRENT_MODE_START 0x55

/* motor_set_current, argument 1: the motor */
#define MOTOR_FEED     0
#define MOTOR_PICK     1
#define MOTOR_PICK_ARM 2
#define MOTOR_BACKING  3
#define MOTOR_FB       4

/* motor_set_current, argument 2: the row of the motor's current table */
#define MOTOR_CURRENT_OFF         0
#define MOTOR_CURRENT_HOLD        1
#define MOTOR_CURRENT_START       2
#define MOTOR_CURRENT_BACKING_RUN 3
#define MOTOR_CURRENT_RUN         4
#define MOTOR_CURRENT_RUN_CLASS_4 5
#define MOTOR_CURRENT_RUN_CLASS_5 6
#define MOTOR_CURRENT_BOOST       7

/* hw_set_ad_mode: what the A/D converter samples */
#define AD_SCAN_AN0_3           0
#define AD_ULTRASONIC           1
#define AD_ULTRASONIC_TRIGGERED 2
#define AD_DF                   3

/* bw_motor_arm_request (ARM_*) and bw_motor_backing_request (BACKING_*) */
#define BW_MOTOR_ARM_HOME              1
#define BW_MOTOR_ARM_TO_LEVEL          2
#define BW_MOTOR_ARM_TEST              3
#define BW_MOTOR_ARM_LEAVE_RETRY       4
#define BW_MOTOR_ARM_LIFT              5
#define BW_MOTOR_ARM_NEXT_LEVEL        6
#define BW_MOTOR_ARM_SEEK_STEP         7
#define BW_MOTOR_ARM_REHOME_NEXT_LEVEL 9
#define BW_MOTOR_BACKING_WHITE         1
#define BW_MOTOR_BACKING_BLACK         2
#define BW_MOTOR_BACKING_TEST          3
#define BW_MOTOR_BACKING_REVERSE       5

/* fb_motor_request: the move the FB motor interrupt runs */
#define FB_MOTOR_HOME     1
#define FB_MOTOR_SCAN     2
#define FB_MOTOR_RUN_TEST 3
#define FB_MOTOR_STOP     4
#define FB_MOTOR_TEST     5

/* Intelligent multifeed: the overlap mode (job_overlap_flags, settings +0xCB) and the pattern match */
#define OVERLAP_MODE_MASK              0x07
#define OVERLAP_MODE_MANUAL            0
#define OVERLAP_MODE_AUTO_1            4
#define OVERLAP_MODE_AUTO_2            6
#define OVERLAP_SETTINGS_MODE_MASK     0x06
#define OVERLAP_SETTINGS_KEEP_PATTERNS 0x08
#define OVERLAP_MATCH_TOLERANCE        0x27
#define OVERLAP_PATTERN_COUNT          32
#define OVERLAP_OPTIONS_MASK           0xf8
#define OVERLAP_UNMATCHED_SAVED        0x40
#define OVERLAP_UNMATCHED_MODE_5       0x20

/* feed_overlap_match_state: the pattern search of feed_match_overlap */
#define OVERLAP_MATCH_START     0
#define OVERLAP_MATCH_SEARCHING 0x12
#define OVERLAP_MATCH_FOUND     0x22
#define OVERLAP_MATCH_NONE      0x24

/* feed_auto_scan: the console's endless feed-and-scan loop, the command it posts next */
#define FEED_AUTO_SCAN_OFF            0
#define FEED_AUTO_SCAN_POST_FEED      0x55
#define FEED_AUTO_SCAN_POST_SCAN_MOVE 0x66

/* Feed limits (feed counts; the gap timeout in feed events) */
#define FEED_JAM_LIMIT_DEFAULT      0xFA00
#define FEED_JAM_DISABLED           0xFFFF
#define FEED_MIN_SHEET_GAP_STEPS    0x8E
#define FEED_MIN_LENGTH_CHECK_STEPS 0x110
#define FEED_GAP_TIMEOUT_EVENTS     0x1C2

/* Command timings in ticks of isr_tgi5a (about 2 ms), and the run tests' default sheet count */
#define CMD_TIMEOUT_TICKS             2500
#define CMD_SETTLE_DELAY_TICKS        50
#define CMD_FEED_WAIT_TICKS           5000
#define CMD_RUN_TEST_PAUSE_UNIT_TICKS 250
#define CMD_RUN_TEST_DEFAULT_SHEETS   200
#define TICKS_PER_SECOND              500

/* FB motor positions */
#define FB_MOTOR_DONE_POSITION 0x20EC
#define FB_MOTOR_TEST_SCAN_END 0x4BD4

/* Sensor levels, counts and limits */
#define SENSOR_DF_ADJUST_LEVEL       0xE8
#define SENSOR_MUX_ALARM_LEVEL       0x5A
#define SENSOR_MUX_ALARM_LEVEL_6_7   0x31
#define SENSOR_MUX_ALARM_COUNT       3
#define SENSOR_DEBOUNCE_MAX          5
#define SENSOR_DEBOUNCE_ON           2
#define ULTRASONIC_THRESHOLD_DEFAULT 0x50
#define ULTRASONIC_IDLE_MIN          0x30
#define ULTRASONIC_IDLE_MAX          0x80
#define ULTRASONIC_PAPER_MIN_IDLE    0xC0
#define ULTRASONIC_PAPER_MIN         0x4B
#define ULTRASONIC_PAPER_MAX         0xF8
#define ULTRASONIC_THRESHOLD_MAX     0xD0
#define ULTRASONIC_SAMPLES           0x200

/* Log record codes in p_log_buf */
#define LOG_END              0xFF
#define LOG_POWER_SAVE_ENTER 0xBB
#define LOG_POWER_SAVE_LEAVE 0xAA
#define LOG_SHEET_END        0xF2
#define LOG_SHEET_LENGTH     0xF5
#define LOG_FRONT_START      0xF8

/* Commands 0x1C-0x1F, parameter byte 0: the test mode switch */
#define MDC_TEST_MODE_PARAM 0x55

/* bw_motor_arm_state: the pick arm states of isr_tgi3a that other code tests (the other states stay numbers) */
#define BW_MOTOR_ARM_STATE_START     0
#define BW_MOTOR_ARM_STATE_AT_LEVEL  6
#define BW_MOTOR_ARM_STATE_SEEK_HOME 7
#define BW_MOTOR_ARM_STATE_AT_HOME   11

/* job_state: the steps of job_run */
#define JOB_STATE_RESET        0
#define JOB_STATE_IDLE         2
#define JOB_STATE_FEEDING      4
#define JOB_STATE_SHEET_READY  6
#define JOB_STATE_SCAN_START   8
#define JOB_STATE_SCAN_RESTART 10
#define JOB_STATE_SCANNING     12
#define JOB_STATE_EJECT        14
#define JOB_STATE_SHEET_END    16
#define JOB_STATE_FLATBED      18

/* job_feed_state: the steps of job_run_feed */
#define JOB_FEED_STATE_IDLE             0
#define JOB_FEED_STATE_PREPARE          2
#define JOB_FEED_STATE_WAIT_ARM         4
#define JOB_FEED_STATE_ARM_TO_LEVEL     6
#define JOB_FEED_STATE_START            8
#define JOB_FEED_STATE_PICK             10
#define JOB_FEED_STATE_PICKING          12
#define JOB_FEED_STATE_SHEET_READY      14
#define JOB_FEED_STATE_SHEET_READY_2    16
#define JOB_FEED_STATE_WAIT_SHEET_CLEAR 18
#define JOB_FEED_STATE_PREPICK          20
#define JOB_FEED_STATE_DONE             22
#define JOB_FEED_STATE_ERROR            24
#define JOB_FEED_STATE_ARM_HOME_EMPTY   26

/* job_scan_state: the steps of job_run_adf_scan */
#define JOB_SCAN_STATE_IDLE            0
#define JOB_SCAN_STATE_START           2
#define JOB_SCAN_STATE_WAIT_PATH_EMPTY 4
#define JOB_SCAN_STATE_WAIT_SPEED      6
#define JOB_SCAN_STATE_SETTLE          8
#define JOB_SCAN_STATE_DONE            10
#define JOB_SCAN_STATE_RESTART_STOPPED 12

/* job_fb_motor_state: the steps of job_run_fb_motor */
#define JOB_FB_MOTOR_STATE_IDLE      0
#define JOB_FB_MOTOR_STATE_START     2
#define JOB_FB_MOTOR_STATE_WAIT_HOME 4

/* job_eject_state: the steps of job_run_eject */
#define JOB_EJECT_STATE_IDLE          0
#define JOB_EJECT_STATE_START         2
#define JOB_EJECT_STATE_PICK_ASSIST   4
#define JOB_EJECT_STATE_WAIT_FEED_END 6
#define JOB_EJECT_STATE_WAIT_STOPPED  8

/* feed_state: the states of feed_handle_event (the feed motor) */
#define FEED_STATE_RESET             0
#define FEED_STATE_START             1
#define FEED_STATE_WAIT_SHEET        2
#define FEED_STATE_TRANSPORT         3
#define FEED_STATE_EJECT             4
#define FEED_STATE_STOP              5
#define FEED_STATE_WAIT_STOPPED      6
#define FEED_STATE_ULTRASONIC_IDLE   7
#define FEED_STATE_ULTRASONIC_PAPER  8
#define FEED_STATE_ULTRASONIC_START  9
#define FEED_STATE_ULTRASONIC_SAMPLE 10
#define FEED_STATE_ULTRASONIC_DONE   11
#define FEED_STATE_CAPTURE_STOP      12
#define FEED_STATE_CAPTURE_SETTLE    13
#define FEED_STATE_CAPTURE_PAUSED  14
#define FEED_STATE_CAPTURE_RESUME    15
#define FEED_STATE_IDLE              0xFE

/* pick_state: the states of isr_tgi1a (the pick motor) */
#define PICK_STATE_RESET              0
#define PICK_STATE_START              1
#define PICK_STATE_WAIT_PICK_SENSOR   2
#define PICK_STATE_AT_PICK_SENSOR     3
#define PICK_STATE_PICKED             4
#define PICK_STATE_CONTINUOUS_GAP     5
#define PICK_STATE_CONTINUOUS_RESTART 6
#define PICK_STATE_WAIT_TOP           7
#define PICK_STATE_STOP               8
#define PICK_STATE_RETRY              9
#define PICK_STATE_RETRY_WAIT         10
#define PICK_STATE_RETRY_PICK         11
#define PICK_STATE_STOP_AT_TOP        12
#define PICK_STATE_AT_TOP             13
#define PICK_STATE_TOP_RETRY          14
#define PICK_STATE_TOP_RETRY_WAIT     15
#define PICK_STATE_TOP_RETRY_FEED     16
#define PICK_STATE_IDLE               0xFE
#define PICK_STATE_ERROR              0xFF

/* fb_motor_state: the states of isr_tgi4a (the FB motor) */
#define FB_MOTOR_STATE_START            1
#define FB_MOTOR_STATE_CHECK_HOME       2
#define FB_MOTOR_STATE_LEAVE_HOME       3
#define FB_MOTOR_STATE_LEAVING_HOME     4
#define FB_MOTOR_STATE_PAST_HOME        5
#define FB_MOTOR_STATE_WAIT_STOPPED     6
#define FB_MOTOR_STATE_SEEK_HOME        7
#define FB_MOTOR_STATE_SEEKING_HOME     8
#define FB_MOTOR_STATE_HOME_OVERRUN     9
#define FB_MOTOR_STATE_TEST_PAUSE       10
#define FB_MOTOR_STATE_AT_HOME          11
#define FB_MOTOR_STATE_SCAN_START       12
#define FB_MOTOR_STATE_SCANNING         13
#define FB_MOTOR_STATE_SCAN_STOP        14
#define FB_MOTOR_STATE_CAPTURE_STOP     32
#define FB_MOTOR_STATE_CAPTURE_SETTLE   33
#define FB_MOTOR_STATE_CAPTURE_BACK_UP  34
#define FB_MOTOR_STATE_CAPTURE_PAUSED 35
#define FB_MOTOR_STATE_CAPTURE_RESUME   36

/* job_busy_flags masks: what the job has under way (the tests are masks, not the bit views) */
#define JOB_BUSY_PICK           0x01
#define JOB_BUSY_SCAN_MOVE      0x02
#define JOB_BUSY_EJECT          0x04
#define JOB_BUSY_SCAN_OR_EJECT  0x06
#define JOB_BUSY_PREPICK        0x10
#define JOB_BUSY_PREPICK_PICKED 0x80
#define JOB_BUSY_PREPICK_ANY    0x90

/* job_overlap_check_flags masks: the overlap match of the current sheet */
#define JOB_OVERLAP_CHECK_FOUND     0x0001
#define JOB_OVERLAP_CHECK_UNMATCHED 0x0002
#define JOB_OVERLAP_CHECK_ACTIVE    0x0003

/* main_flags_1 masks */
#define MAIN_FLAG_FUSE_CHECK     0x01
#define MAIN_FLAG_ADF_COVER_OPEN 0x80

/* hw_p2_out bits: the image of port 2's switched outputs (P2.3 multiplexer S2 and P2.7 flash ready are not outputs) */
#define HW_P2_ADF_FRONT_LAMP 0x01
#define HW_P2_WARMUP_OUTPUT  0x02
#define HW_P2_ADF_BACK_LAMP  0x04
#define HW_P2_FLATBED_HEATER 0x10
#define HW_P2_FRONT_HEATER   0x20
#define HW_P2_BACK_HEATER    0x40
#define HW_P2_ADF_LAMPS      0x05
#define HW_P2_ADF_HEATERS    0x60
#define HW_P2_HEATERS        0x70
#define HW_P2_OUTPUTS        0x77

/* feed_top_window_flags masks: the window the top sensor opens */
#define FEED_TOP_WINDOW_ARMED          0x0001
#define FEED_TOP_WINDOW_TRAILING_EDGE  0x0002
#define FEED_TOP_WINDOW_GAP_CORRECTION 0x0040
#define FEED_TOP_WINDOW_OPEN           0x0080

/* lamp_keep_warm_flags masks */
#define LAMP_KEEP_WARM_STARTED 0x0001
#define LAMP_KEEP_WARM_ACTIVE  0x0002

/* feed_pick_start_flags masks: the scan starts taken from the pick sensor */
#define FEED_PICK_START_FRONT   0x0001
#define FEED_PICK_START_BACK    0x0002
#define FEED_PICK_START_ENABLED 0x0080

/* job_scan_start_flags masks */
#define JOB_SCAN_START_TOP_COVERED 0x0001

/* feed_mode bits */
#define FEED_MODE_HALF_STEP       0x01
#define FEED_MODE_BIT_1           0x02
#define FEED_MODE_SKIP_NEXT_EVENT 0x80

/* fb_motor_mode bits */
#define FB_MOTOR_MODE_HALF_STEP 0x01

/* imprinter_line_status bits: the 16550 line status register */
#define IMPRINTER_LSR_DATA_READY    0x01
#define IMPRINTER_LSR_OVERRUN       0x02
#define IMPRINTER_LSR_PARITY_ERROR  0x04
#define IMPRINTER_LSR_FRAMING_ERROR 0x08

/* ---- External devices: two identical chips at 0x200000 and 0x400000, reached through
 * pointer pairs (one pointer per register block and chip). ---- */
struct chip_base_regs {                        /* block +0x00 (8 bytes) */
    union byte_view reg_00,
                    inputs;
    unsigned char reg_02_03[2];
    union byte_view reg_04;
    unsigned char reg_05;
    union byte_view reg_06,
                    outputs;
};
struct chip_uart_regs {                        /* block +0x10 */
    union byte_view rbr_thr,
                    ier,
                    iir_fcr,
                    lcr,
                    mcr,
                    lsr,
                    msr,
                    scr;
};
struct chip_sensor_regs {                        /* block +0x30 */
    union byte_view control,
                    reg_01,
                    reg_02,
                    reg_03,
                    reg_04,
                    reg_05;
    unsigned char reg_06_07[2];
};
struct motor_regs {                        /* block +0x40 (64 bytes) */
    union byte_view enable,
                    events,
                    reg_02;
    unsigned char reg_03;
    union byte_view reg_04,
                    reg_05,
                    control_1,
                    control_2,
                    period_1_high,
                    period_1_low,
                    period_2_high,
                    period_2_low,
                    ramp_control_1;
    unsigned char reg_0d;
    union byte_view ramp_index_1,
                    ramp_steps_1;
    union byte_view reg_10;
    unsigned char reg_11;
    union byte_view reg_12;
    unsigned char reg_13_14[2];
    union byte_view phase_2;
    unsigned char reg_16_2f[26];
    unsigned char current_1[8];               /* +0x30 */
    unsigned char current_2[8];               /* +0x38 */
};
struct chip_event_regs {                        /* block +0x80 */
    union byte_view reg_00,
                    events;
};

/* ---- Chip B's RAM 0x400100 up (the SDC's back window 0xC60000, odd bytes) ---- */
struct overlap_pattern_block {                  /* what commands 0x75 / 0x76 move */
    unsigned char count[2];                     /* job_overlap_pattern_count, big-endian */
    unsigned char flags;                        /* job_overlap_flags */
    unsigned char pad[5];
    unsigned char patterns_a[64];               /* eight rows of job_overlap_patterns_a, big-endian words */
    unsigned char patterns_b[64];               /* the same of job_overlap_patterns_b */
};

/* ---- Constants that the code shows only as a dead branch or an added zero ---- */
#define FEED_TOP_LIMIT_ADD 1            /* `g += k` (1) or `g = k` (0) */
#define IMPRINTER_STATUS_CODE_6 1            /* command 6 (1) or 4 (0) */
#define IMPRINTER_QUEUE_HIGH_OFFSET 0   /* added to the byte that becomes bits 16-23 */

/* ---- Structs of the on-chip RAM ---- */

struct ultrasonic_check {
    unsigned short sample_count,
                   sample_interval,
                   sample_countdown,
                   low_limit,
                   low_count,
                   threshold,
                   threshold_sum,
                   sum,
                   idle_threshold;
    union word_view flags;
    unsigned long sample_total;
    unsigned short idle_level,
                   paper_level,
                   w28,
                   w30,
                   w32,
                   w34,
                   w36;
    unsigned short pad_26[3];
    unsigned short w44,
                   paper_level_adjusted,
                   adjust_state;
    unsigned short pad_32;
    unsigned short in_area,
                   area_mode,
                   area_start,
                   area_end,
                   detect_start,
                   detect_end,
                   overlap_count,
                   overlap_high_count;
    union word_view overflow_flags;
    unsigned short w70,
                   threshold_scaled,
                   threshold_scaled_sum;
    unsigned short positions[4];
    unsigned short ends[4];
    unsigned short position_count,
                   overlap_pending,
                   high_count,
                   overlap_reference;
};

struct imprinter_channel_state {
    unsigned char *buf;
    unsigned short count,
                   index,
                   expected_len;
    unsigned short pad_0a;
    unsigned long error_count;
    union word_view rx_flags;
    union byte_view tx_flags;
    unsigned char b19;
    union word_view code;
    union byte_view status;
    unsigned char unknown_code;
    union word_view w24;
    unsigned short busy;
};

struct imprinter_record {                       /* 68 bytes */
    short w0;
    union byte_view r2;
    unsigned char block_count;
    unsigned char state;
    unsigned char pad_05;
    unsigned long send_chunk;
    union long_view send_remaining;
    union long_view receive_chunk;
    union long_view receive_remaining;
    union word_view w22;
    unsigned char b24,
                  b25;
    union word_view w26;
    unsigned char b28,
                  b29;
    unsigned char pad_1e[4];
    unsigned char *data;
    unsigned char blocks[10];
    unsigned char a48[20];
};

struct imprinter_header {                          /* 16 bytes */
    unsigned char flags[2];
    unsigned short s4,
                   y_offset;
    unsigned long l8,
                  counter;
    unsigned char counter_step,
                  b17;
};
struct imprinter_queue_entry {                         /* 60 bytes */
    unsigned char pad_00;
    unsigned char send_flags;
    struct imprinter_header header;                     /* +2 */
    unsigned char text_len;
    unsigned char text[40];
    signed char counter_increment;
};
struct imprinter_error {
    union err_flags_bits flags;
    unsigned char code;
};
struct imprinter_queue_state {                          /* 256 bytes */
    struct imprinter_queue_entry entries[4];
    unsigned char read_index,
                  write_index;
    union imprinter_queue_counter { long value; short top_word; char top_byte; } counter;
    unsigned long l246;
    unsigned short s250;
    unsigned char b252;
    unsigned char state;
    struct imprinter_error err;
};
struct imprinter_queue_cmd {
    unsigned char send_flags,
                  b1;
    unsigned char pad_02;
    unsigned char b3;
    unsigned char pad_04;
    unsigned char b5,
                  counter_low,
                  counter_mid,
                  counter_step,
                  y_offset_low,
                  y_offset_high,
                  counter_high;
    unsigned char text[40];
};

struct byte_pair {
    unsigned char a,
                  b;
};

struct imprinter_cmd_entry {
    unsigned char code;
    void (*handler)();
};

struct feed_edge_record {
    unsigned short edge[4];
};
#define FEED_EDGE_RECORD_COUNT 1        /* records in feed_edge_records: the index is computed `% FEED_EDGE_RECORD_COUNT` */

struct ramp_feed_record {
    unsigned char b[18];
};

/* The flag words with several subjects: one type each, so that a bit is named once (members
 * bN by position where a bit has no name; the word or byte view for the multi-bit masks). */
union job_stop_flags_bits {
    unsigned short w;
    struct { unsigned short :8, requested:1, restore_backing:1, :5, manual_feed_stop:1; } bit;
};
union job_overlap_check_flags_bits {
    unsigned short w;
    struct { unsigned short :14, unmatched:1, found:1; } bit;
};
union job_busy_flags_bits {
    unsigned char b;
    struct { unsigned char prepick_picked:1, :2, prepick:1, :1, eject:1, scan_move:1, pick:1; } bit;
};
union main_flags_1_bits {
    unsigned char b;
    struct { unsigned char adf_cover_open:1, :6, fuse_check:1; } bit;
};
union pick_flags_1_bits {
    unsigned char b;
    struct { unsigned char :3, b4:1, :1, ultrasonic_multifeed:1, :2; } bit;
};
/* The debounced sensor inputs, each set beside the mailbox sensor_status bit of the same name (in the
 * reverse order: bits 0-4 and 7 in the order of chip A's +0x01 inputs, the covers in 5 and 6). */
union sensor_flags_bits {
    unsigned char b;
    struct { unsigned char fb_home:1, document_cover_open:1, adf_cover_open:1, no_paper:1, top:1, backing:1, pick:1, pick_arm:1; } bit;
};
union pick_sensor_flags_bits {
    unsigned char b;
    struct { unsigned char :6, pick:1, :1; } bit;
};
/* Flag bytes and words whose bits the code sets and tests one by one: a type each, members bN by
 * position where a bit has no name; the bits no code touches are unnamed. */
union job_capture_flags_bits {
    unsigned char b;
    struct { unsigned char pause_requested:1, resume_requested:1, :5, pause_active:1; } bit;
};
union job_wake_flags_bits {
    unsigned char b;
    struct { unsigned char flatbed_lamp_pending:1, :6, wake_event:1; } bit;
};
union cmd_standby_flags_bits {
    unsigned char b;
    struct { unsigned char standby_allowed:1, wake_line_high:1, :6; } bit;
};
union feed_top_window_flags_bits {
    unsigned short w;
    struct { unsigned short :8, open:1, gap_correction:1, :4, trailing_edge:1, armed:1; } bit;
};
union hw_p2_out_bits {
    unsigned char b;
    struct { unsigned char :1, back_heater:1, front_heater:1, :2, adf_back_lamp:1, :1, adf_front_lamp:1; } bit;
};
union feed_mode_bits {
    unsigned char b;
    struct { unsigned char :7, half_step:1; } bit;
};
union imprinter_status_flags_bits {
    unsigned char b;
    struct { unsigned char :2, ink_low:1, :5; } bit;
};
union fb_motor_mode_bits {
    unsigned char b;
    struct { unsigned char :7, half_step:1; } bit;
};
union imprinter_send_flags_bits {
    unsigned char b;
    struct { unsigned char sending:1, :7; } bit;
};
union lamp_keep_warm_flags_bits {
    unsigned short w;
    struct { unsigned short :14, active:1, started:1; } bit;
};
union job_scan_start_flags_bits {
    unsigned short w;
    struct { unsigned short :15, top_covered:1; } bit;
    struct { unsigned char h, l; } byte;
};
union feed_pick_start_flags_bits {
    unsigned short w;
    struct { unsigned short :14, back_started:1, front_started:1; } bit;
    struct { unsigned char h, l; } byte;
};
union job_overlap_flags_bits {
    unsigned char b;
    struct { unsigned char :1, unmatched_saved:1, unmatched_mode_5:1, :5; } bit;
};
union imprinter_line_status_bits {
    unsigned char b;
    struct { unsigned char :7, data_ready:1; } bit;
};
union pick_flags_2_bits {
    unsigned char b;
    struct { unsigned char :3, b4:1, :4; } bit;
};
union imprinter_flags_5_bits {
    unsigned char b;
    struct { unsigned char b7:1, :7; } bit;
};
union imprinter_rx_buf {                        /* imprinter.c reads beyond it */
    unsigned char b[4];
    unsigned short w;
};

union imprinter_tx_word {                        /* the last object of section B */
    unsigned char b[4];
    struct imprinter_tx_view {
        unsigned char pad_00[3];
        union byte_view flags;
    } view;
};

/* ---- ROM constants and pointer tables ---- */
extern const unsigned short motor_filler_1;
extern const unsigned short cmd_pick_overrun_steps;
extern const unsigned short motor_filler_2;
extern const unsigned short bw_motor_backing_on_steps;
extern const unsigned short bw_motor_backing_off_steps;
extern const unsigned short bw_motor_backing_delay_ticks;
extern const unsigned short motor_filler_3;
extern const unsigned short bw_motor_backing_max_steps;
extern const unsigned short bw_motor_arm_base_steps;
extern const unsigned short bw_motor_arm_max_steps;
extern const unsigned short bw_motor_arm_on_steps;
extern const unsigned short bw_motor_arm_delay_ticks;
extern const unsigned short motor_filler_4;
extern const unsigned short bw_motor_arm_settle_ticks;
extern const unsigned short bw_motor_arm_lift_steps;
extern const unsigned short motor_filler_5;
extern const unsigned short bw_motor_arm_request_7_steps;
extern const unsigned short bw_motor_arm_request_4_steps;
extern const unsigned short bw_motor_arm_level_steps;
extern const unsigned short bw_motor_arm_request_6_steps;
extern const unsigned short pick_arm_adjust_steps;
extern const unsigned short bw_motor_arm_level_0;
extern const unsigned short bw_motor_arm_level_1;
extern const unsigned short bw_motor_arm_level_2;
extern const unsigned short bw_motor_arm_level_3;
extern const unsigned short bw_motor_arm_level_4;
extern const unsigned short bw_motor_arm_level_5;
extern const unsigned short bw_motor_arm_level_6;
extern const unsigned short bw_motor_arm_level_7;
extern const unsigned short bw_motor_arm_level_8;
extern const unsigned short bw_motor_arm_level_9;
extern const unsigned short bw_motor_arm_level_10;
extern const unsigned short bw_motor_arm_level_11;
extern const unsigned short bw_motor_arm_level_12;
extern const unsigned short bw_motor_arm_level_13;
extern const unsigned short bw_motor_arm_level_14;
extern const unsigned short bw_motor_arm_extra_steps;
extern const unsigned short bw_motor_arm_limit_request_4;
extern const unsigned short bw_motor_arm_limit;
extern const unsigned short cmd_arm_base_steps;
extern const unsigned short bw_motor_arm_mode_1_steps;
extern const unsigned short motor_filler_6;
extern const unsigned short job_length_margin_by_option[];         /* read as low bytes */
extern const unsigned short job_length_margin_by_setting[];         /* read as low bytes */
extern const unsigned short job_pick_to_front_distance;
extern const unsigned short job_pick_to_back_distance;
extern const unsigned short job_pick_start_timeout;
extern const unsigned short job_pick_start_timeout_slow;
extern const unsigned short main_top_to_front_distance;
extern const unsigned short main_top_to_back_distance;
extern const unsigned short job_read_lead_distance;
extern const unsigned short job_read_lead_margin;
extern const unsigned short job_pad_extra_length;
extern const unsigned short motor_filler_7[3];
extern const unsigned short job_eject_count;
extern const unsigned short job_eject_count_pad;
extern const unsigned short feed_last_page_wait_steps;
extern const unsigned short feed_speed_step_window_low;
extern const unsigned short feed_speed_step_window_high;
extern const unsigned short imprinter_queue_time_factor;
extern const unsigned short motor_filler_8[4];
extern const unsigned short feed_ultrasonic_check_count_a;
extern const unsigned short feed_ultrasonic_check_count_b;
extern const unsigned short feed_ultrasonic_sample_limit;
extern const unsigned short feed_ultrasonic_sample_offset;
extern const unsigned short main_pick_timeout_base;
extern const unsigned short pick_top_timeout;
extern const unsigned short motor_filler_9[4];
extern const unsigned short pick_retry_ticks;
extern const unsigned short pick_top_retry_ticks;
extern const unsigned short motor_filler_10[6];
extern const unsigned short feed_ultrasonic_limit;
extern const unsigned short motor_filler_11;
extern const unsigned short feed_ultrasonic_limit_low;
extern const unsigned short feed_fast_ramp_by_class[];
extern const unsigned short ramp_feed_fast_class_2_period;
extern const unsigned short ramp_feed_fast_class_3_period;
extern const unsigned short ramp_feed_fast_accel[];
extern const unsigned short ramp_feed_fast_brake[];
extern const unsigned short ramp_feed_accel[];
extern const unsigned short ramp_feed_brake[];
extern const unsigned short ramp_feed_gentle_accel[];
extern const unsigned short pick_continuous_by_class[];
extern const unsigned short pick_fast_next_by_class[];
extern const unsigned short pick_gap_steps_by_class[6];
extern const unsigned short ramp_pick_accel[];
extern const unsigned short ramp_pick_brake[];
extern const unsigned short pick_brake_entry[];
extern const unsigned short fb_motor_ramp_end_by_class[7];
extern const unsigned short ramp_fb_motor_accel[];
extern const unsigned short feed_manual_timeout_seconds[];
extern const unsigned long ramp_temperature_correction[];
extern const unsigned char motor_filler_12[2];
extern const unsigned char cmd_settle_ticks;
extern const unsigned char main_mailbox_350_value;
extern const unsigned char motor_filler_13[8];
extern const unsigned char feed_ramp_steps_by_class[];
extern const unsigned char feed_ramp_steps_default;
extern const unsigned char motor_filler_14[2];
extern const unsigned char feed_fast_ramp_steps[];
extern const unsigned char pick_ramp_steps_by_class[];
extern const unsigned char pick_ramp_steps_slow;
extern const unsigned char pick_ramp_steps_next_by_class[];
extern const unsigned char pick_slow_steps_a;
extern const unsigned char pick_slow_steps_b;
extern const unsigned char motor_filler_15;
extern const unsigned char pick_slow_steps_class_4_a;
extern const unsigned char pick_slow_steps_class_4_b;
extern const unsigned char motor_filler_16;
extern const unsigned char pick_slow_steps_class_5_a;
extern const unsigned char pick_slow_steps_class_5_b;
extern const unsigned long imprinter_flash_sectors[18];
extern const struct imprinter_cmd_entry imprinter_cmd_table[];
extern const struct imprinter_cmd_entry imprinter_cmd_table_7x[];
extern const unsigned char sensor_temperature_table[];
extern const unsigned char sensor_temperature_table_a[];
extern const unsigned char sensor_temperature_table_b[];
extern const unsigned char imprinter_code_lengths[];
extern const struct byte_pair imprinter_status_table[];
extern const unsigned short fb_motor_pause_ticks;
extern const unsigned short fb_motor_run_pause_ticks;
extern unsigned long *const hw_section_d_rom;
extern unsigned long *const hw_section_d_start;
extern unsigned long *const hw_section_d_end;
extern unsigned long *const hw_section_b_start;
extern unsigned long *const hw_section_b_end;
extern unsigned char *const p_ram_vectors;
extern unsigned char *const p_mailbox;
extern unsigned char *const p_mailbox_end;
extern unsigned char *const p_chip_b_ram;
extern unsigned char *const p_chip_b_ram_end;
extern unsigned char *const p_log_buf;
extern unsigned char *const hw_sdc_vdcc_pointers[20];
extern struct chip_base_regs * const p_chip_a;
extern struct chip_base_regs * const p_chip_b;
extern unsigned char *const p_chip_a_block_08;
extern unsigned char *const p_chip_b_block_08;
extern unsigned char *const p_chip_a_block_10;
extern struct chip_uart_regs * const p_chip_b_uart;
extern unsigned char *const p_chip_a_block_20;
extern unsigned char *const p_chip_b_block_20;
extern struct chip_sensor_regs * const p_chip_a_sensor;
extern unsigned char *const p_chip_b_sensor;
extern struct motor_regs * const p_chip_a_motor;
extern struct motor_regs * const p_chip_b_motor;
extern unsigned char *const p_chip_a_ramp;
extern unsigned char *const p_chip_b_ramp;
extern struct chip_event_regs * const p_chip_a_event;
extern struct chip_event_regs * const p_chip_b_event;
extern const unsigned char motor_current_template[5][8][8];
extern const unsigned long hw_flash_sector_records[36][4];
extern unsigned short *const p_wdt;
extern unsigned char *const p_ram_above_stack;
extern const unsigned long hw_vector_jumps[];

/* ---- On-chip RAM 0xFFDC00-0xFFFBFF ---- */
extern unsigned short feed_wait_steps;
extern unsigned char feed_filler_1;
extern unsigned char feed_ramp_state;
extern unsigned char feed_action;
extern unsigned char feed_start_ticks;
extern unsigned char feed_current_mode;
extern unsigned short feed_wait_ticks;
extern unsigned char feed_state;
extern unsigned char feed_ramp_steps;
extern unsigned char feed_ramp_steps_next;
extern union feed_mode_bits feed_mode;
extern unsigned char feed_sheets_in_path;
extern unsigned char feed_pick_edge_state;
extern unsigned char feed_top_edge_state;
extern volatile unsigned short feed_pick_edge_count;
extern volatile unsigned short feed_top_edge_count;
extern struct feed_edge_record feed_edge_records[FEED_EDGE_RECORD_COUNT];
extern unsigned short job_front_pick_start_steps;
extern unsigned short job_back_pick_start_steps;
extern unsigned short job_front_start_steps;
extern unsigned short job_front_end_steps;
extern unsigned short job_back_start_steps;
extern unsigned short job_back_end_steps;
extern unsigned char job_length_tolerance;
extern unsigned char job_filler_1[2];
extern unsigned short feed_run_current;
extern unsigned char feed_speed_step;
extern unsigned char feed_speed_step_target;
extern unsigned char feed_speed_step_phase;
extern unsigned char feed_speed_tracking;
extern unsigned char feed_ready_stop;
extern unsigned char feed_first_start;
extern unsigned short pick_step_count;
extern unsigned char pick_sheet_at_start;
extern unsigned char pick_ramp_state;
extern unsigned char pick_action;
extern unsigned char pick_start_ticks;
extern unsigned char pick_current_mode;
extern unsigned char pick_state;
extern unsigned short pick_wait_ticks;
extern unsigned char pick_ramp_steps;
extern unsigned char pick_ramp_steps_next;
extern unsigned short pick_delay_steps;
extern unsigned char pick_slow_steps;
extern unsigned char job_pick_slow_steps_b;
extern unsigned char pick_next_sheet;
extern unsigned char pick_run_current;
extern unsigned char fb_motor_filler_1[2];
extern unsigned char fb_motor_ramp_state;
extern unsigned char fb_motor_request;
extern unsigned char fb_motor_run_current_steps;
extern unsigned char fb_motor_filler_2;
extern unsigned char fb_motor_state;
extern union fb_motor_mode_bits fb_motor_mode;
extern unsigned char fb_motor_filler_3[2];
extern unsigned short fb_motor_position;
extern unsigned short fb_motor_scan_end;
extern unsigned short fb_motor_ramp_index;
extern unsigned short fb_motor_ramp_target;
extern unsigned short fb_motor_filler_4;
extern unsigned short fb_motor_unused_limit;
extern unsigned short fb_motor_done_position;
extern unsigned short fb_motor_scan_ramp_end;
extern unsigned short fb_motor_stop_position;
extern unsigned short fb_motor_mark_position;
extern unsigned char main_fb_home_at_init;
extern unsigned char main_fb_home_cmd;
extern unsigned char fb_motor_blocked;
extern unsigned short cmd_test_mode;
extern unsigned short tick_timeout;
extern unsigned short tick_period_count;
extern unsigned short job_wait_ticks;
extern unsigned short pick_timeout_steps;
extern unsigned short pick_top_timeout_steps;
extern unsigned short main_pick_timeout_first_steps;
extern unsigned short main_pick_timeout_steps;
extern unsigned short bw_motor_backing_delay;
extern unsigned short bw_motor_backing_steps;
extern unsigned short bw_motor_arm_delay;
extern unsigned short job_eject_ticks;
extern unsigned short bw_motor_arm_steps;
extern unsigned short bw_motor_arm_position;
extern unsigned short bw_motor_arm_saved_position;
extern unsigned short pick_arm_pick_count;
extern unsigned short main_arm_seek_picks;
extern unsigned short bw_motor_arm_lift_limit;
extern unsigned short main_arm_level_pending;
extern unsigned short pick_arm_adjust_limit;
extern unsigned short bw_motor_arm_idle_ticks;
extern unsigned short bw_motor_arm_steps_once;
extern unsigned short bw_motor_arm_position_once;
extern unsigned short bw_motor_arm_lifted;
extern unsigned short bw_motor_arm_default_steps;
extern unsigned short bw_motor_arm_default_position;
extern unsigned short util_arm_lift_position;
extern unsigned short fb_motor_delay_ticks;
extern unsigned short fb_motor_step_count;
extern unsigned long job_hold_ticks;
extern unsigned long pick_hold_ticks;
extern unsigned short pick_hold_current;
extern unsigned short cmd_timeout;
extern unsigned short cmd_delay;
extern unsigned short globals_filler_1[2];
extern unsigned short job_page_limit;
extern unsigned short job_page_count;
extern unsigned short job_feed_slot;
extern unsigned short job_feed_slot_flags[2];
extern unsigned short feed_event_count;
extern union word_view feed_event_divider;
extern unsigned short feed_in_transport;
extern unsigned short pick_sheet_index;
extern unsigned short pick_gap_index;
extern unsigned short globals_filler_2[2];
extern unsigned short feed_length_index;
extern unsigned short feed_gap_index;
extern unsigned short pick_edge_steps;
extern unsigned short job_pick_gap_steps;
extern unsigned short job_pick_gap_min;
extern unsigned short job_pick_gap_extended;
extern unsigned short job_pick_gap_extend;
extern unsigned short feed_sheet_counters[2];
extern unsigned short feed_sheet_active[2];
extern unsigned short job_sheet_slot;
extern unsigned short feed_top_edge_steps;
extern unsigned short feed_top_steps;
extern unsigned short globals_filler_3[2];
extern unsigned short pick_short_gap_stopped;
extern unsigned short feed_gap_steps;
extern unsigned short feed_max_sheet_length;
extern unsigned short feed_jam_steps;
extern unsigned short feed_jam_limit;
extern unsigned short pick_short_gap_slow_steps;
extern unsigned short pick_word_3;
extern unsigned short pick_short_gap_stop_steps;
extern unsigned short bw_motor_backing_period;
extern unsigned short bw_motor_arm_period_seek;
extern unsigned short bw_motor_arm_period;
extern unsigned short bw_motor_arm_period_return;
extern unsigned short tick_seconds;
extern unsigned short lamp_off_limit;
extern unsigned short lamp_off_count;
extern unsigned short lamp_keep_warm_delay;
extern union lamp_keep_warm_flags_bits lamp_keep_warm_flags;
extern unsigned short lamp_keep_warm_period_front;
extern unsigned short lamp_keep_warm_count_front;
extern unsigned short lamp_keep_warm_period_back;
extern unsigned short lamp_keep_warm_count_back;
extern unsigned char sensor_mux_enable;
extern unsigned char sensor_mux_step;
extern unsigned char sensor_temperature_a;
extern unsigned char sensor_temperature_a_level;
extern unsigned char console_byte_1;
extern unsigned char console_byte_2;
extern unsigned char sensor_temperature_b;
extern unsigned char sensor_temperature_b_level;
extern unsigned char sensor_an1_level;
extern unsigned char sensor_an2_level;
extern unsigned char sensor_an3_level;
extern unsigned short sensor_mux_count;
extern unsigned short sensor_mux_interval;
extern unsigned char lamp_heater_flatbed_on;
extern unsigned char lamp_heater_flatbed_target;
extern unsigned char lamp_heater_flatbed_warmup;
extern unsigned char lamp_heater_flatbed_inhibit;
extern unsigned char lamp_heater_front_on;
extern unsigned char lamp_heater_front_target;
extern unsigned char lamp_heater_front_warmup;
extern unsigned char lamp_heater_front_inhibit;
extern unsigned char lamp_heater_back_on;
extern unsigned char lamp_heater_back_target;
extern unsigned char lamp_heater_back_warmup;
extern unsigned char lamp_heater_back_inhibit;
extern unsigned short lamp_heater_control;
extern unsigned short lamp_heater_ticks;
extern unsigned short lamp_heater_interval;
extern unsigned short sensor_us_monitor;
extern unsigned short fb_motor_ramp_end_class_0;
extern unsigned short fb_motor_ramp_end_class_1;
extern unsigned short fb_motor_ramp_end_class_2;
extern unsigned short fb_motor_ramp_end_class_3;
extern unsigned short fb_motor_ramp_end_class_4;
extern unsigned short fb_motor_ramp_end_class_5;
extern unsigned short fb_motor_ramp_end_class_6;
extern unsigned short fb_motor_home_sync;
extern unsigned short globals_filler_4[2];
extern unsigned short tick_lamp_fuse_count;
extern unsigned short tick_motor_fuse_count;
extern unsigned short globals_filler_5;
extern unsigned short feed_fast_ramp;
extern unsigned short pick_fast_next;
extern unsigned short pick_continuous;
extern unsigned short job_last_page;
extern unsigned short job_feed_last_page;
extern unsigned short pick_continuous_gap_steps;
extern unsigned short job_last_page_flags;
extern union feed_top_window_flags_bits feed_top_window_flags;
extern union job_scan_start_flags_bits job_scan_start_flags;
extern unsigned short job_late_start_margin;
extern unsigned short feed_front_late_steps;
extern unsigned short feed_front_late_steps_saved;
extern union job_stop_flags_bits job_stop_flags;
extern unsigned short pick_sensor_on_steps;
extern unsigned short feed_pick_on_steps;
extern unsigned short feed_pick_off_steps;
extern unsigned short job_pad_offset;
extern unsigned short job_eject_steps;
extern unsigned short pick_gap_stop_count;
extern unsigned short pick_top_stop_count;
extern unsigned short globals_filler_6;
extern unsigned short cmd_run_test_pause_ticks;
extern unsigned short pick_sheet_steps[18];
extern unsigned short pick_gap_steps[18];
extern unsigned short feed_length_history[18];
extern unsigned short feed_gap_history[18];
extern unsigned short feed_ramp_stopped;
extern union feed_pick_start_flags_bits feed_pick_start_flags;
extern unsigned short feed_pick_start_timeout;
extern unsigned short feed_pick_start_countdown;
extern unsigned short tick_warmup_hold;
extern unsigned short globals_filler_7[4];
extern unsigned short pick_ready_edge_steps;
extern unsigned short pick_ready_edge_steps_logged;
extern unsigned short tick_word_1;
extern unsigned short tick_flatbed_lamp_delay;
extern unsigned short isr_irq4_count;
extern unsigned short feed_top_window_on_steps;
extern unsigned short feed_top_window_steps;
extern unsigned short feed_top_off_threshold;
extern unsigned short feed_gap_correction_steps;
extern unsigned short feed_top_window_on_saved;
extern unsigned short console_tx_index;
extern unsigned short console_rx_index;
extern unsigned short sensor_word_1;
extern unsigned short feed_ultrasonic_running;
extern union job_overlap_flags_bits job_overlap_flags;
extern unsigned char feed_overlap_hold_sheets;
extern unsigned char feed_overlap_bypass;
extern union job_overlap_check_flags_bits job_overlap_check_flags;
extern unsigned short feed_overlap_starts[];
extern unsigned short feed_overlap_lengths[];
extern unsigned short feed_overlap_starts_saved[];
extern unsigned short feed_overlap_lengths_saved[];
extern unsigned short job_overlap_pattern_count;
extern unsigned short feed_overlap_match_state;
extern unsigned short feed_overlap_search_index;
extern unsigned short feed_overlap_count;
extern unsigned short feed_overlap_max_length;
extern unsigned short feed_ultrasonic_starts_a;
extern unsigned short feed_ultrasonic_starts_b;
extern unsigned short job_df_on_count;
extern unsigned short pick_df_on_count;
extern struct ultrasonic_check multifeed_check;
extern unsigned short job_overlap_patterns_a[OVERLAP_PATTERN_COUNT][4];
extern unsigned short job_overlap_patterns_b[OVERLAP_PATTERN_COUNT][4];
extern unsigned short log_index;
extern union word_view log_error_word;
extern unsigned char log_error_armed;
extern unsigned short log_frozen;
extern unsigned short log_limit;
extern unsigned char log_filler_1[13];
extern union imprinter_line_status_bits imprinter_line_status;
extern unsigned char log_filler_2[8];
extern struct imprinter_channel_state imprinter_state;
extern unsigned short imprinter_filler_1;
extern struct imprinter_channel_state *imprinter_channel;       /* set to &imprinter_state */
extern struct imprinter_header imprinter_entry_header;
extern unsigned char imprinter_entry_text[41];
extern unsigned char imprinter_filler_2;
extern union imprinter_status_flags_bits imprinter_status_flags;
extern unsigned short imprinter_word_1;
extern unsigned char imprinter_byte_1;
extern unsigned char imprinter_byte_2;
extern unsigned char imprinter_filler_3[6];
extern unsigned long imprinter_ink_used;
extern unsigned long imprinter_long_2;
extern unsigned long imprinter_long_3;
extern unsigned char imprinter_byte_3;
extern unsigned char imprinter_byte_4;
extern unsigned char imprinter_byte_5;
extern unsigned char imprinter_byte_6;
extern unsigned char imprinter_byte_7;
extern unsigned char imprinter_byte_8;
extern unsigned short imprinter_word_2;
extern struct imprinter_record imprinter_records[1];
extern union imprinter_send_flags_bits imprinter_send_flags;
extern unsigned char imprinter_filler_4;
extern unsigned char sensor_mux_mode;
extern unsigned char sensor_mux_levels[8];
extern unsigned char sensor_mux_alarm_counts[8];
extern unsigned char sensor_mux_cycles;
extern unsigned short feed_speed_change_misses;
extern unsigned short lamp_unit_front_on;
extern unsigned short lamp_unit_back_on;
extern unsigned short lamp_unit_flatbed_on;
extern unsigned short tick_word_2;
extern unsigned short main_init_after_wake;
extern unsigned short feed_clearing_path;
extern unsigned short feed_gap_timeout;
extern unsigned long job_top_clear_steps;
extern unsigned short job_front_reached;
extern unsigned short sensor_df_test;
extern unsigned short job_scan_seconds;
extern unsigned short main_error_seconds;
extern unsigned long tick_warmup_seconds;
extern unsigned short feed_empty_seconds;
extern unsigned short feed_manual_timeout_limit;
extern unsigned short pick_empty_stop_seconds;
extern unsigned char feed_manual_empty_count;
extern unsigned short feed_manual_empty;
extern unsigned short tick_manual_end_ticks;
extern unsigned short tick_manual_end_active;
extern signed char pick_retries;
extern signed char pick_retry_limit;
extern unsigned char pick_top_retry_limit;
extern unsigned char pick_speed_select;
extern unsigned char pick_boost_steps;
extern unsigned char main_clear_path_pending;
extern unsigned char bw_motor_backing_request;
extern unsigned char bw_motor_backing_state;
extern unsigned char bw_motor_backing_retries;
extern unsigned char globals_filler_8[2];
extern unsigned char cmd_byte_1;
extern union pick_flags_1_bits pick_flags_1;
extern union pick_flags_2_bits pick_flags_2;
extern unsigned char main_backing_homed;
extern unsigned char bw_motor_backing_quiet;
extern unsigned char bw_motor_arm_request;
extern unsigned char bw_motor_arm_state;
extern unsigned char bw_motor_arm_mode;
extern union job_busy_flags_bits job_busy_flags;
extern unsigned char tick_adf_cover_state;
extern unsigned char tick_hopper_state;
extern unsigned char tick_document_cover_state;
extern unsigned char cmd_phase;
extern unsigned char feed_auto_scan;
extern unsigned char cmd_bw_motor_cycle_state;
extern unsigned char job_pick_speed_class;
extern unsigned char job_feed_speed_class;
extern unsigned char job_fb_motor_speed_class;
extern unsigned char err_state;
extern unsigned char err_deferred;
extern union err_flags_bits err_flags;
extern unsigned char err_code;
extern unsigned char main_init_error_flags;
extern unsigned char main_init_error_code;
extern unsigned char err_saved_flags;
extern unsigned char err_saved_code;
extern union main_flags_1_bits main_flags_1;
extern unsigned char main_power_save_error_flags;
extern unsigned char main_power_save_error_code;
extern unsigned char globals_filler_9;
extern unsigned char main_init_state;
extern unsigned char globals_filler_10;
extern unsigned char main_byte_9;
extern unsigned char feed_byte_6;
extern union hw_p2_out_bits hw_p2_out;
extern unsigned char job_state;
extern unsigned char job_feed_state;
extern unsigned char job_scan_state;
extern unsigned char job_fb_motor_state;
extern unsigned char job_eject_state;
extern unsigned char main_progress_code;
extern unsigned char globals_filler_11;
extern unsigned char request_saved_imask;
extern unsigned char tick_saved_imask;
extern unsigned char feed_saved_imask;
extern unsigned char pick_saved_imask;
extern unsigned char bw_motor_saved_imask;
extern unsigned char fb_motor_saved_imask;
extern unsigned char globals_filler_12[2];
extern unsigned char lamp_adf_off_limit;
extern unsigned char lamp_adf_off_count;
extern unsigned char lamp_keep_warm_on_front;
extern unsigned char lamp_keep_warm_on_back;
extern unsigned char lamp_keep_warm_off_front;
extern unsigned char lamp_keep_warm_off_back;
extern union job_capture_flags_bits job_capture_flags;
extern unsigned char sensor_df_active;
extern unsigned char pick_df_armed;
extern unsigned char pick_df_active;
extern unsigned char pick_df_steps;
extern unsigned char pick_df_hits;
extern unsigned char sensor_df_level;
extern unsigned char sensor_df_range;
extern unsigned char err_history_index;
extern unsigned char err_history_last;
extern unsigned char err_history[];
extern unsigned char ramp_feed_saved[];
extern unsigned char ramp_feed_restore_pending;
extern unsigned char ramp_feed_restore_step;
extern unsigned char job_first_speed_class;
extern union mailbox_multifeed_flags_bits feed_multifeed_flags;
extern unsigned char globals_filler_14;
extern unsigned char cmd_run_test_phase;
extern unsigned char cmd_run_test_sheets;
extern unsigned char cmd_run_test_picking;
extern unsigned char cmd_run_test_class;
extern unsigned char job_state_history[];
extern unsigned char job_cmd_history[];
extern unsigned char job_state_history_index;
extern unsigned char job_state_history_last;
extern unsigned char job_cmd_history_index;
extern unsigned char job_cmd_history_last;
extern union imprinter_rx_buf imprinter_packet;
extern unsigned char job_length_history_reset;
extern union job_wake_flags_bits job_wake_flags;
extern union cmd_standby_flags_bits cmd_standby_flags;
extern struct ramp_feed_record ramp_feed_speed_records[3];
extern struct byte_pair ramp_feed_periods[9];
extern unsigned char motor_current_tables[5][8][8];
extern unsigned char ramp_fb_motor_high[];
extern unsigned char ramp_fb_motor_low[];
extern unsigned char ramp_fb_motor_half_high[];
extern unsigned char ramp_fb_motor_half_low[];
extern union sensor_flags_bits sensor_flags;
extern union pick_sensor_flags_bits pick_sensor_flags;
extern unsigned char sensor_home_count;
extern unsigned char sensor_document_cover_count;
extern unsigned char sensor_cover_count;
extern unsigned char sensor_empty_count;
extern unsigned char sensor_top_count;
extern unsigned char sensor_backing_count;
extern unsigned char sensor_pick_count;
extern unsigned char sensor_pick_arm_count;
extern unsigned char sensor_top_count_limit;
extern unsigned char sensor_top_threshold;
extern unsigned char globals_filler_15[6];
extern unsigned char pick_sensor_count;
extern unsigned char globals_filler_16[3];
extern union byte_view bw_motor_arm_phase;
extern unsigned char bw_motor_backing_phase;
extern unsigned char console_answer[16];
extern unsigned char console_command[16];
extern unsigned char log_buf[];
extern union imprinter_flags_5_bits imprinter_flags_5;
extern unsigned char imprinter_tx_header[8];
extern union imprinter_tx_word imprinter_tx_data;

/* ---- External RAM and devices, 0x200000 up ---- */
extern unsigned char main_chip_b_byte_1;
extern unsigned char main_chip_b_byte_2;
extern unsigned char console_chip_b_byte_1[];
extern unsigned char cmd_log_dump_count[2];
extern unsigned char cmd_log_dump[0xF0];
extern struct overlap_pattern_block cmd_overlap_pattern_block;

/* ---- The boot program's flash routines (the other link: extern_mdc.src) ---- */
extern unsigned char flash_section_rom[];        /* their ROM copy, section PFLASH */
extern unsigned char flash_section_start[];      /* where they run, section PFLASHR */

/* ---- Functions ---- */
extern void boot_delay(void);
extern void main(void);
extern void imprinter_queue_run(struct imprinter_queue_state *, unsigned char);
extern unsigned char imprinter_queue_check_error(struct imprinter_error *, unsigned char);
extern void imprinter_queue_load_counter(struct imprinter_queue_state *, unsigned char);
extern void imprinter_queue_advance_counter(struct imprinter_queue_state *, unsigned char);
extern void imprinter_queue_undo_counter(struct imprinter_queue_state *, unsigned char);
extern void imprinter_queue_fill_entry(struct imprinter_queue_entry *, struct imprinter_queue_cmd *);
extern void isr_tgi2a(void);
extern void isr_tgi3a(void);
extern void bw_motor_arm_run_seek(void);
extern void bw_motor_arm_start_seek(void);
extern void bw_motor_arm_run(unsigned char);
extern void bw_motor_arm_start_leave(void);
extern void bw_motor_arm_stop(void);
extern void bw_motor_backing_run_reverse(void);
extern void bw_motor_backing_start_reverse(void);
extern void bw_motor_backing_run(void);
extern void bw_motor_backing_start_forward(void);
extern void bw_motor_backing_set_period(void);
extern void bw_motor_backing_halt(void);
extern void bw_motor_backing_stop(void);
extern void bw_motor_record_backing(unsigned char);
extern void feed_handle_event(void);
extern void feed_read_sensors(union byte_view);
extern void feed_check_ultrasonic(void);
extern int feed_multifeed_can_pass(void);
extern int feed_match_overlap(void);
extern void feed_count_overlaps(void);
extern void feed_find_max_overlap_length(void);
extern void feed_start_ultrasonic_check(void);
extern int feed_in_check_area(void);
extern void feed_check_pick_start(void);
extern void feed_start(unsigned char);
extern void feed_stop(void);
extern void feed_stop_keep_current(void);
extern void feed_change_speed(unsigned char, unsigned char);
extern void feed_set_cmd_state(unsigned char);
extern void isr_tgi4a(void);
extern void fb_motor_start_forward(unsigned short);
extern void fb_motor_stop(void);
extern void fb_motor_start_reverse(unsigned short);
extern void fb_motor_stop_at_home(void);
extern void fb_motor_set_target(unsigned short);
extern void isr_tgi1a(void);
extern void pick_start(unsigned char);
extern void pick_stop(void);
extern void pick_stop_keep_current(void);
extern void pick_change_speed(unsigned char, unsigned char);
extern void isr_nmi(void);
extern void isr_empty(void);
extern void isr_irq4(void);
extern void isr_unused(void);
extern void isr_adi(void);
extern void isr_irq0(void);
extern void isr_irq7(void);
extern void imprinter_sci0_rx(void);
extern void imprinter_sci0_tx(void);
extern void imprinter_sci0_tx_end(void);
extern void imprinter_sci1_rx(void);
extern void imprinter_sci1_tx(void);
extern void imprinter_sci1_tx_end(void);
extern void motor_set_current(unsigned short, unsigned short);
extern void sensor_set_ultrasonic_sampling(unsigned char);
extern void sensor_stop_ultrasonic(void);
extern void sensor_df_on(void);
extern void sensor_df_off(void);
extern void sensor_hook(unsigned char, unsigned char);
extern void isr_tgi5a(void);
extern void tick_poll_covers(void);
extern void tick_read_sensor_mux(void);
extern void tick_check_mux_level(short);
extern void tick_control_heaters(void);
extern void tick_keep_lamps_warm(void);
extern void main_loop(void);
extern void main_handle_error(void);
extern void main_init_mechanics(void);
extern void main_copy_image(void);
extern void isr_rxi0(void);
extern void isr_txi0(void);
extern void isr_eri0(void);
extern void console_reset_rx(void);
extern void hw_hook_started(void);
extern void hw_copy_vectors(void);
extern void hw_init(void);
extern int hw_init_delay(void);
extern void hw_init_hook(void);
extern void hw_init_dtc(void);
extern void hw_init_sci(void);
extern void hw_init_tpu(void);
extern void hw_init_tmr(void);
extern void hw_init_sensors(void);
extern void hw_set_ad_mode(unsigned char);
extern void hw_init_da(void);
extern void hw_init_intc(void);
extern int hw_init_step_10(void);
extern int hw_test_words(unsigned short *, unsigned long, unsigned short);
extern void hw_init_sections(void);
extern void hw_hook_sections(void);
extern void hw_init_motor_chips(void);
extern void hw_clear_mailbox(void);
extern void main_handshake(void);
extern void main_scale_ultrasonic_threshold(void);
extern void main_read_initial_sensors(void);
extern void motor_enable_chips(unsigned char);
extern void ramp_load(void);
extern void ramp_build_feed(void);
extern void ramp_build_feed_gentle(void);
extern void ramp_restore_feed(void);
extern void ramp_load_feed_speed_steps(unsigned char);
extern void ramp_build_fb_motor(void);
extern void util_delay(void);
extern void util_flash_enter_download(void);
extern void util_lamp_clear_timers(void);
extern void util_bw_motor_load_arm_settings(void);
extern void util_overlap_clear_after_error(void);
extern void util_log_init(void);
extern void util_log_add_error_word(void);
extern void util_backing_to_home(void);
extern void util_fb_motor_home(void);
extern void util_feed_clear_path(void);
extern void util_mem_clear(unsigned char *, unsigned long);
extern void util_mem_copy(unsigned char *, unsigned char *, unsigned long);
extern int util_mem_equal(unsigned char *, unsigned char *, unsigned long);
extern int util_mem_equal_count(unsigned char *, unsigned char *, unsigned long, unsigned long *);
extern int util_mem_check_fill(unsigned char *, unsigned char, unsigned long, unsigned long *);
extern void util_mem_fill(unsigned char *, unsigned char, unsigned long);
extern void util_mem_fill_short(unsigned char *, short, unsigned char);
extern void util_put_be16(unsigned char *, unsigned short);
extern unsigned short util_get_be16(unsigned char *);
extern void job_run(void);
extern void job_run_feed(void);
extern void job_run_adf_scan(void);
extern void job_run_fb_motor(void);
extern void job_hook(unsigned long, unsigned char);
extern unsigned short job_scale_1000_254(unsigned short);
extern void job_run_eject(void);
extern void job_set_multifeed_area(void);
extern void job_set_multifeed_area_steps(unsigned short, unsigned char, unsigned long, unsigned long);
extern void job_set_overlap_mode(void);
extern void job_store_overlap_pattern(void);
extern void job_select_fast_ramps(unsigned char);
extern void job_set_cmd_state();
extern void job_restore_backing(void);
extern void cmd_dispatch(void);
/* the block at 0x200440 (shared.h) */
extern void cmd_read_memory(union mailbox_cmd_params *);
extern void cmd_write_memory(union mailbox_cmd_params *);
extern void cmd_bw_motor_cycle(unsigned char);
extern void cmd_lamps(void);
extern void cmd_motor_test(unsigned char);
extern void cmd_adjust_df_sensor(void);
extern void cmd_pick_sheet(void);
extern void cmd_adjust_ultrasonic(unsigned char);
extern void cmd_backing_test(unsigned char *, unsigned char);
extern void cmd_backing(unsigned char);
extern void cmd_pick_arm(unsigned char);
extern void cmd_power_save(void);
extern void cmd_end_heater_warmup(void);
extern void cmd_unit_power(void);
extern void cmd_eject(void);
extern void cmd_feed_run_test(unsigned char);
extern void cmd_bw_motor_run_test(unsigned char);
extern void cmd_feed_flatbed_run_test(unsigned char);
extern void cmd_clear_overlap_patterns(void);
extern void cmd_read_overlap_patterns(void);
extern void cmd_write_overlap_patterns(void);
extern void cmd_dump_log(void);
extern void cmd_test_mode_on(void);
extern void cmd_test_mode_off(void);
extern unsigned char cmd_adjust_ultrasonic_idle(void);
extern unsigned char cmd_adjust_ultrasonic_result(void);
extern void cmd_warmup_on(void);
extern void cmd_warmup_off(void);
extern void imprinter_sci0_transmit(void);
extern void imprinter_sci0_receive(void);
extern void imprinter_sci0_send_byte(void);
extern void imprinter_sci0_take_reply(void);
extern void imprinter_rx_more(void);
extern void imprinter_rx_complete(void);
extern void imprinter_sci0_clear_errors(void);
extern void imprinter_hook_rx_complete(void);
extern void imprinter_sci0_poll(void);
extern void imprinter_sci1_transmit(void);
extern void imprinter_sci1_receive(void);
extern void imprinter_sci1_send_byte(void);
extern void imprinter_sci1_take_reply(void);
extern void imprinter_sci1_rx_more(void);
extern void imprinter_sci1_rx_complete(void);
extern void imprinter_sci1_clear_errors(void);
extern void imprinter_hook_start_tx(void);
extern void imprinter_sci1_poll(void);
extern void imprinter_poll_dispatch(void);
extern void imprinter_channel_reset(void);
extern void imprinter_channel_clear(void);
extern void imprinter_parse_reply(unsigned char);
extern void imprinter_parse_reply_step_2(unsigned char);
extern void imprinter_rx_dispatch(void);
extern void imprinter_rx_01(void);
extern void imprinter_rx_02(void);
extern void imprinter_rx_0a(void);
extern void imprinter_rx_0c(void);
extern void imprinter_rx_0e(void);
extern void imprinter_rx_10(void);
extern void imprinter_rx_1a(void);
extern void imprinter_rx_22(void);
extern void imprinter_rx_26(void);
extern void imprinter_rx_28(void);
extern void imprinter_rx_2c(void);
extern void imprinter_rx_30(void);
extern void imprinter_rx_32(void);
extern void imprinter_rx_34(void);
extern void imprinter_rx_04(void);
extern void imprinter_rx_06(void);
extern void imprinter_rx_08(void);
extern void imprinter_rx_12(void);
extern void imprinter_rx_14(void);
extern void imprinter_rx_20(void);
extern void imprinter_rx_21(void);
extern void imprinter_rx_29(void);
extern void imprinter_rx_2a(void);
extern void imprinter_rx_38(void);
extern void imprinter_rx_3a(void);
extern void imprinter_rx_3c(void);
extern void imprinter_rx_2f(void);
extern void imprinter_rx_78(short);
extern void imprinter_rx_79(short);
extern void imprinter_rx_7a(short);
extern void imprinter_rx_7b(short);
extern void imprinter_rx_7c(short);
extern void imprinter_rx_76(short);
extern void imprinter_rx_7d(short);
extern int imprinter_rx_check_frame(void);
extern void imprinter_rx_reject(void);
extern void imprinter_rx_unknown(void);
extern void imprinter_rx_done(void);
extern void imprinter_start_send(unsigned char);
extern void imprinter_send_done(void);
extern void imprinter_send_10(void);
extern void imprinter_send_26(unsigned char);
extern void imprinter_send_02(unsigned char);
extern void imprinter_send_wait(short, unsigned char, unsigned char);
extern void imprinter_send(unsigned char, unsigned char);
extern void imprinter_send_14_wait_2(short);
extern void imprinter_run_record(unsigned char);
extern void imprinter_rx_76_step_2(unsigned char);
extern void imprinter_copy_record(unsigned char *, unsigned char);
extern void imprinter_send_01(unsigned char);
extern void imprinter_send_28(unsigned char);
extern void imprinter_send_12(short, unsigned char);
extern void imprinter_send_14(short, unsigned char);
extern void imprinter_send_14_wait(short, unsigned char);
extern void imprinter_send_20(unsigned char);
extern void imprinter_send_24(unsigned char);
extern void imprinter_send_21_wait(unsigned char);
extern void imprinter_send_21(unsigned char);
extern void imprinter_send_06(unsigned char);
extern void imprinter_send_34(unsigned char);
extern void imprinter_send_status(short, unsigned char);
extern unsigned char imprinter_send_status_step_2(unsigned char);
extern unsigned char imprinter_get_ink_percent(void);
extern void imprinter_probe(void);
extern void imprinter_probe_uart(void);
extern void log_hook_begin(short);
extern void log_advance(void);
extern void log_advance_255(void);
extern void log_end(void);
extern void hw_hook_p3(void);
extern void flash_run_download(void);
extern int flash_erase_sector(unsigned long, unsigned short *, unsigned short);
extern int flash_program_word(unsigned long, unsigned short *, unsigned short, unsigned short);
extern int flash_write_unlock_cycles(void);

#endif /* MDC_H */
