#include "fw.h"
#pragma section H
extern int _builtin_get_imask(void);

int job_prepare_pipeline(void)
{
    int saved_imask;
    saved_imask = _builtin_get_imask();
    if (hw_option_flags.f.b0 == 0) {
        return 1;
    }
    return 1;
}

int job_get_last_error(void)
{
    return err_last_code.c.v;
}

int job_hook(void)
{
    return 0;
}
