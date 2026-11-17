/* S, the SCSI file: one source file with code in two sections. The lo part (section P)
   and the hi part (`#pragma section H`, section PH) alternate in source order. */
#define util_get_be16 util_get_be16_hdr
#include "fw.h"
#undef util_get_be16

extern int util_get_be16();
struct cell_s8_scsi { char v; };
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
extern void _builtin_set_gbr(void *);
/* Initialised data (section D). */
unsigned char (*scsi_status_table[10])(void) = {
    scsi_status_step_0, scsi_status_step_0, scsi_status_step_2, scsi_status_step_3,
    scsi_status_step_4, scsi_status_step_5, scsi_status_step_6, scsi_status_step_7,
    scsi_status_step_8, scsi_dispatch
};
unsigned char (*scsi_end_state_table[8])(void) = {
    scsi_end_state_0, scsi_end_state_0, scsi_end_state_2, scsi_end_state_3,
    scsi_end_state_4, scsi_end_state_5, scsi_end_state_6, scsi_end_state_7
};
unsigned char (*scsi_cmd_state_table[7])(void) = {
    scsi_cmd_state_0, scsi_cmd_state_0, scsi_cmd_state_2, scsi_cmd_state_3,
    scsi_cmd_state_4, scsi_cmd_state_5, scsi_cmd_state_6
};
unsigned char (*xfer_state_table[13])(void) = {
    xfer_init, xfer_init, xfer_compute_length, xfer_wait_data,
    xfer_state_4, xfer_state_5, xfer_state_6, xfer_state_7,
    xfer_state_8, xfer_start_dma, xfer_state_10, xfer_state_11,
    xfer_state_12
};
int (*scsi_message_state_table[22])() = {
    scsi_message_check_atn, scsi_message_check_atn, scsi_message_receive, scsi_message_read_fifo,
    scsi_message_classify, scsi_message_identify, scsi_message_dispatch, scsi_message_extended,
    scsi_message_device_reset, scsi_message_abort, scsi_message_initiator_error, scsi_message_reject_received,
    scsi_message_nop, scsi_message_parity_error, scsi_message_send_reject, scsi_message_retry,
    scsi_message_error, scsi_message_abort_command, scsi_message_next, scsi_message_check_atn_end,
    scsi_message_send_sdtr, scsi_message_sdtr_answer
};
/* Constants (section C). */
const struct scsi_cmd scsi_cmd_table[21] = {
    {SCSI_OP_READ, (void (*)(void))scsi_cmd_read}, {SCSI_OP_SET_WINDOW, scsi_cmd_set_window}, {SCSI_OP_SET_SUBWINDOW, scsi_cmd_set_subwindow}, {SCSI_OP_TEST_UNIT_READY, (void (*)(void))scsi_cmd_test_unit_ready},
    {SCSI_OP_REQUEST_SENSE, scsi_cmd_request_sense}, {SCSI_OP_INQUIRY, (void (*)(void))scsi_cmd_inquiry}, {SCSI_OP_SCAN, scsi_cmd_scan}, {SCSI_OP_OBJECT_POSITION, scsi_cmd_object_position},
    {SCSI_OP_SEND, scsi_cmd_send}, {SCSI_OP_MODE_SELECT, scsi_cmd_mode_select}, {SCSI_OP_MODE_SENSE, scsi_cmd_mode_sense}, {SCSI_OP_SCANNER_CONTROL, scsi_cmd_scanner_control},
    {SCSI_OP_RESERVE_UNIT, (void (*)(void))scsi_cmd_reserve_unit}, {SCSI_OP_RELEASE_UNIT, (void (*)(void))scsi_cmd_release_unit}, {SCSI_OP_SEND_DIAGNOSTIC, scsi_cmd_send_diagnostic}, {SCSI_OP_READ_DIAGNOSTIC, scsi_cmd_read_diagnostic},
    {SCSI_OP_GET_WINDOW, scsi_cmd_get_window}, {SCSI_OP_GET_DATA_BUFFER_STATUS, scsi_cmd_get_data_buffer_status}, {SCSI_OP_ENDORSER, (void (*)(void))scsi_cmd_endorser}, {SCSI_OP_GET_HW_STATUS, scsi_cmd_get_hw_status},
    {0, 0}
};
/* {paper code of descriptor byte 53, width, length in 1/1200 inch}: 3 A3, 4 A4, 5 A5, 6 11 x 17 in, 7 letter, 0xC B4, 0xD B5, 0xF legal */
const struct scsi_win_paper_size_entry scsi_win_paper_sizes[13] = {
    {3, 0x36d2, 0x4d83}, {4, 0x26c2, 0x36d0}, {5, 0x1b7f, 0x26c2}, {6, 0x3390, 0x4fb0},
    {7, 0x27d8, 0x3390}, {0xc, 0x2f6e, 0x432d}, {0xd, 0x2197, 0x2f6e}, {0xf, 0x27d8, 0x41a0},
    {0x14, 0x36d0, 0x26c2}, {0x15, 0x36d0, 0x26c2}, {0x17, 0x36d0, 0x26c2}, {0x1d, 0x36d0, 0x26c2},
    {0, 0, 0}
};
const unsigned char scsi_sop_item_masks[86] = {
    0, 0xd0, 0x1b, 0, 0xd9, 0xff, 0, 0xd8, 0xff, 0, 0xd7, 0xff, 0, 0xd6, 0xff, 0,
    0xd3, 0x77, 0, 0xd2, 0x72, 0, 0xc0, 0x20, 0, 0xbb, 0xff, 0, 0xbe, 0xff, 0, 0xcb,
    0xd6, 0, 0x31, 0x80, 0, 0xb7, 0xff, 0, 0xc6, 0xff, 0, 0xc7, 0xff, 0, 0xb6, 0x88,
    0, 0xcf, 0xff, 0, 0x80, 0xe0, 0, 0xb1, 1, 0, 0x81, 0xf0, 0, 0xb2, 0x87, 0,
    0x29, 3, 0, 0xb4, 0x80, 0, 0x76, 0x8f, 1, 0x48, 0xff, 0, 0xc5, 0x0f, 0, 0x56,
    0xff, 0, 0x57, 0xff, 0, 0
};
#pragma section H
const unsigned char scsi_counter_format_5[8] = "05ud";
const unsigned char scsi_counter_format_8[8] = "08ud";
const unsigned char scsi_keyword_debug_e2p_wrt[20] = "DEBUG,E2P,WRT   ";
const unsigned char scsi_keyword_debug_e2t_wrt_w[20] = "DEBUG,E2T,WRT  W";
const unsigned char scsi_keyword_debug_e2t_red_w[20] = "DEBUG,E2T,RED  W";
const unsigned char scsi_keyword_setup_start_t01[20] = "SETUP START  T01";
const unsigned char scsi_keyword_get_imp_life[16] = "GET IMP-LIFE ";
const unsigned char scsi_keyword_clear_ink_life[16] = "CLEAR INK-LIFE ";
const unsigned char scsi_keyword_debug_e2t_wrt[20] = "DEBUG,E2T,WRT   ";
const unsigned char scsi_keyword_debug_e2t_red[20] = "DEBUG,E2T,RED   ";
const unsigned char scsi_keyword_debug_e2t_wrt_2[20] = "DEBUG,E2T,WRT  2";
const unsigned char scsi_keyword_debug_e2t_red_2[20] = "DEBUG,E2T,RED  2";
const unsigned char scsi_keyword_debug_ram_wrt[20] = "DEBUG,RAM,WRT   ";
const unsigned char scsi_keyword_debug_ram_red[20] = "DEBUG,RAM,RED   ";
const unsigned char scsi_keyword_adf_sensor[20] = "ADF SENSOR      ";
const unsigned char scsi_keyword_read_count[20] = "READ COUNT      ";
const unsigned char scsi_keyword_read_count2[20] = "READ COUNT2     ";
const unsigned char scsi_keyword_read_count_clr[16] = "READ COUNT CLR ";
const unsigned char scsi_keyword_first_read_date[20] = "FIRST READ DATE ";
const unsigned char scsi_keyword_get_first_date[20] = "GET FIRST DATE  ";
const unsigned char scsi_keyword_get_device_id[20] = "GET DEVICE ID   ";
const unsigned char scsi_keyword_rom_information[20] = "ROM INFORMATION ";
const unsigned char scsi_keyword_agc_clear[20] = "AGC CLEAR       ";
const unsigned char scsi_keyword_get_eeprom_sts[20] = "GET EEPROM STS  ";
const unsigned char scsi_keyword_check_prnu_pos[20] = "CHECK PRNU POS  ";
const unsigned char scsi_keyword_erase_flash_work[20] = "ERASE FLASF WORK";
const unsigned char scsi_keyword_test_length[20] = "TEST LENGTH     ";
const unsigned char scsi_keyword_testmode[20] = "TESTMODE        ";
const unsigned char scsi_keyword_scsi_id_get[20] = "SCSI ID GET     ";
const unsigned char scsi_keyword_load_cancel[20] = "LOAD CANCEL     ";
const unsigned char scsi_keyword_omr_sensor_test[20] = "OMR SENSOR TEST ";
const unsigned char scsi_keyword_setup_start[20] = "SETUP START     ";
const unsigned char scsi_keyword_setup_end[20] = "SETUP END       ";
const unsigned char scsi_keyword_measure_readtime[20] = "MEASURE READTIME";
const unsigned char scsi_keyword_get_readtime[20] = "GET READTIME    ";
const unsigned char scsi_keyword_get_firmversion[20] = "GET FIRMVERSION ";
const unsigned char scsi_keyword_setup_imprinter[20] = "SETUP IMPRINTER ";
const unsigned char scsi_keyword_setup_imprinterr[20] = "SETUP IMPRINTERR";
const unsigned char scsi_keyword_diag_start[20] = "DIAG START      ";
const unsigned char scsi_keyword_diag_end[20] = "DIAG END        ";
const unsigned char scsi_keyword_long_page[20] = "LONG PAGE       ";
const unsigned char scsi_keyword_get_sop_default[20] = "GET SOP DEFAULT ";
const unsigned char scsi_keyword_get_powerswmode[20] = "GET POWERSWMODE ";
const unsigned char scsi_keyword_get_sop_set_len[20] = "GET SOP SET LEN ";
const unsigned char scsi_keyword_get_sop_set_dat[20] = "GET SOP SET DAT ";
const unsigned char scsi_keyword_put_sop_set_dat[20] = "PUT SOP SET DAT ";
const unsigned char scsi_keyword_model_fi_6670dj[20] = "fi-6670dj       ";
const unsigned char scsi_keyword_model_fi_6770dj[20] = "fi-6770dj       ";
const unsigned char scsi_keyword_model_fi_6750sj[20] = "fi-6750Sj       ";
const unsigned char scsi_keyword_get_dev_log_len[20] = "GET DEV LOG LEN ";
const unsigned char scsi_keyword_get_dev_log_dat[20] = "GET DEV LOG DAT ";
const unsigned char scsi_keyword_debug_e2p_wrt_x[20] = "DEBUG,E2P,WRT  X";
const unsigned char scsi_keyword_debug_e2t_wrt_x[20] = "DEBUG,E2T,WRT  X";
const unsigned char scsi_keyword_debug_e2t_red_x[20] = "DEBUG,E2T,RED  X";
const unsigned char scsi_keyword_set_powoff_time[20] = "SET POWOFF TIME ";
const unsigned char scsi_keyword_get_powoff_time[20] = "GET POWOFF TIME ";
/* One table of [49][2] shorts in the view the code reads (scsi_testmode_reply_lens[i][0],
   scsi_testmode_lens_column[i][0]); scsi_testmode_lens_column is a view into it; the last short is the padding
   up to the next module. */
const unsigned short scsi_testmode_reply_lens[49][2] = {
    {0x0000, 0x0000}, {0x0001, 0x0002}, {0x0002, 0x0001}, {0x0003, 0x0002}, {0x0004, 0x0001}, {0x0005, 0x0001},
    {0x0006, 0x0001}, {0x0007, 0x0006}, {0x0008, 0x0002}, {0x0009, 0x0008}, {0x000a, 0x0002}, {0x000b, 0x000c},
    {0x000f, 0x0001}, {0x0010, 0x0001}, {0x0020, 0x0001}, {0x0021, 0x0001}, {0x0022, 0x0001}, {0x0024, 0x0001},
    {0x0025, 0x0001}, {0x0026, 0x0000}, {0x0027, 0x0002}, {0x0028, 0x0002}, {0x0029, 0x0000}, {0x002a, 0x0001},
    {0x002b, 0x0000}, {0x002e, 0x0003}, {0x0014, 0x0000}, {0x0015, 0x0200}, {0x0017, 0x0001}, {0x0000, 0x0000},
    {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000},
    {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000},
    {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000}, {0x0000, 0x0000},
    {0x0000, 0x0000}
};
#pragma section


void scsi_run_layer(void)
{
    scsi_saved_imask = _builtin_get_imask();
    scsi_run_layer_step_1();
    _builtin_set_imask(scsi_saved_imask);
}

int scsi_run_layer_step_1(void)
{
    scsi_status = 0;
    if (scsi_mainint_status.b.b4) {
        if (scsi_scsiint1_status.b.b6) {
            scsi_status = 1;
        } else if (scsi_scsiint1_status.b.b3) {
            scsi_status = 1;
        } else if (scsi_scsiint1_status.b.b2) {
            scsi_status = 1;
        } else if (scsi_scsiint1_status.b.b1) {
            scsi_status = 1;
        }
    }
    if (scsi_status)
        scsi_run_status_steps();
    if (scsi_resume_flags.f.page_ended || scsi_resume_flags.f.resume_pending || scsi_deferred != 0)
        scsi_finish_resume();
    else
        scsi_handle_command();
}

void scsi_resume(void)
{
    scsi_saved_imask = _builtin_get_imask();
    scsi_status = 0;
    if (scsi_resume_flags.f.page_ended || scsi_resume_flags.f.resume_pending || scsi_deferred != 0)
        scsi_finish_resume();
    _builtin_set_imask(scsi_saved_imask);
}

int scsi_handle_command(void)
{
    scsi_last_ctrl_command = 0;
    scsi_read_initiator();
    if (scsi_scsiint1_status.b.b0 == 0)
        scsi_command_received = 1;
    scsi_status = scsi_handle_command_step_2();
    if ((unsigned char)scsi_status == 0)
        scsi_execute_command(0);
    scsi_run_status_steps();
}

void scsi_read_initiator(void)
{
    char initiator_id;
    _builtin_set_gbr(0);
    initiator_id = scsi_ctrl->sdid.f.initiator_id;
    _builtin_set_gbr(0);
    scsi_ctrl->sdid.f.reselect_id = initiator_id;
    scsi_initiator_bit.c = scsi_id_to_bit(initiator_id);
    scsi_identify_message = 0;
    scsi_sense_information.v = 0;
}

int scsi_id_to_bit(char id)
{
    unsigned char shift;
    shift = id;
    id = 1;
    while (shift-- != 0)
        id <<= 1;
    return id;
}

char scsi_handle_command_step_2(void)
{
    int j;
    int i;
    int remaining;
    int limit;
    _builtin_set_gbr(0);
    scsi_ctrl->scsimode1.b.b5 = 0;
    if (scsi_scsiint1_status.b.b0 != 0)
        goto L1;
    if (!scsi_check_parity_error())
        return scsi_status;
    if (!scsi_run_messages(1)) {
        scsi_command_received = 0;
        scsi_set_sense(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_MESSAGE_ERROR);
        return 1;
    } else {
        scsi_command_received = 0;
    }
L1:
    if (scsi_initiator_allowed() == 0)
        return 5;
    scsi_message_retries.v = 0;
    _builtin_set_gbr(0);
    if (scsi_ctrl->fifoctl.b0) {
        _builtin_set_gbr(0);
        scsi_ctrl->fifoctl.b2 = 1;
        scsi_receive_cdb(1, scsi_cdb);
    } else {
        _builtin_set_gbr(0);
        scsi_cdb[0] = scsi_ctrl->fifodata;
    }
    switch (scsi_cdb[0] & 0xe0) {
    case 0:
        remaining = 5;
        break;
    case 0x20:
        remaining = 9;
        break;
    case 0xc0:
        remaining = 9;
        break;
    case 0xe0:
        remaining = 9;
        break;
    default:
        _builtin_set_gbr(0);
        scsi_ctrl->fifoctl.b2 = 1;
        scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_OPCODE);
        return 3;
    }
    limit = remaining;
    _builtin_set_gbr(0);
    if (scsi_ctrl->fifoctl.b0) {
        i = 0;
    } else {
        i = 0;
        while (i < limit) {
            _builtin_set_gbr(0);
            if (scsi_ctrl->fifoctl.b0)
                break;
            _builtin_set_gbr(0);
            *((unsigned char *)scsi_cdb + i + 1) = scsi_ctrl->fifodata;
            remaining--;
            i++;
        }
    }
    _builtin_set_gbr(0);
    scsi_ctrl->fifoctl.b2 = 1;
    if (remaining != 0) {
        scsi_receive_cdb(remaining, (unsigned char *)scsi_cdb + i + 1);
        while (1) {
            if (!scsi_check_parity_error())
                return scsi_status;
            if (scsi_run_messages(1) != 0)
                break;
            else if (scsi_status != 0)
                return scsi_status;
            if (scsi_finish_command() == 0) {
                if (scsi_status != 0)
                    return scsi_status;
                else
                    return 3;
            } else {
                switch (scsi_cdb[0] & 0xe0) {
                case 0:
                    scsi_receive_cdb(6, scsi_cdb);
                    break;
                case 0x20:
                case 0xc0:
                    scsi_receive_cdb(10, scsi_cdb);
                    break;
                default:
                    _builtin_set_gbr(0);
                    scsi_ctrl->fifoctl.b2 = 1;
                    scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_OPCODE);
                    return 3;
                }
            }
        }
    }
    if ((scsi_identify_message & 0x80) == 0) {
        i = scsi_cdb[1];
        i = i << 3;
        scsi_identify_message = (unsigned short)i >> 8;
    }
    if ((scsi_identify_message & 7) != 0) {
        if (scsi_cdb[0] != SCSI_OP_INQUIRY && scsi_cdb[0] != SCSI_OP_REQUEST_SENSE) {
            return 3;
        }
    } else if (scsi_disconnected_initiator.v != 0) {
        scsi_set_sense(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_OVERLAPPED_COMMANDS);
        return 3;
    }
    return 0;
}

int scsi_disable_irq(void)
{
    if (hw_hostif_flags.f.usb)
        return;
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    scsi_ctrl->scsimode0.b.b7 = 0;
    p_intc->IPRA.WORD &= 0xfffc;
    _builtin_set_imask(scsi_saved_imask);
}

int scsi_enable_irq(void)
{
    if (hw_hostif_flags.f.usb)
        return;
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    scsi_ctrl->scsimode0.b.b7 = 1;
    p_intc->IPRA.WORD |= 3;
    _builtin_set_imask(scsi_saved_imask);
}

void scsi_wait_ctrl_command(void)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    tick_hostif_wait_ticks.v = 0;
    for (;;) {
        if (tick_hostif_wait_ticks.v > 0x9c4) {
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            scsi_ctrl->fifoctl.b2 = 1;
            _builtin_set_gbr(0);
            scsi_ctrl->command = 1;
            scsi_last_ctrl_command = 1;
            _builtin_set_imask(saved_imask);
        }
        _builtin_set_gbr(0);
        if (!scsi_ctrl->mainint.b.b5) {
            _builtin_set_gbr(0);
            if ((scsi_ctrl->mainint.v & 0xdb) != 0)
                break;
        }
    }
    scsi_read_interrupts();
    _builtin_set_imask(15);
    if (p_intc->ISR.BIT.IRQ3F)
        p_intc->ISR.WORD = 0xef;
    _builtin_set_imask(saved_imask);
}

void scsi_ctrl_command(char cmd)
{
    tick_hostif_wait_ticks.v = 0;
    if (hw_hostif_flags.f.usb)
        return;
    if (scsi_last_ctrl_command == 12) {
        _builtin_set_gbr(0);
        scsi_ctrl->fifoctl.b2 = 1;
        _builtin_set_gbr(0);
        scsi_ctrl->command = 1;
        scsi_last_ctrl_command = 1;
    } else {
        _builtin_set_gbr(0);
        while (scsi_ctrl->mainint.b.b5) {
            if (tick_hostif_wait_ticks.v > 0x9c4) {
                _builtin_set_gbr(0);
                scsi_ctrl->fifoctl.b2 = 1;
                _builtin_set_gbr(0);
                scsi_ctrl->command = 1;
                scsi_last_ctrl_command = 1;
            }
            _builtin_set_gbr(0);
        }
    }
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    scsi_ctrl->command = cmd;
    scsi_last_ctrl_command = cmd;
    _builtin_set_imask(scsi_saved_imask);
    for (;;) {
        _builtin_set_gbr(0);
        if (!scsi_ctrl->scsiint2.b.b3)
            break;
    }
}

void scsi_read_interrupts(void)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    scsi_mainint_status.v = scsi_ctrl->mainint.v;
    _builtin_set_gbr(0);
    scsi_scsiint1_status.v = scsi_ctrl->scsiint1.v;
    _builtin_set_gbr(0);
    scsi_scsiint2_status.v = scsi_ctrl->scsiint2.v;
    _builtin_set_gbr(0);
    scsi_ctrl->mainint.v = scsi_mainint_status.v;
    _builtin_set_gbr(0);
    scsi_ctrl->scsiint1.v = scsi_scsiint1_status.v;
    _builtin_set_gbr(0);
    scsi_ctrl->scsiint2.v = scsi_scsiint2_status.v;
    _builtin_set_imask(saved_imask);
    _builtin_set_gbr(0);
    scsi_scsictl_status = scsi_ctrl->scsictl.v;
    if (scsi_scsiint2_status.b.b6) {
        _builtin_set_imask(15);
        scsi_handle_bus_reset();
    }
}

void scsi_handle_bus_reset(void)
{
    int saved_imask;
    unsigned char status;
    saved_imask = _builtin_get_imask();
    _builtin_set_gbr(0);
    while (scsi_ctrl->scsiint2.b.b6) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        status = scsi_ctrl->mainint.v;
        _builtin_set_gbr(0);
        scsi_ctrl->mainint.v = status;
        status = scsi_ctrl->scsiint1.v;
        _builtin_set_gbr(0);
        scsi_ctrl->scsiint1.v = status;
        _builtin_set_gbr(0);
        status = scsi_ctrl->scsiint2.v;
        _builtin_set_gbr(0);
        scsi_ctrl->scsiint2.v = status;
        _builtin_set_gbr(0);
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    if (p_intc->ISR.BIT.IRQ3F)
        p_intc->ISR.WORD = 0xef;
    _builtin_set_imask(saved_imask);
    mdc_wake_board();
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    scsi_ctrl->reset = 7;
    _builtin_set_gbr(0);
    scsi_ctrl->scsimode0.b.b7 = 0;
    _builtin_set_gbr(0);
    scsi_ctrl->scsimode0.b.b6 = 1;
    hw_restart_flags.f.host_reset = 1;
    hw_restart_flags.f.leaving_power_save = 0;
    scsi_ua_reported = 0;
    isr_stop_cmt1();
    rt_restart();
}

void scsi_receive_cdb(int count, unsigned char *p)
{
    _builtin_set_gbr(0);
    scsi_ctrl->fifoctl.b2 = 1;
    _builtin_set_gbr(0);
    scsi_ctrl->ndmasize = count;
    scsi_disable_irq();
    scsi_ctrl_command(18);
    scsi_wait_ctrl_command();
    scsi_enable_irq();
    for (; count != 0; count--) {
        _builtin_set_gbr(0);
        *p = scsi_ctrl->fifodata;
        p++;
    }
}

void scsi_send_status_byte(void)
{
    _builtin_set_gbr(0);
    scsi_ctrl->fifoctl.b2 = 1;
    _builtin_set_gbr(0);
    scsi_ctrl->ndmasize = 1;
    _builtin_set_gbr(0);
    scsi_ctrl->fifodata = scsi_status_byte;
    scsi_disable_irq();
    scsi_ctrl_command(23);
    scsi_wait_ctrl_command();
    scsi_enable_irq();
}

#pragma section H
extern void _builtin_set_gbr(void *);

int scsi_finish_command(void)
{
    scsi_message_retries.v = 0;
    while (1) {
        scsi_reply_message.v = 3;
        scsi_send_message_byte();
        if (scsi_run_messages(0) == 0) {
            if (scsi_status) {
                return 0;
            }
        } else {
            if (((volatile unsigned char *)&scsi_disconnect_allowed)[0]) {
                return 1;
            } else {
                scsi_set_sense(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_MESSAGE_ERROR);
                return 0;
            }
        }
    }
}

/* The message phases while the initiator asserts ATN: returns 1 when the states end (0), 0 on state 22 (error status). */
int scsi_run_messages(int no_disconnect)
{
    unsigned char state;
    if (no_disconnect) {
        scsi_disconnect_allowed = 0;
    } else {
        scsi_disconnect_allowed = 128;
    }
    scsi_message_retries.v = 0;
    if (hw_hostif_flags.f.usb) {
        return 1;
    }
    _builtin_set_gbr(0);
    if (scsi_command_received == 0 && !scsi_ctrl->scsictl.b.b6) {
        return 1;
    }
    scsi_message_state = 1;
    while (1) {
        state = (unsigned char)scsi_message_state;
        scsi_message_state = 0;
        if ((unsigned char)(scsi_message_state = scsi_message_state_table[state]()) == 0) {
            return 1;
        } else if ((unsigned char)scsi_message_state == 22) {
            return 0;
        }
    }
}

int scsi_message_check_atn(void)
{
    int x;
    if (scsi_command_received) {
        return 3;
    }
    _builtin_set_gbr(0);
    if (scsi_ctrl->scsictl.b.b6) {
        return 2;
    }
    return 0;
}

int scsi_message_receive(void)
{
    if (scsi_receive_message_byte(0) == 0) {
        return (char)scsi_message_state;
    } else {
        return 4;
    }
}

int scsi_message_read_fifo(void)
{
    _builtin_set_gbr(0);
    scsi_message_byte = scsi_ctrl->fifodata;
    return 4;
}

int scsi_message_classify(void)
{
    if (scsi_message_byte & 128) {
        return 5;
    }
    return 6;
}

extern void _builtin_set_gbr(void *);


int scsi_message_identify(void)
{
    unsigned char previous_identify;
    unsigned char identify;
    unsigned char *p;
    previous_identify = scsi_identify_message;
    identify = scsi_message_byte;
    p = &((short *)scsi_sense_data)[6];
    if (previous_identify & 0x80) {
        previous_identify &= 0x27;
        identify &= 0x27;
        if (previous_identify != identify) {
            return 16;
        }
    }
    identify = scsi_message_byte;
    if (identify & 0x38) {
        return 14;
    } else {
        scsi_identify_message = identify;
        _builtin_set_gbr(0);
        if (scsi_ctrl->scsictl.b.b6) {
            return 2;
        }
        return 0;
    }
}

/* SCSI message codes: 01 extended, 0C bus device reset, 06 abort, 05 initiator detected error, 07 reject, 08 no-op, 09 parity error. */
int scsi_message_dispatch(void)
{
    switch (scsi_message_byte) {
    case 1: return 7;
    case 12: return 8;
    case 6: return 9;
    case 5: return 10;
    case 7: return 11;
    case 8: return 12;
    case 9: return 13;
    default: return 14;
    }
    return 0;
}

int scsi_message_extended(void)
{
    char period;
    scsi_ext_message[0] = scsi_message_byte;
    if (scsi_receive_message_byte(0) == 0)
        return scsi_message_state;
    else
    scsi_extended_message_len[0] = scsi_message_byte;
    if (scsi_receive_message_byte(0) == 0)
        return scsi_message_state;
    else
    scsi_extended_message_code[0] = scsi_message_byte;
    if (*(volatile unsigned char *)scsi_extended_message_code != 1)
        return 14;
    period = ((unsigned char)scsi_own_id_bit);
    if (period == scsi_initiator_bit.c)
        return 14;
    if ((unsigned char)scsi_extended_message_len[0] != 3) {
        scsi_clear_sync_agreement();
        return 14;
    }
    if (scsi_receive_message_byte(0) == 0)
        return scsi_message_state;
    else
    scsi_sdtr_period[0] = scsi_message_byte;
    if (scsi_receive_message_byte(0) == 0)
        return scsi_message_state;
    else
    scsi_sdtr_offset[0] = scsi_message_byte;
    _builtin_set_gbr(0);
    if (scsi_ctrl->scsictl.b.b6) {
        scsi_clear_sync_agreement();
        return 16;
    }
    if (scsi_sdtr_offset[0] == 0)
        return 20;
    else if (scsi_sdtr_offset[0] > 15) {
        scsi_sdtr_offset[0] = 15;
    }
    if (scsi_sdtr_period[0] <= 12) {
        period = 12;
    } else if (scsi_sdtr_period[0] <= 18) {
        period = 18;
    } else if (scsi_sdtr_period[0] <= 25) {
        period = 25;
    } else if (scsi_sdtr_period[0] <= 50) {
        period = 50;
    } else {
        scsi_sdtr_offset[0] = 0;
        return 20;
    }
    scsi_sdtr_period[0] = period;
    return 20;
}

extern void _builtin_set_imask(int);

int scsi_message_device_reset(void)
{
    _builtin_set_imask(15);
    scsi_handle_bus_reset();
    return 0;
}


int scsi_message_abort(void)
{
    struct page_record *current_page;
    current_page = &page_records[page_current.v];
    if (scsi_reserved_by.v == 0 || (scsi_reserved_by.v != 0 && scsi_reserved_by.v == scsi_initiator_bit.c)) {
        scsi_sense_owner.v = 0;
        current_page->error_flags.v = 0;
        current_page->hardware_error_bits = 0;
        current_page->medium_error_bits.s = 0;
        current_page->illegal_request_bits = 0;
    }
    scsi_sense_data[2] = 11;
    ((short *)scsi_sense_data)[6] = 0;
    scsi_status = 1;
    return 22;
}

int scsi_message_initiator_error(void)
{
    scsi_message_retries.v++;
    if (scsi_message_retries.v > 3) {
        ((short *)scsi_sense_data)[6] = 0x4800;
        return 17;
    } else {
        return 15;
    }
}

int scsi_message_reject_received(void)
{
    if (scsi_disconnect_allowed != 0) {
        scsi_disconnect_allowed = 0;
        return 18;
    } else if (!scsi_reply_message.v) {
        return 18;
    } else if (scsi_reply_message.v == 7) {
        scsi_sense_data[2] = 11;
        ((short *)scsi_sense_data)[6] = 0;
        return 22;
    } else {
        return 16;
    }
}

int scsi_message_nop(void)
{
    return 18;
}

int scsi_message_parity_error(void)
{
    scsi_message_retries.v++;
    if (scsi_message_retries.v <= 3) {
        scsi_send_message_byte();
        return 1;
    }
    return 16;
}

extern void _builtin_set_gbr(void *);


int scsi_message_send_reject(void)
{
    unsigned char retries;
    retries = scsi_message_retries.v;
    retries++;
    if (retries > 3) {
        return 16;
    } else {
        scsi_message_retries.v = retries;
        scsi_reply_message.v = 7;
        scsi_send_message_byte();
        return 1;
    }
}

int scsi_message_retry(void)
{
    scsi_status = 0;
    return 22;
}

int scsi_message_error(void)
{
    ((short *)scsi_sense_data)[6] = 0x4300;
    return 17;
}

int scsi_message_abort_command(void)
{
    scsi_sense_data[2] = 11;
    scsi_status = 1;
    return 22;
}

int scsi_message_next(void)
{
    if (scsi_message_byte & 128) {
        return 1;
    }
    return 19;
}

int scsi_message_check_atn_end(void)
{
    _builtin_set_gbr(0);
    if (scsi_ctrl->scsictl.b.b6) {
        return 16;
    }
    return 0;
}

int scsi_message_send_sdtr(void)
{
    int i;
    for (i = 0; i < 5; i++) {
        scsi_reply_message.v = scsi_ext_message[i];
        scsi_send_message_byte();
    }
    _builtin_set_gbr(0);
    if (scsi_ctrl->scsictl.b.b6) {
        return 21;
    } else {
        scsi_store_sync_agreement();
        return 0;
    }
}

int scsi_message_sdtr_answer(void)
{
    if (scsi_receive_message_byte(1) == 0) {
        return scsi_message_state;
    }
    if (scsi_message_byte == 7) {
        scsi_clear_sync_agreement();
        return 18;
    } else {
        scsi_store_sync_agreement();
        return 4;
    }
}
#pragma section

void scsi_send_message_byte(void)
{
    _builtin_set_gbr(0);
    scsi_ctrl->fifoctl.b2 = 1;
    _builtin_set_gbr(0);
    scsi_ctrl->ndmasize = 1;
    _builtin_set_gbr(0);
    scsi_ctrl->fifodata = scsi_reply_message.v;
    scsi_disable_irq();
    scsi_ctrl_command(24);
    scsi_wait_ctrl_command();
    scsi_enable_irq();
}

int scsi_receive_message_byte(int sdtr_answer)
{
    unsigned char retries;
    short *sense_asc;
    for (;;) {
        _builtin_set_gbr(0);
        scsi_ctrl->fifoctl.b2 = 1;
        _builtin_set_gbr(0);
        scsi_ctrl->ndmasize = 1;
        scsi_disable_irq();
        scsi_ctrl_command(25);
        scsi_wait_ctrl_command();
        scsi_enable_irq();
        if (!scsi_scsiint1_status.b.b7) {
            _builtin_set_gbr(0);
            scsi_message_byte = scsi_ctrl->fifodata;
            return 1;
        } else {
            _builtin_set_gbr(0);
            if (!scsi_ctrl->scsictl.b.b6) {
                retries = scsi_message_retries.v;
                retries++;
                if (retries > 3) {
                    sense_asc = &((short *)scsi_sense_data)[6];
                    *sense_asc = 0x4700;
                    scsi_message_state = 16;
                    return 0;
                } else {
                    if (sdtr_answer)
                        scsi_message_state = 21;
                    else
                        scsi_message_state = 2;
                    return 0;
                }
            }
        }
    }
}

int scsi_receive_params(void)
{
    scsi_message_retries.v = 0;
    for (;;) {
        scsi_receive_params_step_1(scsi_param_len.v);
        if (scsi_status)
            return 0;
        if (!(scsi_check_parity_error() != 0))
            return 0;
        if (scsi_run_messages(1) == 0)
            if (!(scsi_status == 0)) {
                return 0;
            } else if (!scsi_finish_command()) {
                if (scsi_status) {
                    return 0;
                } else {
                    scsi_status = SCSI_STEP_CHECK_CONDITION;
                    return 0;
                }
            } else {
            }
        else
            return 1;
    }
}

void scsi_receive_params_step_1(len)
unsigned short len;
{
    unsigned short i;
    if (hw_hostif_flags.f.usb) {
        usb_receive_params();
        return;
    }
    scsi_set_sync_mode();
    _builtin_set_gbr(0);
    scsi_ctrl->fifoctl.b2 = 1;
    _builtin_set_gbr(0);
    scsi_ctrl->dtbc2 = len >> 16;
    _builtin_set_gbr(0);
    scsi_ctrl->dtbc1 = (unsigned)(len & 0xff00) >> 8;
    _builtin_set_gbr(0);
    scsi_ctrl->dtbc0 = len >> 0;
    scsi_disable_irq();
    _builtin_set_gbr(0);
    scsi_ctrl->dmactl.v |= 3;
    scsi_ctrl_command(19);
    for (i = 0; i < len; i++) {
        tick_hostif_wait_ticks.v = 0;
        _builtin_set_gbr(0);
        while (scsi_ctrl->fifoctl.b0) {
            if (tick_hostif_wait_ticks.v > 0x9c4) {
                _builtin_set_gbr(0);
                scsi_ctrl->fifoctl.b2 = 1;
                _builtin_set_gbr(0);
                scsi_ctrl->command = 1;
                scsi_last_ctrl_command = 1;
                scsi_enable_irq();
                scsi_set_sense(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_SCSI_PARITY_ERROR);
                err_display_flags.b.b5 = 1;
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_OPCODE);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            _builtin_set_gbr(0);
        }
        if ((flash_download_session & 0x80) && len > 0x2000) {
            _builtin_set_gbr(0);
            *(char *)(flash_download_buf + i) = scsi_ctrl->fifodata;
        } else if ((scsi_defect_transfer.defect_area) && len > 0x500) {
            _builtin_set_gbr(0);
            *(char *)(scsi_param_buf_large + i) = scsi_ctrl->fifodata;
        } else {
            _builtin_set_gbr(0);
            *(unsigned char *)(scsi_param_buf + i) = scsi_ctrl->fifodata;
        }
    }
    scsi_wait_ctrl_command();
    scsi_enable_irq();
}

int scsi_send_reply(void)
{
    if (hw_hostif_flags.f.usb) {
        usb_send_reply_data();
        return;
    }
    scsi_message_retries.v = 0;
    for (;;) {
        scsi_send_data_in(scsi_param_len.c.lo);
        if (scsi_status)
            return;
        if (!(scsi_check_parity_error() != 0))
            return;
        if (!scsi_run_messages(1)) {
            if (scsi_status)
                return;
            if (!scsi_finish_command()) {
                if (scsi_status) {
                    return;
                } else {
                    scsi_status = SCSI_STEP_CHECK_CONDITION;
                    return;
                }
            } else {
            }
        } else
            break;
    }
    scsi_status = SCSI_STEP_GOOD;
}

void scsi_send_data_in(len)
int len;
{
    int i;
    char *p;
    if (scsi_cdb[0] == SCSI_OP_GET_HW_STATUS) {
        p = scsi_reply_buf;
    } else if (flash_download_targets[flash_download_session & 3].read_pending != 0) {
        flash_download_targets[flash_download_session & 3].read_pending = 0;
        p = flash_download_buf;
    } else if (scsi_defect_transfer.defect_area) {
        p = flash_defect_area;
    } else {
        p = scsi_reply_buf;
    }
    scsi_set_sync_mode();
    scsi_disable_irq();
    _builtin_set_gbr(0);
    if (scsi_ctrl->syncmode.f.l) {
        _builtin_set_gbr(0);
        scsi_ctrl->dtbc2 = len >> 16;
        _builtin_set_gbr(0);
        scsi_ctrl->dtbc1 = (unsigned)(len & 0xff00) >> 8;
        _builtin_set_gbr(0);
        scsi_ctrl->dtbc0 = len;
        _builtin_set_gbr(0);
        scsi_ctrl->fifoctl.b2 = 1;
        scsi_ctrl_command(21);   /* DMA_DATA_IN, fed by the CPU through the FIFO */
        _builtin_set_gbr(0);
        scsi_ctrl->dmactl.v |= 3;
        for (i = 0; i < len; i++) {
            _builtin_set_gbr(0);
            while (scsi_ctrl->fifoctl.b1) {
                _builtin_set_gbr(0);
            }
            _builtin_set_gbr(0);
            scsi_ctrl->fifodata = *p++;
        }
        tick_hostif_wait_ticks.v = 0;
        while (1) {
            _builtin_set_gbr(0);
            if (scsi_ctrl->fifoctl.b0) {
                scsi_wait_ctrl_command();
                break;
            }
            _builtin_set_gbr(0);
            if (scsi_ctrl->mainint.b.b1) {
                scsi_wait_ctrl_command();
                break;
            }
            if (tick_hostif_wait_ticks.v > 0x9c4) {
                _builtin_set_gbr(0);
                scsi_ctrl->fifoctl.b2 = 1;
                _builtin_set_gbr(0);
                scsi_ctrl->command = 1;
                scsi_last_ctrl_command = 1;
                scsi_enable_irq();
                scsi_set_sense(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_SCSI_PARITY_ERROR);
                err_display_flags.b.b5 = 1;
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_OPCODE);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
        }
    } else {
        _builtin_set_gbr(0);
        scsi_ctrl->fifoctl.b2 = 1;
        while (len != 0) {
            _builtin_set_gbr(0);
            scsi_ctrl->fifodata = *p;
            _builtin_set_gbr(0);
            scsi_ctrl->ndmasize = 1;
            scsi_ctrl_command(22);
            tick_hostif_wait_ticks.v = 0;
            _builtin_set_gbr(0);
            while (scsi_ctrl->mainint.b.b5) {
                if (tick_hostif_wait_ticks.v > 10) {
                    _builtin_set_gbr(0);
                    scsi_ctrl->fifoctl.b2 = 1;
                    _builtin_set_gbr(0);
                    scsi_ctrl->command = 1;
                    scsi_last_ctrl_command = 1;
                    while (1) {
                        _builtin_set_gbr(0);
                        if (!scsi_ctrl->mainint.b.b5)
                            break;
                    }
                    _builtin_set_gbr(0);
                    if (!scsi_ctrl->fifoctl.b0)
                        scsi_ctrl_command(22);
                }
            }
            p++;
            len--;
            scsi_wait_ctrl_command();
        }
    }
    scsi_enable_irq();
}

void scsi_reselect(void)
{
    if (hw_hostif_flags.f.usb)
        return;
    _builtin_set_gbr(0);
    scsi_ctrl->sdid.f.reselect_id = scsi_lowest_bit(scsi_disconnected_initiator.v);
    scsi_disable_irq();
    scsi_ctrl_command(13);
    scsi_wait_ctrl_command();
    scsi_enable_irq();
    scsi_reply_message.v = 0x80;
    scsi_send_message_byte();
}

int scsi_disconnect(void)
{
    scsi_disconnect_allowed = 0;
    scsi_disconnected_initiator.v = 0;
    if (hw_hostif_flags.f.usb)
        return 0;
    if (!(scsi_identify_message & 0x40))
        return 0;
    if (scsi_initiator_bit.c == scsi_own_id_bit)
        return 0;
    scsi_message_retries.v = 0;
    while (1) {
        scsi_reply_message.v = 4;
        scsi_send_message_byte();
        if (!scsi_run_messages(0)) {
            if (scsi_status) {
                return 0;
            } else {
            }
        } else
            break;
    }
    if ((scsi_disconnect_allowed & 0xff) == 0)
        return 0;
    scsi_disconnected_initiator.v = scsi_initiator_bit.c;
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    scsi_ctrl->fifoctl.b2 = 1;
    scsi_ctrl_command(5);
    while (1) {
        _builtin_set_gbr(0);
        if (!scsi_ctrl->mainint.b.b5)
            break;
    }
    _builtin_set_gbr(0);
    scsi_ctrl->scsimode1.b.b5 = 1;
    scsi_wait_selection_pending = 1;
    _builtin_set_imask(scsi_saved_imask);
    return 1;
}

void scsi_release_bus(void)
{
    if (scsi_reselect_failed) {
        scsi_reselect_failed = 0;
        return;
    }
    if (hw_hostif_flags.f.usb) {
        _builtin_set_imask(scsi_saved_imask);
        return;
    }
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    scsi_ctrl->fifoctl.b2 = 1;
    scsi_ctrl_command(5);
    while (1) {
        _builtin_set_gbr(0);
        if (!scsi_ctrl->mainint.b.b5)
            break;
    }
    _builtin_set_gbr(0);
    scsi_ctrl->scsimode1.b.b5 = 1;
    if (scsi_update_request & 0x40)
        scsi_update_request = 0x80;
    scsi_wait_selection_pending = 1;
    if (!hw_hostif_flags.f.usb) {
        if (scsi_update_request == 0x40)
            scsi_update_request = 0x80;
    }
    _builtin_set_imask(scsi_saved_imask);
}

void scsi_set_sync_mode(void)
{
    unsigned short period_code;
    unsigned char idx;
    unsigned char period;
    unsigned char offset;
    idx = scsi_lowest_bit(scsi_initiator_bit.c);
    period = scsi_sync_params[idx * 2];
    offset = scsi_sync_offsets[idx * 2];
    if (scsi_identify_message & 7) {
        period = scsi_sdtr_period[0];
        offset = scsi_sdtr_offset[0];
    }
    if (!scsi_initiator_bit.c & scsi_sync_initiators || offset == 0) {
        _builtin_set_gbr(0);
        scsi_ctrl->syncmode.f.h = period;
        _builtin_set_gbr(0);
        scsi_ctrl->syncmode.f.l = 0;
        return;
    } else {
        _builtin_set_gbr(0);
        scsi_ctrl->scsimode0.b.b4 = 0;
        if (period <= 12) {
            period_code = 0x30;
        } else if (period <= 18) {
            period_code = 0x48;
        } else if (period <= 25) {
            period_code = 100;
        } else if (period <= 50) {
            period_code = 200;
        } else {
            _builtin_set_gbr(0);
            scsi_ctrl->syncmode.f.h = scsi_sdtr_period[0];
            _builtin_set_gbr(0);
            scsi_ctrl->syncmode.f.l = 0;
            return;
        }
        if (period_code < 100) {
            period_code += 24;
            period_code = rt_divu16(period_code, 25);
            if (period_code > 3) {
                period_code = 0;
            } else {
                if (period_code < 2)
                    period_code = 2;
                period_code -= 2;
                _builtin_set_gbr(0);
                scsi_ctrl->scsimode0.b.b4 = 1;
            }
        } else {
            period_code += 49;
            period_code = rt_divu16(period_code, 50);
            if (period_code < 2)
                period_code = 2;
            period_code -= 2;
        }
        _builtin_set_gbr(0);
        scsi_ctrl->syncmode.f.h = period_code;
        _builtin_set_gbr(0);
        scsi_ctrl->syncmode.f.l = (offset & 15);
    }
}

void scsi_defer_enable_irq(void)
{
    scsi_status = SCSI_STEP_NO_STATUS;
    scsi_deferred = 1;
    scsi_enable_irq();
}

void scsi_defer_disable_irq(void)
{
    scsi_status = SCSI_STEP_NO_STATUS;
    scsi_deferred = 1;
    scsi_disable_irq();
}

int scsi_check_irq_idle(void)
{
    _builtin_set_gbr(0);
    if (scsi_ctrl->mainint.v & 0xdb) {
        scsi_wait_ctrl_command();
        return 0;
    }
    return 1;
}

void scsi_wait_dma_in_done(void)
{
    if (scsi_last_ctrl_command == 21) {
        scsi_disable_irq();
        if (!scsi_mainint_status.b.b1) {
            while (1) {
                scsi_wait_ctrl_command();
                if (scsi_mainint_status.b.b1)
                    break;
            }
        }
        scsi_enable_irq();
    }
}

void scsi_finish_resume(void)
{
    struct page_record *current_page;
    current_page = &page_records[page_current.v];
    if (scsi_resume_flags.f.page_ended)
        scsi_wait_dma_in_done();
    if (!scsi_check_irq_idle()) {
    }
    _builtin_set_imask(15);
    current_page->flags.v &= ~PAGE_FLAGS_REPLY_PENDING;
    scsi_resume_flags.f.resume_pending = 0;
    scsi_resume_flags.f.page_ended = 0;
    scsi_deferred = 0;
    _builtin_set_imask(scsi_saved_imask);
    if (scsi_end_state) {
        scsi_run_end_state();
    } else if (xfer_state_front) {
        scsi_run_xfer_front();
    } else if (xfer_state_back) {
        scsi_run_xfer_back();
    } else if (scsi_cmd_state) {
        scsi_run_cmd_state();
    } else {
        _builtin_set_imask(15);
        current_page->flags.v &= ~PAGE_FLAGS_REPLY_PENDING;
        _builtin_set_imask(scsi_saved_imask);
        scsi_enable_irq();
        return;
    }
    scsi_run_status_steps();
}

void scsi_run_status_steps(void)
{
    unsigned char step;
    if (!((unsigned char)scsi_status & 6))
        scsi_message_retries.v = 0;
    while (1) {
        step = scsi_status;
        scsi_status = 0;
        if (!(unsigned char)(scsi_status = scsi_status_table[step]()))
            break;
    }
}


int scsi_status_step_0(void)
{
    struct page_record *current_page;
    current_page = &page_records[page_current.v];
    if (scsi_sense_owner.v || (!scsi_sense_owner.v && scsi_reserved_by.v && scsi_reserved_by.v != scsi_initiator_bit.c)) {
        scsi_release_bus();
    } else {
        if (!current_page->sides[0].flags.f.requested)
            scsi_sense_data[2] |= SCSI_SENSE_EOM;
        scsi_sense_data[15] = 0;
        scsi_sense_data[16] = 0;
        scsi_sense_data[17] = 0;
        scsi_release_bus();
    }
    return 0;
}

unsigned char scsi_status_step_2(void)
{
    scsi_status_byte = SCSI_STATUS_GOOD;
    scsi_set_sense(SCSI_SENSE_NO_SENSE, SCSI_ASC_NONE);
    scsi_sense_data[17] = 0;
    scsi_sense_owner.v = 0;
    scsi_init_sense(1);
    return SCSI_STEP_SEND_STATUS;
}

unsigned char scsi_status_step_3(void)
{
    if (!scsi_reserved_by.v || (scsi_reserved_by.v != 0 && scsi_reserved_by.v == scsi_initiator_bit.c)) {
        scsi_sense_data[17] = 0;
        scsi_init_sense(1);
        scsi_sense_owner.v = scsi_initiator_bit.c;
    }
    scsi_status_byte = SCSI_STATUS_CHECK_CONDITION;
    return SCSI_STEP_SEND_STATUS;
}


#pragma section H
int scsi_status_step_4(void)
{
    scsi_status_byte = SCSI_STATUS_RESERVATION_CONFLICT;
    return SCSI_STEP_SEND_STATUS;
}

int scsi_status_step_5(void)
{
    scsi_status_byte = SCSI_STATUS_BUSY;
    return SCSI_STEP_SEND_STATUS;
}

int scsi_status_step_6(void)
{
    scsi_init_sense(0);
    scsi_sense_owner.v = scsi_initiator_bit.c;
    scsi_status_byte = SCSI_STATUS_CHECK_CONDITION;
    return SCSI_STEP_SEND_STATUS;
}

int scsi_status_step_7(void)
{
    return 0;
}
#pragma section

char scsi_status_step_8(void)
{
    log_add_event(0x82, scsi_cdb[0]);
    if (scsi_cdb[0] == SCSI_OP_OBJECT_POSITION || scsi_cdb[0] == SCSI_OP_SCAN || scsi_cdb[0] == SCSI_OP_SCANNER_CONTROL ||
        (scsi_cdb[0] == SCSI_OP_MODE_SELECT && scsi_param_byte_4[0] == SCSI_PAGE_BUFFER)) {
        log_add_record(0x82, scsi_cdb[0], 0, 0);
    } else if (scsi_cdb[0] == SCSI_OP_READ) {
        if (scsi_cdb_byte_2[0] == SCSI_READ_IMAGE)
            log_add_record(0x82, SCSI_OP_READ, scsi_read_side.v, 0);
        else if (scsi_cdb_byte_2[0] == SCSI_READ_PIXEL_SIZE)
            log_add_record(0x82, SCSI_OP_READ, scsi_read_side.c.lo | 0x80, 0);
    }
    if (hw_hostif_flags.f.usb) {
        usb_send_status();
        if (scsi_update_request == 0x40)
            scsi_update_request = 0x80;
        return 0;
    }
    scsi_send_status_byte();
    if (scsi_run_messages(1) == 0) {
        if (scsi_status)
            return scsi_status;
        if (!scsi_finish_command()) {
            if (scsi_status)
                return scsi_status;
            else
                return SCSI_STEP_CHECK_CONDITION;
        } else
            return SCSI_STEP_SEND_STATUS;
    } else {
        scsi_message_retries.v = 0;
        return 9;
    }
}

char scsi_dispatch(void)
{
    scsi_reply_message.v = 0;
    scsi_send_message_byte();
    if (scsi_run_messages(1) == 0) {
        if (scsi_status)
            return scsi_status;
        else
            return 9;
    } else {
        scsi_release_bus();
        return 0;
    }
}


void scsi_execute_command(saved_imask)
unsigned char saved_imask;
{
    struct scsi_cmd *entry;
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.usb)
        scsi_saved_imask = saved_imask;
    _builtin_set_imask(scsi_saved_imask);
    entry = scsi_cmd_table;
    while (entry->handler != 0) {
        if (entry->opcode == scsi_cdb[0])
            break;
        entry++;
    }
    log_add_event(2, scsi_cdb[0]);
    if (entry->handler != 0) {
        entry->handler();
    } else {
        scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_OPCODE);
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        if (hw_hostif_flags.f.usb) {
            _builtin_set_gbr(0);
            usb_ctrl->reg_17.b6 = 1;
        }
    }
    if (hw_hostif_flags.f.usb)
        scsi_run_status_steps();
}

int scsi_cmd_test_unit_ready(void)
{
    if (!scsi_check_ready())
        return;
    if ((scsi_cdb_byte_1[0] & ~0xe0) || scsi_cdb_byte_2[0] || scsi_cdb_byte_3[0] || scsi_cdb_byte_4[0] || scsi_cdb_byte_5[0]) {
        scsi_sense_invalid_cdb();
        return;
    }
    scsi_status = SCSI_STEP_GOOD;
}

int scsi_cmd_release_unit(void)
{
    if (!scsi_check_ready())
        return;
    else {
        if ((scsi_cdb_byte_1[0] & 0x11) || scsi_cdb_byte_2[0] || scsi_cdb_byte_3[0] || scsi_cdb_byte_4[0] || scsi_cdb_byte_5[0]) {
            scsi_sense_invalid_cdb();
            return;
        }
        scsi_reserved_by.v = 0;
        scsi_status = SCSI_STEP_GOOD;
    }
}

int scsi_cmd_reserve_unit(void)
{
    if (!scsi_check_ready())
        return;
    else {
        if ((scsi_cdb_byte_1[0] & 0x11) || scsi_cdb_byte_2[0] || scsi_cdb_byte_3[0] || scsi_cdb_byte_4[0] || scsi_cdb_byte_5[0]) {
            scsi_sense_invalid_cdb();
            return;
        }
        scsi_reserved_by.v = scsi_initiator_bit.c;
        scsi_status = SCSI_STEP_GOOD;
    }
}

int scsi_cmd_inquiry(void)
{
    if ((scsi_cdb_byte_1[0] & ~0xe0) != 1) {
        if ((scsi_cdb_byte_1[0] & ~0xe0) || scsi_cdb_byte_2[0] || scsi_cdb_byte_3[0] || scsi_cdb_byte_5[0]) {
            if (hw_hostif_flags.f.usb)
                usb_send_reply(scsi_cdb_byte_4[0]);
            scsi_sense_invalid_cdb();
            return;
        } else {
        }
        if (scsi_inquiry_standard() == 0) {
            if (hw_hostif_flags.f.usb)
                usb_start_data_in();
            return;
        }
    } else {
        if (scsi_cdb_byte_3[0] || scsi_cdb_byte_5[0]) {
            if (hw_hostif_flags.f.usb)
                usb_send_reply(scsi_cdb_byte_4[0]);
            scsi_sense_invalid_cdb();
            return;
        } else {
        }
        if (scsi_cdb_byte_2[0] != SCSI_INQUIRY_VPD_PAGE) {
            if (hw_hostif_flags.f.usb)
                usb_send_reply(scsi_cdb_byte_4[0]);
            scsi_sense_invalid_cdb();
            return;
        }
        if (!scsi_inquiry_vpd()) {
            if (hw_hostif_flags.f.usb)
                usb_start_data_in();
            return;
        }
    }
    scsi_send_reply();
}

void scsi_cmd_request_sense(void)
{
    char *sense;
    int unused_frame;
    unsigned char alloc_len;
    if ((scsi_cdb_byte_1[0] & ~0xe0) || scsi_cdb_byte_2[0] || scsi_cdb_byte_3[0] || scsi_cdb_byte_5[0]) {
        if (hw_hostif_flags.f.usb)
            usb_send_reply(scsi_cdb_byte_4[0]);
        scsi_sense_invalid_cdb();
        return;
    }
    sense = (char *)scsi_sense_data;
    while (1) {
        if ((scsi_identify_message & 7) == 0) {
            if (scsi_sense_owner.v)
                break;
            if (!hw_hostif_flags.f.usb) {
                if (!scsi_check_unit_attention()) {
                    scsi_init_sense(1);
                    break;
                }
                if (!scsi_check_reservation()) {
                    sense = scsi_sense_reserved;
                    break;
                }
            }
            if (!err_report_page_error()) {
                scsi_init_sense(1);
                break;
            }
            if (!scsi_check_not_ready()) {
                scsi_init_sense(1);
                break;
            }
            if (scsi_sense_data[2] != 0) {
                break;
            } else {
                scsi_set_sense(SCSI_SENSE_NO_SENSE, SCSI_ASC_NONE);
                scsi_init_sense(1);
                break;
            }
        } else {
            sense = scsi_sense_lun_not_supported;
            break;
        }
    }
    util_mem_copy(scsi_reply_buf, sense, 18);
    alloc_len = scsi_cdb_byte_4[0];
    if (alloc_len == 0) {
        if (hw_hostif_flags.f.usb)
            usb_start_data_in();
        scsi_status = SCSI_STEP_GOOD;
    } else {
        if (alloc_len > 18 && !hw_hostif_flags.f.usb)
            alloc_len = 18;
        scsi_param_len.v = alloc_len;
        if (hw_hostif_flags.f.usb)
            usb_send_reply_data();
        else
            scsi_send_reply();
    }
}

#pragma section H
struct cell_u8_cdb { unsigned char v; };
extern int util_get_be16();

void scsi_cmd_send(void)
{
    unsigned char *src;
    unsigned char *dst;
    int count;
    int i;
    unsigned short len;
    unsigned short lim;
    unsigned char field;
    unsigned char ok;

    ok = scsi_check_ready();
    if (!ok && !hw_hostif_flags.f.usb) return;
    if (scsi_cdb_byte_2[0] == SCSI_SEND_HALFTONE || scsi_cdb_byte_2[0] == 0x82 || scsi_cdb_byte_2[0] == SCSI_SEND_GAMMA || scsi_cdb_byte_2[0] == SCSI_SEND_LUT ||
        scsi_cdb_byte_2[0] == SCSI_SEND_JPEG_QTABLE || (scsi_cdb_byte_2[0] == SCSI_SEND_ENDORSER_DATA && hw_option_flags.f.b0)) {
    } else {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb) return;
        ok = 0;
    }
    field = scsi_cdb_byte_3[0] | scsi_cdb_byte_4[0] | scsi_cdb_byte_6[0] | scsi_cdb_byte_9[0];
    if ((scsi_cdb_byte_1[0] & ~0xe0) || field) {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb) return;
        ok = 0;
    }
    if (scsi_cdb_byte_2[0] == SCSI_SEND_ENDORSER_DATA) {
        scsi_cmd_send_step_3();
        return;
    }
    if (scsi_cdb_byte_2[0] == SCSI_SEND_HALFTONE || scsi_cdb_byte_2[0] == 0x82 || scsi_cdb_byte_2[0] == SCSI_SEND_LUT || scsi_cdb_byte_2[0] == SCSI_SEND_GAMMA) {
        if (scsi_cdb_byte_5[0] >= 2) {
            scsi_sense_invalid_cdb();
            if (!hw_hostif_flags.f.usb) return;
            ok = 0;
        }
    } else {
        if (scsi_cdb_byte_5[0] >= 1) {
            scsi_sense_invalid_cdb();
            if (!hw_hostif_flags.f.usb) return;
            ok = 0;
        }
    }
    len = scsi_cdb_byte_8[0].v | (scsi_cdb_byte_7[0].v << 8);
    if (scsi_cdb_byte_2[0] == SCSI_SEND_HALFTONE || scsi_cdb_byte_2[0] == 0x82) {
        lim = 0x40a;
    } else if (scsi_cdb_byte_2[0] == SCSI_SEND_GAMMA) {
        lim = 0x40a;
    } else if (scsi_cdb_byte_2[0] == SCSI_SEND_LUT) {
        lim = 0xc0a;
    } else if (scsi_cdb_byte_2[0] == SCSI_SEND_JPEG_QTABLE) {
        lim = 0x8a;
    } else {
    }
    if (len > lim) {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb) return;
        ok = 0;
    }
    scsi_param_len.v = len;
    if (len == 0) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (!scsi_receive_params() || !ok) return;
    if (scsi_cdb_byte_2[0] == SCSI_SEND_HALFTONE || scsi_cdb_byte_2[0] == 0x82) {
        len = util_get_be16(scsi_param_byte_4);
        lim = util_get_be16(scsi_param_byte_6);
        len = len * lim;
        len += 10;
        if (len != scsi_param_len.c.lo) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, len);
            return;
        }
    } else if (scsi_cdb_byte_2[0] == SCSI_SEND_GAMMA) {
        len = util_get_be16(scsi_param_byte_4);
        if (len != 0x100 && len != 0x400) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, 4);
            return;
        }
        if (util_get_be16(scsi_param_byte_6) != 0x100) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, 6);
            return;
        }
        len += 10;
        if (len != scsi_param_len.c.lo) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, len);
            return;
        }
    } else if (scsi_cdb_byte_2[0] == SCSI_SEND_LUT) {
        scsi_send_qualifier = scsi_cdb_byte_5[0];
        field = scsi_param_byte_2[0];
        if (field == 0 || field == 0x10) {
        } else {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, 2);
            return;
        }
        len = util_get_be16(scsi_param_byte_4);
        if (len != 0x100 && len != 0x400) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, 4);
            return;
        }
        if (util_get_be16(scsi_param_byte_6) != 0x100) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, 6);
            return;
        }
    } else if (scsi_cdb_byte_2[0] == SCSI_SEND_JPEG_QTABLE) {
        len = util_get_be16(scsi_param_byte_4);
        if (len != 0x40) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, 4);
            return;
        }
        len = util_get_be16(scsi_param_byte_6);
        if (len != 0x40) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, 6);
            return;
        }
        len = len + len + 10;
        if (len != scsi_param_len.c.lo) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, len);
            return;
        }
    } else {
    }
    if (scsi_cdb_byte_2[0] == SCSI_SEND_HALFTONE || scsi_cdb_byte_2[0] == 0x82) {
        count = scsi_cdb_byte_5[0];
        img_halftone_downloads[count].v = 1;
        img_halftone_downloads[count].width = scsi_param_byte_5[0];
        img_halftone_downloads[count].height = scsi_param_byte_7[0];
        dst = &img_download_halftone[count][0];
        src = scsi_param_byte_10;
        count = scsi_param_len.v - 10;
    } else if (scsi_cdb_byte_2[0] == SCSI_SEND_GAMMA) {
        count = scsi_cdb_byte_5[0];
        img_gamma_loaded[count] = 0xff;
        dst = &img_download_gamma[count][0];
        src = scsi_param_byte_10;
        count = scsi_param_len.v - 10;
    } else if (scsi_cdb_byte_2[0] == SCSI_SEND_LUT) {
        if (scsi_send_qualifier >= 2) {
            scsi_sense_invalid_cdb();
            return;
        }
        img_lut_loaded[scsi_send_qualifier] = 0xff;
        count = scsi_param_len.v - 10;
        util_mem_copy(&img_download_lut[scsi_send_qualifier][0], scsi_param_byte_10, count);
        if (count == 0x100 || count == 0x300) {
            dst = scsi_param_byte_10;
            src = img_download_lut[scsi_send_qualifier];
            util_mem_copy(dst, src, count);
            dst = img_download_lut[scsi_send_qualifier];
            src = scsi_param_byte_10;
        }
    } else if (scsi_cdb_byte_2[0] == SCSI_SEND_JPEG_QTABLE) {
        count = scsi_cdb_byte_5[0];
        if (count) {
            scsi_sense_invalid_cdb();
            return;
        }
        jpeg_qtables_downloaded = 1;
        dst = jpeg_download_qtables;
        src = scsi_param_byte_10;
        count = scsi_param_len.v - 10;
    } else {
    }
    if (scsi_cdb_byte_2[0] == SCSI_SEND_LUT && (count == 0x400 || count == 0xc00)) {
        if (scsi_param_byte_2[0] == 0x10) {
            for (i = 0; i < 0x800; i++) {
                img_download_lut[scsi_send_qualifier][i + 0x400] = img_download_lut[scsi_send_qualifier][i];
            }
        }
    } else if ((scsi_cdb_byte_2[0] == SCSI_SEND_GAMMA && count == 0x100) || (scsi_cdb_byte_2[0] == SCSI_SEND_LUT && (count == 0x300 || count == 0x100))) {
        for (i = 0; i < count; i++) {
            *dst = *src;
            if (i == 0xff || i == 0x1ff || i == 0x2ff) {
                dst[1] = *src;
                dst[2] = *src;
                dst[3] = *src;
            } else {
                dst[1] = (*src * 3 + src[1]) / 4;
                dst[2] = (src[1] * 2 + *src * 2) / 4;
                dst[3] = (src[1] * 3 + *src) / 4;
            }
            dst += 4;
            src++;
        }
        if (scsi_cdb_byte_2[0] == SCSI_SEND_LUT && scsi_param_byte_2[0] == 0x10) {
            for (i = 0; i < 0x800; i++) {
                img_download_lut[scsi_send_qualifier][i + 0x400] = img_download_lut[scsi_send_qualifier][i];
            }
        }
    } else {
        for (i = 0; i < count; i++) {
            *dst = *src;
            dst++;
            src++;
        }
    }
    if (scsi_cdb_byte_2[0] == SCSI_SEND_HALFTONE || scsi_cdb_byte_2[0] == 0x82) {
        img_loaded_tables[0].halftone_built = 0;
        img_loaded_tables[0].halftone_loaded = 0;
        img_loaded_tables[1].halftone_built = 0;
        img_loaded_tables[1].halftone_loaded = 0;
    } else if (scsi_cdb_byte_2[0] == SCSI_SEND_GAMMA || scsi_cdb_byte_2[0] == SCSI_SEND_LUT) {
        util_mem_clear(&img_loaded_tables[0].gamma_built_mono, 4);
        util_mem_clear(img_loaded_tables[0].gamma_loaded_mono, 4);
        util_mem_clear(&img_loaded_tables[1].gamma_built_mono, 4);
        util_mem_clear(img_loaded_tables[1].gamma_loaded_mono, 4);
        util_mem_clear(&img_loaded_tables[0].gamma_built_color, 4);
        util_mem_clear(img_loaded_tables[0].gamma_loaded_color, 4);
        util_mem_clear(&img_loaded_tables[1].gamma_built_color, 4);
        util_mem_clear(img_loaded_tables[1].gamma_loaded_color, 4);
    }
    scsi_status = SCSI_STEP_GOOD;
}

void scsi_cmd_send_step_3(void)
{
    int i;
    int len;
    unsigned char *p;
    int has_counter;
    int counter_8_digits;
    unsigned char text_len;
    unsigned char buf[87];
    unsigned short lim;
    unsigned char style;
    unsigned char unused_frame;
    unsigned short slot;
    unsigned char ok;

    has_counter = 0;
    ok = 1;
    if (scsi_cdb_byte_5[0]) {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb) return;
        ok = 0;
    }
    len = (unsigned short)(scsi_cdb_byte_8[0].v | (scsi_cdb_byte_7[0].v << 8));
    if (len < 18 || len > 98) {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb) return;
        ok = 0;
    }
    scsi_param_len.v = len;
    if (len == 0) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (!scsi_receive_params() || !ok) return;
    if (scsi_param_buf[0]) {
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 0);
        return;
    }
    if ((scsi_param_byte_1[0] & 0xcc) || (scsi_param_byte_1[0] & 3) == 3) {
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 1);
        return;
    }
    if (util_get_be32(scsi_param_byte_2)) {
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 2);
        return;
    }
    if (scsi_param_byte_10[0] > 1) {
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 10);
        return;
    }
    if (scsi_param_byte_11[0]) {
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 11);
        return;
    }
    style = scsi_param_byte_12[0];
    if ((style & ~7) || !(style & 3) || (style & 3) == 2) {
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 12);
        return;
    }
    if (scsi_param_byte_17[0] > 80) {
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 17);
        return;
    }
    i = util_get_be32(scsi_param_byte_6);
    i = (unsigned)i >> 1;
    if (i > 0xffff) {
        lim = 0xffff;
    } else {
        lim = i;
    }
    i = 0;
    text_len = 0;
    for (; i < len - 18; i++, text_len++) {
        p = (unsigned char *)scsi_param_buf + i + 18;
        if (*p == 0)
            break;
        if (*p < 32 || *p > 126) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, (unsigned char)i + 18);
            return;
        }
        if (*p == 37) {
            if (p[1] == 37) {
                buf[text_len] = *p;
                i++;
                continue;
            }
            if (util_mem_equal(p + 1, scsi_counter_format_5, 4)) {
                counter_8_digits = 0;
            } else if (util_mem_equal(p + 1, scsi_counter_format_8, 4)) {
                counter_8_digits = 1;
            } else {
                scsi_sense_invalid_param(SCSI_SENSE_SKSV, (unsigned char)(i + 1) + 18);
                return;
            }
            has_counter = 1;
            break;
        }
        buf[text_len] = *p;
    }
    buf[text_len] = 0;
    i = text_len;
    if (has_counter) {
        if (counter_8_digits) {
            i += 8;
            if (i > 43) {
                scsi_sense_invalid_param(SCSI_SENSE_SKSV, 18);
                return;
            }
        } else {
            i += 5;
            if (i > 40) {
                scsi_sense_invalid_param(SCSI_SENSE_SKSV, 18);
                return;
            }
        }
    } else {
        if (i > 40) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, 18);
            return;
        }
    }
    scsi_imprinter_print_len = i;
    scsi_imprinter_style.v = 0;
    scsi_imprinter_flags.v = 0;
    scsi_imprinter_text_len.v = text_len + 1;
    if (has_counter) {
        scsi_imprinter_flags.f.b7 = 1;
        if (counter_8_digits) {
            scsi_imprinter_flags.f.b0 = 1;
        } else {
            scsi_imprinter_flags.f.b0 = 0;
        }
    } else {
        scsi_imprinter_flags.f.b7 = 0;
    }
    util_mem_copy(scsi_imprinter_text, buf, text_len + 1);
    scsi_imprinter_flags.f.b5 = 1;
    if (scsi_param_byte_1[0] & 0x20) scsi_imprinter_flags.f.b4 = 1;
    if (scsi_param_byte_1[0] & 0x10) {
        scsi_imprinter_flags.f.b1 = 1;
    } else {
        scsi_imprinter_flags.f.b1 = 0;
    }
    scsi_imprinter_counter_step.v = scsi_param_byte_1[0] & 3;
    if (!scsi_imprinter_counter_step.v) scsi_imprinter_flags.f.b5 = 0;
    win_params.imprinter_y_offset = lim;
    style = scsi_param_byte_12[0];
    if ((style & 3) == 3) scsi_imprinter_style.f.direction = 2;
    if (style & 4) scsi_imprinter_style.f.bold = 1;
    if (scsi_param_byte_10[0] == 1) scsi_imprinter_style.f.font = 1;
    if ((settings_emulation[0] & 0x1c) != 0x14 && scsi_param_byte_10[0] == 1 && (style & 8)) scsi_sense_invalid_param(SCSI_SENSE_SKSV, 18);
    win_params.b20.b6 = 1;
    _builtin_set_imask(15);
    slot = scsi_imprinter_next_slot();
    mdc_back_window[slot] = 0xc0;
    slot = scsi_imprinter_next_slot();
    mdc_back_window[slot] = scsi_imprinter_print_len;
    slot += 2;
    mdc_back_window[slot] = 0xff;
    slot += 2;
    mdc_back_window[slot] = 0xff;
    _builtin_set_imask(scsi_saved_imask);
    scsi_status = SCSI_STEP_GOOD;
}


void scsi_cmd_set_window(void)
{
    int unused_frame;
    unsigned char *p;
    unsigned int offset;
    unsigned short len;
    unsigned char reserved_bits;
    unsigned char ok;

    ok = scsi_check_ready();
    if (!ok && !hw_hostif_flags.f.usb) return;
    reserved_bits = scsi_cdb_byte_2[0] | scsi_cdb_byte_3[0] | scsi_cdb_byte_4[0] | scsi_cdb_byte_5[0] | scsi_cdb_byte_6[0] | scsi_cdb_byte_9[0];
    if ((scsi_cdb_byte_1[0] & ~0xe0) || reserved_bits) {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb) return;
        ok = 0;
    }
    len = scsi_cdb_byte_8[0].v | (scsi_cdb_byte_7[0].v << 8);
    scsi_param_len.v = len;
    if (len == 0) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (len < 48 || len > 0x100) {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb) return;
        ok = 0;
    }
    if (!scsi_receive_params() || !ok) return;
    scsi_page_read_done = 0;
    if (main_page_loop_phase == 0 && page_side_state[0].last_buf != 0 && (page_side_state[0].events != 0 || page_side_state[1].events != 0)) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    _builtin_set_imask(15);
    scsi_object_position_done = 0;
    _builtin_set_imask(scsi_saved_imask);
    p = scsi_param_byte_8;
    scsi_win_saved_descriptors[0] = 0;
    scsi_win_saved_back_descriptor[0] = 0;
    p = scsi_param_buf;
    for (;;) {
        scsi_param_valid.v = 1;
        scsi_win_check_header(p);
        if (scsi_param_valid.v == 0) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, scsi_field_pointer);
            return;
        }
        scsi_win_current_descriptor = scsi_win_descriptor_addr;
        scsi_win_current_end.v = (unsigned char *)scsi_win_descriptor_end;
        p = scsi_win_current_descriptor;
        scsi_param_valid.v = 1;
        scsi_win_parse_descriptor((unsigned int)p);
        if (scsi_param_valid.v == 0) {
            scsi_sense_invalid_param(SCSI_SENSE_SKSV, scsi_field_pointer);
            return;
        }
        scsi_win_save_descriptor();
        p = scsi_win_current_end.v;
        offset = scsi_win_current_end.v - scsi_param_buf;
        if (offset >= scsi_param_len.v) break;
    }
    if (scsi_win_saved_descriptors[0] == 0x80) {
        if (scsi_win_saved_back_descriptor[0] == 0x80) {
            win_sides.v = 2;
        } else {
            win_sides.v = 0;
        }
    } else if (scsi_win_saved_back_descriptor[0] == 0x80) {
        util_mem_copy(win_params.side, &win_params.side[1], 60);
        win_sides.v = 1;
    } else {
        scsi_win_saved_descriptors[0] = 0;
        scsi_win_saved_back_descriptor[0] = 0;
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, scsi_field_pointer);
        return;
    }
    if (scsi_win_saved_descriptors[0] == 0x80) scsi_read_sides[0].window_set = 1;
    if (scsi_win_saved_back_descriptor[0] == 0x80) scsi_read_sides[1].window_set = 1;
    scsi_read_sides[0].flags.w = 0;
    scsi_read_sides[1].flags.w = 0;
    scsi_read_progress.v = 0;
    scsi_last_page_size[0].size_final = 0;
    scsi_last_page_size[1].size_final = 0;
    scsi_last_page_size[0].length_lines = 0;
    scsi_last_page_size[1].length_lines = 0;
    scsi_last_page_size[0].width_px = 0;
    scsi_last_page_size[1].width_px = 0;
    scsi_status = SCSI_STEP_GOOD;
}


void scsi_cmd_object_position(void)
{
    struct page_record *current_page;
    struct page_record *scanning_page;
    unsigned short unused_frame;
    unsigned char reserved_bits;

    current_page = (struct page_record *)(page_current.v * 136 + (unsigned long)page_records);
    scanning_page = (struct page_record *)(page_scanning.v * 136 + (unsigned long)page_records);
    if (page_side_state[0].last_buf != 0) {
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    }
    _builtin_set_imask(15);
    job_manual_feed_flags.w = 0;
    _builtin_set_imask(scsi_saved_imask);
    if (scsi_check_ready() == 0) {
        return;
    }
    reserved_bits = scsi_cdb_byte_2[0] | scsi_cdb_byte_3[0] | scsi_cdb_byte_4[0] | scsi_cdb_byte_5[0] | scsi_cdb_byte_6[0] | scsi_cdb_byte_7[0].v | scsi_cdb_byte_8[0].v | scsi_cdb_byte_9[0];
    if (reserved_bits || (scsi_cdb_byte_1[0] & ~0xe0) > 1) {
        scsi_sense_invalid_cdb();
        return;
    }
    scsi_unusual_paper_state &= 0xfe;
    if ((scsi_cdb_byte_1[0] & ~0xe0) == 1) {
        log_add_record(2, 49, 1, 0);
        _builtin_set_imask(15);
        scsi_page_read_done = 0;
        if (current_page->sides[0].flags.f.requested || current_page->sides[1].flags.f.requested) {
            _builtin_set_imask(15);
            scsi_clear_read_state();
        }
        current_page->flags.f.load_pending = 1;
        _builtin_set_imask(scsi_saved_imask);
        scsi_read_progress.v = 0;
        scsi_end_state = 1;
        if (scsi_disconnect() == 0) {
            if (scsi_status) {
                scsi_end_state = 0;
                return;
            }
        }
        scsi_defer_enable_irq();
        return;
    } else {
        log_add_record(2, 49, 0, 0);
        _builtin_set_imask(15);
        scsi_object_position_done = 1;
        _builtin_set_imask(scsi_saved_imask);
        _builtin_set_imask(scsi_saved_imask);
        if (mdc_busy.w) {
            scsi_status = SCSI_STEP_GOOD;
            return;
        }
        if (!scanning_page->flags.f.sheet_fed) {
            scsi_status = SCSI_STEP_GOOD;
            return;
        }
        if (job_buffer_feed_flags & 0x40) {
            scsi_status = SCSI_STEP_GOOD;
            return;
        }
        mdc_run_flatbed_scan_move();
        _builtin_set_imask(15);
        err_multifeed_reported = 0;
        scanning_page->flags.f.sheet_fed = 0;
        scanning_page->flags.f.scan_end = 1;
        _builtin_set_imask(scsi_saved_imask);
        scsi_status = SCSI_STEP_GOOD;
    }
}

extern void scsi_sense_invalid_param_at();


/* SEND DIAGNOSTIC: the parameter list is a 16-byte space-padded keyword (scsi_keyword_debug_e2p_wrt..scsi_keyword_get_powoff_time) and its
   arguments, or a binary block 0xFB/0xFC/0xFE + mode; sets scsi_diag_request for READ DIAGNOSTIC. */
void scsi_cmd_send_diagnostic(void)
{
    struct page_record *current_page = (struct page_record *)(page_current.v * 136 + (unsigned long)page_records);
    unsigned char *p;
    unsigned char *src;
    unsigned char *dst;
    int off;
    int ok;
    struct flash_download_target *rec;
    unsigned char target;
    unsigned short offset;
    unsigned short len;
    unsigned short head_len;
    unsigned char arg1;
    unsigned char arg2;
    unsigned char cdb_ok = 1;
    unsigned short block_len = 0;
    unsigned short block_offset = 0;
    int i20 = 0;
    unsigned short item_count = 0;
    unsigned short item_offset = 0;
    unsigned short item_index = 0;
    unsigned char item_mask = 0;
    unsigned char item_bits = 0;
    unsigned char old_bits = 0;
    unsigned char new_bits = 0;
    unsigned char writable_bits = 0;
    unsigned char protected_bits = 0;
    unsigned char untouched_bits = 0;
    unsigned char written_bits = 0;
    struct page_record *scanning_page = (struct page_record *)(page_scanning.v * 136 + (unsigned long)page_records);

    if (page_side_state[0].last_buf != 0) {
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    }
    if (scsi_check_ready() == 0) {
        if (hw_hostif_flags.f.usb) {
            cdb_ok = 0;
        } else {
            return;
        }
    }
    arg1 = scsi_cdb_byte_1[0];
    arg2 = arg1;
    arg1 &= 8;
    arg1 |= scsi_cdb_byte_2[0];
    arg2 &= 20;
    if (arg2 & 20) {
        if (!(arg2 & 4)) {
            scsi_sense_invalid_cdb();
            if (hw_hostif_flags.f.usb) {
                cdb_ok = 0;
            } else {
                return;
            }
        } else {
            arg1 |= scsi_cdb_byte_3[0];
            arg1 |= scsi_cdb_byte_4[0];
        }
    }
    arg1 |= scsi_cdb_byte_5[0];
    if (arg1) {
        scsi_sense_invalid_cdb();
        if (hw_hostif_flags.f.usb) {
            cdb_ok = 0;
        } else {
            return;
        }
    }
    if (arg2 & 20) {
        if (hw_hostif_flags.f.usb) {
            scsi_param_len.v = (unsigned short)(((struct cell_u8_cdb *)scsi_cdb_byte_4)[0].v | ((struct cell_u8_cdb *)scsi_cdb_byte_3)[0].v << 8);
            if (scsi_param_len.v) {
                scsi_defect_transfer.defect_area = 1;
                scsi_receive_params();
                scsi_defect_transfer.defect_area = 0;
            }
        }
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    scsi_param_len.v = (unsigned short)(((struct cell_u8_cdb *)scsi_cdb_byte_4)[0].v | ((struct cell_u8_cdb *)scsi_cdb_byte_3)[0].v << 8);
    scsi_diag_request = 0;
    if (scsi_param_len.v == 0) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (scsi_param_len.v < 2 || scsi_param_len.v > 0x416) {
        if (!(flash_download_session & 0x80)) {
            if (scsi_param_len.v > 0x500) {
                (*(union UBF *)&scsi_defect_transfer).v |= 0x80;
            } else {
                scsi_sense_invalid_cdb();
                if (hw_hostif_flags.f.usb) {
                    cdb_ok = 0;
                } else {
                    return;
                }
            }
        }
    }
    scsi_message_retries.v = 0;
    if (scsi_receive_params() == 0) {
        scsi_defect_transfer.defect_area = 0;
        return;
    }
    scsi_defect_transfer.defect_area = 0;
    if (cdb_ok == 0) {
        return;
    }
    if ((flash_download_session & 0x80) && scsi_param_len.v > 0x2000) {
        p = (unsigned char *)flash_download_buf;
    } else if (scsi_param_len.v > 0x500) {
        p = (unsigned char *)scsi_param_buf_large;
    } else {
        p = scsi_param_buf;
    }
    if (scsi_setup_mode) {
        if ((scanning_page->flags.v & PAGE_FLAGS_ACTIVE) || (current_page->flags.v & PAGE_FLAGS_FED_OR_SCANNED)) {
            off = 0;
            if (util_mem_compare_count(p, scsi_keyword_debug_e2p_wrt, 16, &off) || util_mem_compare_count(p, scsi_keyword_debug_e2t_wrt_w, 16, &off) ||
                util_mem_compare_count(p, scsi_keyword_debug_e2t_red_w, 16, &off) || util_mem_compare_count(p, scsi_keyword_setup_start_t01, 16, &off) ||
                util_mem_compare_count(p, scsi_keyword_get_imp_life, 13, &off) || util_mem_compare_count(p, scsi_keyword_clear_ink_life, 15, &off)) {
                scsi_sense_invalid_param_at(p);
                return;
            }
        }
    }
    off = 0;
    ok = 1;
    while (1) {
        if (util_mem_compare_count(p, scsi_keyword_debug_e2p_wrt, 16, &off)) {
            if (scsi_param_len.v != 16) {
                ok = 0;
                break;
            }
            if (iic_busy.b7) {
                iic_busy.b5 = 1;
            } else {
                iic_eeprom_save_settings();
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_E2P_WRT;
            scsi_diag_reply_len.v = 0x100;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_e2t_wrt, 16, &off)) {
            if (scsi_param_len.v < 19) {
                ok = 0;
                break;
            }
            p = (unsigned char *)((int)p + off);
            arg1 = *p;
            p++;
            if (arg1 > 0xff) {
                ok = 0;
                break;
            }
            arg2 = *p;
            offset = arg2;
            p++;
            if (offset == 0) {
                ok = 0;
                break;
            }
            offset += 18;
            if (scsi_param_len.v != offset) {
                ok = 0;
                break;
            }
            if (arg2 + arg1 > 0x100) {
                ok = 0;
                break;
            }
            dst = settings;
            if (iic_busy.b7) {
                dst = iic_pending_settings;
                iic_pending_len = arg2;
                iic_pending_offset = arg1;
                iic_busy.b6 = 1;
            }
            dst += arg1;
            util_mem_copy(dst, p, arg2);
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_E2T_WRT;
            scsi_diag_reply_len.v = 0x100;
            if (!iic_busy.b7) {
                job_apply_settings();
            }
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_e2t_red, 16, &off)) {
            if (scsi_param_len.v != 16) {
                ok = 0;
                break;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_E2T_RED;
            scsi_diag_reply_len.v = 0x100;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_e2t_wrt_w, 16, &off)) {
            if (scsi_param_len.v < 21) {
                ok = 0;
                break;
            }
            p = (unsigned char *)((int)p + off);
            offset = p[0] << 8 | p[1];
            if (offset > 0x1ff) {
                ok = 0;
                break;
            }
            len = p[2] << 8 | p[3];
            if (len == 0) {
                ok = 0;
                break;
            }
            if (offset + len > 0x200) {
                ok = 0;
                break;
            }
            p += 4;
            head_len = offset;
            if (head_len < 88 && head_len + len > 88) {
                head_len = 88 - head_len;
                src = p + head_len;
                if (*src != 0) {
                    break;
                }
            }
            dst = settings;
            if (iic_busy.b7) {
                dst = iic_pending_settings;
                iic_pending_len = arg2;
                iic_pending_offset = arg1;
                iic_busy.b6 = 1;
            }
            dst += offset;
            util_mem_copy(dst, p, len);
            if (settings_intelligent_multifeed[0] & 0x40) {
                settings_intelligent_multifeed[0] &= 0xbf;
                main_settings_request = 1;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_E2T_WRT_W;
            scsi_diag_reply_len.v = 0x200;
            if (!iic_busy.b7) {
                job_apply_settings();
            }
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_e2t_red_w, 16, &off)) {
            if (scsi_param_len.v != 16) {
                ok = 0;
                break;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_E2T_RED_W;
            scsi_diag_reply_len.v = 0x200;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_e2t_wrt_2, 16, &off)) {
            if (scsi_param_len.v < 19) {
                ok = 0;
                break;
            }
            p = (unsigned char *)((int)p + off);
            arg1 = *p;
            p++;
            if (arg1 > 0xff) {
                ok = 0;
                break;
            }
            arg2 = *p;
            offset = arg2;
            p++;
            if (offset == 0) {
                ok = 0;
                break;
            }
            offset += 18;
            if (scsi_param_len.v != offset) {
                ok = 0;
                break;
            }
            if (arg2 + arg1 > 0x200) {
                ok = 0;
                break;
            }
            dst = settings_upper_half;
            if (iic_busy.b7) {
                dst = iic_pending_upper_half;
                iic_pending_len = arg2;
                iic_pending_offset = arg1;
                iic_busy.b6 = 1;
            }
            dst += arg1;
            util_mem_copy(dst, p, arg2);
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_E2T_WRT_2;
            scsi_diag_reply_len.v = 0x100;
            if (!iic_busy.b7) {
                job_apply_settings();
            }
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_e2t_red_2, 16, &off)) {
            if (scsi_param_len.v != 16) {
                ok = 0;
                break;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_E2T_RED_2;
            scsi_diag_reply_len.v = 0x100;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_ram_wrt, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v < 23) {
                ok = 0;
                break;
            }
            scsi_diag_ram_addr_hi.v = p[0] << 8 | p[1];
            scsi_diag_ram_addr_lo.v = p[2] << 8 | p[3];
            scsi_diag_reply_len.v = p[4] << 8 | p[5];
            if (scsi_diag_reply_len.v > 0x400 || scsi_diag_reply_len.v + 22 != scsi_param_len.v || scsi_diag_reply_len.v == 0) {
                ok = 0;
                break;
            }
            src = scsi_param_byte_22;
            dst = (unsigned char *)(scsi_diag_ram_addr_lo.v | (unsigned long)scsi_diag_ram_addr_hi.v << 16);
            util_mem_copy(dst, src, scsi_diag_reply_len.v);
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_RAM_WRT;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_ram_red, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 22) {
                ok = 0;
                break;
            }
            scsi_diag_ram_addr_hi.v = p[0] << 8 | p[1];
            scsi_diag_ram_addr_lo.v = p[2] << 8 | p[3];
            scsi_diag_reply_len.v = p[4] << 8 | p[5];
            if (scsi_diag_reply_len.v > 0x400 || scsi_diag_reply_len.v == 0) {
                ok = 0;
                break;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_RAM_RED;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_adf_sensor, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_ADF_SENSOR;
            scsi_diag_reply_len.v = 2;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_read_count, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_READ_COUNT;
            scsi_diag_reply_len.v = 12;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_read_count2, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_READ_COUNT2;
            scsi_diag_reply_len.v = 20;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_read_count_clr, 15, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            arg1 = *p;
            if (arg1 == 49) {
            } else if (arg1 == 50) {
                settings_pick_roller_count.c[0] = settings_pick_roller_count.c[1] = 0;
            } else if (arg1 == 51) {
                settings_brake_roller_count.c[0] = settings_brake_roller_count.c[1] = 0;
            } else {
                ok = 0;
            }
            if (ok) {
                if (iic_busy.b7) {
                    iic_busy.b5 = 1;
                } else {
                    iic_eeprom_save_settings();
                }
            }
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_first_read_date, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            log_clock_year = p[1]; /* +1..+6: year, month, day, hour, minute, second */
            log_clock_month.v = p[2];
            log_clock_day.v = p[3];
            log_clock_hour.v = p[4];
            log_clock_min.v = p[5];
            log_clock_sec.v = p[6];
            if (log_clock_set != 1) {
                log_clock_set = 1;
            }
            log_add_record(7, 1, 0, 0);
            log_start_day = log_clock_day.v;
            log_start_hour = log_clock_hour.v;
            if (settings_first_read_month[0] == 0 || settings_first_read_day[0] == 0) {
                if (*p != 0) {
                    ok = 0;
                    break;
                }
                p++;
                if (job_check_date(p[0], p[1], p[2]) == 0) {
                    scsi_first_read_date_stored = 0;
                } else {
                    _builtin_set_imask(15);
                    settings_first_read_year[0] = *p;
                    p++;
                    settings_first_read_month[0] = *p;
                    p++;
                    settings_first_read_day[0] = *p;
                    p++;
                    if (iic_busy.b7) {
                        iic_busy.b5 = 1;
                    } else {
                        iic_eeprom_save_settings();
                    }
                    _builtin_set_imask(scsi_saved_imask);
                    scsi_first_read_date_stored = 1;
                }
            } else {
                scsi_first_read_date_stored = 0;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_FIRST_READ_DATE;
            scsi_diag_reply_len.v = 1;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_first_date, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_FIRST_DATE;
            scsi_diag_reply_len.v = 4;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_device_id, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_DEVICE_ID;
            scsi_diag_reply_len.v = 12;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_rom_information, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_ROM_INFORMATION;
            scsi_diag_reply_len.v = 30;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_agc_clear, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            _builtin_set_imask(15);
            scsi_clear_read_state();
            _builtin_set_imask(scsi_saved_imask);
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_eeprom_sts, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_EEPROM_STS;
            scsi_diag_reply_len.v = 2;
            break;
        }
        if (*p == SCSI_DIAG_TARGET_DEFECT_LISTS) {
            p++;
            if (*p == SCSI_DIAG_DEFECT_LISTS_WRITE) {
                if (scsi_param_len.v != SCSI_DIAG_DEFECT_LISTS_LEN + 2) {
                    scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                    scsi_status = SCSI_STEP_CHECK_CONDITION;
                    return;
                }
                flash_save_defect_lists();
                power_off_idle_seconds = 0;
                power_off_flags.f.idle_restarted = 1;
                break;
            }
            if (*p == SCSI_DIAG_DEFECT_LISTS_READ) {
                if (scsi_param_len.v != 4) {
                    ok = 0;
                    break;
                }
                p++;
                scsi_diag_reply_len.v = p[0] << 8 | p[1];
                if (scsi_diag_reply_len.v == SCSI_DIAG_DEFECT_LISTS_LEN) {
                    scsi_diag_request = SCSI_DIAG_REQUEST_DEFECT_LISTS;
                } else {
                    ok = 0;
                }
            } else {
                ok = 0;
            }
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_check_prnu_pos, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            dirt_defects_present = 1;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_erase_flash_work, 16, &off)) {   /* "ERASE FLASF WORK" (sic): erase 0x87C000 */
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            flash_erase_work();
            scsi_diag_request = SCSI_DIAG_REQUEST_ERASE_FLASH_WORK;
            scsi_diag_reply_len.v = 2;
            break;
        }
        /* FB (SDC): the application takes only 0x10 (restart into program 2) and 0x81 (block table) */
        if (*p == SCSI_DIAG_TARGET_SDC) {
            p++;
            scsi_diag_update_mode = *p;
            if (flash_download_session & 0x80) {
                ok = 0;
                break;
            }
            if (*p == SCSI_DIAG_FLASH_WRITE_START) {
                if (scsi_param_len.v != 2) {
                    ok = 0;
                    break;
                }
                scsi_update_request = 64;
                if (scsi_disconnect() == 0) {
                    if (scsi_status) {
                        return;
                    } else {
                        scsi_resume_flags.f.resume_pending = 1;
                        scsi_cmd_state = 1;
                        scsi_defer_enable_irq();
                        return;
                    }
                    break;
                } else {
                    scsi_resume_flags.f.resume_pending = 1;
                    scsi_cmd_state = 1;
                    scsi_defer_enable_irq();
                    return;
                }
            } else if (*p == SCSI_DIAG_FLASH_READ_INFO) {
                if (scsi_param_len.v != 4) {
                    ok = 0;
                    break;
                }
                p++;
                scsi_diag_request = SCSI_DIAG_REQUEST_SDC_BLOCK_TABLE;
                scsi_diag_reply_len.v = p[0] << 8 | p[1];
                if (scsi_diag_reply_len.v != 2 && scsi_diag_reply_len.v != 0x202) {
                    ok = 0;
                }
            } else {
                ok = 0;
            }
            break;
        }
        /* FC (imprinter, option bit) and FE (MDC): both are passed to the MDC mailbox by flash_mdc_run_download */
        if (*p == SCSI_DIAG_TARGET_IMPRINTER_POST || *p == SCSI_DIAG_TARGET_MDC) {
            if (*p == SCSI_DIAG_TARGET_IMPRINTER_POST) {
                target = 1;
                if (!hw_option_flags.f.b0 || flash_download_target_2_byte_0[0] != 0) {
                    ok = 0;
                    break;
                }
            } else if (*p == SCSI_DIAG_TARGET_MDC) {
                target = 2;
                if (flash_download_target_1_byte_0[0] != 0) {
                    ok = 0;
                    break;
                }
            } else {
                ok = 0;
                break;
            }
            rec = &flash_download_targets[target];
            flash_download_session &= 0xfc;
            flash_download_session |= target;
            if (rec->block_count == 0) {
                if (target == 1) {
                    rec->block_count = 10;
                } else if (target == 2) {
                    rec->block_count = 8;
                }
            }
            p++;
            if (*p == SCSI_DIAG_FLASH_WRITE_START) {
                if (scsi_param_len.v != 2) {
                    ok = 0;
                    break;
                }
                rec->progress_count = 0;
                panel_set_display_mode(1, PANEL_MODE_FUNCTION);
                rec->state = 2;
                flash_download_session = 0x80;
            } else if (*p == SCSI_DIAG_FLASH_WRITE_END) {
                if (!(flash_download_session & 0x80)) {
                    ok = 0;
                    break;
                }
                if (scsi_param_len.v != 2) {
                    ok = 0;
                    break;
                }
                panel_set_display_mode(0, PANEL_MODE_FUNCTION);
                rec->state = 3;
            } else if (*p == SCSI_DIAG_FLASH_WRITE) {
                p++;
                if (!(flash_download_session & 0x80)) {
                    ok = 0;
                    break;
                }
                rec->block = *p;
                p++;
                arg1 = rec->blocks[rec->block].size_kb;
                arg2 = *p;
                rec->write_len = arg2 << 10;
                p++;
                if (arg1 != arg2) {
                    ok = 0;
                    break;
                }
                if (scsi_param_len.v != rec->write_len + 8) {
                    ok = 0;
                    break;
                }
                for (offset = 0; offset < 4; offset++) {
                    if (*p != 0) {
                        ok = 0;
                        break;
                    }
                    p++;
                }
                if (!ok) {
                    break;
                }
                rec->write_addr = rec->blocks[rec->block].address;
                rec->data_ptr = p;
                rec->state = 4;
                if (rec->progress_count < rec->block) {
                    rec->progress_count = rec->block;
                }
                panel_set_display_mode(1, PANEL_MODE_FUNCTION);
            } else if (*p == SCSI_DIAG_FLASH_INFO_SEND) {
                if (!(flash_download_session & 0x80)) {
                    ok = 0;
                    break;
                }
                if (scsi_param_len.v != rec->block_count * 16 + 2) {
                    ok = 0;
                    break;
                }
                p++;
                rec->state = 6;
                util_mem_copy(flash_download_buf, scsi_param_byte_2, scsi_param_len.v - 2);
                if (rec->block >= 8 && rec->progress_count < 16) {
                    rec->progress_count++;
                }
                panel_set_display_mode(1, PANEL_MODE_FUNCTION);
            } else if (*p == SCSI_DIAG_FLASH_READ_BLOCK) {
                if (!(flash_download_session & 0x80)) {
                    ok = 0;
                    break;
                }
                if (scsi_param_len.v != 8) {
                    ok = 0;
                    break;
                }
                p++;
                rec->block = *p;
                p++;
                arg1 = rec->blocks[rec->block].size_kb;
                arg2 = *p;
                scsi_diag_reply_len.v = arg2 << 10;
                p++;
                if (arg1 != arg2) {
                    ok = 0;
                    break;
                }
                for (offset = 0; offset < 4; offset++) {
                    if (*p != 0) {
                        ok = 0;
                        break;
                    }
                    p++;
                }
                rec->read_addr = rec->blocks[rec->block].address;
                rec->data_ptr = (unsigned char *)flash_download_buf;
                rec->read_len = scsi_diag_reply_len.v;
                rec->read_pending = rec->read_len;
                rec->state = 7;
                if (rec->progress_count < 16) {
                    rec->progress_count++;
                }
                panel_set_display_mode(1, PANEL_MODE_FUNCTION);
            } else if (*p == SCSI_DIAG_FLASH_READ_INFO) {
                if (scsi_param_len.v != 4) {
                    ok = 0;
                    break;
                }
                p++;
                scsi_diag_reply_len.v = p[0] << 8 | p[1];
                if (scsi_diag_reply_len.v != 2 && scsi_diag_reply_len.v != rec->block_count * 16 + 2) {
                    ok = 0;
                }
                rec->state = 1;
                scsi_diag_request = SCSI_DIAG_REQUEST_MDC_BLOCK_READ;
                flash_mdc_run_download();
                break;
            } else if (*p == SCSI_DIAG_FLASH_READ_VERSION) {
                if (scsi_param_len.v != 2) {
                    ok = 0;
                    break;
                }
                scsi_diag_reply_len.v = 4;
                rec->state = 9;
                scsi_diag_request = SCSI_DIAG_REQUEST_MDC_BLOCK_READ;
                flash_mdc_run_download();
                break;
            } else {
                ok = 0;
                break;
            }
            flash_download_pending = 1;
            flash_download_session &= 0xfc;
            flash_download_session |= target;
            if (scsi_disconnect() == 0) {
                if (scsi_status) {
                    return;
                } else {
                    _builtin_set_imask(scsi_saved_imask);
                    scsi_cmd_state = 5;
                    scsi_defer_disable_irq();
                    return;
                }
            } else {
                scsi_cmd_state = 5;
                scsi_defer_enable_irq();
                return;
            }
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_test_length, 16, &off)) {   /* "TEST LENGTH" */
            arg1 = scsi_param_byte_16[0];
            if (arg1 == 0 || arg1 > 48) {
                break;
            }
            testmode_number.v = arg1;
            for (arg2 = 0; arg2 <= 48; arg2++) {
                if (testmode_number.v == scsi_testmode_reply_lens[arg2][0]) {
                    scsi_testmode_reply_len.v = scsi_testmode_lens_column[arg2][0];
                    break;
                }
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_TEST_LENGTH;
            scsi_diag_reply_len.v = 2;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_testmode, 16, &off)) {   /* "TESTMODE" n: testmode_number selects an entry of testmode_table */
            arg1 = scsi_param_byte_16[0];
            if (arg1 == 0 || arg1 > 48) {
                break;
            }
            testmode_number.v = arg1;
            scsi_diag_request = SCSI_DIAG_REQUEST_TESTMODE;
            scsi_diag_reply_len.v = scsi_testmode_reply_len.v;
            util_mem_clear(err_code_list, 4);
            if (testmode_number.v == 37 || testmode_number.v == 40 || testmode_number.v == 41) {
                arg1 = scsi_param_byte_17[0];
                if (arg1 == 0) {
                    testmode_param_absent = 1;
                    scsi_testmode_result_state.v = 0;
                } else {
                    testmode_param_absent = 0;
                }
            } else if (testmode_number.v == 38 || testmode_number.v == 43 || testmode_number.v == 39) {
                testmode_param_1.v = scsi_param_byte_17[0];
                arg1 = scsi_param_byte_18[0];
                if (arg1 == 0) {
                    testmode_param_absent = 1;
                    scsi_testmode_result_state.v = 0;
                } else {
                    testmode_param_absent = 0;
                }
            } else if (testmode_number.v == 44) {
                testmode_param_1.v = scsi_param_byte_17[0];
                testmode_param_2 = scsi_param_byte_18[0];
                arg1 = scsi_param_byte_19[0];
                if (arg1 == 0) {
                    testmode_param_absent = 1;
                    scsi_testmode_result_state.v = 0;
                } else {
                    testmode_param_absent = 0;
                }
            } else if (testmode_number.v == 32 || testmode_number.v == 34 || testmode_number.v == 33 || testmode_number.v == 36) {
                testmode_adjust_mode.v = scsi_param_byte_17[0];
            }
            power_off_idle_seconds = 0;
            power_off_flags.f.idle_restarted = 1;
            if (scsi_disconnect() == 0) {
                if (scsi_status) {
                    return;
                } else {
                    scsi_resume_flags.f.resume_pending = 1;
                    scsi_cmd_state = 1;
                    scsi_defer_enable_irq();
                    return;
                }
                break;
            } else {
                scsi_resume_flags.f.resume_pending = 1;
                scsi_cmd_state = 1;
                scsi_defer_enable_irq();
                return;
            }
        }
        if (util_mem_compare_count(p, scsi_keyword_scsi_id_get, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_SCSI_ID_GET;
            scsi_diag_reply_len.v = 1;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_load_cancel, 16, &off)) {
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            _builtin_set_imask(15);
            win_params.flags.f.adf = 0;
            _builtin_set_imask(scsi_saved_imask);
            scsi_diag_request = SCSI_DIAG_REQUEST_LOAD_CANCEL;
            scsi_diag_reply_len.v = 1;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_omr_sensor_test, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 17) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            if (!power_state.f.saving) {
                if (scsi_param_byte_16[0] == 0x80) {
                    mdc_mailbox.df_test_result = 0;
                    mdc_send_df_test(1);
                } else {
                    mdc_mailbox.df_test_result = 0;
                    mdc_send_df_test(0);
                }
            }
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_setup_start, 16, &off)) {
            if ((scanning_page->flags.v & PAGE_FLAGS_ACTIVE) || (current_page->flags.v & PAGE_FLAGS_FED_OR_SCANNED)) {
                ok = 0;
                break;
            }
            scsi_setup_mode = 1;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_setup_end, 16, &off)) {
            scsi_setup_mode = 0;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_measure_readtime, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            job_readtime_pages.v = 0;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_readtime, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_READTIME;
            scsi_diag_reply_len.v = 16;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_setup_start_t01, 16, &off)) {
            testmode_number.v = 48;
            err_code_count.v = 0;
            scsi_diag_request = SCSI_DIAG_REQUEST_SETUP_START_T01;
            scsi_diag_reply_len.v = 2;
            scsi_testmode_result_state.v = 0;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_firmversion, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_FIRMVERSION;
            scsi_diag_reply_len.v = 16;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_setup_imprinter, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            arg1 = *p;
            p++;
            arg1 = *p;
            if (arg1 == 0xff) {
            }
            p++;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_setup_imprinterr, 16, &off)) {
            scsi_diag_request = SCSI_DIAG_REQUEST_SETUP_IMPRINTERR;
            scsi_diag_reply_len.v = 6;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_diag_start, 16, &off)) {
            scsi_diag_started = 1;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_diag_end, 16, &off)) {
            scsi_diag_started = 0;
            break;
        }
        /* scsi_keyword_long_page "LONG PAGE": 00 sets, FF clears the long-page mode */
        if (util_mem_compare_count(p, scsi_keyword_long_page, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (p[1] == 0) {
                job_long_page_flags |= 0x80;
            } else if (p[1] == 0xff) {
                job_long_page_flags = 0;
            }
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_sop_default, 16, &off)) {
            if (scsi_param_len.v != 16) {
                ok = 0;
                break;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_SOP_DEFAULT;
            scsi_diag_reply_len.v = 0x200;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_powerswmode, 16, &off)) {
            if (scsi_param_len.v != 16) {
                ok = 0;
                break;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_POWERSWMODE;
            scsi_diag_reply_len.v = 2;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_sop_set_len, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_SOP_SET_LEN;
            scsi_diag_reply_len.v = 4;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_sop_set_dat, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_SOP_SET_DAT;
            scsi_diag_reply_len.v = 0xa0;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_put_sop_set_dat, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            arg1 = *p;
            block_len = arg1;
            block_len = block_len << 8;
            p++;
            arg1 = *p;
            block_len += arg1;
            p++;
            arg1 = *p;
            block_offset = arg1;
            block_offset = block_offset << 8;
            p++;
            arg1 = *p;
            block_offset += arg1;
            p++;
            p += 8;
            if (util_mem_compare_count(p, scsi_keyword_model_fi_6670dj, 16, &off) || util_mem_compare_count(p, scsi_keyword_model_fi_6770dj, 16, &off) ||
                util_mem_compare_count(p, scsi_keyword_model_fi_6750sj, 16, &off)) {
                scsi_sop_put_valid = 1;
            } else {
                scsi_sop_put_valid = 0;
            }
            p += 16;
            p++;
            arg1 = *p;
            if (arg1 < 68) {
                p++;
                arg1 = *p;
                scsi_sop_put_valid = 0;
            } else if (arg1 == 68) {
                p++;
                arg1 = *p;
                if (arg1 < 49) {
                    scsi_sop_put_valid = 0;
                }
            } else {
                p++;
            }
            p++;
            p += 18;
            p++;
            arg1 = *p;
            item_count = arg1;
            item_count = item_count << 8;
            p++;
            arg1 = *p;
            item_count += arg1;
            if (scsi_sop_put_valid) {
                if (iic_busy.b7) {
                    iic_busy.b5 = 1;
                }
                p++;
                for (item_index = 0; item_index < item_count; item_index++) {
                    arg1 = *p;
                    item_offset = arg1;
                    item_offset = item_offset << 8;
                    p++;
                    arg1 = *p;
                    item_offset += arg1;
                    p++;
                    item_mask = *p;
                    p++;
                    item_bits = *p;
                    old_bits = settings[item_offset + block_offset];
                    writable_bits = scsi_sop_item_mask(block_offset + item_offset);
                    protected_bits = old_bits & ~writable_bits;
                    untouched_bits = old_bits & ~item_mask & writable_bits;
                    written_bits = item_bits & item_mask & writable_bits;
                    new_bits = untouched_bits | protected_bits | written_bits;
                    settings[item_offset + block_offset] = new_bits;
                    p++;
                }
                if (!iic_busy.b5) {
                    iic_eeprom_save_settings();
                }
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_PUT_SOP_SET_DAT;
            scsi_diag_reply_len.v = 3;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_dev_log_len, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 17) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            arg1 = *p;
            if (arg1 != 1 && arg1 != 2) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_dev_log_selector = arg1;
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_DEV_LOG_LEN;
            scsi_diag_reply_len.v = 4;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_dev_log_dat, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 21) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            arg1 = *p;
            if (arg1 != 1 && arg1 != 2) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_dev_log_selector = arg1;
            p++;
            arg1 = *p;
            block_len = arg1;
            block_len = block_len << 8;
            p++;
            arg1 = *p;
            block_len += arg1;
            if (block_len > 0x200) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            p++;
            arg1 = *p;
            block_offset = arg1;
            block_offset = block_offset << 8;
            p++;
            arg1 = *p;
            block_offset += arg1;
            scsi_dev_log_chunk_size = block_len;
            scsi_dev_log_offset = block_offset;
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_DEV_LOG_DAT;
            scsi_diag_data_len.v = block_len;
            if ((settings_log_source[0] & 15) == 1 && hw_option_flags.f.b0 && scsi_dev_log_offset == 0) {
                scsi_mdc_log_request = 1;
                scsi_mdc_log_pending = 1;
                _builtin_set_imask(15);
                scsi_clear_read_state();
                _builtin_set_imask(scsi_saved_imask);
            }
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_e2p_wrt_x, 16, &off)) {
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            if (iic_busy.b7) {
                iic_busy.b4 = 1;
            } else {
                iic_eeprom_save_area2();
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_E2P_WRT_X;
            scsi_diag_data_len.v = 4;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_e2t_wrt_x, 16, &off)) {
            if (scsi_param_len.v < 20) {
                ok = 0;
                break;
            }
            p = (unsigned char *)((int)p + off);
            offset = p[0] << 8 | p[1];
            if (offset > 0x3ff) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            len = p[2] << 8 | p[3];
            if (len == 0) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            if (len + offset > 0x400) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            p += 4;
            dst = iic_eeprom_area2;
            if (iic_busy.b7) {
                dst = iic_pending_settings;
                iic_pending_len = len;
                iic_pending_offset = offset;
                iic_busy.b6 = 1;
            }
            dst = dst + offset;
            scsi_dev_log_offset = offset;
            scsi_dev_log_chunk_size = len;
            util_mem_copy(dst, p, len);
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_E2T_WRT_X;
            scsi_diag_data_len.v = scsi_dev_log_chunk_size;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_debug_e2t_red_x, 16, &off)) {
            if (scsi_param_len.v < 20) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            p = (unsigned char *)((int)p + off);
            offset = p[0] << 8 | p[1];
            if (offset > 0x3ff) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            len = p[2] << 8 | p[3];
            if (len == 0) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            if (len + offset > 0x400) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_dev_log_offset = offset;
            scsi_dev_log_chunk_size = len;
            scsi_diag_request = SCSI_DIAG_REQUEST_DEBUG_E2T_RED_X;
            scsi_diag_data_len.v = scsi_dev_log_chunk_size;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_set_powoff_time, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 18) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            arg1 = *p;
            p++;
            arg2 = *p;
            job_set_power_off_time(arg1, arg2);
            if (iic_busy.b6 == 1) {
                iic_pending_power_off_time[0] = arg1;
                iic_pending_power_off_warning[0] = arg2;
            }
            if (iic_busy.b7) {
                iic_busy.b5 = 1;
            } else {
                iic_eeprom_save_settings();
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_SET_POWOFF_TIME;
            scsi_diag_reply_len.v = 2;
            break;
        }
        if (util_mem_compare_count(p, scsi_keyword_get_powoff_time, 16, &off)) {
            p = (unsigned char *)((int)p + off);
            if (scsi_param_len.v != 16) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            scsi_diag_request = SCSI_DIAG_REQUEST_GET_POWOFF_TIME;
            scsi_diag_reply_len.v = 2;
            break;
        }
        break;
    }
    if (scsi_mdc_log_request) {
        if (scsi_disconnect() == 0) {
            if (scsi_status) {
                return;
            } else {
                scsi_resume_flags.f.resume_pending = 1;
                scsi_cmd_state = 1;
                scsi_defer_enable_irq();
                return;
            }
        } else {
            scsi_resume_flags.f.resume_pending = 1;
            scsi_cmd_state = 1;
            scsi_defer_enable_irq();
            return;
        }
    }
    if (!ok) {
        scsi_sense_invalid_param_at(p);
    } else {
        scsi_status = SCSI_STEP_GOOD;
    }
}


struct cell_u8_cdb_alloc { unsigned char v; };


void scsi_cmd_read_diagnostic(void)
{
    unsigned int src;
    unsigned char *p;
    unsigned int w;
    int count;
    unsigned short alloc_len;
    unsigned char request;
    unsigned char bits;
    struct page_record *current_page;
    struct page_record *scanning_page;

    current_page = &page_records[page_current.v];
    scanning_page = (struct page_record *)(page_scanning.v * 136 + (unsigned long)page_records);
    if (page_side_state[0].last_buf)
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    if (!scsi_check_ready()) {
        if (hw_hostif_flags.f.usb) {
            alloc_len = ((struct cell_u8_cdb_alloc *)scsi_cdb_byte_4)[0].v | ((struct cell_u8_cdb_alloc *)scsi_cdb_byte_3)[0].v << 8;
            usb_send_reply(alloc_len);
        }
        return;
    }
    if ((scsi_cdb_byte_1[0] & ~0xe0) != 0 || scsi_cdb_byte_2[0] != 0 || scsi_cdb_byte_5[0] != 0) {
        if (hw_hostif_flags.f.usb) {
            alloc_len = ((struct cell_u8_cdb_alloc *)scsi_cdb_byte_4)[0].v | ((struct cell_u8_cdb_alloc *)scsi_cdb_byte_3)[0].v << 8;
            usb_send_reply(alloc_len);
        }
        scsi_sense_invalid_cdb();
        return;
    }
    scsi_message_retries.v = 0;
    request = scsi_diag_request;
    scsi_diag_request = 0;
    alloc_len = ((struct cell_u8_cdb_alloc *)scsi_cdb_byte_4)[0].v | ((struct cell_u8_cdb_alloc *)scsi_cdb_byte_3)[0].v << 8;
    if (alloc_len == 0) {
        if (hw_hostif_flags.f.usb)
            usb_start_data_in();
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (!hw_hostif_flags.f.usb) {
        if (alloc_len <= scsi_diag_reply_len.v)
            scsi_diag_reply_len.v = alloc_len;
    } else {
        scsi_diag_reply_len.v = alloc_len;
    }
    if (scsi_setup_mode != 0 && ((scanning_page->flags.v & PAGE_FLAGS_ACTIVE) != 0 || (current_page->flags.v & PAGE_FLAGS_FED_OR_SCANNED) != 0)
        && (request == SCSI_DIAG_REQUEST_DEBUG_E2P_WRT || request == SCSI_DIAG_REQUEST_DEBUG_E2T_WRT_W || request == SCSI_DIAG_REQUEST_DEBUG_E2T_RED_W)) {
        if (hw_hostif_flags.f.usb) {
            alloc_len = ((struct cell_u8_cdb_alloc *)scsi_cdb_byte_4)[0].v | ((struct cell_u8_cdb_alloc *)scsi_cdb_byte_3)[0].v << 8;
            usb_send_reply(alloc_len);
        }
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 0);
        return;
    }
    for (;;) {
    switch (request) {
    case SCSI_DIAG_REQUEST_DEBUG_E2P_WRT:
    case SCSI_DIAG_REQUEST_DEBUG_E2T_WRT:
    case SCSI_DIAG_REQUEST_DEBUG_E2T_RED:
        util_mem_copy(scsi_reply_buf, settings, 0x100);
        break;
    case SCSI_DIAG_REQUEST_DEBUG_E2T_WRT_2:
    case SCSI_DIAG_REQUEST_DEBUG_E2T_RED_2:
        util_mem_copy(scsi_reply_buf, settings_upper_half, 0x100);
        break;
    case SCSI_DIAG_REQUEST_DEBUG_E2T_WRT_W:
    case SCSI_DIAG_REQUEST_DEBUG_E2T_RED_W:
        util_mem_copy(scsi_reply_buf, settings, 0x200);
        break;
    case SCSI_DIAG_REQUEST_DEBUG_RAM_WRT:
    case SCSI_DIAG_REQUEST_DEBUG_RAM_RED:
        src = scsi_diag_ram_addr_lo.v | scsi_diag_ram_addr_hi.v << 16;
        p = scsi_reply_buf;
        util_mem_copy(p, (void *)src, scsi_diag_reply_len.v);
        break;
    case SCSI_DIAG_REQUEST_ADF_SENSOR:
        if (power_state.f.saving) {
            scsi_reply_buf[0] = scsi_reply_byte_1[0] = 0;
        } else {
            bits = 0;
            if (!mdc_mailbox.sensor_status.f.adf_cover_open)
                bits |= 0x80;
            if (!mdc_mailbox.sensor_status.f.no_paper)
                bits |= 0x40;
            if (mdc_mailbox.sensor_status.f.top)
                bits |= 0x20;
            if (mdc_mailbox.sensor_status_2.f.df)
                bits |= 4;
            if (power_state.f.saving || power_state.f.leave_request)
                bits &= 0x40;
            scsi_reply_buf[0] = bits;
            bits = 0;
            if (!mdc_mailbox.sensor_status.f.document_cover_open)
                bits |= 0x80;
            if (mdc_mailbox.sensor_status.f.pick)
                bits |= 0x40;
            if (power_state.f.saving || power_state.f.leave_request)
                bits = 0;
            scsi_reply_byte_1[0] = bits;
        }
        break;
    case SCSI_DIAG_REQUEST_READ_COUNT:
        p = scsi_reply_buf;
        p[0] = settings_adf_count_low[0];
        p[1] = settings_pick_roller_count.c[0];
        p[2] = settings_pick_roller_count.c[1];
        p[3] = settings_adf_count.c[0];
        p[4] = settings_adf_count.c[1];
        p[5] = settings_flatbed_count_low[0];
        p[6] = settings_flatbed_count.c[0];
        p[7] = settings_flatbed_count.c[1];
        p[8] = settings_brake_roller_count.c[0];
        p[9] = settings_brake_roller_count.c[1];
        p[10] = 0;
        p[11] = 0;
        break;
    case SCSI_DIAG_REQUEST_READ_COUNT2:
        p = scsi_reply_buf;
        main_flatbed_sheets_pending.v = 0;
        count = (unsigned short)(settings_adf_count.c[1] << 8 | settings_adf_count.c[0]) * 500 + settings_adf_count_low[0] * 10 + main_adf_sheets_pending.v;
        p[0] = (unsigned short)(count >> 16) >> 8;
        p[1] = count >> 16;
        p[2] = (unsigned short)count >> 8;
        p[3] = count;
        count = (unsigned short)(settings_flatbed_count.c[1] << 8 | settings_flatbed_count.c[0]) * 500 + settings_flatbed_count_low[0] * 10 + main_flatbed_sheets_pending.v;
        p[4] = (unsigned short)(count >> 16) >> 8;
        p[5] = count >> 16;
        p[6] = (unsigned short)count >> 8;
        p[7] = count;
        p[8] = 0;
        p[9] = 0;
        p[10] = 0;
        p[11] = 0;
        count = (unsigned short)(settings_pick_roller_count.c[1] << 8 | settings_pick_roller_count.c[0]) * 500 + settings_adf_count_low[0] * 10 + main_adf_sheets_pending.v;
        p[12] = (unsigned short)(count >> 16) >> 8;
        p[13] = count >> 16;
        p[14] = (unsigned short)count >> 8;
        p[15] = count;
        count = (unsigned short)(settings_brake_roller_count.c[1] << 8 | settings_brake_roller_count.c[0]) * 500 + settings_adf_count_low[0] * 10 + main_adf_sheets_pending.v;
        p[16] = (unsigned short)(count >> 16) >> 8;
        p[17] = count >> 16;
        p[18] = (unsigned short)count >> 8;
        p[19] = count;
        break;
    case SCSI_DIAG_REQUEST_FIRST_READ_DATE:
        if (scsi_first_read_date_stored == 1)
            scsi_reply_buf[0] = 0;
        else
            scsi_reply_buf[0] = 0xff;
        scsi_first_read_date_stored = 0;
        break;
    case SCSI_DIAG_REQUEST_ROM_INFORMATION:
        util_mem_copy(scsi_reply_buf, link_identity_fi6770, 30);
        util_mem_copy(scsi_reply_byte_23, scsi_revision, 4);
        break;
    case SCSI_DIAG_REQUEST_GET_FIRST_DATE:
        scsi_reply_buf[0] = 0;
        scsi_reply_byte_1[0] = settings_first_read_year[0];
        scsi_reply_byte_2[0] = settings_first_read_month[0];
        scsi_reply_byte_3[0] = settings_first_read_day[0];
        break;
    case SCSI_DIAG_REQUEST_GET_DEVICE_ID:
        if ((settings_device_id_1[0] | settings_device_id_2[0] | settings_device_id_3[0]) == 0) {
            /* no serial number stored: 0x05F5E0FF = 99,999,999 */
            scsi_reply_buf[0] = 5;
            scsi_reply_byte_1[0] = 0xf5;
            scsi_reply_byte_2[0] = 0xe0;
            scsi_reply_byte_3[0] = 0xff;
        } else {
            scsi_reply_buf[0] = 0;
            scsi_reply_byte_1[0] = settings_device_id_1[0];
            scsi_reply_byte_2[0] = settings_device_id_2[0];
            scsi_reply_byte_3[0] = settings_device_id_3[0];
        }
        scsi_reply_byte_4[0] = settings_byte_0x7e[0];
        util_mem_clear(scsi_reply_byte_5, 8);
        break;
    case SCSI_DIAG_REQUEST_SDC_BLOCK_TABLE:
        if (scsi_diag_update_mode == 0x81) {
            flash_build_block_table(scsi_reply_buf);
        } else {
            scsi_status = SCSI_STEP_GOOD;
            return;
        }
        break;
    case SCSI_DIAG_REQUEST_GET_EEPROM_STS:
        alloc_len = 0;
        if (iic_busy.b7 || iic_busy.b6 || mdc_motor_test_state.w)
            alloc_len = 1;
        util_mem_copy(scsi_reply_buf, &alloc_len, 2);
        break;
    case SCSI_DIAG_REQUEST_TEST_LENGTH:
        scsi_reply_buf[0] = scsi_testmode_reply_len.v >> 8;
        scsi_reply_byte_1[0] = scsi_testmode_reply_len.c.lo;
        break;
    case SCSI_DIAG_REQUEST_TESTMODE:
        if (testmode_number.v == 0x15)
            util_mem_copy(scsi_reply_buf, flash_settings_mirror, 0x200);
        else
            util_mem_copy(scsi_reply_buf, err_code_list, scsi_testmode_reply_len.c.lo);
        if (testmode_number.v == 0x25 || testmode_number.v == 0x29) {
            scsi_diag_request = request;
            scsi_diag_reply_len.v = 1;
        } else if (testmode_number.v == 0x27) {
            scsi_diag_request = request;
            scsi_diag_reply_len.v = 2;
        } else if (testmode_number.v == 0x28) {
            if (scsi_testmode_result_state.v == 0) {
                scsi_diag_request = request;
                scsi_diag_reply_len.v = 2;
                scsi_reply_buf[0] = 0x80;
                scsi_reply_byte_1[0] = 0;
            } else if (scsi_testmode_result_state.v == 1) {
                scsi_diag_request = request;
                scsi_diag_reply_len.v = 2;
                scsi_testmode_result_state.v = 2;
                scsi_reply_buf[0] = 0xff;
                scsi_reply_byte_1[0] = 0;
            } else {
                testmode_number.v = 0;
                scsi_testmode_result_state.v = 0;
                testmode_feed_test = 0;
            }
            scsi_diag_reply_len.v = 2;
        } else {
            testmode_number.v = 0;
        }
        power_off_idle_seconds = 0;
        power_off_flags.f.idle_restarted = 1;
        break;
    case SCSI_DIAG_REQUEST_SCSI_ID_GET:
        scsi_reply_buf[0] = hw_read_scsi_id();
        break;
    case SCSI_DIAG_REQUEST_GET_READTIME:
        if (win_sides.f.selection == 2) {
            scsi_reply_buf[0] = (unsigned short)(job_readtime_pages.v * 2 + 0xfffc) >> 8;
            scsi_reply_byte_1[0] = (unsigned short)(job_readtime_pages.v * 2 + 0xfffc);
        } else {
            scsi_reply_buf[0] = (unsigned short)(job_readtime_pages.v + 0xfffe) >> 8;
            scsi_reply_byte_1[0] = (unsigned short)(job_readtime_pages.v + 0xfffe);
        }
        scsi_reply_byte_2[0] = (unsigned short)((job_readtime_ticks.v << 1) >> 16) >> 8;
        scsi_reply_byte_3[0] = (job_readtime_ticks.v << 1) >> 16;
        scsi_reply_byte_4[0] = (unsigned short)(job_readtime_ticks.c.lo << 1) >> 8;
        scsi_reply_byte_5[0] = job_readtime_ticks.c.lo << 1;
        scsi_reply_byte_6[0] = win_params.x_res_dpi.v >> 8;
        scsi_reply_byte_7[0] = win_params.x_res_dpi.b.lo;
        scsi_reply_byte_8[0] = win_params.y_res_dpi.v >> 8;
        scsi_reply_byte_9[0] = win_params.y_res_dpi.b.lo;
        scsi_reply_byte_10[0] = 0;
        if (win_sides.f.selection == 2)
            scsi_reply_byte_10[0] |= 0x80;
        if (win_params.side[0].composition.f.color)
            scsi_reply_byte_10[0] |= 0x40;
        else if (win_params.side[0].composition.f.multi_level)
            scsi_reply_byte_10[0] |= 0x20;
        if (win_params.side[0].compression.f.type & 0x80)
            scsi_reply_byte_10[0] |= 0x10;
        if (job_auto_size_flags.b7)
            scsi_reply_byte_10[0] |= 4;
        if (job_auto_size_flags.b6)
            scsi_reply_byte_10[0] |= 2;
        scsi_reply_byte_11[0] = 0;
        scsi_reply_byte_12[0] = win_params.side[0].width_inch1200 >> 8;
        scsi_reply_byte_13[0] = win_params.side[0].width_inch1200;
        scsi_reply_byte_14[0] = (unsigned short)win_params.side[0].length_inch1200 >> 8;
        scsi_reply_byte_15[0] = win_params.side[0].length_inch1200;
        break;
    case SCSI_DIAG_REQUEST_SETUP_START_T01:
        if (scsi_testmode_result_state.v == 0) {
            scsi_diag_request = request;
            scsi_diag_reply_len.v = 2;
            scsi_reply_buf[0] = 0x80;
            scsi_reply_byte_1[0] = 0;
        } else if (scsi_testmode_result_state.v == 1) {
            scsi_diag_request = request;
            scsi_testmode_result_state.v = 2;
            scsi_reply_buf[0] = 0xff;
            if (err_code_count.v == 0)
                scsi_reply_byte_1[0] = 1;
            else
                scsi_reply_byte_1[0] = err_code_count.v;
            scsi_diag_reply_len.v = 2;
        } else {
            scsi_testmode_result_state.v = 0;
            if (err_code_count.v == 0) {
                scsi_reply_buf[0] = 0;
                scsi_diag_reply_len.v = 1;
            } else {
                util_mem_copy(scsi_reply_buf, err_code_list, err_code_count.v);
                scsi_diag_reply_len.v = err_code_count.v;
            }
        }
        break;
    case SCSI_DIAG_REQUEST_GET_FIRMVERSION:
        util_mem_set(scsi_reply_buf, 0xff, 0x10);
        util_mem_copy(scsi_reply_buf, scsi_revision, 4); /* then 4-7 the MDC version, 8-11 the imprinter version */
        if (power_state.f.saving) {
            scsi_reply_byte_4[0] = power_image_mdc_version_0[0].b[1];
            scsi_reply_byte_5[0] = power_image_mdc_version_1[0].w >> 8;
            scsi_reply_byte_6[0] = power_image_mdc_version_1[0].b[1];
            scsi_reply_byte_7[0] = power_image_mdc_version_2[0].w >> 8;
            if (hw_option_flags.f.b0) {
                scsi_reply_byte_8[0] = power_image_imprinter_version_0[0].w >> 8;
                scsi_reply_byte_9[0] = power_image_imprinter_version_0[0].b[1];
                scsi_reply_byte_10[0] = power_image_imprinter_version_1[0].w >> 8;
                scsi_reply_byte_11[0] = power_image_imprinter_version_1[0].b[1];
            } else {
                scsi_reply_byte_8[0] = 0xff;
                scsi_reply_byte_9[0] = 0xff;
                scsi_reply_byte_10[0] = 0xff;
                scsi_reply_byte_11[0] = 0xff;
            }
        } else {
            scsi_reply_byte_4[0] = mdc_version[0];
            scsi_reply_byte_5[0] = mdc_version_1[0];
            scsi_reply_byte_6[0] = mdc_version_2[0];
            scsi_reply_byte_7[0] = mdc_version_3[0];
            if (hw_option_flags.f.b0) {
                scsi_reply_byte_8[0] = mdc_imprinter_version[0];
                scsi_reply_byte_9[0] = mdc_imprinter_version_1[0];
                scsi_reply_byte_10[0] = mdc_imprinter_version_2[0];
                scsi_reply_byte_11[0] = mdc_imprinter_version_3[0];
            }
        }
        break;
    case SCSI_DIAG_REQUEST_SETUP_IMPRINTERR:
        if (scsi_imprinter_error != 0)
            scsi_reply_buf[0] = 0x80;
        else
            scsi_reply_buf[0] = 0;
        if (settings_byte_0x23[0] & 0x40)
            scsi_reply_byte_1[0] = 8;
        else
            scsi_reply_byte_1[0] = 5;
        scsi_reply_byte_2[0] = scsi_imprinter_error_byte_1[0];
        scsi_reply_byte_3[0] = scsi_imprinter_error_byte_2[0];
        scsi_reply_byte_4[0] = scsi_imprinter_error_byte_3[0];
        scsi_reply_byte_5[0] = scsi_imprinter_error_byte_4[0];
        break;
    case SCSI_DIAG_REQUEST_DEFECT_LISTS:
        scsi_defect_transfer.defect_area = 1;
        break;
    case SCSI_DIAG_REQUEST_ERASE_FLASH_WORK:
        scsi_reply_buf[0] = 0;
        scsi_reply_byte_1[0] = flash_status.c.lo;
        break;
    case SCSI_DIAG_REQUEST_MDC_BLOCK_READ:
        request = flash_download_targets[flash_download_session & 3].state;
        switch (request) {
        case 8:
            break;
        case 1:
            flash_mdc_build_block_table(scsi_reply_buf, flash_download_session & 3);
            break;
        case 9:
        default:
            util_mem_copy(scsi_reply_buf, flash_download_targets[flash_download_session & 3].mdc_firmware_version, 4);
            break;
        }
        break;
    case SCSI_DIAG_REQUEST_GET_SOP_DEFAULT:
        util_mem_copy(scsi_reply_buf, settings, 0x200);
        scsi_reply_byte_208[0] = 8;
        scsi_reply_byte_214[0] = 0;
        scsi_reply_byte_215[0] = 0;
        scsi_reply_byte_216[0] = 0;
        scsi_reply_byte_217[0] = 0;
        scsi_reply_byte_211[0] = 0;
        scsi_reply_byte_210[0] = 0;
        scsi_reply_byte_192[0] = 0;
        scsi_reply_byte_187[0] = 0;
        scsi_reply_byte_190[0] = 0;
        scsi_reply_byte_203[0] = 0;
        scsi_reply_byte_183[0] = 0;
        scsi_reply_byte_48[0] = 0;
        scsi_reply_byte_49[0] = 0;
        scsi_reply_byte_198[0] = 0;
        scsi_reply_byte_199[0] = 0;
        scsi_reply_byte_182[0] = 0;
        scsi_reply_byte_207[0] = 0;
        scsi_reply_byte_128[0] = 0;
        scsi_reply_byte_177[0] = 0;
        scsi_reply_byte_178[0] = 0;
        scsi_reply_byte_129[0] = 0;
        scsi_reply_byte_41[0] = 0;
        scsi_reply_byte_180[0] = 0;
        scsi_reply_byte_118[0] = 0;
        scsi_reply_byte_197[0] = 0;
        break;
    case SCSI_DIAG_REQUEST_GET_POWERSWMODE:
        scsi_reply_buf[0] = 0;
        if (hw_power_switch_enabled())
            scsi_reply_byte_1[0] = 1;
        else
            scsi_reply_byte_1[0] = 0;
        break;
    case SCSI_DIAG_REQUEST_GET_SOP_SET_LEN:
        scsi_reply_buf[0] = 0;
        scsi_reply_byte_1[0] = 0xa0;
        scsi_reply_byte_2[0] = 0;
        scsi_reply_byte_3[0] = 0;
        break;
    case SCSI_DIAG_REQUEST_GET_SOP_SET_DAT:
        scsi_build_sop_items();
        break;
    case SCSI_DIAG_REQUEST_PUT_SOP_SET_DAT:
        if (scsi_sop_put_valid)
            scsi_reply_buf[0] = 0;
        else
            scsi_reply_buf[0] = 0x80;
        scsi_reply_byte_1[0] = 0;
        scsi_reply_byte_2[0] = 0;
        break;
    case SCSI_DIAG_REQUEST_GET_DEV_LOG_LEN:
        if ((settings_emulation[0] & 0x1c) == 0x1c || (settings_emulation[0] & 0x1c) == 0x18
            || (settings_emulation[0] & 0x1c) == 0x10 || (settings_emulation[0] & 0x1c) == 0xc)
            goto dflt;
        scsi_reply_buf[0] = 4;
        scsi_reply_byte_1[0] = 0;
        scsi_reply_byte_2[0] = 0;
        scsi_reply_byte_3[0] = 0;
        break;
    case SCSI_DIAG_REQUEST_GET_DEV_LOG_DAT:
        if ((settings_log_source[0] & 0xf) == 1 && hw_option_flags.f.b0)
            scsi_copy_mdc_log(scsi_dev_log_selector, scsi_dev_log_offset, scsi_diag_data_len.v);
        else
            scsi_copy_device_log(scsi_dev_log_offset, scsi_diag_data_len.v);
        break;
    case SCSI_DIAG_REQUEST_DEBUG_E2P_WRT_X:
        scsi_reply_buf[0] = 0;
        scsi_reply_byte_1[0] = 0;
        scsi_reply_byte_2[0] = 0;
        scsi_reply_byte_3[0] = 0;
        break;
    case SCSI_DIAG_REQUEST_DEBUG_E2T_WRT_X:
    case SCSI_DIAG_REQUEST_DEBUG_E2T_RED_X:
        util_mem_copy(scsi_reply_buf, iic_eeprom_area2 + scsi_dev_log_offset, scsi_dev_log_chunk_size);
        break;
    case SCSI_DIAG_REQUEST_SET_POWOFF_TIME:
        if (1)
            scsi_reply_buf[0] = 0;
        else
            scsi_reply_buf[0] = 1;
        scsi_reply_byte_1[0] = 0;
        break;
    case SCSI_DIAG_REQUEST_GET_POWOFF_TIME:
        scsi_reply_buf[0] = settings_power_off_time[0];
        scsi_reply_byte_1[0] = settings_power_off_warning[0];
        break;
    default:
    dflt:
        if (hw_hostif_flags.f.usb)
            usb_send_reply(scsi_diag_reply_len.v);
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    scsi_param_len.v = scsi_diag_reply_len.v;
    scsi_send_reply();
    (*(union UBF *)&scsi_defect_transfer).v &= 0x7f;
    return;
    }
}

signed char scsi_sop_item_mask(unsigned short offset)
{
    signed char mask;
    unsigned short item_offset;
    unsigned char x;
    unsigned char y;
    unsigned char i;

    mask = 0;
    item_offset = 0;
    x = 0;
    y = 0;
    i = 0;
    for (i = 0; i < 84; i += 3) {
        item_offset = scsi_sop_item_masks[i];
        item_offset = item_offset << 8;
        item_offset += (&scsi_sop_item_masks[i])[1];
        if (item_offset == offset) {
            mask = (&scsi_sop_item_masks[i])[2];
            break;
        }
        item_offset = 0;
    }
    return mask;
}

void scsi_build_sop_items(void)
{
    unsigned short i;
    unsigned short j;
    unsigned short offset;

    scsi_reply_buf[0] = 70;
    scsi_reply_byte_1[0] = 85;
    scsi_reply_byte_2[0] = 74;
    scsi_reply_byte_3[0] = 73;
    scsi_reply_byte_4[0] = 84;
    scsi_reply_byte_5[0] = 83;
    scsi_reply_byte_6[0] = 85;
    scsi_reply_byte_7[0] = 32;
    scsi_reply_byte_8[0] = 102;
    scsi_reply_byte_9[0] = 105;
    scsi_reply_byte_10[0] = 45;
    scsi_reply_byte_11[0] = 54;
    scsi_reply_byte_12[0] = 55;
    scsi_reply_byte_13[0] = 55;
    scsi_reply_byte_14[0] = 48;
    scsi_reply_byte_15[0] = 100;
    scsi_reply_byte_16[0] = 106;
    scsi_reply_byte_17[0] = 32;
    scsi_reply_byte_18[0] = 32;
    scsi_reply_byte_19[0] = 32;
    scsi_reply_byte_20[0] = 32;
    scsi_reply_byte_21[0] = 32;
    scsi_reply_byte_22[0] = 32;
    scsi_reply_byte_23[0] = 32;
    util_mem_copy(scsi_reply_byte_24, scsi_revision, 4);
    scsi_reply_byte_28[0] = 0;
    scsi_reply_byte_29[0] = 0;
    scsi_reply_byte_30[0] = 0;
    scsi_reply_byte_31[0] = 0;
    scsi_reply_byte_32[0] = 0;
    scsi_reply_byte_33[0] = 0;
    ((unsigned char *)scsi_reply_byte_34)[0] = 0;
    ((unsigned char *)scsi_reply_byte_34)[1] = 0;
    scsi_reply_byte_36[0] = 0;
    scsi_reply_byte_37[0] = 0;
    scsi_reply_byte_38[0] = 0;
    scsi_reply_byte_39[0] = 0;
    scsi_reply_byte_40[0] = 0;
    scsi_reply_byte_41[0] = 0;
    scsi_reply_byte_42[0] = 0;
    scsi_reply_byte_43[0] = 0;
    scsi_reply_byte_44[0] = 0;
    scsi_reply_byte_45[0] = 0;
    scsi_reply_byte_46[0] = 0;
    scsi_reply_byte_47[0] = 28;
    j = 0x30;
    for (i = 0; i < 84; i += 3, j += 4, offset = 0) {
        scsi_reply_buf[j] = scsi_sop_item_masks[i];
        offset = scsi_sop_item_masks[i];
        offset = offset << 8;
        scsi_reply_byte_1[j] = (&scsi_sop_item_masks[i])[1];
        offset += (&scsi_sop_item_masks[i])[1];
        scsi_reply_byte_2[j] = (&scsi_sop_item_masks[i])[2];
        scsi_reply_byte_3[j] = settings[offset];
    }
    return;
}

extern void _builtin_set_imask(int);

void scsi_copy_mdc_log(unsigned short selector, unsigned short offset, unsigned short len)
{
    unsigned short u16;
    char *p;
    int i8;
    unsigned short index;
    unsigned short end;
    unsigned short window_offset;
    unsigned short u0;

    if (selector == 1) {
        if (offset + len < 0x200) {
        } else {
            if (offset >= 0x200) {
                p = (char *)scsi_reply_buf;
                util_mem_clear(p, 0x200);
                return;
            } else {
                len = 0x200 - offset;
                if (len == 0) {
                    offset = 0;
                    len = 0x200;
                }
            }
        }
        window_offset = offset * 2;
        p = (char *)scsi_reply_buf;
        util_mem_copy_from_odd((int)p, (int)&mdc_back_window[0x200 + window_offset] + 1, len);
        _builtin_set_imask(15);
        index = window_offset + 0x200;
        if (index < 0x200) {
            index = 0x200;
        }
        end = window_offset + len * 2 + 0x200;
        if (end > 0x600) {
            end = 0x600;
        }
        for (; index < end; index++) {
            mdc_back_window[index] = 0;
        }
        scsi_imprinter_slot = 0x201;
        _builtin_set_imask(scsi_saved_imask);
    }
    return;
}

void scsi_copy_device_log(unsigned short offset, unsigned short len)
{
    char *dst;
    char *log_base;

    if (offset >= 0x400) {
        return;
    } else {
        if (offset + len > 0x400) {
            len = 0x400 - offset;
        }
    }
    dst = (char *)scsi_reply_buf;
    log_base = (char *)&log_start_day;
    util_mem_copy(dst, log_base + offset, len);
    if (offset == 0) {
        log_clear_records();
    }
    return;
}

void scsi_cmd_scan(void)
{
    struct page_record *current_page;
    int x;
    int window_count;
    int window_ok;
    int start_read;
    unsigned char window_id;
    unsigned char second_window_id;
    unsigned char ok;
    int saved_page_flags;
    int saved_back_flags;
    int saved_front_flags;
    unsigned char saved_sides;

    current_page = &page_records[page_current.v];
    log_add_record(2, 27, 0, 0);
    ok = scsi_check_ready();
    if (!ok && !hw_hostif_flags.f.usb) {
        return;
    }
    window_id = scsi_cdb_byte_2[0] | scsi_cdb_byte_3[0] | scsi_cdb_byte_5[0];
    if (scsi_cdb_byte_1[0] & ~0xe0 || window_id) {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb) {
            return;
        }
        ok = 0;
    }
    scsi_param_len.v = scsi_cdb_byte_4[0];
    if (!scsi_param_len.v) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (!scsi_receive_params() || !ok) {
        return;
    }
    window_id = scsi_param_buf[0];
    second_window_id = scsi_param_byte_1[0];
    window_count = scsi_param_len.v;
    _builtin_set_imask(15);
    scsi_page_read_done = 0;
    err_multifeed_reported = 0;
    start_read = 1;
    if (current_page->sides[0].flags.f.requested || current_page->sides[1].flags.f.requested) {
        _builtin_set_imask(15);
        if (win_params.flags.f.adf) {
            saved_page_flags = current_page->flags.v;
            saved_front_flags = current_page->sides[0].flags.v;
            saved_back_flags = current_page->sides[1].flags.v;
            scsi_clear_read_state();
            current_page->flags.v = saved_page_flags;
            current_page->sides[0].flags.v = saved_front_flags;
            current_page->sides[1].flags.v = saved_back_flags;
        } else {
            scsi_clear_read_state();
        }
        start_read = 0;
    }
    _builtin_set_imask(15);
    scsi_read_progress.v = 0;
    _builtin_set_imask(scsi_saved_imask);
    saved_sides = win_sides.v;
    window_ok = 0;
    if (window_count == 1) {
        if (window_id == 0 && scsi_win_saved_descriptors[0] == 0x80) {
            win_sides.v = 0;
            scsi_read_side.v = 0;
            window_ok = 1;
        } else if (window_id == 0x80 && scsi_win_saved_back_descriptor[0] == 0x80) {
            win_sides.v = 1;
            scsi_read_side.v = 1;
            window_ok = 1;
        }
    } else if (window_count == 2) {
        if (window_id == 0 && second_window_id == 0x80) {
            win_sides.v = 2;
            if (scsi_win_saved_descriptors[0] == 0x80 && scsi_win_saved_back_descriptor[0] == 0x80) {
                scsi_read_side.v = 0;
                window_ok = 1;
            }
        }
    } else {
    }
    if (!window_ok) {
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 0);
        return;
    }
    if (job_buffer_feed_flags & 0x40) {
        job_buffer_feed_flags = 0;
        if (saved_sides != win_sides.v) {
            if (saved_sides == 2 && win_sides.v == 0) {
                _builtin_set_imask(15);
                current_page->sides[1].flags.f.scanned = 1;
                page_side_state[1].buf[0].flags &= 0x7f;
                edge_side_records[1].e[0].flags |= EDGE_FLAG_SIZE_FINAL;
                _builtin_set_imask(scsi_saved_imask);
            } else if (saved_sides == 2 && win_sides.v == 1) {
                _builtin_set_imask(15);
                current_page->sides[0].flags.f.scanned = 1;
                page_side_state[0].buf[0].flags &= 0x7f;
                edge_side_records[0].e[0].flags |= EDGE_FLAG_SIZE_FINAL;
                _builtin_set_imask(scsi_saved_imask);
            } else {
                scsi_sense_invalid_param(SCSI_SENSE_SKSV, 0);
                return;
            }
        }
    }
    scsi_param_len.v = 0;
    scsi_read_sides[0].flags.w = 0;
    scsi_read_sides[1].flags.w = 0;
    scsi_scan_requested = 1;
    if (window_id == 0x80) {
        scsi_read_window_id.v = 1;
    } else {
        scsi_read_window_id.v = 0;
    }
    scsi_last_page_size[0].size_final = 0;
    scsi_last_page_size[1].size_final = 0;
    scsi_last_page_size[0].length_lines = 0;
    scsi_last_page_size[1].length_lines = 0;
    scsi_last_page_size[0].width_px = 0;
    scsi_last_page_size[1].width_px = 0;
    if (!start_read) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (power_state.f.saving) {
        scsi_status = SCSI_STEP_GOOD;
    } else {
        scsi_read_start();
    }
}


void scsi_cmd_mode_sense(void)
{
    unsigned char value;
    unsigned char value_2;
    unsigned char page_control;
    unsigned char page_code;
    char *p;
    unsigned short word;
    unsigned char value_3;
    unsigned char value_4;
    if (!scsi_check_ready_2()) {
        if (hw_hostif_flags.f.usb)
            usb_send_reply(scsi_cdb_byte_4[0]);
        return;
    }
    if ((scsi_cdb_byte_1[0] & 0x17) || scsi_cdb_byte_3[0] || scsi_cdb_byte_5[0]) {
        if (hw_hostif_flags.f.usb)
            usb_send_reply(scsi_cdb_byte_4[0]);
        scsi_sense_invalid_cdb();
        return;
    }
    value = scsi_cdb_byte_2[0];
    page_code = value;
    page_code &= 0x3f;
    if (!(page_code == SCSI_PAGE_DISCONNECT_RECONNECT || page_code == SCSI_PAGE_MEASUREMENT_UNITS || page_code == SCSI_PAGE_SLEEP || page_code == SCSI_PAGE_INTERLACE_READ || page_code == SCSI_PAGE_DEVICE_CONTROLS || page_code == SCSI_PAGE_BACKING || page_code == SCSI_PAGE_DOUBLE_FEED || page_code == SCSI_PAGE_AUTO_SIZE || page_code == SCSI_PAGE_LAMP_TIMER || page_code == SCSI_PAGE_DRIVER_FUNCTION_CONFIRM || page_code == SCSI_PAGE_PREPICK || page_code == SCSI_PAGE_BUFFER || page_code == SCSI_PAGE_JOB_SEPARATION || page_code == SCSI_PAGE_DROPOUT || page_code == SCSI_PAGE_ALL)) {
        if (hw_hostif_flags.f.usb)
            usb_send_reply(scsi_cdb_byte_4[0]);
        scsi_sense_invalid_cdb();
        return;
    }
    page_control = value & 0xc0;
    scsi_param_len.v = scsi_cdb_byte_4[0];
    if (scsi_param_len.v == 0) {
        if (hw_hostif_flags.f.usb)
            usb_start_data_in();
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    p = (char *)scsi_reply_buf;
    util_mem_clear(p, 12);
    p += 4;
    if (!(scsi_cdb_byte_1[0] & 8)) {
        p[-1] = 8;
        p += 8;
        p[-1] = 1;
    }
    if (page_code == SCSI_PAGE_DISCONNECT_RECONNECT || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_DISCONNECT_RECONNECT;
        p[1] = 14;
        util_mem_clear(p + 2, 13);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            word = job_mode_params[0].max_burst_size;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            word = 0xffff;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            word = 0;
            break;
        default:
            word = 0;
            break;
        }
        util_put_be16(p + 10, word);
        p += 16;
    }
    if (page_code == SCSI_PAGE_MEASUREMENT_UNITS || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_MEASUREMENT_UNITS;
        p[1] = 6;
        util_mem_clear(p + 2, 6);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].measurement_unit;
            word = job_mode_params[0].unit_divisor;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 3;
            word = 0xffff;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            word = 0x4b0;
            break;
        default:
            value = 0;
            word = 0;
            break;
        }
        p[2] = value;
        util_put_be16(p + 4, word);
        p += 8;
    }
    if (page_code == SCSI_PAGE_DRIVER_FUNCTION_CONFIRM || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_DRIVER_FUNCTION_CONFIRM;
        p[1] = 6;
        util_mem_clear(p + 2, 6);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].driver_function_confirm.b[0];
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 1;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            break;
        default:
            value = 0;
            break;
        }
        p[2] = value;
        p += 8;
    }
    if (page_code == SCSI_PAGE_PREPICK || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_PREPICK;
        p[1] = 6;
        util_mem_clear(p + 2, 6);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].prepick;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 0xc0;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            break;
        default:
            value = 0;
            break;
        }
        p[2] = value;
        p += 8;
    }
    if (page_code == SCSI_PAGE_SLEEP || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_SLEEP;
        p[1] = 6;
        util_mem_clear(p + 2, 6);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].sleep_timer;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 0xff;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            break;
        default:
            value = settings_sleep_timer_default[0];
            break;
        }
        p[2] = value;
        p += 8;
    }
    if (page_code == SCSI_PAGE_INTERLACE_READ || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_INTERLACE_READ;
        p[1] = 6;
        util_mem_clear(p + 2, 6);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].interlace_read;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 1;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            break;
        default:
            value = 0;
            break;
        }
        p[2] = value;
        p += 8;
    }
    if (page_code == SCSI_PAGE_DEVICE_CONTROLS || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_DEVICE_CONTROLS;
        p[1] = 6;
        util_mem_clear(p + 2, 6);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].device_controls;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 0xff;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            break;
        default:
            value = 0;
            break;
        }
        p[2] = value;
        p += 8;
    }
    if (page_code == SCSI_PAGE_BACKING || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_BACKING;
        p[1] = 6;
        util_mem_clear(p + 2, 6);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].backing;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 0xff;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            break;
        default:
            value = settings_mode_page_defaults[0];
            break;
        }
        p[2] = value;
        p += 8;
    }
    if (page_code == SCSI_PAGE_DOUBLE_FEED || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_DOUBLE_FEED;
        p[1] = 6;
        util_mem_clear(p + 2, 6);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].double_feed;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 0xff;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            break;
        default:
            value = 0;
            break;
        }
        p[2] = value;
        p += 8;
    }
    if (page_code == SCSI_PAGE_DROPOUT || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_DROPOUT;
        p[1] = 10;
        util_mem_clear(p + 2, 8);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].dropout_color;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 0xff;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            break;
        default:
            value = 0;
            break;
        }
        p[2] = value;
        for (word = 0; word < 6; word++) {
            switch (page_control) {
            case SCSI_PAGE_CONTROL_CURRENT:
                value = job_mode_params[0].dropout_levels[word];
                break;
            case SCSI_PAGE_CONTROL_CHANGEABLE:
                value = 0xff;
                break;
            case SCSI_PAGE_CONTROL_DEFAULT:
                value = 0;
                break;
            default:
                value = 0;
                break;
            }
            p[word + 4] = value;
        }
        p += 10;
    }
    if (page_code == SCSI_PAGE_BUFFER || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_BUFFER;
        p[1] = 6;
        util_mem_clear(p + 2, 6);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].buffer_mode[0];
            value_2 = job_mode_params[0].buffer_clear;
            word = job_mode_params[0].page_count_limit;
            value_3 = job_mode_params[0].buffer_byte_6;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 0xff;
            value_2 = 0xff;
            word = 0xffff;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            value_2 = 0;
            word = 0;
            break;
        default:
            value = 0;
            value_2 = 0;
            word = 0;
            break;
        }
        p[2] = value;
        p[3] = value_2;
        util_put_be16(p + 4, word);
        p[6] = value_3;
        p += 8;
    }
    if (page_code == SCSI_PAGE_AUTO_SIZE || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_AUTO_SIZE;
        p[1] = 6;
        util_mem_clear(p + 2, 5);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].auto_length;
            value_2 = job_mode_params[0].auto_width;
            value_3 = job_mode_params[0].deskew;
            value_4 = job_mode_params[0].overscan;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 0xc0;
            value_2 = 0xc0;
            value_3 = 0x80;
            value_4 = 0xc0;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            value_2 = 0;
            value_3 = 0;
            value_4 = 0;
            break;
        default:
            value = 0;
            value_2 = 0;
            value_3 = 0;
            value_4 = 0;
            break;
        }
        p[2] = value_2;
        p[4] = value_3;
        p[5] = value_4;
        p[3] = value;
        p += 8;
    }
    if (page_code == SCSI_PAGE_LAMP_TIMER || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_LAMP_TIMER;
        p[1] = 6;
        util_mem_clear(p + 2, 5);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].lamp_off_minutes;
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 0xff;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            break;
        default:
            value = settings_lamp_timer_default[0];
            break;
        }
        p[2] = value;
        p += 8;
    }
    if (page_code == SCSI_PAGE_JOB_SEPARATION || page_code == SCSI_PAGE_ALL) {
        p[0] = SCSI_PAGE_JOB_SEPARATION;
        p[1] = 6;
        util_mem_clear(p + 2, 5);
        switch (page_control) {
        case SCSI_PAGE_CONTROL_CURRENT:
            value = job_mode_params[0].job_separation[0];
            break;
        case SCSI_PAGE_CONTROL_CHANGEABLE:
            value = 0x80;
            break;
        case SCSI_PAGE_CONTROL_DEFAULT:
            value = 0;
            break;
        default:
            value = 0;
            break;
        }
        p[2] = value;
        p += 8;
    }
    word = (unsigned char *)p - scsi_reply_buf;
    ((unsigned char *)scsi_reply_buf)[0] = word + 0xff;
    if (scsi_param_len.v > word && !hw_hostif_flags.f.usb)
        scsi_param_len.v = word;
    scsi_send_reply();
}

extern void scsi_sense_invalid_param_at();

void scsi_cmd_mode_select(void)
{
    struct page_record *current_page;
    unsigned char *p;
    unsigned char *page;
    unsigned int offset;
    unsigned int remaining;
    int skip_len;
    int flag;
    int j;
    unsigned char value;
    unsigned char b10;
    unsigned char value_3;
    unsigned char page_code;
    unsigned short count;
    unsigned short pages_seen;
    unsigned char value_2;
    unsigned char ok;
    current_page = &page_records[page_current.v];
    ok = scsi_check_ready_2();
    if (ok == 0 && !hw_hostif_flags.f.usb)
        return;
    if ((scsi_cdb_byte_1[0] & 14) || scsi_cdb_byte_2[0] || scsi_cdb_byte_3[0] || scsi_cdb_byte_5[0]) {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb)
            return;
        ok = 0;
    }
    if (!(scsi_cdb_byte_1[0] & 16)) {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb)
            return;
        ok = 0;
    }
    scsi_param_len.v = scsi_cdb_byte_4[0];
    if (scsi_param_len.v == 0) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (scsi_param_len.v < 4 && !hw_hostif_flags.f.usb) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (!scsi_receive_params() || !ok)
        return;
    pages_seen = 0;
    p = scsi_param_buf;
    value = *p;
    p++;
    value = *p;
    p++;
    if (value != 0) {
        scsi_sense_invalid_param_at(p);
        return;
    }
    value = *p;
    p++;
    if (value != 0) {
        scsi_sense_invalid_param_at(p);
        return;
    }
    value = *p;
    p++;
    if (value & 7) {
        scsi_sense_invalid_param_at(p);
        return;
    }
    value >>= 3;
    count = value;
    if (count != 0) {
        flag = 1;
        while (count > 0) {
            offset = p - scsi_param_buf + 8;
            if (scsi_param_len.v < offset) {
                scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return;
            }
            value = *p;
            p++;
            if (value != 0) {
                flag = 0;
                break;
            }
            value = *p;
            p++;
            value |= *p;
            value |= p[1];
            if (value != 0) {
                flag = 0;
                break;
            }
            p += 2;
            value = *p;
            p++;
            if (value != 0) {
                flag = 0;
                break;
            }
            value = *p;
            p++;
            value |= *p;
            if (value != 0) {
                flag = 0;
                break;
            }
            value = p[1];
            if (value != 1) {
                flag = 0;
                break;
            }
            p += 2;
            count--;
        }
        if (!flag) {
            scsi_sense_invalid_param_at(p);
            return;
        }
    } else {
    }
    page = p;
    count = p - scsi_param_buf;
    remaining = scsi_param_len.v;
    flag = 1;
    while (1) {
        if (remaining == count)
            break;
        if (remaining < count + 2) {
            flag = 0;
            break;
        }
        remaining -= count;
        count = page[1] + 2;
        if (count >= 0x100) {
            flag = 0;
            break;
        }
        page = count + page;
    }
    if (!flag) {
        scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_PARAMETER_LIST_LENGTH);
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return;
    }
    flag = 0;
    skip_len = 0;
    for (;;) {
        offset = p - scsi_param_buf;
        if (scsi_param_len.v <= offset) {
            flag = 1;
            break;
        }
        page_code = *p;
        p++;
        if (page_code == SCSI_PAGE_DISCONNECT_RECONNECT) {
            value = *p;
            p++;
            if (value != 14)
                break;
            if (util_mem_all_equal(p, 0, 2, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            for (j = 0; j < 3; j++) {
                value = *p;
                p++;
                value |= p[1];
                if (value)
                    break;
                p++;
            }
            count = util_get_be16(p);
            p++;
            p++;
            job_mode_params[1].max_burst_size = count;
            if (util_mem_all_equal(p, 0, 4, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 0x8000;
        } else if (page_code == SCSI_PAGE_MEASUREMENT_UNITS) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            if (value > 3)
                break;
            else
                job_mode_params[1].measurement_unit = value;
            value = *p;
            p++;
            if (value)
                break;
            count = util_get_be16(p);
            p++;
            if (count == 0)
                break;
            p++;
            job_mode_params[1].unit_divisor = count;
            if (util_mem_all_equal(p, 0, 2, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 0x4000;
        } else if (page_code == SCSI_PAGE_DRIVER_FUNCTION_CONFIRM) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            job_set_driver_function_confirm(value, 1);
            if (util_mem_all_equal(p, 0, 5, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            job_mode_params[0].driver_function_confirm.b[0] = job_mode_params[1].driver_function_confirm.b[0];
        } else if (page_code == SCSI_PAGE_AUTO_COLOR) {
            /* the second test is never true: a value masked with 28 is never 17 */
            if ((settings_emulation[0] & 28) == 20 || (settings_emulation[0] & 28) == 17)
                flag = 1;
            break;
        } else if (page_code == SCSI_PAGE_PREPICK) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            job_set_prepick(value, 1);
            value = *p;
            p++;
            if (util_mem_all_equal(p, 0, 4, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 4;
        } else if (page_code == SCSI_PAGE_SLEEP) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            if (*p == 0x80) {
                if (current_page->flags.v & PAGE_FLAGS_ACTIVE) {
                } else if (power_state.f.saving) {
                    power_state.f.leave_request = 1;
                }
            } else {
                job_set_sleep_timer(value, 1);
            }
            p++;
            if (util_mem_all_equal(p, 0, 4, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 0x20;
        } else if (page_code == SCSI_PAGE_INTERLACE_READ) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            job_set_interlace_read(value, 1);
            /* never true (a value masked with 28 is never 17): the then branch is unreachable as compiled */
            if ((settings_emulation[0] & 28) == 17) {
                flag = 1;
            } else {
                if (util_mem_all_equal(p, 0, 5, &skip_len)) {
                    p += skip_len;
                } else {
                    break;
                }
            }
            pages_seen |= 0x400;
        } else if (page_code == SCSI_PAGE_DEVICE_CONTROLS) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            job_set_device_controls(value, 1);
            if (util_mem_all_equal(p, 0, 5, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 8;
        } else if (page_code == SCSI_PAGE_BACKING) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            job_set_backing(value, 1);
            if (util_mem_all_equal(p, 0, 5, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 0x100;
        } else if (page_code == SCSI_PAGE_DOUBLE_FEED) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            job_set_double_feed(value, 1);
            if (util_mem_all_equal(p, 0, 5, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 0x80;
        } else if (page_code == SCSI_PAGE_DROPOUT) {
            value = *p;
            p++;
            if (value != 8)
                break;
            value = *p;
            p++;
            job_set_dropout_color(value, 1);
            if ((settings_emulation[0] & 28) == 16 || (settings_emulation[0] & 28) == 28 || (settings_emulation[0] & 28) == 24) {
                if (*p)
                    break;
                p++;
                for (count = 0; count < 6; count++) {
                    value = *p;
                    p++;
                    job_set_dropout_level(value, count, 1);
                }
            } else {
                if (*p)
                    break;
                for (count = 0; count < 3; count++) {
                    if (job_dropout[count].w == 4) {
                        if (1) ;
                        ((int *)job_dropout + 2)[count * 3] = 3;
                    }
                }
                p += 7;
            }
            if (job_dropout[1].w != job_dropout_previous[1].w)
                job_adf_sheet_count = 0;
            if (job_dropout[2].w != job_dropout_previous[2].w)
                job_adf_sheet_count = 0;
            job_dropout_previous[1].w = job_dropout[1].w;
            job_dropout_previous[2].w = job_dropout[2].w;
            pages_seen |= 0x10;
        } else if (page_code == SCSI_PAGE_BUFFER) {
            log_add_record(2, 21, 58, 0);
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            value_2 = *p;
            p++;
            count = (p[0] << 8) | p[1];
            p++;
            p++;
            if (count > 0x7fff)
                break;
            job_pause_requested = 0;
            job_pause_page_count.v = 0;
            job_set_buffer_mode(value, value_2, count, 1);
            value_3 = *p;
            p++;
            job_mode_params[1].buffer_byte_6 = value_3;
            _builtin_set_imask(15);
            if (value_3 & 0x80)
                job_buffer_option = 0x8000;
            else
                job_buffer_option = 0;
            _builtin_set_imask(scsi_saved_imask);
            if (util_mem_all_equal(p, 0, 1, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 0x40;
        } else if (page_code == SCSI_PAGE_AUTO_SIZE) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            value_2 = value;
            p++;
            if (value_2 & 62)
                break;
            else if (value_2 & 0x80)
                job_auto_size_flags.b3 = 0;
            else
                job_auto_size_flags.b3 = 1;
            value = *p;
            p++;
            if (value & 63)
                break;
            else
                job_set_auto_length(value, 1);
            value = *p;
            p++;
            if (value & 127)
                break;
            else if (value & 0x80) {
                if (*p == 0xc0) {
                    if ((settings_emulation[0] & 28) != 17)
                        break;
                }
            }
            job_set_deskew_and_auto_width(value, value_2, 1);
            value = *p;
            p++;
            if (value & 31)
                break;
            /* never true (a value masked with 28 is never 17): unreachable as compiled */
            if ((settings_emulation[0] & 28) == 17 && job_auto_size_flags.b7)
                value &= 63;
            job_set_overscan(value, 1);
            if (util_mem_all_equal(p, 0, 2, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 0x800;
        } else if (page_code == SCSI_PAGE_LAMP_TIMER) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            job_mode_params[1].lamp_off_minutes = value;
            if (util_mem_all_equal(p, 0, 5, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 0x2000;
        } else if (page_code == SCSI_PAGE_JOB_SEPARATION) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            job_mode_params[1].job_separation[0] = value;
            if (value & 127)
                break;
            if (util_mem_all_equal(p, 0, 5, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
            pages_seen |= 0x1000;
        } else if (page_code == SCSI_PAGE_POWER_CONTROL) {
            value = *p;
            p++;
            if (value != 6)
                break;
            value = *p;
            p++;
            if (value & 254)
                break;
            job_set_auto_power_off(value); /* page 0x2C: 0 start scanning, 1 end scanning */
            if (util_mem_all_equal(p, 0, 5, &skip_len)) {
                p += skip_len;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    if (!flag) {
        scsi_sense_invalid_param_at(skip_len + (int)p);
        return;
    }
    value = scsi_cdb_byte_1[0];
    value &= 1;
    b10 = 0;
    value = 0;
    if (pages_seen & 0xffff)
        value |= 0x40;
    if (pages_seen & 0x8000)
        job_mode_params[0].max_burst_size = job_mode_params[1].max_burst_size;
    if (pages_seen & 0x4000) {
        job_mode_params[0].measurement_unit = job_mode_params[1].measurement_unit;
        job_mode_params[0].unit_divisor = 0x4b0;
    }
    if (pages_seen & 0x20)
        job_mode_params[0].sleep_timer = job_mode_params[1].sleep_timer;
    if (pages_seen & 0x400)
        job_mode_params[0].interlace_read = job_mode_params[1].interlace_read;
    if (pages_seen & 8)
        job_mode_params[0].device_controls = job_mode_params[1].device_controls;
    if (pages_seen & 0x100)
        job_mode_params[0].backing = job_mode_params[1].backing;
    if (pages_seen & 0x80)
        job_mode_params[0].double_feed = job_mode_params[1].double_feed;
    if (pages_seen & 0x10) {
        job_mode_params[0].dropout_color = job_mode_params[1].dropout_color;
        for (count = 0; count < 6; count++)
            job_mode_params[0].dropout_levels[count] = job_mode_params[1].dropout_levels[count];
    }
    if (pages_seen & 0x40) {
        job_mode_params[0].buffer_mode[0] = job_mode_params[1].buffer_mode[0];
        job_mode_params[0].buffer_clear = job_mode_params[1].buffer_clear;
        job_mode_params[0].page_count_limit = job_mode_params[1].page_count_limit;
        job_mode_params[0].buffer_byte_6 = job_mode_params[1].buffer_byte_6;
    }
    if (pages_seen & 0x800) {
        job_mode_params[0].auto_length = job_mode_params[1].auto_length;
        job_mode_params[0].auto_width = job_mode_params[1].auto_width;
        job_mode_params[0].overscan = job_mode_params[1].overscan;
        job_mode_params[0].deskew = job_mode_params[1].deskew;
        value_3 = job_mode_params[1].auto_length;
        value_3 &= 0x80;
        value_3 = value_3 >> 1;
    }
    if (pages_seen & 0x2000) {
        job_mode_params[0].lamp_off_minutes = job_mode_params[1].lamp_off_minutes;
        value_3 = job_mode_params[1].lamp_off_minutes;
        if (job_mode_params[1].lamp_off_minutes == 0)
            value_3 = 60;
        if (!power_state.f.saving)
            mdc_mailbox.lamp_off_minutes = value_3;
    }
    if (pages_seen & 0x1000) {
        job_mode_params[0].job_separation[0] = job_mode_params[1].job_separation[0];
        value_3 = job_mode_params[1].job_separation[0];
        value_3 &= 0x80;
    }
    if (value & 0x40)
        scsi_mode_changed = ~scsi_initiator_bit.u;
    if (pages_seen & 0x40) {
        if (selftest_startup_done == 0) {
            scsi_status = SCSI_STEP_GOOD;
            return;
        }
        mdc_request_stop();
        if (!scsi_disconnect()) {
            if (scsi_status) {
                return;
            } else {
                scsi_resume_flags.f.resume_pending = 1;
                scsi_cmd_state = 1;
                scsi_defer_enable_irq();
                return;
            }
            return;
        } else {
            scsi_resume_flags.f.resume_pending = 1;
            scsi_cmd_state = 1;
            scsi_defer_enable_irq();
            return;
        }
    }
    scsi_status = SCSI_STEP_GOOD;
    return;
}


void scsi_cmd_get_hw_status(void)
{
    struct page_record *current_page;
    unsigned short count;
    unsigned char bits;
    unsigned short limit;
    current_page = &page_records[page_current.v];
    if (!scsi_check_ready_2()) {
        if (hw_hostif_flags.f.usb) {
            count = scsi_cdb_byte_8[0].v | (scsi_cdb_byte_7[0].v << 8);
            usb_send_reply(count);
        }
        return;
    }
    if ((scsi_cdb_byte_1[0] & ~0xe0) || scsi_cdb_byte_2[0] || scsi_cdb_byte_3[0] || scsi_cdb_byte_4[0] || scsi_cdb_byte_5[0] || scsi_cdb_byte_6[0] || scsi_cdb_byte_9[0]) {
        scsi_sense_invalid_cdb();
        return;
    }
    count = scsi_cdb_byte_8[0].v | (scsi_cdb_byte_7[0].v << 8);
    if (count == 0) {
        if (hw_hostif_flags.f.usb)
            usb_start_data_in();
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    scsi_param_len.v = count;
    util_mem_clear(scsi_reply_buf, 48);
    scsi_reply_buf[0] = 0;
    scsi_reply_byte_1[0] = 0;
    if (power_state.f.saving) {
        scsi_reply_byte_2[0] = 0;
        if (((volatile struct st_pf *)hw_port_f)->DR.BIT.B1)
            bits = 0x80;
        else
            bits = 0;
        scsi_reply_byte_3[0] = bits;
    } else {
        bits = 0;
        if (mdc_mailbox.sensor_status.f.top)
            bits |= 0x80;
        scsi_reply_byte_2[0] = bits;
        /* byte 3: 0x80 hopper empty, 0x40 OMR sensor, 0x20 ADF open, 0x08 document cover open, 0x04 pick sensor,
           0x02 document cover closed again */
        bits = 0;
        if (mdc_mailbox.sensor_status.f.document_cover_open)
            bits |= 8;
        if (mdc_mailbox.sensor_status.f.adf_cover_open) {
            bits |= 0x20;
            bits |= 0x80;
        } else {
            if (mdc_mailbox.sensor_status.f.no_paper)
                bits |= 0x80;
            if (mdc_mailbox.sensor_status_2.f.df)
                bits |= 0x40;
            if (mdc_mailbox.sensor_status.f.pick)
                bits |= 4;
        }
        if (tick_document_cover_event) {
            bits |= 2;
            tick_document_cover_event = 0;
        }
        if (job_manual_feed_flags.w & 0x8000) {
            if (current_page->flags.f.sheet_fed) {
                bits &= 0x7f;
            } else if (job_manual_feed_flags.w & 1) {
                bits &= 0x7f;
            } else if (mdc_mailbox.sensor_status_2.f.manual_feed_empty) {
                bits |= 0x80;
            } else {
                bits &= 0x7f;
            }
        }
        if (power_state.f.saving || power_state.f.leave_request)
            bits &= 0x82;
        scsi_reply_byte_3[0] = bits;
    }
    bits = 0;
    if (power_state.f.saving)
        bits |= 0x80;
    if (settings_cleaning_cycle[0] == 0)
        limit = 200;
    else
        limit = settings_cleaning_cycle[0] * 20;
    if (settings_cleaning_count[1] & 0x80) {
        count = ((unsigned char)(settings_cleaning_count[1] & 0x7f) << 8) | settings_cleaning_count[0];
        if (count >= limit)
            bits |= 0x40;
    }
    if (settings_manual_feed_timeout[0] != 0) {
        if (job_manual_feed_flags.w & 0x80) {
        } else
            bits |= 2;
    }
    if (panel_current.button_scan_latch) {
        bits |= 1;
        panel_current.button_scan_latch = 0;
    }
    if (panel_current.button_send_to_latch) {
        bits |= 4;
        panel_current.button_send_to_latch = 0;
    }
    scsi_reply_byte_4[0] = bits;
    if (panel_current.function_number == 0 && selftest_startup_done != 0)
        bits = 10;
    else
        bits = panel_current.function_number;
    count = (settings_pick_roller_count.c[1] << 8) | settings_pick_roller_count.c[0];
    if (settings_pick_roller_alarm[0] == 0)
        limit = 0x1db;
    else
        limit = settings_pick_roller_alarm[0] * 19;
    if (count >= limit)
        bits |= 0x80;
    count = (settings_brake_roller_count.c[1] << 8) | settings_brake_roller_count.c[0];
    if (settings_brake_roller_alarm[0] == 0)
        limit = 0x1db;
    else
        limit = settings_brake_roller_alarm[0] * 19;
    if (count >= limit)
        bits |= 0x20;
    scsi_reply_byte_5[0] = bits;
    bits = 0;
    if (job_double_feed_flags.b.length_check || job_double_feed_flags.b.ultrasonic_check) {
        if (err_multifeed_reported)
            bits |= 1;
    }
    scsi_reply_byte_10[0] = 0;
    scsi_reply_byte_11[0] = 0;
    if (job_manual_feed_flags.w & 0x8000) {
        if (job_manual_feed_flags.w & 8)
            bits |= 2;
        if (current_page->error_flags.v & 0xc0)
            bits |= 2;
    }
    if (job_button_pause_flags & 1) {
        if (job_aborted)
            job_button_pause_flags |= 0x80;
        bits |= 8;
    }
    if (power_off_flags.f.warning_due == 1) {
        bits |= 0x10;
        _builtin_set_imask(15);
        power_off_flags.f.warning_due = 0;
        power_off_flags.f.idle_restarted = 0;
        _builtin_set_imask(scsi_saved_imask);
    }
    scsi_reply_byte_6[0] = bits;
    if (err_last_code.s.v == ERR_PAUSED_BY_USER)
        scsi_reply_byte_7[0] = 32;
    else if (err_multifeed_reported)
        scsi_reply_byte_7[0] = err_multifeed_code;
    else
        scsi_reply_byte_7[0] = err_last_code.s.v;
    if (job_button_pause_flags & 1)
        scsi_reply_byte_7[0] = 36;
    ((unsigned short *)scsi_reply_byte_8)[0] = 0;
    scsi_send_reply();
    return;
}


void scsi_cmd_scanner_control(void)
{
    unsigned char function;
    int bytes_left;
    int side;
    struct page_record *current_page;
    struct page_record *scanning_page;
    unsigned int alloc_len;
    current_page = &page_records[page_current.v];
    scanning_page = &page_records[page_scanning.v];
    log_add_record(2, SCSI_OP_SCANNER_CONTROL, scsi_cdb_byte_1[0] & 0x1f, 0);
    if (page_side_state[0].last_buf)
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    function = scsi_cdb_byte_1[0] & 15;
    if (function == SCSI_SCANNER_CONTROL_MANUAL_FEED) {
        _builtin_set_imask(15);
        job_manual_feed_flags.w = 0x8000;
        tick_manual_feed_seconds.v = 0;
        _builtin_set_imask(scsi_saved_imask);
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (hw_hostif_flags.f.usb) {
        if (scsi_cdb_byte_1[0] & 0x10) {
            if (scsi_page_read_done) {
                scsi_status = SCSI_STEP_GOOD;
                return;
            }
        }
    }
    if (!scsi_check_ready())
        return;
    if (hw_hostif_flags.f.usb)
        function = (scsi_cdb_byte_1[0] & 0xe0) | scsi_cdb_byte_3[0] | scsi_cdb_byte_4[0] | scsi_cdb_byte_5[0] | scsi_cdb_byte_9[0];
    else
        function = (scsi_cdb_byte_1[0] & 0xf0) | scsi_cdb_byte_2[0] | scsi_cdb_byte_3[0] | scsi_cdb_byte_4[0] | scsi_cdb_byte_5[0] | scsi_cdb_byte_6[0] | scsi_cdb_byte_7[0].v | scsi_cdb_byte_8[0].v | scsi_cdb_byte_9[0];
    if (function) {
        scsi_sense_invalid_cdb();
        return;
    }
    if (hw_hostif_flags.f.usb && (scsi_cdb_byte_1[0] & 0x10)) {
        scsi_read_window_id.v = scsi_cdb_byte_2[0];
        if (scsi_read_window_id.v == 0)
            side = 0;
        else
            side = 1;
        alloc_len = (unsigned short)(scsi_cdb_byte_8[0].v | (scsi_cdb_byte_7[0].v << 8)) | ((scsi_cdb_byte_6[0] & 0xffff) << 16);
        if (alloc_len == 0) {
            scsi_status = SCSI_STEP_GOOD;
            return;
        }
        if (win_params.side[side].compression.f.type & 0x80) {
            bytes_left = current_page->sides[side].jpeg_bytes - current_page->sides[side].jpeg_bytes_sent;
            if (current_page->sides[side].jpeg_done) {
                if (bytes_left) {
                    scsi_status = SCSI_STEP_GOOD;
                } else {
                    scsi_param_len.v = alloc_len;
                    scsi_set_good();
                    scsi_status = SCSI_STEP_CHECK_CONDITION;
                }
                return;
            }
        } else {
            bytes_left = current_page->sides[side].image_bytes - current_page->sides[side].image_bytes_sent;
            if (current_page->sides[side].flags.f.scanned) {
                if (bytes_left) {
                    scsi_status = SCSI_STEP_GOOD;
                } else {
                    scsi_param_len.v = alloc_len;
                    scsi_set_good();
                    scsi_status = SCSI_STEP_CHECK_CONDITION;
                }
                return;
            }
        }
        if (alloc_len <= bytes_left) {
            scsi_status = SCSI_STEP_GOOD;
            return;
        }
        if (win_sides.f.selection == 2) {
            if (!win_params.side[0].composition.f.multi_level && !job_interlace_read) {
                scsi_status = SCSI_STEP_GOOD;
                return;
            }
            if (!current_page->sides[side].flags.f.requested && !current_page->flags.f.scan_requested && current_page->flags.f.scanned == 0) {
                if (win_params.flags.f.adf && current_page->flags.f.sheet_fed) {
                    scsi_status = SCSI_STEP_GOOD;
                    return;
                }
                scsi_param_len.v = alloc_len;
                scsi_set_good();
                scsi_status = SCSI_STEP_CHECK_CONDITION;
            } else if (!current_page->sides[side].flags.f.requested && current_page->flags.f.scanned != 0) {
                scsi_status = SCSI_STEP_GOOD;
            } else if (jpeg_scan_flags.v & JPEG_SCAN_LONG_PAGE) {
                scsi_set_sense(SCSI_SENSE_MEDIUM_ERROR, SCSI_ASC_MEDIUM_TEMPORARY_NO_DATA);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
            } else {
                scsi_status = SCSI_STEP_GOOD;
            }
        } else {
            scsi_status = SCSI_STEP_GOOD;
        }
        return;
    }
    function = scsi_cdb_byte_1[0] & 15;
    switch (function) {
    case SCSI_SCANNER_CONTROL_ADF:
        win_params.flags.f.adf = 1;
        break;
    case SCSI_SCANNER_CONTROL_FLATBED:
    case SCSI_SCANNER_CONTROL_FLATBED_HS:
        win_params.flags.f.adf = 0;
        break;
    case SCSI_SCANNER_CONTROL_CANCEL:
        _builtin_set_imask(15);
        job_pause_page_count.v = 0;
        job_pause_requested = 0;
        job_button_pause_flags &= 0xfffe;
        scsi_clear_read_state();
        job_long_page_flags &= 0x80;
        mdc_mailbox.long_page = 0;
        _builtin_set_imask(scsi_saved_imask);
        if (selftest_startup_done == 0)
            break;
        mdc_request_stop();
        if (!scsi_disconnect()) {
            if (scsi_status) {
                return;
            } else {
                scsi_resume_flags.f.resume_pending = 1;
                scsi_cmd_state = 1;
                scsi_defer_enable_irq();
                return;
            }
            return;
        } else {
            scsi_resume_flags.f.resume_pending = 1;
            scsi_cmd_state = 1;
            scsi_defer_enable_irq();
            return;
        }
        _builtin_set_imask(scsi_saved_imask);
        break;
    case SCSI_SCANNER_CONTROL_LAMP_OFF:
    case SCSI_SCANNER_CONTROL_LAMP_ON:
    case SCSI_SCANNER_CONTROL_LAMP_NORMAL:
    case SCSI_SCANNER_CONTROL_LAMP_SAVING:
        break;
    case SCSI_SCANNER_CONTROL_SCAN_COMPLETE:
    case SCSI_SCANNER_CONTROL_EJECT_COMPLETE:
        if (job_scan_complete_wait) {
            if (tick_page_wait_ticks.v > 5000) {
                _builtin_set_imask(15);
                job_scan_complete_wait = 0;
                _builtin_set_imask(scsi_saved_imask);
            }
            scsi_set_sense(SCSI_SENSE_NOT_READY, SCSI_ASC_NONE);
            scsi_status = SCSI_STEP_CHECK_CONDITION;
            return;
        }
        break;
    case SCSI_SCANNER_CONTROL_PAUSE:   /* 0x0D: pause after the current page (error 0x24) */
        _builtin_set_imask(15);
        job_pause_requested = 1;
        _builtin_set_imask(scsi_saved_imask);
        break;
    default:
        break;
    }
    scsi_status = SCSI_STEP_GOOD;
}

void scsi_sense_invalid_param_at(unsigned char *field)
{
    scsi_sense_invalid_param(SCSI_SENSE_SKSV, field - scsi_param_buf - 1);
}

void scsi_set_sense(sense_key, asc)
char sense_key;
short asc;
{
    scsi_sense_data[2] = sense_key;
    ((short *)scsi_sense_data)[6] = asc;
}

void scsi_sense_invalid_param(unsigned char key_specific, short field_pointer)
{
    scsi_sense_data[15] = key_specific;
    *(unsigned short *)&scsi_sense_data[16] = field_pointer;
    scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_PARAMETERS);
    scsi_status = SCSI_STEP_CHECK_CONDITION_FIELD;
}

#pragma section

int scsi_check_ready(void)
{
    if (!hw_hostif_flags.f.usb) {
        if (!scsi_check_reservation()) {
            if (scsi_cdb[0] == SCSI_OP_RELEASE_UNIT)
                scsi_status = SCSI_STEP_GOOD;
            else
                scsi_status = SCSI_STEP_RESERVATION_CONFLICT;
            return 0;
        }
        if (!scsi_check_unit_attention()) {
            scsi_status = SCSI_STEP_CHECK_CONDITION;
            return 0;
        }
    }
    if (!err_report_page_error()) {
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    }
    if (!scsi_check_not_ready()) {
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    }
    return 1;
}

int scsi_check_ready_2(void)
{
    if (!hw_hostif_flags.f.usb) {
        if (!scsi_check_reservation()) {
            if (scsi_cdb[0] == SCSI_OP_RELEASE_UNIT)
                scsi_status = SCSI_STEP_GOOD;
            else
                scsi_status = SCSI_STEP_RESERVATION_CONFLICT;
            return 0;
        }
        if (!scsi_check_unit_attention()) {
            scsi_status = SCSI_STEP_CHECK_CONDITION;
            return 0;
        }
    }
    if (!scsi_check_not_ready()) {
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    }
    return 1;
}

int scsi_lowest_bit(bits)
unsigned char bits;
{
    unsigned char bit;
    bit = 0;
    while (bit < 7) {
        if (bits & 1)
            break;
        bits >>= 1;
        bit++;
    }
    return (char)bit;
}

int scsi_initiator_allowed(void)
{
    if (scsi_disconnected_initiator.v != 0) {
        if (scsi_disconnected_initiator.v != scsi_initiator_bit.c)
            return 0;
    } else {
        if (scsi_sense_owner.v != 0 && scsi_sense_owner.v != scsi_initiator_bit.c)
            return 0;
    }
    return 1;
}

int scsi_check_parity_error(void)
{
    int dummy;
    if (scsi_scsiint1_status.b.b7) {
        scsi_set_sense(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_SCSI_PARITY_ERROR);  /* sense 0B/47/00: SCSI parity error */
        err_display_flags.b.b5 = 1;
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    }
    return 1;
}

int scsi_check_reservation(void)
{
    if (hw_hostif_flags.f.usb)
        return 1;
    if (scsi_reserved_by.v == 0 || (scsi_reserved_by.v != 0 && scsi_reserved_by.v == scsi_initiator_bit.c))
        return 1;
    return 0;
}

int scsi_check_unit_attention(void)
{
    unsigned char initiator_bit;
    if (hw_hostif_flags.f.usb)
        return 1;
    initiator_bit = scsi_initiator_bit.c;
    if ((initiator_bit & scsi_ua_reported) && !(initiator_bit & scsi_mode_changed))
        return 1;
    if (!(initiator_bit & scsi_ua_reported)) {
        scsi_ua_reported |= initiator_bit;
        scsi_set_sense(SCSI_SENSE_UNIT_ATTENTION, SCSI_ASC_NONE);  /* UNIT ATTENTION, first command of this initiator */
    } else {
        initiator_bit = ~initiator_bit;
        scsi_mode_changed &= initiator_bit;
        scsi_set_sense(SCSI_SENSE_UNIT_ATTENTION, SCSI_ASC_MODE_PARAMETERS_CHANGED);  /* 6/2A/01: mode parameters changed (MODE SELECT by another initiator) */
    }
    return 0;
}

int scsi_check_not_ready(void)
{
    if (selftest_passed) {
        if (scsi_setup_mode || selftest_mdc_in_boot || scsi_diag_started) {
            if (scsi_cdb[0] == SCSI_OP_RESERVE_UNIT || scsi_cdb[0] == SCSI_OP_RELEASE_UNIT || scsi_cdb[0] == SCSI_OP_INQUIRY || scsi_cdb[0] == SCSI_OP_REQUEST_SENSE ||
                scsi_cdb[0] == SCSI_OP_SEND_DIAGNOSTIC || scsi_cdb[0] == SCSI_OP_READ_DIAGNOSTIC || scsi_cdb[0] == SCSI_OP_TEST_UNIT_READY || scsi_cdb[0] == SCSI_OP_MODE_SENSE ||
                scsi_cdb[0] == SCSI_OP_GET_HW_STATUS) {
                return 1;
            } else {
                scsi_set_sense(SCSI_SENSE_NOT_READY, SCSI_ASC_NONE);
                return 0;
            }
        } else {
            return 1;
        }
    } else {
        if (scsi_cdb[0] == SCSI_OP_RESERVE_UNIT || scsi_cdb[0] == SCSI_OP_RELEASE_UNIT)
            return 1;
        scsi_set_sense(SCSI_SENSE_NOT_READY, SCSI_ASC_NONE);
        return 0;
    }
}

int scsi_sense_invalid_cdb(void)
{
    scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB);
    scsi_status = SCSI_STEP_CHECK_CONDITION;
}

void scsi_clear_sync_agreement(void)
{
    scsi_sync_initiators &= ~scsi_initiator_bit.c;
}

int scsi_store_sync_agreement(void)
{
    unsigned char idx;
    if (scsi_sdtr_offset[0] == 0) {
        scsi_clear_sync_agreement();
    } else {
        scsi_sync_initiators |= scsi_initiator_bit.c;
        idx = scsi_lowest_bit(scsi_initiator_bit.c);
        scsi_sync_params[idx * 2] = scsi_sdtr_period[0];
        scsi_sync_offsets[idx * 2] = scsi_sdtr_offset[0];
    }
}

/* Marks the parameter at p invalid: field pointer = p - scsi_param_buf, scsi_param_valid = 0. */
void scsi_mark_invalid_field(field)
unsigned char *field;
{
    scsi_field_pointer = field - scsi_param_buf;
    scsi_param_valid.v = 0;
}

extern int scsi_halftone_ready(unsigned short);
extern int scsi_gamma_ready(unsigned char);
extern int scsi_lut_ready(unsigned char);
extern int scsi_jpeg_qtables_ready(unsigned char);

int scsi_read_check_window_tables(side)
int side;
{
    unsigned short halftone;
    unsigned char pattern;
    halftone = win_params.side[side].halftone.f.pattern;
    if (!scsi_halftone_ready(halftone)) {
        scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_WINDOW_COMBINATION);  /* ILLEGAL REQUEST, ASC 2C/02: invalid combination of windows */
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    }
    pattern = win_params.side[side].gamma_pattern.f.v;
    if (!scsi_gamma_ready(pattern)) {
        scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_WINDOW_COMBINATION);
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    }
    pattern = win_params.side[side].lut_pattern.f.v;
    if (!scsi_lut_ready(pattern)) {
        scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_WINDOW_COMBINATION);
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    }
    pattern = win_params.side[side].compression.f.type & 255;
    if (pattern & 0x80) {
        pattern = win_params.side[side].compression.f.argument & 255;
        if (!scsi_jpeg_qtables_ready(pattern)) {
            scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_WINDOW_COMBINATION);
            scsi_status = SCSI_STEP_CHECK_CONDITION;
            return 0;
        }
    }
    return 1;
}

int scsi_halftone_ready(pattern)
unsigned short pattern;
{
    if ((pattern & 0x8000) == 0)
        return 1;
    if (img_halftone_downloads[pattern & 7].v == 0)
        return 0;
    return 1;
}

int scsi_gamma_ready(pattern)
unsigned char pattern;
{
    if ((pattern & 0x80) == 0)
        return 1;
    if (img_gamma_loaded[pattern & 7] == 0)
        return 0;
    return 1;
}

int scsi_lut_ready(pattern)
unsigned char pattern;
{
    if ((pattern & 0x80) == 0)
        return 1;
    if (img_lut_loaded[pattern & 1] == 0)
        return 0;
    return 1;
}

int scsi_jpeg_qtables_ready(compression)
unsigned char compression;
{
    if ((compression & 8) == 0) {
    } else {
        if (jpeg_qtables_downloaded == 0)
            return 0;
    }
    if ((compression & 0x80) == 0) {
    } else {
        return 0;
    }
    return 1;
}

struct cell_u8_scsi { unsigned char v; };

int scsi_inquiry_standard(void)
{
    unsigned char alloc_len;
    int i;
    alloc_len = scsi_cdb_byte_4[0];
    if (alloc_len == 0) {
        scsi_status = SCSI_STEP_GOOD;
        return 0;
    }
    if (alloc_len > 0x60 && !hw_hostif_flags.f.usb)
        alloc_len = 0x60;
    scsi_param_len.v = alloc_len;
    if ((scsi_identify_message & 7) == 0) {
        for (i = 0; i < 0x60; i++)
            scsi_reply_buf[i] = scsi_inquiry_data[i];
        alloc_len = settings_emulation[0] & 0x1c;
        i = 0;
        if (alloc_len == 0x10 || alloc_len == 0xc || alloc_len == 0x12 || alloc_len == 0x13 || alloc_len == 0x1c || alloc_len == 0x18 || alloc_len == 0x14 || alloc_len == 0x11 || alloc_len == 0x15 || alloc_len == 8 || alloc_len == 4) {
            scsi_reply_byte_45[0] |= 0x40;
            util_mem_set(scsi_reply_byte_16, 32, 16);
            if (alloc_len == 0x13) {
                scsi_reply_byte_16[0] = 'M';
                scsi_reply_byte_17[0] = '4';
                scsi_reply_byte_18[0] = '0';
                scsi_reply_byte_19[0] = '9';
                scsi_reply_byte_20[0] = '7';
                scsi_reply_byte_21[0] = 'D';
                scsi_reply_byte_22[0] = 'd';
                i = 0x17;
                if (hw_option_flags.f.b0) {
                    scsi_reply_buf[i] = 'e';
                    i++;
                }
                if (hw_option_flags.f.b1) {
                    scsi_reply_buf[i] = 'i';
                    i++;
                }
                scsi_reply_buf[i] = 'm';
            } else if (alloc_len == 0x1c || alloc_len == 0x18) {
                scsi_reply_byte_16[0] = 'f';
                scsi_reply_byte_17[0] = 'i';
                scsi_reply_byte_18[0] = '-';
                scsi_reply_byte_19[0] = '5';
                if (alloc_len == 0x18)
                    scsi_reply_byte_20[0] = '7';
                else
                    scsi_reply_byte_20[0] = '6';
                scsi_reply_byte_21[0] = '5';
                scsi_reply_byte_22[0] = '0';
                scsi_reply_byte_23[0] = 'C';
                scsi_reply_byte_24[0] = 'd';
                scsi_reply_byte_25[0] = 'j';
                for (i = 0; i < 6; i++)
                    (*(struct cell_s8_scsi (*)[])scsi_reply_buf)[26 + i].v = 32;
            } else if (alloc_len == 0x14) {
                scsi_reply_byte_16[0] = 'F';
                scsi_reply_byte_17[0] = 'i';
                scsi_reply_byte_18[0] = '-';
                scsi_reply_byte_19[0] = '5';
                scsi_reply_byte_20[0] = '9';
                scsi_reply_byte_21[0] = '0';
                scsi_reply_byte_22[0] = '0';
                scsi_reply_byte_23[0] = 'C';
                scsi_reply_byte_24[0] = 'd';
                scsi_reply_byte_25[0] = ' ';
                for (i = 0; i < 6; i++)
                    (*(struct cell_s8_scsi (*)[])scsi_reply_buf)[26 + i].v = 32;
            } else if (alloc_len == 0x15 || alloc_len == 8 || alloc_len == 4) {
                scsi_reply_byte_16[0] = 'f';
                scsi_reply_byte_17[0] = 'i';
                scsi_reply_byte_18[0] = '-';
                scsi_reply_byte_19[0] = '6';
                if (alloc_len == 0x15) {
                    scsi_reply_byte_20[0] = '7';
                    scsi_reply_byte_21[0] = '7';
                } else if (alloc_len == 8) {
                    scsi_reply_byte_20[0] = '6';
                    scsi_reply_byte_21[0] = '7';
                } else {
                    scsi_reply_byte_20[0] = '7';
                    scsi_reply_byte_21[0] = '5';
                }
                scsi_reply_byte_22[0] = '0';
                if (alloc_len == 4)
                    scsi_reply_byte_23[0] = 'S';
                else
                    scsi_reply_byte_23[0] = 'd';
                scsi_reply_byte_24[0] = 'j';
                scsi_reply_byte_25[0] = ' ';
                for (i = 0; i < 6; i++)
                    (*(struct cell_s8_scsi (*)[])scsi_reply_buf)[26 + i].v = 32;
            } else {
                scsi_reply_byte_16[0] = 'f';
                scsi_reply_byte_17[0] = 'i';
                scsi_reply_byte_18[0] = '-';
                scsi_reply_byte_19[0] = '4';
                if (alloc_len == 0xc) {
                    scsi_reply_byte_20[0] = '6';
                    scsi_reply_byte_21[0] = '4';
                    scsi_reply_byte_22[0] = '0';
                    scsi_reply_byte_23[0] = 'S';
                    i = 24;
                    if (hw_option_flags.f.b0) {
                        scsi_reply_buf[i] = 'e';
                        i++;
                    }
                    if (hw_option_flags.f.b1) {
                        scsi_reply_buf[i] = 'i';
                        i++;
                    }
                    scsi_reply_buf[i] = 'm';
                } else {
                    scsi_reply_byte_20[0] = '7';
                    scsi_reply_byte_21[0] = '5';
                    scsi_reply_byte_22[0] = '0';
                    if (alloc_len == 0x10) {
                        scsi_reply_byte_23[0] = 'C';
                        scsi_reply_byte_24[0] = 'd';
                        i = 25;
                        if (hw_option_flags.f.b0) {
                            scsi_reply_buf[i] = 'e';
                            i++;
                        }
                        if (hw_option_flags.f.b1) {
                            scsi_reply_buf[i] = 'i';
                            i++;
                        }
                        scsi_reply_buf[i] = 'j';
                        i++;
                        scsi_reply_buf[i] = 'm';
                    } else {
                        scsi_reply_byte_23[0] = 'L';
                        scsi_reply_byte_24[0] = 'd';
                        i = 25;
                        if (hw_option_flags.f.b0) {
                            scsi_reply_buf[i] = 'e';
                            i++;
                        }
                        if (hw_option_flags.f.b1) {
                            scsi_reply_buf[i] = 'i';
                            i++;
                        }
                        scsi_reply_buf[i] = 'm';
                    }
                }
            }
            if (alloc_len == 0x10 || alloc_len == 0xc || alloc_len == 0x12 || alloc_len == 0x13 || alloc_len == 0x1c || alloc_len == 0x18) {
                scsi_reply_byte_44[0] |= 3;
                scsi_reply_byte_45[0] |= 0xc;
            }
        }
    } else {
        for (i = 0; i < 0x60; i++)
            scsi_reply_buf[i] = ((struct cell_u8_scsi *)(i + (int)scsi_inquiry_invalid_lun))->v;
    }
    return 1;
}

int scsi_inquiry_vpd(void)
{
    unsigned char alloc_len;
    unsigned char emulation;
    unsigned long sdram_bytes;
    int i;
    unsigned short overscan;
    emulation = settings_emulation[0] & 0x1c;
    alloc_len = scsi_cdb_byte_4[0];
    if (alloc_len == 0) {
        scsi_status = SCSI_STEP_GOOD;
        return 0;
    }
    if (alloc_len >= 0x90 && !hw_hostif_flags.f.usb && settings_emulation[0] < 4) {
        alloc_len = 0x90;
    } else if (alloc_len > 0x70 && !hw_hostif_flags.f.usb && settings_emulation[0] >= 4) {
        alloc_len = 0x70;
    }
    scsi_param_len.v = alloc_len;
    if ((scsi_identify_message & 7) == 0) {
        for (i = 0; i < alloc_len; i++)
            scsi_reply_buf[i] = ((struct cell_u8_scsi *)(i + (int)scsi_inquiry_vpd_data))->v;
        if (hw_option_flags.f.b1)
            scsi_reply_byte_88[0] = 0xfd40;
        if (hw_option_flags.f.b0) {
            scsi_reply_byte_32[0] |= 8;
            scsi_reply_byte_92[0] = 0x8102;
        }
        if (settings_byte_0x70[0] & 0x80)
            scsi_reply_byte_112[0] |= 6;
        emulation = settings_emulation[0] & 0x1c;
        if (emulation == 0x10 || emulation == 0xc || emulation == 0x12 || emulation == 0x13 || emulation == 0x1c || emulation == 0x18) {
            scsi_reply_byte_86[0] |= 2;
            scsi_reply_byte_87[0] |= 2;
            scsi_reply_byte_111[0] = 0;
            scsi_reply_byte_112[0] = 0;
            scsi_reply_byte_115[0] = 0;
        }
        if (emulation == 4)
            scsi_reply_byte_32[0] &= 0xef;
        if (emulation == 0x15 || emulation == 4)
            scsi_reply_byte_32[0] |= 0x40;
        else if (emulation == 8)
            scsi_reply_byte_32[0] &= 0xbf;
        scsi_reply_byte_90[0] = 8;
        sdram_bytes = img_sdram_bytes[0];
        sdram_bytes >>= 16;
        scsi_reply_byte_34[0] = sdram_bytes;
        if (hw_hostif_flags.f.usb) {
            _builtin_set_gbr(0);
            if (usb_ctrl->usb_status.b6)
                scsi_reply_byte_98[0] = 1;
            else
                scsi_reply_byte_98[0] = 2;
        }
        if (settings_overscan_control[0] & 0x80) {
            overscan = settings_overscan_control[0] & 15;
            if (overscan == 0)
                overscan = 11;
            overscan = rt_divu32(overscan * 6000, 0xfe);
            overscan += 0x178;
            scsi_reply_byte_100[0] = overscan >> 8;
            scsi_reply_byte_101[0] = overscan;
            scsi_reply_byte_102[0] = overscan >> 8;
            scsi_reply_byte_103[0] = overscan;
        }
        if ((settings_emulation[0] & 0x1c) == 0 && hw_board_variant == 1 && (settings_power_off_enable[0] & 0x80))
            scsi_reply_byte_118[0] |= 0x80;
    } else {
        for (i = 0; i < 30; i++)
            scsi_reply_buf[i] = ((struct cell_u8_scsi *)(i + (int)scsi_inquiry_vpd_invalid_lun))->v;
    }
    return 1;
}

int scsi_cmd_endorser(void)
{
    unsigned short len;
    unsigned char reserved_bits;
    unsigned char ok;
    unsigned short slot;
    ok = scsi_check_ready();
    if (!ok) {
        if (!hw_hostif_flags.f.usb)
            return;
    }
    if (!hw_option_flags.f.b0) {
        scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_OPCODE);
        scsi_status = SCSI_STEP_CHECK_CONDITION;
    }
    reserved_bits = scsi_cdb_byte_2[0] | scsi_cdb_byte_3[0] | scsi_cdb_byte_4[0] | scsi_cdb_byte_5[0] | scsi_cdb_byte_6[0] | scsi_cdb_byte_9[0];
    if ((scsi_cdb_byte_1[0] & ~0xe0) || reserved_bits) {
        scsi_sense_invalid_cdb();
        if (!hw_hostif_flags.f.usb)
            return;
        ok = 0;
    }
    len = scsi_cdb_byte_8[0].v | (scsi_cdb_byte_7[0].v << 8);
    scsi_param_len.v = len;
    if (len == 0) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (len != 4 && len != 6 && !hw_hostif_flags.f.usb) {
        return scsi_sense_invalid_cdb();
    }
    if (!scsi_receive_params() || !ok)
        return;
    if ((settings_emulation[0] & 0x1c) == 0x14) {
        if (scsi_param_buf[0] == 0x80) {
            scsi_status = SCSI_STEP_GOOD;
            _builtin_set_imask(scsi_saved_imask);
            return;
        }
    }
    if (!(scsi_param_byte_1[0] & 0x20)) {
        scsi_imprinter_counter.v = util_get_be16(scsi_param_byte_2);
    } else {
        if (len == 6 && (scsi_param_byte_1[0] & 0x20)) {
            scsi_imprinter_counter.v = util_get_be32(scsi_param_byte_2);
            scsi_imprinter_counter.v &= 0xffffff;
        } else {
            return scsi_sense_invalid_cdb();
        }
    }
    _builtin_set_imask(15);
    scsi_imprinter_flags.f.b6 = 1;
    if (scsi_param_buf[0]) {
        _builtin_set_imask(scsi_saved_imask);
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 0);
        return;
    }
    if (!(scsi_param_byte_1[0] & 0x40)) {
        _builtin_set_imask(scsi_saved_imask);
        scsi_sense_invalid_param(SCSI_SENSE_SKSV, 1);
        return;
    }
    if (scsi_param_byte_1[0] & 0x80)
        scsi_imprinter_print.f.b7 = 0;
    else
        scsi_imprinter_print.f.b7 = 1;
    win_params.b20.b6 = 1;
    scsi_imprinter_counter_initial = scsi_imprinter_counter.v;
    _builtin_set_imask(scsi_saved_imask);
    _builtin_set_imask(15);
    mdc_back_window[slot = scsi_imprinter_next_slot()] = 0xc1;
    if (scsi_imprinter_print.f.b7) {
        mdc_back_window[slot = scsi_imprinter_next_slot()] = scsi_imprinter_print.v;
        mdc_back_window[slot = scsi_imprinter_next_slot()] = +(unsigned char)(scsi_imprinter_counter.v >> 24);
        mdc_back_window[slot = scsi_imprinter_next_slot()] = +(unsigned char)(scsi_imprinter_counter.v >> 16);
        mdc_back_window[slot = scsi_imprinter_next_slot()] = +(unsigned char)(scsi_imprinter_counter.v >> 8);
        mdc_back_window[slot = scsi_imprinter_next_slot()] = +scsi_imprinter_counter.c.lo;
    } else {
        mdc_back_window[slot = scsi_imprinter_next_slot()] = 0;
    }
    mdc_back_window[slot += 2] = 0xff;
    mdc_back_window[slot += 2] = 0xff;
    scsi_status = SCSI_STEP_GOOD;
    _builtin_set_imask(scsi_saved_imask);
}

unsigned short scsi_imprinter_next_slot(void)
{
    unsigned short slot;
    unsigned short next_slot;
    slot = scsi_imprinter_slot;
    if (slot >= 0x220) {
        slot = 0x221;
    } else {
        scsi_imprinter_slot = next_slot = slot + 2;
    }
    return slot;
}


#pragma section H
void scsi_init_sense(int clear_key_specific)
{
    struct page_record *current_page;
    current_page = &page_records[page_current.v];
    if (clear_key_specific) {
        scsi_sense_data[15] = 0;
        scsi_sense_data[16] = 0;
    }
    scsi_sense_data[0] = SCSI_SENSE_CURRENT_ERROR;
    scsi_sense_data[1] = 0;
    switch (job_sensor_sides.v) {
    case 1:
        if (!current_page->sides[1].flags.f.requested)
            scsi_sense_data[2] |= 0x40;
        break;
    default:
        if (!current_page->sides[0].flags.f.requested)
            scsi_sense_data[2] |= 0x40;
        break;
    }
    util_mem_copy(&scsi_sense_data[3], &scsi_sense_information.v, 4);
    if (scsi_sense_information.v != 0) {
        if (scsi_sense_information.v == scsi_param_len.v) {
            scsi_sense_data[2] |= 0x20;
            scsi_status_byte = SCSI_STATUS_CHECK_CONDITION;
        }
    }
    scsi_sense_data[7] = 10;
    scsi_sense_data[8] = 0;
    scsi_sense_data[9] = 0;
    scsi_sense_data[10] = 0;
    scsi_sense_data[11] = 0;
    scsi_sense_data[14] = 0;
}

void scsi_set_good(void)
{
    scsi_sense_information.v = scsi_param_len.v;
    scsi_set_sense(SCSI_SENSE_NO_SENSE, SCSI_ASC_NONE);
}

int scsi_reselect_pending(void)
{
    scsi_reselect_active = 0;
    if (scsi_disconnected_initiator.v == 0)
        return 1;
    scsi_message_retries.v = 0;
    scsi_reselect();
    scsi_reselect_active = 0x80;
    return 0;
}

int scsi_check_reselection(void)
{
    if (!(*(union UBF *)&scsi_scsiint1_status).f.b5) {
        scsi_disconnected_initiator.v = 0;
        scsi_reselect_active = 0;
        if (!scsi_run_messages(1) && scsi_status)
            return 0;
    } else {
        scsi_reselect_retries.v++;
        if (scsi_reselect_retries.v <= 4)
            return 0;
        else {
            scsi_reselect_failed = 1;
            scsi_disconnected_initiator.v = 0;
            scsi_reselect_active = 0;
            scsi_set_sense(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_SELECT_RESELECT_FAILURE);
            scsi_status = 1;
            return 0;
        }
    }
    return 1;
}
#pragma section

int scsi_cmd_read(void)
{
    if (!scsi_check_ready()) {
        if (hw_hostif_flags.f.usb)
            scsi_read_reply();
        return;
    }
    {
        if ((scsi_cdb_byte_1[0] & ~0xe0) || scsi_cdb_byte_3[0] || scsi_cdb_byte_4[0] || scsi_cdb_byte_9[0]) {
            if (hw_hostif_flags.f.usb)
                scsi_read_reply();
            scsi_sense_invalid_cdb();
            return;
        }
        if (scsi_cdb_byte_2[0] == SCSI_READ_IMAGE || scsi_cdb_byte_2[0] == SCSI_READ_PIXEL_SIZE || scsi_cdb_byte_2[0] == SCSI_READ_PAPER_SIZE) {
        } else {
            if (hw_hostif_flags.f.usb)
                scsi_read_reply();
            scsi_sense_invalid_cdb();
            return;
        }
        scsi_read_window_id.v = scsi_cdb_byte_5[0];
        if (win_params.flags.f.adf && win_sides.f.selection == 1)
            scsi_read_window_id.v = 0x80;
        if (scsi_read_window_id.v != 0 && scsi_read_window_id.v != 0x80) {
            if (hw_hostif_flags.f.usb)
                scsi_read_reply();
            scsi_sense_invalid_cdb();
            return;
        }
        if (scsi_read_window_id.v == 0)
            scsi_read_side.v = 0;
        else
            scsi_read_side.v = 1;
        scsi_param_len.v = (unsigned short)(scsi_cdb_byte_8[0].v | (scsi_cdb_byte_7[0].v << 8)) | ((unsigned long)(unsigned short)(+scsi_cdb_byte_6[0]) << 16);
        if (scsi_param_len.v == 0) {
            if (hw_hostif_flags.f.usb)
                usb_start_data_in();
            scsi_status = SCSI_STEP_GOOD;
            return;
        }
        if (scsi_cdb_byte_2[0] == SCSI_READ_IMAGE) {
            if (hw_hostif_flags.f.usb) {
                usb_read_len = scsi_param_len.v;
                usb_read_byte_unused = 0;
            }
            log_add_record(2, 40, scsi_read_side.v, 1);
            scsi_read_start();
        } else if (scsi_cdb_byte_2[0] == SCSI_READ_PIXEL_SIZE) {
            scsi_cmd_read_step_7();
        } else {
            scsi_cmd_read_step_8();
        }
    }
}

int scsi_read_reply(void)
{
    scsi_xfer_len.v = (unsigned short)(scsi_cdb_byte_8[0].v | (scsi_cdb_byte_7[0].v << 8)) | ((unsigned long)(unsigned short)(+scsi_cdb_byte_6[0]) << 16);
    usb_send_reply(scsi_xfer_len.v);
}

int scsi_clear_read_state(void)
{
    job_aborted = 1;
    line_side_state[0].state_flags.v = line_side_state[1].state_flags.v = 0;
    line_page_state[0].state_flags.v = line_page_state[1].state_flags.v = 0;
    line_page_state[0].end_line = 0xffff;
    line_page_state[1].end_line = 0xffff;
    job_adf_sheet_count = 0;
    util_mem_clear(&page_buf_count, 0x11ac);
    util_mem_clear(page_side_records, 0x198);
    util_mem_clear(page_side_state, 48);
    util_mem_clear(&page_side_state[1], 48);
    job_pause_requested = 0;
    job_pause_page_count.v = 0;
    job_sheet_count = 0;
}


int scsi_read_start(void)
{
    struct page_record *current_page;
    current_page = &page_records[page_current.v];
    xfer_chunk_bytes = 0;
    if (scsi_page_read_done) {
    } else {
        if (win_params.flags.f.adf && win_sides.f.selection == 1) {
            if (!current_page->sides[1].flags.f.requested && !current_page->sides[1].flags.f.last_chunk) {
                if (!scsi_read_progress.f.b2 && !scsi_read_check_windows(1, 2)) {
                    if (hw_hostif_flags.f.usb && scsi_cdb[0] != SCSI_OP_SCAN)
                        scsi_read_reply();
                    return 0;
                }
            } else {
                scsi_message_retries.v = 0;
            }
        } else {
            if (!current_page->sides[0].flags.f.requested && !current_page->sides[0].flags.f.last_chunk) {
                if (!scsi_read_progress.f.b3) {
                    _builtin_set_imask(15);
                    _builtin_set_imask(scsi_saved_imask);
                    if (!(win_sides.v & 2)) {
                        if (!scsi_read_check_windows(0, 1)) {
                            if (hw_hostif_flags.f.usb && scsi_cdb[0] != SCSI_OP_SCAN)
                                scsi_read_reply();
                            return 0;
                        }
                    } else {
                        if (!win_params.flags.f.adf) {
                            scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_COMMAND_SEQUENCE);
                            scsi_status = SCSI_STEP_CHECK_CONDITION;
                            return 0;
                        }
                        if (!scsi_read_check_windows(2, 2)) {
                            if (hw_hostif_flags.f.usb && scsi_cdb[0] != SCSI_OP_SCAN)
                                scsi_read_reply();
                            return 0;
                        }
                    }
                }
            } else {
                scsi_message_retries.v = 0;
            }
        }
        if (!win_params.flags.f.adf && scsi_read_window_id.v) {
            scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_COMMAND_SEQUENCE);
            scsi_status = SCSI_STEP_CHECK_CONDITION;
            return 0;
        }
    }
    if (scsi_read_side.v == 0) {
        scsi_read_progress.f.b3 = 1;
        xfer_state_front = XFER_STATE_INIT;
        xfer_state_back = 0;
        scsi_run_xfer_front();
    } else {
        scsi_read_progress.f.b2 = 1;
        xfer_state_back = XFER_STATE_INIT;
        xfer_state_front = 0;
        scsi_run_xfer_back();
    }
    return 1;
}


struct scsi_win_descriptor_slot { unsigned short v; char pad_02[254]; };

unsigned char scsi_read_check_windows(int request_side, int side_end)
{
    struct page_record *p;
    int side;
    int first_side;
    p = &page_records[page_current.v];
    if (job_mode_params[0].interlace_read == 1)
        job_interlace_read = 1;
    else
        job_interlace_read = 0;
    if (win_sides.f.selection == 2) {
        if (win_params.side[0].composition.f.multi_level && win_params.side[1].composition.f.multi_level) {
            job_interlace_read = 1;
        } else {
            if (win_long_length)
                job_interlace_read = 1;
        }
    } else {
        job_interlace_read = 0;
    }
    if (request_side == 1)
        first_side = 1;
    else
        first_side = 0;
    side = first_side;
    for (; side < side_end; side++) {
        if (!(((struct scsi_win_descriptor_slot *)scsi_win_saved_descriptors)[side].v & 0x80)) {
            scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_WINDOW_COMBINATION);
            scsi_status = SCSI_STEP_CHECK_CONDITION;
            return 0;
        }
        if (!scsi_read_check_window_tables(side))
            return 0;
        scsi_read_check_windows_step_3(side);
    }
    return 1;
}


void scsi_read_check_windows_step_3(int side)
{
    struct page_record *current_page;
    current_page = &page_records[page_current.v];
    current_page->sides[side].flags.f.requested = 1;
    scsi_read_check_value = 0;
    xfer_chunk_max_2 = 0x10000;
    xfer_chunk_max = 0x10000;
}

void scsi_run_xfer_front(void)
{
    while (1) {
        if ((unsigned char)(xfer_state_front = xfer_state_table[xfer_state_front]()) == 0)
            break;
        else {
            if (xfer_state_front == 4 || xfer_state_front == 11)
                break;
        }
    }
}

void scsi_run_xfer_back(void)
{
    while (1) {
        if ((unsigned char)(xfer_state_back = xfer_state_table[xfer_state_back]()) == 0)
            break;
        else {
            if (xfer_state_back == 4 || xfer_state_back == 11)
                break;
        }
    }
}
