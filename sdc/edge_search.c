#define line_side_state line_side_state_hdr
#include "fw.h"
#undef line_side_state
struct edge_record;


extern void _builtin_set_imask(int);

extern volatile unsigned short line_side_state;


extern int _builtin_get_imask(void);

struct edge_level_bytes {
    unsigned char level_40, half_peak_40, level_64, half_peak_64;
};

void edge_run_line_step(int side, int saved_imask, struct edge_record *edge)
{
    short line;
    unsigned short step_index;

    line = line_captured_lines[side].w;
    if (line_page_state[side].auto_size != 0 && edge->state != EDGE_STATE_START) {
        step_index = (unsigned short)line / edge_line_step;
        if (step_index == edge_last_line[side])
            return;
        else if (step_index > (edge_last_line[side] + 1) && edge->state == EDGE_STATE_COLLECT) {
            edge->search_failed = 1;
            settings_edge_fail_code[0] = 52;
        }
        edge_last_line[side] = step_index;
    }
    if (edge->state < EDGE_STATE_COLLECT)
        edge_run_setup_state(side, saved_imask, (void *)edge);
    else
        edge_run_search_state(side, saved_imask, line, edge);
}

void edge_run_page_step(int side, int saved_imask, unsigned short *edge)
{
    unsigned char output_buf;

    if (win_sides.v != 0 && line_page_state[side].auto_size != 0) {
        output_buf = page_side_state[side].output_buf;
        if (side == 0) {
            if (*edge == EDGE_STATE_APPLY && ((struct edge_record *)&edge_side_records[1])[output_buf].state < EDGE_STATE_APPLY)
                return;
        } else {
            if ((*edge == EDGE_STATE_SKEW_DIRECTION || *edge == EDGE_STATE_FALLBACK) && ((struct edge_record *)edge_side_records)[output_buf].state < EDGE_STATE_APPLY)
                return;
            if (*edge == EDGE_STATE_APPLY && ((struct edge_record *)edge_side_records)[output_buf].state != EDGE_STATE_DONE)
                return;
        }
    }
    if (*edge < EDGE_STATE_SKEW_DIRECTION)
        return;
    else if (*edge < EDGE_STATE_FALLBACK)
        edge_run_page_end_state(side, saved_imask, edge);
    else if (*edge < EDGE_STATE_WAIT_BOTTOM)
        edge_run_result_state(side, saved_imask, edge);
    else
        edge_state_8000(side, saved_imask, edge);
}

void edge_end_long_page(int side, int saved_imask)
{
    unsigned short lines;
    unsigned short length_lines;

    lines = line_captured_lines[side].w;
    if (img_side_params->y_res_dpi == 300)
        lines -= 2;
    length_lines = rt_divu16(lines * img_side_params[side].y_zoom_permille + 0x22b, 1000);
    _builtin_set_imask(15);
    p_pisces_pass_dest[side]->ctrl.enable = 1;
    p_pisces_pass_dest[side]->y_end = 0xffff;
    edge_page_geometry[side].length_lines = length_lines;
    p_pisces_pass_dest[side]->length_lines.f.v = edge_page_geometry[side].length_lines;
    p_pisces_pass_source[side]->reg_076.f = 0;
    _builtin_set_imask(saved_imask);
    page_records[page_scanning.v].sides[side].length_lines = edge_page_geometry[side].length_lines;
    edge_update_output_lines(side, saved_imask);
    _builtin_set_imask(15);
    p_pisces_pass_dest[side]->ctrl.enable = 0;
    _builtin_set_imask(saved_imask);
    page_records[page_scanning.v].sides[side].size_final = 1;
    edge_side_records[side].e[0].state = EDGE_STATE_DONE;
    edge_side_records[side].e[0].flags |= EDGE_FLAG_SIZE_FINAL;
}

void edge_run_setup_state(int side, int saved_imask, unsigned short *edge)
{
    switch (*edge) {
    case EDGE_STATE_START: edge_state_00(side, saved_imask, edge); break;
    case EDGE_STATE_THRESHOLDS: edge_state_04(side, saved_imask, edge); break;
    case EDGE_STATE_LEVEL_WATCH: edge_state_08(side, saved_imask, edge); break;
    case EDGE_STATE_LEVEL_ADJUST: edge_state_0c(side, saved_imask, edge); break;
    }
}

void edge_state_00(int side, int saved_imask, unsigned short *edge)
{
    short i;
    unsigned short min_left;

    util_mem_clear(&edge_top_search[side], 10);
    util_mem_clear(&edge_width_track[side], 32);
    edge_search_hits = 0;
    edge_right_limit_hits = 0;
    if (win_params.flags.f.adf == 0) {
        switch (img_side_params[side].x_res_dpi) {
        case 600: edge_search_steps.v = 12; break;
        case 400: edge_search_steps.v = 8; break;
        case 300: edge_search_steps.v = 6; break;
        }
        edge_search_start_px.v = edge_search_steps.v << 3;
        if (settings_byte_0x17[0])
            min_left = settings_byte_0x17[0];
        else
            min_left = 18;
        edge_search_min_left.v = rt_divu16(min_left * img_side_params[side].x_res_dpi, 600);
        for (i = 0; i < (unsigned)edge_search_steps.v; i++)
            edge_search_offsets[(unsigned short)i] = edge_search_start_px.v - i * 8;
        _builtin_set_imask(15);
        p_pisces_ctrl[side]->ctrl.f.sdram_window = 1;
        _builtin_set_imask(saved_imask);
        edge_right_limit = p_vdcc_capture[side]->margin_end_px.v - 5;
        edge_flatbed_limit_lines.v = edge_inch1200_to_lines(side, 0x3b0);
    } else {
        edge_right_limit = 0xffff;
    }
    _builtin_set_imask(15);
    p_pisces_edge[side]->ctrl.f = 1;
    p_pisces_edge[side]->ctrl.g = 7;
    _builtin_set_imask(saved_imask);
    edge_width_track[side].edge_inset_px = 2;
    if (img_side_params[side].y_res_dpi > 300)
        edge_line_step = 8;
    else
        edge_line_step = 4;
    *edge = EDGE_STATE_THRESHOLDS;
}

void edge_state_04(int side, int saved_imask, unsigned short *edge)
{
    struct edge_level_bytes levels;
    unsigned short margin;

    img_edge_state_04_step_1(side, &levels);
    _builtin_set_imask(15);
    p_pisces_edge[side]->threshold_1.lo = levels.level_40;
    p_pisces_edge[side]->threshold_1.hi = levels.level_40;
    p_pisces_edge[side]->threshold_2.lo = levels.half_peak_40;
    p_pisces_edge[side]->threshold_2.hi = levels.half_peak_40;
    p_pisces_edge[side]->threshold_3.lo = levels.half_peak_40;
    p_pisces_edge[side]->threshold_3.hi = levels.half_peak_40;
    edge_search_level.v = levels.level_40;
    _builtin_set_imask(saved_imask);
    edge_level_search[side].level_40 = levels.level_40;
    edge_level_search[side].half_peak_40 = levels.half_peak_40;
    edge_level_search[side].level_64 = levels.level_64;
    edge_level_search[side].half_peak_64 = levels.half_peak_64;
    edge_level_search[side].dark_level = levels.level_40 / 2;
    edge_level_search[side].watch_lines = edge_inch1200_to_lines(side, 0x1d8);
    edge_level_search[side].line_count = 0;
    edge_level_search[side].dark_count = 0;
    edge_level_search[side].level_sum.v = 0;
    if (win_params.flags.f.adf != 0) {
        margin = rt_divu16(img_side_params[side].x_res_dpi * 0x1d8, 600);
        margin >>= 1;
        _builtin_set_imask(15);
        p_pisces_edge[side]->x_start.v = img_side_params[side].capture_width_px / 2 - margin;
        p_pisces_edge[side]->x_end.v = img_side_params[side].capture_width_px / 2 + margin;
        _builtin_set_imask(saved_imask);
        *edge = EDGE_STATE_LEVEL_ADJUST;
    } else {
        margin = rt_divu16(img_side_params[side].x_res_dpi * 118, 600);
        _builtin_set_imask(15);
        p_pisces_edge[side]->x_start.v = margin;
        p_pisces_edge[side]->x_end.v = img_side_params[side].capture_width_px - margin;
        _builtin_set_imask(saved_imask);
        *edge = EDGE_STATE_LEVEL_WATCH;
    }
}

void edge_state_08(int side, int saved_imask, unsigned short *edge)
{
    unsigned char level;
    unsigned short width_px;
    unsigned short margin_px;

    if ((unsigned)edge_level_search[side].line_count > edge_level_search[side].watch_lines) {
        _builtin_set_imask(15);
        p_pisces_edge[side]->x_start.v = edge_search_start_px.v;
        p_pisces_edge[side]->x_end.v = img_side_params[side].capture_width_px;
        _builtin_set_imask(saved_imask);
        edge_top_search[side].start_width_px = 0;
        *edge = EDGE_STATE_COLLECT;
        return;
    }
    {
        edge_level_search[side].line_count += 1;
        level = p_pisces_edge[side]->level;
        if (level < (unsigned char)edge_level_search[side].dark_level) {
            edge_level_search[side].dark_count += 1;
            if ((unsigned short)edge_level_search[side].dark_count > 2) {
                width_px = rt_divu16(img_side_params[side].x_res_dpi * 0x1d8, 600);
                margin_px = rt_divu16(img_side_params[side].x_res_dpi * 0xbd, 600);
                _builtin_set_imask(15);
                p_pisces_edge[side]->x_start.v = img_side_params[side].capture_width_px - margin_px - width_px;
                p_pisces_edge[side]->x_end.v = img_side_params[side].capture_width_px - margin_px;
                _builtin_set_imask(saved_imask);
                edge_level_search[side].line_count = 0;
                *edge = EDGE_STATE_LEVEL_ADJUST;
            }
        } else {
            edge_level_search[side].dark_count = 0;
        }
    }
}

void edge_state_0c(int side, int saved_imask, unsigned short *edge)
{
    unsigned short level_word;
    unsigned char lo;
    unsigned char hi;
    unsigned char threshold_1;
    unsigned char threshold_2;

    level_word = p_pisces_edge[side]->level;
    hi = level_word >> 8;
    lo = level_word;
    edge_level_search[side].level_sum.v += (unsigned short)(hi + lo) / 2;
    if ((unsigned short)edge_level_search[side].line_count < 4) {
        edge_level_search[side].line_count += 1;
        return;
    }
    edge_level_search[side].level_sum.v >>= 2;
    if ((unsigned short)edge_level_search[side].level_sum.v > (unsigned char)edge_level_search[side].dark_level
        && (unsigned)edge_level_search[side].level_64 > edge_level_search[side].level_40) {
        edge_level_search[side].level_sum.v <<= 1;
        if ((unsigned short)edge_level_search[side].level_sum.v > (unsigned char)edge_level_search[side].level_64) {
            threshold_1 = edge_level_search[side].level_64;
            threshold_2 = edge_level_search[side].half_peak_64;
        } else {
            threshold_1 = edge_level_search[side].level_sum.c.lo;
            threshold_2 = ((unsigned char)edge_level_search[side].half_peak_64 - (unsigned char)edge_level_search[side].half_peak_40) * (threshold_1 - (unsigned char)edge_level_search[side].level_40)
                 / ((unsigned char)edge_level_search[side].level_64 - (unsigned char)edge_level_search[side].level_40);
            threshold_2 += edge_level_search[side].half_peak_40;
        }
        _builtin_set_imask(15);
        p_pisces_edge[side]->threshold_1.lo = threshold_1;
        p_pisces_edge[side]->threshold_1.hi = threshold_1;
        p_pisces_edge[side]->threshold_2.lo = threshold_2;
        p_pisces_edge[side]->threshold_2.hi = threshold_2;
        p_pisces_edge[side]->threshold_3.lo = threshold_2;
        p_pisces_edge[side]->threshold_3.hi = threshold_2;
        _builtin_set_imask(saved_imask);
        edge_search_level.v = threshold_1;
    }
    if (win_params.flags.f.adf != 0) {
        _builtin_set_imask(15);
        p_pisces_edge[side]->x_start.v = 0;
        p_pisces_edge[side]->x_end.v = img_side_params[side].capture_width_px;
        _builtin_set_imask(saved_imask);
    } else {
        _builtin_set_imask(15);
        p_pisces_edge[side]->x_start.v = edge_search_start_px.v;
        p_pisces_edge[side]->x_end.v = img_side_params[side].capture_width_px;
        _builtin_set_imask(saved_imask);
    }
    _builtin_set_imask(15);
    edge[8] = 0;
    ((struct edge_record *)edge)->search_failed = 0;
    _builtin_set_imask(saved_imask);
    edge_top_search[side].start_width_px = 0;
    *edge = EDGE_STATE_COLLECT;
}

void edge_run_search_state(int side, int saved_imask, short line, struct edge_record *edge)
{
    unsigned short index;

    if (win_params.flags.f.adf != 0)
        edge_search_result.v = 0xffff;
    else
        edge_search_line(side, line);
    edge_read_line(side, line, (unsigned short *)edge);
    if (job_auto_size_flags.b7) {
        if (edge->top_line != 0) {
            index = ((unsigned short)line - edge->top_line) / edge_line_step;
            if (index > 0x578)
                index = 0x577;
        } else {
            index = 0;
        }
        if (edge->left_edges == 0 || edge->right_edges == 0)
            return;
        edge->left_edges[index] = edge_line[side].left;
        edge->right_edges[index] = edge_line[side].right;
    }
    switch (edge->state) {
    case EDGE_STATE_COLLECT: edge_state_10(side, saved_imask, line, edge); break;
    case EDGE_STATE_WIDTH_TRACK: edge_state_14(side, saved_imask, line, edge); break;
    }
}

void edge_state_10(int side, int saved_imask, short line, struct edge_record *edge)
{
    short width;

    if (edge_top_search[side].start_width_px == 0) {
        edge->start_width_px = edge_inch1200_to_px(side, 0xec);
        edge_top_search[side].start_width_px = edge->start_width_px;
        edge_top_search[side].confirm_count = 0;
        edge_top_search[side].min_width_px = edge_inch1200_to_px(side, 0x60);
        edge_top_search[side].max_width_px = edge_inch1200_to_px(side, 0x3900);
        edge_top_search[side].confirm_limit = edge_inch1200_to_lines(side, 0xbc);
        edge_top_search[side].confirm_limit >>= 2;
        switch (img_side_params[side].x_res_dpi) {
        case 600: edge_top_confirm_lines[side] = 48; break;
        case 400: edge_top_confirm_lines[side] = 32; break;
        case 300: edge_top_confirm_lines[side] = 24; break;
        default: edge_top_confirm_lines[side] = 16; break;
        }
        edge_top_search[side].confirm_limit += edge_top_confirm_lines[side];
    }
    width = edge_line[side].right - edge_line[side].left;
    if ((unsigned short)width < edge_top_search[side].min_width_px || (unsigned)width > edge_top_search[side].max_width_px) {
        edge_top_search[side].start_width_px = edge->start_width_px;
        if ((unsigned short)edge_top_search[side].confirm_count > 0) {
            edge_top_search[side].confirm_count -= 1;
            if (edge_top_search[side].confirm_count == 0 && edge->top_line != 0) {
                edge->search_failed = 1;
                settings_edge_fail_code[0] = 50;
            }
        } else {
            edge_top_search[side].confirm_count = 0;
        }
    } else if ((unsigned)width > edge_top_search[side].start_width_px) {
        if (edge_top_search[side].confirm_count == 0) {
            edge->top_line = line;
            if (edge->top_line % edge_line_step && edge->top_line > 2) {
                edge->top_line /= edge_line_step;
                edge->top_line *= edge_line_step;
            }
            edge->top_right_px = edge_line[side].right;
            edge->top_left_px = edge_line[side].left;
            edge_top_search[side].confirm_count = edge_top_confirm_lines[side];
        }
        if ((unsigned)edge_top_search[side].confirm_count > (short)edge_top_search[side].confirm_limit) {
            edge_width_track[side].width_count = 1;
            edge_width_track[side].width_sum = (unsigned short)width;
            edge->top_corner_line = 0;
            edge->state = EDGE_STATE_WIDTH_TRACK;
        } else {
            edge_top_search[side].confirm_count += 1;
        }
    }
}

void edge_state_14(int side, int saved_imask, short line, struct edge_record *edge)
{
    short width;
    short tolerance;

    width = edge_line[side].right - edge_line[side].left;
    if ((unsigned short)edge_width_track[side].width_count == 1) {
        edge_width_track[side].settle_lines = edge_inch1200_to_lines(side, 0xec);
        edge_width_track[side].settle_lines >>= 2;
        tolerance = edge_inch1200_to_px(side, 0x90);
        edge_width_track[side].min_width_px = edge_width_track[side].width_sum - tolerance;
        edge_width_track[side].max_width_px = edge_width_track[side].width_sum + tolerance;
        edge_width_track[side].bottom_search_line = edge_inch1200_to_lines(side, 0x675);
        edge_width_track[side].bottom_search_line += edge->top_line;
        edge_width_track[side].bottom_search_line >>= 2;
        edge->bottom_corner_line = 0;
        edge->bottom_line = 0;
        edge->last_width_px = edge->width_px;
        edge->width_px = 0;
        edge_width_track[side].lost_limit = edge_inch1200_to_lines(side, 0x93a);
        edge_width_track[side].lost_limit /= edge_line_step;
        edge_width_track[side].present_width_px = edge_inch1200_to_px(side, 0x3b0);
        edge_width_track[side].scan_length_lines = img_side_params[side].scan_length_lines;
    }
    edge_width_track[side].width_count += 1;
    edge_width_track[side].width_sum += (unsigned short)width;
    if ((unsigned)width > edge_width_track[side].min_width_px && (unsigned)width < edge_width_track[side].max_width_px) {
        if ((unsigned)edge_width_track[side].width_count > (short)edge_width_track[side].settle_lines && edge->top_corner_line == 0) {
            edge->top_corner_line = line - edge_width_track[side].settle_lines;
            edge_width_track[side].width_sum /= (unsigned short)edge_width_track[side].width_count;
            edge->width_px = edge_width_track[side].width_sum;
            tolerance = edge_inch1200_to_px(side, 0x90);
            edge_width_track[side].min_width_px = edge_width_track[side].width_sum - tolerance;
            edge_width_track[side].max_width_px = edge_width_track[side].width_sum + tolerance;
        }
        edge_width_track[side].narrow_count = 0;
        edge_width_track[side].lost_count = 0;
        edge->bottom_line = 0;
    } else {
        if (edge->top_corner_line == 0 && (unsigned)width > edge_width_track[side].min_width_px) {
            edge_width_track[side].width_sum += (unsigned short)width;
            edge_width_track[side].width_sum /= (unsigned short)edge_width_track[side].width_count + 1;
            edge_width_track[side].width_count = 1;
        }
        if ((unsigned)line > (short)edge_width_track[side].bottom_search_line)
            edge_state_14_step_3(side, width, line, edge);
        else
            {}
    }
}

void edge_state_14_step_3(int side, short width, short line, struct edge_record *edge)
{
    if ((unsigned)width < edge_width_track[side].min_width_px) {
        if ((unsigned short)edge_width_track[side].narrow_count > 32) {
            if (edge->top_corner_line == 0)
                edge->top_corner_line = line + 0xff80;
            if (edge->bottom_corner_line == 0)
                edge->bottom_corner_line = line + 0xff80;
            if (edge->width_px == 0)
                edge->width_px = edge_width_track[side].width_sum / (unsigned short)edge_width_track[side].width_count;
        } else {
            edge_width_track[side].narrow_count += 1;
        }
    } else {
        edge_width_track[side].narrow_count = 0;
    }
    if ((unsigned short)width == 0xffff) {
        if (job_auto_size_flags.b7 && edge->top_corner_line == 0)
            return;
        else if (win_params.flags.f.adf != 0) {
            if ((line_side_state & 0x4000)) if (edge->bottom_line == 0)
                edge->bottom_line = line;
        } else {
            if ((unsigned)edge_width_track[side].lost_count > (short)edge_width_track[side].lost_limit) {
                if (edge->bottom_line == 0)
                    edge->bottom_line = line - edge_width_track[side].lost_count * edge_line_step;
            } else {
                edge_width_track[side].lost_count += 1;
            }
        }
    } else {
        if ((unsigned)width > edge_width_track[side].present_width_px) {
            edge_width_track[side].lost_count = 0;
            edge->bottom_line = 0;
        }
    }
}

void edge_search_line(int side, short line)
{
    int saved_imask;
    struct pisces_sdram_pixel *pixel;
    struct pisces_sdram_pixel *probe;
    unsigned int offset;
    unsigned int page;
    unsigned int j;
    unsigned int i;
    unsigned int size;
    unsigned short sum;
    unsigned char level;

    saved_imask = _builtin_get_imask();
    pixel = p_pisces_front_sdram.s;
    edge_search_result.v = 0xffff;
    if ((unsigned short)line > 2)
        line -= 2;
    offset = img_side_params[side].capture_width_px * (unsigned short)line;
    size = p_pisces_capture[side]->size_64k.v;
    size = size << 15;
    if (offset > size)
        offset = offset - size;
    page = offset >> 15;
    _builtin_set_imask(15);
    p_pisces_ctrl[side]->sdram_page = page;
    _builtin_set_imask(saved_imask);
    offset &= 0x7fff;
    pixel += offset + (unsigned short)edge_search_start_px.v;
    probe = pixel;
    sum = 0;
    for (i = 0; i < 4; i++) {
        level = (probe++)->b;
        sum += level;
    }
    sum >>= 2;
    if (sum > edge_search_level.v) {
        for (j = 0; j < (unsigned short)edge_search_steps.v; j++) {
            sum = 0;
            for (i = 0; i < 8; i++) {
                level = (pixel--)->b;
                sum += level;
            }
            sum >>= 3;
            if (sum < edge_search_level.v) {
                edge_search_result.v = *(unsigned short *)(edge_search_offsets + j);
                break;
            }
        }
        if (j == (unsigned short)edge_search_steps.v)
            edge_search_result.v = 4;
        if ((unsigned)(short)edge_search_result.v < (unsigned)edge_search_min_left.v)
            edge_search_result.v = edge_search_min_left.v;
        edge_search_hits += 1;
    }
}

/* Pisces +0x1B0 (p_pisces_edge): +18 left paper edge of the line (0 = none), +20 right edge (0x1FFF = none). */
void edge_read_line(int side, short line, unsigned short *edge)
{
    unsigned short left;
    unsigned short right;
    int z;

    z = 0;
    if (p_pisces_edge[side]->left_edge == 0)
        left = 0xffff;
    else
        left = p_pisces_edge[side]->left_edge;
    if ((unsigned short)p_pisces_edge[side]->right_edge == 0x1fff)
        right = 0;
    else
        right = p_pisces_edge[side]->right_edge;
    if (edge_search_result.v != 0xffff) {
        left = edge_search_result.v;
        if (left == 0)
            left = 0xffff;
    }
    if ((unsigned)(short)right > (short)edge_right_limit)
        edge_right_limit_hits += 1;
    if (left != 0xffff && right != 0) {
        if ((*edge < EDGE_STATE_WIDTH_TRACK && left + 2 < right) || *edge > EDGE_STATE_COLLECT) {
            if (left != 0)
                left += edge_width_track[side].edge_inset_px;
            if (right != 0x1fff && (unsigned)(short)right > (short)edge_width_track[side].edge_inset_px)
                right = right - edge_width_track[side].edge_inset_px;
        }
        if (left + 2 > right) {
            left = 0xffff;
            right = 0;
        }
    } else {
        left = 0xffff;
        right = 0;
    }
    if (left == 0xffff) {
        edge_line[side].left = 0;
        edge_line[side].right = 0xffff;
    } else {
        edge_line[side].left = left;
        edge_line[side].right = right;
    }
}
