#include "boot.h"
#pragma section 2
struct nibbles_usb { unsigned char hi:4, lo:4; };

unsigned int fw_update_usb_run_std_request(void)
{
    unsigned int stall;

    fw_update_sync_dst = fw_update_sync_src;
    if ((unsigned char)fw_update_p_usb_ctrl->q[0x13] < 13) {
        fw_update_sync_dst = fw_update_sync_src;
        stall = fw_update_usb_std_request_table[(unsigned char)fw_update_p_usb_ctrl->q[0x13]]();
    } else
        stall = fw_update_usb_req_unsupported();
    return stall;
}


int fw_update_usb_req_get_status(void)
{
    int stall;

    stall = 0;
    if (fw_update_usb_device_state == USB_STATE_DEFAULT) {
        stall = 1;
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        switch ((unsigned char)fw_update_p_usb_ctrl->q[0x11] & 0x1f) {
        case 0:
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x17] || fw_update_p_usb_ctrl->q[0x15]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x1f] || (unsigned char)fw_update_p_usb_ctrl->q[0x1d] != 2) {
                stall = 1;
                break;
            }
            fw_update_usb_ep0_reply_buf[0] = fw_update_usb_ctrl_flags.b0 * 2 + 1;
            fw_update_usb_ep0_reply_byte_1[0] = 0;
            fw_update_usb_ep0_done_len = fw_update_usb_ep0_data_len = 2;
            fw_update_usb_ep0_data_ptr = fw_update_usb_ep0_reply_buf;
            break;
        case 1:
            if (fw_update_usb_device_state == USB_STATE_ADDRESS) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x17] || fw_update_p_usb_ctrl->q[0x15]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x1f] || (unsigned char)fw_update_p_usb_ctrl->q[0x1d] != 2) {
                stall = 1;
                break;
            }
            fw_update_usb_ep0_reply_buf[0] = fw_update_usb_ep0_reply_byte_1[0] = 0;
            fw_update_usb_ep0_done_len = fw_update_usb_ep0_data_len = 2;
            fw_update_usb_ep0_data_ptr = fw_update_usb_ep0_reply_buf;
            break;
        case 2:
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x17] || fw_update_p_usb_ctrl->q[0x15]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x1b]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x1f] || (unsigned char)fw_update_p_usb_ctrl->q[0x1d] != 2) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x19] == 0) {
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->ep0_control_in.b0 || fw_update_p_usb_ctrl->ep0_control_out.b0)
                    fw_update_usb_ep0_reply_buf[0] = 1;
                else
                    fw_update_usb_ep0_reply_buf[0] = 0;
                fw_update_usb_ep0_reply_byte_1[0] = 0;
                fw_update_usb_ep0_done_len = fw_update_usb_ep0_data_len = 2;
                fw_update_usb_ep0_data_ptr = fw_update_usb_ep0_reply_buf;
            } else {
                if ((unsigned char)fw_update_usb_device_state == USB_STATE_ADDRESS) {
                    stall = 1;
                    break;
                }
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->q[0x19] == fw_update_usb_epa_address[0]) {
                    fw_update_sync_dst = fw_update_sync_src;
                    if (fw_update_p_usb_ctrl->epa_control.b0)
                        fw_update_usb_ep0_reply_buf[0] = 1;
                    else
                        fw_update_usb_ep0_reply_buf[0] = 0;
                } else {
                    fw_update_sync_dst = fw_update_sync_src;
                    if (fw_update_p_usb_ctrl->q[0x19] == fw_update_usb_epb_address[0]) {
                        fw_update_sync_dst = fw_update_sync_src;
                        if (fw_update_p_usb_ctrl->epb_control.b0)
                            fw_update_usb_ep0_reply_buf[0] = 1;
                        else
                            fw_update_usb_ep0_reply_buf[0] = 0;
                    } else {
                        fw_update_sync_dst = fw_update_sync_src;
                        if (fw_update_p_usb_ctrl->q[0x19] == fw_update_usb_epc_address[0]) {
                            fw_update_sync_dst = fw_update_sync_src;
                            if (fw_update_p_usb_ctrl->epc_control.b0)
                                fw_update_usb_ep0_reply_buf[0] = 1;
                            else
                                fw_update_usb_ep0_reply_buf[0] = 0;
                        } else {
                            stall = 1;
                        }
                    }
                }
                if (stall == 0) {
                    fw_update_usb_ep0_reply_byte_1[0] = 0;
                    fw_update_usb_ep0_done_len = fw_update_usb_ep0_data_len = 2;
                    fw_update_usb_ep0_data_ptr = fw_update_usb_ep0_reply_buf;
                }
            }
            break;
        default:
            stall = 1;
        }
    }
    return stall;
}


int fw_update_usb_req_clear_feature(void)
{
    int stall;
    unsigned char saved_imask;

    stall = 0;
    saved_imask = _builtin_get_imask();
    if (fw_update_usb_device_state == USB_STATE_DEFAULT) {
        stall = 1;
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        switch ((unsigned char)fw_update_p_usb_ctrl->q[0x11] & 0x1f) {
        case 0:
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x17]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if ((unsigned char)fw_update_p_usb_ctrl->q[0x15] == 1) {
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19]) {
                    stall = 1;
                    break;
                }
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->q[0x1f] || fw_update_p_usb_ctrl->q[0x1d]) {
                    stall = 1;
                    break;
                }
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->usb_status.b6) {
                    if (!(fw_update_usb_config_attributes_fs.v & 0x20)) {
                        stall = 1;
                        break;
                    }
                } else {
                    if (!(fw_update_usb_config_attributes_hs.v & 0x20)) {
                        stall = 1;
                        break;
                    }
                }
                fw_update_usb_ctrl_flags.b0 = 0;
            } else {
                fw_update_sync_dst = fw_update_sync_src;
                if ((unsigned char)fw_update_p_usb_ctrl->q[0x15] == 2) {
                    fw_update_sync_dst = fw_update_sync_src;
                    if (fw_update_p_usb_ctrl->usb_status.b6) {
                        stall = 1;
                        break;
                    }
                    fw_update_sync_dst = fw_update_sync_src;
                    if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19]) {
                        stall = 1;
                        break;
                    }
                    fw_update_sync_dst = fw_update_sync_src;
                    if (fw_update_p_usb_ctrl->q[0x1f] || fw_update_p_usb_ctrl->q[0x1d])
                        stall = 1;
                } else {
                    stall = 1;
                }
            }
            break;
        case 2:
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x17]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x15] != 0) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x1b]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x1f] || fw_update_p_usb_ctrl->q[0x1d]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x19] == 0) {
                stall = 1;
            } else {
                if ((unsigned char)fw_update_usb_device_state == USB_STATE_ADDRESS) {
                    stall = 1;
                    break;
                }
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->q[0x19] == fw_update_usb_epa_address[0]) {
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->epa_control.b2 = 1;
                    fw_update_sync_dst = fw_update_sync_src;
                    if (fw_update_p_usb_ctrl->epa_control.b0) {
                        fw_update_sync_dst = fw_update_sync_src;
                        fw_update_p_usb_ctrl->reg_17.b1 = 1;
                        if (fw_update_usb_transfer_phase != 3) {
                            fw_update_sync_dst = fw_update_sync_src;
                            fw_update_p_usb_ctrl->epa_control.b0 = 0;
                            fw_update_usb_bulk_state = 2;
                            if (fw_update_usb_transfer_phase == 1) {
                                _builtin_set_imask(15);
                                fw_update_usb_transfer_phase = 2;
                                _builtin_set_imask(saved_imask);
                                fw_update_usb_transfer_flags.done = 1;
                            }
                        }
                    }
                } else {
                    fw_update_sync_dst = fw_update_sync_src;
                    if (fw_update_p_usb_ctrl->q[0x19] == fw_update_usb_epb_address[0]) {
                        fw_update_sync_dst = fw_update_sync_src;
                        fw_update_p_usb_ctrl->epb_control.b2 = 1;
                        fw_update_sync_dst = fw_update_sync_src;
                        if (fw_update_p_usb_ctrl->epb_control.b0) {
                            fw_update_sync_dst = fw_update_sync_src;
                            fw_update_p_usb_ctrl->reg_17.b2 = 1;
                            if (fw_update_usb_transfer_phase != 3) {
                                fw_update_sync_dst = fw_update_sync_src;
                                fw_update_p_usb_ctrl->epb_control.b0 = 0;
                                fw_update_usb_bulk_out_state = 2;
                                if (fw_update_usb_transfer_phase == 1) {
                                    _builtin_set_imask(15);
                                    fw_update_usb_transfer_phase = 2;
                                    _builtin_set_imask(saved_imask);
                                    fw_update_usb_transfer_flags.done = 1;
                                }
                            }
                        }
                    }
                }
            }
            break;
        default:
            stall = 1;
        }
    }
    return stall;
}


int fw_update_usb_req_set_feature(void)
{
    int i;
    int stall;

    stall = 0;
    fw_update_sync_dst = fw_update_sync_src;
    switch ((unsigned char)fw_update_p_usb_ctrl->q[0x11] & 0x1f) {
    case 0:
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x17]) {
            stall = 1;
            break;
        }
        fw_update_sync_dst = fw_update_sync_src;
        if ((unsigned char)fw_update_p_usb_ctrl->q[0x15] == 1) {
            if (fw_update_usb_device_state == USB_STATE_DEFAULT) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x1f] || fw_update_p_usb_ctrl->q[0x1d]) {
                stall = 1;
                break;
            }
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->usb_status.b6) {
                if ((fw_update_usb_config_attributes_fs.v & 0x20) == 0)
                    stall = 1;
            } else {
                if ((fw_update_usb_config_attributes_hs.v & 0x20) == 0)
                    stall = 1;
            }
            if (stall == 0)
                fw_update_usb_ctrl_flags.b0 = 1;
        } else {
            fw_update_sync_dst = fw_update_sync_src;
            if ((unsigned char)fw_update_p_usb_ctrl->q[0x15] == 2) {
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->usb_status.b6) {
                    stall = 1;
                    break;
                }
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->q[0x19]) {
                    stall = 1;
                    break;
                }
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->q[0x1f] || fw_update_p_usb_ctrl->q[0x1d]) {
                    stall = 1;
                    break;
                }
                fw_update_sync_dst = fw_update_sync_src;
                switch ((unsigned char)fw_update_p_usb_ctrl->q[0x1b]) {
                case 1:
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->usb_test.b2 = 1;
                    break;
                case 2:
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->usb_test.b1 = 1;
                    break;
                case 3:
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->usb_test.b3 = 1;
                    break;
                case 4:
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->q[0xf] = 2;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->q[0x21] = 0;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->q[0x23] = 0x40;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->ep0_control_in.b7 = 1;
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct nibbles_usb *)&fw_update_p_usb_ctrl->ep0_control_in)->lo = 15;
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct Bits *)&fw_update_p_usb_ctrl->q[0x15])->b5 = 0;
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct Bits *)&fw_update_p_usb_ctrl->q[0x1d])->b5 = 0;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->ep0_control_in.b5 = 1;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->reg_34 = 9;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->reg_35 = 0xc0;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->reg_36 = 10;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->fifo_byte = 0;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->q[0xf] = 0;
                    fw_update_sync_dst = fw_update_sync_src;
                    ((struct Bits *)&fw_update_p_usb_ctrl->reg_0e)->b1 = 0;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->reg_17.b5 = 1;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->reg_31.b3 = 1;
                    for (i = 0; i < 53; i++) {
                        fw_update_sync_dst = fw_update_sync_src;
                        fw_update_p_usb_ctrl->fifo_byte = fw_update_usb_test_packet[i];
                    }
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->reg_31.b3 = 0;
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->usb_test.b0 = 1;
                    break;
                default:
                    stall = 1;
                    break;
                }
                if (stall == 0)
                    fw_update_usb_ctrl_flags.b4 = 1;
            } else {
                stall = 1;
            }
        }
        break;
    case 2:
        if (fw_update_usb_device_state == USB_STATE_DEFAULT) {
            stall = 1;
            break;
        }
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x17]) {
            stall = 1;
            break;
        }
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x15] != 0) {
            stall = 1;
            break;
        }
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x1b]) {
            stall = 1;
            break;
        }
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x1f] || fw_update_p_usb_ctrl->q[0x1d]) {
            stall = 1;
            break;
        }
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x19] == 0) {
            stall = 1;
            break;
        }
        else if (fw_update_usb_device_state == USB_STATE_ADDRESS) {
            stall = 1;
            break;
        }
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x19] == fw_update_usb_epa_address[0]) {
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->epa_control.b0 == 0) {
                fw_update_sync_dst = fw_update_sync_src;
                fw_update_p_usb_ctrl->epa_control.b0 = 1;
                fw_update_usb_bulk_state = 3;
            }
        } else {
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x19] == fw_update_usb_epb_address[0]) {
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->epb_control.b0 == 0) {
                    fw_update_sync_dst = fw_update_sync_src;
                    fw_update_p_usb_ctrl->epb_control.b0 = 1;
                    fw_update_usb_bulk_out_state = 3;
                }
            } else {
                fw_update_sync_dst = fw_update_sync_src;
                if (fw_update_p_usb_ctrl->q[0x19] == fw_update_usb_epc_address[0]) {
                    fw_update_sync_dst = fw_update_sync_src;
                    if (fw_update_p_usb_ctrl->epc_control.b0 == 0) {
                        fw_update_sync_dst = fw_update_sync_src;
                        fw_update_p_usb_ctrl->epc_control.b0 = 1;
                    }
                } else {
                    stall = 1;
                }
            }
        }
        break;
    default:
        stall = 1;
    }
    return stall;
}

int fw_update_usb_req_set_address(void)
{
    int u;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    if (fw_update_usb_device_state == USB_STATE_CONFIGURED)
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if ((unsigned char)fw_update_p_usb_ctrl->q[0x11] & 0x1f)
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x17])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x1f] || fw_update_p_usb_ctrl->q[0x1d])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if ((unsigned char)fw_update_p_usb_ctrl->q[0x15] > 127)
        return 1;
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_usb_address = fw_update_p_usb_ctrl->q[0x15];
    fw_update_usb_ctrl_flags.b5 = 1;
    _builtin_set_imask(saved_imask);
    return 0;
}


int fw_update_usb_req_get_descriptor(void)
{
    int i;
    int j;
    int byte;
    int len;
    int u;
    unsigned char *src;
    struct fw_update_usb_interface_view *interface;
    struct fw_update_usb_config_set *configuration;
    int z4;
    int z0;

    fw_update_sync_dst = fw_update_sync_src;
    if ((unsigned char)fw_update_p_usb_ctrl->q[0x11] & 0x1f)
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    switch ((unsigned char)fw_update_p_usb_ctrl->q[0x17]) {
    case 1:
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x15] != 0)
            return 1;
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19])
            return 1;
        if (fw_update_usb_ep0_wlength != 0) {
            fw_update_usb_ep0_data_ptr = &fw_update_usb_device_descriptor.v;
            fw_update_usb_ep0_done_len = fw_update_usb_device_descriptor.v;
            if (fw_update_usb_ep0_wlength < fw_update_usb_ep0_done_len)
                fw_update_usb_ep0_done_len = fw_update_usb_ep0_wlength;
            fw_update_usb_ep0_data_len = fw_update_usb_ep0_done_len;
        }
        break;
    case 2:
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x15] != 0)
            return 1;
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19])
            return 1;
        if (fw_update_usb_ep0_wlength != 0) {
            byte = 0;
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->usb_status.b6)
                configuration = &fw_update_usb_config_fs;
            else
                configuration = &fw_update_usb_config_hs;
            len = configuration->bLength;
            src = (unsigned char *)configuration;
            for (i = 0; i < len; i++, byte++)
                fw_update_usb_ep0_reply_buf[byte] = src[i];
            if (fw_update_p_usb_ctrl->usb_status.b6)
                interface = fw_update_usb_interface_fs;
            else
                interface = fw_update_usb_interface_hs;
            len = interface->bLength;
            src = (unsigned char *)interface;
            for (i = 0; i < len; i++, byte++)
                fw_update_usb_ep0_reply_buf[byte] = src[i];
            for (i = 0; i < interface->bNumEndpoints; i++) {
                len = interface->p[i].bLength;
                src = (unsigned char *)&interface->p[i];
                for (j = 0; j < len; j++, byte++)
                    fw_update_usb_ep0_reply_buf[byte] = src[j];
            }
            fw_update_usb_ep0_done_len = ((&configuration->wTotalLength_lo)[1] << 8) + configuration->wTotalLength_lo;
            if (fw_update_usb_ep0_wlength < fw_update_usb_ep0_done_len)
                fw_update_usb_ep0_done_len = fw_update_usb_ep0_wlength;
            fw_update_usb_ep0_data_len = fw_update_usb_ep0_done_len;
            fw_update_usb_ep0_data_ptr = fw_update_usb_ep0_reply_buf;
        }
        break;
    case 3:
        fw_update_sync_dst = fw_update_sync_src;
        if ((unsigned char)fw_update_p_usb_ctrl->q[0x15] > 4)
            return 1;
        if (fw_update_usb_ep0_wlength != 0) {
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->q[0x15] == 0) {
                fw_update_usb_ep0_reply_buf[0] = 4;
                fw_update_usb_ep0_reply_byte_1[0] = 3;
                fw_update_usb_ep0_reply_byte_2[0] = 9;
                fw_update_usb_ep0_reply_byte_3[0] = 4;
                fw_update_usb_ep0_done_len = 4;
                fw_update_usb_ep0_data_ptr = fw_update_usb_ep0_reply_buf;
            } else {
                fw_update_sync_dst = fw_update_sync_src;
                if ((unsigned char)fw_update_p_usb_ctrl->q[0x1b] != 4 || (unsigned char)fw_update_p_usb_ctrl->q[0x19] != 9)
                    return 1;
                fw_update_usb_ep0_data_ptr = fw_update_usb_serial_string_ptr[0];
                fw_update_usb_ep0_done_len = *fw_update_usb_ep0_data_ptr;
                u = 0;
            }
            if (fw_update_usb_ep0_wlength < fw_update_usb_ep0_done_len)
                fw_update_usb_ep0_done_len = fw_update_usb_ep0_wlength;
            fw_update_usb_ep0_data_len = fw_update_usb_ep0_done_len;
        }
        break;
    case 6:
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x15] != 0)
            return 1;
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19])
            return 1;
        if (fw_update_usb_ep0_wlength != 0) {
            fw_update_usb_ep0_data_ptr = &fw_update_usb_qualifier_descriptor.v;
            fw_update_usb_ep0_done_len = fw_update_usb_qualifier_descriptor.v;
            if (fw_update_usb_ep0_wlength < fw_update_usb_ep0_done_len)
                fw_update_usb_ep0_done_len = fw_update_usb_ep0_wlength;
            fw_update_usb_ep0_data_len = fw_update_usb_ep0_done_len;
        }
        break;
    case 7:
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x15] != 0)
            return 1;
        fw_update_sync_dst = fw_update_sync_src;
        if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19])
            return 1;
        if (fw_update_usb_ep0_wlength != 0) {
            byte = 0;
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->usb_status.b6)
                configuration = &fw_update_usb_config_hs;
            else
                configuration = &fw_update_usb_config_fs;
            len = configuration->bLength;
            src = (unsigned char *)configuration;
            for (i = 0; i < len; i++, byte++)
                fw_update_usb_ep0_reply_buf[byte] = src[i];
            fw_update_usb_ep0_reply_byte_1[0] = 7;
            fw_update_sync_dst = fw_update_sync_src;
            if (fw_update_p_usb_ctrl->usb_status.b6)
                interface = fw_update_usb_interface_hs;
            else
                interface = fw_update_usb_interface_fs;
            len = interface->bLength;
            src = (unsigned char *)interface;
            for (i = 0; i < len; i++, byte++)
                fw_update_usb_ep0_reply_buf[byte] = src[i];
            for (i = 0; i < interface->bNumEndpoints; i++) {
                len = interface->p[i].bLength;
                src = (unsigned char *)&interface->p[i];
                for (j = 0; j < len; j++, byte++)
                    fw_update_usb_ep0_reply_buf[byte] = src[j];
            }
            fw_update_usb_ep0_done_len = ((&configuration->wTotalLength_lo)[1] << 8) + configuration->wTotalLength_lo;
            if (fw_update_usb_ep0_wlength < fw_update_usb_ep0_done_len)
                fw_update_usb_ep0_done_len = fw_update_usb_ep0_wlength;
            fw_update_usb_ep0_data_len = fw_update_usb_ep0_done_len;
            fw_update_usb_ep0_data_ptr = fw_update_usb_ep0_reply_buf;
        }
        break;
    case 8:
    default:
        return 1;
    }
    return 0;
}

int fw_update_usb_req_get_configuration(void)
{
    fw_update_sync_dst = fw_update_sync_src;
    if ((unsigned char)fw_update_p_usb_ctrl->q[0x11] & 0x1f)
        return 1;
    if (fw_update_usb_device_state == USB_STATE_DEFAULT)
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x17] || fw_update_p_usb_ctrl->q[0x15])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x1f] || (unsigned char)fw_update_p_usb_ctrl->q[0x1d] != 1)
        return 1;
    if (fw_update_usb_device_state == USB_STATE_CONFIGURED)
        fw_update_usb_ep0_reply_buf[0] = 1;
    else
        fw_update_usb_ep0_reply_buf[0] = 0;
    fw_update_usb_ep0_done_len = fw_update_usb_ep0_data_len = 1;
    fw_update_usb_ep0_data_ptr = fw_update_usb_ep0_reply_buf;
    return 0;
}


int fw_update_usb_req_set_configuration(void)
{
    fw_update_sync_dst = fw_update_sync_src;
    if ((unsigned char)fw_update_p_usb_ctrl->q[0x11] & 0x1f)
        return 1;
    if (fw_update_usb_device_state == USB_STATE_DEFAULT)
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x17])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x1b] || fw_update_p_usb_ctrl->q[0x19])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x1f] || fw_update_p_usb_ctrl->q[0x1d])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if ((unsigned char)fw_update_p_usb_ctrl->q[0x15] > 1)
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x15]) {
        fw_update_usb_device_state = USB_STATE_CONFIGURED;
        fw_update_usb_alternate_setting = 0;
        fw_update_usb_reset_endpoints(fw_update_usb_alternate_setting);
    } else {
        fw_update_usb_device_state = USB_STATE_ADDRESS;
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_usb_ctrl->q[0xf] = 2;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct Bits *)&fw_update_p_usb_ctrl->q[0x15])->b5 = 0;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct Bits *)&fw_update_p_usb_ctrl->q[0x1d])->b5 = 0;
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_usb_ctrl->ep0_control_in.b5 = 0;
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_usb_ctrl->q[0xf] = 0;
    }
    return 0;
}

int fw_update_usb_req_get_interface(void)
{
    fw_update_sync_dst = fw_update_sync_src;
    if (((unsigned char)fw_update_p_usb_ctrl->q[0x11] & 0x1f) != 1)
        return 1;
    if (fw_update_usb_device_state != USB_STATE_CONFIGURED)
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x17] || fw_update_p_usb_ctrl->q[0x15])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x1b])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x1f] || (unsigned char)fw_update_p_usb_ctrl->q[0x1d] != 1)
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x19])
        return 1;
    fw_update_usb_ep0_reply_buf[0] = fw_update_usb_alternate_setting;
    fw_update_usb_ep0_done_len = fw_update_usb_ep0_data_len = 1;
    fw_update_usb_ep0_data_ptr = fw_update_usb_ep0_reply_buf;
    return 0;
}

int fw_update_usb_req_set_interface(void)
{
    fw_update_sync_dst = fw_update_sync_src;
    if (((unsigned char)fw_update_p_usb_ctrl->q[0x11] & 0x1f) != 1)
        return 1;
    if (fw_update_usb_device_state != USB_STATE_CONFIGURED)
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x17])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x1b])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x1f] || fw_update_p_usb_ctrl->q[0x1d])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->q[0x19])
        return 1;
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->usb_status.b6) {
        fw_update_sync_dst = fw_update_sync_src;
        if ((unsigned char)fw_update_p_usb_ctrl->q[0x15] > fw_update_usb_interface_fs[0].bAlternateSetting)
            return 1;
    } else {
        fw_update_sync_dst = fw_update_sync_src;
        if ((unsigned char)fw_update_p_usb_ctrl->q[0x15] > fw_update_usb_interface_hs[0].bAlternateSetting)
            return 1;
    }
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_usb_alternate_setting = fw_update_p_usb_ctrl->q[0x15];
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_usb_reset_endpoints(fw_update_p_usb_ctrl->q[0x15]);
    return 0;
}

int fw_update_usb_req_unsupported(void)
{
    int stall;

    stall = 1;
    return stall;
}

int fw_update_usb_reset_endpoints(unsigned char alternate)
{
    struct fw_update_usb_interface_view *interface;
    unsigned char saved_imask;

    saved_imask = _builtin_get_imask();
    fw_update_sync_dst = fw_update_sync_src;
    if (fw_update_p_usb_ctrl->usb_status.b6)
        interface = &fw_update_usb_interface_fs[alternate];
    else
        interface = &fw_update_usb_interface_hs[alternate];
    fw_update_usb_epa_max_packet = ((&interface->p->wMaxPacketSize_lo)[1] << 8) + interface->p->wMaxPacketSize_lo;
    fw_update_usb_epb_max_packet = ((&interface->p[1].wMaxPacketSize_lo)[1] << 8) + interface->p[1].wMaxPacketSize_lo;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->q[0xf] = 2;
    fw_update_usb_epa_address[0] = interface->p->bEndpointAddress;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->q[0x15] = fw_update_usb_epa_address[0];
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->q[0x11] = fw_update_usb_epa_max_packet >> 8;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->q[0x13] = fw_update_usb_max_packet_epa_low;
    fw_update_usb_epb_address[0] = interface->p[1].bEndpointAddress;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->q[0x1d] = fw_update_usb_epb_address[0];
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->q[0x19] = fw_update_usb_epb_max_packet >> 8;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->q[0x1b] = fw_update_usb_max_packet_epb_low;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->q[0xf] = 0;
    fw_update_sync_dst = fw_update_sync_src;
    *(char *)&fw_update_p_usb_ctrl->epa_control = 4;
    fw_update_sync_dst = fw_update_sync_src;
    *(unsigned char *)&fw_update_p_usb_ctrl->epb_control = 0x84;
    _builtin_set_imask(15);
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->epa_int_stat = 0xff;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->epb_int_stat = 0xff;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->epa_int_enb = 2;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->epb_int_enb = 17;
    _builtin_set_imask(saved_imask);
    fw_update_usb_bulk_state = 2;
    fw_update_usb_bulk_out_state = 2;
    fw_update_usb_epc_address[0] = 0;
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->reg_0e = 0;
    if (fw_update_usb_device_state == USB_STATE_CONFIGURED) {
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_usb_ctrl->q[0xf] = 2;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct Bits *)&fw_update_p_usb_ctrl->q[0x15])->b5 = 1;
        fw_update_sync_dst = fw_update_sync_src;
        ((struct Bits *)&fw_update_p_usb_ctrl->q[0x1d])->b5 = 1;
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_usb_ctrl->ep0_control_in.b5 = 0;
        fw_update_sync_dst = fw_update_sync_src;
        fw_update_p_usb_ctrl->q[0xf] = 0;
    }
    fw_update_sync_dst = fw_update_sync_src;
    fw_update_p_usb_ctrl->reg_17.b5 = 1;
}

void fw_update_usb_build_serial_string(unsigned int number)
{
    unsigned char *p;
    unsigned char digit;

    fw_update_usb_ep0_reply_buf[0] = 0x12;
    fw_update_usb_ep0_reply_byte_1[0] = 3;
    p = fw_update_usb_ep0_reply_byte_2;
    digit = 0;
    while (number >= 10000000) {
        number -= 10000000;
        digit++;
    }
    *p++ = digit | 0x30;
    *p++ = 0;
    digit = 0;
    while (number >= 1000000) {
        number -= 1000000;
        digit++;
    }
    *p++ = digit | 0x30;
    *p++ = 0;
    digit = 0;
    while (number >= 100000) {
        number -= 100000;
        digit++;
    }
    *p++ = digit | 0x30;
    *p++ = 0;
    digit = 0;
    while (number >= 10000) {
        number -= 10000;
        digit++;
    }
    *p++ = digit | 0x30;
    *p++ = 0;
    digit = 0;
    while (number >= 1000) {
        number -= 1000;
        digit++;
    }
    *p++ = digit | 0x30;
    *p++ = 0;
    digit = 0;
    while (number >= 100) {
        number -= 100;
        digit++;
    }
    *p++ = digit | 0x30;
    *p++ = 0;
    digit = 0;
    while (number >= 10) {
        number -= 10;
        digit++;
    }
    *p++ = digit | 0x30;
    *p++ = 0;
    *p++ = number | 0x30;
    *p++ = 0;
}

/* the USB descriptors and the standard request table */
/* USB 2.0 test packet (53 bytes, Test_Packet of chapter 7.1.20) and one 0 */
const unsigned char fw_update_usb_test_packet[54] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
    0xee, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfc, 0x7e, 0xbf, 0xdf,
    0xef, 0xf7, 0xfb, 0xfd, 0x7e, 0x00
};
/* USB device descriptor (18 bytes), split at fw_update_usb_desc_max_packet0 = bMaxPacketSize0, which fw_update_usb_device.c reads by that name */
const struct view_fw_update_usb_device_descriptor fw_update_usb_device_descriptor = {
    0x12, {0x01, 0x00, 0x02, 0x00, 0x00, 0x00}
};
const struct view_fw_update_usb_desc_max_packet0 fw_update_usb_desc_max_packet0 = {
    0x40, {0xc5, 0x04, 0x74, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01}
};
/* device qualifier (10 bytes), then string descriptor "20000000" (UTF-16, 18
   bytes; fw_update_usb_serial_string_ptr points to it) */
const struct view_fw_update_usb_qualifier_descriptor fw_update_usb_qualifier_descriptor = {
    0x0a, {0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00, 0x12, 0x03,
    0x32, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00,
    0x30, 0x00, 0x30, 0x00}
};
/* full-speed configuration descriptor (bulk endpoints of 64 bytes; 9 bytes + 1 pad), split at
   bmAttributes (fw_update_usb_config_attributes_fs, read by fw_update_usb_requests.c by that name), then two endpoint descriptors
   (struct fw_update_usb_endpoint_entry, 8 bytes each: 7 + 1 pad) and 2 pad bytes */
const struct fw_update_usb_config_set fw_update_usb_config_fs = {
    0x09, 0x02, 0x20, {0x00, 0x01, 0x01, 0x00}
};
const struct fw_update_usb_config_view fw_update_usb_config_attributes_fs = {
    0xc0, {0x31, 0x00, 0x07, 0x05, 0x81, 0x02, 0x40, 0x00, 0xff, 0x00, 0x07,
    0x05, 0x02, 0x02, 0x40, 0x00, 0xff, 0x00, 0x00, 0x00}
};
/* interface descriptor (9 bytes + 3 pad) with a pointer to its endpoints */
const struct fw_update_usb_interface_view fw_update_usb_interface_fs[1] = {
    {0x09, {0x04, 0x00}, 0x00, 0x02, {0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00}, (struct fw_update_usb_config_bytes *)&fw_update_usb_config_attributes_fs.max_power_endpoints[2]}
};
/* high-speed configuration descriptor (bulk endpoints of 512 bytes; 9 bytes + 1 pad), split at
   bmAttributes (fw_update_usb_config_attributes_hs, read by fw_update_usb_requests.c by that name), then two endpoint descriptors
   (struct fw_update_usb_endpoint_entry, 8 bytes each: 7 + 1 pad) and 2 pad bytes */
const struct fw_update_usb_config_set fw_update_usb_config_hs = {
    0x09, 0x02, 0x20, {0x00, 0x01, 0x01, 0x00}
};
const struct fw_update_usb_config_view fw_update_usb_config_attributes_hs = {
    0xc0, {0x31, 0x00, 0x07, 0x05, 0x81, 0x02, 0x00, 0x02, 0xff, 0x00, 0x07,
    0x05, 0x02, 0x02, 0x00, 0x02, 0xff, 0x00, 0x00, 0x00}
};
/* interface descriptor (9 bytes + 3 pad) with a pointer to its endpoints */
const struct fw_update_usb_interface_view fw_update_usb_interface_hs[1] = {
    {0x09, {0x04, 0x00}, 0x00, 0x02, {0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00}, (struct fw_update_usb_config_bytes *)&fw_update_usb_config_attributes_hs.max_power_endpoints[2]}
};
/* standard request handlers, indexed by bRequest (0..12) */
unsigned int (*const fw_update_usb_std_request_table[13])(void) = {
    fw_update_usb_req_get_status, fw_update_usb_req_clear_feature, fw_update_usb_req_unsupported, fw_update_usb_req_set_feature,
    fw_update_usb_req_unsupported, fw_update_usb_req_set_address, fw_update_usb_req_get_descriptor, fw_update_usb_req_unsupported,
    fw_update_usb_req_get_configuration, fw_update_usb_req_set_configuration, fw_update_usb_req_get_interface, fw_update_usb_req_set_interface,
    fw_update_usb_req_unsupported
};
