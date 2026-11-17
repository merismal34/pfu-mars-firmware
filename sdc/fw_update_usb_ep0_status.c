#include "boot.h"
#pragma section 2
struct byte_bits_1_0 { unsigned char p:6, f:2; };
struct usb_regs_ep0_status {
    char pad_00;
    union byte_bits main_int_stat;
    char p[5];
    union byte_bits ep0_int_stat;
    char reg_04_07[9];
    char main_int_enb;
    char pad_12;
    char sie_int_enb;
    char pad_14;
    char reg_0a;
    char pad_16;
    char ep0_int_enb;
    char pad_18;
    char epa_int_enb;
    char pad_1a;
    char epb_int_enb;
    char pad_1c;
    char reg_0e;
    char reg_0f_12[9];
    unsigned char usb_control;
    char reg_14[3];
    struct byte_bits_1_0 xcvr_control;
    char pad_2c;
    struct Bits usb_test;
    char pad_2e;
    struct Bits reg_17;
    char q[35];
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
    char fifo_byte;
};

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

void fw_update_usb_ep0_status(void)
{
    unsigned short len;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    if (fw_update_usb_ctrl_flags.b2 == 0) {
        fw_update_usb_ctrl_flags.b2 = 1;
        _builtin_set_imask(15);
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_int_stat.c = ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_int_stat.c;
        _builtin_set_imask(saved_imask);
        fw_update_sync_dst = fw_update_sync_src;
        if (((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control.b7) {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control.b7 = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_out.b1 = 0;
        } else {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control.b7 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->reg_17.b0 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_in.b1 = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_in.b6 = 1;
        }
    } else if (fw_update_usb_ep0_event == USB_EVENT_IN_ACK || fw_update_usb_ep0_event == USB_EVENT_OUT_ACK) {
        fw_update_usb_ep0_event = 0;
        fw_update_usb_bulk_wait_count = 0;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->reg_30.b0 = 1;
        fw_update_sync_dst = fw_update_sync_src;
        len = (((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->rd_remain_h << 8) | ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->rd_remain_l;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->reg_30.b0 = 0;
        if (len != 0) {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_in.b0 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_out.b0 = 1;
            fw_update_usb_ep0_stage = 7;
        } else {
            if (fw_update_usb_ctrl_flags.b4) {
                fw_update_usb_ctrl_flags.b4 = 0;
                fw_update_sync_dst = fw_update_sync_src;
                if (((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->usb_test.b2 || ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->usb_test.b1) {
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->xcvr_control.f = 2;
                }
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->usb_test.b7 = 1;
                fw_update_usb_ep0_status_step_1();
            }
            fw_update_usb_ep0_stage = 7;
        }
    } else if (fw_update_usb_ep0_event == USB_EVENT_IN_ERROR || fw_update_usb_ep0_event == USB_EVENT_OUT_ERROR) {
        fw_update_usb_ep0_event = 0;
        ++fw_update_usb_bulk_wait_count;
        if (fw_update_usb_bulk_wait_count > 255) {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_in.b0 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_out.b0 = 1;
            fw_update_usb_ep0_stage = 7;
            fw_update_usb_bulk_wait_count = 0;
        }
    } else if (fw_update_usb_ctrl_flags.b7) {
        fw_update_usb_ep0_stage = 0;
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        if (((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control.b7) {
            fw_update_sync_dst = fw_update_sync_src;
            if (((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_int_stat.b.b2 && !(fw_update_usb_ep0_int_status.v & 51) && ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->main_int_stat.b.b0 == 0) {
                fw_update_usb_bulk_wait_count = 0;
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control.b7 = 0;
                if (fw_update_usb_ep0_wlength != 0) {
                    if (fw_update_usb_ep0_data_len != 0) {
                        fw_update_sync_dst = fw_update_sync_src;
                        ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_in.b0 = 1;
                        fw_update_sync_dst = fw_update_sync_src;
                        ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_out.b0 = 1;
                        fw_update_usb_ep0_stage = 7;
                    } else {
                        fw_update_sync_dst = fw_update_sync_src;
                        ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_out.b1 = 0;
                        fw_update_usb_ep0_stage = 3;
                    }
                } else {
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_in.b0 = 1;
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_out.b0 = 1;
                    fw_update_usb_ep0_stage = 7;
                }
            }
        } else {
            fw_update_sync_dst = fw_update_sync_src;
            if (((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_int_stat.b.b3 && !(fw_update_usb_ep0_int_status.v & 51) && ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->main_int_stat.b.b0 == 0) {
                fw_update_usb_bulk_wait_count = 0;
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control.b7 = 1;
                if (fw_update_usb_ep0_data_len != 0) {
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_in.b0 = 1;
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_out.b0 = 1;
                    fw_update_usb_ep0_stage = 7;
                } else {
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_in.b1 = 0;
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_control_in.b6 = 1;
                    fw_update_usb_ep0_stage = 2;
                }
            }
        }
    }
}

void fw_update_usb_ep0_status_step_1(void)
{
    char x;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->main_int_enb = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->sie_int_enb = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->reg_0a = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->ep0_int_enb = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->epa_int_enb = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->epb_int_enb = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->reg_0e = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ep0_status *)fw_update_p_usb_ctrl)->usb_control = 0x81;
    while (1) {
    }
}

void fw_update_finish_record(struct fw_update_command_link *record)
{
    if (fw_update_current_record.p != fw_update_disconnected_record.p) {
        fw_update_release_record(record);
    } else {
        fw_update_clear_current_record(record);
    }
}
