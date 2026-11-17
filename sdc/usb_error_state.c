#include "fw.h"
#pragma section H

void usb_save_error_state(void)
{
    p_pisces_hostif[1]->ctrl.f.b0 = 1;
    p_pisces_hostif[1]->ctrl.f.b9 = 0;
    p_pisces_hostif[0]->ctrl.f.b0 = 1;
    p_pisces_hostif[0]->ctrl.f.b9 = 0;
    p_pisces_ctrl[0]->ctrl.f.hostif_stop = 1;
    util_mem_copy(usb_error_registers, usb_ctrl, 0x82);
    usb_error_ep0_stage = usb_ep0_stage;
    usb_error_bulk_state = usb_bulk_state;
    usb_error_bulk_out_state = usb_bulk_out_state;
    usb_error_unused_state_1 = usb_unused_state_1;
    usb_error_transfer_phase = usb_transfer_phase;
    usb_error_unused_state_2 = usb_unused_state_2;
    usb_error_ep0_event = usb_ep0_event;
    usb_error_epa_event = usb_epa_event;
    usb_error_epb_event = usb_epb_event;
    usb_error_ep0_max_packet = (unsigned char)usb_ep0_max_packet;
    usb_error_epa_max_packet = usb_epa_max_packet.v;
    usb_error_epb_max_packet = usb_epb_max_packet;
}
