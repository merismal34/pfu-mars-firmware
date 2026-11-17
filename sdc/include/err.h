/* err.h: declarations of err.c. */
#ifndef ERR_H
#define ERR_H
#include "common.h"

/* error codes (err_raise, err_last_code, mdc_mailbox.error_code; the index of err_class_table / err_display_codes / err_count_offsets; the operator's guide's codes) */
#define ERR_ULTRASONIC_SENSOR       0x02
#define ERR_DF_SENSOR               0x03
#define ERR_SHIPPING_LOCK           0x10
#define ERR_NO_PAPER                0x20
#define ERR_PAUSED_BY_USER          0x24
#define ERR_JAM                     0x31
#define ERR_EJECT_JAM               0x36
#define ERR_JAM_DIRTY_ROLLERS       0x39
#define ERR_PICK_SENSOR_JAM         0x3A
#define ERR_ADF_COVER_OPEN          0x40
#define ERR_ADF_COVER_OPEN_2        0x4A
#define ERR_PICK                    0x50
#define ERR_MULTIFEED_LENGTH        0x56
#define ERR_SHEET_GAP               0x57
#define ERR_MULTIFEED_ULTRASONIC    0x58
#define ERR_OPTICAL_ADF_FRONT       0x74
#define ERR_OPTICAL_ADF_BACK        0x75
#define ERR_IMAGE_TRANSFER          0x77
#define ERR_STOPPED                 0x78
#define ERR_OPTICAL_FLATBED         0x7E
#define ERR_MOTOR_FUSE              0x81
#define ERR_LAMP_FUSE               0x84
#define ERR_LAMP_FUSE_2             0x85
#define ERR_FLATBED_MOTOR           0x87
#define ERR_CGA_BOARD               0x8A
#define ERR_HEATER_ADF_FRONT        0x92
#define ERR_HEATER_ADF_BACK         0x93
#define ERR_HEATER_FLATBED          0x94
#define ERR_IMPRINTER_SYSTEM        0xB1
#define ERR_IMPRINTER_CONTROL_BOARD 0xB2
#define ERR_IMPRINTER_TIMEOUT       0xB3
#define ERR_IMPRINTER_NOT_INSTALLED 0xB4
#define ERR_IMPRINTER_HEAD          0xB5
#define ERR_IMPRINTER_EEPROM        0xB6
#define ERR_IMPRINTER_ROM           0xB8
#define ERR_IMPRINTER_AREA          0xB9
#define ERR_HOPPER                  0xC0
#define ERR_BACKING                 0xC2
#define ERR_PICK_ARM                0xC4
#define ERR_VDCC                    0xE9
#define ERR_PISCES                  0xEA
#define ERR_FAN                     0xEC
#define ERR_MDC_TIMEOUT             0xF0
#define ERR_IMAGE_MEMORY_FRONT      0xF5
#define ERR_IMAGE_MEMORY_BACK       0xF6
#define ERR_IMAGE_MEMORY_EXT_FRONT  0xF7
#define ERR_IMAGE_MEMORY_EXT_BACK   0xF8

struct err_sense_entry {
    unsigned short error_code;
    char sense_key;
    char c;
    short asc_ascq;
};

/* functions */
extern int err_check_pending(void);
extern void err_raise(unsigned char);
extern void err_raise_step_4(unsigned char code);
extern void err_add_to_list(unsigned char code);
extern int err_report_page_error(void);
extern void err_set_illegal_request(short);
extern void err_set_illegal_request_detail(short bits, unsigned char detail_high, unsigned char detail_low);
extern void err_set_illegal_request_at();
extern void err_set_medium_error(unsigned short bits, unsigned char display_main, unsigned char display_sub);
extern void err_set_hardware_error(unsigned short bits, unsigned char display_main, unsigned char display_sub);
extern void err_abort_page();
extern int err_get_class(unsigned char code);

/* constants */
extern const struct err_sense_entry err_hardware_sense_table[13];
extern const struct err_sense_entry err_medium_sense_table[11];
extern const int err_class_table[256];
extern const unsigned char err_display_codes[256];
extern const unsigned short err_count_offsets[256];

#endif
