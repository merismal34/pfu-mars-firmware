#define fw_update_usb_address fw_update_usb_address_hdr
#include "boot.h"
#undef fw_update_usb_address
#pragma section 2
struct usb_reg_41_bits {
    unsigned char vbus:1,
                  full_speed:1,
                  :4,
                  line_state:2;
};
struct usb_reg_43_bits {
    unsigned char term_select:1,
                  xcvr_select:1,
                  :4,
                  op_mode:2;
};
struct usb_regs_device {
    char pad_00;
    union CB main_int_stat;
    char pad_02;
    char sie_int_stat;
    char reg_02_08[15];
    struct Bits sie_int_enb;
    char reg_0a[3];
    char ep0_int_enb;
    char pad_18;
    char epa_int_enb;
    char pad_1a;
    union CB epb_int_enb;
    char reg_0e_11[9];
    struct Bits pm_control;
    char pad_26;
    struct usb_control_bits usb_control;
    char pad_28;
    struct usb_reg_41_bits usb_status;
    char pad_2a;
    struct usb_reg_43_bits xcvr_control;
    char pad_2c;
    char usb_test;
    char pad_2e;
    struct Bits reg_17;
    char reg_18_1e[15];
    char usb_bank;
    char reg_20_21[5];
    struct Bits epa_config_0;
    char reg_23_25[7];
    struct Bits epb_config_0;
    char reg_27[3];
    char address;
    char reg_29[3];
    struct Bits epc_config_0;
    char pad_56;
    struct Bits ep0_control_out;
    char reg_2c[3];
    struct Bits epb_control;
    char reg_2e[3];
    char ep0_max_size;
};

struct fw_update_usb_endpoint_entry {
    char pad_00[2];
    unsigned char bEndpointAddress;
    char pad_03;
    unsigned char wMaxPacketSize_lo, wMaxPacketSize_hi;
    char pad_06[2];
};
struct fw_update_usb_interface_link { char p[12]; struct fw_update_usb_endpoint_entry *e; };
extern unsigned char fw_update_usb_address;

void fw_update_usb_irq_device_events(void)
{
    unsigned char vbus;
    unsigned char saved_imask;
    short z[5];

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (fw_update_usb_main_status.f.b7) {
        if (fw_update_usb_sie_status.f.vbus_changed) {
            vbus = fw_update_usb_read_vbus();
            if (vbus == 0) {
                fw_update_usb_event_flags.b6 = 1;
                fw_update_usb_main_status.v = 0;
            }
        } else if (fw_update_usb_sie_status.f.bus_reset) {
            fw_update_usb_event_flags.b5 = 1;
        } else if (fw_update_usb_sie_status.f.suspend) {
            fw_update_usb_event_flags.b4 = 1;
            fw_update_usb_main_status.v = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_enb.b6 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.dis_bus_detect = 1;
        }
        fw_update_usb_sie_status.v = 0;
    }
    if (fw_update_usb_main_status.f.b2)
        fw_update_usb_epa_event = USB_EVENT_DMA_DONE;
    if (fw_update_usb_main_status.f.b6)
        fw_update_usb_update_ep_status();
    fw_update_sync_dst = fw_update_sync_src;
    if (((struct usb_regs_device *)fw_update_p_usb_ctrl)->main_int_stat.b.b0) {
        if (fw_update_usb_ep0_stage == 7) {
            if (!fw_update_usb_ctrl_flags.b7) {
                fw_update_usb_ctrl_flags.b7 = 1;
                fw_update_usb_ep0_stage = 0;
            }
        }
    }
    fw_update_usb_main_status.v = 0;
    _builtin_set_imask(saved_imask);
}

void fw_update_usb_poll_events(void)
{
    unsigned char vbus;
    unsigned char saved_imask;
    short z[3];

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (fw_update_usb_main_status.f.b7) {
        if (fw_update_usb_sie_status.f.vbus_changed) {
            vbus = fw_update_usb_read_vbus();
            if (vbus == 0) {
                fw_update_usb_event_flags.b6 = 1;
                fw_update_usb_main_status.v = 0;
            }
        } else if (fw_update_usb_sie_status.f.bus_reset) {
            fw_update_usb_event_flags.b5 = 1;
        } else if (fw_update_usb_sie_status.f.suspend) {
            fw_update_usb_event_flags.b4 = 1;
            fw_update_usb_main_status.v = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_enb.b6 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.dis_bus_detect = 1;
        }
        fw_update_usb_sie_status.v = 0;
    }
    if (fw_update_usb_main_status.f.b2)
        fw_update_usb_epa_event = USB_EVENT_DMA_DONE;
    if (fw_update_usb_main_status.f.b6)
        fw_update_usb_update_ep_status();
    fw_update_usb_main_status.v = 0;
    _builtin_set_imask(saved_imask);
}

void fw_update_usb_update_ep_status(void)
{
    if (fw_update_usb_ep_int_status.f.b0) {
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_usb_ep0_int_status.v = fw_update_usb_ep0_int_status.v & (*(struct usb_regs_device **)&fw_update_p_usb_ctrl)->ep0_int_enb;
        if (fw_update_usb_ep0_int_status.f.b1)
            fw_update_usb_ep0_event = USB_EVENT_IN_ERROR;
        if (fw_update_usb_ep0_int_status.f.b0)
            fw_update_usb_ep0_event = USB_EVENT_OUT_ERROR;
        if (fw_update_usb_ep0_int_status.f.b5)
            fw_update_usb_ep0_event = USB_EVENT_IN_ACK;
        if (fw_update_usb_ep0_int_status.f.b4)
            fw_update_usb_ep0_event = USB_EVENT_OUT_ACK;
        fw_update_usb_ep0_int_status.v = 0;
    }
    if (fw_update_usb_ep_int_status.f.b1) {
        if (fw_update_usb_epa_event != USB_EVENT_DMA_DONE) {
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_usb_epa_int_status.v = fw_update_usb_epa_int_status.v & (*(struct usb_regs_device **)&fw_update_p_usb_ctrl)->epa_int_enb;
            if (fw_update_usb_epa_int_status.f.b1)
                fw_update_usb_epa_event = USB_EVENT_IN_ERROR;
            if (fw_update_usb_epa_int_status.f.b0)
                fw_update_usb_epa_event = USB_EVENT_OUT_ERROR;
            if (fw_update_usb_epa_int_status.f.b5)
                fw_update_usb_epa_event = USB_EVENT_IN_ACK;
            if (fw_update_usb_epa_int_status.f.b4)
                fw_update_usb_epa_event = USB_EVENT_OUT_ACK;
        }
        fw_update_usb_epa_int_status.v = 0;
    }
    if (fw_update_usb_ep_int_status.f.b2) {
        if (fw_update_usb_epb_event != 8) {
            fw_update_sync_dst = fw_update_sync_src;
            fw_update_usb_epb_int_status.v = fw_update_usb_epb_int_status.v & (*(struct usb_regs_device **)&fw_update_p_usb_ctrl)->epb_int_enb.c;
            if (fw_update_usb_epb_int_status.f.b1)
                fw_update_usb_epb_event = USB_EVENT_IN_ERROR;
            if (fw_update_usb_epb_int_status.f.b0)
                fw_update_usb_epb_event = USB_EVENT_OUT_ERROR;
            if (fw_update_usb_epb_int_status.f.b5)
                fw_update_usb_epb_event = USB_EVENT_IN_ACK;
            if (fw_update_usb_epb_int_status.f.b4)
                fw_update_usb_epb_event = USB_EVENT_OUT_ACK;
        }
        fw_update_usb_epb_int_status.v = 0;
    }
    fw_update_usb_ep_int_status.v = 0;
}

void fw_update_usb_run_events(void)
{
    if (*(unsigned char *)&fw_update_usb_event_flags) {
        if (fw_update_usb_event_flags.b6)
            fw_update_usb_detach();
        if (fw_update_usb_event_flags.b5)
            fw_update_usb_handle_bus_reset();
        if (fw_update_usb_event_flags.b4)
            fw_update_usb_handle_suspend();
        if (fw_update_usb_event_flags.b7)
            fw_update_usb_handle_attach();
    }
}

void fw_update_usb_run_events_6_4(void)
{
    if (*(unsigned char *)&fw_update_usb_event_flags) {
        if (fw_update_usb_event_flags.b6)
            fw_update_usb_detach();
        if (fw_update_usb_event_flags.b4)
            fw_update_usb_handle_suspend();
    }
}

int fw_update_usb_read_vbus(void)
{
    int i;
    int vbus;

    fw_update_sync_dst = fw_update_sync_src;
    vbus = ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_status.vbus;
    for (i = 0; i < 5; i++) {
        fw_update_sync_dst = fw_update_sync_src;
        if (((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_status.vbus != vbus) {
            fw_update_sync_dst = fw_update_sync_src;
            vbus = ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_status.vbus;
            i = 0;
        }
    }
    return vbus;
}

int fw_update_usb_read_line_state(void)
{
    int i;
    int line_state;

    fw_update_sync_dst = fw_update_sync_src;
    line_state = ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_status.line_state;
    for (i = 0; i < 5; i++) {
        if (fw_update_usb_sie_status.f.vbus_changed == 1)
            break;
        if ((((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_status.line_state) != line_state) {
            fw_update_sync_dst = fw_update_sync_src;
            line_state = ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_status.line_state;
            i = 0;
        }
    }
    return line_state;
}

void fw_update_usb_start_receive(int len)
{
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_usb_bulk_len = len;
    fw_update_usb_transfer_flags.done = 0;
    fw_update_usb_transfer_flags.pending = 0;
    fw_update_usb_transfer_flags.pisces_in = 0;
    fw_update_usb_bulk_out_state = 1;
    _builtin_set_imask(15);
    fw_update_usb_transfer_phase = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->epb_int_enb.b.b4 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->epb_control.b1 = 0;
    _builtin_set_imask(saved_imask);
}

void fw_update_usb_set_transfer_len(int len)
{
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_usb_bulk_len = len;
    fw_update_usb_wake_events.b7 = 1;
    fw_update_usb_transfer_flags.done = 0;
    fw_update_usb_transfer_flags.pending = 1;
    fw_update_usb_transfer_flags.pisces_in = 0;
    fw_update_usb_bulk_state = 0;
    _builtin_set_imask(15);
    fw_update_usb_transfer_phase = 1;
    _builtin_set_imask(saved_imask);
}

void fw_update_usb_set_status_len(void)
{
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_usb_bulk_len = 13;
    fw_update_usb_transfer_flags.done = 0;
    fw_update_usb_transfer_flags.pending = 1;
    fw_update_usb_transfer_flags.pisces_in = 0;
    fw_update_usb_bulk_state = 0;
    _builtin_set_imask(15);
    fw_update_usb_transfer_phase = 2;
    _builtin_set_imask(saved_imask);
}

void fw_update_usb_reset_state(void)
{
    int x;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_usb_main_status.v = 0;
    fw_update_usb_sie_status.v = 0;
    fw_update_usb_ep_int_status.v = 0;
    fw_update_usb_ep0_int_status.v = 0;
    fw_update_usb_epa_int_status.v = 0;
    fw_update_usb_epb_int_status.v = 0;
    _builtin_set_imask(saved_imask);
    fw_update_usb_unused_state_2 = 0;
    *(unsigned char *)&fw_update_usb_wake_events = 0;
    fw_update_usb_ep0_stage = 7;
    fw_update_usb_bulk_state = 2;
    fw_update_usb_bulk_out_state = 2;
    _builtin_set_imask(15);
    fw_update_usb_transfer_phase = 0;
    _builtin_set_imask(saved_imask);
    fw_update_usb_clear_transfer();
}

void fw_update_usb_clear_transfer(void)
{
    unsigned int i;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_usb_device_state = USB_STATE_DEFAULT;
    fw_update_usb_address = 0;
    *(unsigned char *)&fw_update_usb_ctrl_flags = 0;
    *(unsigned char *)&fw_update_usb_event_flags = 0;
    fw_update_usb_bulk_wait_count = 0;
    fw_update_usb_alternate_setting = 0;
    fw_update_usb_ep0_data_ptr = 0;
    fw_update_usb_ep0_wlength = fw_update_usb_ep0_done_len = fw_update_usb_ep0_data_len = 0;
    fw_update_usb_ep0_max_packet = fw_update_usb_epa_max_packet = fw_update_usb_epb_max_packet = 0;
    fw_update_usb_ep0_event = fw_update_usb_epa_event = 0;
    fw_update_usb_epb_event = 0;
    for (i = 0; i < 0x100; i++)
        fw_update_usb_ep0_reply_buf[i] = 0;
    fw_update_usb_transfer_flags.done = 0;
    fw_update_usb_transfer_flags.pending = 0;
    fw_update_usb_transfer_flags.pisces_in = 0;
    fw_update_usb_bulk_len = 0;
}

void fw_update_usb_init(void)
{
    struct fw_update_usb_interface_link *interface;

    fw_update_usb_main_status.v = 0;
    fw_update_usb_sie_status.v = 0;
    fw_update_usb_ep_int_status.v = 0;
    fw_update_usb_ep0_int_status.v = 0;
    fw_update_usb_epa_int_status.v = 0;
    fw_update_usb_epb_int_status.v = 0;
    fw_update_usb_unused_state_2 = 0;
    *(unsigned char *)&fw_update_usb_wake_events = 0;
    fw_update_usb_ep0_stage = 7;
    fw_update_usb_bulk_state = 2;
    fw_update_usb_bulk_out_state = 2;
    fw_update_usb_transfer_phase = 0;
    fw_update_usb_device_state = USB_STATE_DEFAULT;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_usb_address = ((struct usb_regs_device *)fw_update_p_usb_ctrl)->address & 0x7f;
    *(unsigned char *)&fw_update_usb_ctrl_flags = 0;
    *(unsigned char *)&fw_update_usb_event_flags = 0;
    fw_update_usb_bulk_wait_count = 0;
    fw_update_usb_alternate_setting = 0;
    fw_update_usb_ep0_data_ptr = 0;
    fw_update_usb_ep0_wlength = fw_update_usb_ep0_done_len = fw_update_usb_ep0_data_len = 0;
    fw_update_sync_dst = fw_update_sync_src;
    if (((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_status.full_speed)
        interface = &fw_update_usb_interface_fs;
    else
        interface = &fw_update_usb_interface_hs;
    fw_update_usb_epa_max_packet = ((&interface->e->wMaxPacketSize_lo)[1] << 8) + interface->e->wMaxPacketSize_lo;
    fw_update_usb_epb_max_packet = ((&interface->e[1].wMaxPacketSize_lo)[1] << 8) + interface->e[1].wMaxPacketSize_lo;
    fw_update_usb_epa_address[0] = interface->e->bEndpointAddress;
    fw_update_usb_epb_address[0] = interface->e[1].bEndpointAddress;
    fw_update_usb_ep0_event = fw_update_usb_epa_event = 0;
    fw_update_usb_epb_event = 0;
    fw_update_usb_transfer_flags.done = 0;
    fw_update_usb_transfer_flags.pending = 0;
    fw_update_usb_transfer_flags.pisces_in = 0;
    fw_update_usb_bulk_len = 0;
}

void fw_update_usb_handle_attach(void)
{
    unsigned char vbus, line_state;
    int go;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    go = 1;
    do {
        vbus = fw_update_usb_read_vbus();
        if (vbus == 1) {
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.active_usb = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.dis_bus_detect = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->xcvr_control.term_select = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->xcvr_control.xcvr_select = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->xcvr_control.op_mode = 0;
            while (1) {
                line_state = fw_update_usb_read_line_state();
                if (line_state == 1) {
                    if (!fw_update_hostif_flags.usb && !fw_update_hostif_flags.scsi)
                        fw_update_hostif_flags.usb = 1;
                    go = 0;
                    break;
                }
                if (fw_update_usb_sie_status.f.vbus_changed == 1) {
                    _builtin_set_imask(15);
                    fw_update_usb_sie_status.f.vbus_changed = 0;
                    _builtin_set_imask(saved_imask);
                    fw_update_usb_detach();
                    if (fw_update_hostif_flags.scsi == 1) {
                        go = 0;
                        fw_update_usb_event_flags.b7 = 0;
                        return;
                    }
                    break;
                }
            }
        } else {
            fw_update_usb_detach();
            if (fw_update_hostif_flags.scsi == 1) {
                go = 0;
                fw_update_usb_event_flags.b7 = 0;
                return;
            }
        }
    } while (go == 1);
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_enb.b5 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_enb.b4 = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.dis_bus_detect = 0;
    fw_update_usb_event_flags.b7 = 0;
    _builtin_set_imask(saved_imask);
}

void fw_update_usb_detach(void)
{
    unsigned char vbus;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->xcvr_control.op_mode = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->xcvr_control.term_select = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->xcvr_control.xcvr_select = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.active_usb = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.restore_usb = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.go_chirp = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_test = 0;
    while (1) {
        vbus = fw_update_usb_read_vbus();
        if (vbus == 1)
            break;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->pm_control.b0 = 1;
        while (fw_update_usb_sie_status.f.vbus_changed == 0 && fw_update_hostif_flags.scsi == 0)
            ;
        if (fw_update_hostif_flags.scsi == 1) {
            fw_update_usb_sie_status.f.vbus_changed = 0;
            fw_update_usb_event_flags.b6 = 0;
            return;
        }
        _builtin_set_imask(15);
        fw_update_usb_sie_status.f.vbus_changed = 0;
        _builtin_set_imask(saved_imask);
    }
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->pm_control.b0 = 0;
    fw_update_sync_dst = fw_update_sync_src;
    while (1) {
        fw_update_sync_dst = fw_update_sync_src;
        if (!((struct usb_regs_device *)fw_update_p_usb_ctrl)->pm_control.b7)
            break;
    }
    fw_update_usb_event_flags.b6 = 0;
    fw_update_usb_event_flags.b7 = 1;
}

void fw_update_usb_handle_bus_reset(void)
{
    unsigned char line_state;
    int x;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_usb_event_flags.b5 = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.dis_bus_detect = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.go_chirp = 1;
    if (!fw_update_usb_ctrl_flags.b6) {
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->address = 0;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->ep0_control_out.b7 = 1;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_bank = 2;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->epa_config_0.b5 = 0;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->epb_config_0.b5 = 0;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->epc_config_0.b5 = 0;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_bank = 0;
        fw_update_usb_ep0_stage = 7;
        fw_update_usb_bulk_state = 2;
        fw_update_usb_bulk_out_state = 2;
        _builtin_set_imask(15);
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->main_int_stat.c = 1;
        fw_update_usb_main_status.v = 0;
        fw_update_usb_sie_status.v = 0;
        fw_update_usb_ep_int_status.v = 0;
        fw_update_usb_ep0_int_status.v = 0;
        fw_update_usb_epa_int_status.v = 0;
        fw_update_usb_epb_int_status.v = 0;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->ep0_int_enb = 51;
        _builtin_set_imask(saved_imask);
        fw_update_usb_clear_transfer();
        _builtin_set_imask(15);
        fw_update_usb_transfer_phase = 0;
        _builtin_set_imask(saved_imask);
        fw_update_usb_ctrl_flags.b6 = 1;
    }
    if (fw_update_record_active.b7) {
        if (fw_update_current_record.p != fw_update_disconnected_record.p)
            fw_update_release_record(fw_update_current_record.p);
        else
            fw_update_clear_current_record(fw_update_current_record.p);
    }
    _builtin_set_imask(15);
    fw_update_tick_count = 0;
    _builtin_set_imask(saved_imask);
    while (1) {
        if (fw_update_usb_sie_status.f.chirp_done) {
            _builtin_set_imask(15);
            fw_update_usb_sie_status.f.chirp_done = 0;
            _builtin_set_imask(saved_imask);
            fw_update_sync_dst = fw_update_sync_src;
            if (((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_status.full_speed == 1) {
                _builtin_set_imask(15);
                fw_update_tick_count = 0;
                _builtin_set_imask(saved_imask);
                while (1) {
                    line_state = fw_update_usb_read_line_state();
                    if (line_state != 0)
                        break;
                    if (fw_update_usb_sie_status.f.vbus_changed == 1) {
                        _builtin_set_imask(15);
                        fw_update_usb_sie_status.f.vbus_changed = 0;
                        _builtin_set_imask(saved_imask);
                        fw_update_usb_detach();
                        return;
                    }
                    if (fw_update_tick_count > 5000)
                        break;
                }
            }
            _builtin_set_imask(15);
            fw_update_usb_ep0_max_packet = fw_update_usb_desc_max_packet0.v;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_enb.b5 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_enb.b4 = 1;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.dis_bus_detect = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_bank = 2;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->ep0_max_size = fw_update_usb_max_packet_ep0_low;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_bank = 0;
            fw_update_sync_dst = fw_update_sync_src;
            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->reg_17.b5 = 1;
            _builtin_set_imask(saved_imask);
            break;
        } else {
            if (fw_update_usb_sie_status.f.vbus_changed) {
                _builtin_set_imask(15);
                fw_update_usb_sie_status.f.vbus_changed = 0;
                _builtin_set_imask(saved_imask);
                fw_update_usb_detach();
                break;
            }
            if (fw_update_tick_count > 30000) {
                _builtin_set_imask(15);
                fw_update_usb_sie_status.f.chirp_done = 0;
                fw_update_usb_ep0_max_packet = fw_update_usb_desc_max_packet0.v;
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_enb.b5 = 1;
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_enb.b4 = 1;
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.dis_bus_detect = 0;
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_bank = 2;
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_device *)fw_update_p_usb_ctrl)->ep0_max_size = fw_update_usb_max_packet_ep0_low;
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_bank = 0;
                fw_update_sync_dst = fw_update_sync_src;
                ((struct usb_regs_device *)fw_update_p_usb_ctrl)->reg_17.b5 = 1;
                _builtin_set_imask(saved_imask);
                break;
            }
        }
    }
}

void fw_update_usb_handle_suspend(void)
{
    unsigned char vbus, line_state;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_usb_event_flags.b4 = 0;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.in_suspend = 1;
    fw_update_sync_dst = fw_update_sync_src;
    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_stat = 64;
    _builtin_set_imask(saved_imask);
    while (1) {
        _builtin_set_imask(15);
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_enb.b6 = 1;
        _builtin_set_imask(saved_imask);
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.dis_bus_detect = 1;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->pm_control.b0 = 1;
        while (1) {
            if (fw_update_usb_sie_status.f.vbus_changed) {
                vbus = fw_update_usb_read_vbus();
                _builtin_set_imask(15);
                fw_update_usb_sie_status.f.vbus_changed = 0;
                _builtin_set_imask(saved_imask);
                if (vbus == 0) {
                    _builtin_set_imask(15);
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->sie_int_enb.b6 = 0;
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.in_suspend = 0;
                    _builtin_set_imask(saved_imask);
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->pm_control.b0 = 0;
                    while (1) {
                        fw_update_sync_dst = fw_update_sync_src;
                        if (!((struct usb_regs_device *)fw_update_p_usb_ctrl)->pm_control.b7)
                            break;
                    }
                    fw_update_usb_detach();
                    return;
                }
                break;
            } else {
                if (fw_update_usb_sie_status.f.non_j) {
                    line_state = fw_update_usb_read_line_state();
                    _builtin_set_imask(15);
                    fw_update_usb_sie_status.f.non_j = 0;
                    _builtin_set_imask(saved_imask);
                    if (line_state != 1 && line_state != 3) {
                        fw_update_sync_dst = fw_update_sync_src;
                        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->pm_control.b0 = 0;
                        while (1) {
                            fw_update_sync_dst = fw_update_sync_src;
                            if (!((struct usb_regs_device *)fw_update_p_usb_ctrl)->pm_control.b7)
                                break;
                        }
                        _builtin_set_imask(15);
                        fw_update_sync_dst = fw_update_sync_src;
                        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.in_suspend = 0;
                        _builtin_set_imask(saved_imask);
                        if (line_state == 0) {
                            fw_update_usb_handle_bus_reset();
                            return;
                        }
                        if (line_state == 2) {
                            fw_update_sync_dst = fw_update_sync_src;
                            ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.restore_usb = 1;
                            while (1) {
                                if (fw_update_usb_sie_status.f.restore_done) {
                                    fw_update_sync_dst = fw_update_sync_src;
                                    ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.dis_bus_detect = 0;
                                    _builtin_set_imask(15);
                                    fw_update_usb_sie_status.f.restore_done = 0;
                                    _builtin_set_imask(saved_imask);
                                    return;
                                }
                                if (fw_update_usb_sie_status.f.vbus_changed) {
                                    vbus = fw_update_usb_read_vbus();
                                    _builtin_set_imask(15);
                                    fw_update_usb_sie_status.f.vbus_changed = 0;
                                    _builtin_set_imask(saved_imask);
                                    if (vbus == 0) {
                                        fw_update_sync_dst = fw_update_sync_src;
                                        ((struct usb_regs_device *)fw_update_p_usb_ctrl)->usb_control.dis_bus_detect = 0;
                                        fw_update_usb_detach();
                                        return;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}
