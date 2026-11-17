#include "fw.h"
#pragma section H

extern int _builtin_get_imask(void);
extern void _builtin_set_imask(int);
/* Constants (section C). */
const struct err_sense_entry err_hardware_sense_table[13] = {
    {0x200, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_HARDWARE_IMPRINTER},
    {0x400, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_HARDWARE_IMPRINTER_FUSE},
    {0x800, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_HARDWARE_MECHANICAL},
    {0x100, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_HARDWARE_OPTION},
    {0x80, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_HARDWARE_MECHANICAL},
    {0x40, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_HARDWARE_OPTICAL},
    {0x10, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_HARDWARE_LAMP_FUSE},
    {8, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_HARDWARE_MOTOR_CIRCUIT},
    {4, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_HARDWARE_MOTOR_FUSE},
    {2, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_HARDWARE_FAN},
    {1, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_INTERNAL_TARGET_FAILURE},
    {0x20, SCSI_SENSE_ABORTED_COMMAND, 0, SCSI_ASC_ABORTED_IMAGE_TRANSFER},
    {0, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_NONE}
};
const struct err_sense_entry err_medium_sense_table[11] = {
    {0x80, SCSI_SENSE_MEDIUM_ERROR, 0, SCSI_ASC_MEDIUM_STOP_BUTTON},
    {0x40, SCSI_SENSE_ABORTED_COMMAND, 0, SCSI_ASC_ABORTED_IMAGE_TRANSFER},
    {0x20, SCSI_SENSE_MEDIUM_ERROR, 0, SCSI_ASC_MEDIUM_NO_CARTRIDGE},
    {0x10, SCSI_SENSE_HARDWARE_ERROR, 0, SCSI_ASC_NONE},
    {8, SCSI_SENSE_MEDIUM_ERROR, 0, SCSI_ASC_MEDIUM_IMPRINTER},
    {4, SCSI_SENSE_MEDIUM_ERROR, 0, SCSI_ASC_MEDIUM_COVER_OPEN},
    {2, SCSI_SENSE_MEDIUM_ERROR, 0, SCSI_ASC_MEDIUM_HOPPER_EMPTY},
    {1, SCSI_SENSE_MEDIUM_ERROR, 0, SCSI_ASC_MEDIUM_PAPER_JAM},
    {0x200, SCSI_SENSE_MEDIUM_ERROR, 0, SCSI_ASC_MEDIUM_SCAN_PAUSED},
    {0x400, SCSI_SENSE_MEDIUM_ERROR, 0, SCSI_ASC_MEDIUM_PAUSED_BY_USER},
    {0, SCSI_SENSE_ABORTED_COMMAND, 0, SCSI_ASC_NONE}
};
const int err_class_table[256] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0xff,
    0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0xff, 0xff,
    0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
    2, 0xff, 0xff, 0xff, 0xb, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 0xff, 0xff, 0xff, 0xff,
    3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 3, 0xff, 0xff, 0xff, 0xff, 0xff,
    1, 0xff, 0xff, 0xff, 0xff, 6, 6, 1,
    6, 0xff, 1, 0xff, 0xff, 0xff, 0xff, 0xff,
    4, 4, 4, 4, 4, 4, 4, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x18, 0x19,
    7, 0x16, 0x16, 0xff, 0xff, 0xff, 0x16, 0xff,
    0x13, 0x13, 0x13, 0x13, 0x15, 0xff, 0xff, 0x17,
    0x13, 0x13, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x11, 0x11, 0x11, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x17, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x10, 0x12, 0x10, 0x10, 8, 0x10, 0x10, 0x10,
    0x10, 5, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff,
    0x17, 0xff, 0x1b, 0x1b, 0x17, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x18, 0x1a, 0x1a, 0xff,
    0xff, 0xff, 0x17, 0x17, 0x17, 0x17, 0x17, 0x16,
    0x16, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
const unsigned char err_display_codes[256] = {
    /* 0x00 */ PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR,
    /* 0x08 */ PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR,
    /* 0x10 */ PANEL_ERROR_U0, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR,
    /* 0x18 */ PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR, PANEL_ERROR_SENSOR,
    /* 0x20 */ PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE_PAUSED, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE,
    /* 0x28 */ PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE,
    /* 0x30 */ PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1,
    /* 0x38 */ PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1,
    /* 0x40 */ PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4,
    /* 0x48 */ PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4, PANEL_ERROR_U4,
    /* 0x50 */ PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J1, PANEL_ERROR_J2, PANEL_ERROR_J2, PANEL_ERROR_J1,
    /* 0x58 */ PANEL_ERROR_J2, PANEL_ERROR_J2, PANEL_ERROR_J1, PANEL_ERROR_J2, PANEL_ERROR_J2, PANEL_ERROR_J2, PANEL_ERROR_J2, PANEL_ERROR_J2,
    /* 0x60 */ PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE,
    /* 0x68 */ PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE,
    /* 0x70 */ PANEL_ERROR_E2, PANEL_ERROR_E3, PANEL_ERROR_E2, PANEL_ERROR_E3, PANEL_ERROR_E2, PANEL_ERROR_E3, PANEL_ERROR_F6, PANEL_ERROR_NONE,
    /* 0x78 */ PANEL_ERROR_NONE, PANEL_ERROR_E1, PANEL_ERROR_E1, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_NONE, PANEL_ERROR_E1, PANEL_ERROR_NONE,
    /* 0x80 */ PANEL_ERROR_H1, PANEL_ERROR_H1, PANEL_ERROR_H1, PANEL_ERROR_H1, PANEL_ERROR_H7, PANEL_ERROR_H6, PANEL_ERROR_C0_2, PANEL_ERROR_E0,
    /* 0x88 */ PANEL_ERROR_H1, PANEL_ERROR_H1, PANEL_ERROR_H1, PANEL_ERROR_H1, PANEL_ERROR_H1, PANEL_ERROR_H1, PANEL_ERROR_H1, PANEL_ERROR_H1,
    /* 0x90 */ PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER,
    /* 0x98 */ PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER, PANEL_ERROR_HEATER,
    /* 0xA0 */ PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0,
    /* 0xA8 */ PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0,
    /* 0xB0 */ PANEL_ERROR_A, PANEL_ERROR_H6, PANEL_ERROR_A, PANEL_ERROR_A, PANEL_ERROR_U6, PANEL_ERROR_A, PANEL_ERROR_A, PANEL_ERROR_A,
    /* 0xB8 */ PANEL_ERROR_A, PANEL_ERROR_U7, PANEL_ERROR_A, PANEL_ERROR_A, PANEL_ERROR_A, PANEL_ERROR_A, PANEL_ERROR_A, PANEL_ERROR_A,
    /* 0xC0 */ PANEL_ERROR_E0, PANEL_ERROR_F2_F4, PANEL_ERROR_F2_F4, PANEL_ERROR_F2_F4, PANEL_ERROR_F2_F4, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0,
    /* 0xC8 */ PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0,
    /* 0xD0 */ PANEL_ERROR_E7, PANEL_ERROR_E7, PANEL_ERROR_E7, PANEL_ERROR_E7, PANEL_ERROR_C0, PANEL_ERROR_C0, PANEL_ERROR_C0, PANEL_ERROR_C0,
    /* 0xD8 */ PANEL_ERROR_C0, PANEL_ERROR_C0, PANEL_ERROR_C0, PANEL_ERROR_C0, PANEL_ERROR_C0, PANEL_ERROR_C0, PANEL_ERROR_C0, PANEL_ERROR_C0,
    /* 0xE0 */ PANEL_ERROR_C0_2, PANEL_ERROR_E0, PANEL_ERROR_NONE_FLASH, PANEL_ERROR_C0_2, PANEL_ERROR_E9, PANEL_ERROR_C0, PANEL_ERROR_C0, PANEL_ERROR_C0,
    /* 0xE8 */ PANEL_ERROR_E9, PANEL_ERROR_C0_2, PANEL_ERROR_C0_2, PANEL_ERROR_NONE_FLASH, PANEL_ERROR_F6, PANEL_ERROR_NONE_IPC, PANEL_ERROR_E9_2, PANEL_ERROR_NONE_INTERFACE,
    /* 0xF0 */ PANEL_ERROR_C8, PANEL_ERROR_C0_2, PANEL_ERROR_F2_F4, PANEL_ERROR_E0, PANEL_ERROR_NONE_INTERFACE, PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_E0,
    /* 0xF8 */ PANEL_ERROR_E0, PANEL_ERROR_E0, PANEL_ERROR_C8, PANEL_ERROR_C8, PANEL_ERROR_C8, PANEL_ERROR_C8, PANEL_ERROR_C8, PANEL_ERROR_C8
};
const unsigned short err_count_offsets[256] = {
    0x162, 0x162, 0x162, 0x162, 0x162, 0x162, 0x162, 0x162,
    0x162, 0x162, 0x162, 0x162, 0x162, 0x162, 0x162, 0x162,
    0xffff, 0x162, 0x162, 0x162, 0x162, 0x162, 0x162, 0x162,
    0x162, 0x162, 0x162, 0x162, 0x162, 0x162, 0x162, 0x162,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0x160, 0x161, 0x160, 0x161, 0x160, 0x161, 0xffff, 0x166,
    0xffff, 0x15f, 0x15f, 0xffff, 0xffff, 0xffff, 0x15f, 0xffff,
    0x164, 0x164, 0x164, 0x164, 0x165, 0x16a, 0xffff, 0xffff,
    0x164, 0x164, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0x16a, 0x16a, 0x16a, 0x16a, 0xffff, 0x16a, 0x16a, 0x16a,
    0x16a, 0xffff, 0x16a, 0x16a, 0x16a, 0x16a, 0x16a, 0x16a,
    0x162, 0x162, 0x16b, 0x16b, 0x162, 0x162, 0x162, 0x162,
    0x162, 0x162, 0x162, 0x162, 0x162, 0x162, 0x162, 0x162,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0x167, 0xffff, 0xffff, 0x167, 0x167, 0x167, 0x167, 0x167,
    0x167, 0x167, 0x167, 0xffff, 0x163, 0xffff, 0xffff, 0xffff,
    0x162, 0x162, 0x16b, 0x16b, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
};

int err_check_pending(void)
{
    unsigned char code;
    int ok;
    ok = 1;
    if (power_state.f.saving)
        return ok;
    if (hw_hostif_flags.f.own_hostif && settings_manual_feed_timeout[0] != 0) {
        if (main_page_loop_phase == 0 && page_side_state[0].last_buf) {
        } else {
            if (*(unsigned short *)((unsigned char *)&job_manual_feed_flags) & 0x8000)
                return ok;
        }
    }
    if (err_pending_flags.v != 0) {
        if (err_pending_flags.f.panel_expander) {
            code = 0;
            err_pending_flags.f.panel_expander = 0;
        }
        if (err_pending_flags.f.eeprom) {
            code = 210;
            err_pending_flags.f.eeprom = 0;
        }
        if (!(settings_byte_0x7f[0] & 128) && err_pending_flags.f.scsi_interface) {
            code = 239;
            err_pending_flags.f.scsi_interface = 0;
        }
        if (err_pending_flags.f.cga_board) {
            code = 138;
            err_pending_flags.f.cga_board = 0;
        }
        if (err_pending_flags.f.image_memory) {
            code = 228;
            err_pending_flags.f.image_memory = 0;
        }
        err_raise(code);
        mdc_run_command(MDC_CMD_CLEAR_ERRORS);
        ok = 0;
        return ok;
    }
    code = mdc_mailbox.error_code.v;
    if (code == 64) {
        if (mdc_is_adf_cover_open() != 0) {
            if (err_feed_params_pending == 0) {
                mdc_run_command(MDC_CMD_CLEAR_ERRORS);
                code = 0;
            }
        } else {
            mdc_run_command(MDC_CMD_CLEAR_ERRORS);
            code = 0;
        }
    }
    err_feed_params_pending = 0;
    if (code == 32) {
        if (mdc_is_paper_in_hopper() == 0) {
            if (job_feed_starting == 0) {
                mdc_run_command(MDC_CMD_CLEAR_ERRORS);
                code = 0;
            }
        } else {
            mdc_run_command(MDC_CMD_CLEAR_ERRORS);
            code = 0;
        }
    }
    if (code == 16)
        err_shipping_lock_pending = 1;
    if (code != 0) {
        err_raise(code);
        mdc_run_command(MDC_CMD_CLEAR_ERRORS);
        ok = 0;
    }
    if (code == 233) {  /* 0xE9: the MDC's analog input alarm */
        err_mdc_alarm = 1;
        power_state.v |= 128;
    }
    return ok;
}


void err_raise(unsigned char code)
{
    struct page_record *page;
    int i;
    page = &page_records[page_scanning.v];
    log_add_event(4, code);
    log_add_record(4, code, 0, 0);
    if (main_page_loop_phase == 0 && page_side_state[0].last_buf != 0)
        page = &page_side_records[page_side_state[0].capture_buf];
    if (code == ERR_PICK && (job_dirty_roller_flags.v & 256)) {
        code = ERR_JAM_DIRTY_ROLLERS;
        job_dirty_roller_flags.v &= 65279;
    }
    if (job_pause_requested && code == ERR_NO_PAPER)
        code = ERR_PAUSED_BY_USER;
    if (job_pause_requested && code == ERR_PAUSED_BY_USER)
        job_set_prepick(job_mode_params[1].prepick, 1);
    job_pause_requested = 0;
    job_pause_page_count.v = 0;
    if (code == ERR_MDC_TIMEOUT || code == 241) {
        if (settings_error_count[0] != 255)
            settings_error_count[0] += 1;
        if (power_mdc_timeout_count != 255)
            power_mdc_timeout_count += 1;
        if (power_mdc_restart_state.f.retry_state != 2) {
            power_mdc_restart_request = 255;
            code = ERR_PICK_SENSOR_JAM;
            power_mdc_restart_state.f.timeout_error = 1;
            power_mdc_restart_state.f.retry_allowed = 1;
        }
    }
    if (code == ERR_STOPPED && (settings_autocrop_boundary[0] & 4))
        code = ERR_JAM;
    err_last_code.s.v = code;
    if (code == ERR_ULTRASONIC_SENSOR)
        err_ultrasonic_error = 1;
    if (testmode_feed_test != 0) {
        err_raise_step_4(code);
        return;
    }
    if (testmode_number.v == 48) {
        err_add_to_list(code);
        return;
    }
    page->error_kind = 0;
    switch (err_class_table[code]) {
    case 1:
        err_set_medium_error(1, PANEL_MODE_ERROR, PANEL_ERROR_J1);
        break;
    case 2:
        err_set_medium_error(2, PANEL_MODE_ERROR, PANEL_ERROR_NONE);
        break;
    case 3:
        err_set_medium_error(4, PANEL_MODE_ERROR, PANEL_ERROR_U4);
        break;
    case 4:
        err_set_medium_error(2, PANEL_MODE_ERROR, PANEL_ERROR_NONE);
        break;
    case 5:
        if (hw_hostif_flags.f.own_hostif)
            err_set_medium_error(8, PANEL_MODE_ERROR, err_display_codes[code]);
        else
            err_set_medium_error(32, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 6:
        if (err_last_code.s.v == ERR_MULTIFEED_LENGTH)
            page->error_kind = 6;
        else
            page->error_kind = 7;
        err_set_medium_error(1, PANEL_MODE_ERROR, PANEL_ERROR_J2);
        break;
    case 7:
        err_set_medium_error(128, PANEL_MODE_ERROR, PANEL_ERROR_NONE);
        err_display_flags.b.b7 = 1;
        break;
    case 8:
        err_set_medium_error(32, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 11:
        if ((job_button_pause_flags & 0x8000) && (mdc_mailbox.page_end & MDC_PAGE_END_DONE))
            err_set_medium_error(1024, PANEL_MODE_ERROR, err_display_codes[code]);
        else
            err_set_medium_error(512, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 16:
        if (hw_hostif_flags.f.own_hostif)
            err_set_hardware_error(512, PANEL_MODE_ERROR, err_display_codes[code]);
        else
            err_set_hardware_error(1, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 27:
        if (hw_hostif_flags.f.own_hostif)
            err_set_hardware_error(2048, PANEL_MODE_ERROR, err_display_codes[code]);
        else
            err_set_hardware_error(128, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 17:
        if (hw_hostif_flags.f.own_hostif)
            err_set_hardware_error(1, PANEL_MODE_ERROR, err_display_codes[code]);
        else
            err_set_hardware_error(2, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 18:
        if (hw_hostif_flags.f.own_hostif)
            err_set_hardware_error(1024, PANEL_MODE_ERROR, err_display_codes[code]);
        else
            err_set_hardware_error(4, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 19:
        if (hw_hostif_flags.f.own_hostif)
            err_set_hardware_error(8, PANEL_MODE_ERROR, err_display_codes[code]);
        else
            err_set_hardware_error(4, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 21:
        err_set_hardware_error(16, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 22:
        err_set_hardware_error(64, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 23:
        err_set_hardware_error(128, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 24:
        if (hw_hostif_flags.f.own_hostif)
            err_set_hardware_error(2, PANEL_MODE_ERROR, err_display_codes[code]);
        else
            err_set_hardware_error(128, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 26:
        err_set_hardware_error(256, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    case 25:
        err_set_hardware_error(32, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    default:
        if (hw_hostif_flags.f.own_hostif)
            err_set_hardware_error(1, PANEL_MODE_ERROR, err_display_codes[code]);
        else
            err_set_hardware_error(128, PANEL_MODE_ERROR, err_display_codes[code]);
        break;
    }
}

void err_raise_step_4(unsigned char code)
{
    switch (err_class_table[code]) {
    case 1:
        err_code_list[0] = 64;
        break;
    case 2:
        err_code_list[0] = 128;
        break;
    case 3:
        err_code_list[0] = 32;
        break;
    case 4:
        err_code_list[0] = 128;
        break;
    case 5:
        break;
    case 6:
        err_code_list[0] = 16;
        break;
    case 7:
        err_code_list[0] = 8;
        break;
    case 16:
        break;
    case 17:
        break;
    case 18:
        break;
    case 19:
        testmode_result_1[0] = 16;
        break;
    case 21:
        testmode_result_1[0] = 32;
        break;
    case 22:
        if (err_display_codes[code] == 18 || err_display_codes[code] == 17)
            testmode_result_1[0] = 128;
        else
            testmode_result_1[0] = 64;
        break;
    case 23:
        testmode_result_1[0] = 8;
        if (err_display_codes[code] == 0)
            err_code_list[0] = 4;
        break;
    case 24:
        break;
    case 25:
        break;
    default:
        break;
    }
}

void err_add_to_list(unsigned char code)
{
    int i;
    i = err_code_count.v;
    while (1) {
        if (i == 0)
            break;
        i = i - 1;
        if (code == err_code_list[i])
            return;
    }
    err_code_list[err_code_count.v] = code;
    if (err_code_count.v <= 15)
        err_code_count.v += 1;
}


int err_report_page_error(void)
{
    const struct err_sense_entry *entry;
    struct page_record *page;
    unsigned short bits;
    int saved_imask;
    page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    if (err_mdc_alarm != 0) {
        err_last_code.s.v = ERR_VDCC;
        page->error_flags.c.f.hardware_error = 1;
        page->hardware_error_reported.v = page->hardware_error_bits = 1;
    }
    if (power_mdc_restart_state.f.retry_state == 2 && scsi_cdb[0] == SCSI_OP_OBJECT_POSITION) {
        panel_set_display_mode(PANEL_ERROR_C8, PANEL_MODE_ERROR);
        page->error_flags.c.f.hardware_error = 1;
        page->hardware_error_reported.v = page->hardware_error_bits = 1;
    }
    bits = page->error_flags.v;
    if ((bits & 224) == 0)
        return 1;
    if (!(bits & 128) && !(bits & 64)) {
    } else if (scsi_cdb[0] == SCSI_OP_REQUEST_SENSE) {
    } else {
        if (scsi_cdb[0] == SCSI_OP_READ && scsi_cdb_byte_2[0])
            return 1;
        if (scsi_cdb[0] == SCSI_OP_SEND_DIAGNOSTIC) {
            if (scsi_cdb_byte_1[0] & 4) {
            } else if ((bits & 128) && hw_device_errors.f.eeprom) {
            } else if (err_mdc_alarm != 0) {
                return 1;
            }
        } else if (scsi_cdb[0] == SCSI_OP_SCANNER_CONTROL && job_scan_complete_wait != 0) {
            _builtin_set_imask(15);
            job_scan_complete_wait = 0;
            _builtin_set_imask(saved_imask);
        } else if (scsi_cdb[0] != SCSI_OP_READ && scsi_cdb[0] != SCSI_OP_OBJECT_POSITION && scsi_cdb[0] != SCSI_OP_SCAN) {
            if (hw_hostif_flags.f.usb && scsi_cdb[0] == SCSI_OP_SCANNER_CONTROL && (scsi_cdb_byte_1[0] & 16)) {
            } else {
                return 1;
            }
        } else {
            if ((bits & 128) && page->hardware_error_bits == 32) {
                if (scsi_cdb[0] == SCSI_OP_READ) {
                } else {
                    return 1;
                }
            } else {
                if (scsi_cdb[0] == SCSI_OP_READ && page->medium_error_bits.v == 2)
                    return 1;
                else if (scsi_cdb[0] == SCSI_OP_READ && scsi_page_read_done != 0)
                    return 1;
                else if ((page->medium_error_bits.v == 1024 || page->medium_error_bits.v == 512) && scsi_cdb[0] != SCSI_OP_OBJECT_POSITION)
                    return 1;
            }
        }
    }
    _builtin_set_imask(15);
    page->hardware_error_reported.v = page->hardware_error_bits;
    page->medium_error_reported.v = page->medium_error_bits.v;
    page->illegal_request_reported.v = page->illegal_request_bits;
    page->error_flags.v = 0;
    page->hardware_error_bits = 0;
    page->medium_error_bits.v = 0;
    page->illegal_request_bits = 0;
    if (!(job_buffer_option & 0x8000))
        err_multifeed_code = err_last_code.s.v;
    _builtin_set_imask(saved_imask);
    if (((union view_mdc_motor_test_state *)&bits)->by.lo.b7) {
        bits = page->hardware_error_reported.v;
        if (bits == 1) {
            scsi_sense_data[2] = 4;
            ((short *)scsi_sense_data)[6] = 0x4400;
            log_add_event(132, err_last_code.s.v);
            log_add_record(132, err_last_code.s.v, 0, 0);
            return 0;
        } else {
            for (entry = err_hardware_sense_table; ; entry++) {
                if (entry->error_code == bits || entry->error_code == 0)
                    break;
            }
            if (entry->error_code == 0)
                return 1;
            scsi_sense_data[2] = entry->sense_key;
            if (scsi_sense_data[2] == 11 && (win_params.side[scsi_read_side.v].compression.f.type & 0x80))
                ((unsigned short *)scsi_sense_data)[6] = 0x8003;
            else
                ((short *)scsi_sense_data)[6] = entry->asc_ascq;
        }
    } else if (((union view_mdc_motor_test_state *)&bits)->by.lo.b6) {
        bits = page->medium_error_reported.v;
        for (entry = err_medium_sense_table; ; entry++) {
            if (entry->error_code == bits || entry->error_code == 0)
                break;
        }
        if (entry->error_code == 0)
            return 1;
        scsi_sense_data[2] = entry->sense_key;
        if (page->error_kind == 6 || page->error_kind == 7) {
            ((unsigned short *)scsi_sense_data)[6] = SCSI_ASC_MEDIUM_MULTIFEED;
            err_multifeed_reported = 1;
            err_multifeed_code = err_last_code.s.v;
        } else {
            ((short *)scsi_sense_data)[6] = entry->asc_ascq;
        }
        if (bits == 1 || bits == 2) {
            if (power_mdc_restart_request == 0)
                mdc_request_stop();
        }
    } else {
        scsi_sense_data[2] = 5;
        ((short *)scsi_sense_data)[6] = 0;
        switch (page->illegal_request_reported.v) {
        case 8:
        case 32:
        case 4:
            ((short *)scsi_sense_data)[6] = 0x2600;
            break;
        }
    }
    _builtin_set_imask(15);
    page_scanning.v = page_current.v;
    scsi_clear_read_state();
    _builtin_set_imask(saved_imask);
    log_add_event(132, err_last_code.s.v);
    log_add_record(132, err_last_code.s.v, 0, 0);
    return 0;
}

void err_set_illegal_request(short bits)
{
    err_set_illegal_request_at(bits, 0, 0);
}

void err_set_illegal_request_detail(short bits, unsigned char detail_high, unsigned char detail_low)
{
    err_set_illegal_request_at(bits, (detail_high << 8) | detail_low, 0x8004);
}

void err_set_illegal_request_at(bits, detail, reply_code)
short bits, detail, reply_code;
{
    struct page_record *page;
    int saved_imask;
    page = &page_records[page_scanning.v];
    if (main_page_loop_phase == 0 && page_side_state[0].last_buf != 0)
        page = &page_side_records[page_side_state[0].capture_buf];
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    page->illegal_request_bits |= bits;
    _builtin_set_imask(saved_imask);
    err_abort_page(32, detail, reply_code);
}


struct uart_reg_mcr { char pad_00[9]; struct Bits mcr; };

void err_set_medium_error(unsigned short bits, unsigned char display_main, unsigned char display_sub)
{
    struct page_record *page;
    int saved_imask;
    page = &page_records[page_scanning.v];
    if (main_page_loop_phase == 0 && page_side_state[0].last_buf != 0)
        page = &page_side_records[page_side_state[0].capture_buf];
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    page->medium_error_bits.v |= bits;
    _builtin_set_imask(saved_imask);
    if (bits & 1) {
        if (page->flags.f.scan_end != 0 && (page->medium_error_reported.v & 1)) {
            _builtin_set_imask(15);
            page->medium_error_bits.v &= ~bits;
            _builtin_set_imask(saved_imask);
            return;
        }
        _builtin_set_imask(15);
        err_display_flags.b.b6 = 1;
        err_display_flags.b.b2 = 1;
        tick_error_ticks.v = 0;
        if (page->error_kind == 6 || page->error_kind == 7)
            job_separation_sheet_flags = 0;
        _builtin_set_imask(saved_imask);
        if (maint_mode_word.v & 0x4000) {
            if (maint_mode_word.c.mode == MAINT_MODE_WHITE_LEVEL) {
                err_display_flags.b.b2 = 0;
                switch (page->error_kind) {
                case 1:
                    err_maint_error_count_1 += 1;
                    break;
                case 2:
                    err_maint_error_count_2 += 1;
                    break;
                case 3:
                    err_maint_error_count_3 += 1;
                    break;
                case 4:
                    err_maint_error_count_4 += 1;
                    break;
                default:
                    break;
                }
            }
        } else {
            if (hw_hostif_flags.f.own_hostif && page->error_kind == 1) {
                display_main = PANEL_MODE_ERROR;
                display_sub = PANEL_ERROR_J1;
            }
        }
        if (page->error_kind == 6 || page->error_kind == 7 || page->error_kind == 1) {
        } else {
        }
    } else {
        _builtin_set_imask(15);
        err_display_flags.b.b6 = 0;
        _builtin_set_imask(saved_imask);
    }
    if (bits & 512)
        scsi_imprinter_flags.f.b6 = 1;
    err_abort_page(64, (display_main << 8) | display_sub, 0x8005);
}

void err_set_hardware_error(unsigned short bits, unsigned char display_main, unsigned char display_sub)
{
    struct page_record *page;
    int saved_imask;
    unsigned short index;
    page = &page_records[page_scanning.v];
    saved_imask = _builtin_get_imask();
    if (main_page_loop_phase == 0 && page_side_state[0].last_buf != 0)
        page = &page_side_records[page_side_state[0].capture_buf];
    _builtin_set_imask(15);
    err_display_flags.b.b5 = 1;
    page->hardware_error_bits |= bits;
    _builtin_set_imask(saved_imask);
    index = err_count_offsets[err_last_code.s.v];
    if (settings[index] != 255 && index != 0xffff)
        settings[index] += 1;
    index = settings_error_history_index[0] + 256;
    index += 1;
    if (index > 0x183 || index == 0x101)
        index = 0x170;
    _builtin_set_imask(15);
    settings_error_history_index[0] = index;
    settings[index] = err_last_code.s.v;
    _builtin_set_imask(saved_imask);
    iic_eeprom_save_settings();
    if (display_sub == 0)
        err_abort_page(128, (display_main << 8) | display_sub, 0x8005);
    else
        err_abort_page(128, (display_main << 8) | display_sub, 0x8006);
}

void err_abort_page(error_flag, display_word, reply_code)
unsigned short error_flag;
short display_word, reply_code;
{
    struct page_record *page;
    int saved_imask;
    int notify_host;
    int dummy;
    page = &page_records[page_scanning.v];
    saved_imask = _builtin_get_imask();
    notify_host = 0;
    if (main_page_loop_phase == 0 && page_side_state[0].last_buf != 0)
        page = &page_side_records[page_side_state[0].capture_buf];
    if ((unsigned char)display_word != 5 && (unsigned char)display_word != 50 && error_flag != 32)
        panel_set_display_mode((unsigned char)display_word, PANEL_MODE_ERROR);
    _builtin_set_imask(15);
    page->error_flags.v |= error_flag;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    if (mdc_motor_test_state.w) {
        mdc_motor_test_state.w |= 0x2000;
        if (mdc_motor_test_state.w & 32) {
            mdc_motor_test_state.w &= 0xffdf;
            notify_host = 1;
        }
        mdc_motor_test_state.w = 0;
    }
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.own_hostif)
        page_force_end();
    line_side_state[0].state_flags.v = 0;
    if (!hw_hostif_flags.f.own_hostif)
        line_side_state[1].state_flags.v = 0;
    job_adf_sheet_count = 0;
    jpeg_scan_flags.v &= JPEG_SCAN_LONG_PAGE;
    job_long_page_flags &= 128;
    mdc_mailbox.long_page = 0;
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.own_hostif) {
        if (scsi_read_sides[0].flags.b.lo.f.b5) {
            notify_host = 1;
            scsi_read_sides[0].flags.b.lo.f.b5 = 0;
        }
    } else {
        job_sheet_count = 0;
        if (job_link_abort_flags & 0x8000)
            job_link_abort_flags = 256;
    }
    page->flags.v &= PAGE_FLAGS_REPLY_PENDING;
    page->flags.f.scan_end = 1;
    util_mem_clear((char *)&page->sides[0], 48);
    util_mem_clear((char *)&page->sides[1].flags, 48);
    if (page->flags.v & PAGE_FLAGS_REPLY_PENDING) {
        if (page->flags.v & PAGE_FLAGS_SCAN)
            page->flags.v &= ~PAGE_FLAGS_SCAN;
        page->flags.v &= ~PAGE_FLAGS_REPLY_PENDING;
        notify_host = 1;
    }
    _builtin_set_imask(saved_imask);
    if (maint_mode_word.v & 0x800)
        return;
    if (notify_host == 0 || reply_code == 0)
        return;
    {
        _builtin_set_imask(15);
        if (hw_hostif_flags.f.own_hostif) {
            scsi_resume_flags.f.page_ended = 1;
            _builtin_set_imask(saved_imask);
            scsi_disable_irq();
        } else if (!((struct uart_reg_mcr *)p_uart[0])->mcr.b0) {
            _builtin_set_imask(saved_imask);
            return;
        }
        _builtin_set_imask(15);
        link_reply.w = reply_code;
        _builtin_set_imask(saved_imask);
        isr_request_command_layer(0);
        _builtin_set_imask(saved_imask);
    }
}

int err_get_class(unsigned char code) { return err_class_table[code]; }
