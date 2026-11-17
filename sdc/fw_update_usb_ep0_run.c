#include "boot.h"
#pragma section 2
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

void fw_update_usb_ep0_run(void)
{
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_tick_count = 0;
    _builtin_set_imask(saved_imask);
    for (;;) {
        fw_update_usb_ep0_stage_table[fw_update_usb_ep0_stage]();
        if (fw_update_usb_ep0_stage >= 7)
            break;
        fw_update_usb_poll_events();
        fw_update_usb_run_events_6_4();
        if (fw_update_usb_event_flags.b5) {
            _builtin_set_imask(15);
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_usb_ctrl->reg_17.b0 = 1;
            _builtin_set_imask(saved_imask);
            fw_update_usb_ep0_event = 0;
            fw_update_usb_ep0_stage = 7;
            break;
        }
        if (fw_update_usb_event_flags.b6) {
            fw_update_usb_ep0_stage = 7;
            if (fw_update_command_flags.b4 || fw_update_command_flags.b5)
                fw_update_command_flags.b0 = 1;
            break;
        }
        if (fw_update_tick_count > 0x9c4) {
            fw_update_usb_ep0_event = 0;
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_usb_ctrl->ep0_control_in.b0 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_p_usb_ctrl->ep0_control_out.b0 = 1;
            fw_update_usb_ep0_stage = 7;
            break;
        }
    }
}

/* the stage handlers by fw_update_usb_ep0_stage */
void (*const fw_update_usb_ep0_stage_table[7])(void) = {
    fw_update_usb_ep0_setup, fw_update_usb_ep0_data_in, fw_update_usb_ep0_wait_in, fw_update_usb_ep0_wait_status,
    fw_update_usb_ep0_data_out, fw_update_usb_ep0_stall, fw_update_usb_ep0_status
};
