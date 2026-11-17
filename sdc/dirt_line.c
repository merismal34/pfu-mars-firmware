#include "fw.h"
extern void _builtin_set_imask(int);


/* Constants (section C). */
const unsigned char dirt_dip_filter[5] = {1, 1, 0x84, 1, 1};
const unsigned char dirt_flat_filter[7] = {1, 0x81, 0, 0x81, 1, 0, 0};
/* Uninitialised data (section B). */
unsigned short dirt_line_buf[7500];

void dirt_scan_line(unsigned char port)
{
    unsigned short *src;
    unsigned short *ref;
    unsigned short *ref_walk, *tail, *tap;
    unsigned short x54, x52;
    unsigned short i, j, sum, level, patched, chip_bit, mean, prev_sum, lim;
    unsigned char window;
    unsigned char weight;
    const unsigned char *dip_tap, *flat_tap;
    int value;
    int dip_sum;
    int flat_sum;
    unsigned char run_limit;
    unsigned char run;
    unsigned short x8;
    unsigned total;
    unsigned short x2;
    unsigned char saved_back_bank;
    unsigned char saved_front_bank;

    _builtin_set_imask(15);
    switch (port) {
    case 0: p_vdcc_front_bank[0]->bank = 1; ref = p_vdcc_front_ref_0[0]; chip_bit = 4; break;
    case 1: p_vdcc_front_bank[0]->bank = 1; ref = p_vdcc_front_ref_1[0]; chip_bit = 2; break;
    case 2: p_vdcc_front_bank[0]->bank = 2; ref = p_vdcc_front_ref_2[0]; chip_bit = 1; break;
    case 3: p_vdcc_back_bank[0]->bank = 1; ref = p_vdcc_back_ref_0[0]; chip_bit = 4; break;
    case 4: p_vdcc_back_bank[0]->bank = 1; ref = p_vdcc_back_ref_1[0]; chip_bit = 2; break;
    case 5: p_vdcc_back_bank[0]->bank = 2; ref = p_vdcc_back_ref_2[0]; chip_bit = 1; break;
    default: p_vdcc_front_bank[0]->bank = 1; ref = p_vdcc_front_ref_0[0]; chip_bit = 0; break;
    }
    _builtin_set_imask(13);
    src = ref;
    ref_walk = dirt_line_buf;
    if (img_side_params[0].ccd_300dpi_mode != 0) {
        lim = 0xea6;
    } else {
        lim = 0x1d4c;
    }
    for (i = 0; i < lim; i++) {
        *ref_walk++ = *src++;
    }
    _builtin_set_imask(15);
    saved_front_bank = p_vdcc_front_bank[0]->bank;
    saved_back_bank = p_vdcc_back_bank[0]->bank;
    p_vdcc_front_bank[0]->bank = 0;
    p_vdcc_back_bank[0]->bank = 0;
    _builtin_set_imask(0);
    total = 0;
    sum = 0x100;
    level = (lim >> 1) + 0xff80;
    src = dirt_line_buf + level;
    for (i = 0; i < sum; i++) {
        total += *src++;
    }
    mean = total / sum;
    dirt_line_average[port].v = mean;
    if (win_params.flags.f.adf) {
        sum = rt_divu16(dirt_depth_limit * mean, 0x1e0);
    } else {
        if (calib_flatbed_high_target) {
            sum = rt_divu16(dirt_depth_limit * mean, 0x320);
        } else {
            sum = rt_divu16(dirt_depth_limit * mean, 0x280);
        }
    }
    dirt_depth_scaled = sum;
    dirt_depth_by_port[port].v = sum;
    if (win_params.flags.f.adf) {
        value = rt_divu32(dirt_dip_limit.c.lo * mean, 0x1e0);
    } else {
        if (calib_flatbed_high_target) {
            value = rt_divu32(dirt_dip_limit.c.lo * mean, 0x320);
        } else {
            value = rt_divu32(dirt_dip_limit.c.lo * mean, 0x280);
        }
    }
    dirt_dip_limit.v = value;
    dirt_dip_limit_by_port[port].v = value;
    dirt_patched_count = 0;
    dirt_candidate_count = 0;
    dirt_flat_count.v = 0;
    dirt_flat_low_count = 0;
    dirt_run_count = 0;
    dirt_half_width.v = 10;
    ref_walk = ref + 11;
    window = dirt_half_width.c.lo * 2 + 1;
    run_limit = 3;
    dirt_depth_limit = dirt_depth_scaled;
    mean = 0;
    prev_sum = sum = 0;
    src = dirt_line_buf;
    for (i = 0; i < window; i++) {
        sum += *src++;
        if (prev_sum <= sum) {
            prev_sum = sum;
        } else {
            sum = prev_sum = 0xffff;
            src = dirt_line_buf + window;
            break;
        }
    }
    tail = src - window;
    for (i = window; i < lim; i++) {
        sum += *src++;
        if (prev_sum <= sum) {
            prev_sum = sum;
        } else {
            prev_sum = 0xffff;
            tail++;
            run = 0;
            ref_walk++;
            continue;
        }
        sum -= *tail++;
        if (sum <= prev_sum) {
            prev_sum = sum;
        } else {
            prev_sum = 1;
            run = 0;
            ref_walk++;
            continue;
        }
        mean = rt_divu16(prev_sum - *(src - dirt_half_width.v - 1), window - 1);
        if (mean > 0x3ff) {
            run = 0;
            ref_walk++;
            continue;
        }
        level = *(src - dirt_half_width.v - 1);
        if (mean <= level) {
            run = 0;
            ref_walk++;
            continue;
        }
        if (dirt_defects_present != 0) {
            if (dirt_is_known_defect(i - dirt_half_width.v, port) != 0) {
                run = 0;
                ref_walk++;
                continue;
            }
        }
        if (dirt_depth_limit < mean - level) {
            dip_sum = 0;
            tap = src - (dirt_half_width.v + 3);
            dip_tap = dirt_dip_filter;
            for (j = 0; j < 5; j++) {
                value = *tap++;
                weight = *dip_tap;
                weight = weight & 0x7f;
                if (weight == 0) {
                    value = 0;
                } else if (weight != 1) {
                    value = dirt_shift_left(value, dirt_bit_to_shift(weight));
                }
                if (*dip_tap++ & 0x80) {
                    dip_sum -= value;
                } else {
                    dip_sum += value;
                }
            }
            if (!(dip_sum > dirt_dip_limit.v && dip_sum >= 0)) {
                patched = (mean * 100u) / 100u;
                if (patched > level) {
                    dirt_window_max.v = 0;
                    for (dirt_window_ptr.p = src - dirt_half_width.v * 2 - 1; dirt_window_ptr.p < src; dirt_window_ptr.p++) {
                        if (*dirt_window_ptr.p > dirt_window_max.v) {
                            dirt_window_max.v = *dirt_window_ptr.p;
                        }
                    }
                    _builtin_set_imask(15);
                    p_vdcc_front_bank[0]->bank = saved_front_bank;
                    p_vdcc_back_bank[0]->bank = saved_back_bank;
                    *(ref + i - dirt_half_width.v) = ((patched + dirt_window_max.v >> 1) + dirt_window_max.v) >> 1;
                    p_vdcc_front_bank[0]->bank = 0;
                    p_vdcc_back_bank[0]->bank = 0;
                    _builtin_set_imask(0);
                    dirt_patched_count++;
                }
                run = 0;
                ref_walk++;
                continue;
            }
            dirt_candidate_count++;
            flat_sum = 0;
            tap = src - (dirt_half_width.v + 3);
            flat_tap = dirt_flat_filter;
            for (j = 0; j < 5; j++) {
                value = *tap++;
                weight = *flat_tap;
                weight = weight & 0x7f;
                if (weight == 0) {
                    value = 0;
                } else if (weight != 1) {
                    value = dirt_shift_left(value, dirt_bit_to_shift(weight));
                }
                if (*flat_tap++ & 0x80) {
                    flat_sum -= value;
                } else {
                    flat_sum += value;
                }
            }
            if (flat_sum <= dirt_flat_limit) {
                dirt_flat_count.v++;
                if (flat_sum <= dirt_flat_limit_low) {
                    dirt_flat_low_count++;
                    run = 0;
                    ref_walk++;
                    continue;
                }
                if (run_limit <= run) {
                    dirt_run_count++;
                    run = 0;
                    ref_walk++;
                    continue;
                }
                run++;
                ref_walk++;
                continue;
            }
            patched = (mean * 100u) / 100u;
            if (patched > level) {
                dirt_window_max.v = 0;
                for (dirt_window_ptr.p = src - dirt_half_width.v * 2 - 1; dirt_window_ptr.p < src; dirt_window_ptr.p++) {
                    if (*dirt_window_ptr.p > dirt_window_max.v) {
                        dirt_window_max.v = *dirt_window_ptr.p;
                    }
                }
                _builtin_set_imask(15);
                p_vdcc_front_bank[0]->bank = saved_front_bank;
                p_vdcc_back_bank[0]->bank = saved_back_bank;
                *(ref + i - dirt_half_width.v) = ((patched + dirt_window_max.v >> 1) + dirt_window_max.v) >> 1;
                p_vdcc_front_bank[0]->bank = 0;
                p_vdcc_back_bank[0]->bank = 0;
                _builtin_set_imask(0);
                dirt_patched_count++;
            }
        }
        run = 0;
        ref_walk++;
    }
    _builtin_set_imask(15);
    p_vdcc_front_bank[0]->bank = 0;
    p_vdcc_back_bank[0]->bank = 0;
    _builtin_set_imask(0);
}

char dirt_bit_to_shift(unsigned char bit)
{
    char shift;
    switch (bit) {
    case 2: shift = 1; break;
    case 4: shift = 2; break;
    case 8: shift = 3; break;
    case 16: shift = 4; break;
    case 32: shift = 5; break;
    case 64: shift = 6; break;
    case 128: shift = 7; break;
    case 1: shift = 0; break;
    case 0: shift = 0; break;
    default: shift = 0; break;
    }
    return shift;
}

int dirt_shift_left(int value, char shift)
{
    while (shift != 0) {
        value <<= 1;
        shift--;
    }
    return value;
}

int dirt_is_known_defect(short pixel, unsigned char port)
{
    short *p;
    unsigned short k;
    unsigned short lo, hi;
    lo = 0;
    hi = 0;
    if (win_params.flags.f.adf) {
        if (port >= 3) {
            port -= 3;
            p = (short *)dirt_defects_adf_back[port];
        } else {
            p = ((struct dirt_defect_list *)flash_defect_area)[port].w;
        }
    } else {
        p = (short *)dirt_defects_flatbed[port];
    }
    if (img_side_params[0].ccd_300dpi_mode != 0) {
        pixel = pixel << 1;
    }
    lo = pixel + 0xffff;
    hi = pixel + 1;
    for (k = 0; k < 0x80; k++, p++) {
        if ((unsigned)pixel < (unsigned)*p) {
            return 0;
        }
        if ((unsigned)(short)lo <= (unsigned)*p && (unsigned)*p <= (unsigned)(short)hi) {
            return 1;
        }
    }
    return 0;
}
