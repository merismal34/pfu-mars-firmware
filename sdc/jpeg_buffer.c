#include "fw.h"
#pragma section H
extern void _builtin_set_imask(int);
extern void _builtin_set_cr(int);
extern int _builtin_get_cr(void);


void jpeg_sync_buffer_pointer(int side, int saved_imask)
{
    int tries;
    unsigned short lines;

    _builtin_set_imask(15);
    if (p_pisces_output[side]->lines != p_pisces_output[side]->lines_done) {
top:
    {
        p_pisces_output[side]->ctrl.enable = 1;
        lines = p_pisces_output[side]->lines_done;
        lines += 16;
        if ((lines & 7) != 0) {
            lines += 8;
            lines &= 0xfff8;
        }
        p_pisces_output[side]->lines = p_pisces_output[side]->reg_0d4 = lines;
        p_pisces_output[side]->ctrl.enable = 0;
        _builtin_set_imask(saved_imask);
        for (tries = 0; 1;) {
            tries++;
            if (tries >= 500) {
                p_pisces_output[side]->output_gate = 0;
            }
            p_pisces_mode[side]->run.f.b12 = 0;
            rt_delay_24k();
            p_pisces_mode[side]->run.f.b12 = 1;
            rt_delay_24k();
            if (p_pisces_mode[side]->run.f.output_on == 0) {
                p_pisces_mode[side]->run.f.output_on = 1;
            }
            if (p_pisces_output[side]->lines == p_pisces_output[side]->lines_done) {
                goto end;
            }
            if (p_pisces_output[side]->reg_0da.enable) {
                goto end;
            }
            if (p_pisces_output[side]->lines < p_pisces_output[side]->lines_done) {
                goto top;
            }
        }
    }
    }
end:
    _builtin_set_imask(saved_imask);
}

struct vdcc_counter_reg_0b {
    char pad_00[11];
    struct vdcc_counter_ctrl_bits counter_ctrl;
    unsigned short line_count:16;
};

void jpeg_resume_capture(int saved_imask)
{
    _builtin_set_imask(15);
    jpeg_scan_flags.v &= JPEG_SCAN_LONG_PAGE;
    jpeg_scan_flags.v |= JPEG_SCAN_RESUMING;
    jpeg_restart_counter = ((volatile struct vdcc_counter_reg_0b *)p_vdcc_front_counter[0])->line_count;
    tick_capture_wait_ticks.v = 0;
    if (win_params.flags.f.adf) {
        line_gap_steps_left[0] = line_gap_steps_left_back[0] = line_gap_count_next;
        line_gap_lines_left[0] = line_gap_lines_left_back[0] = line_gap_lines_next;
        line_gap_step_ptr[0] = line_gap_step_ptr_back[0] = line_gap_table_next.v;
        ((volatile struct vdcc_counter_reg_0b *)p_vdcc_front_counter[0])->counter_ctrl.front_run = 0;
        ((volatile struct vdcc_counter_reg_0b *)p_vdcc_front_counter[0])->counter_ctrl.front_clear = 1;
        _builtin_set_imask(saved_imask);
        while (((volatile struct vdcc_counter_reg_0b *)p_vdcc_front_counter[0])->counter_ctrl.front_clear) {
        }
        _builtin_set_imask(15);
        ((volatile struct vdcc_counter_reg_0b *)p_vdcc_front_counter[0])->counter_ctrl.front_run = 1;
        line_gap_counter[0] = line_gap_counter_back[0] = 0;
    } else {
        p_vdcc_back_counter[0]->counter_ctrl.back_run = 0;
        p_vdcc_back_counter[0]->counter_ctrl.back_clear = 1;
        _builtin_set_imask(saved_imask);
        while (p_vdcc_back_counter[0]->counter_ctrl.back_clear) {
        }
        _builtin_set_imask(15);
        p_vdcc_back_counter[0]->counter_ctrl.back_run = 1;
    }
    _builtin_set_imask(saved_imask);
    mdc_request_capture(1);
    _builtin_set_imask(saved_imask);
}


void jpeg_check_buffer_limit(int side)
{
    struct page_record *current_page;
    unsigned int pending_bytes;

    current_page = &page_records[page_current.v];
    if (p_pisces_output[side]->output_gate == 0) {
        return;
    }
    pending_bytes = current_page->sides[side].image_bytes - current_page->sides[side].image_bytes_sent;
    if (pending_bytes > page_buf_limit_bytes) {
        _builtin_set_imask(15);
        p_pisces_output[side]->output_gate = 0;
        _builtin_set_imask(page_poll_imask);
    }
}
