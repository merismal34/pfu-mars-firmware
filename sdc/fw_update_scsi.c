#define fw_update_onchip_blocks fw_update_onchip_blocks_hdr
#define fw_update_ext_sectors fw_update_ext_sectors_hdr
#include "boot.h"
#undef fw_update_onchip_blocks
#undef fw_update_ext_sectors
#pragma section 2

void fw_update_handle_command(struct fw_update_command_link *record)
{
    struct fw_update_cmd_entry *entry;
    char opcode;

    opcode = record->cdb[0];
    entry = fw_update_cmd_table;
    while (entry->fn != 0) {
        if (entry->opcode == opcode)
            break;
        entry++;
    }
    if (entry->fn != 0) {
        entry->fn();
    } else {
        fw_update_set_sense(record, SCSI_SENSE(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_OPCODE));  /* sense 5/20/00: invalid command opcode */
        record->status_flags.b1 = 1;
        if (fw_update_hostif_flags.scsi == 1) {
            fw_update_end_command();
        } else {
            _builtin_set_imask(15);
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_usb_ctrl->reg_17.b6 = 1;
            _builtin_set_imask(0);
            fw_update_end_command();
        }
    }
}


void fw_update_handle_status(struct fw_update_command_link *record)
{
    int saved_imask;
    unsigned char status;
    unsigned char i;

    saved_imask = _builtin_get_imask();
    fw_update_unqueue_record(record);
    _builtin_set_imask(15);
    status = *(unsigned char *)&record->status_flags;
    if (status & 0x80)
        return;
    if (status & 0x10) {
        status = SCSI_STATUS_CHECK_CONDITION;
    } else if (status & 8) {
        status = SCSI_STATUS_RESERVATION_CONFLICT;
    } else if (status & 4) {
        status = SCSI_STATUS_BUSY;
    } else if (status & 2) {
        status = SCSI_STATUS_CHECK_CONDITION;
    } else {
        fw_update_clear_sense();
        status = SCSI_STATUS_GOOD;
    }
    record->status_byte = status;
    record->message_in = 0;
    record->handler = fw_update_current_record.p->saved_handler;
    record->saved_handler = fw_update_current_record.p->saved_handler_outer;
    _builtin_set_imask(saved_imask);
    if (fw_update_hostif_flags.scsi == 1) {
        fw_update_scsi_send_status(record);
    } else {
        for (i = 0; i < 13; i++)
            fw_update_reply_buf[i] = fw_update_status_template[i];
        fw_update_reply_byte_9[0] = status;
        fw_update_usb_set_status_len();
        fw_update_usb_send_status(record);
        if (fw_update_restart_request == 0x40)
            fw_update_restart_request = 0x80;
    }
}

struct scsi_reg_scsictl { char p[23]; struct Bits scsictl; };


void fw_update_handle_reselect(struct fw_update_command_link *record)
{
    unsigned char flags;

    fw_update_unqueue_record(record);
    _builtin_set_imask(15);
    flags = *(unsigned char *)&record->status_flags;
    if (flags & 0x90) {
        _builtin_set_imask(0);
        fw_update_end_command();
        return;
    }
    if ((unsigned char)record->retries >= 3) {
        fw_update_set_sense(record, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_SELECT_RESELECT_FAILURE));  /* sense 4/45/00: select or reselect failure */
        _builtin_set_imask(0);
        fw_update_end_command();
        return;
    }
    if (fw_update_record_active.b7) {
        if (record != fw_update_current_record.p) {
            if (record != fw_update_current_record.p->next) {
                record->retries++;
                fw_update_queue_record(fw_update_current_record.p);
                return;
            }
        }
        if (fw_update_hostif_flags.scsi == 1) {
            if (((struct scsi_reg_scsictl *)fw_update_p_scsi_ctrl)->scsictl.b4) {
                fw_update_end_command();
                return;
            }
        } else {
            fw_update_end_command();
            return;
        }
    } else {
        fw_update_set_current_record(record);
    }
    record->saved_handler_outer = record->saved_handler;
    record->saved_handler = record->handler;
    _builtin_set_imask(0);
    if (fw_update_hostif_flags.scsi == 1)
        fw_update_scsi_reselect(record);
    return;
}

void fw_update_end_command(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if ((unsigned char)fw_update_current_record.p->handler == 0x20)
        return;
    fw_update_current_record.p->c54 = 0;
    fw_update_unqueue_record(fw_update_current_record.p);
    _builtin_set_imask(15);
    fw_update_current_record.p->saved_handler_outer = fw_update_current_record.p->saved_handler;
    fw_update_current_record.p->saved_handler = fw_update_current_record.p->handler;
    fw_update_current_record.p->handler = 0x20;
    _builtin_set_imask(saved_imask);
    fw_update_queue_record(fw_update_current_record.p);
}

int fw_update_requeue_record(unsigned char state)
{
    fw_update_unqueue_record(fw_update_current_record.p);
    _builtin_set_imask(15);
    fw_update_current_record.p->saved_handler = state;
    _builtin_set_imask(0);
    if (fw_update_current_record.p->lun_flags.b4) {
        _builtin_set_imask(15);
        fw_update_current_record.p->message_in = 4;
        _builtin_set_imask(0);
        fw_update_scsi_send_message(fw_update_current_record.p);
        return 1;
    }
    _builtin_set_imask(15);
    fw_update_current_record.p->handler = fw_update_current_record.p->saved_handler;
    fw_update_current_record.p->saved_handler = 0;
    _builtin_set_imask(0);
    fw_update_queue_record(fw_update_current_record.p);
    return 0;
}


void fw_update_clear_sense_bufs(void)
{
    fw_update_mem_clear_fast(fw_update_sense_bufs, 0x100);
    fw_update_mem_clear_fast(fw_update_sense_buf_usb, 32);
}

void fw_update_clear_sense(void)
{
    unsigned int sense_buf;

    if (fw_update_hostif_flags.scsi == 1) {
        if (!fw_update_current_record.p->lun_flags.b3)
            return;
        if (fw_update_current_record.p->lun[0])
            return;
    }
    sense_buf = (unsigned int)fw_update_get_sense_buf();
    fw_update_mem_clear_fast(sense_buf, 32);
    fw_update_set_sense(fw_update_current_record.p, 0);
}

void fw_update_set_sense(struct fw_update_command_link *record, int sense)
{
    fw_update_record_set_sense((unsigned char *)record, sense);
    if (fw_update_hostif_flags.scsi == 1) {
        if (!record->lun_flags.b3)
            return;
        if (record->lun[0])
            return;
    }
    fw_update_build_sense((struct fw_update_command_sense_bytes *)record);
}

void fw_update_build_sense(struct fw_update_command_sense_bytes *record)
{
    unsigned char *sense_buf;

    sense_buf = fw_update_get_sense_buf();
    *sense_buf = SCSI_SENSE_CURRENT_ERROR;
    sense_buf[1] = 0;
    sense_buf[2] = record->sense_key;
    sense_buf[12] = record->asc;
    sense_buf[13] = record->ascq;
    sense_buf[7] = 10;
}

void fw_update_record_set_sense(unsigned char *record, int sense)
{
    record[32] = sense >> 16;
    ((short *)record)[17] = sense;
}

unsigned char *fw_update_get_sense_buf(void)
{
    if (fw_update_hostif_flags.scsi == 1) {
        if (fw_update_current_record.p->initiator_id)
            return fw_update_sense_bufs + fw_update_current_record.p->initiator_id * 32;
        else
            return fw_update_sense_buf_usb;
    } else {
        return fw_update_sense_buf_usb;
    }
}

int fw_update_has_sense(void)
{
    unsigned char *sense_buf;
    unsigned char sense_key;

    if (fw_update_hostif_flags.scsi == 1) {
        if (fw_update_current_record.p->lun[0])
            return 0;
    }
    sense_buf = fw_update_get_sense_buf();
    sense_key = sense_buf[2] & 15;
    if (sense_key == 0)
        return 0;
    return 1;
}

unsigned char fw_update_initiator_bit(unsigned char id)
{
    unsigned char bit;

    if (id > 7)
        id = 7;
    bit = 1;
    while (id-- != 0)
        bit <<= 1;
    return bit;
}

unsigned char fw_update_lowest_bit(unsigned char bits)
{
    unsigned char bit;

    for (bit = 0; bit < 7; bit++) {
        if (bits & 1)
            break;
        bits >>= 1;
    }
    return bit;
}

void fw_update_cmd_test_unit_ready(void)
{
    if (fw_update_check_command() == 0) {
        if (fw_update_hostif_flags.usb == 1)
            fw_update_end_command();
        return;
    }
    if ((fw_update_current_record.p->cdb[1] & 0xffffff1f) || fw_update_current_record.p->cdb[2] || fw_update_current_record.p->cdb[3] ||
        fw_update_current_record.p->cdb[4] || fw_update_current_record.p->cdb[5]) {
        fw_update_fail_command();
        return;
    }
    fw_update_end_command();
}

void fw_update_cmd_request_sense(void)
{
    unsigned char alloc_len;

    if ((fw_update_current_record.p->cdb[1] & 0xffffff1f) || fw_update_current_record.p->cdb[2] || fw_update_current_record.p->cdb[3] ||
        fw_update_current_record.p->cdb[5]) {
        if (fw_update_hostif_flags.usb == 1) {
            if (fw_update_current_record.p->cdb[4]) {
                fw_update_current_record.p->transfer_len = fw_update_current_record.p->cdb[4];
                fw_update_current_record.p->data_ptr = fw_update_reply_buf;
                fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
                fw_update_usb_send_status(fw_update_current_record.p);
            } else {
                fw_update_usb_run_data(fw_update_current_record.p);
            }
        }
        fw_update_fail_command();
        return;
    }
    if (fw_update_hostif_flags.scsi == 1) {
        if (fw_update_current_record.p->lun[0] != 0) {
            fw_update_mem_copy(fw_update_reply_buf, fw_update_sense_lun_not_supported, 18);
        } else {
            if (fw_update_check_overlapped() == 0) {
                fw_update_end_command();
                return;
            }
            if (fw_update_has_sense() != 0) {
            } else if (fw_update_check_unit_attention() == 0) {
            } else {
                if (fw_update_take_pending_error() == 0) {
                }
            }
            fw_update_mem_copy(fw_update_reply_buf, fw_update_get_sense_buf(), 18);
            fw_update_clear_sense();
        }
    } else {
        if (fw_update_check_overlapped() == 0) {
            if (fw_update_current_record.p->cdb[4]) {
                fw_update_current_record.p->transfer_len = fw_update_current_record.p->cdb[4];
                fw_update_current_record.p->data_ptr = fw_update_reply_buf;
                fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
                fw_update_usb_send_status(fw_update_current_record.p);
            } else {
                fw_update_usb_run_data(fw_update_current_record.p);
            }
            fw_update_end_command();
            return;
        }
        if (fw_update_has_sense() != 0) {
        } else {
            if (fw_update_take_pending_error() == 0) {
            }
        }
        fw_update_mem_copy(fw_update_reply_buf, fw_update_get_sense_buf(), 18);
        fw_update_clear_sense();
    }
    alloc_len = fw_update_current_record.p->cdb[4];
    if (alloc_len) {
        if (alloc_len > 18)
            alloc_len = 18;
        fw_update_current_record.p->transfer_len = alloc_len;
        fw_update_current_record.p->data_ptr = fw_update_reply_buf;
        if (fw_update_hostif_flags.scsi == 1) {
            fw_update_unqueue_record(fw_update_current_record.p);
            fw_update_scsi_send_data(fw_update_current_record.p);
        } else {
            fw_update_usb_set_transfer_len(alloc_len);
            fw_update_usb_send_status(fw_update_current_record.p);
            fw_update_end_command();
        }
    } else {
        if (fw_update_hostif_flags.usb == 1)
            fw_update_usb_run_data(fw_update_current_record.p);
        fw_update_end_command();
    }
}

void fw_update_cmd_inquiry(void)
{
    if ((fw_update_current_record.p->cdb[1] & 31) != 1) {
        if ((fw_update_current_record.p->cdb[1] & 31) || fw_update_current_record.p->cdb[2] || fw_update_current_record.p->cdb[3] ||
            fw_update_current_record.p->cdb[5])
            {
            if (fw_update_hostif_flags.usb == 1) {
                if (fw_update_current_record.p->cdb[4]) {
                    fw_update_current_record.p->transfer_len = fw_update_current_record.p->cdb[4];
                    fw_update_current_record.p->data_ptr = fw_update_reply_buf;
                    fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
                    fw_update_usb_send_status(fw_update_current_record.p);
                } else {
                    fw_update_usb_run_data(fw_update_current_record.p);
                }
            }
            fw_update_fail_command();
            return;
        }
        if (fw_update_cmd_inquiry_step_5() == 0) {
            if (fw_update_hostif_flags.usb == 1)
                fw_update_usb_run_data(fw_update_current_record.p);
            fw_update_end_command();
            return;
        }
    } else {
        if (fw_update_current_record.p->cdb[3] || fw_update_current_record.p->cdb[5])
            {
            if (fw_update_hostif_flags.usb == 1) {
                if (fw_update_current_record.p->cdb[4]) {
                    fw_update_current_record.p->transfer_len = fw_update_current_record.p->cdb[4];
                    fw_update_current_record.p->data_ptr = fw_update_reply_buf;
                    fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
                    fw_update_usb_send_status(fw_update_current_record.p);
                } else {
                    fw_update_usb_run_data(fw_update_current_record.p);
                }
            }
            fw_update_fail_command();
            return;
        }
        if (fw_update_current_record.p->cdb[2] != SCSI_INQUIRY_VPD_PAGE)
            {
            if (fw_update_hostif_flags.usb == 1) {
                if (fw_update_current_record.p->cdb[4]) {
                    fw_update_current_record.p->transfer_len = fw_update_current_record.p->cdb[4];
                    fw_update_current_record.p->data_ptr = fw_update_reply_buf;
                    fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
                    fw_update_usb_send_status(fw_update_current_record.p);
                } else {
                    fw_update_usb_run_data(fw_update_current_record.p);
                }
            }
            fw_update_fail_command();
            return;
        }
        if (fw_update_build_vpd() == 0) {
            if (fw_update_hostif_flags.usb == 1)
                fw_update_usb_run_data(fw_update_current_record.p);
            fw_update_end_command();
            return;
        }
    }
    fw_update_current_record.p->data_ptr = fw_update_reply_buf;
    if (fw_update_hostif_flags.scsi == 1) {
        fw_update_unqueue_record(fw_update_current_record.p);
        fw_update_scsi_send_data(fw_update_current_record.p);
    } else {
        fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
        fw_update_usb_send_status(fw_update_current_record.p);
        fw_update_end_command();
    }
}

int fw_update_cmd_inquiry_step_5(void)
{
    int i;
    unsigned char alloc_len;

    alloc_len = fw_update_current_record.p->cdb[4];
    if (alloc_len == 0)
        return 0;
    if (alloc_len > 96)
        alloc_len = 96;
    fw_update_current_record.p->transfer_len = alloc_len;
    if (fw_update_hostif_flags.scsi == 1) {
        if (fw_update_current_record.p->lun_flags.b6 && (fw_update_current_record.p->identify & 7) == 0 &&
            (fw_update_current_record.p->cdb[1] & 31) == 0) {
            for (i = 0; i < 96; i++)
                fw_update_reply_buf[i] = fw_update_inquiry_data[i];
            i = 3;
        } else {
            for (i = 0; i < 96; i++)
                fw_update_reply_buf[i] = fw_update_inquiry_invalid_lun[i];
        }
    } else {
        for (i = 0; i < 96; i++)
            fw_update_reply_buf[i] = fw_update_inquiry_data[i];
        i = 3;
    }
    return 1;
}

int fw_update_build_vpd(void)
{
    int i;
    char pad1;
    char pad2;
    unsigned char alloc_len;

    alloc_len = fw_update_current_record.p->cdb[4];
    if (alloc_len == 0)
        return 0;
    if (alloc_len > 100)
        alloc_len = 100;
    fw_update_current_record.p->transfer_len = alloc_len;
    if (fw_update_hostif_flags.scsi == 1) {
        if ((fw_update_current_record.p->lun_flags.b6 && (fw_update_current_record.p->identify & 7) == 0) ||
            (fw_update_current_record.p->cdb[1] & 31) == 0) {
            for (i = 0; i < 100; i++)
                fw_update_reply_buf[i] = fw_update_vpd_data[i];
        } else {
            for (i = 0; i < 30; i++)
                fw_update_reply_buf[i] = fw_update_vpd_invalid_lun[i];
            for (; i < 100; i++)
                fw_update_reply_buf[i] = 0;
        }
    } else {
        for (i = 0; i < 100; i++)
            fw_update_reply_buf[i] = fw_update_vpd_data[i];
    }
    if (fw_update_hostif_flags.usb == 1) {
        /* 1 full speed, 2 high speed */
        if (fw_update_p_usb_ctrl->usb_status.b6)
            fw_update_reply_byte_98[0] = 1;
        else
            fw_update_reply_byte_98[0] = 2;
    }
    return 1;
}

void fw_update_cmd_reserve_unit(void)
{
    if (fw_update_check_command() == 0) {
        if (fw_update_hostif_flags.usb == 1)
            fw_update_end_command();
        return;
    }
    if ((fw_update_current_record.p->cdb[1] & 0x11) || fw_update_current_record.p->cdb[2] || fw_update_current_record.p->cdb[3] ||
        fw_update_current_record.p->cdb[4] || fw_update_current_record.p->cdb[5]) {
        fw_update_fail_command();
        return;
    }
    fw_update_reserved_by = fw_update_current_record.p->initiator_id;
    fw_update_end_command();
}

void fw_update_cmd_release_unit(void)
{
    if (fw_update_check_command() == 0) {
        if (fw_update_hostif_flags.usb == 1)
            fw_update_end_command();
        return;
    }
    if ((fw_update_current_record.p->cdb[1] & 0x11) || fw_update_current_record.p->cdb[2] || fw_update_current_record.p->cdb[3] ||
        fw_update_current_record.p->cdb[4] || fw_update_current_record.p->cdb[5]) {
        fw_update_fail_command();
        return;
    }
    fw_update_reserved_by = 0;
    fw_update_end_command();
}

extern unsigned char *fw_update_onchip_blocks[], *fw_update_ext_sectors[];


/* SEND DIAGNOSTIC (0x1D): 0xFB + mode (0x10 begin, 0x11 end, 0x01 data block, 0x81/0x80 select a report/block,
   0x00 program a block), or "GET FIRMVERSION ". */
void fw_update_cmd_send_diagnostic(void)
{
    int u1;
    int u2;
    unsigned char *p;
    int i;
    unsigned short len;
    unsigned char reserved_bits;
    int ok;
    int expected_len;
    int block_len;
    int d1;
    char d2;
    char d3;
    unsigned char cdb_ok;

    cdb_ok = 1;
    if (fw_update_hostif_flags.scsi == 1) {
        if (fw_update_check_command() == 0)
            return;
    } else {
        cdb_ok = fw_update_check_command();
    }
    reserved_bits = fw_update_current_record.p->cdb[2] | fw_update_current_record.p->cdb[5];
    if ((fw_update_current_record.p->cdb[1] & 0xffffff1b) || reserved_bits) {
        if (fw_update_hostif_flags.scsi == 1) {
            fw_update_fail_command();
            return;
        } else {
            fw_update_set_sense(fw_update_current_record.p, SCSI_SENSE(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB));   /* sense key 5, ASC 0x24: invalid field in CDB */
            fw_update_current_record.p->status_flags.b1 = 1;
            cdb_ok = 0;
        }
    }
    len = fw_update_current_record.p->cdb[4] | fw_update_current_record.p->cdb[3] << 8;
    if (len == 0 || (fw_update_current_record.p->cdb[1] & 4) != 0) {
        fw_update_end_command();
        return;
    }
    fw_update_current_record.p->transfer_len = len;
    fw_update_current_record.p->rx_ptr = fw_update_rx_buf;
    if (fw_update_hostif_flags.scsi == 1) {
        if (fw_update_scsi_receive_data(fw_update_current_record.p) == 0) {
            fw_update_current_record.p->rx_ptr = 0;
            return;
        }
    } else {
        fw_update_usb_start_receive(len);
        if ((cdb_ok = fw_update_usb_run_command(fw_update_current_record.p)) == 0) {
            fw_update_end_command();
            return;
        }
    }
    p = fw_update_rx_buf;
    ok = 1;
    for (;;) {
        if (*p == SCSI_DIAG_TARGET_SDC) {
            p++;
            if (*p == SCSI_DIAG_FLASH_WRITE_START) {
                fw_update_diag_mode.v = *p;
                if (len != 2) {
                    ok = 0;
                    break;
                }
                fw_update_begin_session();
                break;
            } else if (*p == SCSI_DIAG_FLASH_WRITE_END) {
                if (fw_update_session_active == 0) {
                    ok = 0;
                    break;
                }
                if (len != 2) {
                    ok = 0;
                    break;
                }
                fw_update_diag_mode.v = *p;
                fw_update_end_session();
                break;
            } else if (*p == SCSI_DIAG_FLASH_INFO_SEND) {
                if (len != 0x202) {
                    ok = 0;
                    break;
                }
                fw_update_diag_mode.v = *p;
                p++;
                fw_update_mem_clear(fw_update_info_block, 0x200);
                fw_update_mem_copy(fw_update_info_block_buf, p, 0x200);
                fw_update_write_info_block();
                break;
            } else if (*p == SCSI_DIAG_FLASH_READ_INFO) {
                if (len != 4) {
                    ok = 0;
                    break;
                }
                fw_update_diag_mode.v = *p;
                p++;
                fw_update_diag_read_len.v = (unsigned short)((p[0] << 8) | p[1]);
                if (fw_update_diag_read_len.v != 2 && fw_update_diag_read_len.v != 0x202) {
                    ok = 0;
                    break;
                }
                fw_update_diag_request = FW_UPDATE_DIAG_REQUEST_BLOCK_TABLE;
                fw_update_diag_reply_len.v = fw_update_block_table_len;
                break;
            } else if (*p == SCSI_DIAG_FLASH_WRITE) {
                if (fw_update_erased_mask == 0)
                    fw_update_display_step.v = 0;
                fw_update_diag_mode.v = *p;
                p++;
                fw_update_block_index.v = *p;
                p++;
                if (fw_update_block_index.v < 15) {
                    expected_len = fw_update_onchip_blocks[fw_update_block_index.v + 1] - fw_update_onchip_blocks[fw_update_block_index.v];
                } else {
                    reserved_bits = fw_update_block_index.v - 15;
                    expected_len = fw_update_ext_sectors[reserved_bits + 1] - fw_update_ext_sectors[reserved_bits];
                }
                block_len = *p;
                block_len <<= 10;
                fw_update_block_len = block_len;
                p++;
                if (expected_len != block_len) {
                    ok = 0;
                    break;
                }
                if (len != fw_update_block_len + 8) {
                    ok = 0;
                    break;
                }
                for (i = 0; i < 4; i++) {
                    if (*p != 0) {
                        ok = 0;
                        break;
                    }
                    p++;
                }
                if (ok == 0)
                    break;
                fw_update_block_data = p;
                fw_update_write_block();
                break;
            } else if (*p == SCSI_DIAG_FLASH_READ_BLOCK) {
                if (len != 8) {
                    ok = 0;
                    break;
                }
                fw_update_diag_mode.v = *p;
                p++;
                fw_update_block_index.v = *p;
                p++;
                if (fw_update_block_index.v < 15) {
                    expected_len = fw_update_onchip_blocks[fw_update_block_index.v + 1] - fw_update_onchip_blocks[fw_update_block_index.v];
                } else {
                    reserved_bits = fw_update_block_index.v - 15;
                    expected_len = fw_update_ext_sectors[reserved_bits + 1] - fw_update_ext_sectors[reserved_bits];
                }
                block_len = *p;
                block_len <<= 10;
                fw_update_block_len = block_len;
                p++;
                if (expected_len != block_len) {
                    ok = 0;
                    break;
                }
                for (i = 0; i < 4; i++) {
                    if (*p != 0) {
                        ok = 0;
                        break;
                    }
                    p++;
                }
                fw_update_diag_request = FW_UPDATE_DIAG_REQUEST_BLOCK_TABLE;
                fw_update_diag_reply_len.v = block_len;
            }
        }
        if (fw_update_mem_equal(p, fw_update_version_key, 16)) {
            p += 16;
            fw_update_diag_request = FW_UPDATE_DIAG_REQUEST_GET_FIRMVERSION;
            fw_update_diag_reply_len.v = 16;
            break;
        }
        break;
    }
    if (ok == 0) {
        fw_update_fail_command();
        return;
    }
    fw_update_end_command();
}

void fw_update_cmd_read_diagnostic(void)
{
    int unused_frame;
    unsigned char request;
    unsigned short alloc_len;
    int unused_frame_2;

    if (fw_update_check_command() == 0) {
        if (fw_update_hostif_flags.usb == 1) {
            alloc_len = fw_update_current_record.p->cdb[4] | fw_update_current_record.p->cdb[3] << 8;
            if (alloc_len != 0) {
                fw_update_current_record.p->transfer_len = alloc_len;
                fw_update_current_record.p->data_ptr = fw_update_reply_buf;
                fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
                fw_update_usb_send_status(fw_update_current_record.p);
            } else {
                fw_update_usb_run_data(fw_update_current_record.p);
            }
            fw_update_end_command();
        }
        return;
    }
    request = (fw_update_current_record.p->cdb[1] & 4) | fw_update_current_record.p->cdb[2] | fw_update_current_record.p->cdb[5];
    if ((fw_update_current_record.p->cdb[1] & 0xffffff1f) || request) {
        if (fw_update_hostif_flags.usb == 1) {
            alloc_len = fw_update_current_record.p->cdb[4] | fw_update_current_record.p->cdb[3] << 8;
            if (alloc_len != 0) {
                fw_update_current_record.p->transfer_len = alloc_len;
                fw_update_current_record.p->data_ptr = fw_update_reply_buf;
                fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
                fw_update_usb_send_status(fw_update_current_record.p);
            } else {
                fw_update_usb_run_data(fw_update_current_record.p);
            }
        }
        fw_update_fail_command();
        return;
    }
    request = fw_update_diag_request;
    fw_update_diag_request = 0;
    alloc_len = fw_update_current_record.p->cdb[4] | fw_update_current_record.p->cdb[3] << 8;
    if (alloc_len == 0) {
        if (fw_update_hostif_flags.usb == 1)
            fw_update_usb_run_data(fw_update_current_record.p);
        fw_update_end_command();
        return;
    }
    if (alloc_len > fw_update_diag_reply_len.v)
        alloc_len = fw_update_diag_reply_len.v;
    fw_update_current_record.p->transfer_len = alloc_len;
    fw_update_current_record.p->data_ptr = fw_update_reply_buf;
    switch (request) {
    case FW_UPDATE_DIAG_REQUEST_BLOCK_TABLE:
        if (fw_update_diag_mode.v == SCSI_DIAG_FLASH_READ_INFO) {
            fw_update_report_blocks(fw_update_rx_buf);
            fw_update_current_record.p->data_ptr = fw_update_rx_buf;
        } else if (fw_update_diag_mode.v == SCSI_DIAG_FLASH_READ_BLOCK) {
            if (fw_update_block_index.v < 15) {
                fw_update_current_record.p->data_ptr = fw_update_onchip_blocks[fw_update_block_index.v];
            } else if (fw_update_block_index.v < 32) {
                fw_update_current_record.p->data_ptr = fw_update_ext_sectors[fw_update_block_index.v - 15];
            } else {
                if (fw_update_hostif_flags.usb == 1) {
                    alloc_len = fw_update_current_record.p->cdb[4] | fw_update_current_record.p->cdb[3] << 8;
                    fw_update_current_record.p->transfer_len = alloc_len;
                    fw_update_current_record.p->data_ptr = fw_update_reply_buf;
                    fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
                    fw_update_usb_send_status(fw_update_current_record.p);
                }
                fw_update_end_command();
                return;
            }
        }
        break;
    case FW_UPDATE_DIAG_REQUEST_GET_FIRMVERSION:   /* version reply: 4 bytes of the boot header + "0001FFFFFFFF" */
        fw_update_mem_copy(fw_update_reply_buf, ((unsigned char *)&boot_firmware_version), 4);
        fw_update_reply_buf[4] = '0';
        fw_update_reply_buf[5] = '0';
        fw_update_reply_buf[6] = '0';
        fw_update_reply_buf[7] = '1';
        fw_update_reply_buf[8] = 'F';
        fw_update_reply_buf[9] = 'F';
        fw_update_reply_buf[10] = 'F';
        fw_update_reply_buf[11] = 'F';
        fw_update_reply_buf[12] = 'F';
        fw_update_reply_buf[13] = 'F';
        fw_update_reply_buf[14] = 'F';
        fw_update_reply_buf[15] = 'F';
    default:
        if (fw_update_hostif_flags.usb == 1) {
            alloc_len = fw_update_current_record.p->cdb[4] | fw_update_current_record.p->cdb[3] << 8;
            fw_update_current_record.p->transfer_len = alloc_len;
            fw_update_current_record.p->data_ptr = fw_update_reply_buf;
            fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
            fw_update_usb_send_status(fw_update_current_record.p);
        }
        fw_update_end_command();
        return;
    }
    if (fw_update_hostif_flags.scsi == 1) {
        fw_update_unqueue_record(fw_update_current_record.p);
        fw_update_scsi_send_data(fw_update_current_record.p);
    } else {
        fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
        fw_update_usb_send_status(fw_update_current_record.p);
        fw_update_end_command();
    }
}

void fw_update_cmd_get_hw_status(void)
{
    int u;
    unsigned short alloc_len;

    if (fw_update_check_command() == 0) {
        if (fw_update_hostif_flags.usb == 1) {
            alloc_len = fw_update_current_record.p->cdb[8] | fw_update_current_record.p->cdb[7] << 8;
            if (alloc_len != 0) {
                fw_update_current_record.p->transfer_len = alloc_len;
                fw_update_current_record.p->data_ptr = fw_update_reply_buf;
                fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
                fw_update_usb_send_status(fw_update_current_record.p);
            } else {
                fw_update_usb_run_data(fw_update_current_record.p);
            }
            fw_update_end_command();
        }
        return;
    }
    if ((fw_update_current_record.p->cdb[1] & 0x1f) || fw_update_current_record.p->cdb[2] || fw_update_current_record.p->cdb[3] ||
        fw_update_current_record.p->cdb[4] || fw_update_current_record.p->cdb[5] || fw_update_current_record.p->cdb[6] ||
        fw_update_current_record.p->cdb[9]) {
        if (fw_update_hostif_flags.usb == 1) {
            alloc_len = fw_update_current_record.p->cdb[8] | fw_update_current_record.p->cdb[7] << 8;
            if (alloc_len != 0) {
                fw_update_current_record.p->transfer_len = alloc_len;
                fw_update_current_record.p->data_ptr = fw_update_reply_buf;
                fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
                fw_update_usb_send_status(fw_update_current_record.p);
            } else {
                fw_update_usb_run_data(fw_update_current_record.p);
            }
        }
        fw_update_fail_command();
        return;
    }
    alloc_len = fw_update_current_record.p->cdb[8] | fw_update_current_record.p->cdb[7] << 8;
    if (alloc_len == 0) {
        if (fw_update_hostif_flags.usb == 1)
            fw_update_usb_run_data(fw_update_current_record.p);
        fw_update_end_command();
        return;
    }
    if (alloc_len > 48)
        alloc_len = 48;
    fw_update_current_record.p->transfer_len = alloc_len;
    fw_update_mem_clear(fw_update_reply_buf, 48);
    fw_update_reply_buf[0] = 0;
    fw_update_reply_byte_1[0] = 0;
    fw_update_reply_byte_2[0] = 0;
    fw_update_reply_byte_3[0] = 0x80;
    fw_update_reply_byte_4[0] = 0;
    fw_update_reply_byte_5[0] = 0;
    fw_update_reply_byte_6[0] = 0;
    fw_update_reply_byte_7[0] = 0;
    if (fw_update_state_flags.b4)
        fw_update_reply_byte_7[0] = 1;
    fw_update_reply_byte_8[0] = 0;
    fw_update_reply_byte_10[0] = 0;
    fw_update_reply_byte_11[0] = 0;
    fw_update_current_record.p->data_ptr = fw_update_reply_buf;
    if (fw_update_hostif_flags.scsi == 1) {
        fw_update_unqueue_record(fw_update_current_record.p);
        fw_update_scsi_send_data(fw_update_current_record.p);
    } else {
        fw_update_usb_set_transfer_len(fw_update_current_record.p->transfer_len);
        fw_update_usb_send_status(fw_update_current_record.p);
        fw_update_end_command();
    }
}

void fw_update_fail_command(void)
{
    fw_update_set_sense(fw_update_current_record.p, SCSI_SENSE(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELD_IN_CDB));
    fw_update_current_record.p->status_flags.b1 = 1;
    fw_update_end_command();
}

int fw_update_check_command(void)
{
    if (fw_update_hostif_flags.scsi == 1) {
        if (fw_update_check_reservation() == 0) {
            if (fw_update_current_record.p->cdb[0] != 23)
                fw_update_current_record.p->status_flags.b3 = 1;
            fw_update_current_record.p->status_flags.b1 = 1;
            fw_update_end_command();
            return 0;
        }
        if (fw_update_check_unit_attention() == 0) {
            fw_update_current_record.p->status_flags.b1 = 1;
            fw_update_end_command();
            return 0;
        }
    }
    if (fw_update_take_pending_error() == 0) {
        fw_update_current_record.p->status_flags.b1 = 1;
        if (fw_update_hostif_flags.scsi == 1)
            fw_update_end_command();
        return 0;
    }
    if (fw_update_check_ready() == 0) {
        fw_update_current_record.p->status_flags.b1 = 1;
        if (fw_update_hostif_flags.scsi == 1)
            fw_update_end_command();
        return 0;
    }
    return 1;
}

int fw_update_check_reservation(void)
{
    if (fw_update_current_record.p->lun[0] != 0)
        return 1;
    if (!fw_update_reserved_by)
        return 1;
    else if (*(volatile unsigned char *)&fw_update_reserved_by == fw_update_current_record.p->initiator_id)
        return 1;
    return 0;
}

int fw_update_check_overlapped(void)
{
    if (fw_update_current_record.p->status_flags.b2 == 0)
        return 1;
    if (fw_update_hostif_flags.scsi == 1) {
        if (fw_update_current_record.p->lun[0] != 0) {
            fw_update_current_record.p->status_flags.b2 = 0;
            return 1;
        }
    }
    if (fw_update_state_flags.b7) {
        if (fw_update_current_record.p->cdb[0] == 3 || fw_update_current_record.p->cdb[0] == 18) {
            fw_update_current_record.p->status_flags.b2 = 0;
            return 1;
        }
        fw_update_build_sense((struct fw_update_command_sense_bytes *)fw_update_current_record.p);
        return 0;
    }
    if (fw_update_disconnected_record.p->status_flags.b7) {
        fw_update_current_record.p->status_flags.b2 = 0;
        return 1;
    }
    if (fw_update_current_record.p == fw_update_disconnected_record.p && fw_update_disconnected_record.p->disconnect_flags.b4) {
        fw_update_current_record.p->status_flags.b4 = 1;
        fw_update_disconnected_record.p->status_flags.b7 = 1;
        fw_update_abort_disconnected();
        fw_update_set_sense(fw_update_current_record.p, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_OVERLAPPED_COMMANDS));
        return 0;
    }
    return 1;
}

int fw_update_abort_disconnected(void)
{
    if (fw_update_disconnected_record.p == 0 || !fw_update_current_record.p->lun_flags.b3 || fw_update_disconnected_record.p->status_flags.b7 ||
        fw_update_current_record.p != fw_update_disconnected_record.p)
        return 1;
    if (fw_update_hostif_flags.scsi == 1 && fw_update_current_record.p->lun[0] != 0)
        return 1;
    fw_update_disconnected_record.p->status_flags.b7 = 1;
    fw_update_command_flags.b0 = 1;
    return 0;
}

int fw_update_check_unit_attention(void)
{
    unsigned char initiator_bit;

    if (fw_update_current_record.p->lun[0] != 0)
        return 1;
    initiator_bit = fw_update_initiator_bit(fw_update_current_record.p->initiator_id);
    if (initiator_bit) {
        if (initiator_bit & fw_update_ua_reset_pending) {
            fw_update_ua_reset_pending &= ~initiator_bit;
            fw_update_set_sense(fw_update_current_record.p, SCSI_SENSE(SCSI_SENSE_UNIT_ATTENTION, SCSI_ASC_POWER_ON_RESET));
            return 0;
        }
        if (initiator_bit & fw_update_ua_mode_changed) {
            fw_update_ua_mode_changed &= ~initiator_bit;
            fw_update_set_sense(fw_update_current_record.p, SCSI_SENSE(SCSI_SENSE_UNIT_ATTENTION, SCSI_ASC_MODE_PARAMETERS_CHANGED));
            return 0;
        }
    } else {
        if (fw_update_ua_reset_pending_usb) {
            fw_update_ua_reset_pending_usb = 0;
            fw_update_set_sense(fw_update_current_record.p, SCSI_SENSE(SCSI_SENSE_UNIT_ATTENTION, SCSI_ASC_POWER_ON_RESET));
            return 0;
        }
        if (fw_update_ua_mode_changed_usb) {
            fw_update_ua_mode_changed_usb = 0;
            fw_update_set_sense(fw_update_current_record.p, SCSI_SENSE(SCSI_SENSE_UNIT_ATTENTION, SCSI_ASC_MODE_PARAMETERS_CHANGED));
            return 0;
        }
    }
    return 1;
}
int fw_update_check_ready(void)
{
    if (fw_update_state_flags.b7)
        return 1;
    if (fw_update_current_record.p->cdb[0] == 22 || fw_update_current_record.p->cdb[0] == 23)  /* RESERVE, RELEASE */
        return 1;
    fw_update_set_sense(fw_update_current_record.p, SCSI_SENSE(SCSI_SENSE_NOT_READY, SCSI_ASC_NONE));  /* NOT READY, ASC 00 */
    return 0;
}

union fw_update_scsi_byte_view {
    struct { unsigned char h; struct Bits l; } b;
};
#define VB(x) (*(union fw_update_scsi_byte_view *)&(x))
struct fw_update_command_sense_word {
    char p[32];
    unsigned char sense_key;
    char pad_21;
    unsigned short asc_ascq;
};

int fw_update_take_pending_error(void)
{
    struct fw_update_sense_entry *entry;
    unsigned short errors;
    unsigned char opcode;

    errors = fw_update_pending_errors;
    opcode = fw_update_current_record.p->cdb[0];
    if ((errors & 0x120) == 0)
        return 1;
    if ((errors & 0xc0) != 0 && opcode != 3) {
        if (opcode == 40 && fw_update_current_record.p->cdb[2] != 0)
            return 1;
        if (opcode == 29 && (fw_update_current_record.p->cdb[1] & 4)) {
        } else {
            if (opcode != 40 && opcode != 49 && opcode != 27)
                return 1;
            if ((errors & 0x80) && fw_update_pending_code_1 == 32) {
                if (opcode != 40)
                    return 1;
            }
        }
    }
    fw_update_reported_code_1 = fw_update_pending_code_1;
    fw_update_reported_code_2 = fw_update_pending_code_2;
    fw_update_reported_code_3 = fw_update_pending_code_3;
    fw_update_pending_errors = 0;
    fw_update_pending_code_1 = 0;
    fw_update_pending_code_2 = 0;
    fw_update_pending_code_3 = 0;
    if (VB(errors).b.l.b7) {
        errors = fw_update_reported_code_1;
        if (errors == 1) {
            ((struct fw_update_command_sense_word *)fw_update_current_record.p)->sense_key = 4;
            ((struct fw_update_command_sense_word *)fw_update_current_record.p)->asc_ascq = fw_update_error_detail;
            return 0;
        }
        for (entry = fw_update_hardware_sense_table;; entry++) {
            if ((errors & entry->error_mask) != 0 || entry->error_mask == 0)
                break;
        }
        ((struct fw_update_command_sense_word *)fw_update_current_record.p)->sense_key = entry->v >> 16;
        ((struct fw_update_command_sense_word *)fw_update_current_record.p)->asc_ascq = entry->v;
    } else {
        if (VB(errors).b.l.b6) {
            errors = fw_update_reported_code_2;
            for (entry = fw_update_medium_sense_table;; entry++) {
                if ((errors & entry->error_mask) != 0 || entry->error_mask == 0)
                    break;
            }
            ((struct fw_update_command_sense_word *)fw_update_current_record.p)->sense_key = entry->v >> 16;
        } else {
            ((struct fw_update_command_sense_word *)fw_update_current_record.p)->sense_key = 5;
            ((struct fw_update_command_sense_word *)fw_update_current_record.p)->asc_ascq = 0;
            if (fw_update_reported_code_3 == 8)
                ((struct fw_update_command_sense_word *)fw_update_current_record.p)->asc_ascq = 0x2600;
        }
    }
    fw_update_build_sense((struct fw_update_command_sense_bytes *)fw_update_current_record.p);
    return 0;
}

/* the command tables, INQUIRY data and strings */
/* command code -> handler, ended by {0, 0} */
const struct fw_update_cmd_entry fw_update_cmd_table[9] = {
    {SCSI_OP_TEST_UNIT_READY, {0}, fw_update_cmd_test_unit_ready}, {SCSI_OP_REQUEST_SENSE, {0}, fw_update_cmd_request_sense}, {SCSI_OP_INQUIRY, {0}, fw_update_cmd_inquiry},
    {SCSI_OP_RESERVE_UNIT, {0}, fw_update_cmd_reserve_unit}, {SCSI_OP_RELEASE_UNIT, {0}, fw_update_cmd_release_unit}, {SCSI_OP_SEND_DIAGNOSTIC, {0}, fw_update_cmd_send_diagnostic},
    {SCSI_OP_READ_DIAGNOSTIC, {0}, fw_update_cmd_read_diagnostic}, {SCSI_OP_GET_HW_STATUS, {0}, fw_update_cmd_get_hw_status}, {0x00, {0}, 0}
};
const struct fw_update_sense_entry fw_update_hardware_sense_table[11] = {
    {0x0200, 0x0000, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_HARDWARE_IMPRINTER)}, {0x0100, 0x0000, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_HARDWARE_OPTION)},
    {0x0080, 0x0000, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_HARDWARE_MECHANICAL)}, {0x0040, 0x0000, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_HARDWARE_OPTICAL)},
    {0x0010, 0x0000, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_HARDWARE_LAMP_FUSE)}, {0x0008, 0x0000, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_HARDWARE_MOTOR_CIRCUIT)},
    {0x0004, 0x0000, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_HARDWARE_MOTOR_FUSE)}, {0x0002, 0x0000, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_HARDWARE_FAN)},
    {0x0001, 0x0000, SCSI_SENSE(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_INTERNAL_TARGET_FAILURE)}, {0x0020, 0x0000, SCSI_SENSE(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_ABORTED_IMAGE_TRANSFER)},
    {0x0000, 0x0000, 0x00000000}
};
const struct fw_update_sense_entry fw_update_medium_sense_table[8] = {
    {0x0080, 0x0000, SCSI_SENSE(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_NONE)}, {0x0040, 0x0000, SCSI_SENSE(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_ABORTED_IMAGE_TRANSFER)},
    {0x0020, 0x0000, SCSI_SENSE(SCSI_SENSE_MEDIUM_ERROR, SCSI_ASC_MEDIUM_NO_CARTRIDGE)}, {0x0008, 0x0000, SCSI_SENSE(SCSI_SENSE_MEDIUM_ERROR, SCSI_ASC_MEDIUM_IMPRINTER)},
    {0x0004, 0x0000, SCSI_SENSE(SCSI_SENSE_MEDIUM_ERROR, SCSI_ASC_MEDIUM_COVER_OPEN)}, {0x0002, 0x0000, SCSI_SENSE(SCSI_SENSE_MEDIUM_ERROR, SCSI_ASC_MEDIUM_HOPPER_EMPTY)},
    {0x0001, 0x0000, SCSI_SENSE(SCSI_SENSE_MEDIUM_ERROR, SCSI_ASC_MEDIUM_PAPER_JAM)}, {0x0000, 0x0000, SCSI_SENSE(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_NONE)}
};
const unsigned char fw_update_sense_lun_not_supported[68] = {
    0xf0, 0x00, 0x45, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00,
    0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
/* INQUIRY data (8-byte header, vendor, product, revision, ...) */
const unsigned char fw_update_inquiry_data[112] = {
    0x06, 0x00, 0x02, 0x02, 0x5b, 0x00, 0x00, 0x10,
    'F', 'U', 'J', 'I', 'T', 'S', 'U', ' ', 'f', 'i', '-', '6',  /* vendor, product, revision */
    '7', '7', '0', 'd', 'j', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    '0', '0', '0', '1',
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0d, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
/* INQUIRY data for an invalid LUN (peripheral qualifier 3, type 0x1F) */
const unsigned char fw_update_inquiry_invalid_lun[112] = {
    0x7f, 0x00, 0x02, 0x02, 0x5b, 0x00, 0x00, 0x10,
    'F', 'U', 'J', 'I', 'T', 'S', 'U', ' ', 'I', 'N', 'V', 'A',  /* the invalid LUN reply */
    'L', 'I', 'D', ' ', 'L', 'U', 'N', ' ', ' ', ' ', ' ', ' ',
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
/* split at fw_update_vpd_invalid_lun (fw_update_scsi.c names it) */
const unsigned char fw_update_vpd_data[111] = {
    0x06, 0xf0, 0x02, 0x00, 0x5f, 0x02, 0x58, 0x02, 0x58, 0x11, 0x02, 0x58,
    0x02, 0x58, 0x00, 0x32, 0x00, 0x32, 0xff, 0xfc, 0x00, 0x00, 0x1c, 0x80,
    0x00, 0x00, 0x4f, 0xb0, 0x8f, 0x00, 0x00, 0x00, 0xd2, 0x0a, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xef, 0xbf, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
    0xff, 0x00, 0x42, 0x32, 0xbd, 0x40, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00
};
const unsigned char fw_update_vpd_invalid_lun[46] = {
    0x7f, 0xf0, 0x02, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char fw_update_status_template[15] = {
    0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00
};
const unsigned char fw_update_version_key[20] = "GET FIRMVERSION ";
