#include "fw.h"
#pragma section H

void scsi_run_cmd_state(void)
{
    unsigned char state;
    state = scsi_cmd_state;
    scsi_cmd_state = 0;
    while (1) {
        state = scsi_cmd_state_table[state]();
        if (state == 0) {
            break;
        }
    }
}

int scsi_cmd_state_0(void)
{
    scsi_reselect_retries.v = 0;
    return 2;
}
