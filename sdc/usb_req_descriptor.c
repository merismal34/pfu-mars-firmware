#include "fw.h"
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);
extern void _builtin_set_gbr(void *);
union usb_setup_byte { unsigned char c; struct Bits f; };
struct usb_regs_req_descriptor {
    char reg_00_03[9];
    unsigned char epa_int_stat;
    char pad_0a;
    unsigned char epb_int_stat;
    char reg_06_0b[13];
    unsigned char epa_int_enb;
    char pad_1a;
    unsigned char epb_int_enb;
    char pad_1c;
    unsigned char reg_0e;
    char reg_0f_13[11];
    struct Bits usb_status;
    char reg_15_16[5];
    struct Bits reg_17;
    char reg_18_1e[15];
    unsigned char usb_bank;
    char pad_40;
    unsigned char reg_20;
    char pad_42;
    unsigned char epa_max_size_l;
    char pad_44;
    union usb_setup_byte reg_22;
    char pad_46;
    unsigned char ep0_setup_3;
    char pad_48;
    unsigned char reg_24;
    char pad_4a;
    unsigned char reg_25;
    char pad_4c;
    union usb_setup_byte reg_26;
    char pad_4e;
    unsigned char ep0_setup_7;
    char reg_28_29[5];
    struct Bits reg_2a;
    char pad_56;
    struct Bits ep0_control_out;
    char pad_58;
    unsigned char epa_control;
    char pad_5a;
    unsigned char epb_control;
};


int usb_req_get_descriptor(void)
{
    int i;
    int byte;
    int j;
    int len;
    int x;
    const unsigned char *src;
    const struct usb_interface_descriptor *interface;
    const struct usb_interface_descriptor *configuration;
    const unsigned char *descriptor;
    unsigned char z;
    unsigned char emulation;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_20 & 31) return 1;
    _builtin_set_gbr(0);
    switch (((struct usb_regs_req_descriptor *)usb_ctrl)->ep0_setup_3) {
    case 1:
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c) return 1;
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_25) return 1;
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_24) return 1;
        if (usb_ep0_wlength) {
            emulation = settings_emulation[0] & 28;
            if (emulation == 28 || emulation == 24 || emulation == 20 || emulation == 17 || emulation == 21 || emulation == 8 || emulation == 4) {
                descriptor = (const unsigned char *)&usb_device_descriptor;
                len = *descriptor;
                src = descriptor;
                i = 0;
                j = 0;
                while (i < len) {
                    usb_ep0_reply_buf[j] = src[i];
                    i++;
                    j++;
                }
                switch (emulation) {
                case 28:
                    usb_ep0_reply_byte_10[0] = 173;
                    usb_ep0_reply_byte_11[0] = 16;
                    break;
                case 24:
                    usb_ep0_reply_byte_10[0] = 149;
                    usb_ep0_reply_byte_11[0] = 16;
                    break;
                case 20:
                    usb_ep0_reply_byte_10[0] = 231;
                    usb_ep0_reply_byte_11[0] = 16;
                    break;
                case 21:
                    usb_ep0_reply_byte_10[0] = 116;
                    usb_ep0_reply_byte_11[0] = 17;
                    break;
                case 8:
                    usb_ep0_reply_byte_10[0] = 118;
                    usb_ep0_reply_byte_11[0] = 17;
                    break;
                case 17:
                    usb_ep0_reply_byte_10[0] = 241;
                    usb_ep0_reply_byte_11[0] = 17;
                    break;
                default:
                    usb_ep0_reply_byte_10[0] = 120;
                    usb_ep0_reply_byte_11[0] = 17;
                    break;
                }
                usb_ep0_data_ptr = usb_ep0_reply_buf;
            } else {
                usb_ep0_data_ptr = (const unsigned char *)&usb_device_descriptor;
            }
            usb_ep0_done_len = usb_device_descriptor.bLength;
            if (usb_ep0_wlength < usb_ep0_done_len) usb_ep0_done_len = usb_ep0_wlength;
            usb_ep0_data_len = usb_ep0_done_len;
        }
        break;
    case 2:
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c) return 1;
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_25) return 1;
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_24) return 1;
        if (usb_ep0_wlength != 0) {
            j = 0;
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_descriptor *)usb_ctrl)->usb_status.b6) configuration = &usb_configuration_fs; else configuration = &usb_configuration_hs;
            len = configuration->bLength;
            src = (const unsigned char *)configuration;
            i = 0;
            while (i < len) {
                usb_ep0_reply_buf[j] = src[i];
                i++;
                j++;
            }
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_descriptor *)usb_ctrl)->usb_status.b6) interface = &usb_interface_fs[0]; else interface = &usb_interface_hs[0];
            len = interface->bLength;
            src = (const unsigned char *)interface;
            i = 0;
            while (i < len) {
                usb_ep0_reply_buf[j] = src[i];
                i++;
                j++;
            }
            i = 0;
            while (i < interface->bNumEndpoints) {
                len = interface->endpoints[i].bLength;
                src = (const unsigned char *)&interface->endpoints[i];
                byte = 0;
                while (byte < len) {
                    usb_ep0_reply_buf[j] = src[byte];
                    byte++;
                    j++;
                }
                i++;
            }
            usb_ep0_done_len = ((&configuration->bInterfaceNumber)[1] << 8) + configuration->bInterfaceNumber;
            if (usb_ep0_wlength < usb_ep0_done_len) usb_ep0_done_len = usb_ep0_wlength;
            usb_ep0_data_len = usb_ep0_done_len;
            usb_ep0_data_ptr = usb_ep0_reply_buf;
        }
        break;
    case 3:
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c > 4) return 1;
        if (usb_ep0_wlength != 0) {
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c == 0) {
                usb_ep0_reply_buf[0] = 4;
                usb_ep0_reply_byte_1[0] = 3;
                usb_ep0_reply_byte_2[0] = 9;
                usb_ep0_reply_byte_3[0] = 4;
                usb_ep0_done_len = 4;
                usb_ep0_data_ptr = usb_ep0_reply_buf;
            } else {
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_25 != 4) return 1;
                _builtin_set_gbr(0);
                if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_24 != 9) return 1;
                usb_ep0_data_ptr = usb_serial_string_ptr[0];
                usb_ep0_done_len = *usb_ep0_data_ptr;
                x = 0;
            }
            if (usb_ep0_wlength < usb_ep0_done_len) usb_ep0_done_len = usb_ep0_wlength;
            usb_ep0_data_len = usb_ep0_done_len;
        }
        break;
    case 6:
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c) return 1;
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_25) return 1;
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_24) return 1;
        if (usb_ep0_wlength != 0) {
            usb_ep0_data_ptr = (const unsigned char *)&usb_device_qualifier;
            usb_ep0_done_len = usb_device_qualifier.bLength;
            if (usb_ep0_wlength < usb_ep0_done_len) usb_ep0_done_len = usb_ep0_wlength;
            usb_ep0_data_len = usb_ep0_done_len;
        }
        break;
    case 7:
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c) return 1;
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_25) return 1;
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_24) return 1;
        if (usb_ep0_wlength != 0) {
            j = 0;
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_descriptor *)usb_ctrl)->usb_status.b6) configuration = &usb_configuration_hs; else configuration = &usb_configuration_fs;
            len = configuration->bLength;
            src = (const unsigned char *)configuration;
            i = 0;
            while (i < len) {
                usb_ep0_reply_buf[j] = src[i];
                i++;
                j++;
            }
            usb_ep0_reply_byte_1[0] = 7;
            _builtin_set_gbr(0);
            if (((struct usb_regs_req_descriptor *)usb_ctrl)->usb_status.b6) interface = &usb_interface_hs[0]; else interface = &usb_interface_fs[0];
            len = interface->bLength;
            src = (const unsigned char *)interface;
            i = 0;
            while (i < len) {
                usb_ep0_reply_buf[j] = src[i];
                i++;
                j++;
            }
            i = 0;
            while (i < interface->bNumEndpoints) {
                len = interface->endpoints[i].bLength;
                src = (const unsigned char *)&interface->endpoints[i];
                byte = 0;
                while (byte < len) {
                    usb_ep0_reply_buf[j] = src[byte];
                    byte++;
                    j++;
                }
                i++;
            }
            usb_ep0_done_len = ((&configuration->bInterfaceNumber)[1] << 8) + configuration->bInterfaceNumber;
            if (usb_ep0_wlength < usb_ep0_done_len) usb_ep0_done_len = usb_ep0_wlength;
            usb_ep0_data_len = usb_ep0_done_len;
            usb_ep0_data_ptr = usb_ep0_reply_buf;
        }
        break;
    case 8:
    default:
        return 1;
    }
    return 0;
}

int usb_req_get_configuration(void)
{
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_20 & 31) return 1;
    if (usb_device_state == USB_STATE_DEFAULT) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->ep0_setup_3) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_25) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_24) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->ep0_setup_7) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_26.c != 1) return 1;
    if (usb_device_state == USB_STATE_CONFIGURED) usb_ep0_reply_buf[0] = 1;
    else usb_ep0_reply_buf[0] = 0;
    usb_ep0_done_len = usb_ep0_data_len = 1;
    usb_ep0_data_ptr = usb_ep0_reply_buf;
    return 0;
}

int usb_req_set_configuration(void)
{
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_20 & 31) return 1;
    if (usb_device_state == USB_STATE_DEFAULT) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->ep0_setup_3) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_25) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_24) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->ep0_setup_7) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_26.c) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c > 1) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c) {
        usb_device_state = USB_STATE_CONFIGURED;
        usb_alternate_setting = 0;
        usb_reset_endpoints(usb_alternate_setting);
    } else {
        usb_device_state = USB_STATE_ADDRESS;
        _builtin_set_gbr(0);
        ((struct usb_regs_req_descriptor *)usb_ctrl)->usb_bank = 2;
        _builtin_set_gbr(0);
        ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.f.b5 = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_26.f.b5 = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_2a.b5 = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_req_descriptor *)usb_ctrl)->usb_bank = 0;
    }
    return 0;
}

int usb_req_get_interface(void)
{
    _builtin_set_gbr(0);
    if ((((struct usb_regs_req_descriptor *)usb_ctrl)->reg_20 & 31) != 1) return 1;
    if (usb_device_state != USB_STATE_CONFIGURED) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->ep0_setup_3) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_25) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->ep0_setup_7) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_26.c != 1) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_24) return 1;
    usb_ep0_reply_buf[0] = usb_alternate_setting;
    usb_ep0_done_len = usb_ep0_data_len = 1;
    usb_ep0_data_ptr = usb_ep0_reply_buf;
    return 0;
}

int usb_req_set_interface(void)
{
    _builtin_set_gbr(0);
    if ((((struct usb_regs_req_descriptor *)usb_ctrl)->reg_20 & 31) != 1) return 1;
    if (usb_device_state != USB_STATE_CONFIGURED) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->ep0_setup_3) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_25) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->ep0_setup_7) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_26.c) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_24) return 1;
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->usb_status.b6) {
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c > usb_interface_fs[0].bAlternateSetting) return 1;
    } else {
        _builtin_set_gbr(0);
        if (((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c > usb_interface_hs[0].bAlternateSetting) return 1;
    }
    _builtin_set_gbr(0);
    usb_alternate_setting = ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c;
    _builtin_set_gbr(0);
    usb_reset_endpoints(((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c);
    return 0;
}

int usb_req_unsupported(void)
{
    int stall;
    stall = 1;
    return stall;
}

void usb_reset_endpoints(unsigned char alternate)
{
    const struct usb_interface_descriptor *interface;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    _builtin_set_gbr(0);
    if (((struct usb_regs_req_descriptor *)usb_ctrl)->usb_status.b6) interface = &usb_interface_fs[alternate]; else interface = &usb_interface_hs[alternate];
    usb_epa_max_packet.v = ((&interface->endpoints->wMaxPacketSize_lo)[1] << 8) + interface->endpoints->wMaxPacketSize_lo;
    usb_epb_max_packet = ((&interface->endpoints[1].wMaxPacketSize_lo)[1] << 8) + interface->endpoints[1].wMaxPacketSize_lo;
    _builtin_set_imask(15);
    /* bank 2: max packet size high/low and endpoint address (bit 5 = enabled) of EPa, then EPb */
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->usb_bank = 2;
    usb_epa_address[0] = interface->endpoints->bEndpointAddress;
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.c = usb_epa_address[0];
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_20 = usb_epa_max_packet.v >> 8;
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->epa_max_size_l = usb_epa_max_packet.c.lo;
    usb_epb_address[0] = interface->endpoints[1].bEndpointAddress;
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_26.c = usb_epb_address[0];
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_24 = usb_epb_max_packet >> 8;
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_25 = (unsigned char)usb_epb_max_packet;
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->usb_bank = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->epa_control = 4;
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->epb_control = 132;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->epa_int_stat = 255;
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->epb_int_stat = 255;
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->epa_int_enb = 2;
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->epb_int_enb = 17;
    _builtin_set_imask(saved_imask);
    usb_bulk_state = 2;
    usb_bulk_out_state = 2;
    usb_epc_address[0] = 0;
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_0e = 0;
    if (usb_device_state == USB_STATE_CONFIGURED) {
        _builtin_set_gbr(0);
        ((struct usb_regs_req_descriptor *)usb_ctrl)->usb_bank = 2;
        _builtin_set_gbr(0);
        ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_22.f.b5 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_26.f.b5 = 1;
        _builtin_set_gbr(0);
        ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_2a.b5 = 0;
        _builtin_set_gbr(0);
        ((struct usb_regs_req_descriptor *)usb_ctrl)->usb_bank = 0;
    }
    _builtin_set_gbr(0);
    ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_17.b5 = 1;
    _builtin_set_imask(saved_imask);
}

void usb_ep0_run(void)
{
    struct page_record *current_page;
    int saved_imask;
    current_page = &page_records[page_current.v];
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    tick_hostif_wait_ticks.v = 0;
    _builtin_set_imask(saved_imask);
    while (1) {
        if (usb_ep0_stage >= 7)
            break;
        usb_ep0_stage_table[usb_ep0_stage]();
        if (usb_ep0_stage >= 7)
            break;
        usb_poll_device_events();
        usb_run_events_6_4();
        if (usb_event_flags.f.bus_reset) {
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_17.b0 = 1;
            _builtin_set_imask(saved_imask);
            usb_ep0_event = 0;
            usb_ep0_stage = 7;
            break;
        }
        if (usb_event_flags.f.detach) {
            usb_ep0_stage = 7;
            _builtin_set_imask(15);
            if (current_page->sides[scsi_read_side.v].flags.f.requested || current_page->sides[scsi_read_side.v].flags.f.requested)
                current_page->flags.f.scan_end = 1;
            _builtin_set_imask(saved_imask);
            break;
        }
        if (tick_hostif_wait_ticks.v > 0x9c4) {
            usb_ep0_event = 0;
            _builtin_set_imask(15);
            _builtin_set_gbr(0);
            ((struct usb_regs_req_descriptor *)usb_ctrl)->reg_2a.b0 = 1;
            _builtin_set_gbr(0);
            ((struct usb_regs_req_descriptor *)usb_ctrl)->ep0_control_out.b0 = 1;
            _builtin_set_imask(saved_imask);
            usb_ep0_stage = 7;
            break;
        }
    }
}
