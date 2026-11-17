#include "fw.h"
#pragma section H

extern void _builtin_set_imask(int);

void testmode_pick_arm(void)
{
    int done;
    if (scsi_resume_flags.f.resume_pending) {
        isr_request_command_layer(0);
    }
    done = 0;
    while (1) {
        _builtin_set_imask(0);
        if (testmode_param_absent == 0) {
            mdc_run_pick_arm(0, 1);
            _builtin_set_imask(15);
            testmode_number.v = 0;
            scsi_testmode_result_state.v = 1;
            _builtin_set_imask(0);
            return;
        }
        if (done == 0) {
            mdc_run_pick_arm(1, 1);
            done = 1;
        }
    }
}

void testmode_2c(void)
{
    char motor;
    int done;

    if (scsi_resume_flags.f.resume_pending) {
        isr_request_command_layer(0);
    }
    motor = MDC_MOTOR_TEST_PICK;
    done = 0;
    err_check_pending();
    while (1) {
        _builtin_set_imask(0);
        if (testmode_param_absent == 0) {
            mdc_end_motor_test(motor, testmode_param_2);
            mdc_run_pick_arm(0, 1);
            _builtin_set_imask(15);
            testmode_number.v = 0;
            scsi_testmode_result_state.v = 1;
            _builtin_set_imask(0);
            return;
        }
        if (done == 0) {
            mdc_run_pick_arm(1, 1);
            mdc_run_motor_test(motor, testmode_param_2);
            done = 1;
        }
    }
}

void testmode_save_defect_lists(void)
{
    if (mdc_mailbox.df_test_result & MDC_DF_TEST_RESULT_TOP_EDGE)
        testmode_result_1[0] = 0;
    else
        testmode_result_1[0] = 0x80;
    testmode_result_2[0] = 0;
    if (testmode_result_1[0] == 0 && testmode_result_2[0] == 0)
        err_code_list[0] = 0x80;
    else
        err_code_list[0] = 0xff;
}

void testmode_save_defect_lists_no_result(void)
{
    flash_save_settings_copy();
}

void testmode_15(void)
{
}


void testmode_save_settings_backup(void)
{
    if (hw_cga_hostif_flags.f.cga_test_scan && hw_cga_board_type <= 1)
        err_code_list[0] = 1;
    else
        err_code_list[0] = 0;
}

void testmode_save_eeprom_data(void)
{
    unsigned short i;
    unsigned short j;

    for (i = 0; i < 50; i++)
        rt_delay_60k();
    for (i = 0; i < 16; i++) {
        if (flash_backup_slot_used[i * 0x220] == 0xff)
            break;
        for (j = 0; j < 0x200; j++) {
            if (flash_backup_area[i][j] != 0)
                break;
        }
        if (flash_backup_slot_current[i * 0x220] == 0xff) {
            maint_screen = MAINT_SCREEN_WRITTEN;
            panel_set_display_mode(7, PANEL_MODE_MAINT);
            for (i = 0; i < 50; i++)
                rt_delay_60k();
            _builtin_set_imask(0);
            mdc_lamp_cover_override = 1;
            for (j = 0; j < 3; j++) {
                mdc_run_lamps(7);
                for (i = 0; i < 100; i++)
                    rt_delay_60k();
                mdc_run_lamps(0);
                for (i = 0; i < 100; i++)
                    rt_delay_60k();
            }
            mdc_lamp_cover_override = 0;
            mdc_run_lamps(0);
            return;
        }
    }
    if (settings_byte_0x1e[0] & 0x80) {
        maint_screen = MAINT_SCREEN_FAILED;
        panel_set_display_mode(7, PANEL_MODE_MAINT);
        return;
    }
    while (mdc_mailbox.sensor_status.f.top)
        ;
    i = 0;
    while (mdc_mailbox.sensor_status.f.adf_cover_open) {
        tick_wait_count = 0;
        while (tick_wait_count < 150) {
            if (!mdc_mailbox.sensor_status.f.top)
                break;
        }
        if (tick_wait_count >= 150) {
            i++;
            while (mdc_mailbox.sensor_status.f.top)
                tick_wait_count = 0;
        }
    }
    if (i == 2) {
        flash_save_settings_backup();
        if (flash_status.v == 0x88) {
            flash_status.v = 0;
        } else {
            maint_screen = MAINT_SCREEN_WRITING;
            panel_set_display_mode(7, PANEL_MODE_MAINT);
            tick_wait_count = 0;
            for (j = 0; j < 400; j++)
                rt_delay_60k();
        }
    }
    if (flash_status.v != 0 || i != 2) {
        maint_screen = MAINT_SCREEN_FAILED;
        panel_set_display_mode(7, PANEL_MODE_MAINT);
    } else {
        maint_screen = MAINT_SCREEN_WRITTEN;
        panel_set_display_mode(7, PANEL_MODE_MAINT);
        while (!mdc_mailbox.sensor_status.f.adf_cover_open)
            ;
        _builtin_set_imask(0);
        for (i = 0; i < 100; i++)
            rt_delay_60k();
        mdc_lamp_cover_override = 1;
        for (j = 0; j < 3; j++) {
            mdc_run_lamps(7);
            for (i = 0; i < 100; i++)
                rt_delay_60k();
            mdc_run_lamps(0);
            for (i = 0; i < 100; i++)
                rt_delay_60k();
        }
        mdc_lamp_cover_override = 0;
        mdc_run_lamps(0);
    }
}

