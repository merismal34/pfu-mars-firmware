#include "fw.h"
#pragma section H
extern void _builtin_set_gbr(void *);

struct scsi_reg_fifoctl { char pad_00[35]; struct Bits fifoctl; };

int scsi_cmd_state_2(void)
{
    if (scsi_reselect_pending() == 0) {
        return 3;
    }
    else if (scsi_run_messages(1) == 0 && scsi_status != 0) {
        return 0;
    }
    return 4;
}

int scsi_cmd_state_3(void)
{
    if (scsi_check_reselection() == 0) {
        if (scsi_status) {
            return 0;
        } else {
            return 2;
        }
    } else {
        return 4;
    }
}

int scsi_cmd_state_4(void)
{
    unsigned short *p = scsi_reply_buf;
    unsigned short *q = scsi_reply_buf;
    if (page_size_reply_pending != 0) {
        page_size_reply_pending = 0;
        p[5] = page_size_paper_width_px;
        p[7] = page_size_paper_length_lines;
        p[12] = page_size_word_24;
        p[14] = page_size_word_28;
        if (job_auto_size_flags.b7) {
            if (page_size_driver_crop) {
                p[8] = 0x8100;
            } else {
                p[8] = 0x100;
            }
        } else {
            p[8] = 0;
        }
        scsi_send_reply();
    }
    scsi_status = SCSI_STEP_GOOD;
    return 0;
}

int scsi_cmd_state_5(void)
{
    scsi_diag_request = 30;
    scsi_reselect_retries.v = 0;
    return 2;
}

int scsi_cmd_state_6(void)
{
    scsi_reselect_retries.v = 0;
    return 2;
}

void scsi_cmd_set_subwindow(void)
{
    unsigned short len;
    char reserved_bits;
    if (scsi_check_ready() == 0) {
        return;
    }
    reserved_bits = scsi_cdb_byte_2[0] | scsi_cdb_byte_3[0] | scsi_cdb_byte_4[0] | scsi_cdb_byte_5[0] | scsi_cdb_byte_6[0] | scsi_cdb_byte_9[0];
    if ((scsi_cdb_byte_1[0] & ~0xe0) || reserved_bits != 0) {
        scsi_sense_invalid_cdb();
        return;
    }
    len = scsi_cdb_byte_8[0].v | (scsi_cdb_byte_7[0].v << 8);
    scsi_param_len.v = len;
    if (len == 0) {
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    if (len < 0x30 || len > 0x240) {
        scsi_sense_invalid_cdb();
        return;
    }
    if (scsi_receive_params() == 0) {
        return;
    }
    scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_WINDOW_COMBINATION);
    scsi_status = SCSI_STEP_CHECK_CONDITION;
    return;
}

void scsi_cmd_get_data_buffer_status(void)
{
    int u;
    if (scsi_check_ready() == 0) {
        return;
    }
    scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_WINDOW_COMBINATION);
    scsi_status = SCSI_STEP_CHECK_CONDITION;
    return;
}

void scsi_cmd_get_window(void)
{
    if (scsi_check_ready_2() == 0) {
        return;
    }
    scsi_sense_invalid_cdb();
    return;
}

void scsi_unused_set_ctl_bit2(void)
{
    _builtin_set_gbr(0);
    ((struct scsi_reg_fifoctl *)scsi_ctrl)->fifoctl.b2 = 1;
    scsi_enable_irq();
}

void scsi_unused_hook(void)
{
}

void scsi_run_end_state(void)
{
    unsigned char state;
    state = scsi_end_state;
    scsi_end_state = 0;
    while (1) {
        state = scsi_end_state_table[state]();
        if (state == 0) {
            break;
        }
    }
}

int scsi_end_state_0(void)
{
    scsi_unusual_paper_state |= 1;
    return 4;
}

int scsi_end_state_2(void)
{
    return 4;
}

int scsi_end_state_3(void)
{
    return 4;
}

int scsi_end_state_4(void)
{
    scsi_reselect_retries.v = 0;
    return 5;
}

int scsi_end_state_5(void)
{
    if (scsi_reselect_pending() == 0) {
        return 6;
    }
    else if (scsi_run_messages(1) == 0 && scsi_status != 0) {
        return 0;
    }
    return 7;
}

int scsi_end_state_6(void)
{
    if (scsi_check_reselection() == 0) {
        if (scsi_status) {
            return 0;
        } else {
            return 5;
        }
    } else {
        return 7;
    }
}

int scsi_end_state_7(void)
{
    unsigned char flags;
    if (err_report_page_error() == 0) {
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    }
    if ((job_mode_params[0].job_separation[0] & 0x80) == 0) {
        scsi_status = SCSI_STEP_GOOD;
    } else {
        flags = job_separation_sheet_flags;
        flags &= 0xc0;
        if (flags != 0xc0) {
            scsi_status = SCSI_STEP_GOOD;
        } else {
            flags = scsi_unusual_paper_state;
            if ((flags & 1) == 0) {
                scsi_status = SCSI_STEP_GOOD;
            } else if (flags & 0x80) {
                scsi_status = SCSI_STEP_GOOD;
            } else {
                scsi_unusual_paper_state |= 0x80;
                scsi_set_sense(SCSI_SENSE_MEDIUM_ERROR, SCSI_ASC_MEDIUM_UNUSUAL_PAPER);
                scsi_status = SCSI_STEP_CHECK_CONDITION;
            }
        }
    }
    return 0;
}

extern void _builtin_set_imask(int);


int scsi_cmd_read_step_7(void)
{
    struct page_record *current_page;
    unsigned short *reply;
    int pad8[2];
    unsigned short width_px;
    unsigned short length_lines;
    int driver_crop;
    int flag;

    current_page = &page_records[page_current.v];
    flag = 0;
    log_add_record(2, 0x28, scsi_read_side.c.lo | 0x80, 0);
    scsi_message_retries.v = 0;
    if (scsi_read_sides[scsi_read_side.v].window_set == 0) {
        if (hw_hostif_flags.f.usb != 0) {
            usb_send_reply(scsi_param_len.v);
        }
        scsi_set_sense(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_WINDOW_COMBINATION);
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return;
    }
    if (scsi_param_len.v > 0x28 && !hw_hostif_flags.f.usb) {
        scsi_param_len.v = 0x28;
    }
    if (scsi_param_len.v == 0) {
        if (hw_hostif_flags.f.usb != 0) {
            usb_start_data_in();
        }
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    reply = scsi_reply_buf;
    util_mem_clear(reply, 0x28);
    if (main_page_loop_phase == 1) {
        img_compute_window_size(scsi_read_side.v, 0, (int)&width_px, (int)&length_lines);
    } else if (page_scanned_count.v == 0 && (page_records[0].flags.v & PAGE_FLAGS_ACTIVE) == 0) {
        img_compute_window_size(scsi_read_side.v, 0, (int)&width_px, (int)&length_lines);
    } else {
        width_px = *(short *)(((char *)img_side_params) + scsi_read_side.v * 40);
        length_lines = img_side_params[scsi_read_side.v].output_length_lines;
    }
    reply[1] = width_px;
    reply[3] = length_lines;
    reply[12] = width_px;
    reply[14] = length_lines;
    if (win_params.paper_size.f.auto_length || job_auto_size_flags.b7) {
        if (current_page->sides[scsi_read_side.v].size_final == 0 && scsi_last_page_size[scsi_read_side.v].size_final == 0) {
            if (current_page->sides[scsi_read_side.v].flags.f.requested == 0) {
                reply[5] = width_px;
                reply[7] = length_lines;
                reply[12] = width_px;
                reply[14] = length_lines;
            } else {
                _builtin_set_imask(15);
                page_size_wanted = 1;
                scsi_read_pixel_size_flag = 0x80;
                page_size_side = scsi_read_side.v;
                _builtin_set_imask(scsi_saved_imask);
                if (scsi_disconnect() == 0) {
                    if (scsi_status) {
                        return;
                    } else {
                        scsi_resume_flags.f.resume_pending = 1;
                        scsi_cmd_state = 1;
                        scsi_defer_enable_irq();
                        return;
                    }
                } else {
                    scsi_resume_flags.f.resume_pending = 1;
                    scsi_cmd_state = 1;
                    scsi_defer_enable_irq();
                    return;
                }
            }
        } else if (current_page->sides[scsi_read_side.v].size_final != 0 && scsi_last_page_size[scsi_read_side.v].size_final == 0) {
            if (current_page->sides[scsi_read_side.v].paper_width_px != 0 && current_page->sides[scsi_read_side.v].paper_length_lines != 0) {
                reply[5] = current_page->sides[scsi_read_side.v].paper_width_px;
                reply[7] = current_page->sides[scsi_read_side.v].paper_length_lines;
            } else {
                reply[5] = current_page->sides[scsi_read_side.v].width_px;
                reply[7] = current_page->sides[scsi_read_side.v].length_lines;
            }
            if (current_page->sides[scsi_read_side.v].width_unaligned_px != 0) {
                reply[12] = current_page->sides[scsi_read_side.v].width_unaligned_px;
            } else {
                reply[12] = current_page->sides[scsi_read_side.v].width_px;
            }
            if (current_page->sides[scsi_read_side.v].length_unpadded_lines != 0) {
                reply[14] = current_page->sides[scsi_read_side.v].length_unpadded_lines;
            } else {
                reply[14] = current_page->sides[scsi_read_side.v].length_lines;
            }
            flag = 1;
            driver_crop = current_page->driver_crop;
        } else {
            reply[5] = scsi_last_page_size[scsi_read_side.v].width_px;
            reply[7] = scsi_last_page_size[scsi_read_side.v].length_lines;
            reply[12] = scsi_last_page_size[scsi_read_side.v].width_unaligned_px;
            reply[14] = scsi_last_page_size[scsi_read_side.v].length_unpadded_lines;
            flag = 1;
            driver_crop = scsi_last_page_driver_crop;
        }
    }
    if (job_auto_size_flags.b7 && flag == 1) {
        if (driver_crop != 0) {
            reply[8] = 0x8100;
        } else {
            reply[8] = 0x100;
        }
    } else {
        reply[8] = 0;
    }
    return scsi_send_reply();
}

int scsi_cmd_read_step_8(void)
{
    unsigned char *p;
    unsigned short unused_frame;
    unsigned char unused_frame_2;
    unsigned char flags;

    scsi_message_retries.v = 0;
    if (scsi_param_len.v > 8 && !hw_hostif_flags.f.usb) {
        scsi_param_len.v = 8;
    }
    if (scsi_param_len.v == 0) {
        if (hw_hostif_flags.f.usb != 0) {
            usb_start_data_in();
        }
        scsi_status = SCSI_STEP_GOOD;
        return;
    }
    p = (unsigned char *)scsi_reply_buf;
    util_mem_clear(p, 8);
    flags = job_separation_sheet_flags;
    flags &= 0xc0;
    if (flags == 0xc0) {
        p[2] = 0x80;
    }
    p[3] = job_read_8_byte_3;
    return scsi_send_reply();
}
