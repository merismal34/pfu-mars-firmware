#include "fw.h"
#pragma section H


void scsi_win_inch1200_to_px16(int length, unsigned short dpi)
{
    unsigned int pixels;
    pixels = rt_divu32(length * dpi, 1200);
    if (pixels > 65535) {
        scsi_win_px16_valid = 0;
    } else {
        scsi_win_px16_valid = 1;
        scsi_win_px16.v = pixels;
    }
}
