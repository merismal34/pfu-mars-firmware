#include "fw.h"
#pragma section H


/* Finds a dark band in the profile calib_record_mark_sample sampled (positions edge_right_front, levels edge_left_front): 7-point
   average, steepest fall then rise (selftest_find_mark_edges); sets selftest_mark_offset and settings_mark_result. 0 or 0x87. */
unsigned char selftest_find_mark(void)
{
    unsigned short levels[284];
    unsigned long sum;
    unsigned long i;
    unsigned long tail;
    unsigned long head;
    long fall_index;
    unsigned long start;
    unsigned short fall_pos;
    unsigned short rise_pos;
    unsigned short z;
    unsigned short width;
    unsigned char base_offset;
    unsigned char result;

    if (selftest_mark_start < 0xe0) {
        return 0x87;
    }
    if (settings_byte_0x35[0] & 1) {
        fall_pos = selftest_mark_start - 0xb1;
    } else {
        fall_pos = selftest_mark_start - 0xe0;
    }
    for (start = 0; start < selftest_profile_count; start++) {
        if (edge_right_front[start] > fall_pos) {
            break;
        }
    }
    if (selftest_profile_count < 0x11b) {
        return 0x87;
    } else {
        if (start > selftest_profile_count - 0x11b) {
            start = selftest_profile_count - 0x11b;
        }
        for (i = 0; i < 0x11b; i++) {
            edge_right_front[i] = edge_right_front[i + start] - fall_pos;
        }
        sum = 0;
        for (i = 0; i < 7; i++) {
            sum += edge_left_front[i + start];
        }
        tail = start;
        head = start + 7;
        for (i = 3; i < 0x118; i++) {
            levels[i] = sum / 7;
            sum += edge_left_front[head++];
            sum -= edge_left_front[tail++];
        }
        fall_pos = rise_pos = z = 0;
        tail = 4;
        head = 0x8e;
        result = 0x87;
        for (i = 0; i < 2; i++) {
            selftest_find_mark_edges(&fall_pos, &rise_pos, &fall_index, levels, tail, head);
            if (fall_index == 0) {
                return 0x87;
            }
            if (rise_pos > fall_pos) { width = rise_pos - fall_pos; if (width > 0x46 && width < 0x76) {
                width = (rise_pos + fall_pos) / 2;
                if (width > 0x36) { if (width < 0xb6) {
                    base_offset = settings_mark_flags[0] & 0x7f;
                    if (base_offset == 0) {
                        base_offset = 0x40;
                    }
                    selftest_mark_offset = width + 10;
                    settings_mark_result[0] = selftest_mark_offset + 0xc0;
                    selftest_mark_offset -= base_offset;
                    result = 0;
                    break;
                }}
            }}
            if (fall_pos < 0x47) {
                tail = fall_index + 23;
                head = 0x8e;
            } else {
                tail = 4;
                head = fall_index - 23;
            }
        }
        return result;
    }
}

void selftest_find_mark_edges(unsigned short *fall_pos, unsigned short *rise_pos, long *fall_index, unsigned short *levels,
              unsigned long start, unsigned long end_pos)
{
    unsigned long i;
    unsigned short *before;
    unsigned short *after;
    unsigned short best;
    unsigned short step;

    best = 0;
    before = levels + start - 1;
    after = levels + start + 1;
    for (i = start; i < 0x118; i++) {
        if (*before > *after && (step = *before - *after, step > best)) {
            best = step;
            *fall_index = i;
            *fall_pos = edge_right_front[i];
        }
        if (edge_right_front[i] > end_pos) {
            break;
        }
        before++;
        after++;
    }
    if (best == 0) {
        *fall_index = 0;
        return;
    }
    best = best >> 1;
    before = levels + *fall_index - 1;
    after = levels + *fall_index + 1;
    for (i = *fall_index; i < 0x118; i++) {
        if (*after > *before && (step = *after - *before, step > best)) {
            *rise_pos = edge_right_front[i];
            break;
        }
        before++;
        after++;
    }
}
