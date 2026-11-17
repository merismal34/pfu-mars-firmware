#include "fw.h"

extern void _builtin_set_imask(int);

void page_copy_multifeed_flags(int from_capture)
{
    struct page_record *scanning_page;
    if (from_capture)
        scanning_page = &page_side_records[page_side_state[0].capture_buf];
    else
        scanning_page = &page_records[page_scanning.v];
    _builtin_set_imask(15);
    scanning_page->multifeed_flags.v = 0;
    if (mdc_mailbox.multifeed_flags & MDC_MULTIFEED_ULTRASONIC) scanning_page->multifeed_flags.f.ultrasonic = 1;
    if (mdc_mailbox.multifeed_flags & MDC_MULTIFEED_LENGTH) scanning_page->multifeed_flags.f.length = 1;
    if (from_capture && (job_buffer_option & 0x8000) && scanning_page->multifeed_flags.v)
        ((volatile unsigned short *)&job_buffer_option)[0] |= 0x100;
    _builtin_set_imask(page_tick_imask);
}
