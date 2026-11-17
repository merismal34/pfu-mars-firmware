#include "fw.h"
extern void _builtin_set_imask(int);

int xfer_compute_length(void)
{
    unsigned available;
    unsigned chunk_len;
    struct page_record *current_page;

    current_page = &page_records[page_current.v];
    _builtin_set_imask(15);
    if (current_page->sides[scsi_read_side.v].flags.f.requested == 0) {
        _builtin_set_imask(scsi_saved_imask);
        if (err_report_page_error() == 0) {
            scsi_status = SCSI_STEP_CHECK_CONDITION;
            return 0;
        }
        return 12;
    } else if (scsi_xfer_len.v == 0)
        return 10;
    if (current_page->sides[scsi_read_side.v].flags.f.data_ready == 0) {
        _builtin_set_imask(scsi_saved_imask);
        return XFER_STATE_WAIT_DATA;
    }
    if ((win_params.side[scsi_read_side.v].compression.f.type & 0x80) != 0)
        available = current_page->sides[scsi_read_side.v].jpeg_bytes - current_page->sides[scsi_read_side.v].jpeg_bytes_sent;
    else
        available = current_page->sides[scsi_read_side.v].image_bytes - current_page->sides[scsi_read_side.v].image_bytes_sent;
    if (available < scsi_xfer_len.v) {
        if ((win_params.side[scsi_read_side.v].compression.f.type & 0x80) != 0) {
            if (current_page->sides[scsi_read_side.v].jpeg_done != 0) {
                scsi_xfer_len.v = available;
                xfer_last_chunk = 1;
                current_page->sides[scsi_read_side.v].flags.f.last_chunk = 1;
            } else {
                _builtin_set_imask(scsi_saved_imask);
                return XFER_STATE_WAIT_DATA;
            }
        } else {
            if (current_page->sides[scsi_read_side.v].flags.f.scanned != 0) {
                scsi_xfer_len.v = available;
                xfer_last_chunk = 1;
                current_page->sides[scsi_read_side.v].flags.f.last_chunk = 1;
            } else {
                _builtin_set_imask(scsi_saved_imask);
                return XFER_STATE_WAIT_DATA;
            }
        }
    }
    _builtin_set_imask(scsi_saved_imask);
    if (hw_hostif_flags.f.usb != 0)
        usb_read_chunk_len = scsi_xfer_len.v;
    if (scsi_read_sides[scsi_read_side.v].data_due != 0 && (scsi_identify_message & 0x40) != 0)
        return XFER_STATE_WAIT_DATA;
    if (xfer_last_chunk != 0)
        return XFER_STATE_START_DMA;
    while (1) {
        chunk_len = available;
        if ((win_params.side[scsi_read_side.v].compression.f.type & 0x80) != 0) {
            if (current_page->sides[scsi_read_side.v].jpeg_done != 0)
                current_page->sides[scsi_read_side.v].flags.f.last_chunk = 1;
            else
                chunk_len = chunk_len & 0xffff0000;
        } else {
            if (current_page->sides[scsi_read_side.v].flags.f.scanned != 0)
                current_page->sides[scsi_read_side.v].flags.f.last_chunk = 1;
            else
                chunk_len = chunk_len & 0xffff0000;
        }
        break;
    }
    if (chunk_len <= scsi_param_len.v) {
        if (chunk_len < scsi_xfer_len.v)
            current_page->sides[scsi_read_side.v].flags.f.last_chunk = 0;
        else
            scsi_xfer_len.v = chunk_len;
    } else {
        current_page->sides[scsi_read_side.v].flags.f.last_chunk = 0;
        scsi_xfer_len.v = scsi_param_len.v;
    }
    if (hw_hostif_flags.f.usb != 0)
        usb_read_chunk_len = scsi_xfer_len.v;
    return XFER_STATE_START_DMA;
}
