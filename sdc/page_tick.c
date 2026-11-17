#define page_tick_imask page_tick_imask_hdr
#define page_read_lines page_read_lines_hdr
#define page_saved_record_tail page_saved_record_tail_hdr
#define page_pass_held page_pass_held_hdr
#include "fw.h"
#undef page_tick_imask
#undef page_read_lines
#undef page_saved_record_tail
#undef page_pass_held
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);


/* Uninitialised data (section B). */
int page_tick_imask;
unsigned short page_read_lines[2];
struct page_record_tail page_saved_record_tail;
int page_pass_held[2];

void page_tick(void)
{
    page_tick_imask = _builtin_get_imask();
    page_run_side_events(0);
    page_run_side_events(1);
}

void page_run_side_events(int side)
{
    unsigned short events;
    events = page_side_state[side].events;
    if (events & 0x40) page_event_40(side);
    if (events & 1) page_event_01(side);
    if (events & 0x20) page_event_20(side);
    if (events & 8) page_event_08();
    if (events & 0x80) page_event_80(side);
    if (events & 4) page_event_04(side);
    if (events & 2) page_event_02(side);
}

void page_event_80(unsigned int side)
{
    unsigned char capture_buf;
    unsigned char buf_flags;
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    _builtin_set_imask(15);
    capture_buf = page_side_state[side].capture_buf;
    buf_flags = page_side_state[side].buf[capture_buf].flags;
    _builtin_set_imask(page_tick_imask);
    if (page_side_state[side].uncropped && p_vdcc_capture[side]->capture_ctrl.capture_run && !(page_side_state[side].events & 0x41)) buf_flags |= 0x40;
    if (buf_flags & 0x40) {
        if (page_side_state[side].uncropped == 0 && win_params.flags.f.adf && !(line_motion_flags & 1)) if ((settings_test_flags[0] & 4) == 0) return;
        _builtin_set_imask(15);
        page_side_state[side].events &= 0xff7f;
        if (page_side_state[0].last_buf == 0) {
            if (side == 0) page_copy_multifeed_flags(0);
            page_side_state[side].events |= 2;
        } else {
            if (0 == side) page_copy_multifeed_flags(1);
            if (page_side_state[side].capture_buf != page_side_state[side].output_buf) {
                if (page_side_state[side].queued_count == 0) page_side_state[side].queued_buf = page_side_state[side].capture_buf;
                page_side_state[side].queued_count++;
                page_side_state[side].events |= 0x20;
            } else {
                page_side_state[side].start_buf = page_side_state[side].capture_buf;
                page_side_state[side].events |= 4;
            }
            page_side_state[side].capture_buf++;
            if (page_side_state[side].capture_buf > page_side_state[0].last_buf) page_side_state[side].capture_buf = 0;
            page_side_state[side].captured_count++;
            if (side == 0) page_fed_count.v++;
        }
        _builtin_set_imask(page_tick_imask);
    }
}

void page_event_20(int side)
{
    if (page_side_state[side].queued_buf != page_side_state[side].output_buf) return;
    _builtin_set_imask(15);
    if (page_side_state[side].queued_count > 0) page_side_state[side].queued_count--;
    page_side_state[side].start_buf = page_side_state[side].queued_buf;
    page_side_state[side].events |= 4;
    if (page_side_state[side].queued_count) {
        page_side_state[side].queued_buf++;
        if (page_side_state[side].queued_buf > page_side_state[0].last_buf) page_side_state[side].queued_buf = 0;
    } else {
        page_side_state[side].events &= 0xffdf;
    }
    _builtin_set_imask(page_tick_imask);
}

void page_event_04(unsigned int side)
{
    unsigned char start_buf;
    struct page_record *scanning_page;
    int last_side;
    scanning_page = &page_records[page_scanning.v];
    if (page_side_state[0].last_buf == 0 || page_buf_count.v < 2) {
        if (page_scanned_count.v > page_sent_count.v) return;
    } else if (page_buf_count.v - 2 < page_scanned_count.v - page_sent_count.v) return;
    if (win_sides.f.selection == 2 && page_side_state[0].output_buf != page_side_state[1].output_buf) return;
    if (page_side_state[side].events & 0x41) return;
    _builtin_set_imask(15);
    start_buf = page_side_state[side].start_buf;
    if (win_sides.f.selection != 0) last_side = 1; else last_side = 0;
    if (side == last_side) {
        scanning_page->flags.v |= page_side_records[start_buf].flags.v & PAGE_FLAG_SHEET_FED;
        scanning_page->multifeed_flags.v = page_side_records[start_buf].multifeed_flags.v;
        util_mem_clear(&page_side_records[start_buf], 0x88);
    }
    page_side_state[side].events &= 0xfffb;
    page_side_state[side].events |= 2;
    _builtin_set_imask(page_tick_imask);
}

void page_event_01(int side)
{
    unsigned short done_state;
    unsigned char output_buf;
    int last_side;
    unsigned short *edge_state;
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    output_buf = page_side_state[side].output_buf;
    edge_state = (unsigned short *)&edge_side_records[side].e[output_buf];
    edge_run_page_step(side, page_tick_imask, edge_state);
    if (page_side_state[side].uncropped) done_state = EDGE_STATE_WAIT_BOTTOM; else done_state = EDGE_STATE_DONE;
    if (edge_side_records[side].e[output_buf].state < done_state) {
        page_read_lines[side] = 0;
    } else {
        if (win_sides.f.selection != 0) last_side = 1; else last_side = 0;
        _builtin_set_imask(15);
        if (side == last_side) {
            if (win_sides.f.selection == 2) {
            } else if (win_sides.f.selection == 1) scanning_page->sides[0].size_final = 1;
            else scanning_page->sides[1].size_final = 1;
        }
        _builtin_set_imask(page_tick_imask);
        if (page_size_wanted && page_size_side == side) page_set_image_size();
        page_set_stall_limit(side);
        page_stall_count[side].w = 0;
        _builtin_set_imask(15);
        page_side_state[side].events &= 0xfffe;
        page_side_state[side].events |= 0x40;
        _builtin_set_imask(page_tick_imask);
    }
}

void page_event_40(int side)
{
    struct page_record *scanning_page;
    unsigned char output_buf;
    unsigned short mask;
    int each_side;
    int last_side;
    int x;
    struct edge_record *edge;
    int complete;
    scanning_page = &page_records[page_scanning.v];
    page_event_40_step_1(side);
    output_buf = page_side_state[side].output_buf;
    edge = &edge_side_records[side].e[output_buf];
    if (edge->state < EDGE_STATE_DONE) {
        mask = 0x100;
        if (line_side_state[side].state_flags.v & mask) edge_run_page_step(side, page_tick_imask, &edge->state);
    }
    if (page_buf_limit_bytes) page_event_40_step_3(side);
    if (page_read_lines[side] >= scanning_page->sides[side].length_lines) {
        if (page_side_state[side].uncropped && page_size_wanted && page_size_side == side) page_set_image_size();
        _builtin_set_imask(15);
        complete = page_mark_complete(side);
        page_side_state[side].events &= 0xffbf;
        if (complete) {
            if (win_sides.f.selection != 0) {
                each_side = 0;
                last_side = 1;
            } else {
                each_side = 0;
                last_side = 0;
            }
            while (each_side <= last_side) {
                page_side_state[each_side].buf[output_buf].flags &= 0xbf;
                page_side_state[each_side].buf[output_buf].flags &= 0x7f;
                page_side_state[each_side].output_buf++;
                if (page_side_state[each_side].output_buf > page_side_state[each_side].last_buf) page_side_state[each_side].output_buf = 0;
                page_side_state[each_side].output_count++;
                p_pisces_mode[each_side]->run.f.b4 = 0;
                p_pisces_mode[each_side]->run.f.b8 = 0;
                each_side++;
            }
        }
        _builtin_set_imask(page_tick_imask);
    }
}

void page_force_end(void)
{
    unsigned char last_page_buf;
    unsigned short events;
    int saved_imask;
    saved_imask = _builtin_get_imask();
    events = page_side_state[0].events;
    if (events & 8) return;
    _builtin_set_imask(15);
    page_side_state[0].events &= 0xff7f;
    if (page_side_state[0].capture_buf == page_side_state[1].capture_buf) page_side_state[1].events &= 0xff7f;
    page_side_state[0].events |= 8;
    page_side_state[1].events |= 8;
    last_page_buf = page_side_state[0].capture_buf;
    page_side_state[0].last_page_buf = last_page_buf;
    page_side_state[1].last_page_buf = last_page_buf;
    page_side_state[0].buf[last_page_buf].flags &= 0x7f;
    page_side_state[1].buf[last_page_buf].flags &= 0x7f;
    page_saved_record_tail = *(struct page_record_tail *)page_side_records[last_page_buf].tail;
    mdc_mailbox.last_page.v = 0x80;
    _builtin_set_imask(saved_imask);
    page_flush_image_size();
}

void page_flush_image_size(void)
{
    if (page_size_wanted) page_set_image_size();
}

void page_event_08(void)
{
    unsigned char last_page_buf;
    int last_side;
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    if (page_side_state[0].last_buf == 0 || page_buf_count.v < 2) {
        if (page_scanned_count.v > page_sent_count.v) return;
    } else if (page_buf_count.v - 2 < page_scanned_count.v - page_sent_count.v) return;
    if (win_sides.f.selection == 2 && page_side_state[0].output_buf != page_side_state[1].output_buf) return;
    if (win_sides.f.selection != 0) last_side = 1; else last_side = 0;
    if (page_side_state[0].last_page_buf != (char)page_side_state[last_side].output_buf) return;
    _builtin_set_imask(15);
    last_page_buf = page_side_state[0].last_page_buf;
    scanning_page->flags.f.sheet_fed = 0;
    scanning_page->flags.f.scanned = 0;
    if (job_aborted == 0) scanning_page->flags.f.scan_end = 1;
    if (main_page_loop_phase == 0 && page_side_state[0].last_buf != 0) {
        *(struct page_record_tail *)scanning_page->tail = page_saved_record_tail;
        util_mem_clear(&page_side_records[last_page_buf], 0x88);
    }
    page_side_state[0].events &= 0xfff7;
    page_side_state[1].events &= 0xfff7;
    _builtin_set_imask(page_tick_imask);
}

void page_event_02(int side)
{
    int last_side;
    int u;
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    page_event_02_step_1(side);
    if (win_sides.f.selection != 0) last_side = 1; else last_side = 0;
    _builtin_set_imask(15);
    if (side == last_side) {
        if (win_sides.f.selection == 2) {
        } else if (win_sides.f.selection == 1) scanning_page->sides[0].flags.f.scanned = 1;
        else scanning_page->sides[1].flags.f.scanned = 1;
    }
    scanning_page->sides[side].size_final = 0;
    page_side_state[side].events &= 0xfffd;
    page_side_state[side].events |= 1;
    _builtin_set_imask(page_tick_imask);
}

void page_event_02_step_1(int side)
{
    struct win_side *window;
    _builtin_set_imask(15);
    window = &win_params.side[side];
    if (window->composition.f.color) {
        p_pisces_mode[side]->format.f.f14 = 2;
        p_pisces_mode[side]->format.f.b8 = 1;
        p_pisces_mode[side]->format.f.b7 = 1;
    } else {
        p_pisces_mode[side]->format.f.f14 = 0;
        p_pisces_mode[side]->format.f.b8 = 0;
        p_pisces_mode[side]->format.f.b7 = 0;
    }
    _builtin_set_imask(page_tick_imask);
}

void page_set_image_size(void)
{
    struct page_record *scanning_page;
    int saved_imask;
    scanning_page = &page_records[page_scanning.v];
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    page_size_wanted = 0;
    page_size_ready = 1;
    if (scanning_page->sides[page_size_side].paper_width_px != 0 && scanning_page->sides[page_size_side].paper_length_lines != 0) {
        page_size_paper_width_px = scanning_page->sides[page_size_side].paper_width_px;
        page_size_paper_length_lines = scanning_page->sides[page_size_side].paper_length_lines;
    } else {
        page_size_paper_width_px = scanning_page->sides[page_size_side].width_px;
        page_size_paper_length_lines = scanning_page->sides[page_size_side].length_lines;
    }
    if (scanning_page->sides[page_size_side].width_unaligned_px != 0) page_size_word_24 = scanning_page->sides[page_size_side].width_unaligned_px;
    else page_size_paper_width_px = scanning_page->sides[page_size_side].width_px;
    if (scanning_page->sides[page_size_side].length_unpadded_lines != 0) page_size_word_28 = scanning_page->sides[page_size_side].length_unpadded_lines;
    else page_size_paper_length_lines = scanning_page->sides[page_size_side].length_lines;
    page_size_driver_crop = scanning_page->driver_crop;
    _builtin_set_imask(saved_imask);
}

void page_event_40_step_1(int side)
{
    struct page_record *scanning_page;
    struct win_side *window;
    int u;
    unsigned short lines_done;
    unsigned short width_px;
    scanning_page = &page_records[page_scanning.v];
    window = &win_params.side[side];
    while (1) {
        lines_done = p_pisces_pass_dest[side]->lines_done;
        if (lines_done == p_pisces_pass_dest[side]->lines_done) break;
    }
    if (lines_done == page_read_lines[side]) {
        if (window->compression.f.type & 0x80) {
        } else if (++*(unsigned short *)(page_stall_count + side) >= *(unsigned short *)(page_stall_limit + side) && lines_done == 0) {
            err_raise(ERR_IMAGE_TRANSFER);
            return;
        }
    } else {
        page_stall_count[side].w = 0;
    }
    page_read_lines[side] = lines_done;
    _builtin_set_imask(15);
    if (page_read_lines[side] > scanning_page->sides[side].length_lines) lines_done = page_read_lines[side] = scanning_page->sides[side].length_lines;
    _builtin_set_imask(page_tick_imask);
    width_px = p_pisces_pass_dest[side]->width_px.v;
    if (window->composition.f.color) scanning_page->sides[side].image_bytes = width_px * lines_done * 3;
    else if (window->composition.f.multi_level) scanning_page->sides[side].image_bytes = width_px * lines_done;
    else scanning_page->sides[side].image_bytes = width_px * lines_done / 8;
}

void page_event_40_step_3(int side)
{
    struct page_record *scanning_page;
    scanning_page = &page_records[page_scanning.v];
    if (scanning_page->sides[side].flags.f.requested) {
        if (page_pass_held[side] == 1) {
            if (scanning_page->sides[side].flags.f.data_ready) {
                page_pass_held[side] = 0;
                _builtin_set_imask(15);
                p_pisces_pass_dest[side]->ctrl.enable = 0;
                _builtin_set_imask(page_tick_imask);
            }
        }
    } else if (scanning_page->sides[side].image_bytes > page_buf_limit_bytes) {
        if (!page_pass_held[side]) {
            page_pass_held[side] = 1;
            _builtin_set_imask(15);
            p_pisces_pass_dest[side]->ctrl.enable = 1;
            _builtin_set_imask(page_tick_imask);
        }
    }
}


int page_next_index(int next)
{
    int in_use = 0;
    unsigned char index;
    unsigned char count;
    index = page_side_state[0].capture_buf;
    count = page_side_state[0].last_buf + 1;
    if (next) index = index + 1;
    if (page_side_state[0].buf[index % count].flags & 0x80) in_use = 1;
    if (page_side_state[1].buf[index % count].flags & 0x80) in_use = 1;
    return in_use;
}
