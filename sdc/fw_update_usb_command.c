#include "boot.h"
#pragma section 2
struct usb_regs_command {
    char reg_00_03[9];
    unsigned char epa_int_stat;
    char pad_0a;
    char epb_int_stat;
    char q[13];
    struct Bits epa_int_enb;
    char pad_1a;
    struct Bits epb_int_enb;
    char reg_0e_16[19];
    struct Bits reg_17;
    char reg_18_2b[41];
    struct Bits epa_control;
    char pad_5a;
    struct Bits epb_control;
    char reg_2e_2f[5];
    struct Bits reg_30;
    char pad_62;
    struct Bits reg_31;
    char pad_64;
    unsigned char rd_remain_h;
    char pad_66;
    unsigned char rd_remain_l;
    char reg_34_36[7];
    unsigned char fifo_byte;
    char pad_70;
    struct Bits reg_38;
    char pad_72;
    struct Bits reg_39;
    char reg_3a_3b[5];
    unsigned char dma_count_hh;
    char pad_7a;
    unsigned char dma_count_hl;
    char pad_7c;
    unsigned char dma_count_lh;
    char pad_7e;
    unsigned char dma_count_ll;
};

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

int fw_update_usb_run_command(struct fw_update_command_view_2 *record)
{
    unsigned char state;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_tick_count = 0;
    _builtin_set_imask(saved_imask);
    while (1) {
        state = fw_update_usb_command_states[fw_update_usb_bulk_out_state](record);
        if (fw_update_usb_bulk_out_state >= 2)
            break;
        fw_update_usb_poll_events();
        fw_update_usb_run_events_6_4();
        if (fw_update_usb_event_flags.b6) {
            fw_update_usb_bulk_out_state = 2;
            state = 0;
            if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                fw_update_command_flags.b0 = 1;
            break;
        }
        if (fw_update_tick_count > 15000) {
            fw_update_usb_epb_event = 0;
            if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                fw_update_command_flags.b0 = 1;
            record->status_flags.b7 = 1;
            fw_update_usb_ack_and_reset();
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_17.b6 = 1;
            _builtin_set_imask(15);
            fw_update_usb_transfer_phase = 3;
            _builtin_set_imask(saved_imask);
            fw_update_usb_bulk_state = 3;
            fw_update_usb_bulk_out_state = 3;
            return 0;
        }
    }
    return state;
}

int fw_update_usb_receive_command(struct fw_update_command_view_2 *record)
{
    char *q;
    char cdb_len;
    int i;
    int j;
    unsigned short len;
    int unused_frame;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_int_stat = ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_int_stat;
    _builtin_set_imask(saved_imask);
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_30.b2 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    len = (((struct usb_regs_command *)fw_update_p_usb_ctrl)->rd_remain_h << 8) | ((struct usb_regs_command *)fw_update_p_usb_ctrl)->rd_remain_l;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_30.b2 = 0;
    if (len == 0) {
        if (fw_update_usb_bulk_len) {
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_enb.b4 = 1;
            fw_update_usb_bulk_out_state = 1;
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_control.b1 = 0;
        } else {
            fw_update_usb_transfer_flags.done = 1;
            fw_update_usb_bulk_out_state = 2;
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_enb.b4 = 0;
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_control.b1 = 1;
        }
    } else {
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_control.b1 = 1;
        if (fw_update_usb_transfer_phase == 0) {
            ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_30.b2 = 1;
            for (i = 0; i < len; i++)
                fw_update_command_buf[i] = ((struct usb_regs_command *)fw_update_p_usb_ctrl)->fifo_byte;
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_30.b2 = 0;
            if (i == 31 && fw_update_command_buf[0] == 67) {
                i = 1;
                while (i < 19) {
                    if (fw_update_command_buf[i] != 0) {
                        if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                            fw_update_command_flags.b0 = 1;
                        record->status_flags.b7 = 1;
                        fw_update_finish_record(record);
                        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_17.b6 = 1;
                        fw_update_usb_bulk_state = 3;
                        fw_update_usb_bulk_out_state = 3;
                        _builtin_set_imask(15);
                        fw_update_usb_transfer_phase = 3;
                        _builtin_set_imask(saved_imask);
                        return 0;
                    }
                    i++;
                }
            } else {
                if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                    fw_update_command_flags.b0 = 1;
                record->status_flags.b7 = 1;
                fw_update_finish_record(record);
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_17.b6 = 1;
                fw_update_usb_bulk_state = 3;
                fw_update_usb_bulk_out_state = 3;
                _builtin_set_imask(15);
                fw_update_usb_transfer_phase = 3;
                _builtin_set_imask(saved_imask);
                return 0;
            }
            cdb_len = fw_update_command_byte_19[0];
            record->lun_flags.b3 = 1;
            record->lun_flags.b6 = 1;
            fw_update_current_record.p->identify = 0x80;
            fw_update_current_record.p->lun_flags.b4 = 0;
            record->lun = 0;
            record->cdb[0] = cdb_len;
            fw_update_scsi_cdb_len(record, &cdb_len);
            i = 1;
            j = 20;
            while (i < 12) {
                record->cdb[i] = fw_update_command_buf[j];
                i++;
                j++;
            }
            if (record->lun_flags.b6 == 0) {
                record->lun = record->cdb[1] & 7;
                record->lun_flags.b3 = 1;
            }
            fw_update_usb_transfer_flags.done = 0;
            fw_update_usb_transfer_flags.pending = 1;
            fw_update_usb_wake_events.b7 = 1;
            fw_update_usb_bulk_out_state = 2;
            _builtin_set_imask(15);
            fw_update_usb_transfer_phase = 1;
            fw_update_current_record.p->c54 = 0;
            fw_update_unqueue_record(fw_update_current_record.p);
            fw_update_current_record.p->handler = 35;
            fw_update_queue_record(fw_update_current_record.p);
            _builtin_set_imask(saved_imask);
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_enb.b4 = 0;
            return 1;
        } else {
            if (fw_update_usb_bulk_len < len) {
                if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                    fw_update_command_flags.b0 = 1;
                record->status_flags.b7 = 1;
                fw_update_finish_record(record);
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_17.b6 = 1;
                _builtin_set_imask(15);
                fw_update_usb_transfer_phase = 3;
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_enb.b4 = 0;
                _builtin_set_imask(saved_imask);
                fw_update_usb_bulk_state = 3;
                fw_update_usb_bulk_out_state = 3;
                return 0;
            } else {
            q = record->rx_ptr;
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_30.b2 = 1;
            for (i = 0; i < len; i++) {
                fw_update_sync_dst = fw_update_sync_src;
    *q++ = ((struct usb_regs_command *)fw_update_p_usb_ctrl)->fifo_byte;
            }
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_30.b2 = 0;
            fw_update_usb_bulk_len -= len;
            record->rx_ptr = q;
            if (fw_update_usb_bulk_len) {
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_enb.b4 = 1;
                fw_update_usb_bulk_out_state = 1;
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_control.b1 = 0;
            } else {
                fw_update_usb_transfer_flags.done = 1;
                fw_update_usb_bulk_out_state = 2;
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_enb.b4 = 0;
            }
            }
        }
    }
    return 1;
}
