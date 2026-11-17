#include "fw.h"
#pragma section H

unsigned char img_get_fill_byte(int reverse_image, int invert_if_reverse)
{
    unsigned char fill;

    if ((job_black_backing && !((int *)img_gamma_inverted)[0]) || (!job_black_backing && ((int *)img_gamma_inverted)[0])) {
        fill = 0;
    } else {
        fill = 0xff;
    }
    if (reverse_image && invert_if_reverse) {
        if (fill == 0) {
            fill = 0xff;
        } else {
            fill = 0;
        }
    }
    return fill;
}
