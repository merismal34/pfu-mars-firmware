#include "fw.h"
int xfer_state_6(void)
{
    if (scsi_check_reselection() == 0) {
        if (scsi_status != 0) {
            return 0;
        } else {
            return 5;
        }
    } else {
        return 7;
    }
}


int xfer_state_7(void)
{
    if (err_report_page_error() == 0) {
        if (hw_hostif_flags.f.usb != 0) {
            usb_send_reply(usb_read_len);
        }
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return 0;
    } else {
        return XFER_STATE_COMPUTE_LENGTH;
    }
}


int xfer_state_8(void)
{
    if (xfer_chunk_bytes != 0) {
        scsi_set_sense(SCSI_SENSE_ABORTED_COMMAND, SCSI_ASC_INITIATOR_DETECTED_ERROR);
        scsi_status = SCSI_STEP_CHECK_CONDITION;
        return;
    } else if (scsi_finish_command() == 0) {
        if (((char *)&scsi_status)[0]) {
        } else {
            scsi_status = SCSI_STEP_CHECK_CONDITION;
        }
        return 0;
    } else {
        xfer_chunk_bytes = 0;
        return XFER_STATE_START_DMA;
    }
}
