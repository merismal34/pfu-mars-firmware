#include "boot.h"
#pragma section 2
struct usb_regs_ack { char c[16]; };

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

void fw_update_usb_ack_and_reset(void)
{
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[3] = ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[3];
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[7] = ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[7];
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[9] = ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[9];
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[11] = ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[11];
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[5] = ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[5];
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[1] = ((struct usb_regs_ack *)fw_update_p_usb_ctrl)->c[1];
    _builtin_set_imask(saved_imask);
    fw_update_usb_reset_state();
}
