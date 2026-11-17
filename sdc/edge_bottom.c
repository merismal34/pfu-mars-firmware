#include "fw.h"

  /* the 44-byte request block; same layout in edge_fit.c (T) and edge_bottom.c (C) */
extern int abs(int);
struct edge_record_state;

void edge_run_substate(int side, int mode, int rising, void *edge)
{
    switch (edge_substate[side]) {
    case 0:
        edge_substate_0(side, rising);
        break;
    case 0x10:
        edge_substate_10(side, mode, rising, (struct edge_record *)edge);
        break;
    case 0x100:
        edge_substate_100(side, (struct edge_record_state *)edge);
        break;
    case 0x400:
        edge_substate_400(side, (unsigned short *)edge);
        break;
    case 0x1000:
        edge_substate_1000(side, mode, rising, (struct edge_record *)edge);
        break;
    case 0x4000:
        edge_substate_0(side, rising);
        break;
    }
}

void edge_substate_0(int side, int rising)
{
    double offset;
    int one;

    one = 1;
    if (edge_top_line[side].run != 0 && edge_top_line[side].rise != 0) {
        offset = (double)edge_side_line_sums[side].sum_y - (double)edge_side_line_sums[side].sum_x * (double)edge_top_line[side].run / (double)edge_top_line[side].rise;
        offset /= (double)edge_side_line_sums[side].count;
        if (offset < 8388600.0 && offset > -8388600.0) {
            if (rising != 0)
                edge_top_line[side].right_intercept = edge_round(offset);
            else
                edge_top_line[side].left_intercept = edge_round(offset);
            goto check;
        }
        edge_top_line[side].run = 0;
        edge_top_line[side].rise = 0;
    }
check:
    if (edge_top_line[side].run == 0 || edge_top_line[side].rise == 0) {
        if (rising != 0)
            edge_top_line[side].right_intercept = edge_side_line_sums[side].sum_x / edge_side_line_sums[side].count;
        else
            edge_top_line[side].left_intercept = edge_side_line_sums[side].sum_x / edge_side_line_sums[side].count;
    }
    if (edge_substate[side] != 0x4000)
        edge_substate[side] = 0x10;
    else
        edge_substate[side] = 0x8000;
}

void edge_substate_10(int side, int mode, int rising, struct edge_record *edge)
{
    unsigned short *edges;
    unsigned short count;

    if (rising != 0)
        edges = edge->right_edges;
    else
        edges = edge->left_edges;
    if (edge_sample_pos[side] == 0)
        util_mem_clear(edge_sample_values, 0x100);
    count = 0x20;
    if (edge_sample_pos[side] + count > edge_side_line_sums[side].sample_count && (unsigned short)edge_side_line_sums[side].sample_count >= edge_sample_pos[side])
        count = edge_side_line_sums[side].sample_count - edge_sample_pos[side];
    if (edge_top_line[side].run != 0 && edge_top_line[side].rise != 0)
        edge_substate_10_step_2(edge_sample_values + edge_sample_pos[side], count, edge_samples + edge_sample_pos[side], side, rising, mode, edges, edge);
    else
        edge_substate_10_step_3(edge_sample_values + edge_sample_pos[side], count, edge_samples + edge_sample_pos[side], side, rising, edges);
    edge_sample_pos[side] += count;
    if (edge_sample_pos[side] >= edge_side_line_sums[side].sample_count) {
        edge_sample_pos[side] = 0;
        edge_substate[side] = 0x100;
    }
}

void edge_substate_100(int side, struct edge_record_state *edge)
{
    unsigned short max;
    unsigned short min;
    unsigned short min_range;
    int i;
    int count;
    unsigned short *walk;
    unsigned short max_2;
    unsigned short max_3;
    unsigned short min_2;
    unsigned short min_3;
    unsigned short edge_w2;
    unsigned short limit;

    count = edge_side_line_sums[side].sample_count;
    walk = edge_sample_values;
    if (side == 0) {
        max = max_2 = max_3 = 0;
        min = min_2 = min_3 = 0xffff;
        for (i = 0; i < count; walk++, i++) {
            if (*walk > max_3) {
                if (*walk > max_2) {
                    if (*walk > max) {
                        max_3 = max_2;
                        max_2 = max;
                        max = *walk;
                    } else if (*walk != max) {
                        max_3 = max_2;
                        max_2 = *walk;
                    }
                } else if (*walk != max_2) {
                    max_3 = *walk;
                }
            }
            if (*walk < min_3) {
                if (*walk < min_2) {
                    if (*walk < min) {
                        min_3 = min_2;
                        min_2 = min;
                        min = *walk;
                    } else if (*walk != min) {
                        min_3 = min_2;
                        min_2 = *walk;
                    }
                } else if (*walk != min_2) {
                    min_3 = *walk;
                }
            }
        }
        if (max_3 == 0) {
            if (max_2 != 0)
                max_3 = max_2;
            else
                max_3 = max;
        }
        if (min_3 == 0xffff) {
            if (min_2 != 0xffff)
                min_3 = min_2;
            else
                min_3 = min;
        }
    } else {
        max_3 = min_3 = 0;
        max = 0;
        min = 0xffff;
        for (i = 0; i < count; walk++, i++) {
            if (*walk > max)
                max = *walk;
            if (*walk < min)
                min = *walk;
        }
    }
    edge_substate_range_high = max;
    edge_substate_range_low.v = min;
    max = max - min;
    switch (img_side_params[side].x_res_dpi) {
    case 600: min_range = 37; break;
    case 400: min_range = 25; break;
    case 300: min_range = 19; break;
    default: min_range = 13; break;
    }
    if (max < min_range) {
        edge_substate_upper = 0xffff;
        edge_substate_lower = 0;
        edge_substate[side] = 0x8000;
    } else if (job_auto_size_flags.b2) {
        edge_w2 = edge->top_lines;
        limit = img_side_params[side].y_res_dpi * 5;
        if (edge_w2 < limit) {
            switch (img_side_params[side].x_res_dpi) {
            case 600: min_range = 48; break;
            case 400: min_range = 32; break;
            case 300: min_range = 24; break;
            default: min_range = 16; break;
            }
        } else {
            switch (img_side_params[side].x_res_dpi) {
            case 600: min_range = 60; break;
            case 400: min_range = 40; break;
            case 300: min_range = 30; break;
            default: min_range = 20; break;
            }
        }
        if (max_3 > min_3)
            max = max_3 - min_3;
        else
            max = 0;
        if (max < min_range || side == 1)
            edge_substate[side] = 0x400;
        else
            edge_substate[side] = 0xffff;
    } else {
        edge_substate[side] = 0x400;
    }
}

void edge_substate_400(int side, unsigned short *edge)
{
    unsigned short i;
    unsigned short count;
    unsigned short *walk;
    unsigned short histogram[4];
    unsigned short bin;
    unsigned short range;
    unsigned short low;
    unsigned short sum;
    unsigned short outer;
    unsigned short inner;

    count = edge_side_line_sums[side].sample_count;
    walk = edge_sample_values;
    util_mem_clear(histogram, 8);
    range = edge_substate_range_high - edge_substate_range_low.v + 1;
    low = edge_substate_range_low.v;
    for (i = 0; i < count; walk++, i++) {
        bin = (*walk - low) << 2;
        bin = rt_divu16(bin, range);
        histogram[bin]++;
    }
    outer = histogram[0] + histogram[3];
    inner = histogram[1] + histogram[2];
    if (job_auto_size_flags.b2) {
        if (side == 0) {
            sum = count >> 2;
            if (inner > sum) {
                if (*edge == 0x40) {
                    edge_substate_found = 1;
                } else if (edge_substate_found != 0) {
                    edge_substate[side] = 0xffff;
                    return;
                } else {
                }
            } else {
            }
        }
    }
    if (inner > outer) {
        edge_substate_upper = range * 3 / 4 + low;
        edge_substate_lower = (range >> 2) + low;
    } else {
        bin = edge_side_line_sums[side].sample_count * 7 / 10;
        sum = 0;
        for (i = 3; i > 0; i--) {
            sum += histogram[i];
            if (sum > bin)
                break;
        }
        edge_substate_upper = 0xffff;
        edge_substate_lower = i * range / 4 + low;
    }
    edge_substate[side] = 0x1000;
}

void edge_substate_1000(int side, int mode, int rising, struct edge_record *edge)
{
    int i;
    int taken;
    int count;
    int sum_x;
    int sum_y;
    unsigned short upper;
    unsigned short lower;
    unsigned short first_line;
    unsigned short u12;
    unsigned short *edges;
    unsigned short *values;
    unsigned short *samples;

    if (rising != 0)
        edges = edge->right_edges;
    else
        edges = edge->left_edges;
    upper = edge_substate_upper;
    lower = edge_substate_lower;
    sum_x = edge_side_line_sums[side].sum_x;
    sum_y = edge_side_line_sums[side].sum_y / edge_line_step;
    taken = edge_side_line_sums[side].count;
    values = edge_sample_values;
    samples = edge_samples;
    count = edge_side_line_sums[side].sample_count;
    first_line = edge->top_line / edge_line_step;
    for (i = 0; i < count; values++, samples++, i++) {
        if (*values > upper || *values < lower) {
            sum_x -= edges[*samples];
            sum_y -= *samples + first_line;
            taken--;
        }
    }
    edge_side_line_sums[side].sum_x = sum_x;
    edge_side_line_sums[side].sum_y = edge_line_step * sum_y;
    edge_side_line_sums[side].count = taken;
    edge_substate[side] = 0x4000;
}

void edge_substate_10_step_2(unsigned short *dest, int count, unsigned short *samples, int side, short rising, int mode, unsigned short *edges, struct edge_record *edge)
{
    unsigned short line;
    unsigned short first_line;
    int i;
    int run;
    int rise;
    int intercept;
    long edge_pos;
    long fit_pos;

    run = edge_top_line[side].run;
    first_line = edge->top_line;
    rise = edge_top_line[side].rise;
    if (rising)
        intercept = edge_top_line[side].right_intercept;
    else
        intercept = edge_top_line[side].left_intercept;
    for (i = 0; i < count; i++) {
        line = (samples[i] + first_line) * edge_line_step;
        edge_pos = edges[samples[i]];
        fit_pos = (line - intercept) * rise / run;
        if (rising)
            dest[i] = edge_pos - fit_pos + 0x8000;
        else
            dest[i] = fit_pos - edge_pos + 0x8000;
    }
}

void edge_substate_10_step_3(unsigned short *dest, int count, unsigned short *samples, int side, short rising, unsigned short *edges)
{
    unsigned short sample;
    int i;
    long edge_pos;
    int intercept;

    if (rising)
        intercept = edge_top_line[side].right_intercept;
    else
        intercept = edge_top_line[side].left_intercept;
    for (i = 0; i < count; i++) {
        sample = samples[i];
        edge_pos = edges[samples[i]];
        if (rising)
            dest[i] = edge_pos - intercept + 0x8000;
        else
            dest[i] = intercept - edge_pos + 0x8000;
        sample = 0;
    }
}

unsigned short edge_state_4c_step_1(int side, struct edge_record *edge)
{
    unsigned short index;
    unsigned short count;
    unsigned short limit;
    unsigned short step_px;
    unsigned short start;
    unsigned short *walk;
    int rise;
    int run;
    int end;

    if (edge_top_line[side].run == 0 || edge_top_line[side].rise == 0) {
        run = 0x10000;
        rise = 0;
    } else {
        run = edge_top_line[side].run;
        rise = abs(edge_top_line[side].rise);
    }
    rise += 0x23c;
    if (edge->width_px != 0)
        end = edge->width_px;
    else if (edge->last_width_px != 0)
        end = edge->last_width_px;
    else
        end = edge->start_width_px;
    count = (end - edge->start_width_px) * rise / run;
    count /= edge_line_step;
    step_px = run / rise;
    if (count > 4) {
        start = count >> 2;
        if (edge_top_corners[side].left_lower) {
            walk = edge->left_edges + start;
            limit = 0xffff;
            for (index = start; index < count; walk++, index++) {
                if (*walk != 0) {
                    if (*walk < limit) {
                        if (*walk > step_px)
                            limit = *walk - step_px;
                        else
                            break;
                    } else
                        break;
                }
            }
        } else {
            walk = edge->right_edges + start;
            limit = 0;
            for (index = start; index < count; walk++, index++) {
                if (*walk != 0xffff) {
                    if (*walk > limit)
                        limit = *walk + step_px;
                    else
                        break;
                }
            }
        }
    } else {
        index = 1;
    }
    if (index > 2)
        index -= 2;
    else
        index = 1;
    return index;
}

unsigned short edge_state_50_step_2(int side, struct edge_record *edge)
{
    unsigned short line;
    unsigned short first_line;
    unsigned short max;
    int i;
    int run;
    int rise;
    int intercept;
    int edge_x;
    int top_y;
    int count;
    unsigned short *values;
    unsigned short *edges;
    unsigned short *samples;

    values = edge_sample_values;
    count = edge_side_line_sums[side].sample_count;
    samples = edge_samples;
    if (edge_top_corners[side].left_lower == 0)
        edges = edge->right_edges;
    else
        edges = edge->left_edges;
    if (edge_top_line[side].run != 0 && edge_top_line[side].rise != 0) {
        run = edge_top_line[side].run;
        rise = edge_top_line[side].rise;
        intercept = edge_top_line[side].top_intercept;
        first_line = edge->top_line;
        for (i = 0; i < count; i++) {
            line = samples[i] * edge_line_step;
            line += first_line;
            edge_x = edges[samples[i]];
            top_y = intercept - rise * edge_x / run;
            values[i] = top_y - line + 0x8000u;
        }
    }
    count = edge_side_line_sums[side].sample_count;
    values = edge_sample_values;
    max = 0;
    for (i = 0; i < count; i++) {
        if (*values > max)
            max = *values;
        values++;
    }
    if (max > 0x8000) {
        max -= 0x8000;
        max++;
        max >>= 1;
        switch (img_side_params[side].x_res_dpi) {
        case 600: first_line = 24; break;
        case 400: first_line = 16; break;
        case 300: first_line = 12; break;
        default: first_line = 8; break;
        }
        if (max > first_line)
            max = first_line;
    } else
        max = 0;
    return max;
}

int edge_intersect_top_line(int side, int saved_imask, int flip)
{
    double slope_sum;
    double value;
    int rounded;
    int result;
    int use_left;

    slope_sum = (double)edge_side_line_sums[side].sum_y + (double)edge_side_line_sums[side].sum_x * (double)edge_top_line[side].rise / (double)edge_top_line[side].run;
    slope_sum /= (double)edge_side_line_sums[side].count;
    edge_top_line[side].top_intercept = edge_round(slope_sum);
    slope_sum = (double)edge_top_line[side].run / (double)edge_top_line[side].rise;
    slope_sum += (double)edge_top_line[side].rise / (double)edge_top_line[side].run;
    if (flip) {
        if (edge_top_corners[side].left_lower == 0)
            use_left = 1;
        else
            use_left = 0;
    } else {
        if (edge_top_corners[side].left_lower != 0)
            use_left = 1;
        else
            use_left = 0;
    }
    if (use_left) {
        value = (double)(edge_top_line[side].top_intercept - edge_top_line[side].left_intercept) / slope_sum;
        value = value * (double)edge_top_line[side].run / (double)edge_top_line[side].rise;
        rounded = edge_round(value);
        result = rounded + edge_top_line[side].left_intercept;
    } else {
        value = (double)(edge_top_line[side].top_intercept - edge_top_line[side].right_intercept) / slope_sum;
        value = value * (double)edge_top_line[side].run / (double)edge_top_line[side].rise;
        rounded = edge_round(value);
        result = rounded + edge_top_line[side].right_intercept;
    }
    return result;
}

void edge_state_8000(int side, int saved_imask, struct edge_record *edge)
{
    int x;
    edge_state_8000_step_1(side, saved_imask, edge);
}


extern void _builtin_set_imask(int);
extern void _builtin_set_cr(int);
extern int _builtin_get_cr(void);
#define RESTORE(x) _builtin_set_imask(x)

void edge_state_8000_step_1(int side, int saved_imask, struct edge_record *edge)
{
    unsigned short pad_lines;
    unsigned short source_lines;
    unsigned short length_lines;
    unsigned short y_end;
    struct page_record *scanning_page;
    struct win_side *window;
    unsigned short unpadded_lines;

    scanning_page = &page_records[page_scanning.v];
    window = &win_params.side[side];
    if (job_auto_size_flags.b6) {
        if (edge->event_20_lines == 0xffff)
            return;
    } else {
        if (edge->top_lines == 0xffff)
            return;
    }
    if (job_auto_size_flags.b7 && (edge->flags & EDGE_FLAG_FALLBACK))
        source_lines = edge->deskew_end_lines;
    else if (job_auto_size_flags.b6)
        source_lines = edge->event_20_lines + line_page_state[side].top_offset_lines;
    else
        source_lines = edge->top_lines + line_page_state[side].top_offset_lines;
    length_lines = rt_divu16(source_lines * img_side_params[side].y_zoom_permille + 0x22b, 1000);
    _builtin_set_imask(15);
    p_pisces_pass_dest[side]->ctrl.enable = 1;
    if (settings_byte_0x34[0] & 1)
        p_pisces_pass_dest[side]->y_end = 0xffff;
    else {
        y_end = length_lines - edge_page_geometry[side].bottom_margin_lines;
        if (y_end < p_pisces_pass_dest[side]->y_end)
            p_pisces_pass_dest[side]->y_end = y_end;
    }
    RESTORE(saved_imask);
    if (win_params.paper_size.f.auto_length) {
    if (source_lines < img_crop_area[side].length_lines) {
    if (length_lines < img_side_params[side].output_length_lines) {
        unpadded_lines = length_lines;
        if (window->compression.f.type & 0x80)
            pad_lines = 8 - (length_lines & 7);
        else
            pad_lines = 0;
        length_lines += pad_lines;
        p_pisces_pass_dest[side]->y_end += pad_lines;
        if (length_lines < edge_page_geometry[side].length_lines) {
            _builtin_set_imask(15);
            edge_page_geometry[side].length_lines = length_lines;
            edge_page_geometry[side].length_unpadded_lines = unpadded_lines;
            scanning_page->sides[side].length_unpadded_lines = edge_page_geometry[side].length_unpadded_lines;
            p_pisces_pass_dest[side]->length_lines.f.v = edge_page_geometry[side].length_lines;
            scanning_page->sides[side].length_lines = edge_page_geometry[side].length_lines;
            RESTORE(saved_imask);
        }
    }
    }
    }
    edge_update_output_lines(side, saved_imask);
    _builtin_set_imask(15);
    p_pisces_pass_dest[side]->ctrl.enable = 0;
    scanning_page->sides[side].size_final = 1;
    edge->state = EDGE_STATE_DONE;
    edge->flags |= EDGE_FLAG_SIZE_FINAL;
    RESTORE(saved_imask);
}


void edge_state_80(int side, int saved_imask, struct edge_record *edge)
{
    unsigned short stride;
    unsigned short count;
    unsigned short window;
    unsigned short start;
    unsigned short bottom_line;
    unsigned short margin;

    if (edge_top_corners[side].left_lower)
        edge_bottom_point[side].corner_line = edge_top_corners[side].left_line;
    else
        edge_bottom_point[side].corner_line = edge_top_corners[side].right_line;
    edge_bottom_point[side].bottom_line = edge->bottom_line;
    if (edge_top_line[side].run == 0 || edge_top_line[side].rise == 0) {
        edge_bottom_point[side].bottom_line = edge_bottom_point[side].bottom_line - (edge_line_step >> 1);
        edge->state = EDGE_STATE_PAPER_LENGTH;
        return;
    } else {
        bottom_line = edge_bottom_point[side].bottom_line / edge_line_step;
        if (edge_top_corners[side].left_lower)
            start = edge_top_corners[side].left_line - edge_top_corners[side].right_line;
        else
            start = edge_top_corners[side].right_line - edge_top_corners[side].left_line;
        start = bottom_line - start / edge_line_step;
        stride = bottom_line - start;
        margin = stride;
        if (stride < 4) {
            edge->state = EDGE_STATE_PAPER_LENGTH;
        } else {
            if (stride < 17) {
                count = stride;
                stride = 1;
            } else {
                count = 16;
                stride /= count + 1;
            }
            margin = (margin - count * stride) / 2;
            start -= margin;
            if (stride > 16)
                window = 16;
            else
                window = stride;
            util_mem_clear(edge_samples, 0x100);
            edge_side_line_sums[side].sum_x = edge_side_line_sums[side].count = edge_side_line_sums[side].sum_y = 0;
            edge_copy_samples(side, start, stride, count, window, edge_top_corners[side].left_lower == 0, edge);
            edge_side_line_sums[side].sample_count = edge_side_line_sums[side].count;
            if (edge_side_line_sums[side].count > 2)
                edge->state = EDGE_STATE_BOTTOM_INTERSECT;
            else
                edge->state = EDGE_STATE_PAPER_LENGTH;
        }
    }
}

void edge_state_84(int side, int saved_imask, struct edge_record *edge)
{
    edge_bottom_point[side].bottom_line = edge_intersect_top_line(side, saved_imask, 0);
    if (edge_bottom_point[side].bottom_line > edge->bottom_line)
        edge_bottom_point[side].bottom_line = edge->bottom_line;
    edge->state = EDGE_STATE_PAPER_LENGTH;
}

void edge_state_a0(int side, int saved_imask, struct edge_record *edge)
{
    unsigned short source_lines;
    unsigned short length_lines;
    struct win_side *window;

    window = &win_params.side[side];
    edge_page_geometry[side].source_lines = edge_bottom_point[side].bottom_line - edge_bottom_point[side].corner_line;
    if (job_auto_size_flags.b4)
        edge_page_geometry[side].source_lines += edge_source_lines_extra;
    source_lines = edge_scale_lines(side, edge_page_geometry[side].source_lines);
    length_lines = rt_divu16(img_side_params[side].y_zoom_permille * source_lines + 0x22b, 1000);
    edge_page_geometry[side].paper_length_lines = length_lines;
    edge_page_geometry[side].length_unpadded_lines = edge_page_geometry[side].paper_length_lines;
    length_lines = edge_align_lines(length_lines, window->compression.f.type & 0x80);
    if (job_auto_size_flags.b5) {
        edge_page_geometry[side].source_lines = img_crop_area[side].length_lines;
        length_lines = img_side_params[side].output_length_lines;
    }
    if (length_lines < img_side_params[side].output_length_lines) {
        edge_page_geometry[side].length_source_lines = source_lines;
        edge_page_geometry[side].length_lines = length_lines;
    } else {
        edge_page_geometry[side].paper_length_lines = img_side_params[side].output_length_lines;
        edge_page_geometry[side].length_unpadded_lines = edge_page_geometry[side].paper_length_lines;
    }
    if (((settings_byte_0xca[0] & 0x80) || job_auto_size_flags.b2) && side == 0) {
        edge->state = EDGE_STATE_OPTIONAL_PASS_1;
        edge_b0_state = 1;
        edge_b0_runs.v = 0;
    } else
        edge->state = EDGE_STATE_APPLY;
    if (!win_params.flags.f.adf)
        mdc_request_stop();
}

int edge_scale_lines(int side, int lines)
{
    int scaled;

    if (edge_page_geometry[side].skew_angle != 0) {
        scaled = rt_divu32(edge_page_geometry[side].y_zoom_permille * 1000 + (img_side_params[side].y_zoom_permille >> 1), img_side_params[side].y_zoom_permille);
        scaled = rt_divu16(lines * scaled + 0x22b, 1000);
    } else
        scaled = lines;
    return scaled;
}


void edge_update_output_lines(int side, int saved_imask)
{
    if (page_scanned_count.v == page_sent_count.v && page_records[page_current.v].sides[side].flags.f.data_ready) {
        _builtin_set_imask(15);
        p_pisces_output[side]->ctrl.enable = 1;
        p_pisces_output[side]->lines = page_records[page_current.v].sides[side].length_lines;
        p_pisces_output[side]->reg_0d4 = page_records[page_current.v].sides[side].length_lines;
        p_pisces_output[side]->ctrl.enable = 0;
        page_records[page_current.v].sides[side].output_lines_set = 1;
        _builtin_set_imask(saved_imask);
    }
}

unsigned short edge_inch1200_to_px(int side, int inch1200)
{
    unsigned short px;
    int product;

    product = inch1200 * img_side_params[side].x_res_dpi;
    px = rt_divu16(product, 0x4b0);
    return px;
}

unsigned short edge_inch1200_to_lines(int side, int inch1200)
{
    unsigned short lines;
    int product;

    product = inch1200 * img_side_params[side].y_res_dpi;
    lines = rt_divu16(product, 0x4b0);
    return lines;
}

void edge_align_width(int side, unsigned short *width)
{
    if (img_mode[side].path_flags.f.multi_level) {
        if (settings_byte_0x35[0] & 0x20) {
            if (*width & 1)
                *width += 1;
        } else if (*width & 3) {
            *width += 4;
            *width &= 0xfffc;
        }
    } else if ((settings_emulation[0] & 28) == 28 || (settings_emulation[0] & 28) == 24) {
        if (!win_params.side[0].size_rounding.f.round_width_down)
            *width += 7;
        *width &= 0xfff8;
    } else {
        if ((settings_autocrop_boundary[0] & 0x80) || job_auto_size_flags.b4)
            *width += 31;
        *width &= 0xffe0;
    }
}

int edge_round(double value)
{
    int rounded;

    if (value > 0.0)
        rounded = (int)(value + 0.5);
    else
        rounded = (int)(value + -0.5);
    return rounded;
}

unsigned short edge_align_lines(unsigned short lines, int compressed)
{
    if (job_auto_size_flags.b7 && !job_auto_size_flags.b5 && !job_auto_size_flags.b4) {
        if ((settings_byte_0xca[0] & 3) == 0) {
            if (compressed && (lines & 7)) {
                lines += 8;
                lines &= 0xfff8;
            }
        } else if ((settings_byte_0xca[0] & 3) == 1) {
            lines &= 0xfff0;
        } else if ((settings_byte_0xca[0] & 3) == 2) {
            lines -= 16;
            lines &= 0xfff0;
        } else {
            lines -= 32;
            lines &= 0xffe0;
        }
    } else if (compressed && (lines & 7)) {
        lines += 8;
        lines &= 0xfff8;
    }
    return lines;
}
