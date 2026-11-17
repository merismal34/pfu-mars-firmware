#include "boot.h"
#pragma section 2
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
/* Handler table of fw_update_scsi_ctrl.c (section D). */
struct fw_update_command_link;

int (*fw_update_scsi_event_table[32])(struct fw_update_command_link *) = {
    fw_update_scsi_event_00,
    fw_update_scsi_bus_reset,
    fw_update_scsi_selected,
    fw_update_scsi_disconnected,
    fw_update_scsi_release_bus,
    fw_update_scsi_reselected,
    /* [6..15] */
    fw_update_scsi_event_default, fw_update_scsi_event_default, fw_update_scsi_event_default, fw_update_scsi_event_default, fw_update_scsi_event_default, fw_update_scsi_event_default, fw_update_scsi_event_default, fw_update_scsi_event_default,
    fw_update_scsi_event_default, fw_update_scsi_event_default,
    fw_update_scsi_receive_cdb,
    fw_update_scsi_event_default,
    fw_update_scsi_data_in_done,
    fw_update_scsi_data_out_done,
    fw_update_scsi_event_default, fw_update_scsi_event_default,
    fw_update_scsi_message_in_done,
    fw_update_scsi_event_default,
    fw_update_scsi_receive_message,
    fw_update_scsi_event_default,
    fw_update_scsi_event_1a,
    fw_update_scsi_read_opcode,
    fw_update_scsi_event_default, fw_update_scsi_event_default, fw_update_scsi_event_default, fw_update_scsi_event_default
};


void fw_update_dispatch_scsi_event(void)
{
    if (fw_update_record_active.b7) {
        fw_update_scsi_read_status(fw_update_current_record.p);
        fw_update_scsi_ack_interrupts();
        if (fw_update_current_record.p->scsiint2.b.b6) {
            fw_update_scsi_bus_reset(fw_update_current_record.p);
            return;
        }
        if (fw_update_current_record.p->scsiint2.b.b3) {
            fw_update_scsi_hook_cmd_error(fw_update_current_record.p);
            return;
        }
        if (fw_update_current_record.p->mainint.b.b6) {
            return;
        }
        if (fw_update_current_record.p->mainint.b.b1) {
            if (fw_update_dma_armed) {
                fw_update_dma_done = 1;
            }
        }
        /* phase 24: message out */
        if (fw_update_scsi_phase.v == 0x18) {
            fw_update_scsi_receive_message(fw_update_current_record.p);
        } else {
            fw_update_current_record.p->message_out_len = 0;
            fw_update_current_record.p->c3b = 0;
            fw_update_current_record.p->c38 = 0;
            fw_update_current_record.p->phase_flags.c &= 0xcf;
            if (fw_update_scsi_phase.v < 0x20) {
                fw_update_scsi_event_table[fw_update_scsi_phase.v](fw_update_current_record.p);
            } else {
                if (!fw_update_scsi_event_unknown(fw_update_current_record.p)) {
                }
            }
        }
    } else {
        fw_update_scsi_selected();
    }
}

void fw_update_scsi_event_00(int record)
{
    fw_update_scsi_ack_interrupts();
}

void fw_update_scsi_ack_interrupts(void)
{
    int saved_imask;
    unsigned char status;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    status = fw_update_p_scsi_ctrl->mainint.c;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->mainint.c = status;
    fw_update_sync_dst = fw_update_sync_src;
    status = fw_update_p_scsi_ctrl->scsiint1;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->scsiint1 = status;
    fw_update_sync_dst = fw_update_sync_src;
    status = fw_update_p_scsi_ctrl->scsiint2.c;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->scsiint2.c = status;
    _builtin_set_imask(saved_imask);
}

void fw_update_scsi_event_default(int record)
{
    fw_update_scsi_ack_interrupts();
}

void fw_update_scsi_read_status(struct fw_update_command_link *record)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    record->mainint.c = fw_update_p_scsi_ctrl->mainint.c;
    fw_update_sync_dst = fw_update_sync_src;
    record->scsiint1.c = fw_update_p_scsi_ctrl->scsiint1;
    fw_update_sync_dst = fw_update_sync_src;
    record->scsiint2.c = fw_update_p_scsi_ctrl->scsiint2.c;
    fw_update_sync_dst = fw_update_sync_src;
    record->scsictl = fw_update_p_scsi_ctrl->scsictl.c;
    fw_update_sync_dst = fw_update_sync_src;
    record->command = fw_update_p_scsi_ctrl->command;
    _builtin_set_imask(saved_imask);
}

int fw_update_scsi_event_unknown(struct fw_update_command_link *record)
{
    return 1;
}

int fw_update_scsi_hook_cmd_error(struct fw_update_command_link *record)
{
}

void fw_update_scsi_bus_reset(struct fw_update_command_link *record)
{
    fw_update_sync_dst = fw_update_sync_src;
    while (fw_update_p_scsi_ctrl->scsiint2.b.b6) {
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_scsi_ack_interrupts();
    }
    fw_update_scsi_release_bus(record);
    fw_update_scsi_phase.v = 0;
    _builtin_set_imask(15);
    fw_update_bus_reset_seen.b7 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->reset = 7;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->scsimode0.b7 = 0;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->scsimode0.b6 = 1;
    fw_update_restart_arg1 = 0;
    fw_update_restart_arg2 = 0;
    fw_update_entry();
}

void fw_update_scsi_release_bus(struct fw_update_command_link *record)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (fw_update_current_record.p != fw_update_disconnected_record.p) {
        fw_update_release_record(record);
    } else {
        fw_update_clear_current_record(record);
    }
    _builtin_set_imask(15);
    if (fw_update_restart_request == 64) {
        fw_update_restart_request = 128;
    }
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->fifoctl.b2 = 1;
    fw_update_scsi_phase.v = 4;
    fw_update_scsi_ctrl_command(5);
    fw_update_sync_dst = fw_update_sync_src;
    while (fw_update_p_scsi_ctrl->mainint.b.b5) {
    }
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->scsimode1.b5 = 1;
    fw_update_scsi_ctrl_command(12);
    _builtin_set_imask(saved_imask);
}

void fw_update_scsi_disconnected(struct fw_update_command_view_1 *record)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    record->handler = record->saved_handler;
    record->saved_handler = 0;
    fw_update_disconnected_record.p->disconnect_flags.b4 = 1;
    fw_update_disconnected_record.p->status_flags.b5 = 0;
    _builtin_set_imask(saved_imask);
    fw_update_queue_record(fw_update_current_record.p);
}

void fw_update_scsi_selected(void)
{
    struct fw_update_command_link *record;
    unsigned char initiator_id;
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    record = fw_update_alloc_record();
    if (fw_update_disconnected_record.p != 0 && !fw_update_disconnected_record.p->status_flags.b7) {
        fw_update_sync_dst = fw_update_sync_src;
        initiator_id = fw_update_p_scsi_ctrl->sdid & 0x70;
        initiator_id >>= 4;
        if (fw_update_disconnected_record.p->initiator_id == initiator_id) {
            record->next = fw_update_disconnected_record.p;
        }
    }
    fw_update_scsi_read_status(record);
    fw_update_scsi_ack_interrupts();
    fw_update_set_current_record(record);
    _builtin_set_imask(saved_imask);
    fw_update_scsi_phase.v = 2;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->scsimode1.b5 = 0;
    if (record->scsiint2.b.b6) {
        fw_update_scsi_bus_reset(record);
        return;
    }
    if (record->scsiint1.b.b6) {
        _builtin_set_imask(15);
        fw_update_scsi_phase.v = 4;
        fw_update_scsi_ctrl_command(5);
        _builtin_set_imask(saved_imask);
        fw_update_scsi_release_bus(record);
        return;
    }
    fw_update_sync_dst = fw_update_sync_src;
    initiator_id = fw_update_p_scsi_ctrl->sdid & 0x70;
    initiator_id >>= 4;
    record->initiator_id = initiator_id;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
        if (record->scsiint1.b.b4) {
            record->identify = fw_update_p_scsi_ctrl->fifodata;
            fw_update_scsi_take_identify();
            record->message_out_len = 0;
            record->c3b = 0;
            record->c38 = 0;
        } else {
            record->message_out = fw_update_p_scsi_ctrl->fifodata;
            record->lun_flags.b6 = 0;
            if (record->message_out == 1) {
                fw_update_scsi_message[0] = record->message_out;
                record->message_out_len++;
            }
        }
        fw_update_scsi_prev_phase.v = fw_update_scsi_phase.v;
        fw_update_scsi_phase.v = 24;
        fw_update_p_scsi_ctrl->ndmasize = 1;
        fw_update_scsi_ctrl_command(25);
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        initiator_id = fw_update_p_scsi_ctrl->fifodata;
        if (record->scsiint1.b.b0 == 0) {
            record->identify = initiator_id;
            fw_update_scsi_take_identify();
            fw_update_sync_dst = fw_update_sync_src;
            record->cdb[0] = fw_update_p_scsi_ctrl->fifodata;
        } else {
            record->cdb[0] = initiator_id;
        }
        record->phase_flags.b.b1 = 1;
        if (record->cdb[0] == SCSI_OP_ENDORSER || record->cdb[0] == SCSI_OP_GET_HW_STATUS || record->cdb[0] == SCSI_OP_SCANNER_CONTROL) {
            _builtin_set_imask(15);
            fw_update_scsi_phase.v = 16;
            _builtin_set_imask(saved_imask);
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_scsi_ctrl->ndmasize = 9;
            fw_update_scsi_ctrl_command(18);
        } else {
            fw_update_scsi_phase.v = 16;
            fw_update_scsi_receive_cdb(record);
        }
    }
    return;
}

void fw_update_scsi_read_opcode(struct fw_update_command_link *record)
{
    unsigned char cdb_len;
    int saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_sync_dst = fw_update_sync_src;
    cdb_len = fw_update_p_scsi_ctrl->fifodata;
    record->cdb[0] = cdb_len;
    record->phase_flags.b.b1 = 1;
    fw_update_scsi_cdb_len(record, &cdb_len);
    _builtin_set_imask(15);
    fw_update_scsi_phase.v = 16;
    _builtin_set_imask(saved_imask);
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->ndmasize = cdb_len + 255;
    fw_update_scsi_ctrl_command(18);
}

void fw_update_scsi_receive_cdb(struct fw_update_command_link *record)
{
    unsigned char cdb_len;
    unsigned char i;
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (record->phase_flags.b.b1 == 0) {
        fw_update_sync_dst = fw_update_sync_src;
        record->cdb[0] = fw_update_p_scsi_ctrl->fifodata;
    }
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_scsi_ctrl->fifoctl.b0) {
        fw_update_build_sense(record);
        record->status_flags.b1 = 1;
        record->handler = 32;
        fw_update_queue_record(record);
        return;
    }
    if (record->phase_flags.b.b3 && record->scsiint1.b.b7) {
    }
    fw_update_scsi_cdb_len(record, &cdb_len);
    for (i = 1; i < cdb_len; i++) {
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_scsi_ctrl->fifoctl.b0) {
        }
        fw_update_sync_dst = fw_update_sync_src;
        record->cdb[i] = fw_update_p_scsi_ctrl->fifodata;
    }
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->fifoctl.b2 = 1;
    if (!record->lun_flags.b6) {
        record->lun[0] = record->cdb[1] & 7;
        record->lun_flags.b3 = 1;
    }
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
        if (record->lun_flags.b5) {
            fw_update_scsi_prev_phase.v = fw_update_scsi_phase.v;
            fw_update_scsi_phase.v = 24;
            fw_update_scsi_ctrl_command(25);
            return;
        }
    } else {
        record->handler = 35;
        _builtin_set_imask(15);
        fw_update_queue_record(record);
        _builtin_set_imask(saved_imask);
    }
}

void fw_update_scsi_reselected(struct fw_update_command_link *record)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (!record->mainint.b.b7) {
        if (record->scsiint2.b.b0 != 0) {
            fw_update_build_sense(record);
            fw_update_clear_current_record(record);
            record->retries++;
            record->handler = 33;
            _builtin_set_imask(15);
            fw_update_queue_record(record);
            _builtin_set_imask(saved_imask);
            return;
        }
        if (record->scsiint2.b.b1) {
            fw_update_build_sense(record);
            fw_update_clear_current_record(record);
            record->retries++;
            record->handler = 33;
            _builtin_set_imask(15);
            fw_update_queue_record(record);
            _builtin_set_imask(saved_imask);
            return;
        }
        if (record->scsiint1.b.b5) {
            record->retries++;
            record->handler = 33;
            fw_update_queue_record(record);
            return;
        }
    }
    if (record->phase_flags.b.b3) {
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_scsi_ctrl->scsimode1.b0 = 1;
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_scsi_ctrl->scsimode1.b0 = 0;
    }
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->fifoctl.b2 = 1;
    fw_update_scsi_phase.v = 22;
    record->message_in = 0x80; /* IDENTIFY message, LUN 0 */
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->fifodata = 0x80;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->ndmasize = 1;
    fw_update_scsi_ctrl_command(24);
    return;
}

void fw_update_scsi_data_out_done(struct fw_update_command_link *record)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_scsi_ctrl->fifoctl.b0 == 0) {
        if (record->phase_flags.b.b3 && record->scsiint1.b.b7) {
            return;
        }
        record->data_out_count--;
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
            if (record->lun_flags.b5) {
                fw_update_scsi_prev_phase.v = fw_update_scsi_phase.v;
                fw_update_scsi_phase.v = 24;
                fw_update_scsi_ctrl_command(25);
                return;
            }
        } else {
            if (record->data_out_count == 0) {
                record->handler = 37;
                _builtin_set_imask(15);
                fw_update_queue_record(record);
                _builtin_set_imask(saved_imask);
            } else {
                fw_update_scsi_ctrl_command(20);
            }
        }
    } else {
        fw_update_build_sense(record);
        return;
    }
}


void fw_update_scsi_data_in_done(struct fw_update_command_link *record)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_scsi_ctrl->fifoctl.b0) {
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
            if (record->lun_flags.b5) {
                fw_update_scsi_prev_phase.v = fw_update_scsi_phase.v;
                fw_update_scsi_phase.v = 24;
                fw_update_scsi_ctrl_command(25);
                return;
            }
        }
        fw_update_end_command();
    } else {
        fw_update_build_sense(record);
        fw_update_scsi_phase.v = 4;
        fw_update_scsi_ctrl_command(5);
        fw_update_scsi_release_bus(record);
        return;
    }
}

void fw_update_scsi_event_1a(struct fw_update_command_link *record)
{
    fw_update_scsi_phase.v = 4;
    fw_update_scsi_ctrl_command(5);
    fw_update_scsi_release_bus(record);
    return;
}

void fw_update_scsi_receive_message(struct fw_update_command_link *record)
{
    int u1;
    int u2;
    int u3;
    unsigned char answer;
    int saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_scsi_ctrl->fifoctl.b0 == 0) {
        if (record->phase_flags.b.b3 && record->scsiint1.b.b7) {
        }
        if (record->message_out_len == 0) {
            fw_update_sync_dst = fw_update_sync_src;
            record->message_out = fw_update_p_scsi_ctrl->fifodata;
            fw_update_scsi_message[record->message_out_len] = record->message_out;
        }
        switch (record->message_out) {
        case 1:
            if (record->message_out_len == 0) {
                record->message_out_len++;
            } else {
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_scsi_message[record->message_out_len] = fw_update_p_scsi_ctrl->fifodata;
                record->message_out_len++;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                fw_update_scsi_next_message();
            } else {
                answer = fw_update_scsi_negotiate_sync(record);
                switch (answer) {
                case 1:
                case 3:
                    record->message_in = 1;
                    break;
                case 7:
                    record->message_in = 7;
                }
                fw_update_scsi_prev_phase.v = 24;
                fw_update_scsi_phase.v = 22;
                fw_update_scsi_send_message(record);
            }
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            if (fw_update_scsi_prev_phase.v == 22) {
                switch (record->message_in) {
                case 1:
                    break;
                case 2:
                case 4:
                    break;
                case 3:
                    if (record->lun_flags.b3) {
                        record->sense_key = 11;
                        if (fw_update_scsi_message_from.v != 26) {
                            record->status_flags.b1 = 1;
                            fw_update_build_sense(record);
                            record->handler = 32;
                            _builtin_set_imask(15);
                            fw_update_queue_record(record);
                            _builtin_set_imask(saved_imask);
                        } else {
                            fw_update_scsi_phase.v = 4;
                            fw_update_scsi_ctrl_command(5);
                            fw_update_scsi_release_bus(record);
                        }
                    } else {
                        record->sense_key = 11;
                        fw_update_scsi_phase.v = 4;
                        fw_update_scsi_ctrl_command(5);
                        fw_update_scsi_release_bus(record);
                    }
                    break;
                case 7:
                    if (record->lun_flags.b3) {
                        fw_update_build_sense(record);
                        fw_update_scsi_phase.v = 32;
                        _builtin_set_imask(15);
                        fw_update_queue_record(record);
                        _builtin_set_imask(saved_imask);
                    } else {
                        fw_update_scsi_phase.v = 4;
                        fw_update_scsi_ctrl_command(5);
                        fw_update_scsi_release_bus(record);
                    }
                    break;
                default:
                    if (record->lun_flags.b6) {
                        fw_update_build_sense(record);
                        fw_update_scsi_phase.v = 4;
                        fw_update_scsi_ctrl_command(5);
                        fw_update_scsi_release_bus(record);
                    } else {
                        if (record->lun_flags.b3) {
                            fw_update_build_sense(record);
                        }
                        fw_update_scsi_phase.v = 4;
                        fw_update_scsi_ctrl_command(5);
                        fw_update_scsi_release_bus(record);
                    }
                }
            } else {
                if (record->lun_flags.b3) {
                    record->status_flags.b1 = 1;
                    fw_update_build_sense(record);
                    fw_update_scsi_phase.v = 32;
                    _builtin_set_imask(15);
                    fw_update_queue_record(record);
                    _builtin_set_imask(saved_imask);
                } else {
                    fw_update_scsi_phase.v = 4;
                    fw_update_scsi_ctrl_command(5);
                    fw_update_scsi_release_bus(record);
                }
            }
            break;
        case 8:
            break;
        case 9:
            break;
        default:
            if (record->lun_flags.b6) {
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                    if (fw_update_scsi_prev_phase.v != 24 || fw_update_scsi_prev_phase.v != 22) {
                        fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                    }
                    fw_update_scsi_prev_phase.v = 22;
                    fw_update_scsi_phase.v = 24;
                    fw_update_scsi_ctrl_command(25);
                } else {
                    fw_update_scsi_phase.v = 32;
                    _builtin_set_imask(15);
                    fw_update_queue_record(record);
                    _builtin_set_imask(saved_imask);
                }
            } else {
                fw_update_build_sense(record);
                fw_update_scsi_phase.v = 4;
                fw_update_scsi_ctrl_command(5);
                fw_update_scsi_release_bus(record);
                return;
            }
        }
    } else {
        fw_update_build_sense(record);
        fw_update_scsi_phase.v = 4;
        fw_update_scsi_ctrl_command(5);
        fw_update_scsi_release_bus(record);
    }
}

void fw_update_scsi_message_in_done(struct fw_update_command_link *record)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_scsi_ctrl->fifoctl.b0) {
    again:
        switch (record->message_in) {
        case 0:
            fw_update_scsi_phase.v = 4;
            fw_update_scsi_ctrl_command(5);
            fw_update_scsi_release_bus(record);
            break;
        case 1:
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                if (fw_update_scsi_prev_phase.v != 24 || fw_update_scsi_prev_phase.v != 22) {
                    fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                }
                fw_update_scsi_prev_phase.v = 22;
                fw_update_scsi_phase.v = 24;
                record->message_out_len = 0;
                record->c3b = 0;
                record->c38 = 0;
                fw_update_scsi_next_message();
            } else {
                record->phase_flags.b.b1 = 0;
                fw_update_scsi_phase.v = 27;
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_p_scsi_ctrl->ndmasize = 1;
                fw_update_scsi_ctrl_command(18);
            }
            break;
        case 2:
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                if (fw_update_scsi_prev_phase.v != 24 || fw_update_scsi_prev_phase.v != 22) {
                    fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                }
                fw_update_scsi_prev_phase.v = 22;
                fw_update_scsi_phase.v = 24;
                fw_update_scsi_ctrl_command(25);
            } else {
                record->message_in = 4;
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_p_scsi_ctrl->fifodata = 4;
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_p_scsi_ctrl->ndmasize = 1;
                fw_update_scsi_ctrl_command(24);
            }
            break;
        case 3:
            switch (fw_update_scsi_phase.v) {
            case 16:
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                    fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                    fw_update_scsi_prev_phase.v = 22;
                    fw_update_scsi_phase.v = 24;
                    fw_update_scsi_ctrl_command(25);
                } else {
                    fw_update_scsi_phase.v = 16;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->command = 18;
                }
                break;
            case 26:
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                    fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                    fw_update_scsi_prev_phase.v = 22;
                    fw_update_scsi_phase.v = 24;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->command = 25;
                } else {
                    fw_update_scsi_phase.v = 26;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->fifodata = record->status_byte;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->command = 23;
                }
                break;
            case 18:
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                    fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                    fw_update_scsi_prev_phase.v = 22;
                    fw_update_scsi_phase.v = 24;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->command = 25;
                } else {
                    record->data_ptr = fw_update_reply_buf;
                    record->data_out_count = 0;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->fifodata = fw_update_reply_buf[0];
                    fw_update_scsi_phase.v = 26;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->ndmasize = 1;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->command = 22;
                }
                break;
            case 19:
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                    fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                    fw_update_scsi_prev_phase.v = 22;
                    fw_update_scsi_phase.v = 24;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->command = 25;
                } else {
                    record->data_ptr = fw_update_reply_buf;
                    record->data_out_count = 0;
                    fw_update_scsi_phase.v = 19;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->command = 20;
                }
                break;
            case 24:
                fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                goto again;
                break;
            default:
                fw_update_build_sense(record);
                fw_update_scsi_phase.v = 4;
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_p_scsi_ctrl->command = 5;
                fw_update_scsi_release_bus(record);
            }
            break;
        case 4:
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                if (fw_update_scsi_prev_phase.v != 24 || fw_update_scsi_prev_phase.v != 22) {
                    fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                }
                fw_update_scsi_prev_phase.v = 22;
                fw_update_scsi_phase.v = 24;
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_p_scsi_ctrl->command = 25;
            } else {
                fw_update_scsi_phase.v = 3;
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_p_scsi_ctrl->command = 5;
                fw_update_scsi_disconnected((struct fw_update_command_view_1 *)record);
            }
            break;
        case 7:
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                if (fw_update_scsi_prev_phase.v != 24 || fw_update_scsi_prev_phase.v != 22) {
                    fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                }
                fw_update_scsi_prev_phase.v = 22;
                fw_update_scsi_phase.v = 24;
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_p_scsi_ctrl->command = 25;
            } else {
                fw_update_build_sense(record);
                record->status_flags.b1 = 1;
                fw_update_scsi_phase.v = 32;
                _builtin_set_imask(15);
                fw_update_queue_record(record);
                _builtin_set_imask(saved_imask);
            }
            break;
        default:
            if (record->lun_flags.b6) {
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_scsi_ctrl->scsictl.b.b6) {
                    if (fw_update_scsi_prev_phase.v != 24 || fw_update_scsi_prev_phase.v != 22) {
                    fw_update_scsi_message_from.v = fw_update_scsi_prev_phase.v;
                }
                    fw_update_scsi_prev_phase.v = 22;
                    fw_update_scsi_phase.v = 24;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_scsi_ctrl->command = 25;
                } else {
                    record->disconnect_flags.b4 = 0;
                    fw_update_disconnected_record.p->handler = fw_update_disconnected_record.p->saved_handler;
                    fw_update_disconnected_record.p->saved_handler = fw_update_disconnected_record.p->saved_handler_outer;
                    if (fw_update_disconnected_record.p->disconnect_flags.b7) {
                        fw_update_queue_record(fw_update_disconnected_record.p);
                    } else {
                        fw_update_end_command();
                    }
                    return;
                }
            } else {
                fw_update_build_sense(record);
                fw_update_scsi_phase.v = 4;
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_p_scsi_ctrl->command = 5;
                fw_update_scsi_release_bus(record);
                return;
            }
        }
    } else {
        fw_update_build_sense(record);
        fw_update_scsi_phase.v = 4;
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_scsi_ctrl->command = 5;
        fw_update_scsi_release_bus(record);
        return;
    }
}

/* CDB length from the group code (opcode bits 5-7); the vendor opcodes 0xC1, 0xC2, 0xF1 are 10
   bytes. Returns 0 for an unknown group (length 6). */
int fw_update_scsi_cdb_len(struct fw_update_command_link *record, unsigned char *len)
{
    *len = (record->cdb[0] & 0xe0) >> 5;
    switch (*len) {
    case 0:
        *len = 6;
        break;
    case 1:
    case 2:
        *len = 10;
        break;
    case 5:
        *len = 12;
        break;
    default:
        if (record->cdb[0] == SCSI_OP_ENDORSER || record->cdb[0] == SCSI_OP_GET_HW_STATUS || record->cdb[0] == SCSI_OP_SCANNER_CONTROL) {
            *len = 10;
        } else {
            *len = 6;
            return 0;
        }
    }
    return 1;
}

int fw_update_scsi_read_fifo(int unused)
{
    unsigned short count;
    unsigned short i;

    count = fw_update_current_record.p->transfer_len;
    for (i = 0; i < count; i++) {
        fw_update_sync_dst = fw_update_sync_src;
        while (fw_update_p_scsi_ctrl->fifoctl.b0) {
        }
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_command_buf[i] = fw_update_p_scsi_ctrl->fifodata;
    }
    return 1;
}

int fw_update_scsi_start_dma(struct fw_update_command_link *record)
{
    unsigned int count;
    int u1;
    int u2;

    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->dmactl.b.b1 = 0;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->fifoctl.b2 = 1;
    count = record->transfer_len;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->dtbc2 = count >> 16;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->dtbc1 = (count & 0xff00) >> 8;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->dtbc0 = count;
    fw_update_scsi_phase.v = 18;
    fw_update_scsi_ctrl_command(21);
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->dmactl.b.b0 = 1;
    while (!fw_update_dma_done) {
    }
    fw_update_dma_armed = 0;
    fw_update_dma_done = 0;
    return 1;
}

int fw_update_scsi_send_message(struct fw_update_command_link *record)
{
    unsigned char message;
    int i;

    fw_update_scsi_phase.v = 22;
    message = record->message_in;
    if (message == 1) {
        message = fw_update_extended_message_len[0];
        if (message >= 2 && message <= 3) {
            for (i = 0; i < message + 2; i++) {
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_p_scsi_ctrl->fifodata = fw_update_scsi_message[i];
            }
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_scsi_ctrl->ndmasize = message + 2;
        } else {
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_scsi_ctrl->fifodata = 7; /* MESSAGE REJECT */
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_scsi_ctrl->ndmasize = 1;
        }
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_scsi_ctrl->fifodata = message;
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_scsi_ctrl->ndmasize = 1;
    }
    fw_update_scsi_ctrl_command(24);
    return 1;
}


void fw_update_scsi_next_message(void)
{
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->ndmasize = 1;
    fw_update_scsi_ctrl_command(25);
}

int fw_update_scsi_send_status(struct fw_update_command_link *record)
{
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->fifoctl.b2 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->fifodata = record->status_byte;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->fifodata = record->message_in;
    fw_update_scsi_phase.v = 22;
    fw_update_scsi_ctrl_command(26);
    return 1;
}

int fw_update_scsi_reselect(struct fw_update_command_link *record)
{
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->sdid = record->initiator_id;
    fw_update_scsi_phase.v = 5;
    fw_update_scsi_ctrl_command(13);
    return 1;
}

int fw_update_scsi_receive_data(struct fw_update_command_link *record)
{
    unsigned int count;
    unsigned int i;
    unsigned char *q;

    count = record->transfer_len;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->dtbc2 = count >> 16;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->dtbc1 = (count & 0xff00) >> 8;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->dtbc0 = count;
    fw_update_scsi_phase.v = 19;
    fw_update_scsi_ctrl_command(19);
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->dmactl.c |= 3;
    q = record->rx_ptr;
    for (i = 0; i < count; i++, q++) {
        fw_update_sync_dst = fw_update_sync_src;
        while (fw_update_p_scsi_ctrl->fifoctl.b0) {
        }
        fw_update_sync_dst = fw_update_sync_src;
        *q = fw_update_p_scsi_ctrl->fifodata;
    }
    return 1;
}

int fw_update_scsi_send_data(struct fw_update_command_link *record)
{
    unsigned int count;
    unsigned int i;
    volatile unsigned char *q;
    int u1;

    count = record->transfer_len;
    q = record->data_ptr;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_scsi_ctrl->syncmode.f.l) {
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_scsi_ctrl->dtbc2 = count >> 16;
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_scsi_ctrl->dtbc1 = (count & 0xff00) >> 8;
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_scsi_ctrl->dtbc0 = count;
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_scsi_ctrl->fifoctl.b2 = 1;
        fw_update_scsi_phase.v = 18;
        fw_update_scsi_ctrl_command(21);
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_scsi_ctrl->dmactl.c |= 3;
        *fw_update_p_intc &= 0xfffc;
        for (i = 0; i < count; i++, q++) {
            while (fw_update_p_scsi_ctrl->fifoctl.b1) {
            }
            fw_update_p_scsi_ctrl->fifodata = *q;
        }
        fw_update_send_timeout = 0;
        while (1) {
            if (fw_update_p_scsi_ctrl->fifoctl.b0 || fw_update_p_scsi_ctrl->mainint.b.b1) {
                break;
            }
            if (fw_update_send_timeout > 500) {
                fw_update_p_scsi_ctrl->fifoctl.b2 = 1;
                fw_update_current_record.p->mainint.b.b6 = 0;
                fw_update_p_scsi_ctrl->command = 1;
                *fw_update_p_intc |= 3;
                while (!fw_update_current_record.p->mainint.b.b6) {
                }
                fw_update_current_record.p->mainint.b.b6 = 0;
                break;
            }
        }
        *fw_update_p_intc |= 3;
    } else {
        *fw_update_p_intc &= 0xfffc;
        for (; count != 0; q++, count--) {
            fw_update_p_scsi_ctrl->ndmasize = 1;
            fw_update_scsi_phase.v = 18;
            fw_update_scsi_ctrl_command(22);
            fw_update_p_scsi_ctrl->fifodata = *q;
            fw_update_send_timeout = 0;
            while (fw_update_p_scsi_ctrl->mainint.b.b5) {
                if (fw_update_send_timeout > 5) {
                    fw_update_p_scsi_ctrl->fifoctl.b2 = 1;
                    fw_update_current_record.p->mainint.b.b6 = 0;
                    fw_update_p_scsi_ctrl->command = 1;
                    *fw_update_p_intc |= 3;
                    while (!fw_update_current_record.p->mainint.b.b6) {
                    }
                    fw_update_current_record.p->mainint.b.b6 = 0;
                    if (!fw_update_p_scsi_ctrl->fifoctl.b0) {
                        fw_update_scsi_ctrl_command(22);
                    }
                }
            }
        }
        *fw_update_p_intc |= 3;
    }
    return 1;
}

void fw_update_scsi_unused_hook(void)
{
}

int fw_update_scsi_negotiate_sync(struct fw_update_command_link *record)
{
    unsigned char period;

    if (record->message_out_len < 4 || record->message_out_len > 5) {
        return 7;
    }
    if (fw_update_extended_message_code[0] == 1) { /* extended message 1: SDTR (period x 4 ns, offset) */
        if (record->message_out_len != 5) {
            return 7;
        }
        if (fw_update_extended_message_len[0] != 3) {
            fw_update_scsi_set_async();
            return 7;
        }
        if (fw_update_sdtr_offset[0] == 0) {
            fw_update_scsi_set_async();
            return 1;
        } else {
            if (fw_update_sdtr_offset[0] > 15) {
                fw_update_sdtr_offset[0] = 15;
            }
            if (fw_update_sdtr_period[0] <= 12) {
                period = 12;
            } else if (fw_update_sdtr_period[0] <= 25) {
                period = 25;
            } else if (fw_update_sdtr_period[0] <= 38) {
                period = 38;
            } else if (fw_update_sdtr_period[0] <= 50) {
                period = 50;
            } else if (fw_update_sdtr_period[0] <= 62) {
                period = 62;
            } else if (fw_update_sdtr_period[0] <= 75) {
                period = 75;
            } else if (fw_update_sdtr_period[0] <= 100) {
                period = 100;
            } else if (fw_update_sdtr_period[0] <= 125) {
                period = 125;
            } else if (fw_update_sdtr_period[0] <= 167) {
                period = 167;
            } else {
                fw_update_sdtr_offset[0] = 0;
                fw_update_scsi_set_async();
                return 1;
            }
            fw_update_sync_initiator = fw_update_initiator_bit((char)record->initiator_id);
            fw_update_sdtr_period[0] = period;
            fw_update_sync_params[record->initiator_id].v = period;
            fw_update_sync_offsets[record->initiator_id].v = fw_update_sdtr_offset[0];
            fw_update_scsi_set_sync();
            return 1;
        }
    }
    if (fw_update_extended_message_code[0] == 3) { /* extended message 3: WDTR, answered with 8-bit width */
        if (record->message_out_len != 4) {
            return 7;
        }
        fw_update_sdtr_period[0] = 0;
        fw_update_scsi_set_async();
        return 3;
    }
}

void fw_update_scsi_set_sync(void)
{
    unsigned short period;
    char sync_mode;

    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->scsimode0.b4 = 0;
    period = fw_update_sdtr_period[0];
    period <<= 2;
    if (period < 100) {
        period += 24;
        period = fw_update_divu16(period, 25);
        if (period > 3) {
            period = 0;
        } else {
            if (period < 2) {
                period = 2;
            }
            period -= 2;
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_scsi_ctrl->scsimode0.b4 = 1;
        }
    } else {
        period += 49;
        period = fw_update_divu16(period, 50);
        if (period < 2) {
            period = 2;
        }
        period -= 2;
    }
    sync_mode = (period << 4) & 0xf0;
    sync_mode += fw_update_sdtr_offset[0] & 15;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->syncmode.c = sync_mode;
}

int fw_update_scsi_set_async(void)
{
    char sync_mode;

    sync_mode = (fw_update_sdtr_period[0] << 4) & 0xf0;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->syncmode.c = sync_mode;
}

/* Issues controller command cmd (+41) once the controller is idle, then waits for it to be taken. */
void fw_update_scsi_ctrl_command(unsigned char cmd)
{
    fw_update_send_timeout = 0;
    fw_update_sync_dst = fw_update_sync_src;
    while (fw_update_p_scsi_ctrl->mainint.b.b5) {
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_send_timeout > 5) {
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_scsi_ctrl->fifoctl.b2 = 1;
            fw_update_current_record.p->mainint.b.b6 = 0;
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_scsi_ctrl->command = 1;
            while (!fw_update_current_record.p->mainint.b.b6) {
            }
            fw_update_current_record.p->mainint.b.b6 = 0;
        }
    }
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_scsi_ctrl->command = cmd;
    fw_update_sync_dst = fw_update_sync_src;
    while (fw_update_p_scsi_ctrl->scsiint2.b.b3) {
    }
}

void fw_update_scsi_take_identify(void)
{
    fw_update_current_record.p->lun_flags.b3 = 1;
    fw_update_current_record.p->lun_flags.b6 = 1;
    if (fw_update_current_record.p->identify & 0x40) { /* IDENTIFY bit 6: disconnect privilege */
        fw_update_current_record.p->lun_flags.b4 = 1;
    } else {
        fw_update_current_record.p->lun_flags.b4 = 0;
    }
    fw_update_current_record.p->lun[0] = fw_update_current_record.p->identify & 7;
}
