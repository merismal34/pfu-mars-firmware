#include "fw.h"
#pragma section H
void hook_unused_01(short unused_arg) {}
void hook_unused_02(short unused_arg) {}
void hook_unused_03(short unused_arg) {}
void hook_unused_04(short unused_arg) {}
void hook_unused_05(short unused_arg, short unused_arg_2) {}
void hook_unused_06(short unused_arg) {}
void hook_unused_07(short unused_arg) {}
void hook_unused_08(short unused_arg) {}
void hook_unused_09(int unused_arg) {}
void hook_unused_10(int unused_arg) {}
void hook_power_save_2(void) {}
void hook_unused_11(void) {}
void hook_unused_12(void) {}
void hook_unused_13(void) {}
void hook_unused_14(void) {}
void hook_unused_15(void) {}
void hook_unused_16(void) {}
void hook_power_save_1(void) {}
void hook_unused_17(void) {}
void hook_unused_18(void) {}
int hook_is_adf_empty(void)
{
    if (!mdc_mailbox.sensor_status.f.no_paper || mdc_mailbox.sensor_status.f.top || mdc_mailbox.sensor_status.f.pick)
        return 0;
    return 1;
}
