#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

int mdc_run_feed(void)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    if (settings_test_flags[0] & 4)
        return 1;
    afe_select_source();
    _builtin_set_imask(15);
    mdc_set_feed_params();
    _builtin_set_imask(saved_imask);
    while (mdc_busy.w)
        _builtin_set_imask(saved_imask);
    err_feed_params_pending = 1;
    if (err_check_pending() == 0)
        return 0;
    _builtin_set_imask(15);
    if (job_feed_backing_request != 0) {
        mdc_mailbox.feed_backing_request = job_feed_backing_request;
        job_feed_backing_request = 0;
    } else {
        mdc_mailbox.feed_backing_request = 0;
    }
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    mdc_busy.f.feed = 1;
    _builtin_set_imask(saved_imask);
    if (main_page_loop_phase == 0)
        return 1;
    else {
        while (mdc_busy.f.feed)
            ;
        if (mdc_cmd_posted.c.last_result == MDC_RESULT_ERROR)
            return 0;
        if (mdc_cmd_posted.c.last_result == MDC_RESULT_TIMEOUT)
            return 0;
        return 1;
    }
}
