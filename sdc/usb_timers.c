#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
extern void _builtin_set_gbr(void *);
struct usb_regs_timers {
    char reg_00_07[17];
    unsigned char main_int_enb;
    char pad_12[1];
    unsigned char sie_int_enb;
    char pad_14[1];
    unsigned char reg_0a;
    char reg_0b_11[15];
    union byte_bits_c pm_control;
    char pad_26[1];
    struct usb_control_bits usb_control;
    char reg_14[3];
    unsigned char xcvr_control;
    char pad_2c[1];
    unsigned char usb_test;
};

void usb_hook(void)
{
}

void usb_stop_timers(void)
{
    usb_suspend_ticks++;
    if (usb_suspend_ticks < 20000) {
        return;
    }
    usb_suspended = 0;
    if (usb_device_state == USB_STATE_DEFAULT) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_timers *)usb_ctrl)->pm_control.b.b0 = 0;
        for (;;) {
            _builtin_set_gbr(0);
            if (((struct usb_regs_timers *)usb_ctrl)->pm_control.b.b7 == 0) {
                break;
            }
        }
        _builtin_set_gbr(0);
        ((struct usb_regs_timers *)usb_ctrl)->xcvr_control = 0x41;
        _builtin_set_gbr(0);
        ((struct usb_regs_timers *)usb_ctrl)->usb_control.active_usb = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_timers *)usb_ctrl)->usb_control.restore_usb = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_timers *)usb_ctrl)->usb_control.go_chirp = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_timers *)usb_ctrl)->usb_test = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_timers *)usb_ctrl)->main_int_enb = 0xc4;
        _builtin_set_gbr(0);
        ((struct usb_regs_timers *)usb_ctrl)->sie_int_enb = 0x86;
        _builtin_set_gbr(0);
        ((struct usb_regs_timers *)usb_ctrl)->reg_0a = 7;
        *(*(unsigned char **)&p_mtu) = 0;
        p_cmt->CMSTR.WORD = 0;
        if (p_cmt0->CMCSR.BIT.CMF) {
            p_cmt0->CMCSR.BIT.CMF = 0;
        }
        if (p_cmt1->CMCSR.BIT.CMF) {
            p_cmt1->CMCSR.BIT.CMF = 0;
        }
        if (p_mtu1->TSR.BIT.TGFA) {
            p_mtu1->TSR.BIT.TGFA = 0;
        }
        if (p_mtu2->TSR.BIT.TGFA) {
            p_mtu2->TSR.BIT.TGFA = 0;
        }
        if (p_mtu3->TSR.BIT.TGFA) {
            p_mtu3->TSR.BIT.TGFA = 0;
        }
        if (p_mtu4->TSR.BIT.TGFA) {
            p_mtu4->TSR.BIT.TGFA = 0;
        }
        hw_restart_flags.f.usb_restart = 1;
        hw_hostif_flags.v = 0;
        hw_restart_flags.f.leaving_power_save = 0;
        usb_restart_pending = 1;
        rt_restart();
    }
}
