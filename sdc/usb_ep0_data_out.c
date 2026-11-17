#include "fw.h"
extern int _builtin_get_imask(void);
extern void _builtin_set_imask(int);
extern void _builtin_set_gbr(void *);
struct usb_regs_ep0_data_out {
    char pad_00;
    struct Bits main_int_stat;
    char reg_01_02[5];
    union { char v; struct Bits f; } ep0_int_stat;
    char reg_04_07[9];
    char main_int_enb, pad_12, sie_int_enb, pad_14, reg_0a, pad_16, ep0_int_enb, pad_18, epa_int_enb, pad_1a, epb_int_enb, pad_1c, reg_0e;
    char reg_0f_12[9];
    unsigned char usb_control;
    char reg_14[3];
    struct usb_reg_43_bits xcvr_control;
    char pad_2c;
    struct Bits usb_test;
    char pad_2e;
    struct Bits reg_17;
    char reg_18_28[35];
    struct Bits ep0_control;
    char pad_54;
    struct Bits ep0_control_in;
    char pad_56;
    struct Bits ep0_control_out;
    char reg_2c_2f[9];
    struct Bits reg_30;
    char reg_31[3];
    unsigned char rd_remain_h;
    char pad_66;
    unsigned char rd_remain_l;
    char reg_34_36[7];
    unsigned char fifo_byte;
};


void usb_ep0_data_out(void)
{
    int i;
    unsigned short len;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->reg_30.b0 = 1;
    _builtin_set_gbr(0);
    len = (((struct usb_regs_ep0_data_out *)usb_ctrl)->rd_remain_h << 8) | ((struct usb_regs_ep0_data_out *)usb_ctrl)->rd_remain_l;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->reg_30.b0 = 0;
    _builtin_set_imask(saved_imask);
    if (len == 0) {
        if (usb_ep0_data_len) {
            if (usb_ep0_done_len == usb_ep0_data_len)
                usb_ep0_stage = 3;
            else
                usb_ep0_stage = 5;
        } else {
            usb_ep0_stage = 5;
        }
    } else if (usb_ep0_data_len < len) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b0 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_out.b0 = 1;
        _builtin_set_imask(saved_imask);
        usb_ep0_stage = 7;
    } else {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_out *)usb_ctrl)->reg_30.b0 = 1;
        _builtin_set_imask(saved_imask);
        i = len;
        while (i > 0) {
            _builtin_set_gbr(0);
            *usb_ep0_data_ptr = ((struct usb_regs_ep0_data_out *)usb_ctrl)->fifo_byte;
            usb_ep0_data_ptr++;
            i--;
        }
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_out *)usb_ctrl)->reg_30.b0 = 0;
        _builtin_set_imask(saved_imask);
        usb_ep0_data_len -= len;
        if (usb_ep0_data_len) {
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_out.b1 = 0;
            _builtin_set_imask(saved_imask);
            usb_ep0_stage = 3;
        } else {
            usb_ep0_stage = 5;
        }
    }
}

void usb_ep0_stall(void)
{
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b0 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_out.b0 = 1;
    usb_ep0_stage = 7;
}

void usb_ep0_status(void)
{
    unsigned short len;
    int saved_imask;
    char status;
    saved_imask = _builtin_get_imask();
    if (!usb_ctrl_flags.b2) {
        usb_ctrl_flags.b2 = 1;
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        status = ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_int_stat.v;
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_int_stat.v = status;
        _builtin_set_imask(saved_imask);
        _builtin_set_gbr(0);
        if (((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control.b7) {
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control.b7 = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_out.b1 = 0;
        } else {
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control.b7 = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->reg_17.b0 = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b1 = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b6 = 1;
        }
    } else if (usb_ep0_event == USB_EVENT_IN_ACK || usb_ep0_event == USB_EVENT_OUT_ACK) {
        usb_ep0_event = 0;
        usb_bulk_wait_count = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_out *)usb_ctrl)->reg_30.b0 = 1;
        _builtin_set_gbr(0);
        len = (((struct usb_regs_ep0_data_out *)usb_ctrl)->rd_remain_h << 8) | ((struct usb_regs_ep0_data_out *)usb_ctrl)->rd_remain_l;
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_out *)usb_ctrl)->reg_30.b0 = 0;
        if (len) {
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b0 = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_out.b0 = 1;
            usb_ep0_stage = 7;
        } else {
            if (usb_ctrl_flags.b4) {
                usb_ctrl_flags.b4 = 0;
                _builtin_set_gbr(0);
                _builtin_set_gbr(0);
                if (((struct usb_regs_ep0_data_out *)usb_ctrl)->usb_test.b2) {
                    _builtin_set_gbr(0);
                    ((struct usb_regs_ep0_data_out *)usb_ctrl)->xcvr_control.op_mode = 2;
                }
                _builtin_set_gbr(0);
                if (((struct usb_regs_ep0_data_out *)usb_ctrl)->usb_test.b1) {
                    _builtin_set_gbr(0);
                    ((struct usb_regs_ep0_data_out *)usb_ctrl)->xcvr_control.op_mode = 2;
                }
                _builtin_set_gbr(0);
                ((struct usb_regs_ep0_data_out *)usb_ctrl)->usb_test.b7 = 1;
                usb_enter_test_mode();
            }
            usb_ep0_stage = 7;
        }
    } else if (usb_ep0_event == USB_EVENT_IN_ERROR || usb_ep0_event == USB_EVENT_OUT_ERROR) {
        usb_ep0_event = 0;
        usb_bulk_wait_count++;
        if (usb_bulk_wait_count > 255) {
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b0 = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_out.b0 = 1;
            usb_ep0_stage = 7;
            usb_bulk_wait_count = 0;
        }
    } else if (usb_ctrl_flags.b7) {
        usb_ep0_stage = 0;
    } else {
        _builtin_set_gbr(0);
        if (((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control.b7) {
            _builtin_set_gbr(0);
            if (((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_int_stat.f.b2 && (usb_ep0_int_status.v & 0x33) == 0 && ((struct usb_regs_ep0_data_out *)usb_ctrl)->main_int_stat.b0 == 0) {
                usb_bulk_wait_count = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control.b7 = 0;
                if (usb_ep0_wlength) {
                    if (usb_ep0_data_len) {
                        _builtin_set_gbr(0);
                        ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b0 = 1;
                        _builtin_set_gbr(0);
                        ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_out.b0 = 1;
                        usb_ep0_stage = 7;
                    } else {
                        _builtin_set_gbr(0);
                        ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_out.b1 = 0;
                        usb_ep0_stage = 3;
                    }
                } else {
                    _builtin_set_gbr(0);
                    ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b0 = 1;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_out.b0 = 1;
                    usb_ep0_stage = 7;
                }
            }
        } else {
            _builtin_set_gbr(0);
            if (((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_int_stat.f.b3 && (usb_ep0_int_status.v & 0x33) == 0 && ((struct usb_regs_ep0_data_out *)usb_ctrl)->main_int_stat.b0 == 0) {
                usb_bulk_wait_count = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control.b7 = 1;
                if (usb_ep0_data_len) {
                    _builtin_set_gbr(0);
                    ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b0 = 1;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_out.b0 = 1;
                    usb_ep0_stage = 7;
                } else {
                    _builtin_set_gbr(0);
                    ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b1 = 0;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_control_in.b6 = 1;
                    usb_ep0_stage = 2;
                }
            }
        }
    }
}

void usb_enter_test_mode(void)
{
    int unused;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->main_int_enb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->sie_int_enb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->reg_0a = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->ep0_int_enb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->epa_int_enb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->epb_int_enb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->reg_0e = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_out *)usb_ctrl)->usb_control = 0x81;
    _builtin_set_imask(saved_imask);
    while (1)
        ;
}

void usb_send_status(void)
{
    unsigned char i;
    for (i = 0; i < 13; i++)
        scsi_reply_buf[i] = usb_status_template[i];
    scsi_reply_byte_9[0] = scsi_status_byte;
    usb_set_status_len();
    usb_start_transfer();
}

void usb_send_reply_data(void)
{
    char *src;
    if (scsi_cdb[0] == SCSI_OP_GET_HW_STATUS) {
        src = scsi_reply_buf;
    } else if (flash_download_targets[flash_download_session & 3].read_pending != 0) {
        flash_download_targets[flash_download_session & 3].read_pending = 0;
        src = flash_download_buf;
    } else {
        if (scsi_defect_transfer.defect_area)
            src = &flash_defect_area;
        else
            src = scsi_reply_buf;
    }
    usb_bulk_in_ptr = src;
    usb_set_transfer_len(scsi_param_len.v);
    usb_start_transfer();
    scsi_status = SCSI_STEP_GOOD;
}

int usb_receive_params(void)
{
    int unused;
    if ((flash_download_session & 0x80) && scsi_param_len.v > 0x2000) {
        usb_bulk_out_ptr = flash_download_buf;
    } else if (scsi_defect_transfer.defect_area && scsi_param_len.v > 0x500) {
        usb_bulk_out_ptr = scsi_param_buf_large;
    } else {
        usb_bulk_out_ptr = (char *)scsi_param_buf;
    }
    usb_start_receive(scsi_param_len.v);
    usb_bulk_receive_data();
    return 1;
}

void usb_send_cdb_alloc_len(void)
{
    if (scsi_cdb_byte_4[0]) {
        scsi_param_len.v = scsi_cdb_byte_4[0];
        usb_set_transfer_len(scsi_param_len.v);
        usb_start_transfer();
    } else {
        usb_start_data_in();
    }
}

void usb_send_reply(unsigned int alloc_len)
{
    if (alloc_len) {
        scsi_param_len.v = alloc_len;
        util_mem_clear(scsi_reply_buf, 8);
        usb_send_reply_data();
    } else {
        usb_start_data_in();
    }
    scsi_status = SCSI_STEP_CHECK_CONDITION;
}
