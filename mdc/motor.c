#include "mdc.h"

void motor_set_current(unsigned short motor, unsigned short row)
{
    unsigned short i;
    unsigned short x;
    unsigned char *p;

    switch (motor) {
    case MOTOR_FEED:
        p = p_chip_a_motor->current_1;
        if (row == MOTOR_CURRENT_OFF) {
            row = 1;
            p_chip_a_motor->control_1.bit.b6 = 0;
        }
        break;
    case MOTOR_PICK:
        p = p_chip_b_motor->current_1;
        if (row == MOTOR_CURRENT_OFF) {
            pick_hold_current = 0;
            p_chip_b_motor->control_1.bit.b6 = 0;
            p_chip_b_motor->control_1.bit.b3 = 0;
        }
        if (row == MOTOR_CURRENT_HOLD) {
            pick_hold_current = 1;
            pick_hold_ticks = 15000;
            p_chip_b_motor->control_1.bit.b6 = 0;
            p_chip_a->inputs.b |= 0x40;
        }
        break;
    case 2:
        p = p_chip_a_motor->current_2;
        if (row == MOTOR_CURRENT_OFF) {
            p_chip_a_motor->control_2.bit.b3 = 0;
            p_chip_a_motor->control_2.bit.b6 = 0;
        }
        break;
    case 3:
        p = p_chip_a_motor->current_2;
        if (row == MOTOR_CURRENT_OFF) {
            p_chip_a_motor->control_2.bit.b3 = 0;
            p_chip_a_motor->control_2.bit.b6 = 0;
        }
        break;
    case 4:
        p = p_chip_b_motor->current_2;
        if (row == MOTOR_CURRENT_OFF) {
            if (mailbox.capture_flags.b & MDC_CAPTURE_PAUSED) {
                row = 1;
                p_chip_b_motor->control_2.bit.b6 = 0;
            } else {
                p_chip_b_motor->control_2.bit.b3 = 0;
                p_chip_b_motor->control_2.bit.b6 = 0;
            }
            fb_motor_ramp_index = 0;
        }
        break;
    default:
        return;
    }
    for (i = 0; i < 8; i++) {
        *p++ = motor_current_tables[motor][row][i];
    }
    if (motor == 1 && row != MOTOR_CURRENT_HOLD) {
        p_chip_a->inputs.b &= ~0x40;
    }
}
