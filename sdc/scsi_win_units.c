#include "fw.h"
#pragma section H


/* Converts a window field from the MODE SELECT measurement unit (job_mode_params[0]) to 1/1200 inch:
   result in scsi_win_inch1200, scsi_win_inch1200_valid = 0 when it does not fit in 16 bits. */
void scsi_win_units_to_inch1200(unsigned int value, unsigned short dpi)
{
    unsigned int multiplier;
    unsigned int divisor;
    unsigned int high;
    unsigned int low;
    unsigned int result;
    unsigned short quotient;
    unsigned short bit;

    if (value == 0) {
        scsi_win_inch1200_valid = 1;
        scsi_win_inch1200.v = value;
        return;
    }
    if (job_mode_params[0].unit_divisor == 0) {
        scsi_win_inch1200_valid = 0;
        scsi_win_inch1200.v = value;
        return;
    }
    switch (job_mode_params[0].measurement_unit) {
    case 0:
        if (job_mode_params[0].unit_divisor == 0x4b0) {
            scsi_win_inch1200_valid = 1;
            scsi_win_inch1200.v = value;
            return;
        } else {
            multiplier = 0x4b0;
            divisor = job_mode_params[0].unit_divisor;
        }
        break;
    case 1:
        multiplier = 12000;
        divisor = job_mode_params[0].unit_divisor * 0xfe;
        break;
    default:
        multiplier = 0x4b0;
        divisor = job_mode_params[0].unit_divisor * dpi;
        break;
    }
    if (value < 0x57619) {
        result = rt_divu32(value * multiplier, divisor);
        if (result <= 0xffff) {
            scsi_win_inch1200_valid = 1;
            scsi_win_inch1200.v = result;
            return;
        } else {
            scsi_win_inch1200_valid = 0;
            scsi_win_inch1200.v = result;
        }
    } else {
        high = rt_divu32(value, 0x10000);
        low = value & 0xffff;
        high *= multiplier;
        low *= multiplier;
        multiplier = rt_divu32(low, 0x10000);
        low = low & 0xffff;
        high = multiplier = high + multiplier;
        low += (multiplier & 0xffff) << 4;
        high >>= 4;
        if (high >= divisor) {
            scsi_win_inch1200_valid = 0;
            return;
        }
        quotient = 0;
        for (bit = 0x8000; bit >= 1; bit >>= 1) {
            high <<= 1;
            if (low & 0x80000000) {
                high++;
            }
            low <<= 1;
            if (high >= divisor) {
                quotient |= bit;
                high -= divisor;
            }
        }
        scsi_win_inch1200_valid = 1;
        scsi_win_inch1200.v = quotient;
        return;
    }
}
