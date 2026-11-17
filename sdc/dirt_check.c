#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
void dirt_run_check(void)
{
    if (job_adf_sheet_count)
        return;
    if ((settings_test_flags[0] & 0x10) || (settings_test_flags[0] & 2))
        return;
    if (win_sides.f.selection == 2) {
        dirt_check(0);
        dirt_check(1);
    } else if (win_sides.f.selection == 1) {
        dirt_check(1);
    } else {
        dirt_check(0);
    }
}


void dirt_check(int side)
{
    unsigned char saved_mode_chip0;
    unsigned char saved_mode_chip1;
    unsigned char saved_mode_chip2;
    unsigned short retries_chip0;
    unsigned short retries_chip1;
    unsigned short retries_chip2;

    _builtin_set_imask(15);
    if (settings_calib_options[0] & 0x80) {
        _builtin_set_imask(0);
        return;
    }
    retries_chip0 = retries_chip1 = retries_chip2 = 0;
    saved_mode_chip0 = p_vdcc_front_correction[side]->mode_chip0;
    saved_mode_chip1 = p_vdcc_front_correction[side]->mode_chip1;
    saved_mode_chip2 = p_vdcc_front_correction[side]->mode_chip2;
    while (p_vdcc_front_correction[side]->mode_chip0 != CALIB_CORRECTION_RAW) {
        retries_chip0++;
        p_vdcc_front_correction[side]->mode_chip0 = CALIB_CORRECTION_RAW;
    }
    while (p_vdcc_front_correction[side]->mode_chip1 != CALIB_CORRECTION_RAW) {
        retries_chip1++;
        p_vdcc_front_correction[side]->mode_chip1 = CALIB_CORRECTION_RAW;
    }
    while (p_vdcc_front_correction[side]->mode_chip2 != CALIB_CORRECTION_RAW) {
        retries_chip2++;
        p_vdcc_front_correction[side]->mode_chip2 = CALIB_CORRECTION_RAW;
    }
    _builtin_set_imask(0);
    dirt_depth_scaled = 40;
    dirt_depth_limit = 40;
    if (win_params.flags.f.adf != 0)
        dirt_dip_limit.v = 0x9c;
    else
        dirt_dip_limit.v = 0xe8;
    dirt_flat_limit = 80;
    dirt_flat_limit_low = 20;
    if (win_params.flags.f.adf != 0) {
        if (win_sides.f.selection == 2) {
            if (side == 0) {
                dirt_scan_line(0);
                dirt_scan_line(1);
                dirt_scan_line(2);
            } else {
                dirt_scan_line(3);
                dirt_scan_line(4);
                dirt_scan_line(5);
            }
        } else if (win_sides.f.selection == 1) {
            dirt_scan_line(3);
            dirt_scan_line(4);
            dirt_scan_line(5);
        } else {
            dirt_scan_line(0);
            dirt_scan_line(1);
            dirt_scan_line(2);
        }
    } else {
        dirt_scan_line(0);
        dirt_scan_line(1);
        dirt_scan_line(2);
    }
    _builtin_set_imask(15);
    p_vdcc_front_correction[side]->mode_chip0 = saved_mode_chip0;
    p_vdcc_front_correction[side]->mode_chip1 = saved_mode_chip1;
    p_vdcc_front_correction[side]->mode_chip2 = saved_mode_chip2;
    _builtin_set_imask(0);
}
