#include "boot.h"
#pragma section 2
struct usb_regs_ep0_stages {
    char pad_00;
    union byte_bits main_int_stat;
    char p[5];
    union byte_bits ep0_int_stat;
    char reg_04_16[39];
    struct Bits reg_17;
    char q[17];
    unsigned char ep0_setup_0;
    char reg_21_25[11];
    unsigned char ep0_setup_6;
    char pad_4e;
    unsigned char ep0_setup_7;
    char reg_28[3];
    struct Bits ep0_control;
    char pad_54;
    struct Bits ep0_control_in;
    char pad_56;
    struct Bits ep0_control_out;
    char reg_2c_2f[9];
    struct Bits reg_30;
    char pad_62;
    struct Bits reg_31;
    char pad_64;
    unsigned char rd_remain_h;
    char pad_66;
    unsigned char rd_remain_l;
    char reg_34_36[7];
    char fifo_byte;
};

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

void fw_update_usb_ep0_setup(void)
{
    int stall;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_usb_ctrl_flags.b6 = 0;
    fw_update_usb_ctrl_flags.b7 = 0;
    fw_update_usb_ctrl_flags.b2 = 0;
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_17.b0 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_in.b6 = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_int_stat.c = ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_int_stat.c;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->main_int_stat.c = 1;
    _builtin_set_imask(saved_imask);
    fw_update_sync_dst = fw_update_sync_src;
    if (((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_setup_0 & 0x80) {
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control.b7 = 1;
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control.b7 = 0;
    }
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_usb_ep0_wlength = ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_setup_6 | (((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_setup_7 << 8);
    fw_update_sync_dst = fw_update_sync_src;
    switch (((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_setup_0 & 0x60) {
    case 0:
        stall = fw_update_usb_run_std_request();
        break;
    default:
        stall = 1;
        break;
    }
    if (stall == 0) {
        if (fw_update_usb_ep0_wlength != 0) {
            fw_update_sync_dst = fw_update_sync_src;
            if (((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control.b7) {
                fw_update_usb_ep0_stage = 1;
            } else {
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_out.b1 = 0;
                fw_update_usb_ep0_stage = 4;
            }
        } else {
            fw_update_usb_ep0_stage = 6;
        }
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_in.b0 = 1;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_out.b0 = 1;
        fw_update_usb_ep0_stage = 7;
    }
}

void fw_update_usb_ep0_data_in(void)
{
    int i;

    if (fw_update_usb_ep0_data_len >= 64) {
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_31.b0 = 1;
        for (i = 0; i < 64; i++) {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->fifo_byte = *fw_update_usb_ep0_data_ptr;
            fw_update_usb_ep0_data_ptr++;
        }
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_31.b0 = 0;
        fw_update_usb_ep0_data_len -= 64;
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_31.b0 = 1;
        for (i = fw_update_usb_ep0_data_len; i > 0; i--) {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->fifo_byte = *fw_update_usb_ep0_data_ptr;
            fw_update_usb_ep0_data_ptr++;
        }
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_31.b0 = 0;
        fw_update_usb_ep0_data_len = 0;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_in.b6 = 1;
    }
    fw_update_usb_ep0_stage = 2;
    fw_update_usb_ep0_event = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_in.b1 = 0;
}

void fw_update_usb_ep0_wait_in(void)
{
    if (fw_update_usb_ep0_event == USB_EVENT_IN_ACK) {
        fw_update_usb_ep0_event = 0;
        fw_update_usb_bulk_wait_count = 0;
        if (fw_update_usb_ep0_data_len != 0) {
            fw_update_usb_ep0_stage = 1;
        } else if (fw_update_usb_ep0_wlength == fw_update_usb_ep0_done_len) {
            fw_update_usb_ep0_stage = 6;
        } else if (fw_update_usb_ep0_done_len & 63) {
            fw_update_usb_ep0_stage = 6;
        } else {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_in.b6 = 1;
        }
    } else if (fw_update_usb_ep0_event == USB_EVENT_IN_ERROR) {
        fw_update_usb_ep0_event = 0;
        ++fw_update_usb_bulk_wait_count;
        if (fw_update_usb_bulk_wait_count > 255) {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_in.b0 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_out.b0 = 1;
            fw_update_usb_ep0_stage = 7;
            fw_update_usb_bulk_wait_count = 0;
        }
    } else if (fw_update_usb_ctrl_flags.b7) {
        fw_update_usb_ep0_stage = 0;
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        if (((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_int_stat.b.b2 && !(fw_update_usb_ep0_int_status.v & 51) && ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->main_int_stat.b.b0 == 0) {
            fw_update_usb_bulk_wait_count = 0;
            fw_update_usb_ep0_data_len = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_17.b0 = 1;
            fw_update_usb_ep0_stage = 6;
        }
    }
}

void fw_update_usb_ep0_wait_status(void)
{
    if (fw_update_usb_ep0_event == USB_EVENT_OUT_ACK) {
        fw_update_usb_ep0_event = 0;
        fw_update_usb_bulk_wait_count = 0;
        fw_update_usb_ep0_stage = 4;
    } else if (fw_update_usb_ep0_event == USB_EVENT_OUT_ERROR) {
        fw_update_usb_ep0_event = 0;
        ++fw_update_usb_bulk_wait_count;
        if (fw_update_usb_bulk_wait_count > 255) {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_in.b0 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_out.b0 = 1;
            fw_update_usb_ep0_stage = 7;
        }
    } else if (fw_update_usb_ctrl_flags.b7) {
        fw_update_usb_ep0_stage = 0;
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        if (((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_int_stat.b.b3 && !(fw_update_usb_ep0_int_status.v & 51) && ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->main_int_stat.b.b0 == 0) {
            fw_update_usb_bulk_wait_count = 0;
            fw_update_usb_ep0_data_len = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_17.b0 = 1;
            fw_update_usb_ep0_stage = 6;
        }
    }
}

void fw_update_usb_ep0_data_out(void)
{
    int i;
    unsigned short len;

    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_30.b0 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    len = (((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->rd_remain_h << 8) | ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->rd_remain_l;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_30.b0 = 0;
    if (len == 0) {
        if (fw_update_usb_ep0_data_len != 0) {
            if (fw_update_usb_ep0_done_len == fw_update_usb_ep0_data_len) {
                fw_update_usb_ep0_stage = 3;
            } else {
                fw_update_usb_ep0_stage = 5;
            }
        } else {
            fw_update_usb_ep0_stage = 5;
        }
    } else if (fw_update_usb_ep0_data_len < len) {
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_in.b0 = 1;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_out.b0 = 1;
        fw_update_usb_ep0_stage = 7;
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_30.b0 = 1;
        for (i = len; i > 0; i++) {
            fw_update_sync_dst = fw_update_sync_src;
            *fw_update_usb_ep0_data_ptr = ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->fifo_byte;
            fw_update_usb_ep0_data_ptr++;
        }
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->reg_30.b0 = 0;
        if (fw_update_usb_ep0_data_len -= len) {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_out.b1 = 0;
            fw_update_usb_ep0_stage = 3;
        } else {
            fw_update_usb_ep0_stage = 5;
        }
    }
}

void fw_update_usb_ep0_stall(void)
{
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_in.b0 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_stages *)fw_update_p_usb_ctrl)->ep0_control_out.b0 = 1;
    fw_update_usb_ep0_stage = 7;
}
