#include "fw.h"
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
extern void _builtin_set_gbr(void *);
struct usb_regs_ep0_setup {
    char pad_00;
    char main_int_stat;
    char reg_01_02[5];
    char ep0_int_stat;
    char reg_04_16[39];
    struct Bits reg_17;
    char reg_18_1f[17];
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
};

void usb_ep0_setup(void)
{
    int stall;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    usb_ctrl_flags.b6 = 0;
    usb_ctrl_flags.b7 = 0;
    usb_ctrl_flags.b2 = 0;
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_setup *)usb_ctrl)->reg_17.b0 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_control_in.b6 = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_int_stat = ((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_setup *)usb_ctrl)->main_int_stat = 1;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    if (((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_setup_0 & 0x80) {
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_control.b7 = 1;
    } else {
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_control.b7 = 0;
    }
    _builtin_set_gbr(0);
    usb_ep0_wlength = ((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_setup_6 | (((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_setup_7 << 8);
    _builtin_set_imask(saved_imask);
    _builtin_set_gbr(0);
    switch (((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_setup_0 & 0x60) {
    case 0:
        stall = usb_dispatch_std_request();
        break;
    default:
        stall = 1;
        break;
    }
    if (stall == 0) {
        if (usb_ep0_wlength != 0) {
            _builtin_set_gbr(0);
            if (((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_control.b7) {
                usb_ep0_stage = 1;
            } else {
                _builtin_set_gbr(0);
                ((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_control_out.b1 = 0;
                usb_ep0_stage = 4;
            }
        } else {
            usb_ep0_stage = 6;
        }
    } else {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_control_in.b0 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_setup *)usb_ctrl)->ep0_control_out.b0 = 1;
        _builtin_set_imask(saved_imask);
        usb_ep0_stage = 7;
    }
}
