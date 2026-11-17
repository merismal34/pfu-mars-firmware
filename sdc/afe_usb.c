#include "fw.h"

extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

/* Constants (section C). */
const unsigned char usb_test_packet[54] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfc, 0x7e, 0xbf, 0xdf,
    0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0
};
/* The USB device descriptor. */
const struct usb_device_desc usb_device_descriptor = {0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0xc5, 0x04, 0x74, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01};
const struct usb_qualifier_desc usb_device_qualifier = {0x0a, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00};
const unsigned char usb_serial_string[18] = {
    0x12, 3, 0x32, 0, 0x30, 0, 0x30, 0, 0x30, 0, 0x30, 0, 0x30, 0, 0x30, 0,
    0x30, 0
};
const unsigned char usb_configuration_fs[7] = {0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00};
const struct usb_config_attributes_view usb_config_attributes_fs = {0xc0, 0x31, 0x00};
const unsigned char usb_endpoints_fs[18] = {
    0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0xff, 0x00, 0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0xff, 0x00,
    0x00, 0x00
};
const struct usb_interface_descriptor usb_interface_fs[1] = {{9, 4, 0, 0, 2, {-1, -1, -1, 0, 0, 0, 0}, (void *)usb_endpoints_fs}};
const unsigned char usb_configuration_hs[7] = {0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00};
const struct usb_config_attributes_view usb_config_attributes_hs = {0xc0, 0x31, 0x00};
const unsigned char usb_endpoints_hs[18] = {
    0x07, 0x05, 0x81, 0x02, 0x00, 0x02, 0xff, 0x00, 0x07, 0x05, 0x02, 0x02, 0x00, 0x02, 0xff, 0x00,
    0x00, 0x00
};
const struct usb_interface_descriptor usb_interface_hs[1] = {{9, 4, 0, 0, 2, {-1, -1, -1, 0, 0, 0, 0}, (void *)usb_endpoints_hs}};
const char usb_status_template[16] = {0x53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char (*const usb_bulk_state_table[2])(void) = {usb_bulk_state_0, usb_bulk_state_1};
int (*const usb_std_request_table[13])() = {
    usb_req_get_status, usb_req_clear_feature, usb_req_unsupported, usb_req_set_feature,
    usb_req_unsupported, usb_req_set_address, usb_req_get_descriptor, usb_req_unsupported,
    usb_req_get_configuration, usb_req_set_configuration, usb_req_get_interface, usb_req_set_interface,
    usb_req_unsupported
};
void (*const usb_ep0_stage_table[7])(void) = {
    usb_ep0_setup, usb_ep0_data_in, usb_ep0_wait_in, usb_ep0_wait_status,
    usb_ep0_data_out, usb_ep0_stall, usb_ep0_status
};

/* Initialised data (section D). */
const unsigned char *usb_serial_string_ptr[1] = {usb_serial_string};
unsigned char (*usb_bulk_send_state_table[2])(void) = {usb_bulk_send_data, usb_bulk_state_1};
unsigned char (*usb_bulk_out_state_table[2])(void) = {usb_bulk_receive_command, usb_bulk_out_wait};
/* Uninitialised data (section B). */
unsigned short usb_transfer_busy;
unsigned short usb_sie_pending;

void afe_select_source(void)
{
    int i;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (!win_params.flags.f.adf || (settings_byte_0x9a[0] & 1) != 0) {
        if (p_vdcc_front_sensor->source_gate.b.flatbed_ccd_off == 0 && p_vdcc_front_sensor->source_gate.b.adf_ccd_off == 1 && hw_port_e->DRL.BIT.B10 == 0
            && hw_port_e->DRL.BIT.B7 == 1 && hw_port_e->DRL.BIT.B2 == 0 && hw_port_a->DR.BIT.B0 == 1) {
            _builtin_set_imask(saved_imask);
            return;
        }
        p_vdcc_front_sensor->source_gate.b.flatbed_ccd_off = 1;
        p_vdcc_front_sensor->source_gate.b.adf_ccd_off = 1;
        hw_port_e->DRL.BIT.B7 = 0;
        hw_port_e->DRL.BIT.B10 = 0;
        p_vdcc_front_sensor->source_gate.b.flatbed_ccd_off = 0;
        _builtin_set_imask(saved_imask);
        for (i = 0; i < 4; i++)
            rt_delay_60k();
        hw_port_a->DR.BIT.B0 = 0;
        hw_port_e->DRL.BIT.B2 = 0;
        hw_port_a->DR.BIT.B0 = 1;
        hw_port_e->DRL.BIT.B7 = 1;
    } else {
        if (p_vdcc_front_sensor->source_gate.b.flatbed_ccd_off == 1 && p_vdcc_front_sensor->source_gate.b.adf_ccd_off == 0 && hw_port_e->DRL.BIT.B10 == 1
            && hw_port_e->DRL.BIT.B7 == 0 && hw_port_e->DRL.BIT.B2 == 1 && hw_port_a->DR.BIT.B0 == 0) {
            _builtin_set_imask(saved_imask);
            return;
        }
        p_vdcc_front_sensor->source_gate.b.flatbed_ccd_off = 1;
        p_vdcc_front_sensor->source_gate.b.adf_ccd_off = 1;
        hw_port_e->DRL.BIT.B7 = 0;
        hw_port_e->DRL.BIT.B10 = 0;
        p_vdcc_front_sensor->source_gate.b.adf_ccd_off = 0;
        _builtin_set_imask(saved_imask);
        for (i = 0; i < 4; i++)
            rt_delay_60k();
        hw_port_e->DRL.BIT.B2 = 0;
        hw_port_a->DR.BIT.B0 = 0;
        hw_port_e->DRL.BIT.B2 = 1;
        hw_port_e->DRL.BIT.B10 = 1;
    }
    if (hw_restart_flags.f.leaving_power_save) {
    } else {
        for (i = 0; i < 200; i++)
            rt_delay_60k();
    }
    _builtin_set_imask(15);
    job_adf_sheet_count = 0;
    _builtin_set_imask(saved_imask);
}

void afe_write_config(int side)
{
    int u;

    if (side == 0) {
        if (!win_params.flags.f.adf || (settings_byte_0x9a[0] & 1) != 0) {
            p_vdcc_afe[side]->ctrl.f.adf_select = 0;
        } else {
            p_vdcc_afe[side]->ctrl.f.adf_select = 1;
        }
    } else {
        p_vdcc_afe[side]->ctrl.f.adf_select = 0;
    }
    p_vdcc_afe[side]->data_high = AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_CONFIG);
    p_vdcc_afe[side]->data_low = 0;
    afe_wait_write(side);
    p_vdcc_afe[side]->data_high = AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_CONFIG);
    p_vdcc_afe[side]->data_low = 0;
    afe_wait_write(side);
    p_vdcc_afe[side]->data_high = AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_CONFIG);
    p_vdcc_afe[side]->data_low = 0;
    afe_wait_write(side);
}

void afe_set_test_mode(int side)
{
    char unused_frame;
    char test_mode;

    if (side == 0) {
        if (!win_params.flags.f.adf || (settings_byte_0x9a[0] & 1) != 0) {
            p_vdcc_afe[side]->ctrl.f.adf_select = 0;
        } else {
            p_vdcc_afe[side]->ctrl.f.adf_select = 1;
        }
    } else {
        p_vdcc_afe[side]->ctrl.f.adf_select = 0;
    }
    if (settings_test_flags[0] & 0x10) {
        test_mode = 1;
    } else {
        test_mode = 0;
    }
    p_vdcc_afe[side]->data_high = AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_TEST_MODE);
    p_vdcc_afe[side]->data_low = test_mode;
    afe_wait_write(side);
    p_vdcc_afe[side]->data_high = AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_TEST_MODE);
    p_vdcc_afe[side]->data_low = test_mode;
    afe_wait_write(side);
    p_vdcc_afe[side]->data_high = AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_TEST_MODE);
    p_vdcc_afe[side]->data_low = test_mode;
    afe_wait_write(side);
}

/* One 16-bit word to the side's analog front end through the VDCC3 serial port (+4 high, +5 low):
   VSP5000 format, chip address in bits 15-14 (chip 0, 1 or 2 selects chip 0, 1 or 3: 0x08/0x48/0xC8 in the
   high byte), register in bits 13-10 (02h DPGA gain even; odd adds 0x04 for 03h, the odd channel). */
void afe_set_gain(int side, int chip, int odd, unsigned short gain)
{
    unsigned char hi;

    if (side == 0) {
        if (!win_params.flags.f.adf || (settings_byte_0x9a[0] & 1) != 0) {
            p_vdcc_afe[side]->ctrl.f.adf_select = 0;
        } else {
            p_vdcc_afe[side]->ctrl.f.adf_select = 1;
        }
    } else {
        p_vdcc_afe[side]->ctrl.f.adf_select = 0;
    }
    hi = gain >> 8;
    if (chip == 0) {
        hi |= AFE_WORD_HIGH(AFE_CHIP_0, AFE_REG_GAIN_EVEN);
    } else if (chip == 1) {
        hi |= AFE_WORD_HIGH(AFE_CHIP_1, AFE_REG_GAIN_EVEN);
    } else {
        hi |= AFE_WORD_HIGH(AFE_CHIP_3, AFE_REG_GAIN_EVEN);
    }
    if (odd != 0) {
        hi |= 4;
    }
    p_vdcc_afe[side]->data_high = hi;
    p_vdcc_afe[side]->data_low = (unsigned char)gain;
    afe_wait_write(side);
}

int afe_wait_write(int side)
{
    int i;

    p_vdcc_afe[side]->ctrl.f.busy = 1;
    i = 0;
    while (i <= 1000) {
        if (p_vdcc_afe[side]->ctrl.f.busy == 0) {
            break;
        }
        if (i >= 100) {
            rt_delay_240();
        }
        i++;
    }
}

extern void _builtin_set_gbr(void *);
union usb_reg_43_view { unsigned char v; struct usb_reg_43_bits f; };
/* S1R72013 register +41 (USB status): bit 7 VBUS, bit 6 FSxHS, bits 1-0 LineState */
struct usb_reg_41_bits {
    unsigned char vbus:1,
                  full_speed:1,
                  p:4,
                  line_state:2;
};
struct usb_regs_afe_usb {
    char pad_00;
    unsigned char main_int_stat;
    char pad_02;
    union { unsigned char v; struct Bits f; } sie_int_stat;
    char pad_04;
    unsigned char reg_02;
    char pad_06;
    unsigned char ep0_int_stat;
    char pad_08;
    unsigned char epa_int_stat;
    char pad_0a;
    unsigned char epb_int_stat;
    char reg_06_07[5];
    unsigned char main_int_enb;
    char pad_12;
    union UBF sie_int_enb;
    char pad_14;
    unsigned char reg_0a;
    char pad_16;
    unsigned char ep0_int_enb;
    char pad_18;
    unsigned char epa_int_enb;
    char pad_1a;
    union { unsigned char v; struct Bits f; } epb_int_enb;
    char reg_0e_11[9];
    struct Bits pm_control;
    char pad_26;
    struct usb_control_bits usb_control;
    char pad_28;
    struct usb_reg_41_bits usb_status;
    char pad_2a;
    union usb_reg_43_view xcvr_control;
    char pad_2c;
    unsigned char usb_test;
    char pad_2e;
    struct Bits reg_17;
    char reg_18_1e[15];
    unsigned char usb_bank;
    char reg_20_21[5];
    struct Bits epa_config_0;
    char reg_23_25[7];
    struct Bits epb_config_0;
    char reg_27[3];
    unsigned char address;
    char reg_29[3];
    struct Bits epc_config_0;
    char pad_56;
    struct Bits ep0_control_out;
    char reg_2c[3];
    struct Bits epb_control;
    char reg_2e[3];
    unsigned char ep0_max_size;
    char reg_30_37[17];
    struct Bits reg_38;
};


void usb_read_irq_status(void)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    usb_main_int_read.v = ((struct usb_regs_afe_usb *)usb_ctrl)->main_int_stat;
    if (usb_main_int_read.f.b7) {
        _builtin_set_gbr(0);
        usb_sie_int_read.v = ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_stat.v;
        usb_sie_pending |= 0x80;
        if (usb_sie_int_read.f.b7) {
            usb_event_flags.f.detach = 1;
        }
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_stat.v = usb_sie_int_read.v;
    }
    if (usb_main_int_read.f.b6) {
        _builtin_set_gbr(0);
        usb_ep_int_read.v = ((struct usb_regs_afe_usb *)usb_ctrl)->reg_02;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->reg_02 = usb_ep_int_read.v;
        if (usb_ep_int_read.f.b0) {
            _builtin_set_gbr(0);
            usb_ep0_int_read.v = ((struct usb_regs_afe_usb *)usb_ctrl)->ep0_int_stat;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->ep0_int_stat = usb_ep0_int_read.v;
        }
        if (usb_ep_int_read.f.b1) {
            _builtin_set_gbr(0);
            usb_epa_int_read.v = ((struct usb_regs_afe_usb *)usb_ctrl)->epa_int_stat;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->epa_int_stat = usb_epa_int_read.v;
        }
        if (usb_ep_int_read.f.b2) {
            _builtin_set_gbr(0);
            usb_epb_int_read.v = ((struct usb_regs_afe_usb *)usb_ctrl)->epb_int_stat;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->epb_int_stat = usb_epb_int_read.v;
        }
    }
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->main_int_stat = usb_main_int_read.v & 0xfe;
    _builtin_set_imask(saved_imask);
    usb_main_status.v |= usb_main_int_read.v;
    if (usb_main_int_read.f.b7) {
        _builtin_set_imask(15);
        if (usb_sie_int_read.f.b6) {
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b6 = 0;
        }
        if (usb_sie_int_read.f.b5) {
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 1;
        }
        usb_sie_status.v |= usb_sie_int_read.v;
        _builtin_set_imask(saved_imask);
    }
    if (usb_main_int_read.f.b6) {
        usb_ep_int_status.v |= usb_ep_int_read.v;
        if (usb_ep_int_read.f.b0) {
            usb_ep0_int_status.v |= usb_ep0_int_read.v;
        }
        if (usb_ep_int_read.f.b1) {
            usb_epa_int_status.v |= usb_epa_int_read.v;
        }
        if (usb_ep_int_read.f.b2) {
            if (usb_transfer_phase == 0) {
                _builtin_set_imask(15);
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->epb_int_enb.f.b4 = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->epb_control.b1 = 1;
                _builtin_set_imask(saved_imask);
                usb_bulk_out_state = 1;
                usb_irq_code = 0x81;
                if (usb_epb_int_read.f.b0) {
                    usb_epb_event = USB_EVENT_OUT_ERROR;
                }
                if (usb_epb_int_read.f.b4) {
                    usb_epb_event = USB_EVENT_OUT_ACK;
                }
                usb_ep_int_status.f.b2 = 0;
            } else {
                usb_epb_int_status.v |= usb_epb_int_read.v;
            }
        }
    }
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    if (((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_stat.f.b5) {
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 1;
        usb_sie_status.f.bus_reset = 1;
        usb_main_status.f.b7 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_stat.v |= 0x20;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->main_int_stat |= 0x80;
    }
    _builtin_set_imask(saved_imask);
}

struct board_ctrl_reg_02_bits {
    unsigned short pad_00;
    unsigned short :6,
                   hostif_idle:1,
                   :9;
    unsigned short :6,
                   hostif_idle_enable:1,
                   :9;
};


void usb_handle_irq(void)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    if ((maint_mode_word.v & 0x5000) == 0) {
        if (hw_hostif_flags.f.usb) {
            _builtin_set_imask(15);
            if (((volatile struct board_ctrl_reg_02_bits *)p_board_ctrl)->hostif_idle == 1) {
                if (*(unsigned char *)&usb_event_flags != 0 || usb_ctrl_flags.b7 || usb_main_status.v != 0)
                    ((volatile struct board_ctrl_reg_02_bits *)p_board_ctrl)->hostif_idle = 0;
            }
            _builtin_set_imask(saved_imask);
            if (usb_irq_code == 0x81) {
                usb_bulk_receive_data();
                usb_sie_pending &= 0xff7f;
                return;
            }
            if (usb_irq_code == 0x23) {
                scsi_execute_command(saved_imask);
                _builtin_set_imask(15);
                if (usb_irq_code != 0x81)
                    usb_irq_code = 0;
                _builtin_set_imask(saved_imask);
                return;
            }
            usb_run_events();
            if (usb_ctrl_flags.b7)
                usb_ep0_run();
            usb_irq_device_events();
            if (usb_suspended)
                usb_stop_timers();
            _builtin_set_imask(15);
            if (((volatile struct board_ctrl_reg_02_bits *)p_board_ctrl)->hostif_idle_enable == 1) {
                if (*(unsigned char *)&usb_event_flags == 0 && usb_ctrl_flags.b7 == 0 && usb_main_status.v == 0 && usb_transfer_busy == 0 && usb_sie_pending == 0)
                    ((volatile struct board_ctrl_reg_02_bits *)p_board_ctrl)->hostif_idle = 1;
                if (hw_hostif_flags.f.selected == 0)
                    usb_event_flags.f.detach = 0;
            }
            _builtin_set_imask(saved_imask);
        } else {
            if (usb_restart_pending != 0) {
                if (usb_event_flags.f.attach) {
                    usb_restart_pending = 0;
                    usb_handle_attach();
                }
            }
        }
    }
}

void usb_run_events(void)
{
    if (*(unsigned char *)&usb_event_flags) {
        if (usb_event_flags.f.detach)
            usb_handle_detach();
        if (usb_event_flags.f.bus_reset)
            usb_handle_bus_reset();
        if (usb_event_flags.f.suspend)
            usb_handle_suspend();
        if (usb_event_flags.f.attach)
            usb_handle_attach();
    }
}

void usb_irq_device_events(void)
{
    unsigned char vbus;
    int saved_imask;
    int u;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (usb_main_status.f.b7) {
        usb_wake_events.f.vbus_changed = 0;
        usb_wake_events.f.non_j = 0;
        if (usb_sie_status.f.vbus_changed) {
            usb_wake_events.f.vbus_changed = 1;
            vbus = usb_read_vbus();
            if (vbus == 0) {
                usb_event_flags.f.detach = 1;
                usb_main_status.v = 0;
                usb_disable_device();
            }
        } else if (usb_sie_status.f.non_j) {
            usb_wake_events.f.non_j = 1;
        } else if (usb_sie_status.f.bus_reset) {
            usb_event_flags.f.bus_reset = 1;
            usb_main_status.v = 0;
        } else if (usb_sie_status.f.suspend) {
            usb_event_flags.f.suspend = 1;
            usb_main_status.v = 0;
        }
        usb_sie_status.v = 0;
    }
    if (usb_main_status.f.b2) {
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->reg_38.b6 = 0;
        usb_epa_event = USB_EVENT_DMA_DONE;
    }
    if (usb_main_status.f.b6)
        usb_update_ep_status();
    if (usb_bulk_sending) {
        usb_main_status.v = 0;
        _builtin_set_imask(saved_imask);
        return;
    }
    if (!((volatile struct board_ctrl_reg_02_bits *)p_board_ctrl)->hostif_idle) {
        _builtin_set_gbr(0);
        if (((volatile struct Bits *)&((struct usb_regs_afe_usb *)usb_ctrl)->main_int_stat)->b0) {
            if (usb_ep0_stage == 7) {
                if (!usb_ctrl_flags.b7) {
                    usb_ctrl_flags.b7 = 1;
                    usb_ep0_stage = 0;
                }
            }
        }
    }
    usb_main_status.v = 0;
    _builtin_set_imask(saved_imask);
}

void usb_poll_device_events(void)
{
    unsigned char vbus;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (usb_main_status.f.b7) {
        if (usb_sie_status.f.vbus_changed) {
            vbus = usb_read_vbus();
            if (vbus == 0) {
                usb_main_status.v = 0;
                usb_disable_device();
            }
        } else if (usb_sie_status.f.bus_reset) {
            usb_event_flags.f.bus_reset = 1;
        } else if (usb_sie_status.f.suspend) {
            usb_event_flags.f.suspend = 1;
            usb_main_status.v = 0;
        }
        usb_sie_status.v = 0;
    }
    if (usb_main_status.f.b2) {
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->reg_38.b6 = 0;
        usb_epa_event = USB_EVENT_DMA_DONE;
    }
    if (usb_main_status.f.b6)
        usb_update_ep_status();
    usb_main_status.v = 0;
    _builtin_set_imask(saved_imask);
}

void usb_update_ep_status(void)
{
    _builtin_set_gbr(0);
    if (usb_ep_int_status.f.b0) {
        _builtin_set_gbr(0);
        usb_ep0_int_status.v = usb_ep0_int_status.v & (*(struct usb_regs_afe_usb **)&usb_ctrl)->ep0_int_enb;
        if (usb_ep0_int_status.f.b1)
            usb_ep0_event = USB_EVENT_IN_ERROR;
        if (usb_ep0_int_status.f.b0)
            usb_ep0_event = USB_EVENT_OUT_ERROR;
        if (usb_ep0_int_status.f.b5)
            usb_ep0_event = USB_EVENT_IN_ACK;
        if (usb_ep0_int_status.f.b4)
            usb_ep0_event = USB_EVENT_OUT_ACK;
        usb_ep0_int_status.v = 0;
    }
    if (usb_ep_int_status.f.b1) {
        if (usb_epa_event != USB_EVENT_DMA_DONE) {
            _builtin_set_gbr(0);
            usb_epa_int_status.v = usb_epa_int_status.v & (*(struct usb_regs_afe_usb **)&usb_ctrl)->epa_int_enb;
            if (usb_epa_int_status.f.b1)
                usb_epa_event = USB_EVENT_IN_ERROR;
            if (usb_epa_int_status.f.b0)
                usb_epa_event = USB_EVENT_OUT_ERROR;
            if (usb_epa_int_status.f.b5)
                usb_epa_event = USB_EVENT_IN_ACK;
            if (usb_epa_int_status.f.b4)
                usb_epa_event = USB_EVENT_OUT_ACK;
        }
        usb_epa_int_status.v = 0;
    }
    if (usb_ep_int_status.f.b2) {
        if (usb_epb_event != 8) {
            _builtin_set_gbr(0);
            usb_epb_int_status.v = usb_epb_int_status.v & (*(struct usb_regs_afe_usb **)&usb_ctrl)->epb_int_enb.v;
            if (usb_epb_int_status.f.b1)
                usb_epb_event = USB_EVENT_IN_ERROR;
            if (usb_epb_int_status.f.b0)
                usb_epb_event = USB_EVENT_OUT_ERROR;
            if (usb_epb_int_status.f.b5)
                usb_epb_event = USB_EVENT_IN_ACK;
            if (usb_epb_int_status.f.b4)
                usb_epb_event = USB_EVENT_OUT_ACK;
        }
        usb_epb_int_status.v = 0;
    }
    usb_ep_int_status.v = 0;
}

void usb_run_events_6_4(void)
{
    if (*(unsigned char *)&usb_event_flags) {
        if (usb_event_flags.f.detach)
            usb_handle_detach();
        if (usb_event_flags.f.suspend)
            usb_handle_suspend();
    }
}

void usb_handle_attach(void)
{
    unsigned char vbus;
    unsigned char line_state;
    int u;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    vbus = usb_read_vbus();
    if (vbus == 1) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.active_usb = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->xcvr_control.f.term_select = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->xcvr_control.f.xcvr_select = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->xcvr_control.f.op_mode = 2;
        _builtin_set_imask(saved_imask);
        while (1) {
            line_state = usb_read_line_state();
            if (line_state == 1) {
                if (!hw_hostif_flags.f.usb)
                    hw_hostif_flags.f.usb = 1;
                _builtin_set_imask(15);
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b5 = 1;
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b4 = 1;
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 0;
                _builtin_set_imask(saved_imask);
                usb_event_flags.f.attach = 0;
                usb_event_flags.f.detach = 0;
                usb_event_flags.f.suspend = 0;
                break;
            } else if (usb_sie_status.f.vbus_changed == 1) {
                _builtin_set_imask(15);
                usb_sie_status.f.vbus_changed = 0;
                _builtin_set_imask(saved_imask);
                usb_event_flags.f.attach = 0;
                usb_disable_device();
                break;
            }
        }
    } else {
        usb_event_flags.f.attach = 0;
        usb_disable_device();
    }
}

int usb_read_vbus(void)
{
    int i;
    int vbus;
    _builtin_set_gbr(0);
    vbus = ((struct usb_regs_afe_usb *)usb_ctrl)->usb_status.vbus;
    for (i = 0; i < 5; i++) {
        _builtin_set_gbr(0);
        if (((struct usb_regs_afe_usb *)usb_ctrl)->usb_status.vbus != vbus) {
            _builtin_set_gbr(0);
            vbus = ((struct usb_regs_afe_usb *)usb_ctrl)->usb_status.vbus;
            i = 0;
        }
    }
    return vbus;
}

int usb_read_line_state(void)
{
    int i;
    int line_state;
    _builtin_set_gbr(0);
    line_state = ((struct usb_regs_afe_usb *)usb_ctrl)->usb_status.line_state;
    for (i = 0; i < 5; i++) {
        if (usb_sie_status.f.vbus_changed == 1)
            break;
        _builtin_set_gbr(0);
        if (((struct usb_regs_afe_usb *)usb_ctrl)->usb_status.line_state != line_state) {
            _builtin_set_gbr(0);
            line_state = ((struct usb_regs_afe_usb *)usb_ctrl)->usb_status.line_state;
            i = 0;
        }
    }
    return line_state;
}

void usb_handle_detach(void)
{
    unsigned char vbus;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    if (usb_sie_status.f.vbus_changed == 1) {
        _builtin_set_imask(15);
        usb_sie_status.f.vbus_changed = 0;
        _builtin_set_imask(saved_imask);
    }
    vbus = usb_read_vbus();
    if (vbus == 1) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b0 = 0;
        _builtin_set_imask(saved_imask);
        _builtin_set_gbr(0);
        while (1) {
            _builtin_set_gbr(0);
            if (!((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b7)
                break;
        }
        usb_event_flags.f.detach = 0;
        usb_event_flags.f.attach = 1;
    } else {
        if (hw_hostif_flags.f.selected) {
            if (hw_hostif_flags.f.usb) {
                mdc_wake_board();
                if (!power_state.f.saving) {
                    jpeg_sync_buffer_pointer(0, saved_imask);
                    jpeg_sync_buffer_pointer(1, saved_imask);
                }
                _builtin_set_imask(15);
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b0 = 0;
                for (;;) {
                    _builtin_set_gbr(0);
                    if (!((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b7)
                        break;
                }
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->xcvr_control.v = 0x41;
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.active_usb = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.restore_usb = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.go_chirp = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->usb_test = 0;
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->main_int_enb = 0xc4;
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.v = 0x86;
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->reg_0a = 7;
                *(*(volatile unsigned char **)&p_mtu) = 0;
                p_cmt->CMSTR.WORD = 0;
                if (p_cmt0->CMCSR.BIT.CMF)
                    p_cmt0->CMCSR.BIT.CMF = 0;
                if (p_cmt1->CMCSR.BIT.CMF)
                    p_cmt1->CMCSR.BIT.CMF = 0;
                if (p_mtu1->TSR.BIT.TGFA)
                    p_mtu1->TSR.BIT.TGFA = 0;
                if (p_mtu2->TSR.BIT.TGFA)
                    p_mtu2->TSR.BIT.TGFA = 0;
                if (p_mtu3->TSR.BIT.TGFA)
                    p_mtu3->TSR.BIT.TGFA = 0;
                if (p_mtu4->TSR.BIT.TGFA)
                    p_mtu4->TSR.BIT.TGFA = 0;
                hw_restart_flags.f.usb_restart = 1;
                hw_hostif_flags.v = 0;
                hw_restart_flags.f.leaving_power_save = 0;
                rt_restart();
            }
        }
    }
}

void usb_handle_bus_reset(void)
{
    unsigned char line_state;
    int u;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    usb_event_flags.f.bus_reset = 0;
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 1;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->xcvr_control.f.op_mode = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.go_chirp = 1;
    _builtin_set_imask(saved_imask);
    if (!usb_ctrl_flags.b6) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->address = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->ep0_control_out.b7 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->usb_bank = 2;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->epa_config_0.b5 = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->epb_config_0.b5 = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->epc_config_0.b5 = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->usb_bank = 0;
        _builtin_set_imask(saved_imask);
        usb_ep0_stage = 7;
        usb_bulk_state = 2;
        usb_bulk_out_state = 2;
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->main_int_stat = 1;
        usb_main_status.v = 0;
        usb_sie_status.v = 0;
        usb_ep_int_status.v = 0;
        usb_ep0_int_status.v = 0;
        usb_epa_int_status.v = 0;
        usb_epb_int_status.v = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->ep0_int_enb = 0x33;
        _builtin_set_imask(saved_imask);
        usb_clear_transfer_state();
        _builtin_set_imask(15);
        usb_transfer_phase = 0;
        _builtin_set_imask(saved_imask);
        usb_ctrl_flags.b6 = 1;
    }
    _builtin_set_imask(15);
    tick_hostif_wait_ticks.v = 0;
    _builtin_set_imask(saved_imask);
    for (;;) {
        if (usb_sie_status.f.chirp_done) {
            _builtin_set_imask(15);
            usb_sie_status.f.chirp_done = 0;
            _builtin_set_imask(saved_imask);
            _builtin_set_gbr(0);
            if (((struct usb_regs_afe_usb *)usb_ctrl)->usb_status.full_speed == 1) {
                _builtin_set_imask(15);
                tick_hostif_wait_ticks.v = 0;
                _builtin_set_imask(saved_imask);
                while (1) {
                    line_state = usb_read_line_state();
                    if (line_state != 0)
                        break;
                    if (usb_sie_status.f.vbus_changed == 1) {
                        _builtin_set_imask(15);
                        usb_sie_status.f.vbus_changed = 0;
                        _builtin_set_imask(saved_imask);
                        usb_disable_device();
                        return;
                    }
                    if (tick_hostif_wait_ticks.v > 0x1388)
                        break;
                }
            }
            _builtin_set_imask(15);
            usb_ep0_max_packet = usb_device_descriptor.bMaxPacketSize0;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b5 = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b4 = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->usb_bank = 2;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->ep0_max_size = (unsigned char)usb_ep0_max_packet;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->usb_bank = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->reg_17.b5 = 1;
            _builtin_set_imask(saved_imask);
            break;
        }
        if (usb_sie_status.f.vbus_changed) {
            _builtin_set_imask(15);
            usb_sie_status.f.vbus_changed = 0;
            _builtin_set_imask(saved_imask);
            usb_disable_device();
            break;
        }
        if (tick_hostif_wait_ticks.v > 0x7530) {
            _builtin_set_imask(15);
            usb_sie_status.f.chirp_done = 0;
            usb_ep0_max_packet = usb_device_descriptor.bMaxPacketSize0;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b5 = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b4 = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->usb_bank = 2;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->ep0_max_size = (unsigned char)usb_ep0_max_packet;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->usb_bank = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->reg_17.b5 = 1;
            _builtin_set_imask(saved_imask);
            break;
        }
    }
}

void usb_handle_suspend(void)
{
    unsigned char vbus;
    unsigned char line_state;
    int saved_imask;
    int u;
    saved_imask = _builtin_get_imask();
    _builtin_set_gbr(0);
    usb_event_flags.f.suspend = 0;
    _builtin_set_imask(15);
    if (usb_suspend_entered == 0) {
        usb_suspend_entered = 1;
        ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_stat.v = 64;
    }
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.in_suspend = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b6 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b0 = 1;
    _builtin_set_imask(saved_imask);
    if (usb_wake_events.f.vbus_changed) {
        usb_wake_events.f.vbus_changed = 0;
        vbus = usb_read_vbus();
        _builtin_set_imask(15);
        usb_sie_status.f.vbus_changed = 0;
        _builtin_set_imask(saved_imask);
        if (vbus == 0) {
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b6 = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.in_suspend = 0;
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b0 = 0;
            _builtin_set_imask(saved_imask);
            usb_suspend_entered = 0;
            while (1) {
                _builtin_set_gbr(0);
                if (!((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b7)
                    break;
            }
            usb_disable_device();
            return;
        }
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b6 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b0 = 1;
        _builtin_set_imask(saved_imask);
        usb_event_flags.f.suspend = 1;
        return;
    }
    if (usb_bulk_state != 2 || usb_bulk_out_state != 2) {
        _builtin_set_imask(15);
        usb_transfer_phase = 0;
        _builtin_set_imask(saved_imask);
        ((struct usb_regs_afe_usb *)usb_ctrl)->epb_int_enb.f.b4 = 1;
        ((struct usb_regs_afe_usb *)usb_ctrl)->epb_control.b1 = 0;
        usb_ep0_stage = 7;
        usb_bulk_state = 2;
        usb_bulk_out_state = 2;
        usb_ep0_event = 0;
        usb_epa_event = 0;
        usb_epb_event = 0;
        usb_transfer_flags.done = 0;
        usb_transfer_flags.pending = 0;
        usb_transfer_flags.pisces_in = 0;
        usb_bulk_len = 0;
        usb_irq_code = 0;
    }
    if (usb_wake_events.f.non_j) {
        usb_wake_events.f.non_j = 0;
        line_state = usb_read_line_state();
        _builtin_set_imask(15);
        usb_sie_status.f.non_j = 0;
        _builtin_set_imask(saved_imask);
        if (line_state != 1 && line_state != 3) {
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b0 = 0;
            _builtin_set_imask(saved_imask);
            while (1) {
                _builtin_set_gbr(0);
                if (!((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b7)
                    break;
            }
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.in_suspend = 0;
            _builtin_set_imask(saved_imask);
            usb_suspend_entered = 0;
            if (line_state == 2 && usb_device_state != USB_STATE_CONFIGURED) {
                _builtin_set_imask(15);
                tick_hostif_wait_ticks.v = 0;
                _builtin_set_imask(saved_imask);
                while (1) {
                    line_state = usb_read_line_state();
                    if (line_state == 0)
                        break;
                    if (tick_hostif_wait_ticks.v > 100) {
                        line_state = 0;
                        break;
                    }
                }
                usb_suspended = 1;
                usb_suspend_ticks = 0;
            }
            if (line_state == 0) {
                usb_handle_bus_reset();
                return;
            }
            if (line_state == 2) {
                _builtin_set_imask(15);
                _builtin_set_gbr(0);
                ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.restore_usb = 1;
                _builtin_set_imask(saved_imask);
                while (1) {
                    if (usb_sie_status.f.restore_done) {
                        _builtin_set_imask(15);
                        _builtin_set_gbr(0);
                        ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 0;
                        usb_sie_status.f.restore_done = 0;
                        _builtin_set_imask(saved_imask);
                        return;
                    }
                    if (usb_sie_status.f.vbus_changed) {
                        vbus = usb_read_vbus();
                        _builtin_set_imask(15);
                        usb_sie_status.f.vbus_changed = 0;
                        _builtin_set_imask(saved_imask);
                        if (vbus == 0) {
                            _builtin_set_imask(15);
                            _builtin_set_gbr(0);
                            ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 0;
                            _builtin_set_imask(saved_imask);
                            usb_disable_device();
                            return;
                        }
                    }
                }
            }
        }
    }
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_enb.f.b6 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.dis_bus_detect = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->pm_control.b0 = 1;
    _builtin_set_imask(saved_imask);
    usb_event_flags.f.suspend = 1;
}

void usb_disable_device(void)
{
    usb_event_flags.f.detach = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->xcvr_control.v = 65;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.active_usb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.restore_usb = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->usb_control.go_chirp = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->usb_test = 0;
}

void usb_start_receive(unsigned int len)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    usb_bulk_len = len;
    usb_transfer_flags.done = 0;
    usb_transfer_flags.pending = 0;
    usb_transfer_flags.pisces_in = 0;
    usb_bulk_out_state = 1;
    _builtin_set_imask(15);
    usb_transfer_phase = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->epb_int_enb.f.b4 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->epb_control.b1 = 0;
    _builtin_set_imask(saved_imask);
}

void usb_set_transfer_len(unsigned int len)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    usb_bulk_len = len;
    usb_wake_events.f.transfer_armed = 1;
    usb_transfer_flags.done = 0;
    usb_transfer_flags.pending = 1;
    usb_transfer_flags.pisces_in = 0;
    usb_bulk_state = 0;
    _builtin_set_imask(15);
    usb_transfer_phase = 1;
    _builtin_set_imask(saved_imask);
}

void usb_set_status_len(void)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    usb_bulk_len = 13; /* the bulk-only status block (CSW) */
    usb_transfer_flags.done = 0;
    usb_transfer_flags.pending = 1;
    usb_transfer_flags.pisces_in = 0;
    usb_bulk_state = 0;
    _builtin_set_imask(15);
    usb_transfer_phase = 2;
    _builtin_set_imask(saved_imask);
}

void usb_reset_state(void)
{
    int u;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    usb_main_status.v = 0;
    usb_sie_status.v = 0;
    usb_ep_int_status.v = 0;
    usb_ep0_int_status.v = 0;
    usb_epa_int_status.v = 0;
    usb_epb_int_status.v = 0;
    _builtin_set_imask(saved_imask);
    usb_unused_state_2 = 0;
    *(unsigned char *)&usb_wake_events = 0;
    usb_ep0_stage = 7;
    usb_bulk_state = 2;
    usb_bulk_out_state = 2;
    _builtin_set_imask(15);
    usb_transfer_phase = 0;
    _builtin_set_imask(saved_imask);
    usb_clear_transfer_state();
}

void usb_clear_transfer_state(void)
{
    unsigned int i;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    usb_device_state = USB_STATE_DEFAULT;
    usb_address = 0;
    *(unsigned char *)&usb_ctrl_flags = 0;
    *(unsigned char *)&usb_event_flags = 0;
    usb_bulk_wait_count = 0;
    usb_alternate_setting = 0;
    usb_ep0_data_ptr = 0;
    usb_ep0_wlength = usb_ep0_done_len = usb_ep0_data_len = 0;
    usb_ep0_max_packet = usb_epa_max_packet.v = usb_epb_max_packet = 0;
    usb_ep0_event = usb_epa_event = 0;
    usb_epb_event = 0;
    for (i = 0; i < 256; i++)
        usb_ep0_reply_buf[i] = 0;
    usb_transfer_flags.done = 0;
    usb_transfer_flags.pending = 0;
    usb_transfer_flags.pisces_in = 0;
    usb_bulk_len = 0;
    usb_irq_code = 0;
}

void usb_ack_and_reset(void)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_stat.v = ((struct usb_regs_afe_usb *)usb_ctrl)->sie_int_stat.v;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->ep0_int_stat = ((struct usb_regs_afe_usb *)usb_ctrl)->ep0_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->epa_int_stat = ((struct usb_regs_afe_usb *)usb_ctrl)->epa_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->epb_int_stat = ((struct usb_regs_afe_usb *)usb_ctrl)->epb_int_stat;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->reg_02 = ((struct usb_regs_afe_usb *)usb_ctrl)->reg_02;
    _builtin_set_gbr(0);
    ((struct usb_regs_afe_usb *)usb_ctrl)->main_int_stat = ((struct usb_regs_afe_usb *)usb_ctrl)->main_int_stat;
    _builtin_set_imask(saved_imask);
    usb_reset_state();
}

int usb_start_transfer(void)
{
    unsigned char state;
    int timeout_logged;
    struct page_record *current_page;
    int saved_imask;
    current_page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    tick_hostif_wait_ticks.v = 0;
    _builtin_set_imask(saved_imask);
    timeout_logged = 0;
    _builtin_set_imask(15);
    if (((volatile struct board_ctrl_reg_02_bits *)p_board_ctrl)->hostif_idle == 1) {
        usb_transfer_busy |= 1;
        ((volatile struct board_ctrl_reg_02_bits *)p_board_ctrl)->hostif_idle = 0;
    }
    _builtin_set_imask(saved_imask);
    while (1) {
        if (usb_bulk_state >= 2) {
            return 0;
        }
        state = usb_bulk_send_state_table[usb_bulk_state]();
        if (usb_bulk_state >= 2) {
            break;
        }
        usb_irq_device_events();
        usb_run_events_6_4();
        if (usb_event_flags.f.detach) {
            usb_bulk_state = 2;
            state = 0;
            _builtin_set_imask(15);
            if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                current_page->flags.f.scan_end = 1;
            _builtin_set_imask(saved_imask);
            break;
        }
        if (tick_hostif_wait_ticks.v > 1500) {
            if (timeout_logged == 0) {
                timeout_logged = 1;
                _builtin_set_imask(15);
                if (usb_transfer_flags.pisces_in)
                    p_pisces_hostif[scsi_read_side.v]->ctrl.f.b10 = 1;
                usb_error_main_int = usb_last_main_int;
                usb_error_sie_int = usb_last_sie_int;
                usb_error_ep_int = usb_last_ep_int;
                usb_error_ep0_int = usb_last_ep0_int;
                usb_error_epa_int = usb_last_epa_int;
                usb_error_epb_int = usb_last_epb_int;
                _builtin_set_imask(saved_imask);
            }
        }
        if (tick_hostif_wait_ticks.v > 15000) {
            usb_epa_event = 0;
            _builtin_set_imask(15);
            if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                current_page->flags.f.scan_end = 1;
            _builtin_set_imask(saved_imask);
            usb_save_error_state();
            usb_error_code = 1;
            usb_ack_and_reset();
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_afe_usb *)usb_ctrl)->reg_17.b6 = 1;
            usb_transfer_phase = 3;
            _builtin_set_imask(saved_imask);
            usb_bulk_state = 3;
            usb_bulk_out_state = 3;
            _builtin_set_imask(15);
            usb_transfer_busy &= 0xfffe;
            _builtin_set_imask(saved_imask);
            return 0;
        }
    }
    _builtin_set_imask(15);
    usb_transfer_busy &= 0xfffe;
    _builtin_set_imask(saved_imask);
    return state;
}
