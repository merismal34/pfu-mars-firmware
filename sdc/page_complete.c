#include "fw.h"
#pragma section H

int page_mark_complete(int side)
{
    struct page_record *scanning_page;
    int pad;

    scanning_page = &page_records[page_scanning.v];
    scanning_page->sides[side].flags.f.scanned = 1;
    if (scanning_page->sides[0].flags.f.scanned && scanning_page->sides[1].flags.f.scanned) {
    } else {
        return 0;
    }
    scanning_page->flags.f.scanned = 1;
    scanning_page->flags.f.adf = 0;
    scanning_page->flags.f.scan_requested = 0;
    if (page_scanning.v == page_record_ring - 1 || page_buf_count.v == 0) {
        page_scanning.v = 0;
    } else {
        page_scanning.v++;
    }
    page_scanned_count.v++;
    if ((maint_mode_word.v & 0x5000) && maint_mode_word.c.mode == MAINT_MODE_FEED_SENSOR_TEST) {
        util_mem_clear(scanning_page, 0x88);
        page_sent_count.v++;
    }
    p_pisces_output[0]->ctrl.enable = 1;
    p_pisces_output[1]->ctrl.enable = 1;
    p_pisces_output[0]->output_gate = p_pisces_output[1]->output_gate = 0;
    p_pisces_output[0]->ctrl.enable = 0;
    p_pisces_output[1]->ctrl.enable = 0;
    return 1;
}
