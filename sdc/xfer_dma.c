#include "fw.h"
extern void _builtin_set_imask(int);
extern void _builtin_set_gbr(void *);
struct scsi_regs_xfer_dma {
    char pad_00[35];
    struct Bits fifoctl;
    char pad_24[11];
    struct Bits dmactl;
    char pad_30[3];
    unsigned char dtbc2;
    char pad_34;
    unsigned char dtbc1;
    char pad_36;
    unsigned char dtbc0;
    char pad_38[1];
    unsigned char config0;
};
struct usb_regs_xfer {
    char reg_00_1e[63];
    unsigned char usb_bank;
    char reg_20_21[5];
    struct Bits reg_22;
    char pad_46;
    struct Bits reg_23;
    char pad_48;
    unsigned char reg_24;
    char pad_4a;
    unsigned char reg_25;
};


int xfer_start_dma(void)
{
    struct page_record *p;
    p = &page_records[page_current.v];
    if (scsi_xfer_len.v == 0) {
        return 10;
    }
    if (hw_hostif_flags.f.usb == 0) {
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct scsi_regs_xfer_dma *)scsi_ctrl)->config0 = 0xa0;
        _builtin_set_imask(scsi_saved_imask);
        scsi_set_sync_mode();
    }
    _builtin_set_imask(15);
    if (hw_hostif_flags.f.usb == 0) {
        if (scsi_read_side.s == 0) {
            p_pisces_hostif[1]->ctrl.f.b0 = 1;
            p_pisces_hostif[1]->ctrl.f.b9 = 0;
            p_pisces_hostif[0]->ctrl.f.b9 = 1;
            p_pisces_hostif[0]->ctrl.f.b0 = 0;
        } else {
            p_pisces_hostif[0]->ctrl.f.b0 = 1;
            p_pisces_hostif[0]->ctrl.f.b9 = 0;
            p_pisces_hostif[1]->ctrl.f.b9 = 1;
            p_pisces_hostif[1]->ctrl.f.b0 = 0;
        }
        _builtin_set_gbr(0);
        ((struct scsi_regs_xfer_dma *)scsi_ctrl)->dmactl.b1 = 0;
        _builtin_set_gbr(0);
        ((struct scsi_regs_xfer_dma *)scsi_ctrl)->fifoctl.b2 = 1;
        _builtin_set_gbr(0);
        ((struct scsi_regs_xfer_dma *)scsi_ctrl)->dtbc2 = scsi_xfer_len.v >> 16;
        _builtin_set_gbr(0);
        ((struct scsi_regs_xfer_dma *)scsi_ctrl)->dtbc1 = (scsi_xfer_len.v & 0xff00) >> 8;
        _builtin_set_gbr(0);
        ((struct scsi_regs_xfer_dma *)scsi_ctrl)->dtbc0 = scsi_xfer_len.c.lo;
        scsi_ctrl_command(21);
        _builtin_set_imask(15);
        _builtin_set_gbr(0);
        ((struct scsi_regs_xfer_dma *)scsi_ctrl)->dmactl.b0 = 1;
        goto L1;
    }
    _builtin_set_imask(scsi_saved_imask);
    if (usb_event_flags.f.suspend) {
        return 0;
    }
    _builtin_set_imask(15);
    _builtin_set_gbr(0);
    /* USB controller bank 1 (+0x45..+0x4B): the DMA/CPU interface set-up */
    ((struct usb_regs_xfer *)usb_ctrl)->usb_bank = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_22.b7 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_22.b3 = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_22.b2 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_22.b1 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_23.b7 = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_23.b6 = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_23.b5 = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_23.b3 = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_23.b2 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_23.b1 = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_23.b0 = 1;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_24 = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->reg_25 = 0;
    _builtin_set_gbr(0);
    ((struct usb_regs_xfer *)usb_ctrl)->usb_bank = 0;
    _builtin_set_imask(scsi_saved_imask);
    usb_wake_events.f.dma_started = 1;
    usb_bulk_out_ptr = 0;
    if (scsi_xfer_len.v >= scsi_param_len.v) {
        usb_set_transfer_len(scsi_xfer_len.v);
        usb_transfer_flags.pisces_in = 1;
        usb_start_transfer();
        p_pisces_hostif[scsi_read_side.s]->ctrl.f.b10 = 0;
    } else {
        usb_set_transfer_len(scsi_param_len.v);
        usb_transfer_flags.pisces_in = 1;
        p_pisces_hostif[scsi_read_side.s]->ctrl.f.b10 = 1;
        usb_start_transfer();
        p_pisces_hostif[scsi_read_side.s]->ctrl.f.b10 = 0;
    }
    _builtin_set_imask(scsi_saved_imask);
    return xfer_state_11();
L1:
    scsi_defer_enable_irq();
    _builtin_set_imask(scsi_saved_imask);
    return 11;
}

int xfer_state_10(void)
{
    int len;
    struct page_record *current_page;
    current_page = &page_records[page_current.v];
    if (err_report_page_error() == 0) {
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    }
    scsi_message_retries.v = 0;
    _builtin_set_imask(15);
    len = scsi_xfer_len.v;
    if (hw_hostif_flags.f.usb) {
    }
    if (win_params.side[scsi_read_side.v].compression.f.type & 0x80) {
        current_page->sides[scsi_read_side.v].jpeg_bytes_sent += len;
    } else {
        current_page->sides[scsi_read_side.v].image_bytes_sent += len;
    }
    xfer_chunk_bytes += len;
    scsi_param_len.v -= len;
    if (scsi_param_len.v != 0) {
        scsi_read_sides[scsi_read_side.v].data_due = 1;
        _builtin_set_imask(scsi_saved_imask);
        return XFER_STATE_INIT;
    }
    if (current_page->sides[scsi_read_side.v].flags.f.last_chunk) {
        _builtin_set_imask(scsi_saved_imask);
        return XFER_STATE_INIT;
    }
    if (current_page->sides[scsi_read_side.v].flags.f.scanned == 0) {
        scsi_status = SCSI_STEP_GOOD;
        _builtin_set_imask(scsi_saved_imask);
        return 0;
    } else {
        _builtin_set_imask(15);
    }
    if (win_params.side[scsi_read_side.v].compression.f.type & 0x80) {
        if (current_page->sides[scsi_read_side.v].jpeg_bytes == current_page->sides[scsi_read_side.v].jpeg_bytes_sent && current_page->sides[scsi_read_side.v].jpeg_done != 0) {
            _builtin_set_imask(scsi_saved_imask);
            current_page->sides[scsi_read_side.v].flags.f.last_chunk = 1;
            return XFER_STATE_INIT;
        } else {
            _builtin_set_imask(scsi_saved_imask);
            scsi_status = SCSI_STEP_GOOD;
            return 0;
        }
    } else {
        if (current_page->sides[scsi_read_side.v].image_bytes == current_page->sides[scsi_read_side.v].image_bytes_sent && current_page->sides[scsi_read_side.v].flags.f.scanned) {
            _builtin_set_imask(scsi_saved_imask);
            current_page->sides[scsi_read_side.v].flags.f.last_chunk = 1;
            return XFER_STATE_INIT;
        } else {
            _builtin_set_imask(scsi_saved_imask);
            scsi_status = SCSI_STEP_GOOD;
            return 0;
        }
    }
    _builtin_set_imask(scsi_saved_imask);
}

int xfer_state_11(void)
{
    int x;
    scsi_read_sides[scsi_read_side.v].flags.b.lo.f.b5 = 0;
    if (scsi_check_parity_error() == 0) {
        return 0;
    }
    if (scsi_run_messages(1) == 0) {
        if (scsi_status != 0) {
            return 0;
        } else {
            return 8;
        }
    } else {
        return 10;
    }
}


int xfer_state_12(void)
{
    struct page_record *current_page;
    int saved_buf_offset;
    short saved_front_width, saved_front_length, saved_back_width, saved_back_length;
    current_page = &page_records[page_current.v];
    _builtin_set_imask(15);
    if (scsi_page_read_done != 0) {
        if (scsi_param_len.v == 0) {
            scsi_status = SCSI_STEP_GOOD;
        } else {
            if (hw_hostif_flags.f.usb) {
                usb_set_transfer_len(scsi_param_len.v);
                usb_transfer_flags.pisces_in = 1;
                p_pisces_hostif[scsi_read_side.s]->ctrl.f.b10 = 1;
                _builtin_set_imask(scsi_saved_imask);
                usb_start_transfer();
                _builtin_set_imask(15);
                p_pisces_hostif[scsi_read_side.s]->ctrl.f.b10 = 0;
                _builtin_set_imask(scsi_saved_imask);
            }
            scsi_set_good();
            scsi_status = SCSI_STEP_CHECK_CONDITION;
        }
        _builtin_set_imask(scsi_saved_imask);
        return 0;
    }
    if (current_page->sides[scsi_read_side.v].flags.f.requested == 0) {
        if (scsi_param_len.v == 0) {
            scsi_status = SCSI_STEP_GOOD;
        } else {
            if (err_report_page_error() == 0) {
                if (hw_hostif_flags.f.usb) {
                    usb_send_reply(usb_read_len);
                }
                scsi_status = SCSI_STEP_CHECK_CONDITION;
                return 0;
            }
            if (hw_hostif_flags.f.usb) {
                usb_set_transfer_len(scsi_param_len.v);
                usb_transfer_flags.pisces_in = 1;
                p_pisces_hostif[scsi_read_side.s]->ctrl.f.b10 = 1;
                _builtin_set_imask(scsi_saved_imask);
                usb_start_transfer();
                _builtin_set_imask(15);
                p_pisces_hostif[scsi_read_side.s]->ctrl.f.b10 = 0;
                _builtin_set_imask(scsi_saved_imask);
            }
            scsi_set_good();
            scsi_status = SCSI_STEP_CHECK_CONDITION;
        }
        _builtin_set_imask(scsi_saved_imask);
        return 0;
    } else {
        current_page->sides[scsi_read_side.v].flags.f.requested = 0;
        if (scsi_read_side.v == 0) {
            scsi_read_progress.f.b1 = 1;
        } else {
            scsi_read_progress.f.b0 = 1;
        }
        if (scsi_param_len.v == 0) {
            scsi_status = SCSI_STEP_GOOD;
        } else {
            scsi_set_good();
            scsi_status = SCSI_STEP_CHECK_CONDITION;
        }
        _builtin_set_imask(15);
        if (job_auto_size_flags.b7) {
          if (scsi_last_page_size[scsi_read_side.v].size_final == 0) {
            current_page->sides[scsi_read_side.v].width_px = (short)p_pisces_output[scsi_read_side.v]->width_px;
            current_page->sides[scsi_read_side.v].length_lines = (short)p_pisces_output[scsi_read_side.v]->reg_0d4;
            current_page->sides[scsi_read_side.v].size_final = 1;
          }
        }
        scsi_last_page_size[scsi_read_side.v].size_final = current_page->sides[scsi_read_side.v].size_final;
        if (current_page->sides[scsi_read_side.v].size_final != 0 && current_page->sides[scsi_read_side.v].paper_width_px != 0 && current_page->sides[scsi_read_side.v].paper_length_lines != 0) {
            scsi_last_page_size[scsi_read_side.v].width_px = current_page->sides[scsi_read_side.v].paper_width_px;
            scsi_last_page_size[scsi_read_side.v].length_lines = current_page->sides[scsi_read_side.v].paper_length_lines;
        } else {
            scsi_last_page_size[scsi_read_side.v].width_px = current_page->sides[scsi_read_side.v].width_px;
            scsi_last_page_size[scsi_read_side.v].length_lines = current_page->sides[scsi_read_side.v].length_lines;
        }
        if (current_page->sides[scsi_read_side.v].width_unaligned_px != 0) {
            scsi_last_page_size[scsi_read_side.v].width_unaligned_px = current_page->sides[scsi_read_side.v].width_unaligned_px;
        } else {
            scsi_last_page_size[scsi_read_side.v].width_unaligned_px = current_page->sides[scsi_read_side.v].width_px;
        }
        if (current_page->sides[scsi_read_side.v].length_unpadded_lines != 0) {
            scsi_last_page_size[scsi_read_side.v].length_unpadded_lines = current_page->sides[scsi_read_side.v].length_unpadded_lines;
        } else {
            scsi_last_page_size[scsi_read_side.v].length_unpadded_lines = current_page->sides[scsi_read_side.v].length_lines;
        }
        scsi_last_page_driver_crop = current_page->driver_crop;
        if (!current_page->sides[0].flags.f.requested && !current_page->sides[1].flags.f.requested) {
            scsi_scan_requested = 0;
            _builtin_set_imask(15);
            if (current_page->multifeed_flags.f.length) {
                err_multifeed_reported = 1;
                err_multifeed_code = 0x56;
            } else if (current_page->multifeed_flags.f.ultrasonic) {
                err_multifeed_reported = 1;
                err_multifeed_code = 0x58;
            } else {
                err_multifeed_reported = 0;
                err_multifeed_code = 0;
            }
            current_page->flags.f.sent = 1;
            saved_buf_offset = current_page->buf_offset;
            saved_front_width = current_page->sides[0].width_px;
            saved_front_length = current_page->sides[0].length_lines;
            saved_back_width = current_page->sides[1].width_px;
            saved_back_length = current_page->sides[1].length_lines;
            util_mem_clear(current_page, 0x88);
            current_page->buf_offset = saved_buf_offset;
            current_page->sides[0].width_px = saved_front_width;
            current_page->sides[0].length_lines = saved_front_length;
            current_page->sides[1].width_px = saved_back_width;
            current_page->sides[1].length_lines = saved_back_length;
            if (page_current.v == page_record_ring - 1 || page_buf_count.v == 0) {
                page_current.v = 0;
            } else {
                page_current.v += 1;
            }
            page_sent_count.v += 1;
            scsi_page_read_done = 1;
            _builtin_set_imask(scsi_saved_imask);
        }
        _builtin_set_imask(scsi_saved_imask);
        return 0;
    }
}
