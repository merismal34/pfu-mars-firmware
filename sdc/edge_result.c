#include "fw.h"
#pragma section H


extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);


void edge_run_result_state(int side, int saved_imask, struct edge_record_words *edge)
{
    unsigned char output_buf;
    switch (edge->state) {
    case EDGE_STATE_FALLBACK:
        edge_state_100(side, saved_imask, edge);
        break;
    case EDGE_STATE_APPLY:
        if ((edge->flags & EDGE_FLAG_FALLBACK) == 0) {
            output_buf = page_side_state[side].output_buf;
            if (side == 0) {
                if ((((struct edge_record *)&edge_side_records[1])[output_buf].flags & EDGE_FLAG_FALLBACK) != 0) {
                    edge->state = EDGE_STATE_FALLBACK;
                    return;
                }
            } else {
                if ((((struct edge_record *)edge_side_records)[output_buf].flags & EDGE_FLAG_FALLBACK) != 0) {
                    edge->state = EDGE_STATE_FALLBACK;
                    return;
                }
                if (!job_auto_size_flags.b5) {
                    edge_state_1000_a(saved_imask, edge);
                }
            }
        }
        if ((job_long_page_flags & 0x80) != 0 && line_long_page_state != 0) {
            return;
        }
        edge_state_1000_b(side, saved_imask, edge);
        edge_state_1000_c(side, saved_imask, edge);
        edge_state_1000_d(side, saved_imask, edge);
        edge_state_1000_e(side, saved_imask, edge);
        break;
    }
}
void edge_state_1000_a(int saved_imask, struct edge_record_words *edge)
{
    int zoom_permille;
    edge_page_geometry[1].skew_angle = edge_page_geometry[0].skew_angle;
    edge_page_geometry[1].dx_px = edge_page_geometry[0].dx_px;
    edge_page_geometry[1].source_lines = edge_page_geometry[0].source_lines;
    zoom_permille = edge_page_geometry[0].edge_length_px * 1000 + ((unsigned)edge_page_geometry[0].dx_px >> 1);
    zoom_permille = rt_divu16(zoom_permille, edge_page_geometry[0].dx_px);
    edge_page_geometry[1].x_zoom_permille = rt_divu16(img_side_params[1].x_zoom_permille * zoom_permille + 0x22b, 1000);
    edge_page_geometry[1].width_px = rt_divu16(edge_page_geometry[1].dx_px * edge_page_geometry[1].x_zoom_permille + 0x22b, 1000);
    edge_page_geometry[1].width_unaligned_px = edge_page_geometry[1].width_px;
    edge_align_width(1, &edge_page_geometry[1].width_px);
    edge_page_geometry[1].paper_width_px = edge_page_geometry[1].width_px;
    img_align_width(1, &edge_page_geometry[1].width_px, 1);
    if (edge_page_geometry[1].width_px > img_side_params[1].width_px) {
        edge_page_geometry[1].width_px = img_side_params[1].width_px;
        edge_page_geometry[1].paper_width_px = img_side_params[1].width_px;
    }
    if (edge_page_geometry[1].width_unaligned_px > edge_page_geometry[1].width_px) {
        edge_page_geometry[1].width_unaligned_px = edge_page_geometry[1].width_px;
    }
    edge_page_geometry[1].y_zoom_permille = rt_divu16(img_side_params[1].y_zoom_permille * zoom_permille + 0x22b, 1000);
    edge_top_line[1].run = edge_top_line[0].run;
    edge_top_line[1].rise = edge_top_line[0].rise;
    edge_page_geometry[1].length_lines = edge_compute_paper_length(1, edge_page_geometry[1].source_lines);
}
void edge_state_100(int side, int saved_imask, struct edge_record_words *edge)
{
    unsigned short top_line, source_lines, length_lines, u;
    unsigned char output_buf;
    edge->flags |= EDGE_FLAG_FALLBACK;
    edge_page_geometry[side].skew_angle = 0;
    edge_top_corners[side].left_lower = 0;
    if (job_auto_size_flags.b7 || job_auto_size_flags.b6) {
        top_line = img_crop_area[side].y_start_lines;
        output_buf = page_side_state[side].output_buf;
        if (img_overscan_shift_on[side]) {
            top_line += img_overscan_shift_lines[side][output_buf];
        }
        if (job_auto_size_flags.b5) {
            top_line += img_crop_area[side].y_offset_lines;
        }
        if (win_params.paper_size.f.auto_length) {
            source_lines = edge->event_20_lines + line_page_state[side].top_offset_lines;
            if (img_overscan_shift_on[side]) {
                source_lines -= img_overscan_shift_lines[side][output_buf];
            }
            length_lines = edge_compute_paper_length(side, source_lines);
        } else {
            source_lines = img_crop_area[side].length_lines;
            length_lines = img_side_params[side].output_length_lines;
        }
    } else {
        top_line = img_crop_area[side].y_start_lines;
        source_lines = img_crop_area[side].length_lines;
        length_lines = img_side_params[side].output_length_lines;
    }
    edge_top_corners[side].left_x_px = img_crop_area[side].x_start_px;
    edge_top_corners[side].right_x_px = img_crop_area[side].x_start_px + img_crop_area[side].width_px;
    edge_top_corners[side].left_line = top_line;
    edge_top_corners[side].right_line = top_line;
    edge_page_geometry[side].dx_px = img_crop_area[side].width_px;
    edge_page_geometry[side].source_lines = source_lines;
    edge_page_geometry[side].edge_length_px = img_crop_area[side].width_px;
    edge_page_geometry[side].length_source_lines = source_lines;
    edge_page_geometry[side].width_px = img_side_params[side].width_px;
    edge_page_geometry[side].length_lines = length_lines;
    util_mem_clear(&edge_bottom_point[side], 4);
    edge->state = EDGE_STATE_APPLY;
}

void edge_state_1000_b(int side, unsigned saved_imask, struct edge_record_words *edge)
{
    unsigned short x_offset, y_offset;
    int addr;
    unsigned x_start;
    struct win_side *window;
    unsigned char fill;
    window = &win_params.side[side];
    x_start = edge_top_corners[side].left_x_px;
    if (x_start & 1) {
        x_start = x_start + 1;
    }
    if ((int)x_start >= 0) {
        x_offset = x_start;
    } else {
        x_offset = -(int)x_start;
        x_offset |= 0x4000;
    }
    _builtin_set_imask(15);
    p_pisces_pass_source[side]->x_offset.f.v = x_offset;
    _builtin_set_imask(saved_imask);
    if (edge_top_corners[side].left_line >= 0) {
        y_offset = edge_top_corners[side].left_line;
    } else {
        y_offset = -edge_top_corners[side].left_line;
        y_offset |= 0x8000;
    }
    _builtin_set_imask(15);
    p_pisces_pass_source[side]->y_offset.f.v = y_offset;
    if (edge_top_corners[side].left_line < 0 || (!edge_top_corners[side].left_line && (int)x_start < 0)) {
        addr = p_pisces_capture[side]->size_64k.v;
        addr = addr << 15;
        addr = addr + edge_top_corners[side].left_line * img_side_params[side].capture_width_px + x_start;
    } else {
        addr = edge_top_corners[side].left_line * img_side_params[side].capture_width_px + x_start;
    }
    if ((job_long_page_flags & 0x80) && !win_params.side[0].composition.f.color && job_dropout[1].w != 3) {
    } else {
        addr += addr;
    }
    addr += page_side_state[side].buf[page_side_state[side].output_buf].base_64k.v * 0x10000;
    p_pisces_pass_source[side]->addr_high.f.v = addr >> 16;
    p_pisces_pass_source[side]->addr_low.f.v = addr;
    p_pisces_pass_source[side]->width.v = edge_page_geometry[side].dx_px + 0x80;
    p_pisces_pass_source[side]->length.v = edge_page_geometry[side].source_lines + 0x20;
    p_pisces_pass_source[side]->base_64k.v = page_side_state[side].buf[page_side_state[side].output_buf].base_64k.s.lo;
    p_pisces_pass_source[side]->ring_64k.v = p_pisces_capture[side]->size_64k.v;
    p_pisces_pass_source[side]->stride.v = img_side_params[side].capture_width_px;
    if (page_side_state[side].uncropped) {
        p_pisces_pass_source[side]->lines.v = img_side_params[side].scan_length_lines;
    } else {
        p_pisces_pass_source[side]->lines.v = edge->page_end_lines;
    }
    fill = img_get_fill_byte(window->reverse_image.f.on, 0);
    p_pisces_pass_source[side]->fill.v = fill;
    p_pisces_pass_source[side]->fill_cb.lo = 0x80;
    p_pisces_pass_source[side]->fill_cr.lo = 0x80;
    if (page_side_state[side].uncropped) {
        p_pisces_pass_source[side]->reg_076.f = 8;
    } else {
        p_pisces_pass_source[side]->reg_076.f = 0;
    }
    _builtin_set_imask(saved_imask);
}

void edge_state_1000_c(int side, unsigned saved_imask, struct edge_record_words *edge)
{
    _builtin_set_imask(15);
    if (edge_top_corners[side].left_lower) {
        p_pisces_pass_source[side]->skew_on.left_lower = 1;
    } else {
        p_pisces_pass_source[side]->skew_on.left_lower = 0;
    }
    p_pisces_pass_source[side]->skew_0 = edge_page_geometry[side].skew_angle;
    p_pisces_pass_source[side]->skew_1 = edge_page_geometry[side].skew_angle;
    if (side == 0) {
        if (edge->flags & EDGE_FLAG_FALLBACK) {
            page_records[page_scanning.v].driver_crop = 1;
        } else {
            page_records[page_scanning.v].driver_crop = 0;
        }
    }
    _builtin_set_imask(saved_imask);
}

void edge_state_1000_d(int side, unsigned saved_imask, struct edge_record_words *edge)
{
    _builtin_set_imask(15);
    if (edge_page_geometry[side].skew_angle == 0) {
        p_pisces_zoom[side]->x_zoom_permille.v = img_side_params[side].x_zoom_permille;
        p_pisces_zoom[side]->y_zoom_permille.v = img_side_params[side].y_zoom_permille;
    } else {
        p_pisces_zoom[side]->x_zoom_permille.v = edge_page_geometry[side].x_zoom_permille;
        p_pisces_zoom[side]->y_zoom_permille.v = edge_page_geometry[side].y_zoom_permille;
    }
    p_pisces_zoom[side]->reg_144.v = 0;
    p_pisces_zoom[side]->reg_146 = 0;
    p_pisces_zoom[side]->width_px.v = edge_page_geometry[side].width_px;
    p_pisces_zoom[side]->length_lines = edge_page_geometry[side].length_lines + 5;
    if (testmode_number.v == 0xf) {
        p_pisces_zoom[side]->length_lines += 10;
    }
    if (edge->flags & EDGE_FLAG_FALLBACK) {
        if (job_auto_size_flags.b7 || job_auto_size_flags.b6) {
            if (win_params.y_res_dpi.v < 0x96) {
                img_edge_state_1000_d_step_1(side, 0x7005);
            } else if (win_params.y_res_dpi.v < 0xf0) {
                img_edge_state_1000_d_step_1(side, 0x8005);
            } else if (win_params.y_res_dpi.v < 0x1f4) {
                img_edge_state_1000_d_step_1(side, 0x9005);
            } else {
                img_edge_state_1000_d_step_1(side, 0xa005);
            }
        } else {
            img_edge_state_1000_d_step_1(side, 0x5005);
        }
    } else {
        img_edge_state_1000_d_step_1(side, 0x5005);
    }
    _builtin_set_imask(saved_imask);
}

void edge_state_1000_e(int side, unsigned saved_imask, struct edge_record_words *edge)
{
    struct page_record *scanning_page;
    struct win_side *window;
    unsigned char fill;
    scanning_page = &page_records[page_scanning.v];
    window = &win_params.side[side];
    _builtin_set_imask(15);
    p_pisces_pass_dest[side]->base_64k.f.v = scanning_page->buf_offset >> 16;
    p_pisces_pass_dest[side]->size_64k.v = page_buf_bytes.v >> 16;
    p_pisces_pass_dest[side]->width_px.v = edge_page_geometry[side].width_px;
    p_pisces_pass_dest[side]->length_lines.f.v = edge_page_geometry[side].length_lines;
    scanning_page->sides[side].width_px = edge_page_geometry[side].width_px;
    scanning_page->sides[side].length_lines = edge_page_geometry[side].length_lines;
    if (page_buf_limit_bytes) {
        p_pisces_pass_dest[side]->reg_09a.f = 18;
    } else {
        p_pisces_pass_dest[side]->reg_09a.f = 0;
    }
    p_pisces_pass_dest[side]->reg_0a0.b15 = 0;
    if ((settings_byte_0xca[0] & 0x10) || (settings_emulation[0] & 0x1c) == 0xc || (settings_emulation[0] & 0x1c) == 0x10
        || (settings_emulation[0] & 0x1c) == 0x1c || (settings_emulation[0] & 0x1c) == 0x18) {
    } else {
        if (job_auto_size_flags.b7 && !job_auto_size_flags.b5 && !job_auto_size_flags.b4 && (window->compression.f.type & 0x80)
            && !(edge->flags & EDGE_FLAG_FALLBACK)) {
            scanning_page->sides[side].paper_width_px = edge_page_geometry[side].paper_width_px;
            scanning_page->sides[side].paper_length_lines = edge_page_geometry[side].paper_length_lines;
        }
    }
    if (job_auto_size_flags.b7 && !job_auto_size_flags.b5 && !(edge->flags & EDGE_FLAG_FALLBACK)) {
        scanning_page->sides[side].width_unaligned_px = edge_page_geometry[side].width_unaligned_px;
        scanning_page->sides[side].length_unpadded_lines = edge_page_geometry[side].length_unpadded_lines;
    }
    _builtin_set_imask(saved_imask);
    if (job_auto_size_flags.b7 && !(edge->flags & EDGE_FLAG_FALLBACK)) {
        edge_state_1000_e_step_1(side, edge_page_geometry[side].width_px);
        edge->state = EDGE_STATE_DONE;
        edge->flags |= EDGE_FLAG_SIZE_FINAL;
        edge_update_output_lines(side, saved_imask);
        scanning_page->sides[side].size_final = 1;
    } else {
        edge_state_1000_e_step_3(side, edge_page_geometry[side].width_px, edge);
        if (!win_params.flags.f.adf || job_auto_size_flags.b7 || job_auto_size_flags.b6) {
            edge->flags |= EDGE_FLAG_SIZE_FINAL;
            scanning_page->sides[side].size_final = 1;
            edge->state = EDGE_STATE_DONE;
        } else {
            edge->state = EDGE_STATE_WAIT_BOTTOM;
        }
    }
    fill = img_get_fill_byte(window->reverse_image.f.on, 1);
    if (!window->composition.f.multi_level) {
        if (fill == 0) {
            fill = 0;
        } else {
            fill = 0xff;
        }
    }
    _builtin_set_imask(15);
    p_pisces_pass_dest[side]->fill.v = fill;
    p_pisces_pass_dest[side]->fill_cb.lo = 0x80;
    p_pisces_pass_dest[side]->fill_cr.lo = 0x80;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    p_pisces_mode[side]->run.f.b8 = 1;
    if (testmode_number.v == 0xf) {
        p_pisces_mode[side]->run.f.b14 = 1;
    }
    p_pisces_mode[side]->run.f.b4 = 1;
    _builtin_set_imask(saved_imask);
}

void edge_state_1000_e_step_1(int side, short width_px)
{
    int x1, x2;
    int saved_imask;
    struct page_record *scanning_page;
    saved_imask = _builtin_get_imask();
    scanning_page = &page_records[page_scanning.v];
    _builtin_set_imask(15);
    p_pisces_pass_dest[side]->x_start.v = 0;
    p_pisces_pass_dest[side]->x_end.v = width_px;
    p_pisces_pass_dest[side]->y_start = 0;
    p_pisces_pass_dest[side]->y_end = edge_page_geometry[side].length_lines;
    _builtin_set_imask(saved_imask);
}

void edge_state_1000_e_step_3(int side, short width_px, struct edge_record_words *edge)
{
    unsigned short left, right, limit, max_x, top, bottom, bottom_lines, y_end;
    int saved_imask;
    struct win_param_block *params;
    struct page_record *scanning_page;
    saved_imask = _builtin_get_imask();
    scanning_page = &page_records[page_scanning.v];
    params = &win_params;
    img_split_nibbles(side, settings_filler_flatbed_right_left[0], settings_filler_adf_right_left[0], &left, &right);
    limit = left * 3;
    if ((params->side[side].use_window_area != 0 && limit < params->side[side].x_offset_inch1200) || job_keep_backing != 0 || (settings_byte_0x34[0] & 1)) {
        left = 0;
        right = 0;
    }
    if (left != 0) {
        left = rt_divu16(left * params->x_res_dpi.w, 400);
    }
    if (right != 0) {
        right = rt_divu16(right * params->x_res_dpi.w, 400);
    }
    _builtin_set_imask(15);
    p_pisces_pass_dest[side]->x_start.v = left;
    p_pisces_pass_dest[side]->x_end.v = width_px - right;
    _builtin_set_imask(saved_imask);
    if (img_side_params[side].clip_width_px != 0 && job_keep_backing == 0 && !(settings_byte_0x34[0] & 1)) {
        max_x = rt_divu16(img_side_params[side].clip_width_px * win_params.x_res_dpi.v, img_side_params[side].x_res_dpi);
        _builtin_set_imask(15);
        if (p_pisces_pass_dest[side]->x_end.v > max_x) {
            p_pisces_pass_dest[side]->x_end.v = max_x;
        }
        _builtin_set_imask(saved_imask);
    }
    img_split_nibbles(side, settings_filler_flatbed_top_bottom[0], settings_filler_adf_top_bottom[0], &bottom, &top);
    limit = top * 3;
    if ((params->side[side].use_window_area != 0 && limit < (unsigned)params->side[side].y_offset_inch1200) || job_keep_backing != 0 || (settings_byte_0x34[0] & 1)) {
        top = 0;
        bottom = 0;
    }
    if (top != 0) {
        top = rt_divu16(top * params->y_res_dpi.v, 400);
    }
    if (bottom != 0) {
        bottom_lines = rt_divu16(bottom * params->y_res_dpi.v, 400);
        if (win_params.flags.f.adf) {
            if (bottom & 0x80) {
                bottom_lines = 0;
                bottom &= 0xff7f;
                bottom = rt_divu16(bottom * params->y_res_dpi.v, 400);
                bottom = ~bottom + 1;
            } else {
                bottom = bottom_lines;
            }
            edge_page_geometry[side].bottom_margin_lines = bottom;
        } else {
            edge_page_geometry[side].bottom_margin_lines = bottom_lines;
        }
        y_end = edge_page_geometry[side].length_lines - bottom_lines;
    } else {
        y_end = edge_page_geometry[side].length_lines;
        edge_page_geometry[side].bottom_margin_lines = 0;
    }
    _builtin_set_imask(15);
    p_pisces_pass_dest[side]->y_start = top;
    p_pisces_pass_dest[side]->y_end = y_end;
    _builtin_set_imask(saved_imask);
}

unsigned short edge_compute_paper_length(int side, unsigned short source_lines)
{
    unsigned short length_lines;
    struct win_side *window;
    window = &win_params.side[side];
    source_lines = edge_scale_lines(side, source_lines);
    length_lines = rt_divu16(img_side_params[side].y_zoom_permille * source_lines + 555, 1000);
    edge_page_geometry[side].paper_length_lines = length_lines;
    edge_page_geometry[side].length_unpadded_lines = edge_page_geometry[side].paper_length_lines;
    length_lines = edge_align_lines(length_lines, window->compression.f.type & 128);
    if (length_lines < img_side_params[side].output_length_lines)
        return length_lines;
    else {
    edge_page_geometry[side].paper_length_lines = img_side_params[side].output_length_lines;
    edge_page_geometry[side].length_unpadded_lines = edge_page_geometry[side].paper_length_lines;
    return img_side_params[side].output_length_lines;
    }
}
