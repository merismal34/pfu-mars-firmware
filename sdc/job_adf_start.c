#include "fw.h"
#pragma section H

int job_check_adf_page_start(void)
{
    int attempt;
    int j;

    for (attempt = 0; ; attempt++) {
        if (job_check_lamp_at_page(job_black_backing) != 0) {
            break;
        }
        if (attempt >= 1) {
            err_raise(ERR_BACKING);
            return 1;
        }
        for (j = 0; j <= 25; j++) {
            rt_delay_180k();
            rt_delay_60k();
        }
        if (mdc_run_backing(0) == 0) {
            return 1;
        }
        for (j = 0; j <= 25; j++) {
            rt_delay_180k();
            rt_delay_60k();
        }
        if (mdc_run_backing(1) == 0) {
            return 1;
        }
    }
    return 0;
}

int job_check_lamp_at_page(int black)
{
    unsigned long trace_code;
    unsigned long limit;
    if (!win_params.flags.f.adf || (settings_test_flags[0] & 0x10) || (settings_test_flags[0] & 2)) return 1;
    calib_exposure[0][0] = 0;
    calib_exposure[0][1] = 0;
    calib_exposure[0][2] = 0;
    calib_exposure_back[0].chip0 = 0;
    calib_exposure_back[0].chip1 = 0;
    calib_exposure_back[0].chip2 = 0;
    calib_start_lamp_check(3);
    if (black) {
        if (mdc_mailbox.backing_position != MDC_BACKING_BLACK) {
            trace_code = 0xf0010000;
            goto send;
        }
        if (win_sides.f.selection != 1) {
            limit = (0x1e0 - calib_black_level[0][1]) * 50 / 0xff + calib_black_level[0][1];
            if (((unsigned long *)calib_exposure)[1] > limit) {
                trace_code = 0xf0020000;
                goto send;
            }
        }
        if (win_sides.f.selection) {
            limit = (0x1e0 - calib_black_level[1][1]) * 50 / 0xff + calib_black_level[1][1];
            if (((unsigned long *)calib_exposure_back)[1] > limit) {
                trace_code = 0xf0030000;
                goto send;
            }
        }
    } else {
        if (mdc_mailbox.backing_position != MDC_BACKING_WHITE) {
            trace_code = 0xf1010000;
            goto send;
        }
        if (win_sides.f.selection != 1) {
            if (((unsigned long *)calib_exposure)[1] < 0x120) {
                trace_code = 0xf1020000;
                goto send;
            }
        }
        if (win_sides.f.selection) {
            if (((unsigned long *)calib_exposure_back)[1] < 0x120) {
                trace_code = 0xf1030000;
                goto send;
            }
        }
    }
    return 1;
send:
    job_trace_add((job_sheet_count + 1) | trace_code);
    job_trace_add(((mdc_mailbox.sensor_status.v & MDC_SENSOR_BACKING) << 24) | (((unsigned long *)calib_exposure)[1] << 16) | ((unsigned long *)calib_exposure_back)[1]);
    return 0;
}

/* Trace ring: 20 codes in job_trace_ring, the slot after the newest set to 0xFFFFFFFF as an end mark. */
void job_trace_add(unsigned long code)
{
    job_trace_ring[job_trace_index] = code;
    if (++job_trace_index >= 20) job_trace_index = 0;
    job_trace_ring[job_trace_index] = 0xffffffff;
}
