#include "fw.h"
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
extern void _builtin_set_gbr(void *);
struct usb_regs_ep0_data_in {
    char reg_00_29[85];
    struct Bits ep0_control_in;
    char reg_2b_30[13];
    struct Bits reg_31;
    char reg_32_36[11];
    char fifo_byte;
};

void usb_ep0_data_in(void)
{
    int i;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    if (usb_ep0_data_len >= 64) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_in *)usb_ctrl)->reg_31.b0 = 1;
        _builtin_set_imask(saved_imask);
        for (i = 0; i < 64; i++) {
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_in *)usb_ctrl)->fifo_byte = *usb_ep0_data_ptr;
            usb_ep0_data_ptr++;
        }
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_in *)usb_ctrl)->reg_31.b0 = 0;
        _builtin_set_imask(saved_imask);
        usb_ep0_data_len -= 64;
    } else {
        _builtin_set_gbr(0);
        _builtin_set_imask(15);
        ((struct usb_regs_ep0_data_in *)usb_ctrl)->reg_31.b0 = 1;
        _builtin_set_imask(saved_imask);
        i = usb_ep0_data_len;
        while (i > 0) {
            _builtin_set_gbr(0);
            ((struct usb_regs_ep0_data_in *)usb_ctrl)->fifo_byte = *usb_ep0_data_ptr;
            usb_ep0_data_ptr++;
            i--;
        }
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_in *)usb_ctrl)->reg_31.b0 = 0;
        usb_ep0_data_len = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_ep0_data_in *)usb_ctrl)->ep0_control_in.b6 = 1;
        _builtin_set_imask(saved_imask);
    }
    usb_ep0_stage = 2;
    usb_ep0_event = 0;
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_ep0_data_in *)usb_ctrl)->ep0_control_in.b1 = 0;
    _builtin_set_imask(saved_imask);
}
