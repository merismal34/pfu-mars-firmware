#include "mdc.h"

void hw_hook_started(void)
{
}

void hw_copy_vectors(void)
{
    unsigned long *dst;
    unsigned long *src;

    dst = (unsigned long *)p_ram_vectors;
    src = hw_vector_jumps;
    while (dst < (unsigned long *)(p_ram_vectors + 0x170)) {
        *dst = *src;
        dst++;
        src++;
    }
}

void hw_init(void)
{
    if (cmd_standby_flags.bit.wake_line_high) {
        p_p3->DR.BYTE = 0x20;
    }
    sensor_mux_interval = 0x32;
    hw_set_ad_mode(AD_SCAN_AN0_3);
    hw_init_da();
    if (hw_init_delay() == 0) {
        while (1)
            ;
    }
    hw_init_hook();
    hw_init_sci();
    set_imask_exr(7);
    hw_init_intc();
    hw_init_dtc();
    hw_init_tpu();
    hw_init_tmr();
    set_imask_exr(0);
    if (hw_init_step_10() == 0) {
        while (1)
            ;
    }
    set_imask_exr(0);
}

int hw_init_delay(void)
{
    short i;
    unsigned char imask;

    imask = get_imask_exr();
    for (i = 0; i < 20; i++) {
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
        boot_delay();
    }
    boot_delay();
    boot_delay();
    boot_delay();
    boot_delay();
    boot_delay();
    util_delay();
    return 1;
}

void hw_init_hook(void)
{
    short unused;
}

void hw_init_dtc(void)
{
    if (p_dtc->EA.BYTE != 0) {
        p_dtc->EA.BYTE = 0;
    }
    if (p_dtc->EB.BYTE != 0) {
        p_dtc->EB.BYTE = 0;
    }
    if (p_dtc->EC.BYTE != 0) {
        p_dtc->EC.BYTE = 0;
    }
    if (p_dtc->ED.BYTE != 0) {
        p_dtc->ED.BYTE = 0;
    }
    if (p_dtc->EE.BYTE != 0) {
        p_dtc->EE.BYTE = 0;
    }
    p_dtc->VECR.BYTE = 0;
}

void hw_init_sci(void)
{
    p_sci0->SCR.BYTE = 0;
    p_sci0->SMR.BYTE = 0x30;
    p_sci0->BRR = 0x13;
    p_sci0->SCR.BIT.RIE = 1;
    p_sci0->SCR.BIT.RE = 1;
    p_sci0->SCR.BIT.TIE = 0;
    p_sci0->SCR.BIT.TE = 0;
    p_sci0->SSR.BYTE = 0x84;
    console_rx_index = 0;
    console_tx_index = 0;
    p_sci1->SCR.BYTE = 0;
    p_sci1->SMR.BYTE = 0;
}

void hw_init_tpu(void)
{
    p_tpu->TSTR.BYTE = 0;
    p_tpu->TSYR.BYTE = 0;
    p_tpu0->TCR.BYTE = 0;
    p_tpu0->TCR.BIT.CCLR = 0;
    p_tpu0->TCR.BIT.TPSC = 1;
    p_tpu0->TMDR.BYTE = 0;
    p_tpu0->TIOR.WORD = 0;
    p_tpu0->TIER.BYTE = 0;
    if (p_tpu0->TSR.BYTE & 0xff3f) {
        p_tpu0->TSR.BYTE = 0;
    }
    p_tpu0->TCNT = 0;
    p_tpu0->TGRA = 2;
    p_tpu0->TGRB = 0;
    p_tpu0->TGRC = 0;
    p_tpu0->TGRD = 0;
    p_tpu1->TCR.BYTE = 0;
    p_tpu1->TCR.BIT.CCLR = 0;
    p_tpu1->TCR.BIT.TPSC = 1;
    p_tpu1->TMDR.BYTE = 0;
    p_tpu1->TIOR.BYTE = 0;
    p_tpu1->TIER.BYTE = 0;
    if (p_tpu1->TSR.BYTE & 0xff3f) {
        p_tpu1->TSR.BYTE = 0;
    }
    p_tpu1->TCNT = 0;
    p_tpu1->TGRA = 2;
    p_tpu1->TGRB = 0;
    p_tpu2->TCR.BYTE = 0;
    p_tpu2->TCR.BIT.CCLR = 0;
    p_tpu2->TCR.BIT.TPSC = 1;
    p_tpu2->TMDR.BYTE = 0;
    p_tpu2->TIOR.BYTE = 0;
    p_tpu2->TIER.BYTE = 0;
    if (p_tpu2->TSR.BYTE & 0xff3f) {
        p_tpu2->TSR.BYTE = 0;
    }
    p_tpu2->TCNT = 0;
    p_tpu2->TGRA = 2;
    p_tpu2->TGRB = 0;
    p_tpu3->TCR.BYTE = 0;
    p_tpu3->TCR.BIT.CCLR = 0;
    p_tpu3->TCR.BIT.TPSC = 1;
    p_tpu3->TMDR.BYTE = 0;
    p_tpu3->TIOR.WORD = 0;
    p_tpu3->TIER.BYTE = 0;
    if (p_tpu3->TSR.BYTE & 0xff3f) {
        p_tpu3->TSR.BYTE = 0;
    }
    p_tpu3->TCNT = 0;
    p_tpu3->TGRA = 2;
    p_tpu3->TGRB = 0;
    p_tpu3->TGRC = 0;
    p_tpu3->TGRD = 0;
    p_tpu4->TCR.BYTE = 0;
    p_tpu4->TCR.BIT.CCLR = 0;
    p_tpu4->TCR.BIT.TPSC = 1;
    p_tpu4->TMDR.BYTE = 0;
    p_tpu4->TIOR.BYTE = 0;
    p_tpu4->TIER.BYTE = 0;
    if (p_tpu4->TSR.BYTE & 0xff3f) {
        p_tpu4->TSR.BYTE = 0;
    }
    p_tpu4->TCNT = 0;
    p_tpu4->TGRA = 2;
    p_tpu4->TGRB = 0;
    p_tpu5->TCR.BYTE = 0;
    p_tpu5->TCR.BIT.CCLR = 1;
    p_tpu5->TCR.BIT.TPSC = 6;
    p_tpu5->TMDR.BYTE = 0;
    p_tpu5->TIOR.BYTE = 0;
    p_tpu5->TIER.BYTE = 0;
    if (p_tpu5->TSR.BYTE & 0xff3f) {
        p_tpu5->TSR.BYTE = 0;
    }
    p_tpu5->TCNT = 0;
    p_tpu5->TGRA = 188;
    p_tpu5->TGRB = 0;
    p_tpu->TSTR.BIT.CST5 = 1;
    p_tpu5->TIER.BIT.TGIEA = 1;
}

void hw_init_tmr(void)
{
    unsigned char t;

    t = p_tmr0->TCSR.BYTE;
    p_tmr0->TCSR.BYTE = 0;
    p_tmr0->TCR.BYTE = 1;
    p_tmr0->TCNT = 0;
    p_tmr0->TCORA = 2;
    p_tmr0->TCORB = 0;
    if (p_tmr0->TCSR.BYTE & 0xff3f) {
        p_tmr0->TCSR.BYTE = 0;
    }
    t = p_tmr1->TCSR.BYTE;
    p_tmr1->TCSR.BYTE = 0;
    p_tmr1->TCR.BYTE = 1;
    p_tmr1->TCNT = 0;
    p_tmr1->TCORA = 2;
    p_tmr1->TCORB = 0;
    if (p_tmr1->TCSR.BYTE & 0xff3f) {
        p_tmr1->TCSR.BYTE = 0;
    }
    tick_seconds = 0;
    cmd_test_mode = 1;
    hw_init_sensors();
}

void hw_init_sensors(void)
{
    unsigned char i;
    unsigned char pad[16];
    union byte_view st;
    unsigned char buf[160];

    i = 0;
    sensor_cover_count = 0;
    for (; i < 10; i++) {
        st.b = p_p1->PORT.BYTE;
        if (st.bit.b1) {
            if (sensor_cover_count < 5) {
                sensor_cover_count++;
            }
        } else {
            if (sensor_cover_count != 0) {
                sensor_cover_count--;
            }
        }
        if (st.bit.b5) {
            if (sensor_document_cover_count < 5) {
                sensor_document_cover_count++;
            }
        } else {
            if (sensor_document_cover_count != 0) {
                sensor_document_cover_count--;
            }
        }
    }
    if (sensor_cover_count > 2) {
        sensor_flags.bit.adf_cover_open = 1;
        sensor_cover_count = 0x31;
        mailbox.sensor_status.bit.adf_cover_open = 1;
        tick_adf_cover_state = 0;
    } else {
        sensor_flags.bit.adf_cover_open = 0;
        sensor_cover_count = 0;
        mailbox.sensor_status.bit.adf_cover_open = 0;
        tick_adf_cover_state = 2;
    }
    if (sensor_document_cover_count > 2) {
        sensor_flags.bit.document_cover_open = 1;
        mailbox.sensor_status.bit.document_cover_open = 1;
        tick_document_cover_state = 2;
    } else {
        sensor_flags.bit.document_cover_open = 0;
        mailbox.sensor_status.bit.document_cover_open = 0;
        tick_document_cover_state = 0;
    }
}

void hw_set_ad_mode(unsigned char mode)
{
    unsigned char t;

    t = p_ad->DRA >> 8;
    p_ad->CR.BYTE = 0;
    p_ad->CSR.BYTE = 0;
    t = p_ad->DRA >> 8;
    if (mode == AD_SCAN_AN0_3) {
        sensor_df_active = 0;
        p_ad->CSR.BYTE = 0x13;
        p_ad->CR.BYTE = 0x37;
        p_ad->CSR.BYTE = 0x33;
        sensor_mux_enable = 1;
        lamp_heater_flatbed_on = 1;
        lamp_heater_front_on = 1;
        lamp_heater_back_on = 1;
    } else if (mode == AD_ULTRASONIC) {
        sensor_mux_enable = 0;
        lamp_heater_flatbed_on = 0;
        lamp_heater_front_on = 0;
        lamp_heater_back_on = 0;
        sensor_df_active = 0;
        p_ad->CSR.BYTE = 0x4c;
        p_ad->CR.BYTE = 0x37;
        p_ad->CSR.BYTE = 0x6c;
    } else if (mode == AD_ULTRASONIC_TRIGGERED) {
        sensor_mux_enable = 0;
        lamp_heater_flatbed_on = 0;
        lamp_heater_front_on = 0;
        lamp_heater_back_on = 0;
        sensor_df_active = 0;
        p_ad->CSR.BYTE = 0x4c;
        p_ad->CR.BYTE = 0xff;
    } else if (mode == AD_DF) {
        sensor_mux_enable = 0;
        lamp_heater_flatbed_on = 0;
        lamp_heater_front_on = 0;
        lamp_heater_back_on = 0;
        sensor_df_active = 1;
        p_ad->CSR.BYTE = 0x15;
        p_ad->CR.BYTE = 0x37;
        p_ad->CSR.BYTE = 0x35;
    }
}

void hw_init_da(void)
{
    p_da->CR.BYTE = 0;
    p_da->DR0 = 0;
    p_da->DR1 = 0;
    p_da->CR.BYTE = 0xe0;
}

void hw_init_intc(void)
{
    short unused;

    p_intc->IER.BYTE = 0;
    p_syscr->BIT.INTM = 2;
    p_intc->ISCR.WORD = 0;
    p_chip_a_event->reg_00.bit.b0 = 0;
    p_chip_a_motor->enable.b = 0;
    p_chip_b_event->reg_00.bit.b0 = 0;
    p_chip_b_motor->enable.b = 0;
    p_chip_a_motor->control_1.f_0_2.f = 0;
    p_chip_a_motor->control_2.f_0_2.f = 0;
    p_chip_a_motor->reg_12.bit.b7 = 0;
    p_chip_b_motor->control_1.f_0_2.f = 0;
    p_chip_b_motor->control_2.f_0_2.f = 0;
    p_chip_b_motor->reg_12.bit.b7 = 0;
    p_chip_a_event->events.b = 1;
    p_chip_a_motor->events.b = 0x1f;
    p_chip_b_event->events.b = 1;
    p_chip_b_motor->events.b = 0x1f;
    p_intc->IPRA.BYTE = 0x70;
    p_intc->IPRB.BYTE = 0;
    p_intc->IPRC.BYTE = 0x50;
    p_intc->IPRD.BYTE = 0;
    p_intc->IPRE.BYTE = 7;
    p_intc->IPRF.BYTE = 0x66;
    p_intc->IPRG.BYTE = 0x52;
    p_intc->IPRH.BYTE = 0x41;
    p_intc->IPRI.BYTE = 0x34;
    p_intc->IPRJ.BYTE = 4;
    p_intc->IPRK.BYTE = 0;
    while (p_intc->ISR.BYTE & 0x81) {
        p_intc->ISR.BYTE = 0;
    }
    p_intc->IER.BYTE = 0x81;
}

int hw_init_step_10(void)
{
    long unused;
    short pattern;

    pattern = 0xff;
    while (pattern != -85) {
        pattern -= 0x55;
    }
    return 1;
}

int hw_test_words(unsigned short *start, unsigned long count, unsigned short val)
{
    unsigned long i;
    unsigned short *p;
    int ret;

    ret = 1;
    i = 0;
    p = start;
    while (i < count) {
        *p = val;
        i++;
        p++;
    }
    i = 0;
    p = start;
    while (i < count) {
        if (*p != val) {
            break;
        }
        i++;
        p++;
    }
    return ret;
}

void hw_init_sections(void)
{
    unsigned long *dst;
    unsigned long *src;

    dst = hw_section_b_start;
    while (dst < hw_section_b_end) {
        *dst = 0;
        dst++;
    }
    dst = hw_section_d_start;
    src = hw_section_d_rom;
    while (dst < hw_section_d_end) {
        *dst = *src;
        dst++;
        src++;
    }
}

void hw_hook_sections(void)
{
}

void hw_init_motor_chips(void)
{
    unsigned short i;
    unsigned short j;
    union byte_view st;

    i = 0;
    for (; i < 10000; i++) {
        j = i;
    }
    p_chip_a->inputs.b = 0;
    p_chip_a->reg_00.f_0_8.f = 96;
    p_chip_a->inputs.b = 0;
    p_chip_b->reg_04.bit.b7 = 1;
    p_chip_b->outputs.b = 0;
    p_chip_b->reg_06.f_0_7.f = 71;
    p_chip_b->reg_06.bit.b7 = 0;
    p_chip_b->outputs.b = 0;
    i = 0;
    while (p_chip_a_sensor->control.b != 0x32) {
    p_chip_a_sensor->control.bit.b6 = 0;
    p_chip_a_sensor->reg_02.b = 40;
    p_chip_a_sensor->reg_03.b = 40;
    p_chip_a_sensor->reg_04.b = 6;
    p_chip_a_sensor->reg_05.b = 80;
    p_chip_a_sensor->reg_01.bit.b5 = 0;
    p_chip_a_sensor->reg_01.f_3_2.f = 0;
    p_chip_a_sensor->reg_01.f_0_3.f = 4;
    p_chip_a_sensor->control.f_0_3.f = 2;
    p_chip_a_sensor->control.bit.b5 = 1;
    p_chip_a_sensor->control.f_3_2.f = 2;
        if (i > 100) {
            break;
        }
        i++;
    }
    p_chip_a_motor->enable.b = 0;
    p_chip_a_motor->control_1.bit.b3 = 0;
    p_chip_a_motor->control_1.f_0_2.f = 0;
    p_chip_a_motor->control_2.bit.b3 = 0;
    p_chip_a_motor->control_2.f_0_2.f = 0;
    p_chip_a_motor->period_1_high.f_0_8.f = 255;
    p_chip_a_motor->period_1_low.f_0_8.f = 255;
    p_chip_a_motor->period_2_high.f_0_8.f = 41;
    p_chip_a_motor->period_2_low.f_0_8.f = 130;
    p_chip_a_event->events.b = 1;
    p_chip_a_motor->events.b = 31;
    p_chip_a_motor->reg_02.f_0_4.f = 1;
    feed_event_divider.w = 2;
    p_chip_a_motor->reg_02.f_4_4.f = 0;
    p_chip_a_motor->reg_04.bit.b0 = 0;
    p_chip_a_motor->reg_05.bit.b6 = 1;
    p_chip_a_motor->reg_05.bit.b7 = 1;
    p_chip_a_motor->reg_05.f_0_2.f = 0;
    p_chip_a_motor->reg_05.f_2_2.f = 3;
    p_chip_a_motor->ramp_index_1.f_0_8.f = 0;
    p_chip_a_motor->reg_10.f_0_8.f = 122;
    p_chip_b_motor->enable.b = 0;
    p_chip_b_motor->control_1.bit.b3 = 0;
    p_chip_b_motor->control_1.f_0_2.f = 0;
    p_chip_b_motor->control_2.bit.b3 = 0;
    p_chip_b_motor->control_2.f_0_2.f = 0;
    p_chip_b_motor->period_1_high.f_0_8.f = 255;
    p_chip_b_motor->period_1_low.f_0_8.f = 255;
    p_chip_b_motor->period_2_high.f_0_8.f = 255;
    p_chip_b_motor->period_2_low.f_0_8.f = 255;
    p_chip_b_event->events.b = 1;
    p_chip_b_motor->events.b = 31;
    p_chip_b_motor->reg_02.f_0_4.f = 5;
    p_chip_b_motor->reg_02.f_4_4.f = 0;
    p_chip_b_motor->reg_04.bit.b0 = 0;
    p_chip_b_motor->reg_05.bit.b6 = 1;
    p_chip_b_motor->reg_05.bit.b7 = 1;
    p_chip_b_motor->reg_05.f_0_2.f = 0;
    p_chip_b_motor->reg_05.f_2_2.f = 0;
    p_chip_b_motor->ramp_index_1.f_0_8.f = 0;
    p_chip_b_motor->reg_10.f_0_8.f = 0;
    for (i = 0; i < 8; i++) {
        p_chip_a_motor->current_1[i] = motor_current_tables[0][0][i];
        p_chip_a_motor->current_2[i] = motor_current_tables[2][0][i];
        p_chip_b_motor->current_1[i] = motor_current_tables[1][0][i];
        p_chip_b_motor->current_2[i] = motor_current_tables[4][0][i];
    }
    ramp_load();
    p_chip_a_event->reg_00.bit.b0 = 1;
    p_chip_b_event->reg_00.bit.b0 = 0;
    for (i = 0; i < 10; i++) {
        st.b = p_chip_a->inputs.b;
        if (st.bit.b4) {
            if (sensor_empty_count < 5) {
                sensor_empty_count++;
            }
        } else {
            if (sensor_empty_count != 0) {
                sensor_empty_count--;
            }
        }
    }
    if (sensor_empty_count > 2) {
        tick_hopper_state = 2;
        sensor_flags.bit.no_paper = 1;
    } else {
        tick_hopper_state = 0;
        sensor_flags.bit.no_paper = 0;
    }
}

void hw_clear_mailbox(void)
{
    unsigned char *p;
    unsigned char pattern;
    unsigned char flag;

    util_put_be16(&mailbox.handshake_high, 0);
    set_imask_exr(7);
    pattern = 0xff;
    flag = 1;
    while (flag != 0 && pattern != 0xab) {
        for (p = &mailbox.startup_state; p < &mailbox.cmd_result; p++) {
            *p = pattern;
        }
        for (p = &mailbox.startup_state; p < &mailbox.cmd_result; p++) {
            if (*p != pattern) {
            }
        }
        pattern += 0xab;
    }
    set_imask_exr(0);
    if (flag == 0) {
        sleep();
    }
    mailbox.request = 0;
}
