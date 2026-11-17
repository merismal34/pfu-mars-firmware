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

int fw_update_usb_command_state_1(struct fw_update_command_view_2 *record)
{
    int a;
    int b;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    if (fw_update_usb_epb_event == USB_EVENT_OUT_ACK) {
        fw_update_usb_epb_event = 0;
        fw_update_usb_bulk_wait_count = 0;
        fw_update_usb_bulk_out_state = 0;
    } else if (fw_update_usb_epb_event == USB_EVENT_OUT_ERROR) {
        fw_update_usb_epb_event = 0;
        ++fw_update_usb_bulk_wait_count;
        if (fw_update_usb_bulk_wait_count > 255) {
            fw_update_usb_bulk_wait_count = 0;
            if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                fw_update_command_flags.b0 = 1;
            record->status_flags.b7 = 1;
            if (fw_update_usb_transfer_phase == 0)
                fw_update_finish_record(record);
            _builtin_set_imask(15);
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_17.b6 = 1;
            fw_update_usb_transfer_phase = 3;
            _builtin_set_imask(saved_imask);
            fw_update_usb_bulk_state = 3;
            fw_update_usb_bulk_out_state = 3;
            return 0;
        }
    } else {
    }
    return 1;
}

int fw_update_usb_send_status(struct fw_update_command_view_2 *record)
{
    unsigned char state;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_tick_count = 0;
    _builtin_set_imask(saved_imask);
    while (1) {
        state = fw_update_usb_status_states[fw_update_usb_bulk_state](record);
        if (fw_update_usb_bulk_state >= 2)
            break;
        fw_update_usb_irq_device_events();
        fw_update_usb_run_events_6_4();
        if (fw_update_usb_event_flags.b6) {
            fw_update_usb_bulk_state = 2;
            state = 0;
            if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                fw_update_command_flags.b0 = 1;
            break;
        }
        if (fw_update_tick_count > 15000) {
            fw_update_usb_epa_event = 0;
            if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                fw_update_command_flags.b0 = 1;
            record->status_flags.b7 = 1;
            fw_update_finish_record(record);
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

int fw_update_usb_send_status_block(struct fw_update_command_view_2 *record)
{
    char *q;
    int i;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_stat = ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_stat;
    _builtin_set_imask(saved_imask);
    if (fw_update_usb_transfer_flags.pisces_in != 0) {
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_38.b2 = 0;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_38.b1 = 1;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_17.b1 = 1;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_control.b1 = 0;
        if (fw_update_usb_bulk_len & (fw_update_usb_epa_max_packet - 1)) {
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_38.b6 = 1;
        }
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->dma_count_hh = fw_update_usb_bulk_len >> 24;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->dma_count_hl = fw_update_usb_bulk_len >> 16;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->dma_count_lh = fw_update_usb_bulk_len >> 8;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->dma_count_ll = fw_update_usb_status_dma_low;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_39.b0 = 1;
        fw_update_usb_bulk_len = 0;
        fw_update_usb_bulk_state = 1;
    } else {
        _builtin_set_imask(15);
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_int_stat = 255;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_int_enb.b5 = 1;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_38.b6 = 0;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_control.b1 = 1;
        _builtin_set_imask(saved_imask);
        if (fw_update_usb_transfer_phase == 1) {
            q = record->data_ptr;
            if (fw_update_usb_bulk_len >= fw_update_usb_epa_max_packet) {
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_31.b1 = 1;
                for (i = 0; i < fw_update_usb_epa_max_packet; i++) {
                    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->fifo_byte = *q++;
                }
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_31.b1 = 0;
                fw_update_usb_bulk_len -= fw_update_usb_epa_max_packet;
                record->data_ptr = q;
            } else {
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_31.b1 = 1;
                for (i = fw_update_usb_bulk_len; i > 0; i--) {
                    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->fifo_byte = *q++;
                }
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_31.b1 = 0;
                fw_update_usb_bulk_len = 0;
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_control.b6 = 1;
            }
        } else {
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_31.b1 = 1;
            for (i = 0; i < fw_update_usb_bulk_len; i++) {
                fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->fifo_byte = fw_update_reply_buf[i];
            }
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_31.b1 = 0;
            fw_update_usb_bulk_len = 0;
            fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_control.b6 = 1;
        }
        fw_update_usb_bulk_state = 1;
        fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_control.b1 = 0;
    }
    fw_update_usb_epa_event = 0;
    return 1;
}

int fw_update_usb_data_state_1(struct fw_update_command_view_2 *record)
{
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    if (fw_update_usb_epa_event == USB_EVENT_DMA_DONE) {
        fw_update_usb_epa_event = 0;
        fw_update_usb_bulk_wait_count = 0;
        fw_update_usb_bulk_state = 2;
        fw_update_usb_transfer_flags.done = 1;
        _builtin_set_imask(15);
        fw_update_usb_transfer_phase = 2;
        _builtin_set_imask(saved_imask);
    } else if (fw_update_usb_epa_event == USB_EVENT_IN_ACK) {
        fw_update_usb_epa_event = 0;
        fw_update_usb_bulk_wait_count = 0;
        _builtin_set_imask(15);
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_int_enb.b5 = 0;
        _builtin_set_imask(saved_imask);
        if (fw_update_usb_bulk_len) {
            fw_update_usb_bulk_state = 0;
        } else {
            fw_update_usb_transfer_flags.done = 1;
            fw_update_usb_bulk_state = 2;
            if (fw_update_usb_transfer_phase == 1) {
                _builtin_set_imask(15);
                fw_update_usb_transfer_phase = 2;
                _builtin_set_imask(saved_imask);
            } else {
                fw_update_finish_record(record);
                _builtin_set_imask(15);
                fw_update_usb_transfer_phase = 0;
                _builtin_set_imask(saved_imask);
                fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_enb.b4 = 1;
                fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_control.b1 = 0;
            }
        }
    } else if (fw_update_usb_epa_event == USB_EVENT_IN_ERROR || fw_update_usb_epa_event == 10) {
        fw_update_usb_epa_event = 0;
        ++fw_update_usb_bulk_wait_count;
        if (fw_update_usb_bulk_wait_count > 255) {
            if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                fw_update_command_flags.b0 = 1;
            record->status_flags.b7 = 1;
            fw_update_finish_record(record);
            _builtin_set_imask(15);
            fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_17.b6 = 1;
            fw_update_usb_transfer_phase = 3;
            _builtin_set_imask(saved_imask);
            fw_update_usb_bulk_state = 3;
            fw_update_usb_bulk_out_state = 3;
            return 0;
        }
    } else {
    }
    return 1;
}

int fw_update_usb_run_data(struct fw_update_command_view_2 *record)
{
    unsigned char state;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_usb_bulk_state = 0;
    _builtin_set_imask(15);
    fw_update_usb_transfer_phase = 1;
    _builtin_set_imask(saved_imask);
    while (fw_update_usb_bulk_state < 2) {
        state = fw_update_usb_data_states[fw_update_usb_bulk_state](record);
        fw_update_usb_poll_events();
        fw_update_usb_run_events_6_4();
        if (fw_update_usb_event_flags.b6) {
            fw_update_usb_bulk_state = 2;
            state = 0;
            if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                fw_update_command_flags.b0 = 1;
            break;
        }
    }
    return state;
}

int fw_update_usb_data_state_0(int record)
{
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_stat = ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epb_int_stat;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_int_stat = 0xff;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_int_enb.b5 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_38.b6 = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_control.b1 = 1;
    _builtin_set_imask(saved_imask);
    fw_update_usb_bulk_len = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->reg_17.b1 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_control.b6 = 1;
    fw_update_usb_bulk_state = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_command *)fw_update_p_usb_ctrl)->epa_control.b1 = 0;
    fw_update_usb_epa_event = 0;
    return 1;
}
