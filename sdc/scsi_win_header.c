#include "fw.h"
#pragma section H

unsigned int scsi_win_check_header(unsigned char *p)
{
    unsigned char acc;
    unsigned short descriptor_len;
    unsigned char i;
    unsigned int end;
    if (p != scsi_param_buf) {
        descriptor_len = scsi_win_descriptor_len;
    } else {
        i = 0;
        acc = 0;
        for (; i < 6; i++) {
            acc |= *p;
            p++;
            if (acc != 0)
                { scsi_mark_invalid_field(p); return; }
        }
        descriptor_len = util_get_be16(p);
        if (descriptor_len < 40 || descriptor_len > 0xf8)
            { scsi_mark_invalid_field(p); return; }
        scsi_win_descriptor_len = descriptor_len;
        p += 2;
    }
    scsi_win_descriptor_addr = p;
    end = (unsigned int)p;
    end += (unsigned char)descriptor_len;
    scsi_win_descriptor_end = end;
    end -= (unsigned int)scsi_param_buf;
    if (end > scsi_param_len.v)
        { scsi_mark_invalid_field(p); return; }
}
