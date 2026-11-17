#include "mdc.h"

void sensor_set_ultrasonic_sampling(unsigned char triggered)
{
    unsigned char u;

    if (sensor_df_test) return;
    if (triggered) {
        p_chip_a_sensor->control.bit.b6 = 0;
        hw_set_ad_mode(AD_ULTRASONIC_TRIGGERED);
        multifeed_check.threshold_sum = multifeed_check.threshold;
        multifeed_check.threshold_sum = multifeed_check.threshold_sum << 2;
        multifeed_check.threshold_scaled_sum = multifeed_check.threshold_scaled << 2;
        p_chip_a_sensor->control.bit.b6 = 1;
    } else {
        p_chip_a_sensor->control.bit.b6 = 0;
        hw_set_ad_mode(AD_ULTRASONIC);
        sensor_word_1 = 0;
    }
}

void sensor_stop_ultrasonic(void)
{
    p_chip_a_sensor->control.bit.b6 = 0;
    if (sensor_df_test) return;
    if (pick_df_armed == 0 && pick_df_active == 0) hw_set_ad_mode(AD_SCAN_AN0_3);
    sensor_word_1 = 0;
}

void sensor_df_on(void)
{
    unsigned char u;

    p_da->DR1 = sensor_df_level;
    p_chip_b->outputs.b &= 0xfc;
    p_chip_b->outputs.b |= sensor_df_range;
    hw_set_ad_mode(AD_DF);
}

void sensor_df_off(void)
{
    hw_set_ad_mode(AD_SCAN_AN0_3);
}

void sensor_hook(unsigned char unused_arg, unsigned char unused_arg_2)
{
}
