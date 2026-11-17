#include "fw.h"
#pragma section H
extern void _builtin_set_gbr(void *);
struct usb_regs_init {
    char pad_00[41];
    struct Bits usb_status;
    char pad_2a[39];
    unsigned char address;
};


void usb_init(void)
{
    const struct usb_interface_descriptor *interface;

    p_pisces_ctrl[0]->ctrl.f.hostif_stop = 1;
    usb_main_status.v = 0;
    usb_sie_status.v = 0;
    usb_ep_int_status.v = 0;
    usb_ep0_int_status.v = 0;
    usb_epa_int_status.v = 0;
    usb_epb_int_status.v = 0;
    usb_unused_state_2 = 0;
    ((unsigned char *)&usb_wake_events)[0] = 0;
    usb_ep0_stage = 7;
    usb_bulk_state = 2;
    usb_bulk_out_state = 2;
    usb_transfer_phase = 0;
    usb_device_state = USB_STATE_DEFAULT;
    _builtin_set_gbr(0);
    usb_address = ((struct usb_regs_init *)usb_ctrl)->address & 0x7f;
    *(unsigned char *)&usb_ctrl_flags = 0;
    ((unsigned char *)&usb_event_flags)[0] = 0;
    usb_bulk_wait_count = 0;
    usb_alternate_setting = 0;
    usb_ep0_data_ptr = 0;
    usb_ep0_wlength = usb_ep0_done_len = usb_ep0_data_len = 0;
    _builtin_set_gbr(0);
    if (((struct usb_regs_init *)usb_ctrl)->usb_status.b6) {
        interface = &usb_interface_fs[0];
    } else {
        interface = &usb_interface_hs[0];
    }
    usb_epa_max_packet.v = ((&interface->endpoints->wMaxPacketSize_lo)[1] << 8) + interface->endpoints->wMaxPacketSize_lo;
    usb_epb_max_packet = ((&interface->endpoints[1].wMaxPacketSize_lo)[1] << 8) + interface->endpoints[1].wMaxPacketSize_lo;
    usb_epa_address[0] = interface->endpoints->bEndpointAddress;
    usb_epb_address[0] = interface->endpoints[1].bEndpointAddress;
    usb_ep0_event = usb_epa_event = 0;
    usb_epb_event = 0;
    usb_transfer_flags.done = 0;
    usb_transfer_flags.pending = 0;
    usb_transfer_flags.pisces_in = 0;
    usb_bulk_len = 0;
}
