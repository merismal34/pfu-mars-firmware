#include "mdc.h"

void motor_enable_chips(unsigned char mode)
{
    if (mode == 1) {
        p_chip_a_motor->control_1.f_0_2.f = 0;
        p_chip_a_motor->control_2.f_0_2.f = 0;
        p_chip_b_motor->control_1.f_0_2.f = 0;
        p_chip_b_motor->control_2.f_0_2.f = 0;
        p_chip_a_motor->enable.bit.b0 = 1;
        p_chip_a_motor->enable.bit.b1 = 1;
        p_chip_b_motor->enable.bit.b0 = 1;
        p_chip_b_motor->enable.bit.b1 = 1;
        p_chip_a_event->events.b = 3;
        p_chip_a_motor->events.b = 0x1f;
        p_chip_b_event->events.b = 3;
        p_chip_b_motor->events.b = 0x1f;
        p_chip_a_motor->control_1.f_0_2.f = 2;
        p_chip_a_motor->control_2.f_0_2.f = 2;
        p_chip_b_motor->control_1.f_0_2.f = 2;
        p_chip_b_motor->control_2.f_0_2.f = 2;
        tick_timeout = 0x32;
        while (tick_timeout != 0)
            ;
    } else {
        p_chip_a_motor->control_1.f_0_2.f = 0;
        p_chip_a_motor->control_2.f_0_2.f = 0;
        p_chip_b_motor->control_1.f_0_2.f = 0;
        p_chip_b_motor->control_2.f_0_2.f = 0;
        p_chip_a_motor->enable.bit.b0 = 0;
        p_chip_a_motor->enable.bit.b1 = 0;
        p_chip_b_motor->enable.bit.b0 = 0;
        p_chip_b_motor->enable.bit.b1 = 0;
        p_chip_a_event->events.b = 3;
        p_chip_a_motor->events.b = 0x1f;
        p_chip_b_event->events.b = 3;
        p_chip_b_motor->events.b = 0x1f;
    }
}
