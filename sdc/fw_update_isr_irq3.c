#include "boot.h"
#pragma section 2
struct usb_regs_isr_irq3 {
    char pad_00;
    unsigned char main_int_stat;
    char reg_01_07[15];
    unsigned char main_int_enb;
    char pad_12;
    unsigned char sie_int_enb;
    char pad_14;
    unsigned char reg_0a;
    char reg_0b_12[17];
    struct usb_control_bits usb_control;
    char reg_14[3];
    unsigned char xcvr_control;
    char reg_16_1e[19];
    unsigned char usb_bank;
    char reg_20_27[17];
    unsigned char address;
};
struct scsi_regs_isr_irq3 {
    char pad_00;
    unsigned char mainint;
    char pad_02[17];
    struct Bits scsimode0;
};
struct intc_view_irq3 {
    char pad_00[18];
    union {
        unsigned short w;
        struct { unsigned char lo; struct Bits b; } s;
    } irq_status;
};

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

#pragma interrupt(fw_update_isr_irq3)
void fw_update_isr_irq3(void)
{
    unsigned char saved_int_enable;
    unsigned char u2;
    unsigned char saved_imask;
    unsigned char saved_main_enable;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    saved_main_enable = ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->main_int_enb;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->main_int_enb = 0;
    _builtin_set_imask(saved_imask);
    fw_update_sync_dst = fw_update_sync_src;
    saved_int_enable = ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->main_int_stat & 0xfe;
    _builtin_set_imask(15);
    if (((struct intc_view_irq3 *)fw_update_p_intc)->irq_status.s.b.b4)
        ((struct intc_view_irq3 *)fw_update_p_intc)->irq_status.w = 0xef;
    _builtin_set_imask(saved_imask);
    if (fw_update_hostif_flags.usb == 1 && fw_update_usb_ctrl_flags.b5 == 1) {
        fw_update_usb_ctrl_flags.b5 = 0;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->usb_bank = 0;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->address = fw_update_usb_address & 127;
        if (fw_update_usb_address == 0)
            fw_update_usb_device_state = USB_STATE_DEFAULT;
        else
            fw_update_usb_device_state = USB_STATE_ADDRESS;
    }
    fw_update_sync_dst = fw_update_sync_src;
    if (!(((struct scsi_regs_isr_irq3 *)fw_update_p_scsi_ctrl)->mainint & 0xd9) && !(((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->main_int_stat & 0xc4)) {
        _builtin_set_imask(15);
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->main_int_enb = saved_main_enable;
        _builtin_set_imask(saved_imask);
        return;
    }
    fw_update_sync_dst = fw_update_sync_src;
    if (((struct scsi_regs_isr_irq3 *)fw_update_p_scsi_ctrl)->mainint & 0xd9) {
        if (fw_update_hostif_flags.scsi == 1) {
            fw_update_dispatch_scsi_event();
        } else if (fw_update_hostif_flags.usb == 1) {
            fw_update_sync_dst = fw_update_sync_src;
            if (((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->main_int_stat & 0xc4)
                fw_update_usb_read_irq_status();
            _builtin_set_imask(15);
            fw_update_sync_dst = fw_update_sync_src;
            ((struct scsi_regs_isr_irq3 *)fw_update_p_scsi_ctrl)->mainint = u2;
            _builtin_set_imask(saved_imask);
        } else {
            fw_update_hostif_flags.scsi = 1;
            fw_update_hostif_flags.usb = 0;
            _builtin_set_imask(15);
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->main_int_enb = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->sie_int_enb = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->reg_0a = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct scsi_regs_isr_irq3 *)fw_update_p_scsi_ctrl)->scsimode0.b7 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->usb_control.active_usb = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->xcvr_control = 65;
            _builtin_set_imask(saved_imask);
            fw_update_dispatch_scsi_event();
        }
    }
    fw_update_sync_dst = fw_update_sync_src;
    if (((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->main_int_stat & 0xc4) {
        if (fw_update_hostif_flags.usb == 1) {
            fw_update_usb_read_irq_status();
        } else if (fw_update_hostif_flags.scsi == 1) {
            _builtin_set_imask(15);
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->main_int_stat = saved_int_enable;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct scsi_regs_isr_irq3 *)fw_update_p_scsi_ctrl)->mainint = u2;
            _builtin_set_imask(saved_imask);
        } else {
            fw_update_usb_read_irq_status();
        }
    }
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_isr_irq3 *)fw_update_p_usb_ctrl)->main_int_enb = saved_main_enable;
    _builtin_set_imask(saved_imask);
}
