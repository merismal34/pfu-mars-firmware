#include "fw.h"
#pragma section H
int mdc_is_paper_path_clear(void)
{
    if (mdc_mailbox.sensor_status.f.top || mdc_mailbox.sensor_status.f.pick || mdc_mailbox.sensor_status_2.f.df)
        return 0;
    return 1;
}
