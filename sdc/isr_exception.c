#include "fw.h"
struct scsi_regs_isr_exception {
    char pad_00;
    unsigned char mainint;
    char pad_02;
    unsigned char scsiint1;
    char pad_04;
    unsigned char scsiint2;
    char pa[9];
    unsigned char reset;
    char pb[3];
    struct Bits scsimode0;
    char pd[37];
    struct Bits config0;
};
struct usb_regs_isr_exception {
    char pad_00;
    unsigned char main_int_stat;
    char pad_02;
    unsigned char sie_int_stat;
    char pad_04;
    unsigned char reg_02;
    char pad_06;
    unsigned char ep0_int_stat;
    char pad_08;
    unsigned char epa_int_stat;
    char pad_0a;
    unsigned char epb_int_stat;
    char pa[5];
    unsigned char main_int_enb;
    char pad_12;
    unsigned char sie_int_enb;
    char pad_14;
    unsigned char reg_0a;
    char pb[11];
    struct Bits chip_reset;
    char pe[3];
    struct Bits pm_control;
    char pad_26;
    struct usb_control_bits usb_control;
    char pc[3];
    unsigned char xcvr_control;
    char pad_2c;
    unsigned char usb_test;
};
struct board_ctrl_reg_02 {
    char pad_00[2];
    struct {
        unsigned short p:4,
                       :2,
                       hostif_idle:1,
                       :9;
    } reg_02;
};

#pragma interrupt(isr_slot_illegal)
void isr_slot_illegal(void)
{
    _builtin_set_imask(15);
    rt_restart();
}
#pragma interrupt(isr_address_error)
void isr_address_error(void)
{
    _builtin_set_imask(15);
    rt_restart();
}
#pragma interrupt(isr_dma_address_error)
void isr_dma_address_error(void)
{
    _builtin_set_imask(15);
    rt_restart();
}
#pragma interrupt(isr_nmi)
void isr_nmi(void)
{
    volatile unsigned short *ipr;
    int i;

    ipr = &p_intc->IPRA.WORD;
    for (i = 0; i < 8; i++) {
        *ipr = 0;
    }
    p_intc->IPRI.WORD = 0;
    p_intc->IPRJ.WORD = 0;
    _builtin_set_imask(15);
    rt_restart();
}
#pragma interrupt(isr_user_break)
void isr_user_break(void)
{
    _builtin_set_imask(15);
    rt_restart();
}
#pragma interrupt(isr_unused)
void isr_unused(void)
{
    _builtin_set_imask(15);
    rt_restart();
}
void isr_reset_hostif(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    p_intc->IPRA.WORD = 0xfffc;
    isr_stop_output_dma();
    ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsimode0.b7 = 0;
    _builtin_set_gbr(0);
    ((struct scsi_regs_isr_exception *)scsi_ctrl)->reset = 7;
    _builtin_set_gbr(0);
    ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsimode0.b7 = 0;
    _builtin_set_gbr(0);
    ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsimode0.b6 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->pm_control.b0 = 0;
    for (;;) {
        _builtin_set_gbr(0);
        if (((struct usb_regs_isr_exception *)usb_ctrl)->pm_control.b7 == 0) break;
    }
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->xcvr_control = 65;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->usb_control.active_usb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->usb_control.restore_usb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->usb_control.go_chirp = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->usb_test = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->main_int_enb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->sie_int_enb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->reg_0a = 0;
    _builtin_set_gbr(0);
    {
        unsigned char status;
        status = ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsiint1;
        _builtin_set_gbr(0);
        ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsiint1 = status;
        _builtin_set_gbr(0);
        status = ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsiint2;
        _builtin_set_gbr(0);
        ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsiint2 = status;
        _builtin_set_gbr(0);
        status = ((struct scsi_regs_isr_exception *)scsi_ctrl)->mainint;
        _builtin_set_gbr(0);
        ((struct scsi_regs_isr_exception *)scsi_ctrl)->mainint = status;
    }
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->sie_int_stat = ((struct usb_regs_isr_exception *)usb_ctrl)->sie_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->ep0_int_stat = ((struct usb_regs_isr_exception *)usb_ctrl)->ep0_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->epa_int_stat = ((struct usb_regs_isr_exception *)usb_ctrl)->epa_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->epb_int_stat = ((struct usb_regs_isr_exception *)usb_ctrl)->epb_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->reg_02 = ((struct usb_regs_isr_exception *)usb_ctrl)->reg_02;
    _builtin_set_gbr(0);
    ((struct usb_regs_isr_exception *)usb_ctrl)->main_int_stat = ((struct usb_regs_isr_exception *)usb_ctrl)->main_int_stat;
    if (p_intc->ISR.BIT.IRQ3F) {
        p_intc->ISR.WORD = 0xef;
    }
    _builtin_set_imask(saved_imask);
}
void isr_stop_output_dma(void)
{
    if (((volatile struct board_ctrl_reg_02 *)p_board_ctrl)->reg_02.hostif_idle == 1) {
        ((volatile struct board_ctrl_reg_02 *)p_board_ctrl)->reg_02.hostif_idle = 0;
    }
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
}
void isr_init_scsi_ctrl(void)
{
    int saved_imask;

    saved_imask = _builtin_get_imask();
    if (((struct scsi_regs_isr_exception *)scsi_ctrl) != p_scsi_ctrl) {
        _builtin_set_imask(15);
        (*(struct scsi_regs_isr_exception **)&scsi_ctrl) = p_scsi_ctrl;
        ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsimode0.b7 = 0;
        _builtin_set_gbr(0);
        ((struct scsi_regs_isr_exception *)scsi_ctrl)->reset = 7;
        _builtin_set_gbr(0);
        *(unsigned char *)&((struct scsi_regs_isr_exception *)scsi_ctrl)->scsimode0 = 67;
        _builtin_set_gbr(0);
        {
            unsigned char status;
            status = ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsiint1;
            _builtin_set_gbr(0);
            ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsiint1 = status;
            _builtin_set_gbr(0);
            status = ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsiint2;
            _builtin_set_gbr(0);
            ((struct scsi_regs_isr_exception *)scsi_ctrl)->scsiint2 = status;
            _builtin_set_gbr(0);
            status = ((struct scsi_regs_isr_exception *)scsi_ctrl)->mainint;
            _builtin_set_gbr(0);
            ((struct scsi_regs_isr_exception *)scsi_ctrl)->mainint = status;
        }
        _builtin_set_gbr(0);
        ((struct scsi_regs_isr_exception *)scsi_ctrl)->config0.b7 = 0;
        _builtin_set_imask(saved_imask);
    }
    if (((struct usb_regs_isr_exception *)usb_ctrl) != p_usb_ctrl) {
        _builtin_set_imask(15);
        (*(struct usb_regs_isr_exception **)&usb_ctrl) = p_usb_ctrl;
        p_pisces_hostif[1]->ctrl.f.b0 = 1;
        p_pisces_hostif[1]->ctrl.f.b9 = 0;
        p_pisces_hostif[0]->ctrl.f.b0 = 1;
        p_pisces_hostif[0]->ctrl.f.b9 = 0;
        p_pisces_ctrl[0]->ctrl.f.hostif_stop = 1;
        _builtin_set_imask(saved_imask);
        while (1) {
            if (p_pisces_hostif[0]->ctrl.f.b1 && p_pisces_hostif[1]->ctrl.f.b1) break;
        }
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->chip_reset.b0 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->xcvr_control = 65;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->usb_control.active_usb = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->usb_control.restore_usb = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->usb_control.go_chirp = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->usb_test = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->main_int_enb = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->sie_int_enb = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->reg_0a = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->sie_int_stat = ((struct usb_regs_isr_exception *)usb_ctrl)->sie_int_stat;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->ep0_int_stat = ((struct usb_regs_isr_exception *)usb_ctrl)->ep0_int_stat;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->epa_int_stat = ((struct usb_regs_isr_exception *)usb_ctrl)->epa_int_stat;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->epb_int_stat = ((struct usb_regs_isr_exception *)usb_ctrl)->epb_int_stat;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->reg_02 = ((struct usb_regs_isr_exception *)usb_ctrl)->reg_02;
        _builtin_set_gbr(0);
        ((struct usb_regs_isr_exception *)usb_ctrl)->main_int_stat = ((struct usb_regs_isr_exception *)usb_ctrl)->main_int_stat;
        _builtin_set_imask(saved_imask);
    }
}
