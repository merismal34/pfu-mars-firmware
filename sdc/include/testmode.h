/* testmode.h: declarations of testmode_stubs.c, testmode_board.c, testmode_exposure.c,
   testmode_adjust.c, testmode_mechanics.c, testmode_scan.c and 1 more files. */
#ifndef TESTMODE_H
#define TESTMODE_H
#include "common.h"

/* functions */
extern void testmode_unused_01(char unused_arg);
extern void testmode_unused_02(void);
extern void testmode_unused_03(void);
extern void testmode_unused_04(void);
extern void testmode_unused_05(void);
extern void testmode_unused_06(void);
extern void testmode_unused_07(void);
extern void testmode_unused_08(void);
extern void testmode_unused_09(void);
extern void testmode_unused_10(void);
extern void testmode_unused_11(int unused_arg);
extern void testmode_unused_12(int unused_arg);
extern void testmode_unused_13(int unused_arg, short unused_arg_2, short unused_arg_3);
extern void testmode_unused_14(int unused_arg, short unused_arg_2, short unused_arg_3);
extern void testmode_unused_15(int unused_arg, short unused_arg_2, short unused_arg_3, int unused_arg_4);
extern void testmode_unused_16(int unused_arg, char unused_arg_2);
extern void testmode_unused_17(short unused_arg, short unused_arg_2);
extern void testmode_unused_18(short unused_arg, short unused_arg_2);
extern void testmode_unused_19(void);
extern void testmode_unused_20(void);
extern void testmode_unused_21(void);
extern void testmode_unused_22(void);
extern void testmode_unused_23(void);
extern void testmode_unused_24(void);
extern void testmode_unused_25(void);
extern void testmode_unused_26(void);
extern void testmode_unused_27(void);
extern void testmode_unused_28(void);
extern void testmode_unused_29(void);
extern void testmode_unused_30(void);
extern void testmode_unused_31(char unused_arg);
extern void testmode_unused_32(char unused_arg);
extern void testmode_unused_33(void);
extern void testmode_unused_34(void);
extern void testmode_unused_35(void);
extern void testmode_unused_36(void);
extern void testmode_unused_37(void);
extern void testmode_unused_38(void);
extern void testmode_unused_39(void);
extern void testmode_unused_40(short unused_arg);
extern void testmode_unused_41(void);
extern void testmode_unused_42(void);
extern void testmode_wait_lines(unsigned short);
extern void testmode_unused_43(void);
extern void testmode_unused_44(void);
extern void testmode_unused_45(void);
extern void testmode_unused_46(void);
extern void testmode_unused_47(void);
extern void testmode_unused_48(void);
extern void testmode_unused_49(void);
extern void testmode_unused_50(void);
extern void testmode_unused_51(void);
extern void testmode_unused_52(void);
extern void testmode_unused_53(void);
extern void testmode_unused_54(void);
extern void testmode_unused_55(void);
extern void testmode_unused_56(void);
extern void testmode_unused_57(void);
extern void testmode_unused_58(void);
extern void testmode_unused_59(char unused_arg);
extern void testmode_unused_60(void);
extern void testmode_unused_61(void);
extern void testmode_unused_62(void);
extern void testmode_unused_63(int unused_arg);
extern void testmode_unused_64(int unused_arg);
extern void testmode_unused_65(int unused_arg, int unused_arg_2);
extern void testmode_unused_66(char unused_arg, char unused_arg_2);
extern void testmode_unused_67(void);
extern void testmode_unused_68(void);
extern void testmode_unused_69(int unused_arg);
extern void testmode_unused_70(void);
extern void testmode_unused_71(void);
extern void testmode_unused_72(void);
extern void testmode_unused_73(void);
extern void testmode_01(void);
extern void testmode_unused_74(void);
extern void testmode_02(void);
extern void testmode_unused_75(void);
extern void testmode_panel_check(void);
extern void testmode_04(void);
extern void testmode_05(void);
extern void testmode_06(void);
extern void testmode_read_mdc_status(void);
extern void testmode_read_scsi_id(void);
extern void testmode_lsi_memories(void);
extern void testmode_set_result(unsigned short result);
extern void testmode_0a(void);
extern void testmode_read_exposure(void);
extern void testmode_0f(void);
extern void testmode_10(void);
extern short testmode_adjust_offset(void);
extern void testmode_adjust_magnification(void);
extern void testmode_adjust_white_level(void);
extern void testmode_adjust_ultrasonic(void);
extern void testmode_panel_buttons(void);
extern void testmode_26(void);
extern void testmode_27(void);
extern void testmode_28(void);
extern void testmode_lamp(void);
extern void testmode_2a(void);
extern void testmode_30(void);
extern void testmode_pick_arm(void);
extern void testmode_2c(void);
extern void testmode_save_defect_lists(void);
extern void testmode_save_defect_lists_no_result(void);
extern void testmode_15(void);
extern void testmode_save_settings_backup(void);
extern void testmode_save_eeprom_data(void);

/* constants */
extern unsigned char flash_backup_slot_used[];
extern unsigned char flash_backup_slot_current[];

union word_bytes { unsigned short w; unsigned char b[2]; };

#endif
