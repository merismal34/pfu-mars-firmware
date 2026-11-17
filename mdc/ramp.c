#include "mdc.h"

void ramp_load(void)
{
    unsigned short i;
    unsigned short period;
    unsigned char *p;
    unsigned char *q;
    unsigned char *high_dst;
    unsigned char *low_dst;

    p = p_chip_a_ramp;
    for (i = 0; i < 0x7a; i++) {
        period = ramp_feed_accel[i] - 1;
        *p++ = (unsigned char)(period >> 8);
        *p++ = (unsigned char)period;
    }
    q = p_chip_b_ramp;
    for (i = 0; i < 0x64; i++) {
        period = ramp_pick_accel[i] - 1;
        *q++ = (unsigned char)(period >> 8);
        *q++ = (unsigned char)period;
    }
    for (i = 0; i < 0x1c; i++) {
        period = ramp_pick_brake[i] - 1;
        *q++ = (unsigned char)(period >> 8);
        *q++ = (unsigned char)period;
    }
    feed_fast_ramp = 0;
    pick_fast_next = 0;
    high_dst = ramp_fb_motor_high;
    low_dst = ramp_fb_motor_low;
    for (i = 0; i <= 0x17b; i++) {
        period = ramp_fb_motor_accel[i] - 1;
        *high_dst++ = (unsigned char)(period >> 8);
        *low_dst++ = (unsigned char)period;
    }
    high_dst = ramp_fb_motor_half_high;
    low_dst = ramp_fb_motor_half_low;
    for (i = 0; i <= 0x17b; i++) {
        period = ramp_fb_motor_accel[i] - 1;
        period >>= 1;
        *high_dst++ = (unsigned char)(period >> 8);
        *low_dst++ = (unsigned char)period;
    }
}

void ramp_build_feed(void)
{
    unsigned short i;
    unsigned short flag;
    unsigned short record;
    unsigned long period;
    unsigned long divisor;
    unsigned long temperature_correction;
    unsigned long speed_step;
    unsigned char *p;
    unsigned long base_period;
    unsigned char temperature;

    temperature = sensor_temperature_b;
    if (temperature > 0x40) {
        temperature = 0x40;
    }
    temperature_correction = ramp_temperature_correction[temperature];
    if (temperature > 0x40) {
        flag = 0;
    } else {
        flag = 1;
    }
    if (mailbox.settings_af & 2) {
        flag = 1;
        temperature_correction = 0;
    }
    set_imask_exr(7);
    p_chip_a_motor->control_1.f_0_2.f = 0;
    p_chip_a_motor->control_2.f_0_2.f = 0;
    set_imask_exr(feed_saved_imask);
    p = p_chip_a_ramp;
    if (feed_fast_ramp == 1) {
        for (i = 0; i < 0xe0; i++) {
            period = ramp_feed_fast_accel[i] - 1;
            base_period = ramp_feed_fast_accel[i] - 1;
            record = 0;
            switch (i) {
            case 0x8a:
            case 0x8d:
            case 0xdf:
                if (job_feed_speed_class == 3) {
                    if (i == 0x8a) {
                        base_period = period = ramp_feed_fast_class_3_period - 1;
                    } else {
                        break;
                    }
                } else if (job_feed_speed_class == 2) {
                    if (i == 0x8d) {
                        base_period = period = ramp_feed_fast_class_2_period - 1;
                    } else {
                        break;
                    }
                } else {
                    if (i != 0xdf) {
                        break;
                    }
                }

                record++;
                period = period * 1000;
                if (mailbox.settings_0a & 0x80) {
                    divisor = (long)(mailbox.settings_0a & 0x3f) + 1000;
                } else {
                    divisor = 1000 - (long)(mailbox.settings_0a & 0x3f);
                }
                period = period / divisor;
                period = period * 1000;
                if (mailbox.settings_da & 0x80) {
                    divisor = (long)(mailbox.settings_da & 0x1f) + 1000;
                } else {
                    divisor = 1000 - (long)(mailbox.settings_da & 0x1f);
                }
                period = period / divisor;
                period = period * 10000;
                if (flag) {
                    divisor = temperature_correction + 10000;
                } else {
                    divisor = 10000 - temperature_correction;
                }
                period = period / divisor;
                divisor = period;
                period = period * 1000;
                period = period / 0x402;
                break;
            default:
                break;
            }

        if (feed_mode.b & FEED_MODE_HALF_STEP) {
            period = period >> 1;
        }
        *p++ = (unsigned char)(period >> 8);
        *p++ = (unsigned char)period;
        if (record) {
            record--;
            ramp_feed_speed_records[record].b[0] = (unsigned char)(period >> 8);
            ramp_feed_speed_records[record].b[1] = (unsigned char)period;
            ramp_feed_speed_records[record].b[8] = (unsigned char)(divisor >> 8);
            ramp_feed_speed_records[record].b[9] = (unsigned char)divisor;
            speed_step = divisor - period;
            speed_step >>= 2;
            period += speed_step;
            ramp_feed_speed_records[record].b[2] = (unsigned char)(period >> 8);
            ramp_feed_speed_records[record].b[3] = (unsigned char)period;
            period += speed_step;
            ramp_feed_speed_records[record].b[4] = (unsigned char)(period >> 8);
            ramp_feed_speed_records[record].b[5] = (unsigned char)period;
            period += speed_step;
            ramp_feed_speed_records[record].b[6] = (unsigned char)(period >> 8);
            ramp_feed_speed_records[record].b[7] = (unsigned char)period;
            period = divisor;
            period = period * 1000;
            period = period / 0x3ce;
            ramp_feed_speed_records[record].b[16] = (unsigned char)(period >> 8);
            ramp_feed_speed_records[record].b[17] = (unsigned char)period;
            period = period - divisor;
            period >>= 2;
            divisor += period;
            ramp_feed_speed_records[record].b[10] = (unsigned char)(divisor >> 8);
            ramp_feed_speed_records[record].b[11] = (unsigned char)divisor;
            divisor += period;
            ramp_feed_speed_records[record].b[12] = (unsigned char)(divisor >> 8);
            ramp_feed_speed_records[record].b[13] = (unsigned char)divisor;
            divisor += period;
            ramp_feed_speed_records[record].b[14] = (unsigned char)(divisor >> 8);
            ramp_feed_speed_records[record].b[15] = (unsigned char)divisor;
        }
        }
        if (job_feed_speed_class == 3 || job_feed_speed_class == 2) {
            p = p_chip_a_ramp + 0x11a;
            for (i = 0; i < 0x17; i++) {
                period = ramp_feed_fast_brake[i] - 1;
                *p++ = (unsigned char)(period >> 8);
                *p++ = (unsigned char)period;
            }
        }
    } else {
        for (i = 0; i < 0x7a; i++) {
            period = ramp_feed_accel[i] - 1;
            record = 0;
            switch (i) {
            case 0x14:
                record++;
            case 0x31:
                record++;
            case 0x79:
                record++;

                period = period * 1000;
                if (mailbox.settings_0a & 0x80) {
                    divisor = (long)(mailbox.settings_0a & 0x3f) + 1000;
                } else {
                    divisor = 1000 - (long)(mailbox.settings_0a & 0x3f);
                }
                period = period / divisor;
                period = period * 1000;
                if (mailbox.settings_da & 0x80) {
                    divisor = (long)(mailbox.settings_da & 0x1f) + 1000;
                } else {
                    divisor = 1000 - (long)(mailbox.settings_da & 0x1f);
                }
                period = period / divisor;
                period = period * 10000;
                if (flag) {
                    divisor = temperature_correction + 10000;
                } else {
                    divisor = 10000 - temperature_correction;
                }
                period = period / divisor;
                divisor = period;
                period = period * 1000;
                period = period / 0x402;
                break;
            default:
                break;
            }

        if (feed_mode.b & FEED_MODE_HALF_STEP) {
            period = period >> 1;
        }
        *p++ = (unsigned char)(period >> 8);
        *p++ = (unsigned char)period;
        if (record) {
            record--;
            ramp_feed_speed_records[record].b[0] = (unsigned char)(period >> 8);
            ramp_feed_speed_records[record].b[1] = (unsigned char)period;
            ramp_feed_speed_records[record].b[8] = (unsigned char)(divisor >> 8);
            ramp_feed_speed_records[record].b[9] = (unsigned char)divisor;
            speed_step = divisor - period;
            speed_step >>= 2;
            period += speed_step;
            ramp_feed_speed_records[record].b[2] = (unsigned char)(period >> 8);
            ramp_feed_speed_records[record].b[3] = (unsigned char)period;
            period += speed_step;
            ramp_feed_speed_records[record].b[4] = (unsigned char)(period >> 8);
            ramp_feed_speed_records[record].b[5] = (unsigned char)period;
            period += speed_step;
            ramp_feed_speed_records[record].b[6] = (unsigned char)(period >> 8);
            ramp_feed_speed_records[record].b[7] = (unsigned char)period;
            period = divisor;
            period = period * 1000;
            period = period / 0x3ce;
            ramp_feed_speed_records[record].b[16] = (unsigned char)(period >> 8);
            ramp_feed_speed_records[record].b[17] = (unsigned char)period;
            period = period - divisor;
            period >>= 2;
            divisor += period;
            ramp_feed_speed_records[record].b[10] = (unsigned char)(divisor >> 8);
            ramp_feed_speed_records[record].b[11] = (unsigned char)divisor;
            divisor += period;
            ramp_feed_speed_records[record].b[12] = (unsigned char)(divisor >> 8);
            ramp_feed_speed_records[record].b[13] = (unsigned char)divisor;
            divisor += period;
            ramp_feed_speed_records[record].b[14] = (unsigned char)(divisor >> 8);
            ramp_feed_speed_records[record].b[15] = (unsigned char)divisor;
        }
        }
        for (i = 0; i < 0x2a; i++) {
            period = ramp_feed_brake[i] - 1;
            if (feed_mode.b & FEED_MODE_HALF_STEP) {
                period = period >> 1;
            }
            *p++ = (unsigned char)(period >> 8);
            *p++ = (unsigned char)period;
        }
    }
    set_imask_exr(7);
    p_chip_a_motor->control_1.f_0_2.f = 2;
    p_chip_a_motor->control_2.f_0_2.f = 2;
    set_imask_exr(feed_saved_imask);
}

void ramp_build_feed_gentle(void)
{
    unsigned short i;
    unsigned short flag;
    unsigned long period;
    unsigned long divisor;
    unsigned long temperature_correction;
    unsigned char *p;
    unsigned short flag2;
    unsigned short unused;
    unsigned char temperature;

    flag2 = 0;
    if (mailbox.long_page == MDC_LONG_PAGE_ON && mailbox.speed_class_b == 3) {
        flag2 = 1;
    }
    set_imask_exr(7);
    p_chip_a_motor->control_1.f_0_2.f = 0;
    p_chip_a_motor->control_2.f_0_2.f = 0;
    set_imask_exr(feed_saved_imask);
    p = p_chip_a_ramp;
    for (i = 0; i < 0xf4; i++) {
        ramp_feed_saved[i] = *p++;
    }
    ramp_feed_restore_pending = 1;
    temperature = sensor_temperature_b;
    if (temperature > 0x40) {
        temperature = 0x40;
    }
    temperature_correction = ramp_temperature_correction[temperature];
    if (temperature > 0x40) {
        flag = 0;
    } else {
        flag = 1;
    }
    if (mailbox.settings_af & 2) {
        flag = 1;
        temperature_correction = 0;
    }
    if (feed_fast_ramp == 1) {
    } else {
        p = p_chip_a_ramp;
        for (i = 0; i < 0x7a; i++) {
            if (flag2) {
                period = ramp_feed_accel[i] - 1;
            } else {
                period = ramp_feed_gentle_accel[i] - 1;
            }
            if (i == 0x79) {
                period = period * 1000;
                if (mailbox.settings_0a & 0x80) {
                    divisor = (long)(mailbox.settings_0a & 0x3f) + 1000;
                } else {
                    divisor = 1000 - (long)(mailbox.settings_0a & 0x3f);
                }
                period = period / divisor;
                period = period * 1000;
                if (mailbox.settings_da & 0x80) {
                    divisor = (long)(mailbox.settings_da & 0x1f) + 1000;
                } else {
                    divisor = 1000 - (long)(mailbox.settings_da & 0x1f);
                }
                period = period / divisor;
                period = period * 10000;
                if (flag) {
                    divisor = temperature_correction + 10000;
                } else {
                    divisor = 10000 - temperature_correction;
                }
                period = period / divisor;
            }
            if (feed_mode.b & FEED_MODE_HALF_STEP) {
                period = period >> 1;
            }
            *p++ = (unsigned char)(period >> 8);
            *p++ = (unsigned char)period;
        }
    }
    set_imask_exr(7);
    p_chip_a_motor->control_1.f_0_2.f = 2;
    p_chip_a_motor->control_2.f_0_2.f = 2;
    set_imask_exr(feed_saved_imask);
}

void ramp_restore_feed(void)
{
    unsigned short i;
    unsigned char *p;

    switch (ramp_feed_restore_step) {
    case 1:
        p = p_chip_a_ramp;
        for (i = 0; i < 0x1e; i++) {
            *p++ = ramp_feed_saved[i];
        }
        ramp_feed_restore_step = 2;
        break;
    case 2:
        p = p_chip_a_ramp + 0x1e;
        for (i = 0x1e; i < 0x3c; i++) {
            *p++ = ramp_feed_saved[i];
        }
        ramp_feed_restore_step = 3;
        break;
    case 3:
        p = p_chip_a_ramp + 0x3c;
        for (i = 0x3c; i < 0x5a; i++) {
            *p++ = ramp_feed_saved[i];
        }
        ramp_feed_restore_step = 4;
        break;
    case 4:
        p = p_chip_a_ramp + 0x5a;
        for (i = 0x5a; i < 0x78; i++) {
            *p++ = ramp_feed_saved[i];
        }
        ramp_feed_restore_step = 5;
        break;
    case 5:
        p = p_chip_a_ramp + 0x78;
        for (i = 0x78; i < 0x96; i++) {
            *p++ = ramp_feed_saved[i];
        }
        ramp_feed_restore_step = 6;
        break;
    case 6:
        p = p_chip_a_ramp + 0x96;
        for (i = 0x96; i < 0xb4; i++) {
            *p++ = ramp_feed_saved[i];
        }
        ramp_feed_restore_step = 7;
        break;
    case 7:
        p = p_chip_a_ramp + 0xb4;
        for (i = 0xb4; i < 0xd2; i++) {
            *p++ = ramp_feed_saved[i];
        }
        ramp_feed_restore_step = 8;
        break;
    case 8:
        p = p_chip_a_ramp + 0xd2;
        for (i = 0xd2; i < 0xf4; i++) {
            *p++ = ramp_feed_saved[i];
        }
        ramp_feed_restore_step = 0;
        if (mailbox.long_page == MDC_LONG_PAGE_ON && mailbox.speed_class_b == 3) {
            p_chip_a_motor->ramp_index_1.f_0_8.f = 0x79;
        } else {
            p_chip_a_motor->ramp_index_1.f_0_8.f = 0x14;
        }
        break;
    default:
        ramp_feed_restore_step = 0;
        break;
    }
}

void ramp_load_feed_speed_steps(unsigned char speed_class)
{
    if (speed_class > 5) {
        speed_class = 2;
    } else if (speed_class > 2) {
        speed_class -= 3;
    } else {
        speed_class = 0;
    }
    ramp_feed_periods[0].a = ramp_feed_speed_records[speed_class].b[0];
    ramp_feed_periods[0].b = ramp_feed_speed_records[speed_class].b[1];
    ramp_feed_periods[1].a = ramp_feed_speed_records[speed_class].b[2];
    ramp_feed_periods[1].b = ramp_feed_speed_records[speed_class].b[3];
    ramp_feed_periods[2].a = ramp_feed_speed_records[speed_class].b[4];
    ramp_feed_periods[2].b = ramp_feed_speed_records[speed_class].b[5];
    ramp_feed_periods[3].a = ramp_feed_speed_records[speed_class].b[6];
    ramp_feed_periods[3].b = ramp_feed_speed_records[speed_class].b[7];
    ramp_feed_periods[4].a = ramp_feed_speed_records[speed_class].b[8];
    ramp_feed_periods[4].b = ramp_feed_speed_records[speed_class].b[9];
    ramp_feed_periods[5].a = ramp_feed_speed_records[speed_class].b[10];
    ramp_feed_periods[5].b = ramp_feed_speed_records[speed_class].b[11];
    ramp_feed_periods[6].a = ramp_feed_speed_records[speed_class].b[12];
    ramp_feed_periods[6].b = ramp_feed_speed_records[speed_class].b[13];
    ramp_feed_periods[7].a = ramp_feed_speed_records[speed_class].b[14];
    ramp_feed_periods[7].b = ramp_feed_speed_records[speed_class].b[15];
    ramp_feed_periods[8].a = ramp_feed_speed_records[speed_class].b[16];
    ramp_feed_periods[8].b = ramp_feed_speed_records[speed_class].b[17];
}

void ramp_build_fb_motor(void)
{
    unsigned short i;
    unsigned long period;
    unsigned long divisor;
    unsigned short *accel;
    unsigned char *high_dst;
    unsigned char *low_dst;

    accel = ramp_fb_motor_accel;
    fb_motor_ramp_end_class_0 = 0x17b;
    fb_motor_ramp_end_class_1 = 0x17b;
    fb_motor_ramp_end_class_2 = 0x17b;
    fb_motor_ramp_end_class_3 = 0xf7;
    fb_motor_ramp_end_class_4 = 0x8b;
    fb_motor_ramp_end_class_5 = 0x3d;
    fb_motor_ramp_end_class_6 = 0xf;
    fb_motor_scan_ramp_end = fb_motor_ramp_end_by_class[job_fb_motor_speed_class];
    high_dst = ramp_fb_motor_high;
    low_dst = ramp_fb_motor_low;
    for (i = 0; i <= fb_motor_ramp_end_class_0; i++) {
        period = accel[i] - 1;
        if (i == fb_motor_ramp_end_class_1) {
            period = period * 1000;
            if (mailbox.settings_0b & 0x80) {
                divisor = (long)(mailbox.settings_0b & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_0b & 0x3f);
            }
            period = period / divisor;
            period = period * 1000;
            if (mailbox.settings_db & 0x80) {
                divisor = (long)(mailbox.settings_db & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_db & 0x3f);
            }
            period = period / divisor;
        } else if (i == fb_motor_ramp_end_class_3) {
            period = period * 1000;
            if (mailbox.settings_0b & 0x80) {
                divisor = (long)(mailbox.settings_0b & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_0b & 0x3f);
            }
            period = period / divisor;
            period = period * 1000;
            if (mailbox.settings_db & 0x80) {
                divisor = (long)(mailbox.settings_db & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_db & 0x3f);
            }
            period = period / divisor;
        } else if (i == fb_motor_ramp_end_class_4) {
            period = period * 1000;
            if (mailbox.settings_0b & 0x80) {
                divisor = (long)(mailbox.settings_0b & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_0b & 0x3f);
            }
            period = period / divisor;
            period = period * 1000;
            if (mailbox.settings_db & 0x80) {
                divisor = (long)(mailbox.settings_db & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_db & 0x3f);
            }
            period = period / divisor;
        } else if (i == fb_motor_ramp_end_class_5) {
            period = period * 1000;
            if (mailbox.settings_0b & 0x80) {
                divisor = (long)(mailbox.settings_0b & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_0b & 0x3f);
            }
            period = period / divisor;
            period = period * 1000;
            if (mailbox.settings_db & 0x80) {
                divisor = (long)(mailbox.settings_db & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_db & 0x3f);
            }
            period = period / divisor;
        } else if (i == fb_motor_ramp_end_class_6) {
            period = period * 1000;
            if (mailbox.settings_0b & 0x80) {
                divisor = (long)(mailbox.settings_0b & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_0b & 0x3f);
            }
            period = period / divisor;
            period = period * 1000;
            if (mailbox.settings_db & 0x80) {
                divisor = (long)(mailbox.settings_db & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_db & 0x3f);
            }
            period = period / divisor;
        }
        *high_dst++ = (unsigned char)(period >> 8);
        *low_dst++ = (unsigned char)period;
    }
    high_dst = ramp_fb_motor_half_high;
    low_dst = ramp_fb_motor_half_low;
    for (i = 0; i <= fb_motor_ramp_end_class_0; i++) {
        period = accel[i] - 1;
        if (i == fb_motor_ramp_end_class_1) {
            period = period * 1000;
            if (mailbox.settings_0b & 0x80) {
                divisor = (long)(mailbox.settings_0b & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_0b & 0x3f);
            }
            period = period / divisor;
            period = period * 1000;
            if (mailbox.settings_db & 0x80) {
                divisor = (long)(mailbox.settings_db & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_db & 0x3f);
            }
            period = period / divisor;
        } else if (i == fb_motor_ramp_end_class_3) {
            period = period * 1000;
            if (mailbox.settings_0b & 0x80) {
                divisor = (long)(mailbox.settings_0b & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_0b & 0x3f);
            }
            period = period / divisor;
            period = period * 1000;
            if (mailbox.settings_db & 0x80) {
                divisor = (long)(mailbox.settings_db & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_db & 0x3f);
            }
            period = period / divisor;
        } else if (i == fb_motor_ramp_end_class_4) {
            period = period * 1000;
            if (mailbox.settings_0b & 0x80) {
                divisor = (long)(mailbox.settings_0b & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_0b & 0x3f);
            }
            period = period / divisor;
            period = period * 1000;
            if (mailbox.settings_db & 0x80) {
                divisor = (long)(mailbox.settings_db & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_db & 0x3f);
            }
            period = period / divisor;
        } else if (i == fb_motor_ramp_end_class_5) {
            period = period * 1000;
            if (mailbox.settings_0b & 0x80) {
                divisor = (long)(mailbox.settings_0b & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_0b & 0x3f);
            }
            period = period / divisor;
            period = period * 1000;
            if (mailbox.settings_db & 0x80) {
                divisor = (long)(mailbox.settings_db & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_db & 0x3f);
            }
            period = period / divisor;
        } else if (i == fb_motor_ramp_end_class_6) {
            period = period * 1000;
            if (mailbox.settings_0b & 0x80) {
                divisor = (long)(mailbox.settings_0b & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_0b & 0x3f);
            }
            period = period / divisor;
            period = period * 1000;
            if (mailbox.settings_db & 0x80) {
                divisor = (long)(mailbox.settings_db & 0x3f) + 1000;
            } else {
                divisor = 1000 - (long)(mailbox.settings_db & 0x3f);
            }
            period = period / divisor;
        }
        period = period >> 1;
        *high_dst++ = (unsigned char)(period >> 8);
        *low_dst++ = (unsigned char)period;
    }
}
