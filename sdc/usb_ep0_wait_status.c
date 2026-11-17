#include "fw.h"
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
extern void _builtin_set_gbr(void *);

void usb_ep0_wait_status(void)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    if (usb_ep0_event == USB_EVENT_OUT_ACK) {
        usb_ep0_event = 0;
        usb_bulk_wait_count = 0;
        usb_ep0_stage = 4;
    } else if (usb_ep0_event == USB_EVENT_OUT_ERROR) {
        usb_ep0_event = 0;
        usb_bulk_wait_count++;
        if (usb_bulk_wait_count > 255) {
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            usb_ctrl->ep0_control_in.b0 = 1;
            _builtin_set_gbr(0);
            usb_ctrl->ep0_control_out.b0 = 1;
            _builtin_set_imask(saved_imask);
            usb_ep0_stage = 7;
        }
    } else if (usb_ctrl_flags.b7) {
        usb_ep0_stage = 0;
    } else {
        _builtin_set_gbr(0);
        if (usb_ctrl->ep0_int_stat.b.b3 != 0) {
            if ((usb_ep0_int_status.v & 0x33) == 0) {
                if (usb_ctrl->main_int_stat.b.b0 == 0 && 1) {
                    usb_bulk_wait_count = 0;
                    usb_ep0_data_len = 0;
                    _builtin_set_imask(15);
                    _builtin_set_gbr(0);
                    usb_ctrl->reg_17.b0 = 1;
                    _builtin_set_imask(saved_imask);
                    usb_ep0_stage = 6;
                }
            }
        }
    }
}
