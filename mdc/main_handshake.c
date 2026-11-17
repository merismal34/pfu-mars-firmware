#include "mdc.h"

void main_handshake(void)
{
    unsigned char *p;
    unsigned char byte;
    short i;
    unsigned short handshake;

    do {
        handshake = util_get_be16(&mailbox.handshake_high);
        switch (handshake) {
        case 0:
            util_put_be16(&mailbox.handshake_high, MDC_HANDSHAKE_MDC_UP);
            break;
        case MDC_HANDSHAKE_MDC_UP:
            break;
        case MDC_HANDSHAKE_SDC_ANSWER:
            mailbox.version_0 = '0';
            mailbox.version_1 = 'N';
            mailbox.version_2 = '0';
            mailbox.version_3 = '0';
            p = (unsigned char *)0x178000;
            i = 0;
            while (i < 0x20) {
                byte = *p++;
                if (byte == 0xff) {
                    byte = 0;
                }
                mailbox.info_table[i++] = byte;
            }
            mailbox.info_table_valid = 0x80;
            main_read_initial_sensors();
            util_put_be16(&mailbox.handshake_high, MDC_HANDSHAKE_DONE);
            break;
        }
    } while (util_get_be16(&mailbox.handshake_high) != MDC_HANDSHAKE_DONE);
    set_imask_exr(7);
    util_put_be16(&mailbox.top_to_front_distance_high, main_top_to_front_distance);
    util_put_be16(&mailbox.top_to_back_distance_high, main_top_to_back_distance);
    util_put_be16(&mailbox.byte_0x35e, 0x2c4);
    util_put_be16(&mailbox.byte_0x350, main_mailbox_350_value);
    if (mailbox.settings_8f != 0) {
        multifeed_check.threshold = mailbox.settings_8f;
    } else if (multifeed_check.idle_threshold != 0) {
        multifeed_check.threshold = multifeed_check.idle_threshold;
    } else {
        multifeed_check.threshold = ULTRASONIC_THRESHOLD_DEFAULT;
    }
    multifeed_check.low_limit = 2;
    multifeed_check.w28 = 0x1e;
    multifeed_check.w30 = 2;
    main_scale_ultrasonic_threshold();
    set_imask_exr(0);
}

void main_scale_ultrasonic_threshold(void)
{
    unsigned char scale_tenths;

    scale_tenths = mailbox.ultrasonic_scale;
    if (scale_tenths == 0) {
        scale_tenths = 0xc;
    } else if (scale_tenths > 0x13) {
        scale_tenths = 0xa;
    } else {
        scale_tenths += 0xa;
    }
    if (scale_tenths == 0xa) {
        multifeed_check.threshold_scaled = multifeed_check.threshold;
    } else {
        multifeed_check.threshold_scaled = multifeed_check.threshold * scale_tenths / 10;
        if (multifeed_check.threshold_scaled > 0xd0) {
            multifeed_check.threshold_scaled = 0xd0;
        }
    }
    multifeed_check.w70 = 5;
}

void main_read_initial_sensors(void)
{
    unsigned char pad[16];
    unsigned char u;
    unsigned char buf[160];
    union byte_view st;
    unsigned short i;

    sensor_cover_count = 0;
    sensor_document_cover_count = 0;
    sensor_pick_count = 0;
    sensor_top_count = 0;
    sensor_empty_count = 0;
    sensor_pick_arm_count = 0;
    sensor_backing_count = 0;
    sensor_top_count_limit = 0x15;
    sensor_top_threshold = 9;
    set_imask_exr(7);
    i = 0;
    for (; i < 20; i++) {
        u = p_p1->PORT.BYTE;
        st.b = p_chip_a->inputs.b;
        if (!st.bit.b1) {
            if (sensor_pick_count < SENSOR_DEBOUNCE_MAX) {
                sensor_pick_count++;
            }
        } else {
            if (sensor_pick_count != 0) {
                sensor_pick_count--;
            }
        }
        if (sensor_pick_count > SENSOR_DEBOUNCE_ON) {
            sensor_flags.bit.pick = 1;
            mailbox.sensor_status.bit.pick = 1;
        } else {
            sensor_flags.bit.pick = 0;
            mailbox.sensor_status.bit.pick = 0;
        }
        if (st.bit.b3) {
            if (sensor_top_count < SENSOR_DEBOUNCE_MAX) {
                sensor_top_count++;
            }
        } else {
            if (sensor_top_count != 0) {
                sensor_top_count--;
            }
        }
        if (sensor_top_count > SENSOR_DEBOUNCE_ON) {
            sensor_flags.bit.top = 1;
            mailbox.sensor_status.bit.top = 1;
        } else {
            sensor_flags.bit.top = 0;
            mailbox.sensor_status.bit.top = 0;
        }
        if (st.bit.b4) {
            if (sensor_empty_count < SENSOR_DEBOUNCE_MAX) {
                sensor_empty_count++;
            }
        } else {
            if (sensor_empty_count != 0) {
                sensor_empty_count--;
            }
        }
        if (sensor_empty_count > SENSOR_DEBOUNCE_ON) {
            sensor_flags.bit.no_paper = 1;
            mailbox.sensor_status.bit.no_paper = 1;
        } else {
            sensor_flags.bit.no_paper = 0;
            mailbox.sensor_status.bit.no_paper = 0;
        }
        if (st.bit.b0) {
            if (sensor_pick_arm_count < SENSOR_DEBOUNCE_MAX) {
                sensor_pick_arm_count++;
            }
        } else {
            if (sensor_pick_arm_count != 0) {
                sensor_pick_arm_count--;
            }
        }
        if (sensor_pick_arm_count > SENSOR_DEBOUNCE_ON) {
            sensor_flags.bit.pick_arm = 1;
            mailbox.sensor_status.bit.pick_arm = 1;
        } else {
            sensor_flags.bit.pick_arm = 0;
            mailbox.sensor_status.bit.pick_arm = 0;
        }
        if (st.bit.b2) {
            if (sensor_backing_count < SENSOR_DEBOUNCE_MAX) {
                sensor_backing_count++;
            }
        } else {
            if (sensor_backing_count != 0) {
                sensor_backing_count--;
            }
        }
        if (sensor_backing_count > SENSOR_DEBOUNCE_ON) {
            sensor_flags.bit.backing = 1;
            mailbox.sensor_status.bit.backing = 1;
        } else {
            sensor_flags.bit.backing = 0;
            mailbox.sensor_status.bit.backing = 0;
        }
    }
    set_imask_exr(0);
}
