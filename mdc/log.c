#include "mdc.h"

void log_hook_begin(short unused_arg)
{
}

void log_advance(void)
{
    if (log_index < log_limit)
        log_index++;
    else if (log_frozen == 0)
        log_index = 16;
}

void log_advance_255(void)
{
    if (log_index < 255)
        log_index++;
    else if (log_frozen == 0)
        log_index = 16;
}

void log_end(void)
{
    mailbox.log_flags.b &= ~1;
}
