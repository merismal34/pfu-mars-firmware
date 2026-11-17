#include "fw.h"
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
struct board_ctrl_regs_usb_bulk {
    unsigned short pad_00;
    unsigned short :6,
                   hostif_idle:1,
                   :9;
    unsigned short :6,
                   hostif_idle_enable:1,
                   :9;
};

extern void _builtin_set_gbr(void *);
struct usb_regs_bulk {
    char reg_00_03[9];
    unsigned char epa_int_stat;
    char pad_0a;
    unsigned char epb_int_stat;
    char reg_06_0b[13];
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

int usb_bulk_send_data(void)
{
    unsigned char *p;
    int i;
    unsigned char status;
    int saved_imask;
    int flag;
    saved_imask = _builtin_get_imask();
    flag = 0;
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    status = ((struct usb_regs_bulk *)usb_ctrl)->epb_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->epb_int_stat = status;
    _builtin_set_imask(saved_imask);
    usb_epa_event = 0;
    if (usb_transfer_flags.pisces_in) {
        usb_bulk_sending = 1;
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->reg_38.b2 = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->reg_38.b1 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->reg_17.b1 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->epa_control.b1 = 0;
        _builtin_set_imask(saved_imask);
        if (usb_bulk_len & (usb_epa_max_packet.v - 1)) {
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->reg_38.b6 = 1;
        }
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->dma_count_hh = usb_bulk_len >> 24;
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->dma_count_hl = usb_bulk_len >> 16;
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->dma_count_lh = usb_bulk_len >> 8;
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->dma_count_ll = (unsigned char)usb_bulk_len;
        _builtin_set_imask(saved_imask);
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->reg_39.b0 = 1;
        p_pisces_ctrl[0]->ctrl.f.hostif_stop = 0;
        if (((volatile struct board_ctrl_regs_usb_bulk *)p_board_ctrl)->hostif_idle_enable == 1) {
            flag = 1;
            ((volatile struct board_ctrl_regs_usb_bulk *)p_board_ctrl)->hostif_idle_enable = 0;
        }
        if (scsi_read_side.v == 0) {
            p_pisces_hostif[1]->ctrl.f.b0 = 1;
            p_pisces_hostif[1]->ctrl.f.b9 = 0;
            p_pisces_hostif[0]->ctrl.f.b9 = 1;
            p_pisces_hostif[0]->ctrl.f.b0 = 0;
        } else {
            p_pisces_hostif[0]->ctrl.f.b0 = 1;
            p_pisces_hostif[0]->ctrl.f.b9 = 0;
            p_pisces_hostif[1]->ctrl.f.b9 = 1;
            p_pisces_hostif[1]->ctrl.f.b0 = 0;
        }
        if (flag && !power_state.f.leave_request)
            ((volatile struct board_ctrl_regs_usb_bulk *)p_board_ctrl)->hostif_idle_enable = 1;
        usb_bulk_len = 0;
        usb_bulk_state = 1;
    } else {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->epa_int_stat = 0xff;
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->epa_int_enb.b5 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->reg_38.b6 = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->epa_control.b1 = 1;
        _builtin_set_imask(saved_imask);
        if (usb_transfer_phase == 1) {
            p = usb_bulk_in_ptr;
            if (usb_bulk_len >= usb_epa_max_packet.v) {
                _builtin_set_imask(15);
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->reg_31.b1 = 1;
                _builtin_set_imask(saved_imask);
                for (i = 0; i < usb_epa_max_packet.v; i++) {
                    _builtin_set_gbr(0);
                    ((struct usb_regs_bulk *)usb_ctrl)->fifo_byte = *p++;
                }
                _builtin_set_imask(15);
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->reg_31.b1 = 0;
                usb_bulk_len -= usb_epa_max_packet.v;
                usb_bulk_in_ptr = p;
            } else {
                _builtin_set_imask(15);
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->reg_31.b1 = 1;
                _builtin_set_imask(saved_imask);
                for (i = usb_bulk_len; i > 0; i--) {
                    _builtin_set_gbr(0);
                    ((struct usb_regs_bulk *)usb_ctrl)->fifo_byte = *p++;
                }
                _builtin_set_imask(15);
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->reg_31.b1 = 0;
                usb_bulk_len = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->epa_control.b6 = 1;
            }
        } else {
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->reg_31.b1 = 1;
            _builtin_set_imask(saved_imask);
            for (i = 0; i < usb_bulk_len; i++) {
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->fifo_byte = scsi_reply_buf[i];
            }
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->reg_31.b1 = 0;
            usb_bulk_len = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->epa_control.b6 = 1;
        }
        usb_bulk_state = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->epa_control.b1 = 0;
    }
    _builtin_set_imask(saved_imask);
    return 1;
}


int usb_bulk_state_1(void)
{
    struct page_record *current_page;
    int saved_imask;
    current_page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    if (usb_epa_event == USB_EVENT_DMA_DONE) {
        usb_bulk_sending = 0;
        usb_epa_event = 0;
        usb_bulk_wait_count = 0;
        usb_bulk_state = 2;
        usb_transfer_flags.done = 1;
        _builtin_set_imask(15);
        usb_transfer_phase = 2;
        _builtin_set_imask(saved_imask);
    } else if (usb_epa_event == USB_EVENT_IN_ACK) {
        usb_epa_event = 0;
        usb_bulk_wait_count = 0;
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->epa_int_enb.b5 = 0;
        _builtin_set_imask(saved_imask);
        if (usb_bulk_len) {
            usb_bulk_state = 0;
        } else {
            usb_transfer_flags.done = 1;
            usb_bulk_state = 2;
            if (usb_transfer_phase == 1) {
                _builtin_set_imask(15);
                usb_transfer_phase = 2;
                _builtin_set_imask(saved_imask);
            } else {
                _builtin_set_imask(15);
                usb_transfer_phase = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->epb_int_enb.b4 = 1;
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->epb_control.b1 = 0;
                _builtin_set_imask(saved_imask);
            }
        }
    } else if (usb_epa_event == USB_EVENT_IN_ERROR) {
        usb_epa_event = 0;
        usb_bulk_wait_count++;
        if (usb_bulk_wait_count > 255) {
            _builtin_set_imask(15);
            if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                current_page->flags.f.scan_end = 1;
            _builtin_set_imask(saved_imask);
            usb_save_error_state();
            usb_error_code = 2;
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->reg_17.b6 = 1;
            usb_transfer_phase = 3;
            _builtin_set_imask(saved_imask);
            usb_bulk_state = 3;
            usb_bulk_out_state = 3;
            return 0;
        }
    } else {
    }
    return 1;
}


int usb_start_data_in(void)
{
    unsigned char state;
    struct page_record *current_page;
    int saved_imask;
    current_page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    usb_bulk_state = 0;
    _builtin_set_imask(15);
    usb_transfer_phase = 1;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    if (((volatile struct board_ctrl_regs_usb_bulk *)p_board_ctrl)->hostif_idle == 1) {
        usb_transfer_busy |= 4;
        ((volatile struct board_ctrl_regs_usb_bulk *)p_board_ctrl)->hostif_idle = 0;
    }
    _builtin_set_imask(saved_imask);
    while (usb_bulk_state < 2) {
        state = usb_bulk_state_table[usb_bulk_state]();
        usb_poll_device_events();
        usb_run_events_6_4();
        if (usb_event_flags.f.detach) {
            usb_bulk_state = 2;
            state = 0;
            _builtin_set_imask(15);
            if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                current_page->flags.f.scan_end = 1;
            _builtin_set_imask(saved_imask);
            break;
        }
    }
    _builtin_set_imask(15);
    usb_transfer_busy &= 0xfffb;
    _builtin_set_imask(saved_imask);
    return state;
}

int usb_bulk_state_0(void)
{
    int saved_imask;
    unsigned char status;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    status = ((struct usb_regs_bulk *)usb_ctrl)->epb_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->epb_int_stat = status;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->epa_int_stat = 0xff;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->epa_int_enb.b5 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->reg_38.b6 = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->epa_control.b1 = 1;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    usb_bulk_len = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->reg_17.b1 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->epa_control.b6 = 1;
    usb_bulk_state = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->epa_control.b1 = 0;
    usb_epa_event = 0;
    _builtin_set_imask(saved_imask);
    return 1;
}

int usb_bulk_receive_data(void)
{
    struct page_record *current_page;
    unsigned char state;
    int saved_imask;
    current_page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    tick_hostif_wait_ticks.v = 0;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    if (((volatile struct board_ctrl_regs_usb_bulk *)p_board_ctrl)->hostif_idle == 1) {
        usb_transfer_busy |= 2;
        ((volatile struct board_ctrl_regs_usb_bulk *)p_board_ctrl)->hostif_idle = 0;
    }
    _builtin_set_imask(saved_imask);
    while (1) {
        if (usb_bulk_out_state >= 2) {
            return 0;
        }
        state = usb_bulk_out_state_table[usb_bulk_out_state]();
        if (usb_bulk_out_state >= 2) {
            break;
        }
        usb_poll_device_events();
        usb_run_events_6_4();
        if (usb_event_flags.f.detach) {
            usb_bulk_out_state = 2;
            state = 0;
            _builtin_set_imask(15);
            if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                current_page->flags.f.scan_end = 1;
            _builtin_set_imask(saved_imask);
            break;
        }
        if (tick_hostif_wait_ticks.v > 15000) {
            usb_epb_event = 0;
            _builtin_set_imask(15);
            if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                current_page->flags.f.scan_end = 1;
            _builtin_set_imask(saved_imask);
            usb_save_error_state();
            usb_error_code = 19;
            usb_ack_and_reset();
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->reg_17.b6 = 1;
            usb_transfer_phase = 3;
            _builtin_set_imask(saved_imask);
            usb_bulk_state = 3;
            usb_bulk_out_state = 3;
            _builtin_set_imask(15);
            usb_transfer_busy &= 0xfffd;
            _builtin_set_imask(saved_imask);
            return 0;
        }
    }
    _builtin_set_imask(15);
    usb_transfer_busy &= 0xfffd;
    _builtin_set_imask(saved_imask);
    return state;
}

int usb_bulk_receive_command(void)
{
    char *p;
    unsigned char cdb_len;
    int i;
    int j;
    unsigned short len;
    int x;
    struct page_record *current_page;
    int saved_imask;
    unsigned char status;
    current_page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    status = ((struct usb_regs_bulk *)usb_ctrl)->epa_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->epa_int_stat = status;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->reg_30.b2 = 1;
    _builtin_set_gbr(0);
    len = (((struct usb_regs_bulk *)usb_ctrl)->rd_remain_h << 8) | ((struct usb_regs_bulk *)usb_ctrl)->rd_remain_l;
    _builtin_set_gbr(0);
    ((struct usb_regs_bulk *)usb_ctrl)->reg_30.b2 = 0;
    _builtin_set_imask(saved_imask);
    if (len == 0) {
        _builtin_set_imask(15);
        if (usb_bulk_len) {
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->epb_int_enb.b4 = 1;
            usb_bulk_out_state = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->epb_control.b1 = 0;
        } else {
            usb_transfer_flags.done = 1;
            usb_bulk_out_state = 2;
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->epb_int_enb.b4 = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->epb_control.b1 = 1;
        }
        _builtin_set_imask(saved_imask);
    } else {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_bulk *)usb_ctrl)->epb_control.b1 = 1;
        _builtin_set_imask(saved_imask);
        if (usb_transfer_phase == 0) {
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->reg_30.b2 = 1;
            _builtin_set_imask(saved_imask);
            for (i = 0; i < len; i++) {
                _builtin_set_gbr(0);
                scsi_param_buf[i] = ((struct usb_regs_bulk *)usb_ctrl)->fifo_byte;
            }
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->reg_30.b2 = 0;
            _builtin_set_imask(saved_imask);
            if (i == 31 && scsi_param_buf[0] == 67) {
                for (i = 1; i < 19; i++) {
                    if (scsi_param_buf[i] != 0) {
                        _builtin_set_imask(15);
                        if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                            current_page->flags.f.scan_end = 1;
                        _builtin_set_imask(saved_imask);
                        usb_save_error_state();
                        usb_error_code = 20;
                        _builtin_set_imask(15);
                        usb_irq_code = 0;
                        _builtin_set_gbr(0);
                        ((struct usb_regs_bulk *)usb_ctrl)->reg_17.b6 = 1;
                        usb_bulk_state = 3;
                        usb_bulk_out_state = 3;
                        usb_transfer_phase = 3;
                        _builtin_set_imask(saved_imask);
                        return 0;
                    }
                }
            } else {
                _builtin_set_imask(15);
                if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                    current_page->flags.f.scan_end = 1;
                _builtin_set_imask(saved_imask);
                usb_save_error_state();
                usb_error_code = 21;
                _builtin_set_imask(15);
                usb_irq_code = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->reg_17.b6 = 1;
                usb_bulk_state = 3;
                usb_bulk_out_state = 3;
                usb_transfer_phase = 3;
                _builtin_set_imask(saved_imask);
                return 0;
            }
            scsi_sense_information.v = 0;
            cdb_len = *(volatile unsigned char *)&scsi_param_byte_19;
            scsi_identify_message = 128;
            scsi_disconnected_initiator.v = 0;
            scsi_cdb[0] = cdb_len;
            usb_check_cdb(&cdb_len);
            for (i = 1, j = 20; i < 12; i++, j++)
                scsi_cdb[i] = scsi_param_buf[j];
            usb_transfer_flags.done = 0;
            usb_transfer_flags.pending = 1;
            usb_wake_events.f.transfer_armed = 1;
            usb_bulk_out_state = 2;
            _builtin_set_imask(15);
            usb_transfer_phase = 1;
            usb_irq_code = 35;
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->epb_int_enb.b4 = 0;
            _builtin_set_imask(saved_imask);
            return 1;
        } else {
            if (usb_bulk_len < len) {
                _builtin_set_imask(15);
                if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                    current_page->flags.f.scan_end = 1;
                _builtin_set_imask(saved_imask);
                usb_save_error_state();
                usb_error_code = 22;
                _builtin_set_imask(15);
                usb_irq_code = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->reg_17.b6 = 1;
                usb_transfer_phase = 3;
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->epb_int_enb.b4 = 0;
                _builtin_set_imask(saved_imask);
                usb_bulk_state = 3;
                usb_bulk_out_state = 3;
                return 0;
            } else {
            p = usb_bulk_out_ptr;
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->reg_30.b2 = 1;
            _builtin_set_imask(saved_imask);
            for (i = 0; i < len; i++) {
                _builtin_set_gbr(0);
                *p++ = ((struct usb_regs_bulk *)usb_ctrl)->fifo_byte;
            }
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->reg_30.b2 = 0;
            _builtin_set_imask(saved_imask);
            usb_bulk_len -= len;
            usb_bulk_out_ptr = p;
            _builtin_set_imask(15);
            if (usb_bulk_len) {
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->epb_int_enb.b4 = 1;
                usb_bulk_out_state = 1;
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->epb_control.b1 = 0;
            } else {
                usb_transfer_flags.done = 1;
                usb_bulk_out_state = 2;
                _builtin_set_gbr(0);
                ((struct usb_regs_bulk *)usb_ctrl)->epb_int_enb.b4 = 0;
            }
            _builtin_set_imask(saved_imask);
            }
        }
    }
    return 1;
}

int usb_bulk_out_wait(void)
{
    int x;
    int y;
    struct page_record *current_page;
    int saved_imask;
    current_page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    if (usb_epb_event == USB_EVENT_OUT_ACK) {
        usb_epb_event = 0;
        usb_bulk_wait_count = 0;
        usb_bulk_out_state = 0;
    } else if (usb_epb_event == USB_EVENT_OUT_ERROR) {
        usb_epb_event = 0;
        ++usb_bulk_wait_count;
        if (usb_bulk_wait_count > 255) {
            usb_bulk_wait_count = 0;
            _builtin_set_imask(15);
            if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                current_page->flags.f.scan_end = 1;
            _builtin_set_imask(saved_imask);
            usb_save_error_state();
            usb_error_code = 23;
            _builtin_set_imask(15);
            usb_irq_code = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_bulk *)usb_ctrl)->reg_17.b6 = 1;
            usb_transfer_phase = 3;
            _builtin_set_imask(saved_imask);
            usb_bulk_state = 3;
            usb_bulk_out_state = 3;
            return 0;
        }
    } else {
    }
    return 1;
}

int usb_check_cdb(unsigned char *cdb_len)
{
    *cdb_len = (scsi_cdb[0] & 224) >> 5;
    switch (*cdb_len) {
    case 0:
        *cdb_len = 6;
        break;
    case 1:
    case 2:
        *cdb_len = 10;
        break;
    case 5:
        *cdb_len = 12;
        break;
    default:
        if (scsi_cdb[0] == SCSI_OP_ENDORSER || scsi_cdb[0] == SCSI_OP_GET_HW_STATUS || scsi_cdb[0] == SCSI_OP_SCANNER_CONTROL) {
            *cdb_len = 10;
        } else {
            *cdb_len = 6;
            return 0;
        }
    }
    return 1;
}
