#include "mdc.h"

#pragma interrupt (isr_nmi)
void isr_nmi(void)
{
    unsigned char *p;
    short i;

    p = (unsigned char *)p_intc;
    for (i = 0; i < 11; i++) {
        *p = 0;
    }
}

#pragma interrupt (isr_empty)
void isr_empty(void)
{
}

#pragma interrupt (isr_irq4)
void isr_irq4(void)
{
    boot_delay();
    if (isr_irq4_count < 0xffff) isr_irq4_count++;
    if (isr_irq4_count >= 3) p_intc->IER.BIT.IRQ4E = 0;
}

#pragma interrupt (isr_unused)
void isr_unused(void)
{
    while (1) {
    }
}

#pragma interrupt (isr_adi)
void isr_adi(void)
{
    unsigned char level;
    unsigned char u1;
    unsigned short u2;
    unsigned char u3;
    unsigned char overlap_mode;
    unsigned short sum_limit;
    unsigned short low_limit;

    level = p_ad->DRA >> 8;
    if (p_ad->CSR.BYTE & 0xc0) {
        p_ad->CSR.BYTE &= 0x1f;
    }
    if (sensor_us_monitor) {
        if (level <= multifeed_check.threshold) mailbox.sensor_status_2.bit.ultrasonic = 1;
        else mailbox.sensor_status_2.bit.ultrasonic = 0;
    } else {
        multifeed_check.sample_count++;
        if (!multifeed_check.flags.bit.b15) {
            multifeed_check.sum += level;
            if (multifeed_check.sample_count && (multifeed_check.sample_count & 3) == 0) {
                overlap_mode = job_overlap_flags.b & OVERLAP_MODE_MASK;
                if (overlap_mode == OVERLAP_MODE_MANUAL) {
                    sum_limit = multifeed_check.threshold_sum;
                    low_limit = multifeed_check.low_limit;
                } else {
                    sum_limit = multifeed_check.threshold_sum;
                    low_limit = multifeed_check.low_limit;
                }
                if (multifeed_check.sum <= sum_limit) {
                    multifeed_check.low_count++;
                    multifeed_check.overlap_high_count = 0;
                    multifeed_check.high_count = 0;
                    if (!sensor_flags.bit.top && multifeed_check.in_area && multifeed_check.position_count < 4 && multifeed_check.low_count == multifeed_check.low_limit) {
                        multifeed_check.positions[multifeed_check.position_count] = feed_pick_on_steps;
                        multifeed_check.ends[multifeed_check.position_count] = 0xffff;
                    }
                } else {
                    if (multifeed_check.low_count > low_limit) {
                        if (overlap_mode) {
                            multifeed_check.overlap_high_count++;
                            if (multifeed_check.overlap_high_count <= 2) {
                            } else {
                                multifeed_check.low_count = 0;
                                if (!sensor_flags.bit.top && multifeed_check.in_area && multifeed_check.position_count < 4) {
                                    multifeed_check.ends[multifeed_check.position_count] = feed_pick_on_steps;
                                    multifeed_check.position_count++;
                                } else {
                                    multifeed_check.overflow_flags.bit.b0 = 1;
                                }
                            }
                        } else {
                            if (!sensor_flags.bit.top && multifeed_check.in_area && multifeed_check.position_count < 4) {
                                multifeed_check.high_count++;
                                if (multifeed_check.high_count == 2) {
                                    multifeed_check.ends[multifeed_check.position_count] = feed_pick_on_steps;
                                    multifeed_check.position_count++;
                                    multifeed_check.low_count = 0;
                                }
                            } else {
                                multifeed_check.low_count = 0;
                            }
                        }
                    } else {
                        multifeed_check.low_count = 0;
                    }
                }
                multifeed_check.sum = 0;
            }
        } else {
            multifeed_check.sample_total += level;
            if (multifeed_check.sample_count == ULTRASONIC_SAMPLES) {
                sensor_stop_ultrasonic();
                multifeed_check.sum = (multifeed_check.sample_total << 7) >> 16;
            }
        }
    }
}

#pragma interrupt (isr_irq0)
void isr_irq0(void)
{
    unsigned char request;

    if (p_chip_a_event->events.bit.b0) {
        request = mailbox.request;
        p_chip_a_event->events.b = 1;
        p_tpu->TSTR.BIT.CST2 = 0;
        p_tpu2->TCNT = 0;
        p_tpu2->TSR.BIT.TGFA = 0;
        p_tpu->TSTR.BIT.CST2 = 1;
        p_tpu2->TIER.BIT.TGIEA = 1;
    }
    if (p_chip_a_motor->events.bit.b0) {
        p_chip_a_motor->events.b = 1;
        feed_handle_event();
    }
    if (p_chip_a_motor->events.bit.b1) {
        p_chip_a_motor->events.b = 2;
        p_tpu->TSTR.BIT.CST3 = 0;
        p_tpu3->TCNT = 0;
        p_tpu3->TSR.BIT.TGFA = 0;
        p_tpu->TSTR.BIT.CST3 = 1;
        p_tpu3->TIER.BIT.TGIEA = 1;
    }
}

#pragma interrupt (isr_irq7)
void isr_irq7(void)
{
    while (0) {
    }
    if (p_chip_b_motor->events.bit.b0) {
        p_chip_b_motor->events.b = 1;
        p_tpu->TSTR.BIT.CST1 = 0;
        p_tpu1->TCNT = 0;
        p_tpu1->TSR.BIT.TGFA = 0;
        p_tpu->TSTR.BIT.CST1 = 1;
        p_tpu1->TIER.BIT.TGIEA = 1;
    }
    if (p_chip_b_motor->events.bit.b1) {
        p_chip_b_motor->events.b = 2;
        p_tpu->TSTR.BIT.CST4 = 0;
        p_tpu4->TCNT = 0;
        p_tpu4->TSR.BIT.TGFA = 0;
        p_tpu->TSTR.BIT.CST4 = 1;
        p_tpu4->TIER.BIT.TGIEA = 1;
    }
}
