#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);


int job_wait_mdc_page_end(int unused_arg)
{
    job_wait_line_count();
    if (mdc_mailbox.page_end & (MDC_PAGE_END_DONE | MDC_PAGE_END_ACCEPTED)) {
        _builtin_set_imask(15);
        tick_wait_count = 0;
        _builtin_set_imask(0);
        while ((mdc_mailbox.page_end & MDC_PAGE_END_DONE) == 0) {
            if (tick_wait_count > 2500) {
                _builtin_set_imask(15);
                mdc_mailbox.page_end = 0;
                _builtin_set_imask(0);
                return 0;
            }
            if (mdc_mailbox.error_code.v != 0) {
                job_feed_starting = 1;
                err_check_pending();
                job_feed_starting = 0;
                return 1;
            }
        }
        err_raise(ERR_PAUSED_BY_USER);
        _builtin_set_imask(15);
        job_button_pause_flags &= 0x7fff;
        job_button_pause_flags |= 1;
        _builtin_set_imask(0);
        mdc_run_backing(0);
        return 1;
    }
    return 0;
}

int job_end(void)
{
    char u1, u2;
    unsigned char adf_sheets;
    _builtin_set_imask(15);
    panel_multifeed_blink_state.v = 0;
    panel_multifeed_blink_polls.v = 0;
    if (win_sides.f.selection == 0) {
        line_side_state[1].state_flags.v = 0;
    }
    if (win_params.flags.f.adf == 0) {
        if (++job_flatbed_sheet_ones.v >= 10) {
            job_flatbed_sheet_ones.v -= 10;
            main_flatbed_sheets_pending.v++;
        }
    } else {
        if (++job_adf_sheet_ones.v >= 10) {
            job_adf_sheet_ones.v -= 10;
            main_adf_sheets_pending.v++;
        }
        if (++job_adf_sheets_mod_10000.v > 9999) {
            job_adf_sheets_mod_10000.v = 0;
        }
    }
    job_sheet_count++;
    tick_sheet_gap_ticks = 0;
    if (!(maint_mode_word.v & 0x4000) || maint_mode_word.c.mode != MAINT_MODE_WHITE_LEVEL) {
        err_display_flags.v &= 0xef;
    }
    _builtin_set_imask(0);
    adf_sheets = 0;
    if (win_params.flags.f.adf) {
        if (mdc_is_adf_cover_open()) {
            err_raise(ERR_ADF_COVER_OPEN);
            return 0;
        }
        _builtin_set_imask(15);
        if (job_adf_sheet_count < 255) {
            job_adf_sheet_count++;
        }
        _builtin_set_imask(0);
        adf_sheets = job_adf_sheet_count;
    }
    _builtin_set_imask(15);
    job_adf_sheet_count = adf_sheets;
    _builtin_set_imask(0);
    if ((maint_mode_word.v & 0x5000) == 0) {
        panel_set_display_mode(0, PANEL_MODE_FUNCTION);
    }
    if (!win_params.flags.f.adf && (job_calib_valid & 0x8000)) { _builtin_set_imask(15);
        *(volatile unsigned short *)&job_calib_valid |= 4;
        _builtin_set_imask(0); }
    return 1;
}

int job_hook_page_flag(void)
{
    _builtin_set_imask(0);
    return 0;
}
