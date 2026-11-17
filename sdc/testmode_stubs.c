#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);

void testmode_unused_01(char unused_arg) {}
void testmode_unused_02(void) {}
void testmode_unused_03(void) {}
void testmode_unused_04(void) {}
void testmode_unused_05(void) {}
void testmode_unused_06(void) {}
void testmode_unused_07(void) {}
void testmode_unused_08(void) {}
void testmode_unused_09(void) {}
void testmode_unused_10(void) {}
void testmode_unused_11(int unused_arg) {}
void testmode_unused_12(int unused_arg) {}
void testmode_unused_13(int unused_arg, short unused_arg_2, short unused_arg_3) {}
void testmode_unused_14(int unused_arg, short unused_arg_2, short unused_arg_3) {}
void testmode_unused_15(int unused_arg, short unused_arg_2, short unused_arg_3, int unused_arg_4) {}
void testmode_unused_16(int unused_arg, char unused_arg_2) {}
void testmode_unused_17(short unused_arg, short unused_arg_2) {}
void testmode_unused_18(short unused_arg, short unused_arg_2) {}
void testmode_unused_19(void) {}
void testmode_unused_20(void) {}
void testmode_unused_21(void) {}
void testmode_unused_22(void) {}
void testmode_unused_23(void) {}
void testmode_unused_24(void) {}
void testmode_unused_25(void) {}
void testmode_unused_26(void) {}
void testmode_unused_27(void) {}
void testmode_unused_28(void) {}
void testmode_unused_29(void) {}
void testmode_unused_30(void) {}
void testmode_unused_31(char unused_arg) {}
void testmode_unused_32(char unused_arg) {}
void testmode_unused_33(void) {}
void testmode_unused_34(void) {}
void testmode_unused_35(void) {}
void testmode_unused_36(void) {}
void testmode_unused_37(void) {}
void testmode_unused_38(void) {}
void testmode_unused_39(void) {}
void testmode_unused_40(short unused_arg) {}
void testmode_unused_41(void) {}
void testmode_unused_42(void) {}
void testmode_wait_lines(unsigned short lines)
{
    unsigned short irq_count;

    do {
        irq_count = line_irq_count;
        _builtin_set_imask(0);
        while (irq_count == line_irq_count)
            ;
    } while (--lines);
}
void testmode_unused_43(void) {}
void testmode_unused_44(void) {}
void testmode_unused_45(void) {}
void testmode_unused_46(void) {}
void testmode_unused_47(void) {}
void testmode_unused_48(void) {}
void testmode_unused_49(void) {}
void testmode_unused_50(void) {}
void testmode_unused_51(void) {}
void testmode_unused_52(void) {}
void testmode_unused_53(void) {}
void testmode_unused_54(void) {}
void testmode_unused_55(void) {}
void testmode_unused_56(void) {}
void testmode_unused_57(void) {}
void testmode_unused_58(void) {}
void testmode_unused_59(char unused_arg) {}
void testmode_unused_60(void) {}
void testmode_unused_61(void) {}
void testmode_unused_62(void) {}
void testmode_unused_63(int unused_arg) {}
void testmode_unused_64(int unused_arg) {}
void testmode_unused_65(int unused_arg, int unused_arg_2) {}
void testmode_unused_66(char unused_arg, char unused_arg_2) {}
void testmode_unused_67(void) {}
void testmode_unused_68(void) {}
void testmode_unused_69(int unused_arg) {}
void testmode_unused_70(void) {}
void testmode_unused_71(void) {}
void testmode_unused_72(void) {}
void testmode_unused_73(void) {}
void testmode_01(void) { err_code_list[0] = 255; testmode_result_1[0] = 255; }
void testmode_unused_74(void) {}
void testmode_02(void) { err_code_list[0] = 128; }
void testmode_unused_75(void) {}
