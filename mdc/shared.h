/* What the boot block and the application of the MDC both see: the byte and word views, and
 * the RAM block 0x200100-0x2004FF of the external device at 0x200000 (the block that the
 * SDC reads and writes too). */
#ifndef SHARED_H
#define SHARED_H
/* A call made against another declaration of the callee than its definition: the callee is
 * cast to the type the caller sees.
 *   NOPROTO(f)(args)                 no prototype in scope: the arguments are passed as int
 *   CALL_AS(ret, f, (types))(args)   a prototype with other parameter types */
#define CALL_AS(ret, f, params) ((ret (*) params)(f))
#define NOPROTO(f)              CALL_AS(void, f, ())

/* Command codes the SDC posts in mailbox.cmd_code (the SDC's names) */
#define MDC_CMD_CONSOLE_IDLE           0x01
#define MDC_CMD_READ_MEMORY            0x06
#define MDC_CMD_WRITE_MEMORY           0x07
#define MDC_CMD_BW_MOTOR_CYCLE         0x10
#define MDC_CMD_LAMPS                  0x11
#define MDC_CMD_MOTOR_TEST             0x12
#define MDC_CMD_ADJUST_DF_SENSOR       0x14
#define MDC_CMD_PICK_SHEET             0x16
#define MDC_CMD_ADJUST_ULTRASONIC      0x17
#define MDC_CMD_NO_OPERATION           0x18
#define MDC_CMD_IMPRINTER_INK          0x19
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

/* mailbox.cmd_state: the progress of the posted command */
#define MDC_CMD_STATE_IDLE    0
#define MDC_CMD_STATE_POSTED  2
#define MDC_CMD_STATE_RUNNING 4
#define MDC_CMD_STATE_ENDED   6
#define MDC_CMD_STATE_DONE    8

/* mailbox.cmd_result: the answer written with MDC_CMD_STATE_DONE */
#define MDC_RESULT_OK          0
#define MDC_RESULT_ERROR       2
#define MDC_RESULT_SHEET_READY 0x10
#define MDC_RESULT_FRONT_START 0x12
#define MDC_RESULT_BACK_START  0x14

/* The start-up handshake word at mailbox.handshake_high/_low */
#define MDC_HANDSHAKE_MDC_UP     0x5555
#define MDC_HANDSHAKE_SDC_ANSWER 0xAAAA
#define MDC_HANDSHAKE_DONE       0x3377

/* mailbox.startup_state / startup_request: the MDC's start-up phases */
#define MDC_STARTUP_POWER_ON          0
#define MDC_STARTUP_INIT_MECHANICS    2
#define MDC_STARTUP_RUN_4             4
#define MDC_STARTUP_RUN_6             6
#define MDC_STARTUP_RESTART_MECHANICS 8
#define MDC_STARTUP_HOLD              0x0C
#define MDC_STARTUP_HOLD_2            0x0D
#define MDC_STARTUP_HOLD_3            0x0E
#define MDC_STARTUP_RUN               0x10

/* Other mailbox values: the first version byte of the boot program ("BA00"), the imprinter fitted */
#define MDC_VERSION_BOOT   0x42
#define MDC_OPTION_PRESENT 0x80

/* A byte with its views: the byte, its bits (b7 = MSB), and bit-fields f_<lowest bit>_<width>. */
union byte_view {
    unsigned char b;
    struct { unsigned char b7:1, b6:1, b5:1, b4:1, b3:1, b2:1, b1:1, b0:1; } bit;
    struct { unsigned char f:8; } f_0_8;
    struct { unsigned char :1, f:7; } f_0_7;
    struct { unsigned char :4, f:4; } f_0_4;
    struct { unsigned char :5, f:3; } f_0_3;
    struct { unsigned char :6, f:2; } f_0_2;
    struct { unsigned char :4, f:2, :2; } f_2_2;
    struct { unsigned char :3, f:2, :3; } f_3_2;
    struct { unsigned char :2, f:2, :4; } f_4_2;
    struct { unsigned char f:4, :4; } f_4_4;
    struct { unsigned char f:3, :5; } f_5_3;
};

/* A 16-bit word with its bits (b15 = MSB), as a 16-bit field, and its two bytes. */
union word_view {
    unsigned short w;
    struct { unsigned short b15:1, b14:1, b13:1, b12:1, b11:1, b10:1, b9:1, b8:1,
                            b7:1, b6:1, b5:1, b4:1, b3:1, b2:1, b1:1, b0:1; } bit;
    struct { unsigned short f:16; } f_0_16;
    struct { unsigned char h, l; } byte;
};

/* A 32-bit value with its words and bytes. */
union long_view {
    unsigned long l;
    struct { unsigned short h, l; } w;
    unsigned char b[4];
};

/* ---- RAM 0x200100-0x2004FF: one struct. pad_XXX are the bytes at offset 0xXXX (0x200100 + XXX)
 * that no member names. ---- */
union mailbox_cmd_params {                        /* 0x200440-0x20044F */
    unsigned char b[16];
    struct mailbox_mem_params {
        unsigned char *address;
        unsigned short count;
        unsigned short pad_06;
        unsigned char data[8];
    } memory;
};

/* Mailbox bytes with bits in use: one type each, so that a bit is named once (as the flag words of
 * mdc.h; members bN by position where a bit has no name, bits no code touches unnamed; the masks
 * are the MDC_SENSOR_ / MDC_MULTIFEED_ / MDC_LAMP_STATUS_ / MDC_FEED_ / MDC_MOTION_ / MDC_CAPTURE_ /
 * MDC_PAGE_END_ ... groups of mdc.h). feed_options.length_tolerance is MDC_FEED_LENGTH_TOLERANCE,
 * read as that mask (an index). lamp_status.adf_back_lamp is filled from P2.0, the front lamp, not
 * from P2.2 (tick.c). */
union mailbox_sensor_status_bits {
    unsigned char b;
    struct { unsigned char pick_arm:1, pick:1, backing:1, top:1, no_paper:1, adf_cover_open:1, document_cover_open:1, fb_home:1; } bit;
};
union mailbox_multifeed_flags_bits {
    unsigned char b;
    struct { unsigned char imprinter:1, :2, b4:1, length:1, ultrasonic:1, :2; } bit;
};
union mailbox_lamp_status_bits {
    unsigned char b;
    struct { unsigned char adf_front_lamp:1, adf_back_lamp:1, flatbed_lamp:1, :3, keep_warm:1, :1; } bit;
};
union mailbox_feed_options_bits {
    unsigned char b;
    struct { unsigned char prepick:1, multifeed_report_only:1, :1, ultrasonic_check:1, length_check:1, job_separation:1, length_tolerance:2; } bit;
};
union mailbox_motion_flags_bits {
    unsigned char b;
    struct { unsigned char :4, back_pending:1, at_top:1, back:1, front:1; } bit;
};
union mailbox_capture_flags_bits {
    unsigned char b;
    struct { unsigned char :6, steps_valid:1, paused:1; } bit;
};
union mailbox_page_end_bits {
    unsigned char b;
    struct { unsigned char :5, accepted:1, done:1, request:1; } bit;
};
union mailbox_sensor_status_2_bits {
    unsigned char b;
    struct { unsigned char df:1, ultrasonic:1, :5, manual_feed_empty:1; } bit;
};
union mailbox_cover_open_event_bits {
    unsigned char b;
    struct { unsigned char adf_cover_opened:1, :7; } bit;
};
union mailbox_separation_sheet_flags_bits {
    unsigned char b;
    struct { unsigned char detected:1, :7; } bit;
};
union mailbox_df_test_result_bits {
    unsigned char b;
    struct { unsigned char top_edge:1, :7; } bit;
};
union mailbox_warmup_output_bits {
    unsigned char b;
    struct { unsigned char :7, on:1; } bit;
};
union mailbox_fb_motor_status_bits {
    unsigned char b;
    struct { unsigned char moving:1, :6, at_home:1; } bit;
};
union mailbox_imprinter_flags_bits {
    unsigned char b;
    struct { unsigned char :4, not_installed:1, :3; } bit;
};
/* err_flags (mdc.h), its mailbox copy error_flags and struct imprinter_error.flags (copied whole into
 * err_flags): the masks are ERR_FLAG_DEVICE / ERR_FLAG_DOCUMENT of mdc.h. */
union err_flags_bits {
    unsigned char b;
    struct { unsigned char :3, device:1, document:1, :3; } bit;
};

struct mailbox_area {
    unsigned char handshake_high;
    unsigned char handshake_low;
    unsigned char startup_state;
    unsigned char startup_request;
    unsigned char info_table_valid;
    unsigned char version_0;
    unsigned char version_1;
    unsigned char version_2;
    unsigned char version_3;
    unsigned char info_table[32];
    unsigned char download_cmd;
    unsigned char download_len_high;
    unsigned char download_len_low;
    unsigned char download_offset_3;
    unsigned char download_offset_2;
    unsigned char download_offset_1;
    unsigned char download_offset_0;
    unsigned char option_present;
    unsigned char pad_031[1];
    unsigned char board_variant;
    unsigned char pad_033[23];
    unsigned char settings_0a;
    unsigned char settings_0b;
    unsigned char pad_04c[28];
    unsigned char settings_28;
    unsigned char settings_29;
    unsigned char pad_06a[30];
    unsigned char settings_48;
    unsigned char pad_089[22];
    unsigned char settings_5f;
    unsigned char pad_0a0[22];
    unsigned char overscan_control;
    unsigned char pad_0b7[9];
    unsigned char settings_80;
    unsigned char pad_0c1[12];
    unsigned char settings_8d;
    unsigned char settings_8e;
    unsigned char settings_8f;
    unsigned char settings_90;
    unsigned char pad_0d1[5];
    unsigned char settings_96;
    unsigned char pad_0d7[5];
    unsigned char settings_9c;
    unsigned char settings_9d;
    unsigned char pad_0de[8];
    unsigned char settings_a6;
    unsigned char pad_0e7[3];
    unsigned char settings_aa;
    unsigned char settings_ab;
    unsigned char settings_ac;
    unsigned char settings_ad;
    unsigned char pad_0ee[1];
    unsigned char settings_af;
    unsigned char pad_0f0[1];
    unsigned char non_rectangular;
    unsigned char pad_0f2[1];
    unsigned char settings_b3;
    unsigned char pad_0f4[7];
    unsigned char settings_bb;
    unsigned char pad_0fc[2];
    unsigned char settings_be;
    unsigned char pad_0ff[1];
    unsigned char settings_c0;
    unsigned char pad_101[4];
    unsigned char settings_c5;
    unsigned char pad_106[5];
    unsigned char settings_cb;
    unsigned char pad_10c[3];
    unsigned char manual_feed_timeout;
    unsigned char pad_110[4];
    unsigned char settings_d4;
    unsigned char pad_115[5];
    unsigned char settings_da;
    unsigned char settings_db;
    unsigned char pad_11c[255];
    unsigned char ultrasonic_scale;
    unsigned char pad_21c[4];
    unsigned char feed_debug_flags;
    unsigned char pick_ramp_steps_override;
    unsigned char pad_222[30];
    unsigned char ready;
    unsigned char pad_241[2];
    union mailbox_sensor_status_bits sensor_status;
    union mailbox_sensor_status_2_bits sensor_status_2;
    union mailbox_multifeed_flags_bits multifeed_flags;
    unsigned char multifeed_flags_imprinter;
    unsigned char pad_247[1];
    union err_flags_bits error_flags;
    unsigned char error_code;
    unsigned char byte_0x34a;
    union mailbox_separation_sheet_flags_bits separation_sheet_flags;
    unsigned char sheet_count_high;
    unsigned char sheet_count_low;
    unsigned char sheet_length_high;
    unsigned char sheet_length_low;
    unsigned char byte_0x350;
    unsigned char byte_0x351;
    union mailbox_lamp_status_bits lamp_status;
    unsigned char pad_253[7];
    unsigned char top_to_front_distance_high;
    unsigned char top_to_front_distance_low;
    unsigned char top_to_back_distance_high;
    unsigned char top_to_back_distance_low;
    unsigned char byte_0x35e;
    unsigned char byte_0x35f;
    unsigned char backing_position;
    unsigned char byte_0x361;
    union mailbox_fb_motor_status_bits fb_motor_status;
    union mailbox_warmup_output_bits warmup_output;
    unsigned char lamp_temperature_front;
    unsigned char lamp_temperature_back;
    unsigned char lamp_temperature_flatbed;
    union mailbox_capture_flags_bits capture_flags;
    unsigned char byte_0x368;
    unsigned char lamp_temperature;
    unsigned char mechanics_init_mode;
    unsigned char lamp_temperature_b;
    unsigned char pad_26c[1];
    union mailbox_imprinter_flags_bits imprinter_flags;
    unsigned char imprinter_ink_percent;
    unsigned char lamp_off;
    unsigned char pad_270[1];
    unsigned char imprinter_version[4];
    unsigned char pad_275[1];
    unsigned char capture_steps_high;
    unsigned char capture_steps_low;
    unsigned char df_adjust_flags;
    union mailbox_cover_open_event_bits cover_open_event;
    unsigned char pad_27a[42];
    unsigned char backing_trace_code;
    unsigned char backing_trace_mode;
    unsigned char backing_trace_request;
    unsigned char backing_trace_state;
    unsigned char backing_trace_steps;
    unsigned char pad_2a9[1];
    unsigned char backing_trace_timeout;
    unsigned char pad_2ab[1];
    unsigned char sheet_gap_error_count;
    unsigned char job_trace_code;
    unsigned char job_page_limit_high;
    unsigned char job_page_limit_low;
    unsigned char job_page_count_high;
    unsigned char job_page_count_low;
    unsigned char page_count_limit_copy_high;
    unsigned char page_count_limit_copy_low;
    unsigned char pad_2b4[5];
    unsigned char ultrasonic_timeout_count;
    unsigned char cover_open_top_count;
    unsigned char pad_2bb[1];
    unsigned char gap_error_both_count;
    unsigned char path_empty_count;
    unsigned char gap_error_front_count;
    unsigned char gap_error_stopped_count;
    unsigned char source;
    unsigned char speed_class_a;
    unsigned char speed_class_b;
    union mailbox_feed_options_bits feed_options;
    unsigned char max_sheet_length_high;
    unsigned char max_sheet_length_low;
    unsigned char byte_0x3c6;
    unsigned char feed_backing_request;
    unsigned char long_page;
    unsigned char lamp_off_minutes;
    unsigned char lead_front_low;
    unsigned char lead_back_low;
    unsigned char lead_flatbed_low;
    unsigned char byte_0x3cd;
    unsigned char pad_2ce[3];
    unsigned char flatbed_scan_length_high;
    unsigned char flatbed_scan_length_low;
    unsigned char jam_limit_high;
    unsigned char jam_limit_low;
    unsigned char gap_extension;
    unsigned char pad_2d6[2];
    unsigned char lead_front_high;
    unsigned char lead_back_high;
    unsigned char lead_flatbed_high;
    unsigned char last_page;
    unsigned char page_count_limit_high;
    unsigned char page_count_limit_low;
    unsigned char multifeed_bypass;
    unsigned char pad_2df[1];
    unsigned char end_lead_front_low;
    unsigned char end_lead_front_high;
    unsigned char end_lead_back_low;
    unsigned char end_lead_back_high;
    unsigned char wake_flags;
    union mailbox_page_end_bits page_end;
    unsigned char pad_2e6[90];
    union mailbox_cmd_params cmd_params;
    unsigned char cmd_end;
    unsigned char manual_feed_end;
    unsigned char pad_352[3];
    union byte_view log_flags;
    unsigned char log_size_high;
    unsigned char log_size_low;
    unsigned char dump_count;
    unsigned char pad_359[39];
    unsigned char top_window_on_steps_high;
    unsigned char top_window_on_steps_low;
    unsigned char pad_382[110];
    union mailbox_motion_flags_bits motion_flags;
    unsigned char cmd_state;
    unsigned char cmd_code;
    unsigned char gap_extension_flags;
    unsigned char pad_3f4[9];
    union mailbox_df_test_result_bits df_test_result;
    unsigned char cmd_result;
    unsigned char request;
};

/* ---- External RAM and devices, 0x200000 up ---- */
extern struct mailbox_area mailbox;

#endif /* SHARED_H */
