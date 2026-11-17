#include "mdc.h"

#pragma interrupt (isr_tgi3a)
void isr_tgi3a(void)
{
    union byte_view inputs;

    p_tpu->TSTR.BIT.CST3 = 0;
    bw_motor_saved_imask = get_imask_exr();
    if (!p_tpu3->TSR.BIT.TGFA) {
        return;
    }
    p_tpu3->TSR.BIT.TGFA = 0;
    inputs.b = p_chip_a->inputs.b;
    set_imask_exr(bw_motor_saved_imask);
    if (inputs.bit.b0) {
        if (sensor_pick_arm_count < SENSOR_DEBOUNCE_MAX) {
            sensor_pick_arm_count++;
        }
    } else {
        if (sensor_pick_arm_count) {
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
    if (inputs.bit.b2) {
        if (sensor_backing_count < SENSOR_DEBOUNCE_MAX) {
            sensor_backing_count++;
        }
    } else {
        if (sensor_backing_count) {
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
    if (bw_motor_arm_request) {
        switch (bw_motor_arm_state) {
        case BW_MOTOR_ARM_STATE_START:
            if (!sensor_flags.bit.pick_arm) {
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_SEEK_HOME;
            } else {
                bw_motor_arm_state = 1;
            }
            break;
        case 1:
            if (sensor_flags.bit.pick_arm) {
                bw_motor_arm_delay = bw_motor_arm_delay_ticks;
                bw_motor_arm_state = 2;
                set_imask_exr(7);
                bw_motor_arm_start_leave();
                set_imask_exr(bw_motor_saved_imask);
            } else {
                bw_motor_arm_request = 0;
            }
            break;
        case 2:
            if (bw_motor_arm_delay == 0) {
                bw_motor_arm_steps = bw_motor_arm_max_steps;
                bw_motor_arm_state = 3;
                set_imask_exr(7);
                bw_motor_arm_run(0);
                set_imask_exr(bw_motor_saved_imask);
            } else {
                break;
            }
        case 3:
            if (!sensor_flags.bit.pick_arm) {
                if (bw_motor_arm_request == BW_MOTOR_ARM_REHOME_NEXT_LEVEL) {
                    bw_motor_arm_steps = bw_motor_arm_base_steps + bw_motor_arm_saved_position;
                    bw_motor_arm_position = bw_motor_arm_saved_position;
                } else if (bw_motor_arm_request == BW_MOTOR_ARM_HOME) {
                    bw_motor_arm_steps = 10;
                    bw_motor_arm_position = 0;
                } else if (bw_motor_arm_steps_once) {
                    bw_motor_arm_steps = bw_motor_arm_steps_once;
                    bw_motor_arm_position = bw_motor_arm_position_once;
                    bw_motor_arm_steps_once = 0;
                } else {
                    bw_motor_arm_steps = bw_motor_arm_default_steps;
                    bw_motor_arm_position = bw_motor_arm_default_position;
                    bw_motor_arm_lifted = 0;
                    if (mailbox.settings_a6) {
                        if (mailbox.settings_a6 & 0x80) {
                            bw_motor_arm_steps -= (unsigned char)(mailbox.settings_a6 & 0x7f);
                        } else {
                            bw_motor_arm_steps += mailbox.settings_a6;
                        }
                    }
                }
                bw_motor_arm_state = 4;
            } else {
                if (bw_motor_arm_steps == 0) {
                    set_imask_exr(7);
                    if (!(main_flags_1.b & MAIN_FLAG_ADF_COVER_OPEN) && !sensor_flags.bit.adf_cover_open) {
                        err_flags.bit.device = 1;
                        err_saved_flags = err_flags.b;
                        err_saved_code = err_code = ERR_PICK_ARM;
                    }
                    bw_motor_arm_stop();
                    set_imask_exr(bw_motor_saved_imask);
                    bw_motor_arm_request = 0;
                    bw_motor_arm_state = BW_MOTOR_ARM_STATE_START;
                } else {
                    bw_motor_arm_steps--;
                }
            }
            break;
        case 4:
            if (bw_motor_arm_steps == 0) {
                set_imask_exr(7);
                bw_motor_arm_stop();
                set_imask_exr(bw_motor_saved_imask);
                if (bw_motor_arm_request == BW_MOTOR_ARM_HOME || bw_motor_arm_request == BW_MOTOR_ARM_TEST) {
                    bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_LEVEL;
                } else if (bw_motor_arm_request == BW_MOTOR_ARM_REHOME_NEXT_LEVEL) {
                    bw_motor_arm_request = BW_MOTOR_ARM_TO_LEVEL;
                    bw_motor_arm_delay = bw_motor_arm_settle_ticks;
                    bw_motor_arm_state = 5;
                } else {
                    bw_motor_arm_delay = bw_motor_arm_settle_ticks;
                    bw_motor_arm_state = 5;
                }
            } else {
                bw_motor_arm_steps--;
            }
            break;
        case 5:
            if (bw_motor_arm_delay == 0) {
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_LEVEL;
            }
            break;
        case BW_MOTOR_ARM_STATE_AT_LEVEL:
            if (bw_motor_arm_request == BW_MOTOR_ARM_HOME || bw_motor_arm_request == BW_MOTOR_ARM_TEST) {
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_SEEK_HOME;
            } else if (bw_motor_arm_request == BW_MOTOR_ARM_LEAVE_RETRY) {
                bw_motor_arm_state = 0xc;
            } else if (bw_motor_arm_request == BW_MOTOR_ARM_LIFT) {
                if (bw_motor_arm_mode) {
                    if (bw_motor_arm_lifted) {
                        bw_motor_arm_request = 0;
                    } else {
                        bw_motor_arm_state = 0x10;
                    }
                } else {
                    if (bw_motor_arm_position == 0) {
                        bw_motor_arm_request = 0;
                    } else {
                        bw_motor_arm_state = 0x10;
                    }
                }
            } else if (bw_motor_arm_request == BW_MOTOR_ARM_NEXT_LEVEL) {
                bw_motor_arm_state = 0x14;
            } else if (bw_motor_arm_request == BW_MOTOR_ARM_SEEK_STEP) {
                bw_motor_arm_state = 0x18;
            } else if (bw_motor_arm_request == BW_MOTOR_ARM_REHOME_NEXT_LEVEL) {
                if (bw_motor_arm_position < bw_motor_arm_limit) {
                    bw_motor_arm_position += bw_motor_arm_level_steps;
                }
                bw_motor_arm_saved_position = bw_motor_arm_position;
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_SEEK_HOME;
            } else {
                bw_motor_arm_request = 0;
            }
            break;
        case BW_MOTOR_ARM_STATE_SEEK_HOME:
            bw_motor_arm_position = 0;
            if (!sensor_flags.bit.pick_arm) {
                bw_motor_arm_delay = bw_motor_arm_delay_ticks;
                bw_motor_arm_state = 8;
                set_imask_exr(7);
                bw_motor_arm_start_seek();
                set_imask_exr(bw_motor_saved_imask);
            } else {
                bw_motor_arm_request = 0;
            }
            break;
        case 8:
            if (bw_motor_arm_delay == 0) {
                bw_motor_arm_steps = bw_motor_arm_max_steps;
                bw_motor_arm_state = 9;
                set_imask_exr(7);
                bw_motor_arm_run_seek();
                set_imask_exr(bw_motor_saved_imask);
            } else {
                break;
            }
        case 9:
            if (sensor_flags.bit.pick_arm) {
                bw_motor_arm_steps = bw_motor_arm_on_steps;
                bw_motor_arm_state = 10;
            } else {
                if (bw_motor_arm_steps == 0) {
                    set_imask_exr(7);
                    if (!(main_flags_1.b & MAIN_FLAG_ADF_COVER_OPEN) && !sensor_flags.bit.adf_cover_open) {
                        err_flags.bit.device = 1;
                        err_saved_flags = err_flags.b;
                        err_saved_code = err_code = ERR_PICK_ARM;
                    }
                    bw_motor_arm_stop();
                    set_imask_exr(bw_motor_saved_imask);
                    bw_motor_arm_request = 0;
                    bw_motor_arm_state = BW_MOTOR_ARM_STATE_START;
                } else {
                    bw_motor_arm_steps--;
                }
            }
            break;
        case 10:
            if (bw_motor_arm_steps == 0) {
                bw_motor_arm_stop();
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_HOME;
                if (bw_motor_arm_request != BW_MOTOR_ARM_TEST && bw_motor_arm_request != BW_MOTOR_ARM_REHOME_NEXT_LEVEL) {
                    bw_motor_arm_request = 0;
                }
            } else {
                bw_motor_arm_steps--;
            }
            break;
        case BW_MOTOR_ARM_STATE_AT_HOME:
            if (bw_motor_arm_request == BW_MOTOR_ARM_TO_LEVEL || bw_motor_arm_request == BW_MOTOR_ARM_TEST) {
                bw_motor_arm_state = 1;
            } else if (bw_motor_arm_request == BW_MOTOR_ARM_REHOME_NEXT_LEVEL) {
                bw_motor_arm_state = 1;
            } else {
                bw_motor_arm_request = 0;
            }
            break;
        case 12:
            if (bw_motor_arm_position < bw_motor_arm_limit_request_4) {
                bw_motor_arm_delay = bw_motor_arm_delay_ticks;
                set_imask_exr(7);
                bw_motor_arm_start_leave();
                set_imask_exr(bw_motor_saved_imask);
                bw_motor_arm_state = 0xd;
            } else {
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_LEVEL;
                bw_motor_arm_request = 0;
            }
            break;
        case 13:
            if (bw_motor_arm_delay == 0) {
                bw_motor_arm_steps = bw_motor_arm_request_4_steps;
                bw_motor_arm_position += bw_motor_arm_request_4_steps;
                if (bw_motor_arm_mode) {
                    if (bw_motor_arm_lifted == 1) {
                        bw_motor_arm_lifted = 0;
                        bw_motor_arm_steps = bw_motor_arm_extra_steps;
                        bw_motor_arm_position = bw_motor_arm_level_0;
                    }
                }
                set_imask_exr(7);
                bw_motor_arm_run(1);
                set_imask_exr(bw_motor_saved_imask);
                bw_motor_arm_state = 0xe;
            }
            break;
        case 14:
            if (bw_motor_arm_steps == 0) {
                bw_motor_arm_delay = bw_motor_arm_delay_ticks;
                p_chip_a_motor->control_2.bit.b6 = 0;
                bw_motor_arm_state = 0xf;
            } else {
                bw_motor_arm_steps--;
            }
            break;
        case 15:
            if (bw_motor_arm_delay == 0) {
                bw_motor_arm_stop();
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_LEVEL;
                bw_motor_arm_request = 0;
            }
            break;
        case 16:
            bw_motor_arm_delay = bw_motor_arm_delay_ticks;
            set_imask_exr(7);
            bw_motor_arm_start_seek();
            set_imask_exr(bw_motor_saved_imask);
            bw_motor_arm_state = 0x11;
            break;
        case 17:
            if (bw_motor_arm_delay == 0) {
                if (bw_motor_arm_mode) {
                    if (util_arm_lift_position == 0xffff) {
                        bw_motor_arm_steps = bw_motor_arm_position + bw_motor_arm_extra_steps;
                        bw_motor_arm_position = 0;
                        set_imask_exr(7);
                        bw_motor_arm_run_seek();
                        set_imask_exr(bw_motor_saved_imask);
                        bw_motor_arm_state = 0x12;
                        bw_motor_arm_lifted = 1;
                    } else if (bw_motor_arm_position > util_arm_lift_position) {
                        bw_motor_arm_steps = bw_motor_arm_position - util_arm_lift_position;
                        bw_motor_arm_position = util_arm_lift_position;
                        set_imask_exr(7);
                        bw_motor_arm_run_seek();
                        set_imask_exr(bw_motor_saved_imask);
                        bw_motor_arm_state = 0x12;
                        bw_motor_arm_lifted = 2;
                    } else {
                        bw_motor_arm_steps = 0;
                        bw_motor_arm_state = 0x12;
                        bw_motor_arm_lifted = 2;
                    }
                } else {
                    if (bw_motor_arm_position > bw_motor_arm_lift_limit) {
                        bw_motor_arm_steps = bw_motor_arm_lift_limit;
                        bw_motor_arm_position -= bw_motor_arm_lift_limit;
                    } else {
                        bw_motor_arm_steps = bw_motor_arm_position;
                        bw_motor_arm_position = 0;
                    }
                    set_imask_exr(7);
                    bw_motor_arm_run_seek();
                    set_imask_exr(bw_motor_saved_imask);
                    bw_motor_arm_state = 0x12;
                }
            }
            break;
        case 18:
            if (bw_motor_arm_steps == 0) {
                bw_motor_arm_delay = bw_motor_arm_delay_ticks;
                p_chip_a_motor->control_2.bit.b6 = 0;
                bw_motor_arm_state = 0x13;
            } else {
                bw_motor_arm_steps--;
            }
            break;
        case 19:
            if (bw_motor_arm_delay == 0) {
                bw_motor_arm_stop();
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_LEVEL;
                bw_motor_arm_request = 0;
            }
            break;
        case 20:
            if (bw_motor_arm_position < bw_motor_arm_limit) {
                bw_motor_arm_delay = bw_motor_arm_delay_ticks;
                set_imask_exr(7);
                bw_motor_arm_start_leave();
                set_imask_exr(bw_motor_saved_imask);
                bw_motor_arm_state = 0x15;
            } else {
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_LEVEL;
                bw_motor_arm_request = 0;
            }
            break;
        case 21:
            if (bw_motor_arm_delay == 0) {
                if (bw_motor_arm_mode) {
                    if (bw_motor_arm_lifted == 1) {
                        bw_motor_arm_steps = bw_motor_arm_extra_steps + bw_motor_arm_level_1;
                        bw_motor_arm_position = bw_motor_arm_level_1;
                    } else {
                        bw_motor_arm_steps = bw_motor_arm_request_6_steps;
                        bw_motor_arm_position += bw_motor_arm_request_6_steps;
                    }
                } else {
                    bw_motor_arm_steps = bw_motor_arm_level_steps;
                    bw_motor_arm_position += bw_motor_arm_level_steps;
                }
                set_imask_exr(7);
                bw_motor_arm_run(1);
                set_imask_exr(bw_motor_saved_imask);
                bw_motor_arm_state = 0x16;
            }
            break;
        case 22:
            if (bw_motor_arm_steps == 0) {
                bw_motor_arm_delay = bw_motor_arm_delay_ticks;
                p_chip_a_motor->control_2.bit.b6 = 0;
                bw_motor_arm_state = 0x17;
            } else {
                bw_motor_arm_steps--;
            }
            break;
        case 23:
            if (bw_motor_arm_delay == 0) {
                bw_motor_arm_stop();
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_LEVEL;
                bw_motor_arm_request = 0;
            }
            break;
        case 24:
            if (bw_motor_arm_position > bw_motor_arm_level_0) {
                bw_motor_arm_delay = bw_motor_arm_delay_ticks;
                set_imask_exr(7);
                bw_motor_arm_start_seek();
                set_imask_exr(bw_motor_saved_imask);
                bw_motor_arm_state = 0x19;
            } else {
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_LEVEL;
                bw_motor_arm_request = 0;
            }
            break;
        case 25:
            if (bw_motor_arm_delay == 0) {
                bw_motor_arm_steps = bw_motor_arm_request_7_steps;
                bw_motor_arm_state = 0x1a;
                set_imask_exr(7);
                bw_motor_arm_run_seek();
                set_imask_exr(bw_motor_saved_imask);
            }
            break;
        case 26:
            if (bw_motor_arm_steps == 0) {
                bw_motor_arm_delay = bw_motor_arm_delay_ticks;
                p_chip_a_motor->control_2.bit.b6 = 0;
                bw_motor_arm_position -= bw_motor_arm_request_7_steps;
                bw_motor_arm_state = 0x1b;
            } else {
                bw_motor_arm_steps--;
            }
            break;
        case 27:
            if (bw_motor_arm_delay == 0) {
                bw_motor_arm_stop();
                bw_motor_arm_state = BW_MOTOR_ARM_STATE_AT_LEVEL;
                bw_motor_arm_request = 0;
            }
            break;
        case 0xf0:
            bw_motor_arm_run_seek();
            bw_motor_arm_state = 0xf1;
            break;
        case 0xf1:
            if (bw_motor_arm_request) {
                bw_motor_arm_stop();
            }
            break;
        }
    } else if (bw_motor_backing_request) {
        {
            switch (bw_motor_backing_state) {
            case 0:
                mailbox.backing_position = MDC_BACKING_MOVING;
                if (sensor_flags.bit.backing) {
                    bw_motor_backing_state = 6;
                } else {
                    bw_motor_backing_state = 1;
                }
                bw_motor_backing_set_period();
                break;
            case 1:
                bw_motor_backing_retries = 1;
            case 17:
            case 18:
                bw_motor_backing_delay = bw_motor_backing_delay_ticks + 1;
                bw_motor_backing_state = 0x13;
                bw_motor_backing_start_forward();
                break;
            case 19:
                if (bw_motor_backing_delay == 0) {
                    bw_motor_backing_steps = bw_motor_backing_max_steps;
                    bw_motor_backing_state = 2;
                    bw_motor_backing_run();
                } else {
                    break;
                }
            case 2:
                if (sensor_flags.bit.backing) {
                    bw_motor_backing_steps = bw_motor_backing_on_steps;
                    bw_motor_backing_state = 3;
                } else {
                    if (bw_motor_backing_steps == 0) {
                        bw_motor_backing_stop();
                        if (bw_motor_backing_retries == 0) {
                            if (!(main_flags_1.b & MAIN_FLAG_ADF_COVER_OPEN) && !sensor_flags.bit.adf_cover_open) {
                                bw_motor_record_backing(0x81);
                                if (!bw_motor_backing_quiet) {
                                    err_flags.bit.device = 1;
                                    err_saved_flags = err_flags.b;
                                    err_saved_code = err_code = ERR_BACKING;
                                }
                            }
                            bw_motor_backing_quiet = 0;
                            bw_motor_backing_request = 0;
                            bw_motor_backing_state = 0;
                        } else {
                            bw_motor_record_backing(0x41);
                            bw_motor_backing_retries--;
                            bw_motor_backing_state = 0x11;
                        }
                    } else {
                        bw_motor_backing_steps--;
                    }
                }
                break;
            case 3:
                if (bw_motor_backing_steps == 0) {
                    bw_motor_backing_halt();
                    if (bw_motor_backing_request == BW_MOTOR_BACKING_TEST) {
                        bw_motor_backing_delay = bw_motor_backing_delay_ticks;
                        mailbox.backing_position = MDC_BACKING_WHITE;
                        bw_motor_backing_state = 4;
                    } else {
                        mailbox.backing_position = MDC_BACKING_WHITE;
                        bw_motor_backing_state = 0x14;
                        bw_motor_backing_delay = bw_motor_backing_delay_ticks + 1;
                    }
                    bw_motor_backing_quiet = 0;
                } else {
                    bw_motor_backing_steps--;
                }
                break;
            case 4:
                if (bw_motor_backing_delay == 0) {
                    mailbox.backing_position = MDC_BACKING_AT_SENSOR;
                    bw_motor_backing_state = 6;
                }
                break;
            case 20:
                if (bw_motor_backing_delay == 0) {
                    bw_motor_backing_stop();
                    bw_motor_backing_state = 5;
                } else {
                    break;
                }
            case 5:
                if (bw_motor_backing_request == BW_MOTOR_BACKING_BLACK) {
                    mailbox.backing_position = MDC_BACKING_AT_SENSOR;
                    bw_motor_backing_state = 6;
                    bw_motor_backing_set_period();
                } else {
                    mailbox.backing_position = MDC_BACKING_WHITE;
                    bw_motor_backing_request = 0;
                    bw_motor_backing_quiet = 0;
                }
                break;
            case 6:
                bw_motor_backing_retries = 1;
            case 22:
            case 23:
                bw_motor_backing_delay = bw_motor_backing_delay_ticks + 1;
                bw_motor_backing_state = 0x18;
                bw_motor_backing_start_reverse();
                break;
            case 24:
                if (bw_motor_backing_delay == 0) {
                    bw_motor_backing_steps = bw_motor_backing_max_steps;
                    bw_motor_backing_state = 7;
                    bw_motor_backing_run_reverse();
                } else {
                    break;
                }
            case 7:
                if (!sensor_flags.bit.backing) {
                    bw_motor_backing_steps = bw_motor_backing_off_steps;
                    bw_motor_backing_state = 8;
                } else {
                    if (bw_motor_backing_steps == 0) {
                        bw_motor_backing_stop();
                        if (bw_motor_backing_retries == 0) {
                            if (!(main_flags_1.b & MAIN_FLAG_ADF_COVER_OPEN) && !sensor_flags.bit.adf_cover_open) {
                                bw_motor_record_backing(0x82);
                                err_flags.bit.device = 1;
                                err_saved_flags = err_flags.b;
                                err_saved_code = err_code = ERR_BACKING;
                            }
                            bw_motor_backing_request = 0;
                            bw_motor_backing_state = 0;
                        } else {
                            bw_motor_record_backing(0x42);
                            bw_motor_backing_retries--;
                            bw_motor_backing_state = 0x16;
                        }
                    } else {
                        bw_motor_backing_steps--;
                    }
                }
                break;
            case 8:
                if (bw_motor_backing_steps == 0) {
                    bw_motor_backing_halt();
                    mailbox.backing_position = MDC_BACKING_BLACK;
                    bw_motor_backing_state = 0x19;
                    bw_motor_backing_delay = bw_motor_backing_delay_ticks + 1;
                } else {
                    bw_motor_backing_steps--;
                }
                break;
            case 25:
                if (bw_motor_backing_delay == 0) {
                    bw_motor_backing_stop();
                    bw_motor_backing_state = 9;
                } else {
                    break;
                }
            case 9:
                if (bw_motor_backing_request == BW_MOTOR_BACKING_WHITE || bw_motor_backing_request == BW_MOTOR_BACKING_TEST) {
                    mailbox.backing_position = MDC_BACKING_MOVING;
                    bw_motor_backing_state = 1;
                    bw_motor_backing_set_period();
                } else {
                    mailbox.backing_position = MDC_BACKING_BLACK;
                    bw_motor_backing_request = 0;
                }
                break;
            case 10:
                set_imask_exr(7);
                motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_BACKING_RUN);
                set_imask_exr(bw_motor_saved_imask);
                bw_motor_backing_run_reverse();
                bw_motor_backing_state = 0xb;
                break;
            case 11:
                if (bw_motor_backing_request) {
                    bw_motor_backing_stop();
                }
                break;
            }
        }
    }
}

void bw_motor_arm_run_seek(void)
{
    p_chip_a_motor->period_2_high.f_0_8.f = bw_motor_arm_period_seek >> 8;
    p_chip_a_motor->period_2_low.f_0_8.f = bw_motor_arm_period_seek;
    p_chip_a_motor->control_2.bit.b6 = 1;
}

void bw_motor_arm_start_seek(void)
{
    p_chip_b_uart->mcr.bit.b0 = 1;
    p_chip_b_uart->mcr.bit.b1 = 1;
    motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_START);
    p_chip_a_motor->control_2.f_4_2.f = 1;
    p_chip_a_motor->control_2.bit.b2 = 1;
    p_chip_a_motor->phase_2.b = bw_motor_arm_phase.b;
    p_chip_a_motor->control_2.bit.b6 = 0;
    p_chip_a_motor->control_2.bit.b3 = 1;
    p_chip_b_uart->mcr.bit.b0 = 0;
    p_chip_b_uart->mcr.bit.b1 = 1;
}

void bw_motor_arm_run(unsigned char sel)
{
    bw_motor_arm_lifted = 0;
    if (sel) {
        p_chip_a_motor->period_2_high.f_0_8.f = bw_motor_arm_period_return >> 8;
        p_chip_a_motor->period_2_low.f_0_8.f = bw_motor_arm_period_return;
    } else {
        p_chip_a_motor->period_2_high.f_0_8.f = bw_motor_arm_period >> 8;
        p_chip_a_motor->period_2_low.f_0_8.f = bw_motor_arm_period;
    }
    p_chip_a_motor->control_2.bit.b6 = 1;
}

void bw_motor_arm_start_leave(void)
{
    p_chip_b_uart->mcr.bit.b0 = 1;
    p_chip_b_uart->mcr.bit.b1 = 1;
    motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_BOOST);
    p_chip_a_motor->control_2.f_4_2.f = 1;
    p_chip_a_motor->control_2.bit.b2 = 0;
    p_chip_a_motor->phase_2.b = bw_motor_arm_phase.b;
    p_chip_a_motor->control_2.bit.b6 = 0;
    p_chip_a_motor->control_2.bit.b3 = 1;
    p_chip_b_uart->mcr.bit.b0 = 0;
    p_chip_b_uart->mcr.bit.b1 = 1;
}

void bw_motor_arm_stop(void)
{
    p_da->DR0 = motor_current_tables[2][1][7];
    p_chip_b_uart->mcr.bit.b0 = 1;
    p_chip_b_uart->mcr.bit.b1 = 1;
    motor_set_current(MOTOR_PICK_ARM, MOTOR_CURRENT_HOLD);
    p_chip_a_motor->period_2_high.f_0_8.f = 0x29;
    p_chip_a_motor->period_2_low.f_0_8.f = 0x82;
    p_chip_a_motor->control_2.bit.b6 = 0;
    p_chip_a_motor->control_2.bit.b3 = 0;
    bw_motor_arm_phase.b = p_chip_a_motor->phase_2.b;
    bw_motor_arm_phase.f_5_3.f = 5;
}

void bw_motor_backing_run_reverse(void)
{
    p_chip_a_motor->control_2.bit.b6 = 1;
}

void bw_motor_backing_start_reverse(void)
{
    p_chip_b_uart->mcr.bit.b0 = 1;
    p_chip_b_uart->mcr.bit.b1 = 1;
    set_imask_exr(7);
    motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_BACKING_RUN);
    set_imask_exr(bw_motor_saved_imask);
    p_chip_a_motor->control_2.f_4_2.f = 1;
    p_chip_a_motor->control_2.bit.b2 = 0;
    p_chip_a_motor->period_2_high.f_0_8.f = bw_motor_backing_period >> 8;
    p_chip_a_motor->period_2_low.f_0_8.f = bw_motor_backing_period;
    p_chip_a_motor->phase_2.b = bw_motor_backing_phase;
    p_chip_a_motor->control_2.bit.b6 = 0;
    p_chip_a_motor->control_2.bit.b3 = 1;
    p_chip_b_uart->mcr.bit.b0 = 1;
    p_chip_b_uart->mcr.bit.b1 = 0;
}

void bw_motor_backing_run(void)
{
    p_chip_a_motor->control_2.bit.b6 = 1;
}

void bw_motor_backing_start_forward(void)
{
    p_chip_b_uart->mcr.bit.b0 = 1;
    p_chip_b_uart->mcr.bit.b1 = 1;
    set_imask_exr(7);
    motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_BACKING_RUN);
    set_imask_exr(bw_motor_saved_imask);
    p_chip_a_motor->control_2.f_4_2.f = 1;
    p_chip_a_motor->control_2.bit.b2 = 1;
    p_chip_a_motor->period_2_high.f_0_8.f = bw_motor_backing_period >> 8;
    p_chip_a_motor->period_2_low.f_0_8.f = bw_motor_backing_period;
    p_chip_a_motor->phase_2.b = bw_motor_backing_phase;
    p_chip_a_motor->control_2.bit.b6 = 0;
    p_chip_a_motor->control_2.bit.b3 = 1;
    p_chip_b_uart->mcr.bit.b0 = 1;
    p_chip_b_uart->mcr.bit.b1 = 0;
}

void bw_motor_backing_set_period(void)
{
    p_chip_a_motor->period_2_high.f_0_8.f = bw_motor_backing_period >> 8;
    p_chip_a_motor->period_2_low.f_0_8.f = bw_motor_backing_period;
}

void bw_motor_backing_halt(void)
{
    p_chip_a_motor->control_2.bit.b6 = 0;
}

void bw_motor_backing_stop(void)
{
    p_chip_b_uart->mcr.bit.b0 = 1;
    p_chip_b_uart->mcr.bit.b1 = 1;
    motor_set_current(MOTOR_BACKING, MOTOR_CURRENT_HOLD);
    p_chip_a_motor->period_2_high.f_0_8.f = 0x29;
    p_chip_a_motor->period_2_low.f_0_8.f = 0x82;
    bw_motor_backing_phase = p_chip_a_motor->phase_2.b;
    bw_motor_backing_phase = bw_motor_backing_phase & 0x1f | 0xa0;
    p_chip_a_motor->control_2.bit.b6 = 0;
    p_chip_a_motor->control_2.bit.b3 = 0;
}

void bw_motor_record_backing(unsigned char code)
{
    if (code & 0x40) {
        mailbox.backing_trace_code |= code;
    }
    if (code & 0x80) {
        mailbox.backing_trace_code = code;
        mailbox.backing_trace_mode = mailbox.feed_backing_request;
        mailbox.backing_trace_request = bw_motor_backing_request;
        mailbox.backing_trace_state = bw_motor_backing_state;
        util_put_be16(&mailbox.backing_trace_steps, bw_motor_backing_steps);
        util_put_be16(&mailbox.backing_trace_timeout, cmd_timeout);
    }
}
