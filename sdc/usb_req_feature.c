#include "fw.h"
extern void _builtin_set_gbr(void *);
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
struct usb_reg_85_bits {
    unsigned char dir_in:1,
                  :1,
                  enabled:1,
                  :1,
                  number:4;
};
union usb_setup_byte { unsigned char c; struct Bits f; };
struct usb_regs_req_feature {
    char reg_00_0d[29];
    struct Bits reg_0e;
    char reg_0f_13[11];
    struct Bits usb_status;
    char reg_15[3];
    struct Bits usb_test;
    char pad_2e;
    struct Bits reg_17;
    char reg_18_1e[15];
    unsigned char usb_bank;
    char pad_40;
    unsigned char ep0_setup_0;
    char reg_21[3];
    union usb_setup_byte reg_22;
    char pad_46;
    unsigned char ep0_setup_3;
    char pad_48;
    char ep0_setup_4;
    char pad_4a;
    unsigned char ep0_setup_5;
    char pad_4c;
    union usb_setup_byte reg_26;
    char pad_4e;
    unsigned char ep0_setup_7;
    char pad_50;
    unsigned char epc_max_size_h;
    char pad_52;
    unsigned char epc_max_size_l;
    char pad_54;
    struct usb_reg_85_bits epc_config_0;
    char reg_2b[3];
    struct Bits epa_control;
    char pad_5a;
    struct Bits epb_control;
    char pad_5c;
    struct Bits epc_control;
    char reg_2f_30[5];
    struct Bits reg_31;
    char reg_32_33[5];
    unsigned char reg_34;
    char pad_6a;
    unsigned char reg_35;
    char pad_6c;
    unsigned char reg_36;
    char pad_6e;
    unsigned char reg_37;
};

int usb_req_set_feature(void)
{
    int i;
    int stall;
    stall = 0;
    _builtin_set_gbr(0);
    switch (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_0 & 31) {
    case 0:
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_3) { stall = 1; break; }
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_feature *)usb_ctrl)->reg_22.c == 1) {
            if (usb_device_state == USB_STATE_DEFAULT) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_5) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_4) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_7) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_feature *)usb_ctrl)->reg_26.c) { stall = 1; break; }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_feature *)usb_ctrl)->usb_status.b6) {
                if (!(usb_config_attributes_fs.v & 32)) stall = 1;
            } else {
                if (!(usb_config_attributes_hs.v & 32)) stall = 1;
            }
            if (stall == 0) usb_ctrl_flags.b0 = 1;
        } else {
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_feature *)usb_ctrl)->reg_22.c == 2) {
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_feature *)usb_ctrl)->usb_status.b6) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_4) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_7) { stall = 1; break; }
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_feature *)usb_ctrl)->reg_26.c) { stall = 1; break; }
                _builtin_set_gbr(0);
                switch (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_5) {
                case 1:
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->usb_test.b2 = 1;
                    break;
                case 2:
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->usb_test.b1 = 1;
                    break;
                case 3:
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->usb_test.b3 = 1;
                    break;
                case 4:
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->usb_bank = 2;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->epc_max_size_h = 0;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->epc_max_size_l = 64;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->epc_config_0.dir_in = 1;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->epc_config_0.number = 15;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->reg_22.f.b5 = 0;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->reg_26.f.b5 = 0;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->epc_config_0.enabled = 1;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->reg_34 = 9;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->reg_35 = 192;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->reg_36 = 10;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->reg_37 = 0;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->usb_bank = 0;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->reg_0e.b1 = 0;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->reg_17.b5 = 1;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->reg_31.b3 = 1;
                    for (i = 0; i < 53; i++) {
                        _builtin_set_gbr(0);
                        ((struct usb_regs_req_feature *)usb_ctrl)->reg_37 = usb_test_packet[i];
                    }
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->reg_31.b3 = 0;
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->usb_test.b0 = 1;
                    break;
                default:
                    stall = 1;
                    break;
                }
                if (stall == 0) usb_ctrl_flags.b4 = 1;
            } else {
                stall = 1;
            }
        }
        break;
    case 2:
        if (usb_device_state == USB_STATE_DEFAULT) { stall = 1; break; }
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_3) { stall = 1; break; }
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_feature *)usb_ctrl)->reg_22.c) { stall = 1; break; }
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_5) { stall = 1; break; }
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_7) { stall = 1; break; }
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_feature *)usb_ctrl)->reg_26.c) { stall = 1; break; }
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_4 == 0) {
            stall = 1;
            break;
        } else if (usb_device_state == USB_STATE_ADDRESS) {
            stall = 1;
            break;
        }
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_4 == usb_epa_address[0]) {
            _builtin_set_gbr(0);
            if (!((struct usb_regs_req_feature *)usb_ctrl)->epa_control.b0) {
                _builtin_set_gbr(0);
                ((struct usb_regs_req_feature *)usb_ctrl)->epa_control.b0 = 1;
                usb_bulk_state = 3;
            }
        } else {
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_4 == usb_epb_address[0]) {
                _builtin_set_gbr(0);
                if (!((struct usb_regs_req_feature *)usb_ctrl)->epb_control.b0) {
                    _builtin_set_gbr(0);
                    ((struct usb_regs_req_feature *)usb_ctrl)->epb_control.b0 = 1;
                    usb_bulk_out_state = 3;
                }
            } else {
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_4 == usb_epc_address[0]) {
                    _builtin_set_gbr(0);
                    if (!((struct usb_regs_req_feature *)usb_ctrl)->epc_control.b0) {
                        _builtin_set_gbr(0);
                        ((struct usb_regs_req_feature *)usb_ctrl)->epc_control.b0 = 1;
                    }
                } else {
                    stall = 1;
                }
            }
        }
        break;
    default:
        stall = 1;
    }
    return stall;
}

int usb_req_set_address(void)
{
    int x;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    if (usb_device_state == USB_STATE_CONFIGURED) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_0 & 31) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_3) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_5) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_4) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_feature *)usb_ctrl)->ep0_setup_7) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_feature *)usb_ctrl)->reg_26.c) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_feature *)usb_ctrl)->reg_22.c > 127) return 1;
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    usb_address = ((struct usb_regs_req_feature *)usb_ctrl)->reg_22.c;
    usb_ctrl_flags.b5 = 1;
    _builtin_set_imask(saved_imask);
    usb_suspended = 0;
    return 0;
}
