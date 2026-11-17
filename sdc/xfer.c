#include "fw.h"


int xfer_init(void)
{
    struct page_record *current_page;
    unsigned int len;
    current_page = &page_records[page_current.v];
    xfer_last_chunk = 0;
    scsi_read_sides[0].data_due = scsi_read_sides[1].data_due = 0;
    if (current_page->sides[scsi_read_side.v].flags.f.last_chunk) {
        return 12;
    }
    if (scsi_page_read_done != 0) {
        return 12;
    }
    if ((scsi_identify_message & 0x40) == 0) {
        if ((jpeg_scan_flags.v & JPEG_SCAN_LONG_PAGE) != 0 || win_params.side[scsi_read_side.v].compression.f.type & 0x80) {
            len = img_sdram_bytes[0] >> 1;
            if (len > scsi_param_len.v) {
                len = scsi_param_len.v;
                xfer_last_chunk = 1;
            }
            scsi_xfer_len.v = len;
        } else {
            len = scsi_param_len.v;
            xfer_last_chunk = 1;
            scsi_xfer_len.v = len;
        }
    } else {
        len = xfer_chunk_max;
        if (len > scsi_param_len.v) {
            len = scsi_param_len.v;
            xfer_last_chunk = 1;
        }
        scsi_xfer_len.v = len;
    }
    if (hw_hostif_flags.f.usb) {
        usb_read_chunk_len = len;
    }
    return XFER_STATE_COMPUTE_LENGTH;
}
