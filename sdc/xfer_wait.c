#include "fw.h"
extern void _builtin_set_imask(int);

int xfer_wait_data(void)
{
    struct page_record *current_page;
    int x;

    current_page = &page_records[page_current.v];
    if (job_interlace_read != 0) {
        if (hw_hostif_flags.f.usb != 0) {
            if ((jpeg_scan_flags.v & JPEG_SCAN_PAUSED) == 0) {
                _builtin_set_imask(15);
                current_page->flags.f.read_pending = 1;
                _builtin_set_imask(scsi_saved_imask);
                scsi_defer_disable_irq();
                return 4;
            } else {
                usb_send_reply(scsi_param_len.v);
            }
        } else {
            if ((jpeg_scan_flags.v & JPEG_SCAN_LONG_PAGE) == 0 && img_side_params[0].width_px == img_side_params[1].width_px && img_side_params[0].output_length_lines == img_side_params[1].output_length_lines) {
                _builtin_set_imask(15);
                current_page->flags.f.read_pending = 1;
                _builtin_set_imask(scsi_saved_imask);
                scsi_defer_disable_irq();
                return 4;
            }
        }
        scsi_set_sense(SCSI_SENSE_MEDIUM_ERROR, SCSI_ASC_MEDIUM_TEMPORARY_NO_DATA);
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    }
    if ((scsi_identify_message & 0x40) == 0) {
        if (scsi_read_sides[scsi_read_side.v].data_due == 0) {
            _builtin_set_imask(15);
            current_page->flags.f.read_pending = 1;
            _builtin_set_imask(scsi_saved_imask);
            scsi_defer_disable_irq();
            return 4;
        } else {
            _builtin_set_imask(15);
            current_page->flags.f.read_pending = 1;
            _builtin_set_imask(scsi_saved_imask);
            scsi_defer_disable_irq();
            return 4;
        }
    } else {
        if (scsi_read_sides[scsi_read_side.v].data_due != 0) {
            scsi_message_retries.v = 0;
            while (1) {
                scsi_reply_message.v = 2;
                scsi_send_message_byte();
                if (scsi_run_messages(0) == 0) {
                    if (scsi_status != 0)
                        return 0;
                } else {
                    break;
                }
            }
            if (scsi_disconnect_allowed == 0) {
                scsi_initiator_bit.b.b6 = 0;
                return XFER_STATE_COMPUTE_LENGTH;
            } else {
                scsi_read_sides[scsi_read_side.v].data_due = 0;
                xfer_chunk_bytes = 0;
            }
        }
        if (scsi_disconnect() == 0) {
            if (scsi_status != 0)
                return 0;
            else if (hw_hostif_flags.f.usb != 0) {
            } else {
                scsi_initiator_bit.b.b6 = 0;
                return XFER_STATE_COMPUTE_LENGTH;
            }
        }
        _builtin_set_imask(15);
        current_page->flags.f.read_pending = 1;
        _builtin_set_imask(scsi_saved_imask);
        scsi_defer_enable_irq();
        return 4;
    }
}

int xfer_state_4(void)
{
    scsi_reselect_retries.v = 0;
    return 5;
}

