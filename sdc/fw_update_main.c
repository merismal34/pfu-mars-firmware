#include "boot.h"
#pragma section 2
/* program 2's view of the SCSI controller (S1R72104) and the USB controller (S1R72013), register r at offset 2r+1 */
struct scsi_regs_main {
    char pad_00;
    unsigned char mainint;
    char pad_02;
    unsigned char scsiint1;
    char pad_04;
    unsigned char scsiint2;
    char reg_03_06[9];
    unsigned char reset;
    char reg_08[3];
    unsigned char scsimode0;
    char pad_14;
    unsigned char scsimode1;
    char reg_0b_0c[5];
    unsigned char syncmode;
    char pad_1c;
    unsigned char ownid;
    char pad_1e;
    unsigned char sdid;
    char pad_20;
    unsigned char seltime;
    char pad_22;
    unsigned char fifoctl;
    char reg_12_13[5];
    unsigned char command;
    char reg_15_16[5];
    unsigned char dmactl;
    char reg_18[3];
    unsigned char dtbc2;
    char pad_34;
    unsigned char dtbc1;
    char pad_36;
    unsigned char dtbc0;
    char pad_38;
    unsigned char config0;
    char pad_3a;
    unsigned char config1;
    char reg_1e_3f[68];
};
struct usb_regs_main {
    char pad_00;
    unsigned char main_int_stat;
    char pad_02;
    unsigned char sie_int_stat;
    char pad_04;
    unsigned char epr_int_stat;
    char pad_06;
    unsigned char ep0_int_stat;
    char pad_08;
    unsigned char epa_int_stat;
    char pad_0a;
    unsigned char epb_int_stat;
    char pad_0c;
    unsigned char reg_06;
    char reg_07[3];
    unsigned char main_int_enb;
    char pad_12;
    unsigned char sie_int_enb;
    char pad_14;
    unsigned char epr_int_enb;
    char reg_0b[3];
    unsigned char epa_int_enb;
    char pad_1a;
    unsigned char epb_int_enb;
    char reg_0e_0f[5];
    unsigned char chip_reset;
    char reg_11[3];
    unsigned char pm_control;
    char pad_26;
    unsigned char usb_control;
    char reg_14[3];
    unsigned char xcvr_control;
    char pad_2c;
    unsigned char usb_test;
    char pad_2e;
    unsigned char reg_17;
    char reg_18_1e[15];
    unsigned char usb_bank;
    char pad_40;
    unsigned char reg_20;
    char reg_21[3];
    unsigned char reg_22;
    char pad_46;
    unsigned char reg_23;
    char pad_48;
    unsigned char reg_24;
    char pad_4a;
    unsigned char reg_25;
    char reg_26_2c[15];
    unsigned char epb_control;
    char reg_2e_2f[5];
    unsigned char reg_30;
    char pad_62;
    unsigned char reg_31;
    char pad_64;
    unsigned char reg_32;
    char pad_66;
    unsigned char reg_33;
    char pad_68;
    unsigned char reg_34;
    char pad_6a;
    unsigned char reg_35;
    char pad_6c;
    unsigned char reg_36;
    char pad_6e;
    unsigned char reg_37;
    char reg_38_3f[16];
};
struct usb_regs_int_stat {
    char pad_00;
    struct Bits main_int_stat;
    char pad_02;
    struct Bits sie_int_stat;
};

#define E(m) ((struct scsi_regs_main *)fw_update_p_scsi_ctrl)->m
#define F(m) ((struct usb_regs_main *)fw_update_p_usb_ctrl)->m
#define EB(m) ((struct Bits *)&((struct scsi_regs_main *)fw_update_p_scsi_ctrl)->m)
#define FB(m) ((struct Bits *)&((struct usb_regs_main *)fw_update_p_usb_ctrl)->m)
/* program 2's controller registers by name: the SCSI controller (S1R72104, scsi_regs_main) */
#define FW_UPDATE_SCSI_MAININT        E(mainint)
#define FW_UPDATE_SCSI_SCSIINT1       E(scsiint1)
#define FW_UPDATE_SCSI_SCSIINT2       E(scsiint2)
#define FW_UPDATE_SCSI_RESET          E(reset)
#define FW_UPDATE_SCSI_SCSIMODE0      E(scsimode0)
#define FW_UPDATE_SCSI_SCSIMODE1      E(scsimode1)
#define FW_UPDATE_SCSI_SYNCMODE       E(syncmode)
#define FW_UPDATE_SCSI_OWNID          E(ownid)
#define FW_UPDATE_SCSI_SDID           E(sdid)
#define FW_UPDATE_SCSI_SELTIME        E(seltime)
#define FW_UPDATE_SCSI_COMMAND        E(command)
#define FW_UPDATE_SCSI_DTBC2          E(dtbc2)
#define FW_UPDATE_SCSI_DTBC1          E(dtbc1)
#define FW_UPDATE_SCSI_DTBC0          E(dtbc0)
#define FW_UPDATE_SCSI_CONFIG0        E(config0)
#define FW_UPDATE_SCSI_CONFIG1        E(config1)
#define FW_UPDATE_SCSI_SCSIMODE0_BITS EB(scsimode0)
#define FW_UPDATE_SCSI_SCSIMODE1_BITS EB(scsimode1)
#define FW_UPDATE_SCSI_FIFOCTL_BITS   EB(fifoctl)
#define FW_UPDATE_SCSI_DMACTL_BITS    EB(dmactl)
#define FW_UPDATE_SCSI_CONFIG0_BITS   EB(config0)
/* the USB controller (S1R72013, usb_regs_main); B1_/B2_ = bank 1 / 2 selected by FW_UPDATE_USB_USB_BANK */
#define FW_UPDATE_USB_MAIN_INT_STAT   F(main_int_stat)
#define FW_UPDATE_USB_SIE_INT_STAT    F(sie_int_stat)
#define FW_UPDATE_USB_EPR_INT_STAT    F(epr_int_stat)
#define FW_UPDATE_USB_EP0_INT_STAT    F(ep0_int_stat)
#define FW_UPDATE_USB_EPA_INT_STAT    F(epa_int_stat)
#define FW_UPDATE_USB_EPB_INT_STAT    F(epb_int_stat)
#define FW_UPDATE_USB_REG_06          F(reg_06)
#define FW_UPDATE_USB_MAIN_INT_ENB    F(main_int_enb)
#define FW_UPDATE_USB_SIE_INT_ENB     F(sie_int_enb)
#define FW_UPDATE_USB_EPR_INT_ENB     F(epr_int_enb)
#define FW_UPDATE_USB_EPA_INT_ENB     F(epa_int_enb)
#define FW_UPDATE_USB_USB_CONTROL     F(usb_control)
#define FW_UPDATE_USB_XCVR_CONTROL    F(xcvr_control)
#define FW_UPDATE_USB_USB_TEST        F(usb_test)
#define FW_UPDATE_USB_USB_BANK        F(usb_bank)
#define FW_UPDATE_USB_B1_REG_20       F(reg_20)
#define FW_UPDATE_USB_B1_REG_22       F(reg_22)
#define FW_UPDATE_USB_B1_REG_23       F(reg_23)
#define FW_UPDATE_USB_B1_REG_24       F(reg_24)
#define FW_UPDATE_USB_REG_25          F(reg_25)
#define FW_UPDATE_USB_B2_REG_30       F(reg_30)
#define FW_UPDATE_USB_B2_REG_31       F(reg_31)
#define FW_UPDATE_USB_B2_REG_32       F(reg_32)
#define FW_UPDATE_USB_B2_REG_33       F(reg_33)
#define FW_UPDATE_USB_B2_REG_34       F(reg_34)
#define FW_UPDATE_USB_B2_REG_35       F(reg_35)
#define FW_UPDATE_USB_B2_REG_36       F(reg_36)
#define FW_UPDATE_USB_B2_REG_37       F(reg_37)
#define FW_UPDATE_USB_CHIP_RESET_BITS FB(chip_reset)
#define FW_UPDATE_USB_PM_CONTROL_BITS FB(pm_control)
#define FW_UPDATE_USB_REG_17_BITS     FB(reg_17)
#define FW_UPDATE_USB_B1_REG_22_BITS  FB(reg_22)

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

#define SYNC() (fw_update_sync_dst = fw_update_sync_src)

void fw_update_usb_read_irq_status(void)
{
    unsigned char saved_imask;
    struct fw_update_command_link *record;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    SYNC();
    fw_update_usb_main_int_read.v = FW_UPDATE_USB_MAIN_INT_STAT;
    if (fw_update_usb_main_int_read.f.b7) {
        SYNC();
        fw_update_usb_sie_int_read.v = FW_UPDATE_USB_SIE_INT_STAT;
        SYNC();
        FW_UPDATE_USB_SIE_INT_STAT = fw_update_usb_sie_int_read.v;
    }
    if (fw_update_usb_main_int_read.f.b6) {
        SYNC();
        fw_update_usb_ep_int_read.v = FW_UPDATE_USB_EPR_INT_STAT;
        SYNC();
        FW_UPDATE_USB_EPR_INT_STAT = fw_update_usb_ep_int_read.v;
        if (fw_update_usb_ep_int_read.f.b0) {
            SYNC();
            fw_update_usb_ep0_int_read.v = FW_UPDATE_USB_EP0_INT_STAT;
            SYNC();
            FW_UPDATE_USB_EP0_INT_STAT = fw_update_usb_ep0_int_read.v;
        }
        if (fw_update_usb_ep_int_read.f.b1) {
            SYNC();
            fw_update_usb_epa_int_read.v = FW_UPDATE_USB_EPA_INT_STAT;
            SYNC();
            FW_UPDATE_USB_EPA_INT_STAT = fw_update_usb_epa_int_read.v;
        }
        if (fw_update_usb_ep_int_read.f.b2) {
            SYNC();
            fw_update_usb_epb_int_read.v = FW_UPDATE_USB_EPB_INT_STAT;
            SYNC();
            FW_UPDATE_USB_EPB_INT_STAT = fw_update_usb_epb_int_read.v;
        }
    }
    SYNC();
    FW_UPDATE_USB_MAIN_INT_STAT = fw_update_usb_main_int_read.v & 0xfe;
    _builtin_set_imask(saved_imask);
    fw_update_usb_main_status.v |= fw_update_usb_main_int_read.v;
    if (fw_update_usb_main_int_read.f.b7) {
        if (fw_update_usb_sie_int_read.f.b6) {
            SYNC();
            FB(sie_int_enb)->b6 = 0;
        }
        if (fw_update_usb_sie_int_read.f.b5) {
            SYNC();
            FB(usb_control)->b7 = 1;
        }
        fw_update_usb_sie_status.v |= fw_update_usb_sie_int_read.v;
    }
    if (fw_update_usb_main_int_read.f.b6) {
        fw_update_usb_ep_int_status.v |= fw_update_usb_ep_int_read.v;
        if (fw_update_usb_ep_int_read.f.b0) {
            fw_update_usb_ep0_int_status.v |= fw_update_usb_ep0_int_read.v;
        }
        if (fw_update_usb_ep_int_read.f.b1) {
            fw_update_usb_epa_int_status.v |= fw_update_usb_epa_int_read.v;
        }
        if (fw_update_usb_ep_int_read.f.b2) {
            if (fw_update_usb_transfer_phase == 0) {
                if (fw_update_record_active.b7 == 0) {
                    SYNC();
                    FB(epb_int_enb)->b4 = 0;
                    SYNC();
                    FB(epb_control)->b1 = 1;
                    fw_update_usb_bulk_out_state = 1;
                    _builtin_set_imask(15);
                    record = fw_update_alloc_record();
                    if (fw_update_disconnected_record.p != 0 && !fw_update_disconnected_record.p->status_flags.b7) {
                        record->next = fw_update_disconnected_record.p;
                    }
                    fw_update_set_current_record(record);
                    ((unsigned char *)record)[5] = 0x81;
                    _builtin_set_imask(saved_imask);
                    fw_update_queue_record(record);
                }
                if (fw_update_usb_epb_int_read.f.b0) {
                    fw_update_usb_epb_event = USB_EVENT_OUT_ERROR;
                }
                if (fw_update_usb_epb_int_read.f.b4) {
                    fw_update_usb_epb_event = USB_EVENT_OUT_ACK;
                }
                fw_update_usb_ep_int_status.f.b2 = 0;
            } else {
                fw_update_usb_epb_int_status.v |= fw_update_usb_epb_int_read.v;
            }
        }
    }
    SYNC();
    if (FB(sie_int_stat)->b5) {
        SYNC();
        FB(usb_control)->b7 = 1;
        fw_update_usb_sie_status.f.bus_reset = 1;
        fw_update_usb_main_status.f.b7 = 1;
        SYNC();
        ((struct usb_regs_int_stat *)((struct usb_regs_main *)fw_update_p_usb_ctrl))->sie_int_stat.b5 = 1;
        SYNC();
        ((struct usb_regs_int_stat *)((struct usb_regs_main *)fw_update_p_usb_ctrl))->main_int_stat.b7 = 1;
    }
}

struct intc_view {
    short ipra;
    short iprb;
    char p[12];
    short icr1;
    short irq_status;
    char q[10];
    short icr2;
};

/* fw_update_iic_ref..fw_update_port_f: RAM copies of the port pointers, and the address of the IIC pointer */


extern void _builtin_set_vbr(void **);

void fw_update_main(int update)
{
    fw_update_set_vbr();
    if (!fw_update_init_ports(update)) {
    }
    _builtin_set_imask(15);
    fw_update_init_timers();
    fw_update_p_sci1->scr.c = 0;
    fw_update_init_intc(update);
    if (update == 0 || !fw_update_hostif_flags.usb) {
        if (!fw_update_main_step_5()) {
        }
        if (!fw_update_hostif_flags.scsi) {
            fw_update_usb_reset_state();
            fw_update_usb_event_flags.b7 = 1;
        }
    } else {
        fw_update_usb_init();
    }
    _builtin_set_imask(0);
    fw_update_begin_session();
    fw_update_state_flags.b7 = 1;
    while (1) {
        fw_update_poll();
    }
}

void fw_update_poll(void)
{
    fw_update_run_event_handler();
    _builtin_set_imask(15);
    if (fw_update_restart_request == 0x80) {
        if (fw_update_hostif_flags.usb == 1) {
            fw_update_restart_arg1 = 0;
            fw_update_restart_arg2 = 1;
        } else {
            SYNC();
            fw_update_restart_arg1 = FW_UPDATE_SCSI_SYNCMODE;
            SYNC();
            if (FW_UPDATE_SCSI_SCSIMODE0_BITS->b4) {
                fw_update_restart_arg2 = 1;
            } else {
                fw_update_restart_arg2 = 0;
            }
        }
        SYNC();
        FW_UPDATE_SCSI_RESET = 7;
        _builtin_set_imask(2);
        fw_update_tick_count = 0;
        while (fw_update_tick_count < 0xfa) {
        }
        fw_update_shutdown();
    }
    _builtin_set_imask(0);
    if (!fw_update_hostif_flags.scsi) {
        fw_update_usb_run_events();
        if (fw_update_usb_ctrl_flags.b7) {
            fw_update_usb_ep0_run();
        }
        fw_update_usb_irq_device_events();
    }
}

void fw_update_set_vbr(void)
{
    _builtin_set_vbr(((void **)boot_update_vectors));
}

void fw_update_init_intc(int update)
{
    ((struct intc_view *)fw_update_p_intc)->icr1 = 0xff;
    ((struct intc_view *)fw_update_p_intc)->icr2 = 0;
    *fw_update_p_intc = 3;
    fw_update_p_intc[1] = 0;
    fw_update_p_intc[2] = 0;
    fw_update_p_intc[3] = 0;
    fw_update_p_intc[4] = 0;
    fw_update_p_intc[5] = 0;
    fw_update_p_intc[6] = 6;
    fw_update_p_intc[7] = 0;
    fw_update_p_intc[10] = 0;
    fw_update_p_intc[11] = 0;
    if (update) {
    } else {
        while (((struct intc_view *)fw_update_p_intc)->irq_status) {
            fw_update_p_intc[9] = 0;
        }
    }
}

void fw_update_init_timers(void)
{
    *(short *)fw_update_p_cmt = 0;
    ((struct cmt_view *)fw_update_p_cmt1)->cmstr = 66;
    ((struct cmt_view *)fw_update_p_cmt1)->cmcsr0 = 0;
    ((struct cmt_view *)fw_update_p_cmt1)->cmcnt0 = 0x186;
    fw_update_p_mtu->tstr = 0;
    fw_update_p_mtu->tsyr.c = 0;
    ((struct mtu_view *)fw_update_p_cmt)->tsyr.b.b1 = 1;
}

int fw_update_init_ports(int update)
{
    int i;

    _builtin_set_imask(15);
    /* RAM copies: flash block 0 (program 1 and its pointers) may be erased */
    fw_update_port_a = (volatile struct st_pa *)&boot_p_pa->DR;
    fw_update_port_b = (volatile struct st_pb *)&boot_p_pb->DR;
    (*(volatile struct st_pe **)&fw_update_port_e) = (volatile struct st_pe *)&boot_p_pe->DRL;
    fw_update_port_f = (volatile struct st_pf *)&boot_p_pf->DR;
    fw_update_iic_ref = &boot_p_iic;
    fw_update_panel_init();
    fw_update_p_board_ctrl->reg_02.hostif_idle = 0;
    fw_update_p_board_ctrl->reg_02.standby = 0;
    fw_update_p_board_ctrl->reg_04.hostif_idle_enable = 0;
    fw_update_p_board_ctrl->reg_0a.power_enable_8 = 1;
    fw_update_p_board_ctrl->reg_0a.power_enable_9 = 1;
    fw_update_p_board_ctrl->reg_02.hostif_idle = 0;
    fw_update_p_board_ctrl->reg_02.standby = 0;
    fw_update_port_a->DR.BIT.B8 = 1;
    if (update) {
    } else {
        fw_update_port_b->DR.BIT.B7 = 1;
        fw_update_p_board_ctrl->reg_0c.b11 = 1;
    }
    ((volatile struct st_pe *)fw_update_port_e)->DRL.BIT.B12 = 0;
    fw_update_p_board_ctrl->reg_08.b9 = 1;
    fw_update_p_board_ctrl->reg_08.b11 = 0;
    for (i = 0; i < 20; i++) {
        fw_update_delay_9656();
    }
    fw_update_port_b->DR.BIT.B7 = 0;
    fw_update_p_board_ctrl->reg_0c.b11 = 0;
    _builtin_set_imask(0);
    return 1;
}

/* Stops every interrupt source and restarts through program 1 with the host interface fw_update_poll saved. */
void fw_update_shutdown(void)
{
    int i = 0;

    fw_update_flash_read_mode();
    _builtin_set_imask(15);
    SYNC();
    FW_UPDATE_SCSI_RESET = 7;
    SYNC();
    FW_UPDATE_SCSI_SCSIMODE0_BITS->b7 = 0;
    SYNC();
    FW_UPDATE_SCSI_SCSIMODE0_BITS->b6 = 1;
    SYNC();
    FW_UPDATE_SCSI_DTBC2 = 118;
    SYNC();
    FW_UPDATE_SCSI_DTBC1 = 84;
    SYNC();
    FW_UPDATE_SCSI_DTBC0 = 50;
    SYNC();
    SYNC();
    FW_UPDATE_USB_MAIN_INT_ENB = 0;
    SYNC();
    FW_UPDATE_USB_SIE_INT_ENB = 0;
    SYNC();
    FW_UPDATE_USB_EPR_INT_ENB = 0;
    SYNC();
    *fw_update_p_intc = 0;
    ((struct intc_view *)fw_update_p_intc)->iprb = 0;
    fw_update_p_cmt->cmstr = 0;
    if (((struct mtu_view *)((struct cmt_view *)fw_update_p_cmt1))->tsyr.b.b7) {
        ((struct mtu_view *)((struct cmt_view *)fw_update_p_cmt1))->tsyr.b.b7 = 0;
    }
    ((struct cmt_view *)fw_update_p_cmt1)->cmstr = 0;
    ((struct cmt_view *)fw_update_p_cmt1)->cmcsr0 = 0;
    fw_update_p_mtu->tstr = 0;
    if (fw_update_p_mtu2->tsr.b0) {
        fw_update_p_mtu2->tsr.b0 = 0;
    }
    fw_update_p_mtu2->tier.b0 = 0;
    fw_update_p_mtu2->tcnt = 0;
    fw_update_p_sci1->scr.b.b4 = 0;
    fw_update_p_sci1->scr.b.b6 = 0;
    fw_update_restart_boot(fw_update_restart_arg1, fw_update_restart_arg2);
}

void fw_update_run_event_handler(void)
{
    void (*handler)(struct fw_update_command_view_3 *);
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (fw_update_record_queue[0].p && fw_update_record_queue[0].p->in_use) {
        handler = fw_update_record_handler_table[fw_update_record_queue[0].p->handler];
        _builtin_set_imask(saved_imask);
        handler(fw_update_record_queue[0].p);
    }
    _builtin_set_imask(saved_imask);
}

void fw_update_handle_default(int record)
{
    fw_update_unqueue_record((struct fw_update_command_link *)record);
}


#define RET0 { _builtin_set_imask(m); return 0; }

int fw_update_main_step_5(void)
{
    int m;
    unsigned char scsi_id;
    unsigned char vbus;
    unsigned char own_id;

    m = _builtin_get_imask();
    fw_update_ua_reset_pending = 0xff;
    fw_update_ua_reset_pending_usb = 0xff;
    _builtin_set_imask(15);
    scsi_id = fw_update_port_f->DR.BYTE;
    scsi_id = (unsigned char)(scsi_id & 60);
    scsi_id = scsi_id >> 2;
    if (scsi_id <= 7) {
        scsi_id = 5;
    } else if (scsi_id == 8) {
        scsi_id = 7;
    } else if (scsi_id == 9) {
        scsi_id = 6;
    } else if (scsi_id == 10) {
        scsi_id = 5;
    } else if (scsi_id == 11) {
        scsi_id = 4;
    } else if (scsi_id == 12) {
        scsi_id = 3;
    } else if (scsi_id == 13) {
        scsi_id = 2;
    } else if (scsi_id == 14) {
        scsi_id = 1;
    } else if (scsi_id == 15) {
        scsi_id = 0;
    }
    SYNC();
    FW_UPDATE_SCSI_OWNID = scsi_id;
    SYNC();
    own_id = scsi_id;
    SYNC();
    if ((*(union fw_update_usb_int_bits *)&fw_update_bus_reset_seen).v != 0) {
    } else {
        SYNC();
        SYNC();
        FW_UPDATE_SCSI_DTBC2 = 0xff;
        SYNC();
        FW_UPDATE_SCSI_DTBC1 = 0xff;
        SYNC();
        FW_UPDATE_SCSI_DTBC0 = 0xff;
        SYNC();
        if (FW_UPDATE_SCSI_DTBC2 != 0xff) RET0
        SYNC();
        if (FW_UPDATE_SCSI_DTBC1 != 0xff) RET0
        SYNC();
        if (FW_UPDATE_SCSI_DTBC0 != 0xff) RET0
        SYNC();
        FW_UPDATE_SCSI_DTBC2 = 0;
        SYNC();
        FW_UPDATE_SCSI_DTBC1 = 0;
        SYNC();
        FW_UPDATE_SCSI_DTBC0 = 0;
        SYNC();
        if (FW_UPDATE_SCSI_DTBC2 != 0) RET0
        SYNC();
        if (FW_UPDATE_SCSI_DTBC1 != 0) RET0
        SYNC();
        if (FW_UPDATE_SCSI_DTBC0 != 0) RET0
    }
    SYNC();
    FW_UPDATE_SCSI_RESET = 7;
    SYNC();
    FW_UPDATE_SCSI_SCSIMODE0 = 67;
    SYNC();
    FW_UPDATE_SCSI_SCSIMODE1 = 0x81;
    SYNC();
    FW_UPDATE_SCSI_OWNID = own_id;
    SYNC();
    FW_UPDATE_SCSI_CONFIG0_BITS->b6 = 0;
    SYNC();
    FW_UPDATE_SCSI_CONFIG0 = 0xa0;
    SYNC();
    FW_UPDATE_SCSI_CONFIG1 = 1;
    SYNC();
    scsi_id = FW_UPDATE_SCSI_SCSIINT1;
    SYNC();
    FW_UPDATE_SCSI_SCSIINT1 = scsi_id;
    SYNC();
    scsi_id = FW_UPDATE_SCSI_SCSIINT2;
    SYNC();
    FW_UPDATE_SCSI_SCSIINT2 = scsi_id;
    SYNC();
    scsi_id = FW_UPDATE_SCSI_MAININT;
    SYNC();
    FW_UPDATE_SCSI_MAININT = scsi_id;
    SYNC();
    FW_UPDATE_SCSI_DMACTL_BITS->b1 = 1;
    SYNC();
    FW_UPDATE_SCSI_SDID = 0;
    SYNC();
    FW_UPDATE_SCSI_SELTIME = 0xa0;
    SYNC();
    FW_UPDATE_SCSI_FIFOCTL_BITS->b2 = 1;
    SYNC();
    FW_UPDATE_SCSI_SYNCMODE = fw_update_restart_arg1;
    if (fw_update_restart_arg2) {
        SYNC();
        FW_UPDATE_SCSI_SCSIMODE0_BITS->b4 = 1;
    }
    fw_update_main_step_5_1();
    vbus = fw_update_usb_read_vbus();
    _builtin_set_imask(15);
    if (vbus == 1) {
        if (fw_update_hostif_flags.scsi == 1) {
            fw_update_hostif_flags.usb = 0;
            fw_update_main_step_5_3(0, 0);
            fw_update_hostif_flags.scsi = 1;
            fw_update_main_step_5_3(1, 1);
        } else {
            fw_update_hostif_flags.scsi = 0;
            fw_update_main_step_5_3(0, 1);
            fw_update_hostif_flags.usb = 1;
            fw_update_main_step_5_3(1, 0);
        }
    } else if (fw_update_hostif_flags.scsi == 1) {
        fw_update_hostif_flags.usb = 0;
        fw_update_main_step_5_3(0, 0);
        fw_update_hostif_flags.scsi = 1;
        fw_update_main_step_5_3(1, 1);
    } else if (fw_update_hostif_flags.usb == 1) {
        fw_update_hostif_flags.scsi = 0;
        fw_update_main_step_5_3(0, 1);
        fw_update_hostif_flags.usb = 1;
        fw_update_main_step_5_3(1, 0);
    } else {
        fw_update_hostif_flags.scsi = 0;
        fw_update_main_step_5_3(1, 1);
        fw_update_hostif_flags.usb = 0;
        fw_update_main_step_5_3(1, 0);
    }
    fw_update_bus_reset_seen.b7 = 0;
    _builtin_set_imask(m);
    return 1;
}

int fw_update_main_step_5_1(void)
{
    int m;
    int d;

    m = _builtin_get_imask();
    fw_update_p_pisces[0]->n.b0 = 0;
    fw_update_p_pisces_ctrl[0]->f8 = 3;
    fw_update_p_pisces_ctrl[0]->hostif_stop = 1;
    fw_update_ua_reset_pending = 0xff;
    fw_update_ua_reset_pending_usb = 0xff;
    _builtin_set_imask(15);
    if (!fw_update_bus_reset_seen.b7) {
        SYNC();
        FW_UPDATE_USB_PM_CONTROL_BITS->b0 = 0;
        while (1) {
            SYNC();
            if (!FW_UPDATE_USB_PM_CONTROL_BITS->b7) {
                break;
            }
        }
        SYNC();
        FW_UPDATE_USB_CHIP_RESET_BITS->b0 = 1;
        SYNC();
        FW_UPDATE_USB_REG_17_BITS->b5 = 1;
        SYNC();
        FW_UPDATE_USB_EPA_INT_ENB = 0xff;
        SYNC();
        if (FW_UPDATE_USB_EPA_INT_ENB != 0xff) RET0
        SYNC();
        FW_UPDATE_USB_EPA_INT_ENB = 0;
        SYNC();
        if (FW_UPDATE_USB_EPA_INT_ENB != 0) RET0
        SYNC();
        FW_UPDATE_USB_USB_BANK = 2;
        SYNC();
        if (FW_UPDATE_USB_USB_BANK != 2) {
            SYNC();
            FW_UPDATE_USB_USB_BANK = 0;
            RET0
        }
        SYNC();
        FW_UPDATE_USB_USB_BANK = 1;
        SYNC();
        if (FW_UPDATE_USB_USB_BANK != 1) {
            SYNC();
            FW_UPDATE_USB_USB_BANK = 0;
            RET0
        }
        SYNC();
        FW_UPDATE_USB_USB_BANK = 0;
        SYNC();
        if (FW_UPDATE_USB_USB_BANK != 0) RET0
        SYNC();
        FW_UPDATE_USB_USB_BANK = 1;
        SYNC();
        FW_UPDATE_USB_B1_REG_20 = 16;
        SYNC();
        FW_UPDATE_USB_USB_BANK = 0;
        SYNC();
        FW_UPDATE_USB_MAIN_INT_ENB = 0xff;
        SYNC();
        if (FW_UPDATE_USB_MAIN_INT_ENB != 0xff) RET0
        SYNC();
        FW_UPDATE_USB_MAIN_INT_ENB = 0;
        SYNC();
        if (FW_UPDATE_USB_MAIN_INT_ENB != 0) RET0
        SYNC();
        FW_UPDATE_USB_USB_BANK = 1;
        SYNC();
        FW_UPDATE_USB_REG_25 = 15;
        SYNC();
        if (FW_UPDATE_USB_REG_25 != 15) {
            SYNC();
            FW_UPDATE_USB_USB_BANK = 0;
            RET0
        }
        SYNC();
        FW_UPDATE_USB_REG_25 = 0;
        SYNC();
        if (FW_UPDATE_USB_REG_25 != 0) {
            SYNC();
            FW_UPDATE_USB_USB_BANK = 0;
            RET0
        }
    }
    SYNC();
    FW_UPDATE_USB_CHIP_RESET_BITS->b0 = 1;
    SYNC();
    FW_UPDATE_USB_MAIN_INT_ENB = 0;
    SYNC();
    FW_UPDATE_USB_USB_CONTROL = 0;
    SYNC();
    FW_UPDATE_USB_XCVR_CONTROL = 65;
    SYNC();
    FW_UPDATE_USB_USB_TEST = 0;
    SYNC();
    FW_UPDATE_USB_USB_BANK = 1;
    SYNC();
    FW_UPDATE_USB_B1_REG_20 = 16;
    SYNC();
    FW_UPDATE_USB_EP0_INT_STAT = FW_UPDATE_USB_EP0_INT_STAT;
    SYNC();
    FW_UPDATE_USB_EPA_INT_STAT = FW_UPDATE_USB_EPA_INT_STAT;
    SYNC();
    FW_UPDATE_USB_EPB_INT_STAT = FW_UPDATE_USB_EPB_INT_STAT;
    SYNC();
    FW_UPDATE_USB_REG_06 = FW_UPDATE_USB_REG_06;
    SYNC();
    FW_UPDATE_USB_EPR_INT_STAT = FW_UPDATE_USB_EPR_INT_STAT;
    SYNC();
    FW_UPDATE_USB_EPR_INT_STAT = FW_UPDATE_USB_EPR_INT_STAT;
    SYNC();
    FW_UPDATE_USB_SIE_INT_STAT = FW_UPDATE_USB_SIE_INT_STAT;
    SYNC();
    FW_UPDATE_USB_MAIN_INT_STAT = FW_UPDATE_USB_MAIN_INT_STAT;
    SYNC();
    FW_UPDATE_USB_B1_REG_22 = 6;
    SYNC();
    FW_UPDATE_USB_B1_REG_23 = 1;
    SYNC();
    FW_UPDATE_USB_B1_REG_24 = 0;
    SYNC();
    FW_UPDATE_USB_REG_25 = 0;
    SYNC();
    FW_UPDATE_USB_USB_BANK = 2;
    SYNC();
    FW_UPDATE_USB_B2_REG_30 = 2;
    SYNC();
    FW_UPDATE_USB_B2_REG_31 = 0;
    SYNC();
    FW_UPDATE_USB_B2_REG_32 = 6;
    SYNC();
    FW_UPDATE_USB_B2_REG_33 = 0;
    SYNC();
    FW_UPDATE_USB_B2_REG_34 = 10;
    SYNC();
    FW_UPDATE_USB_B2_REG_35 = 0;
    SYNC();
    FW_UPDATE_USB_B2_REG_36 = 10;
    SYNC();
    FW_UPDATE_USB_B2_REG_37 = 0;
    _builtin_set_imask(m);
    return 1;
}

void fw_update_main_step_5_3(int enable, int scsi)
{
    unsigned char status;

    if (scsi) {
        if (enable) {
            SYNC();
            FW_UPDATE_SCSI_SCSIMODE1_BITS->b5 = 1;
            SYNC();
            FW_UPDATE_SCSI_COMMAND = 12;
            SYNC();
            FW_UPDATE_SCSI_SCSIMODE0_BITS->b7 = 1;
            SYNC();
            FW_UPDATE_SCSI_SCSIMODE0_BITS->b6 = 0;
            SYNC();
            while (FW_UPDATE_SCSI_SCSIINT1) {
                SYNC();
                status = FW_UPDATE_SCSI_SCSIINT1;
                SYNC();
                FW_UPDATE_SCSI_SCSIINT1 = status;
                SYNC();
                status = FW_UPDATE_SCSI_SCSIINT2;
                SYNC();
                FW_UPDATE_SCSI_SCSIINT2 = status;
                SYNC();
                status = FW_UPDATE_SCSI_MAININT;
                SYNC();
                FW_UPDATE_SCSI_MAININT = status;
            }
        } else {
            SYNC();
            FW_UPDATE_SCSI_SCSIMODE0 = 67;
            SYNC();
            status = FW_UPDATE_SCSI_SCSIINT1;
            SYNC();
            FW_UPDATE_SCSI_SCSIINT1 = status;
            SYNC();
            status = FW_UPDATE_SCSI_SCSIINT2;
            SYNC();
            FW_UPDATE_SCSI_SCSIINT2 = status;
            SYNC();
            status = FW_UPDATE_SCSI_MAININT;
            SYNC();
            FW_UPDATE_SCSI_MAININT = status;
            SYNC();
            FW_UPDATE_SCSI_CONFIG0_BITS->b7 = 0;
        }
    } else {
        if (enable) {
            SYNC();
            FW_UPDATE_USB_MAIN_INT_ENB = 0xc4;
            SYNC();
            FW_UPDATE_USB_SIE_INT_ENB = 0x86;
            SYNC();
            FW_UPDATE_USB_EPR_INT_ENB = 7;
            SYNC();
            FW_UPDATE_USB_USB_BANK = 0;
            if (!fw_update_usb_wake_events.b5) {
                fw_update_usb_event_flags.b7 = 1;
                fw_update_usb_wake_events.b5 = 1;
            }
        } else {
            SYNC();
            FW_UPDATE_USB_USB_BANK = 1;
            SYNC();
            FW_UPDATE_USB_B1_REG_22_BITS->b7 = 0;
        }
    }
    return;
}


int fw_update_panel_init(void)
{
    boot_p_iic->SCRX.BIT.IICE = 1;
    boot_p_iic->ICCR.BIT.ICE = 1;
    boot_p_iic->ICCR.BIT.ICE = 1;
    boot_p_iic->ICSR.BYTE = 0;
    boot_p_iic->SCRX.BIT.IICX0 = 0;
    boot_p_iic->EQU.ICE1.UN_ICMR.BIT.MLS = 0;
    boot_p_iic->EQU.ICE1.UN_ICMR.BIT.WAIT = 0;
    boot_p_iic->EQU.ICE1.UN_ICMR.BIT.CKS = 3;
    boot_p_iic->EQU.ICE1.UN_ICMR.BIT.BC = 0;
    boot_p_iic->SCRX.BIT.HNDS = 1;
    boot_p_iic->ICCR.BIT.IEIC = 0;
    boot_p_iic->ICCR.BIT._MST = 0;
    boot_p_iic->ICCR.BIT.TRS = 0;
    boot_p_iic->ICCR.BIT.ACKE = 1;
    if (fw_update_iic_wait_bus_free() == 0)
        return 0;
    boot_p_iic->ICCR.BYTE = 0xb1;
    boot_p_iic->ICCR.BYTE = 0xb4;
    fw_update_iic_wait(0);
    fw_update_iic_write_byte(PANEL_EXPANDER_WRITE);
    /* panel expander 0x48: registers 6/7 = port direction */
    fw_update_iic_write_byte(PANEL_EXPANDER_CONFIG_0);
    fw_update_iic_write_byte(0x8f);
    fw_update_iic_write_byte(0);
    fw_update_iic_clear_iric();
    boot_p_iic->ICCR.BYTE = 0xb0;
    fw_update_iic_wait(3);
    boot_p_iic->ICCR.BYTE = 0xa1;
    if (fw_update_iic_wait_bus_free() == 0)
        return 0;
    boot_p_iic->ICCR.BYTE = 0xb1;
    boot_p_iic->ICCR.BYTE = 0xb4;
    fw_update_iic_wait(0);
    fw_update_iic_write_byte(PANEL_EXPANDER_WRITE);
    fw_update_iic_write_byte(PANEL_EXPANDER_CONFIG_1);
    fw_update_iic_write_byte(0);
    fw_update_iic_write_byte(0x8f);
    boot_p_iic->ICCR.BYTE = 0xb0;
    fw_update_iic_wait(3);
    boot_p_iic->ICCR.BYTE = 0xa1;
    if (fw_update_panel_set_digit(0x61) == 0)
        return 0;
}

void fw_update_iic_write_byte(unsigned char byte)
{
    boot_p_iic->EQU.ICE1.UN_ICDR = byte;
    fw_update_iic_clear_iric();
    fw_update_iic_wait(1);
    return;
}

int fw_update_iic_wait_bus_free(void)
{
    unsigned int i;
    unsigned int j;

    for (i = 0; i < 0xffffffff; i++) {
        if (boot_p_iic->ICCR.BIT.BBSY == 0)
            return 1;
        else {
        for (j = 0; j < 0xffffffff; j++) {
            if (boot_p_iic->ICCR.BIT.BBSY == 0)
                return 1;
        }
        }
    }
    return 0;
}

int fw_update_iic_wait(unsigned char phase)
{
    unsigned int i;
    unsigned int j;

    if (phase == 3) {
        if (fw_update_iic_wait_bus_free() == 0)
            return 0;
        fw_update_iic_clear_iric();
        return 1;
    }
    for (i = 0; i < 0xffffffff; i++) {
        if (boot_p_iic->ICCR.BIT.IRIC)
            break;
        else {
        for (j = 0; j < 0xffffffff; j++) {
            if (boot_p_iic->ICCR.BIT.IRIC)
                break;
        }
        }
        if (i >= 0xfffffff0)
            return 0;
    }
    if (phase == 0) {
        for (i = 0; i < 0xffffffff; i++) {
            if (boot_p_iic->ICSR.BIT.IRTR)
                break;
            else {
            for (j = 0; j < 0xffffffff; j++) {
                if (boot_p_iic->ICSR.BIT.IRTR)
                    break;
            }
            }
            if (i >= 0xfffffff0)
                return 0;
        }
        fw_update_iic_clear_iric();
        return 1;
    } else if (phase == 1) {
        for (i = 0; i < 0xffffffff; i++) {
            if (boot_p_iic->ICSR.BIT.ACKB == 0)
                break;
            else {
            for (j = 0; j < 0xffffffff; j++) {
                if (boot_p_iic->ICSR.BIT.ACKB == 0)
                    break;
            }
            }
            if (i >= 0xfffffff0)
                return 0;
        }
        fw_update_iic_clear_iric();
    } else if (phase == 2) {
        for (i = 0; i < 0xffffffff; i++) {
            if (boot_p_iic->SCRX.BIT.ICDRF0 && boot_p_iic->ICCR.BIT.BBSY)
                break;
            else {
            for (j = 0; j < 0xffffffff; j++) {
                if (boot_p_iic->SCRX.BIT.ICDRF0 && boot_p_iic->ICCR.BIT.BBSY)
                    break;
            }
            }
            if (i >= 0xfffffff0)
                return 0;
        }
        fw_update_iic_clear_iric();
        return 1;
    }
}

void fw_update_iic_clear_iric(void)
{
    boot_p_iic->ICCR.BIT.IRIC = 0;
    return;
}

int fw_update_panel_set_digit(unsigned char digit)
{
    if (fw_update_iic_wait_bus_free() == 0)
        return 0;
    boot_p_iic->ICCR.BYTE = 0xb1;
    boot_p_iic->ICCR.BYTE = 0xb4;
    fw_update_iic_wait(0);
    fw_update_iic_write_byte(PANEL_EXPANDER_WRITE);
    fw_update_iic_write_byte(PANEL_EXPANDER_OUTPUT_1);
    fw_update_iic_write_byte(~(digit | 0x80));
    fw_update_iic_write_byte(0x10);
    boot_p_iic->ICCR.BYTE = 0xb0;
    fw_update_iic_wait(3);
    boot_p_iic->ICCR.BYTE = 0xa1;
    return 1;
}
