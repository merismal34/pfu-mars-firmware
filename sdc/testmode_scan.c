#include "fw.h"
#pragma section H


void testmode_30(void)
{
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    if (err_ultrasonic_error != 0) {
        err_code_list[0] = 2;
        err_code_count.v++;
    }
    if (mdc_is_adf_cover_open() != 0) {
        err_code_list[err_code_count.v] = 64;
        err_code_count.v++;
        testmode_number.v = 0;
        scsi_testmode_result_state.v = 1;
        return;
    }
    job_buffer_mode_on = 0;
    win_params.paper_size.f.auto_length = 1;
    win_params.side[0].use_window_area = win_params.side[1].use_window_area = 1;
    win_params.side[0].y_offset_inch1200 = win_params.side[1].y_offset_inch1200 = 0;
    win_params.side[0].length_inch1200 = win_params.side[1].length_inch1200 = 0x4d82;
    win_params.side[0].width_inch1200 = win_params.side[1].width_inch1200 = 100;
    win_params.resolution_mode.v &= 0xfff8;
    win_params.resolution_mode.v |= 1;
    util_mem_clear(scanning_page, 0x88);
    win_sides.f.selection = 0;
    scanning_page->flags.f.adf = 0;
    scanning_page->flags.f.scan_requested = 1;
    scanning_page->sides[0].flags.f.requested = 1;
    win_params.flags.f.adf = 0;
    main_run_page_loop();
    util_mem_clear(scanning_page, 0x88);
    scanning_page->flags.f.adf = 1;
    scanning_page->flags.f.scan_requested = 1;
    win_sides.f.selection = 2;
    scanning_page->sides[0].flags.f.requested = 1;
    job_prepick_on = 0;
    win_params.flags.f.adf = 1;
    main_run_page_loop();
    scanning_page->sides[0].flags.f.requested = 0;
    scsi_clear_read_state();
    testmode_number.v = 0;
    scsi_testmode_result_state.v = 1;
}
