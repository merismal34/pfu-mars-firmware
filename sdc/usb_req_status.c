#include "fw.h"
extern void _builtin_set_gbr(void *);
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
struct usb_regs_req_status {
    char reg_00_13[41];
    struct Bits usb_status;
    char reg_15_16[5];
    struct Bits reg_17;
    char reg_18_1f[17];
    unsigned char ep0_setup_0;
    char pad_42;
    unsigned char ep0_setup_1;
    char pad_44;
    char ep0_setup_2;
    char pad_46;
    char ep0_setup_3;
    char pad_48;
    char ep0_setup_4;
    char pad_4a;
    char ep0_setup_5;
    char pad_4c;
    unsigned char ep0_setup_6;
    char pad_4e;
    char ep0_setup_7;
    char reg_28_29[5];
    struct Bits ep0_control_in;
    char pad_56;
    struct Bits ep0_control_out;
    char pad_58;
    struct Bits epa_control;
    char pad_5a;
    struct Bits epb_control;
    char pad_5c;
    struct Bits epc_control;
};

int usb_req_get_status(void)
{
    int stall;
    stall = 0;
    if (usb_device_state == USB_STATE_DEFAULT) {
        stall = 1;
    } else {
        _builtin_set_gbr(0);
        switch (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_0 & 0x1f) {
        case 0:
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_3) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_2) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_5) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_7) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_6 != 2) { stall = 1; break; }
            usb_ep0_reply_buf[0] = usb_ctrl_flags.b0 * 2 + 1;
            usb_ep0_reply_byte_1[0] = 0;
            usb_ep0_done_len = usb_ep0_data_len = 2;
            usb_ep0_data_ptr = usb_ep0_reply_buf;
            break;
        case 1:
            if (usb_device_state == USB_STATE_ADDRESS) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_3) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_2) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_5) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_7) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_6 != 2) { stall = 1; break; }
            usb_ep0_reply_buf[0] = usb_ep0_reply_byte_1[0] = 0;
            usb_ep0_done_len = usb_ep0_data_len = 2;
            usb_ep0_data_ptr = usb_ep0_reply_buf;
            break;
        case 2:
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_3) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_2) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_5) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_7) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_6 != 2) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4 == 0) {
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_control_in.b0)
                    usb_ep0_reply_buf[0] = 1;
                else
                    _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_control_out.b0)
                    usb_ep0_reply_buf[0] = 1;
                else
                    usb_ep0_reply_buf[0] = 0;
                usb_ep0_reply_byte_1[0] = 0;
                usb_ep0_done_len = usb_ep0_data_len = 2;
                usb_ep0_data_ptr = usb_ep0_reply_buf;
            } else {
            if (usb_device_state == USB_STATE_ADDRESS) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4 == usb_epa_address[0]) {
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->epa_control.b0)
                    usb_ep0_reply_buf[0] = 1;
                else
                    usb_ep0_reply_buf[0] = 0;
            } else {
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4 == usb_epb_address[0]) {
                    _builtin_set_gbr(0);
                    if (((struct usb_regs_req_status *)usb_ctrl)->epb_control.b0)
                        usb_ep0_reply_buf[0] = 1;
                    else
                        usb_ep0_reply_buf[0] = 0;
                } else {
                    _builtin_set_gbr(0);
                    if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4 == usb_epc_address[0]) {
                        _builtin_set_gbr(0);
                        if (((struct usb_regs_req_status *)usb_ctrl)->epc_control.b0)
                            usb_ep0_reply_buf[0] = 1;
                        else
                            usb_ep0_reply_buf[0] = 0;
                    } else {
                        stall = 1;
                    }
                }
            }
            if (stall == 0) {
                usb_ep0_reply_byte_1[0] = 0;
                usb_ep0_done_len = usb_ep0_data_len = 2;
                usb_ep0_data_ptr = usb_ep0_reply_buf;
            }
            }
            break;
        default:
            stall = 1;
        }
    }
    return stall;
}

int usb_req_clear_feature(void)
{
    int stall;
    int saved_imask;
    stall = 0;
    saved_imask = _builtin_get_imask();
    if (usb_device_state == USB_STATE_DEFAULT) {
        stall = 1;
    } else {
        _builtin_set_gbr(0);
        switch (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_0 & 0x1f) {
        case 0:
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_3) { stall = 1; break; }
            _builtin_set_gbr(0);
            if ((unsigned char)((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_2 == 1) {
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_5) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_7) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_6) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->usb_status.b6) {
                    if ((usb_config_attributes_fs.v & 0x20) == 0) { stall = 1; break; }
                } else {
                    if ((usb_config_attributes_hs.v & 0x20) == 0) { stall = 1; break; }
                }
                usb_ctrl_flags.b0 = 0;
            } else {
              _builtin_set_gbr(0);
              if ((unsigned char)((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_2 == 2) {
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->usb_status.b6) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_5) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_7) stall = 1;
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_6) stall = 1;
              } else {
                stall = 1;
              }
            }
            break;
        case 2:
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_3) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_2 != 0) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_5) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_7) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_6) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4 == 0) {
                stall = 1;
            } else {
                if (usb_device_state == USB_STATE_ADDRESS) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4 == usb_epa_address[0]) {
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_status *)usb_ctrl)->epa_control.b2 = 1;
                    _builtin_set_gbr(0);
                    if (((struct usb_regs_req_status *)usb_ctrl)->epa_control.b0) {
                        _builtin_set_gbr(0);
                        ((struct usb_regs_req_status *)usb_ctrl)->reg_17.b1 = 1;
                        if (usb_transfer_phase != 3) {
                            _builtin_set_gbr(0);
                            ((struct usb_regs_req_status *)usb_ctrl)->epa_control.b0 = 0;
                            usb_bulk_state = 2;
                            if (usb_transfer_phase == 1) {
                                _builtin_set_imask(15);
                                usb_transfer_phase = 2;
                                _builtin_set_imask(saved_imask);
                                usb_transfer_flags.done = 1;
                            }
                        }
                    }
                } else {
                    _builtin_set_gbr(0);
                    if (((struct usb_regs_req_status *)usb_ctrl)->ep0_setup_4 == usb_epb_address[0]) {
                        _builtin_set_gbr(0);
                        ((struct usb_regs_req_status *)usb_ctrl)->epb_control.b2 = 1;
                        _builtin_set_gbr(0);
                        if (((struct usb_regs_req_status *)usb_ctrl)->epb_control.b0) {
                            _builtin_set_gbr(0);
                            ((struct usb_regs_req_status *)usb_ctrl)->reg_17.b2 = 1;
                            if (usb_transfer_phase != 3) {
                                _builtin_set_gbr(0);
                                ((struct usb_regs_req_status *)usb_ctrl)->epb_control.b0 = 0;
                                usb_bulk_out_state = 2;
                                if (usb_transfer_phase == 1) {
                                    _builtin_set_imask(15);
                                    usb_transfer_phase = 2;
                                    _builtin_set_imask(saved_imask);
                                    usb_transfer_flags.done = 1;
                                }
                            }
                        }
                    }
                }
            }
            break;
        default:
            stall = 1;
        }
    }
    return stall;
}
