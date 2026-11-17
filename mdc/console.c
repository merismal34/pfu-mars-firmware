#include "mdc.h"

#pragma interrupt (isr_rxi0)
void isr_rxi0(void)
{
    unsigned char st;
    unsigned char *bp;
    unsigned short *wp;
    unsigned short word;
    unsigned short entry;
    unsigned long addr;

    st = p_sci0->SSR.BYTE;
    p_sci0->SSR.BYTE = 0x84;
    set_imask_exr(4);
    if (st & 0x38) {
        console_reset_rx();
        return;
    }
    st = p_sci0->RDR;
    if (console_tx_index && st == 0xff) {
        p_sci0->SCR.BIT.TIE = 0;
        p_sci0->SCR.BIT.TE = 0;
        console_tx_index = 0;
        return;
    }
    console_command[console_rx_index] = st;
    console_rx_index++;
    if (console_command[0] > console_rx_index)
        return;
    st = 0;
    console_answer[1] = 4;
    console_answer[2] = 0x4e;
    console_answer[3] = 0;
    console_answer[4] = 0;
    switch (console_command[1]) {
    case 0:
        break;
    case 0x10:
        addr = (unsigned long)console_command[2] << 24;
        addr += (unsigned long)console_command[3] << 16;
        addr += (unsigned long)console_command[4] << 8;
        addr += console_command[5];
        wp = (unsigned short *)addr;
        word = (console_command[6] << 8) + console_command[7];
        *wp = word;
        break;
    case 0x11:
        addr = (unsigned long)console_command[2] << 24;
        addr += (unsigned long)console_command[3] << 16;
        addr += (unsigned long)console_command[4] << 8;
        addr += console_command[5];
        bp = (unsigned char *)addr;
        *bp = console_command[6];
        break;
    case 0x20:
        if (console_command[2] == 1) {
            mailbox.cmd_code = MDC_CMD_MOTOR_TEST;
            mailbox.cmd_params.b[0] = 0x10;
            mailbox.cmd_params.b[1] = 0;
            if (console_command[3] == 0)
                mailbox.cmd_params.b[2] = 1;
            else if (console_command[3] == 1)
                mailbox.cmd_params.b[2] = 2;
            else if (console_command[3] == 2)
                mailbox.cmd_params.b[2] = 3;
            else if (console_command[3] == 3)
                mailbox.cmd_params.b[2] = 4;
            else if (console_command[3] == 4)
                mailbox.cmd_params.b[2] = 5;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
            mailbox.cmd_end = MDC_CMD_END;
        }
        break;
    case 0x21:
        set_imask_exr(7);
        err_flags.b = 0;
        err_code = 0;
        mailbox.error_flags.b = 0;
        mailbox.error_code = 0;
        util_put_be16(&mailbox.top_to_front_distance_high, main_top_to_front_distance);
        util_put_be16(&mailbox.top_to_back_distance_high, main_top_to_back_distance);
        mailbox.cmd_code = MDC_CMD_FEED;
        job_set_cmd_state(MDC_CMD_STATE_POSTED);
        feed_auto_scan = FEED_AUTO_SCAN_POST_SCAN_MOVE;
        job_state = JOB_STATE_IDLE;
        job_feed_state = JOB_FEED_STATE_IDLE;
        pick_sheet_index = 0;
        pick_gap_index = 0;
        feed_length_index = 0;
        feed_gap_index = 0;
        set_imask_exr(4);
        if (console_command[2] == 0) {
            mailbox.speed_class_a = 3;
            mailbox.speed_class_b = 3;
        } else if (console_command[2] == 1) {
            mailbox.speed_class_a = 4;
            mailbox.speed_class_b = 4;
        } else if (console_command[2] == 2) {
            mailbox.speed_class_a = 5;
            mailbox.speed_class_b = 5;
        } else {
            mailbox.speed_class_a = 3;
            mailbox.speed_class_b = 3;
        }
        if (console_command[3] & 1) {
            mailbox.feed_options.bit.ultrasonic_check = 1;
            for (word = 0; word < 0x400; word++)
                console_chip_b_byte_1[word] = 0;
        } else {
            mailbox.feed_options.bit.ultrasonic_check = 0;
        }
        if (console_command[4])
            bw_motor_arm_mode = 1;
        else
            bw_motor_arm_mode = 0;
        mailbox.feed_options.b &= (unsigned char)~(MDC_FEED_LENGTH_CHECK | MDC_FEED_LENGTH_TOLERANCE);
        if (console_command[3] & 2)
            mailbox.feed_options.b |= MDC_FEED_LENGTH_CHECK | MDC_FEED_LENGTH_TOLERANCE_10MM;
        else if (console_command[3] & 4)
            mailbox.feed_options.b |= MDC_FEED_LENGTH_CHECK | MDC_FEED_LENGTH_TOLERANCE_15MM;
        else if (console_command[3] & 8)
            mailbox.feed_options.b |= MDC_FEED_LENGTH_CHECK | MDC_FEED_LENGTH_TOLERANCE_20MM;
        break;
    case 0x22:
            addr = console_command[2];
            if (addr < 0x32)
                addr = 0x32;
            motor_current_tables[1][2][0] = 0;
            motor_current_tables[1][2][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[1][2][7] = addr;
            addr = console_command[3];
            if (addr < 0x32)
                addr = 0x32;
            motor_current_tables[1][3][0] = 0;
            motor_current_tables[1][3][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[1][3][7] = addr;
            addr = console_command[4];
            if (addr < 0x32)
                addr = 0x32;
            motor_current_tables[1][4][0] = 0;
            motor_current_tables[1][4][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[1][4][7] = addr;
            addr = console_command[5];
            if (addr < 0x32)
                addr = 0x32;
            motor_current_tables[1][5][0] = 0;
            motor_current_tables[1][5][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[1][5][7] = addr;
            addr = console_command[6];
            if (addr < 0x32)
                addr = 0x32;
            motor_current_tables[1][6][0] = 0;
            motor_current_tables[1][6][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[1][6][7] = addr;
            addr = console_command[7];
            if (addr < 0x32)
                addr = 0x32;
            motor_current_tables[1][1][0] = 0;
            motor_current_tables[1][1][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[1][1][7] = addr;
            addr = console_command[8];
            if (addr < 0x32)
                addr = 0x32;
            motor_current_tables[1][7][0] = 0;
            motor_current_tables[1][7][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[1][7][7] = addr;
        break;
    case 0x23:
            addr = console_command[2];
            if (addr < 0x3c)
                addr = 0x3c;
            motor_current_tables[0][2][0] = 0;
            motor_current_tables[0][2][2] = (addr + 0x24) * 383 / 1000 + 0xdc;
            motor_current_tables[0][2][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[0][2][6] = (addr + 0x24) * 924 / 1000 + 0xdc;
            motor_current_tables[0][2][7] = addr;
            addr = console_command[3];
            if (addr < 0x3c)
                addr = 0x3c;
            motor_current_tables[0][3][0] = 0;
            motor_current_tables[0][3][2] = (addr + 0x24) * 383 / 1000 + 0xdc;
            motor_current_tables[0][3][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[0][3][6] = (addr + 0x24) * 924 / 1000 + 0xdc;
            motor_current_tables[0][3][7] = addr;
            addr = console_command[4];
            if (addr < 0x3c)
                addr = 0x3c;
            motor_current_tables[0][4][0] = 0;
            motor_current_tables[0][4][2] = (addr + 0x24) * 383 / 1000 + 0xdc;
            motor_current_tables[0][4][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[0][4][6] = (addr + 0x24) * 924 / 1000 + 0xdc;
            motor_current_tables[0][4][7] = addr;
            addr = console_command[5];
            if (addr < 0x3c)
                addr = 0x3c;
            motor_current_tables[0][5][0] = 0;
            motor_current_tables[0][5][2] = (addr + 0x24) * 383 / 1000 + 0xdc;
            motor_current_tables[0][5][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[0][5][6] = (addr + 0x24) * 924 / 1000 + 0xdc;
            motor_current_tables[0][5][7] = addr;
            addr = console_command[6];
            if (addr < 0x3c)
                addr = 0x3c;
            motor_current_tables[0][6][0] = 0;
            motor_current_tables[0][6][2] = (addr + 0x24) * 383 / 1000 + 0xdc;
            motor_current_tables[0][6][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[0][6][6] = (addr + 0x24) * 924 / 1000 + 0xdc;
            motor_current_tables[0][6][7] = addr;
            addr = console_command[7];
            if (addr < 0x3c)
                addr = 0x3c;
            motor_current_tables[0][1][0] = 0;
            motor_current_tables[0][1][2] = (addr + 0x24) * 383 / 1000 + 0xdc;
            motor_current_tables[0][1][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
            motor_current_tables[0][1][6] = (addr + 0x24) * 924 / 1000 + 0xdc;
            motor_current_tables[0][1][7] = addr;
        break;
    case 0x24:
        if (console_command[2] == 1) {
            mailbox.cmd_code = MDC_CMD_BW_MOTOR_CYCLE;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            mailbox.cmd_code = MDC_CMD_CONSOLE_IDLE;
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
            mailbox.cmd_end = MDC_CMD_END;
        }
        break;
    case 0x25:
        if (console_command[2] == 1) {
            mailbox.cmd_code = MDC_CMD_MOTOR_TEST;
            mailbox.cmd_params.b[0] = MDC_MOTOR_TEST_FEED;
            mailbox.cmd_params.b[1] = 0;
            if (console_command[3] == 0)
                mailbox.cmd_params.b[2] = 1;
            else if (console_command[3] == 1)
                mailbox.cmd_params.b[2] = 2;
            else if (console_command[3] == 2)
                mailbox.cmd_params.b[2] = 3;
            else if (console_command[3] == 3)
                mailbox.cmd_params.b[2] = 4;
            else if (console_command[3] == 4)
                mailbox.cmd_params.b[2] = 5;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
            mailbox.cmd_end = MDC_CMD_END;
        }
        break;
    case 0x26:
        if (console_command[2] == 1) {
            mailbox.cmd_code = MDC_CMD_MOTOR_TEST;
            mailbox.cmd_params.b[0] = 1;
            mailbox.cmd_params.b[1] = 0;
            if (console_command[3] == 0)
                mailbox.cmd_params.b[2] = 1;
            else if (console_command[3] == 1)
                mailbox.cmd_params.b[2] = 2;
            else if (console_command[3] == 2)
                mailbox.cmd_params.b[2] = 3;
            else if (console_command[3] == 3)
                mailbox.cmd_params.b[2] = 4;
            else if (console_command[3] == 4)
                mailbox.cmd_params.b[2] = 5;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
            mailbox.cmd_end = MDC_CMD_END;
        }
        break;
    case 0x27:
        if (console_command[2] == 1) {
            mailbox.cmd_code = MDC_CMD_MOTOR_TEST;
            mailbox.cmd_params.b[0] = 2;
            mailbox.cmd_params.b[1] = 0;
            mailbox.cmd_params.b[2] = 0;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
            mailbox.cmd_end = MDC_CMD_END;
        }
        break;
    case 0x28:
        if (console_command[2] == 1) {
            mailbox.cmd_code = MDC_CMD_BACKING_TEST;
            mailbox.cmd_params.b[0] = 3;
            mailbox.cmd_params.b[1] = 0;
            mailbox.cmd_params.b[2] = 1;
            mailbox.cmd_params.b[3] = 0;
            bw_motor_backing_state = 0;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            mailbox.cmd_end = MDC_CMD_END;
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
        }
        break;
    case 0x29:
        if (console_command[2] == 1) {
            if (mailbox.settings_9c) {
                addr = mailbox.settings_9c;
                if (addr < 0x50)
                    addr = 0x50;
                motor_current_tables[4][2][0] = 0;
                motor_current_tables[4][2][1] = (addr + 0x12) * 195 / 1000 + 0xee;
                motor_current_tables[4][2][2] = (addr + 0x12) * 383 / 1000 + 0xee;
                motor_current_tables[4][2][3] = (addr + 0x12) * 556 / 1000 + 0xee;
                motor_current_tables[4][2][4] = (addr + 0x12) * 707 / 1000 + 0xee;
                motor_current_tables[4][2][5] = (addr + 0x12) * 831 / 1000 + 0xee;
                motor_current_tables[4][2][6] = (addr + 0x12) * 924 / 1000 + 0xee;
                motor_current_tables[4][2][7] = addr;
            }
            if (mailbox.settings_9d) {
                addr = mailbox.settings_9d;
                if (addr < 0x50)
                    addr = 0x50;
                motor_current_tables[4][4][0] = 0;
                motor_current_tables[4][4][1] = (addr + 0x12) * 195 / 1000 + 0xee;
                motor_current_tables[4][4][2] = (addr + 0x12) * 383 / 1000 + 0xee;
                motor_current_tables[4][4][3] = (addr + 0x12) * 556 / 1000 + 0xee;
                motor_current_tables[4][4][4] = (addr + 0x12) * 707 / 1000 + 0xee;
                motor_current_tables[4][4][5] = (addr + 0x12) * 831 / 1000 + 0xee;
                motor_current_tables[4][4][6] = (addr + 0x12) * 924 / 1000 + 0xee;
                motor_current_tables[4][4][7] = addr;
            }
            fb_motor_position = 0x2000;
            mailbox.cmd_code = MDC_CMD_MOTOR_TEST;
            mailbox.cmd_params.b[0] = 4;
            mailbox.cmd_params.b[1] = 0;
            mailbox.cmd_params.b[2] = console_command[3] + 1;
            if (mailbox.cmd_params.b[2] > 4)
                fb_motor_mode.bit.half_step = 1;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
            mailbox.cmd_end = MDC_CMD_END;
        }
        break;
    case 0x2a:
        if (console_command[2] == 1) {
            mailbox.cmd_code = MDC_CMD_FEED_RUN_TEST;
            if (console_command[3] == 0)
                mailbox.cmd_params.b[0] = 3;
            else if (console_command[3] == 1)
                mailbox.cmd_params.b[0] = 4;
            else if (console_command[3] == 2)
                mailbox.cmd_params.b[0] = 5;
            else
                mailbox.cmd_params.b[0] = 3;
            mailbox.cmd_params.b[1] = console_command[4];
            mailbox.cmd_params.b[2] = console_command[5];
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
            mailbox.cmd_end = MDC_CMD_END;
        }
        break;
    case 0x2b:
        if (console_command[2] == 1) {
            err_flags.b = 0;
            err_code = 0;
            mailbox.cmd_code = MDC_CMD_BW_MOTOR_RUN_TEST;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            mailbox.cmd_end = MDC_CMD_END;
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
        }
        break;
    case 0x2c:
        addr = console_command[2];
        if (addr < 0x3c)
            addr = 0x3c;
        motor_current_tables[2][1][0] = console_command[2];
        motor_current_tables[2][1][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
        motor_current_tables[2][1][7] = addr;
        p_da->DR0 = motor_current_tables[2][1][7];
        addr = console_command[3];
        if (addr < 0x3c)
            addr = 0x3c;
        motor_current_tables[2][2][0] = console_command[3];
        motor_current_tables[2][2][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
        motor_current_tables[2][2][7] = addr;
        addr = console_command[4];
        if (addr < 0x3c)
            addr = 0x3c;
        motor_current_tables[2][7][0] = console_command[4];
        motor_current_tables[2][7][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
        motor_current_tables[2][7][7] = addr;
        addr = console_command[5];
        if (addr < 0x3c)
            addr = 0x3c;
        motor_current_tables[3][1][0] = console_command[5];
        motor_current_tables[3][1][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
        motor_current_tables[3][1][7] = addr;
        addr = console_command[6];
        if (addr < 0x3c)
            addr = 0x3c;
        motor_current_tables[3][3][0] = console_command[6];
        motor_current_tables[3][3][4] = (addr + 0x24) * 707 / 1000 + 0xdc;
        motor_current_tables[3][3][7] = addr;
        break;
    case 0x2d:
        if (console_command[2] == 1) {
            if (console_command[3] == 0)
                mailbox.cmd_params.b[0] = 1;
            else
                mailbox.cmd_params.b[0] = console_command[3];
            mailbox.cmd_params.b[2] = console_command[4];
            mailbox.cmd_code = MDC_CMD_FEED_FLATBED_RUN_TEST;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            job_set_cmd_state(MDC_CMD_STATE_DONE);
            mailbox.cmd_result = MDC_RESULT_OK;
            mailbox.cmd_end = MDC_CMD_END;
        }
        break;
    case 0x2f:
        addr = console_command[2];
        if (addr < 0x50)
            addr = 0x50;
        motor_current_tables[4][2][0] = 0;
        motor_current_tables[4][2][1] = (addr + 0x12) * 195 / 1000 + 0xee;
        motor_current_tables[4][2][2] = (addr + 0x12) * 383 / 1000 + 0xee;
        motor_current_tables[4][2][3] = (addr + 0x12) * 556 / 1000 + 0xee;
        motor_current_tables[4][2][4] = (addr + 0x12) * 707 / 1000 + 0xee;
        motor_current_tables[4][2][5] = (addr + 0x12) * 831 / 1000 + 0xee;
        motor_current_tables[4][2][6] = (addr + 0x12) * 924 / 1000 + 0xee;
        motor_current_tables[4][2][7] = addr;
        addr = console_command[3];
        if (addr < 0x50)
            addr = 0x50;
        motor_current_tables[4][4][0] = 0;
        motor_current_tables[4][4][1] = (addr + 0x12) * 195 / 1000 + 0xee;
        motor_current_tables[4][4][2] = (addr + 0x12) * 383 / 1000 + 0xee;
        motor_current_tables[4][4][3] = (addr + 0x12) * 556 / 1000 + 0xee;
        motor_current_tables[4][4][4] = (addr + 0x12) * 707 / 1000 + 0xee;
        motor_current_tables[4][4][5] = (addr + 0x12) * 831 / 1000 + 0xee;
        motor_current_tables[4][4][6] = (addr + 0x12) * 924 / 1000 + 0xee;
        motor_current_tables[4][4][7] = addr;
        break;
    case 0x30:
        word = console_command[2];
        bw_motor_backing_period = 0x14c0 / word;
        word = console_command[3];
        bw_motor_arm_period_seek = 0x14c0 / word;
        word = console_command[4];
        bw_motor_arm_period = 0x14c0 / word;
        break;
    case 0x31:
        bw_motor_arm_lift_limit = console_command[3];
        main_arm_seek_picks = console_command[4];
        break;
    case 0x33:
        pick_arm_adjust_limit = console_command[2];
        pick_arm_adjust_limit <<= 8;
        pick_arm_adjust_limit += console_command[3];
        break;
    case 0x34:
        if (console_command[2]) {
            bw_motor_arm_state = BW_MOTOR_ARM_STATE_START;
            bw_motor_arm_request = BW_MOTOR_ARM_HOME;
        } else {
            bw_motor_arm_request = BW_MOTOR_ARM_TO_LEVEL;
        }
        break;
    case 0x35:
        p_chip_b_uart->mcr.bit.b0 = 1;
        p_chip_b_uart->mcr.bit.b1 = 1;
        motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_HOLD);
        p_chip_a_motor->period_2_high.f_0_8.f = 0x29;
        p_chip_a_motor->period_2_low.f_0_8.f = 0x82;
        bw_motor_arm_phase.b = p_chip_a_motor->phase_2.b;
        bw_motor_arm_phase.b = (bw_motor_arm_phase.b & 0x1f) | 0xa0;
        p_chip_a_motor->control_2.bit.b6 = 0;
        break;
    case 0x42:
        if (console_command[4] == 0) {
            if (console_command[2]) {
                lamp_heater_flatbed_on = 1;
                lamp_heater_flatbed_target = console_command[3];
            } else {
                lamp_heater_flatbed_on = 0;
            }
        } else if (console_command[4] == 1) {
            if (console_command[2]) {
                lamp_heater_front_on = 1;
                lamp_heater_front_target = console_command[3];
            } else {
                lamp_heater_front_on = 0;
            }
        } else if (console_command[4] == 2) {
            if (console_command[2]) {
                lamp_heater_back_on = 1;
                lamp_heater_back_target = console_command[3];
            } else {
                lamp_heater_back_on = 0;
            }
        } else if (console_command[4] == 3) {
            if (console_command[2]) {
            } else {
            }
        }
        if (console_command[5]) {
            sensor_mux_interval = console_command[5] * 50;
            lamp_heater_interval = console_command[5] * 50;
        }
        break;
    case 0x44:
        lamp_heater_flatbed_target = console_command[2];
        lamp_heater_front_target = console_command[3];
        lamp_heater_back_target = console_command[4];
        break;
    case 0x50:
        hw_set_ad_mode(AD_DF);
        p_chip_b->outputs.b &= 0xfc;
        if (console_command[2] == 0) {
        } else if (console_command[2] == 1) {
            p_chip_b->outputs.b |= 1;
        } else if (console_command[2] == 2) {
            p_chip_b->outputs.b |= 2;
        } else if (console_command[2] == 2) {
            p_chip_b->outputs.b |= 3;
        }
        p_da->DR1 = console_command[3];
        break;
    case 0x51:
        multifeed_check.threshold = console_command[2];
        multifeed_check.low_limit = console_command[3];
        multifeed_check.w28 = console_command[4];
        multifeed_check.w30 = console_command[5];
        break;
    case 0x60:
        if (console_command[2]) {
            err_flags.b = 0;
            err_code = 0;
            mailbox.error_flags.b = 0;
            mailbox.error_code = 0;
            mailbox.cmd_code = MDC_CMD_POWER_SAVE;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            if (console_command[3])
                mailbox.request = MDC_REQUEST_REINIT_FAST;
            else
                mailbox.request = MDC_REQUEST_REINIT_WAKE;
            p_tpu->TSTR.BIT.CST2 = 0;
            p_tpu2->TCNT = 0;
            p_tpu2->TSR.BIT.TGFA = 0;
            p_tpu->TSTR.BIT.CST2 = 1;
            p_tpu2->TIER.BIT.TGIEA = 1;
        }
        break;
    case 0x62:
        if (console_command[2]) {
            mailbox.cmd_code = MDC_CMD_DF_TEST_ON;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        } else {
            mailbox.cmd_code = MDC_CMD_DF_TEST_OFF;
            job_set_cmd_state(MDC_CMD_STATE_POSTED);
            cmd_phase = CMD_PHASE_START;
        }
        break;
    case 0x63:
        mailbox.cmd_code = MDC_CMD_ADJUST_ULTRASONIC;
        job_set_cmd_state(MDC_CMD_STATE_POSTED);
        cmd_phase = CMD_PHASE_START;
        break;
    case 0x90:
        addr = (unsigned long)console_command[2] << 24;
        addr += (unsigned long)console_command[3] << 16;
        addr += (unsigned long)console_command[4] << 8;
        addr += console_command[5];
        wp = (unsigned short *)addr;
        console_answer[1] = 9;
        word = *wp++;
        console_answer[2] = word >> 8;
        console_answer[3] = word;
        word = *wp++;
        console_answer[4] = word >> 8;
        console_answer[5] = word;
        word = *wp++;
        console_answer[6] = word >> 8;
        console_answer[7] = word;
        word = *wp++;
        console_answer[8] = word >> 8;
        console_answer[9] = word;
        break;
    case 0x91:
        addr = (unsigned long)console_command[2] << 24;
        addr += (unsigned long)console_command[3] << 16;
        addr += (unsigned long)console_command[4] << 8;
        addr += console_command[5];
        bp = (unsigned char *)addr;
        console_answer[1] = 9;
        console_answer[2] = *bp++;
        console_answer[3] = *bp++;
        console_answer[4] = *bp++;
        console_answer[5] = *bp++;
        console_answer[6] = *bp++;
        console_answer[7] = *bp++;
        console_answer[8] = *bp++;
        console_answer[9] = *bp;
        break;
    case 0xa2:
        console_answer[1] = 6;
        console_answer[2] = motor_current_tables[1][2][7];
        console_answer[3] = motor_current_tables[1][3][7];
        console_answer[4] = motor_current_tables[1][4][7];
        console_answer[5] = motor_current_tables[1][5][7];
        console_answer[6] = motor_current_tables[1][6][7];
        break;
    case 0xa3:
        console_answer[1] = 6;
        console_answer[2] = motor_current_tables[0][2][7];
        console_answer[3] = motor_current_tables[0][3][7];
        console_answer[4] = motor_current_tables[0][4][7];
        console_answer[5] = motor_current_tables[0][5][7];
        console_answer[6] = motor_current_tables[0][6][7];
        break;
    case 0xac:
        console_answer[1] = 6;
        console_answer[2] = motor_current_tables[2][1][7];
        console_answer[3] = motor_current_tables[2][2][7];
        console_answer[4] = motor_current_tables[2][3][7];
        console_answer[5] = motor_current_tables[3][1][7];
        console_answer[6] = motor_current_tables[3][3][7];
        break;
    case 0xd0:
        console_answer[1] = 3;
        console_answer[2] = sensor_flags.b;
        if (mailbox.sensor_status_2.bit.df)
            console_answer[3] = 0x80;
        else
            console_answer[3] = 0;
        break;
    case 0xd1:
        console_answer[1] = 2;
        console_answer[2] = p_ad->DRB >> 8;
        break;
    case 0xd2:
        console_answer[1] = 3;
        console_answer[2] = p_chip_b->outputs.b & 3;
        console_answer[3] = p_da->DR1;
        break;
    case 0xd5:
        console_answer[1] = 7;
        console_answer[2] = sensor_an1_level;
        console_answer[3] = sensor_an2_level;
        console_answer[4] = sensor_an3_level;
        console_answer[5] = sensor_temperature_a_level;
        console_answer[6] = console_byte_2;
        console_answer[7] = sensor_temperature_b_level;
        break;
    case 0xd6:
        console_answer[1] = 7;
        console_answer[2] = mailbox.lamp_temperature_flatbed;
        console_answer[3] = mailbox.lamp_temperature_front;
        console_answer[4] = mailbox.lamp_temperature_back;
        console_answer[5] = sensor_temperature_a;
        console_answer[6] = console_byte_1;
        console_answer[7] = sensor_temperature_b;
        break;
    case 0xd9:
        console_answer[1] = 3;
        console_answer[2] = (unsigned char)multifeed_check.low_count;
        console_answer[3] = (unsigned char)multifeed_check.w32;
        break;
    case 0xda:
        console_answer[1] = 4;
        console_answer[2] = lamp_heater_flatbed_target;
        console_answer[3] = lamp_heater_front_target;
        console_answer[4] = lamp_heater_back_target;
        break;
    case 0xe0:
        console_answer[1] = 3;
        console_answer[2] = 0xd;
        console_answer[3] = 1;
        break;
    case 0xe1:
        console_answer[1] = 6;
        console_answer[2] = main_progress_code;
        console_answer[3] = mailbox.cmd_code;
        console_answer[4] = mailbox.cmd_state;
        console_answer[5] = err_flags.b;
        console_answer[6] = err_code;
        break;
    case 0xe2:
        job_capture_flags.bit.pause_requested = 1;
        job_capture_flags.bit.pause_active = 1;
        fb_motor_mark_position = fb_motor_position;
        break;
    case 0xe3:
        job_capture_flags.bit.resume_requested = 1;
        break;
    case 0xe8:
        console_answer[1] = 0x66;
        console_answer[2] = job_cmd_history_index;
        word = 3;
        entry = 0;
        for (; entry < 0x64; entry++) {
            console_answer[word] = job_cmd_history[entry];
            word++;
        }
        break;
    case 0xe9:
        console_answer[1] = 0x66;
        console_answer[2] = job_state_history_index;
        word = 3;
        entry = 0;
        for (; entry < 0x64; entry++) {
            console_answer[word] = job_state_history[entry];
            word++;
        }
        break;
    case 0xea:
        console_answer[1] = 0x16;
        console_answer[2] = err_history_index;
        word = 3;
        entry = 0;
        for (; entry < 0x14; entry++) {
            console_answer[word] = err_history[entry];
            word++;
        }
        break;
    default:
        st = 1;
        break;
    }
    console_rx_index = 0;
    console_tx_index = 0;
    if (st)
        console_answer[0] = 0x80;
    else
        console_answer[0] = 0xff;
    p_sci0->SCR.BIT.TE = 1;
    p_sci0->SCR.BIT.TIE = 1;
}

#pragma interrupt (isr_txi0)
void isr_txi0(void)
{
    set_imask_exr(4);
    if (p_sci0->SSR.BIT.TDRE == 0) {
        console_reset_rx();
    } else {
        if (console_tx_index <= console_answer[1]) {
            p_sci0->TDR = console_answer[console_tx_index];
            p_sci0->SSR.BIT.TDRE = 0;
            console_tx_index++;
        } else {
            return;
        }
    }
}

#pragma interrupt (isr_eri0)
void isr_eri0(void)
{
    unsigned char u1;
    unsigned char u2;
    unsigned char u3;
    union byte_view st;
    long u4;
    unsigned char code;

    set_imask_exr(4);
    st.b = p_sci0->SSR.BYTE;
    p_sci0->SSR.BYTE = 0x84;
    if (st.bit.b3) {
        code = 0x80;
    } else if (st.bit.b4) {
        code = 0x81;
    } else if (st.bit.b5) {
        code = 0x82;
    } else {
        code = 0x80;
    }
    console_reset_rx();
    p_sci0->TDR = code;
}

void console_reset_rx(void)
{
    unsigned char status;
    unsigned char tries;

    tries = 0;
    do {
        status = p_sci0->SSR.BYTE;
        p_sci0->SSR.BYTE = 0x84;
        if (tries++ > 10)
            break;
    } while (status & 0x78);
    p_sci0->SSR.BYTE = 0;
    p_sci0->SCR.BYTE = 0;
    p_sci0->SMR.BIT.PE = 1;
    p_sci0->SMR.BIT.OE = 1;
    p_sci0->SCR.BIT.CKE = 0;
    p_sci0->BRR = 0x13;
    p_sci0->SCR.BIT.RIE = 1;
    p_sci0->SCR.BIT.RE = 1;
    p_sci0->SCR.BIT.TIE = 0;
    p_sci0->SCR.BIT.TE = 0;
}
