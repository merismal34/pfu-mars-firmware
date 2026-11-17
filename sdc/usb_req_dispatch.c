#include "fw.h"
extern void _builtin_set_gbr(void *);
struct usb_regs_req_dispatch { char pad_00[67]; unsigned char ep0_setup_1; };

int usb_dispatch_std_request(void)
{
    int stall;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_dispatch *)usb_ctrl)->ep0_setup_1 < 13) {
        _builtin_set_gbr(0);
        stall = usb_std_request_table[((struct usb_regs_req_dispatch *)usb_ctrl)->ep0_setup_1]();
    } else {
        stall = usb_req_unsupported();
    }
    return stall;
}
