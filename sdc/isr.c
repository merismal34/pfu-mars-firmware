#include "fw.h"
struct vdcc_reg_05 {
    char pad_00[5];
    union { unsigned char v; struct vdcc_events_bits b; } events;
};

struct uart_regs_isr {
    char pad_00[5];
    union byte_bits iir_fcr;
    char pad_06[5];
    union byte_bits lsr;
};

union pisces_event_status_view {
    unsigned short w;
};


struct board_ctrl_regs_isr {
    char pad_00[2];
    struct {
        unsigned short p:4,
                       :2,
                       hostif_idle:1,
                       :9;
    } reg_02;
    struct {
        unsigned short p:4,
                       :2,
                       hostif_idle_enable:1,
                       :9;
    } reg_04;
    char pad_06[6];
    struct {
        unsigned short p:4,
                       b11:1,
                       cga_reset:1,
                       :1,
                       cga_select:1,
                       :8;
    } reg_0c;
};

struct usb_regs_isr {
    char pad_00;
    unsigned char main_int_stat;
    char reg_01_07[15];
    unsigned char main_int_enb;
    char reg_09_1e[45];
    unsigned char usb_bank;
    char reg_20_21[5];
    struct Bits reg_22;
    char reg_23_27[11];
    unsigned char address;
};
struct scsi_regs_isr {
    char pad_00;
    unsigned char mainint;
    char pad_02[17];
    struct Bits scsimode0;
};

struct vdcc_gpio_reg_05_volatile { char reg_100_104[5]; volatile struct Bits reg_105; };


#pragma interrupt(isr_irq0)
void isr_irq0(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    while (((struct vdcc_reg_05 *)p_vdcc_front[0])->events.b.line_event) {
        _builtin_set_imask(15);
        ((struct vdcc_reg_05 *)p_vdcc_front[0])->events.v = 1;
        _builtin_set_imask(saved_imask);
        line_handle_irq();
    }
}
#pragma interrupt(isr_irq1)
void isr_irq1(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    uart_irq_status[0].iir.v = ((struct uart_regs_isr *)p_uart[0])->iir_fcr.v;
    uart_irq_status[0].lsr.v = ((struct uart_regs_isr *)p_uart[0])->lsr.v;
    if (uart_irq_status[0].iir.b.b0 == 0) {
        uart_handle_irq();
    } else {
        _builtin_set_imask(15);
        if (p_intc->ISR.BIT.IRQ1F) {
            p_intc->ISR.WORD = 0xbf;
        }
        _builtin_set_imask(saved_imask);
    }
}
#pragma interrupt(isr_irq2)
void isr_irq2(void)
{
    int saved_imask;
    union pisces_event_status_view status;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (p_pisces_ctrl[0]->event_status.f.b4) {
        p_pisces_ctrl[0]->event_enable.f.b4 = 0;
        _builtin_set_imask(saved_imask);
        _builtin_set_imask(15);
        p_pisces_ctrl[0]->event_enable.f.b4 = 1;
        _builtin_set_imask(saved_imask);
    } else {
        if (p_intc->ISR.BIT.IRQ2F) {
            p_intc->ISR.WORD = 0xdf;
        }
        status.w = p_pisces_ctrl[0]->event_status.v;
        status.w &= 0xff;
        p_pisces_ctrl[0]->event_enable.v = ~status.w;
        p_pisces_ctrl[0]->event_enable.v |= status.w;
    }
    _builtin_set_imask(saved_imask);
}
#pragma interrupt(isr_irq3)
void isr_irq3(void)
{
    int saved_imask;
    unsigned char z;
    unsigned char y;
    unsigned char saved_main_enable;
    unsigned char w;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.owner_decided && hw_cga_board_present.f.present) {
        isr_reset_hostif();
        hw_hostif_flags.f.usb = 0;
        hw_hostif_flags.f.own_hostif = 0;
        hw_hostif_flags.f.selected = 0;
        _builtin_set_imask(saved_imask);
        return;
    }
    if (!hw_hostif_flags.f.owner_decided) {
        win_params.side[0].size_rounding.v = 0x6002;
        win_params.b13.f.b0 = 0;
        hw_hostif_flags.f.owner_decided = 1;
        hw_hostif_flags.f.cga_test_scan = 0;
        hw_cga_board_present.f.present = 0;
        p_intc->IPRA.WORD &= 0xf0ff;
        if (hw_board_variant == 0) {
            ((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_0c.b11 = 1;
        }
        ((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_0c.cga_reset = 1;
        ((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_0c.cga_select = 0;
    }
    if (((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_02.hostif_idle == 1) {
        ((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_02.hostif_idle = 0;
    }
    if (hw_hostif_flags.f.selected && hw_hostif_flags.f.usb) {
        isr_stop_output_dma();
        if (((struct usb_regs_isr *)usb_ctrl)->main_int_stat == 1) {
            return;
        }
        _builtin_set_gbr(0);
        if ((((struct usb_regs_isr *)usb_ctrl)->main_int_stat & 0xc4) == 0) {
            _builtin_set_gbr(0);
            /* 0xDB: the S1R72104 MAININT interrupt bits (EXEC and bit 2 left out) */
            if (((struct scsi_regs_isr *)scsi_ctrl)->mainint & 0xdb) {
                _builtin_set_imask(saved_imask);
                return;
            }
        }
        _builtin_set_gbr(0);
        saved_main_enable = ((struct usb_regs_isr *)usb_ctrl)->main_int_enb;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr *)usb_ctrl)->main_int_enb = 0;
        _builtin_set_gbr(0);
        z = ((struct usb_regs_isr *)usb_ctrl)->main_int_stat & 0xfe;
    } else if (hw_hostif_flags.f.selected && !hw_hostif_flags.f.usb) {
        if ((((struct scsi_regs_isr *)scsi_ctrl)->mainint & 0xdb) == 0) {
            _builtin_set_gbr(0);
            if (((struct usb_regs_isr *)usb_ctrl)->main_int_stat & 0xc4) {
                if (((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_04.hostif_idle_enable == 1) {
                    ((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_02.hostif_idle = 1;
                }
                _builtin_set_imask(saved_imask);
                return;
            }
        }
    } else if (!hw_hostif_flags.f.selected) {
        if (power_state.f.saving) {
            p_pisces_hostif[0]->ctrl.f.b0 = 1;
            p_pisces_hostif[0]->ctrl.f.b9 = 0;
            p_pisces_ctrl[0]->ctrl.f.hostif_stop = 1;
            while (1) {
                if (p_pisces_hostif[0]->ctrl.f.b1) break;
            }
        } else {
            p_pisces_hostif[1]->ctrl.f.b0 = 1;
            p_pisces_hostif[1]->ctrl.f.b9 = 0;
            p_pisces_hostif[0]->ctrl.f.b0 = 1;
            p_pisces_hostif[0]->ctrl.f.b9 = 0;
            p_pisces_ctrl[0]->ctrl.f.hostif_stop = 1;
            while (1) {
                if (p_pisces_hostif[0]->ctrl.f.b1 && p_pisces_hostif[1]->ctrl.f.b1) break;
            }
        }
        _builtin_set_gbr(0);
        saved_main_enable = ((struct usb_regs_isr *)usb_ctrl)->main_int_enb;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr *)usb_ctrl)->main_int_enb = 0;
        _builtin_set_gbr(0);
        z = ((struct usb_regs_isr *)usb_ctrl)->main_int_stat & 0xfe;
    }
    if (p_intc->ISR.BIT.IRQ3F) {
        p_intc->ISR.WORD = 0xef;
    }
    _builtin_set_imask(saved_imask);
    if (!hw_hostif_flags.f.selected) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        if ((((struct scsi_regs_isr *)scsi_ctrl)->mainint & 0xdb) && !(settings_byte_0x7f[0] & 0x80)) {
            _builtin_set_gbr(0);
            ((struct usb_regs_isr *)usb_ctrl)->usb_bank = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_isr *)usb_ctrl)->reg_22.b7 = 0;
            hw_hostif_flags.f.selected = 1;
            hw_hostif_flags.f.usb = 0;
            hw_hostif_flags.f.own_hostif = 1;
            ((struct vdcc_gpio_reg_05_volatile *)p_vdcc_front_gpio[0])->reg_105.b7 = 0;
            p_pisces_hostif[0]->ctrl.f.a = 2;
            p_pisces_hostif[0]->ctrl.f.b = 3;
            if (!power_state.f.saving) {
                p_pisces_hostif[1]->ctrl.f.a = 2;
                p_pisces_hostif[1]->ctrl.f.b = 3;
            }
        }
        _builtin_set_imask(saved_imask);
        _builtin_set_gbr(0);
        if (((struct usb_regs_isr *)usb_ctrl)->main_int_stat & 0xc4) {
            hw_switch_to_usb();
        }
        if (!hw_hostif_flags.f.selected) {
            _builtin_set_imask(15);
            if (((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_04.hostif_idle_enable == 1 && !hw_hostif_flags.f.usb) {
                ((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_02.hostif_idle = 1;
            }
            _builtin_set_imask(saved_imask);
            return;
        }
    }
    if (hw_hostif_flags.f.usb) {
        _builtin_set_imask(15);
        if (usb_ctrl_flags.b5 == 1) {
            usb_ctrl_flags.b5 = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_isr *)usb_ctrl)->usb_bank = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_isr *)usb_ctrl)->address = usb_address & 0x7f;
            if (usb_address == 0) {
                usb_device_state = USB_STATE_DEFAULT;
            } else {
                usb_device_state = USB_STATE_ADDRESS;
            }
        }
        _builtin_set_imask(saved_imask);
        _builtin_set_gbr(0);
        if ((((struct usb_regs_isr *)usb_ctrl)->main_int_stat & 0xc4) == 0) {
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_isr *)usb_ctrl)->main_int_enb = saved_main_enable;
            _builtin_set_imask(saved_imask);
            return;
        } else {
            usb_read_irq_status();
            usb_last_main_int = usb_main_int_read.v;
            usb_last_sie_int = usb_sie_int_read.v;
            usb_last_ep_int = usb_ep_int_read.v;
            usb_last_ep0_int = usb_ep0_int_read.v;
            usb_last_epa_int = usb_epa_int_read.v;
            usb_last_epb_int = usb_epb_int_read.v;
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_isr *)usb_ctrl)->main_int_enb = saved_main_enable;
            _builtin_set_imask(saved_imask);
        }
        _builtin_set_imask(15);
        if (p_intc->ISR.BIT.IRQ3F) {
            _builtin_set_gbr(0);
            if ((((struct usb_regs_isr *)usb_ctrl)->main_int_stat & 0xc4) == 0) {
                p_intc->ISR.WORD = 0xef;
            }
        }
        _builtin_set_imask(saved_imask);
        return;
    } else {
        _builtin_set_imask(15);
        if (((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_04.hostif_idle_enable == 1) {
            ((volatile struct board_ctrl_regs_isr *)p_board_ctrl)->reg_02.hostif_idle = 1;
        }
        _builtin_set_gbr(0);
        if ((((struct scsi_regs_isr *)scsi_ctrl)->mainint & 0xdb) == 0) {
            _builtin_set_imask(saved_imask);
            return;
        }
        _builtin_set_imask(saved_imask);
        scsi_read_interrupts();
        isr_trigger_mtu0(1);
        _builtin_set_imask(15);
        if (p_intc->ISR.BIT.IRQ3F) {
            _builtin_set_gbr(0);
            if ((((struct scsi_regs_isr *)scsi_ctrl)->mainint & 0xdb) == 0) {
                p_intc->ISR.WORD = 0xef;
            }
        }
        _builtin_set_imask(saved_imask);
    }
}
#pragma interrupt(isr_tgia_0)
void isr_tgia_0(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (!p_mtu0->TSR.BIT.TGFA) {
        return;
    }
    _builtin_set_imask(15);
    scsi_wait_selection_pending = 0;
    p_mtu0->TSR.BIT.TGFA = 0;
    p_mtu0->TIER.BIT.TGIEA = 0;
    if (!p_mtu0->TIER.BIT.TGIEB) {
        ((volatile struct Bits *)p_mtu)->b0 = 0;
    }
    p_mtu0->TGRA = 0xffff;
    _builtin_set_imask(saved_imask);
    if (hw_hostif_flags.f.own_hostif) {
        scsi_run_layer();
    } else {
        uart_process();
    }
    _builtin_set_imask(15);
    if (scsi_wait_selection_pending) {
        scsi_ctrl_command(12);
    }
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct scsi_regs_isr *)scsi_ctrl)->scsimode0.b7 = 1;
    p_intc->IPRA.WORD |= 3;
    _builtin_set_imask(saved_imask);
}
#pragma interrupt(isr_tgib_0)
void isr_tgib_0(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (!p_mtu0->TSR.BIT.TGFB) {
        return;
    }
    _builtin_set_imask(15);
    scsi_wait_selection_pending = 0;
    p_mtu0->TSR.BIT.TGFB = 0;
    p_mtu0->TIER.BIT.TGIEB = 0;
    if (!p_mtu0->TIER.BIT.TGIEA) {
        ((volatile struct Bits *)p_mtu)->b0 = 0;
    }
    p_mtu0->TGRB = 0xffff;
    _builtin_set_imask(saved_imask);
    scsi_run_layer();
    _builtin_set_imask(15);
    if (scsi_wait_selection_pending) {
        scsi_ctrl_command(12);
    }
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct scsi_regs_isr *)scsi_ctrl)->scsimode0.b7 = 1;
    p_intc->IPRA.WORD |= 3;
    _builtin_set_imask(saved_imask);
}
void isr_request_command_layer(unsigned char exclude)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (!hw_hostif_flags.f.own_hostif || exclude != 1) {
        uart_pending_events.b0 = 1;
    }
    if (hw_hostif_flags.f.usb && exclude != 2) {
        _builtin_set_imask(saved_imask);
        scsi_resume();
    } else {
        _builtin_set_imask(15);
        isr_trigger_mtu0(exclude);
    }
    _builtin_set_imask(saved_imask);
}
void isr_trigger_mtu0(unsigned char exclude)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (hw_hostif_flags.f.own_hostif && exclude != 2) {
        _builtin_set_imask(15);
        ((volatile struct Bits *)p_mtu)->b0 = 0;
        if (p_mtu0->TIER.BIT.TGIEA || p_mtu0->TIER.BIT.TGIEB) {
            p_mtu0->TGRB = p_mtu0->TCNT + 2;
        } else {
            p_mtu0->TCNT = 0;
            p_mtu0->TGRB = 2;
        }
        ((volatile struct Bits *)p_mtu)->b0 = 1;
        p_mtu0->TIER.BIT.TGIEB = 1;
        _builtin_set_imask(saved_imask);
        return;
    }
    _builtin_set_imask(15);
    ((volatile struct Bits *)p_mtu)->b0 = 0;
    if (p_mtu0->TIER.BIT.TGIEA) {
        p_mtu0->TGRA = p_mtu0->TCNT + 2;
    } else {
        p_mtu0->TCNT = 0;
        p_mtu0->TGRA = 2;
    }
    ((volatile struct Bits *)p_mtu)->b0 = 1;
    p_mtu0->TIER.BIT.TGIEA = 1;
    _builtin_set_imask(saved_imask);
}
#pragma interrupt(isr_tgia_1)
void isr_tgia_1(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (!p_mtu1->TSR.BIT.TGFA) {
        return;
    }
    _builtin_set_imask(15);
    p_mtu1->TSR.BIT.TGFA = 0;
    _builtin_set_imask(saved_imask);
    tick_handler();
}
#pragma interrupt(isr_tgia_2)
void isr_tgia_2(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (!p_mtu2->TSR.BIT.TGFA) {
        return;
    }
    _builtin_set_imask(15);
    p_mtu2->TSR.BIT.TGFA = 0;
    _builtin_set_imask(saved_imask);
    mdc_poll();
    page_poll_progress();
}
#pragma interrupt(isr_tgia_3)
void isr_tgia_3(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (!p_mtu3->TSR.BIT.TGFA) {
        return;
    }
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.selected && !hw_hostif_flags.f.usb) {
        ((volatile struct Bits *)p_mtu)->b6 = 0;
        p_mtu3->TIER.BIT.TGIEA = 0;
    }
    p_mtu3->TSR.BIT.TGFA = 0;
    _builtin_set_imask(saved_imask);
    usb_handle_irq();
}
#pragma interrupt(isr_tgia_4)
void isr_tgia_4(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (!p_mtu4->TSR.BIT.TGFA) {
        return;
    }
    _builtin_set_imask(15);
    p_mtu4->TSR.BIT.TGFA = 0;
    _builtin_set_imask(saved_imask);
    page_tick();
}
void isr_stop_cmt0(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (p_cmt0->CMCSR.BIT.CMF) {
        p_cmt0->CMCSR.BIT.CMF = 0;
    }
    p_cmt->CMSTR.BIT.STR0 = 0;
    p_cmt0->CMCSR.BIT.CMIE = 0;
    p_cmt0->CMCNT = 0;
    _builtin_set_imask(saved_imask);
}
#pragma interrupt(isr_cmi1)
void isr_cmi1(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (p_cmt1->CMCSR.BIT.CMF) {
        p_cmt1->CMCSR.BIT.CMF = 0;
    }
    _builtin_set_imask(saved_imask);
    if (hw_hostif_flags.f.own_hostif) {
        isr_stop_cmt1();
    } else {
        _builtin_set_imask(15);
        uart_timeout_flags.b7 = 1;
        _builtin_set_imask(saved_imask);
        if (uart_timeout_mode.g.frame_timeout) {
            isr_cmt1_timeout();
        } else if (uart_timeout_mode.g.ack_timeout) {
            isr_start_uart_timeout();
        }
        _builtin_set_imask(15);
        uart_timeout_flags.b7 = 0;
        uart_timeout_flags.b6 = 0;
        _builtin_set_imask(saved_imask);
    }
}
void isr_cmt1_timeout(void)
{
    int saved_imask;

    isr_stop_cmt1();
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    uart_pending_events.b2 = 1;
    isr_trigger_mtu0(2);
    uart_timeout_mode.v = 0;
}
void isr_start_uart_timeout(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (uart_timeout_retries == 0) {
        uart_set_rts();
    }
    if (++uart_timeout_retries <= 50) {
        _builtin_set_imask(15);
        if (!uart_timeout_flags.b6) {
            p_cmt1->CMCOR = 0xb71b;
            p_cmt->CMSTR.BIT.STR1 = 1;
            p_cmt1->CMCSR.BIT.CMIE = 1;
            uart_timeout_mode.g.ack_timeout = 1;
        } else {
        }
        _builtin_set_imask(saved_imask);
    } else {
        isr_stop_cmt1();
        uart_timeout_retries = 0;
        if (++uart_rx_retries < 3) {
            link_reply.f.b7 = 1;
            isr_request_command_layer(2);
        } else {
            uart_rx_retries = 0;
            link_reply.w = 0;
            uart_timeout_mode.g.ack_timeout = 0;
            uart_rx_status.w = 0;
            uart_frame_len.v = 0;
            uart_baud_index_new = uart_baud_index.v;
        }
    }
}
void isr_stop_cmt1(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (p_cmt1->CMCSR.BIT.CMF) {
        p_cmt1->CMCSR.BIT.CMF = 0;
    }
    p_cmt->CMSTR.BIT.STR1 = 0;
    p_cmt1->CMCSR.BIT.CMIE = 0;
    p_cmt1->CMCNT = 0;
    _builtin_set_imask(saved_imask);
}
#pragma interrupt(isr_eri_1)
void isr_eri_1(void)
{
    console_handle_rx();
}
#pragma interrupt(isr_rxi_1)
void isr_rxi_1(void)
{
    console_handle_rx();
}
#pragma interrupt(isr_tei_1)
void isr_tei_1(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    p_sci1->SCR.BIT.TEIE = 0;
    _builtin_set_imask(saved_imask);
    if (p_sci1->SSR.BIT.TEND == 0) {
        return;
    }
    console_send_next();
}
#pragma interrupt(isr_txi_1)
void isr_txi_1(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    p_sci1->SCR.BIT.TIE = 0;
    _builtin_set_imask(saved_imask);
    console_send_next();
}
#pragma interrupt(isr_general_illegal)
void isr_general_illegal(void)
{
    _builtin_set_imask(15);
    rt_restart();
}
