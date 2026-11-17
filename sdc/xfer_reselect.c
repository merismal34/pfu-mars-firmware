#include "fw.h"

int xfer_state_5(void)
{
    if (scsi_reselect_pending() == 0) {
        return 6;
    } else if (scsi_run_messages(1) == 0) {
        if (scsi_status != 0) {
            return 0;
        }
        return 8;
    }
    return 7;
}
