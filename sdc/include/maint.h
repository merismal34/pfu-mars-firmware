/* maint.h: declarations of maint_main.c, maint.c. */
#ifndef MAINT_H
#define MAINT_H
#include "common.h"

/* maint_screen: the screen of the maintenance mode in progress */
#define MAINT_SCREEN_SELECT      0
#define MAINT_SCREEN_ITEM        1
#define MAINT_SCREEN_ADJUSTED    2
#define MAINT_SCREEN_CONFIRM     3
#define MAINT_SCREEN_WRITING     4
#define MAINT_SCREEN_WRITTEN     5
#define MAINT_SCREEN_FAILED      6
#define MAINT_SCREEN_RUNNING     16
#define MAINT_SCREEN_ERROR_1     0xE1
#define MAINT_SCREEN_SAVE_EEPROM 0xFF
/* maint_mode_word.mode: the maintenance mode (service manual 6.1; index of maint_mode_table) */
#define MAINT_MODE_FEED_SENSOR_TEST  1
#define MAINT_MODE_SUB_MAGNIFICATION 2
#define MAINT_MODE_OFFSET            3
#define MAINT_MODE_WHITE_LEVEL       4
#define MAINT_MODE_ROLLER_COUNTERS   5
#define MAINT_MODE_INFORMATION       6
#define MAINT_MODE_EEPROM_RESTORE    7
#define MAINT_MODE_ULTRASONIC        8
#define MAINT_MODE_EMULATION         10

struct testmode_entry {
    unsigned char key;
    char pad_01[3];
    void (*handler) (void);
};

/* functions */
extern void maint_start(void);
extern void maint_run(void);
extern void maint_end_mode(void);
extern void maint_run_mode_1(void);
extern void maint_mode_1_state_0(void);
extern void maint_mode_1_state_1(void);
extern void maint_mode_1_state_2(void);
extern void maint_mode_1_state_3(void);
extern void maint_mode_1_state_4(void);
extern void maint_mode_1_state_5(void);
extern void maint_mode_1_state_6(void);
extern void maint_mode_1_state_7(void);
extern void maint_run_mode_2(void);
extern void maint_mode_2_state_0(void);
extern void maint_mode_2_state_1(void);
extern void maint_mode_2_state_2(void);
extern void maint_mode_2_state_3(void);
extern void maint_mode_2_state_4(void);
extern void maint_mode_2_state_5(void);
extern void maint_mode_2_set_settings(int action);
extern char maint_read_sdram_byte(int side, int byte_offset, char *window, int base_64k);
extern int maint_find_rising_edge(unsigned char *samples, int len);
extern int maint_mode_2_state_3_step_2(void);
extern int maint_mode_2_state_3_step_3(void);
extern void maint_run_mode_3(void);
extern void maint_mode_3_state_0(void);
extern void maint_mode_3_state_1(void);
extern void maint_mode_3_state_2(void);
extern void maint_mode_3_state_3(void);
extern void maint_mode_3_state_4(void);
extern void maint_mode_3_state_5(void);
extern void maint_mode_3_set_settings(int action);
extern int maint_mode_3_state_3_step_2(void);
extern int maint_mode_3_state_3_step_3(void);
extern void maint_run_mode_4(void);
extern void maint_mode_4_state_0(void);
extern void maint_mode_4_state_1(void);
extern void maint_mode_4_state_2(void);
extern void maint_mode_4_state_3(void);
extern void maint_mode_4_state_4(void);
extern void maint_mode_4_state_5(void);
extern void maint_adjust_white_level(int action);
extern int maint_mode_4_state_3_step_2(void);
extern int maint_save_settings(unsigned char);
extern unsigned char maint_mode_4_state_3_step_2_1(unsigned char chip, unsigned char white_level_target);
extern void maint_run_mode_5(void);
extern void maint_mode_5_state_0(void);
extern void maint_mode_5_state_1(void);
extern void maint_mode_5_state_2(void);
extern void maint_mode_5_state_3(void);
extern int maint_mode_5_state_1_step_2(unsigned char item);
extern void maint_run_mode_6(void);
extern void maint_mode_6_state_0(void);
extern void maint_mode_6_state_1(void);
extern void maint_mode_6_state_2(void);
extern void maint_mode_6_state_3(void);
extern void maint_run_mode_7(void);
extern void maint_mode_7_state_0(void);
extern void maint_mode_7_state_1(void);
extern void maint_mode_7_state_2(void);
extern void maint_mode_7_state_3(void);
extern void maint_run_mode_8(void);
extern void maint_mode_8_state_0(void);
extern void maint_mode_8_state_1(void);
extern void maint_mode_8_state_2(void);
extern void maint_mode_8_state_3(void);
extern void maint_run_mode_10(void);
extern void maint_mode_10_state_0(void);
extern void maint_mode_10_state_1(void);
extern void maint_mode_10_state_2(void);
extern void maint_mode_10_state_3(void);
extern int maint_mode_10_state_1_step_2(unsigned char item);
extern void maint_run_mode_11(void);
extern void maint_unused_01(void);
extern void maint_unused_02(void);
extern void maint_unused_03(void);
extern void maint_unused_04(void);
extern void maint_unused_05(void);
extern void maint_unused_06(void);
extern void maint_unused_07(void);
extern void maint_unused_08(void);
extern void maint_unused_09(void);
extern void maint_unused_10(void);
extern void maint_unused_11(void);
extern void maint_unused_12(void);
extern void maint_unused_13(void);
extern void maint_unused_14(void);
extern void maint_unused_15(void);
extern void maint_unused_16(void);
extern void maint_unused_17(void);
extern void maint_unused_18(void);
extern void maint_unused_19(void);

/* constants */
extern const struct testmode_entry testmode_table[31];
extern void (*maint_mode_table[16]) (void);
extern void (*maint_mode_1_states[16]) (void);
extern void (*maint_mode_2_states[16]) (void);
extern int (*maint_mode_3_states[16]) (void);
extern void (*maint_mode_4_states[16]) (void);
extern void (*maint_mode_5_states[16]) ();
extern void (*maint_mode_6_states[16]) ();
extern void (*maint_mode_7_states[16]) ();
extern void (*maint_mode_8_states[16]) ();
extern void (*maint_mode_10_states[16]) (void);

/* I/O */
extern unsigned char mdc_settings_8f[];

#endif
