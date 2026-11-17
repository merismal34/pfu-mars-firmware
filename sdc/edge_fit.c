#include "fw.h"
struct edge_record_tables;
  /* the 44-byte request block; same layout in edge_fit.c (T) and edge_bottom.c (C) */

void edge_state_3c(int side, int saved_imask, short *edge)
{
    short fit_error;
    int flag;

    flag = 0;
    switch (edge_fit_state[side]) {
    case 0:
        edge_fit_state_0(side, saved_imask);
        break;
    case 0x10:
        edge_fit_state_10(side, saved_imask, edge);
        break;
    case 0x100:
        flag = 1;
        fit_error = edge_fit_state_100(side, saved_imask);
        break;
    case 0x1000:
        edge_fit_state_1000(side, saved_imask, edge);
        break;
    }
    if (edge_fit_state[side] == 0x8000) {
        if (edge_top_line[side].rise > 0) {
            edge_top_corners[side].left_lower = 1;
        } else {
            edge_top_corners[side].left_lower = 0;
        }
        edge_sample_pos[side] = 0;
        *edge = EDGE_STATE_FIRST_EDGE_SAMPLES;
    } else if (edge_fit_state[side] == 0xffff) {
        *edge = EDGE_STATE_FALLBACK;
        settings_edge_fail_code[0] = 5;
    } else if (edge_fit_state[side] == 0x1000 && edge_sample_pos[side] == 0) {
        if (edge_fit_params[side].count < edge_fit_params[side].sample_limit / 2) {
            if (flag == 1 && (unsigned)fit_error > edge_fit_limit.v) {
                *edge = EDGE_STATE_FALLBACK;
                settings_edge_fail_code[0] = 6;
            } else {
                if (edge_top_line[side].rise > 0) {
                    edge_top_corners[side].left_lower = 1;
                } else {
                    edge_top_corners[side].left_lower = 0;
                }
                edge_sample_pos[side] = 0;
                *edge = EDGE_STATE_FIRST_EDGE_SAMPLES;
            }
        }
    }
}

void edge_state_3e(int side, int saved_imask, struct edge_record *edge)
{
    short first_line;
    unsigned short last_line;
    int failed;

    first_line = edge->top_line / edge_line_step;
    if (edge->bottom_corner_line == 0) {
        last_line = edge->event_20_lines + line_page_state[side].top_offset_lines;
    } else {
        last_line = edge->bottom_corner_line;
    }
    last_line /= edge_line_step;
    failed = edge_sample_side(side, first_line, last_line, edge);
    if (failed != 0) {
        edge->state = EDGE_STATE_FALLBACK;
        settings_edge_fail_code[0] = 7;
    }
    if (edge_sample_pos[side] == 0) {
        if (edge_side_line_sums[side].count > 2) {
            edge_substate_found = 0;
            edge_substate[side] = 0;
            edge->state = EDGE_STATE_FIRST_EDGE_FIT;
        } else {
            edge->state = EDGE_STATE_FALLBACK;
            settings_edge_fail_code[0] = 8;
        }
    }
}

int edge_sample_side(int side, short from, short to, struct edge_record *edge)
{
    unsigned short stride;
    unsigned short window;
    unsigned short count;
    unsigned short span;
    unsigned int rising;

    if (edge->state == EDGE_STATE_FIRST_EDGE_SAMPLES) {
        span = to - from;
    } else {
        span = from - to;
    }
    if (span < 0x48) {
        return 1;
    } else {
        if (span < 0x81) {
            count = span - 1;
            stride = 1;
        } else {
            stride = span / 0x81;
            stride++;
            count = (span - 1) / stride;
        }
        if (edge->state == EDGE_STATE_FIRST_EDGE_SAMPLES) {
            from += stride;
            rising = edge_top_corners[side].left_lower;
        } else {
            from -= stride;
            rising = edge_top_corners[side].left_lower == 0;
        }
        if (stride > 0x20) {
            window = 0x20;
        } else {
            window = stride;
        }
        if (edge_sample_pos[side] == 0) {
            edge_samples_taken[side] = 0;
        }
        edge_samples_taken[side] += 0x20;
        if (edge_samples_taken[side] > count) {
            edge_samples_taken[side] = count;
        }
        edge_copy_samples(side, from, stride, edge_samples_taken[side], window, rising, edge);
        if (edge_sample_pos[side] >= count) {
            edge_sample_pos[side] = 0;
        }
    }
    return 0;
}

void edge_state_40(int side, int unused, unsigned short *edge)
{
    edge_run_substate(side, 0, edge_top_corners[side].left_lower, edge);
    if (edge_substate[side] == 0x8000) {
        edge_sample_pos[side] = 0;
        *edge = EDGE_STATE_SECOND_EDGE_SAMPLES;
    } else if (edge_substate[side] == 0xffff) {
        *edge = EDGE_STATE_FALLBACK;
        settings_edge_fail_code[0] = 9;
    }
}

void edge_state_44(int side, int unused, struct edge_record *edge)
{
    unsigned short last_line;
    unsigned short first_line;
    int failed;

    first_line = edge->top_corner_line / edge_line_step;
    last_line = edge->bottom_line;
    last_line /= edge_line_step;
    failed = edge_sample_side(side, last_line, first_line, edge);
    if (failed != 0) {
        edge->state = EDGE_STATE_FALLBACK;
        settings_edge_fail_code[0] = 10;
        return;
    }
    if (edge_sample_pos[side] == 0) {
        if (edge_side_line_sums[side].count > 2) {
            edge_substate[side] = 0;
            edge->state = EDGE_STATE_SECOND_EDGE_FIT;
        } else {
            edge->state = EDGE_STATE_FALLBACK;
            settings_edge_fail_code[0] = 11;
        }
    }
}

void edge_state_48(int side, int unused, unsigned short *edge)
{
    edge_run_substate(side, 0, edge_top_corners[side].left_lower == 0, edge);
    if (edge_substate[side] == 0x8000) {
        edge_sample_pos[side] = 0;
        *edge = EDGE_STATE_TOP_SAMPLES;
    } else if (edge_substate[side] == 0xffff) {
        *edge = EDGE_STATE_FALLBACK;
        settings_edge_fail_code[0] = 12;
    }
}


void edge_state_4c(int side, int saved_imask, struct edge_record *edge)
{
    unsigned short stride;
    unsigned short count;
    unsigned short window;
    short start;
    short first_line;
    unsigned short margin;
    int x;
    int y;
    double line;

    first_line = edge->top_line / edge_line_step;
    if (edge_top_line[side].run == 0 || edge_top_line[side].rise == 0) {
        start = edge_state_4c_step_1(side, edge);
        edge_top_corners[side].left_lower = 1;
        first_line = edge_state_4c_step_1(side, edge);
        if ((unsigned)start > (unsigned)first_line)
            stride = start;
        else
            stride = first_line;
        stride = (stride + 1) / 2;
        if (stride > 2)
            stride = 2;
        edge_top_line[side].top_intercept = edge->top_line + stride * edge_line_step + (edge_line_step >> 1);
        edge->state = EDGE_STATE_TOP_CORNERS;
    } else {
        stride = edge_state_4c_step_1(side, edge);
        start = first_line + stride;
        margin = stride;
        if (stride < 4) {
            count = stride;
            stride = 1;
            window = 16;
            start -= 1;
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
        }
        util_mem_clear(edge_samples, 0x100);
        edge_side_line_sums[side].sum_x = edge_side_line_sums[side].count = edge_side_line_sums[side].sum_y = 0;
        edge_copy_samples(side, start, stride, count, window, edge_top_corners[side].left_lower, edge);
        edge_side_line_sums[side].sample_count = edge_side_line_sums[side].count;
        if (edge_side_line_sums[side].count > 0) {
            edge->state = EDGE_STATE_TOP_INTERSECT;
        } else {
            if (edge_top_corners[side].left_lower != 0) {
                x = edge->top_left_px;
            } else {
                x = edge->top_right_px;
            }
            y = edge->top_line;
            line = (double)y + (double)x * (double)edge_top_line[side].rise / (double)edge_top_line[side].run;
            edge_top_line[side].top_intercept = edge_round(line);
            edge->state = EDGE_STATE_TOP_CORNERS;
        }
    }
}

void edge_state_50(int side, int saved_imask, struct edge_record *edge)
{
    int intersect;
    unsigned short correction;

    intersect = edge_intersect_top_line(side, saved_imask, 1);
    if (edge_side_line_sums[side].sample_count > 4)
        correction = edge_state_50_step_2(side, edge);
    else
        correction = 0;
    edge_top_line[side].top_intercept += correction;
    intersect += correction;
    edge->state = EDGE_STATE_TOP_CORNERS;
}

void edge_copy_samples(int side, unsigned short start, short stride, short count, short window, unsigned int rising, struct edge_record *edge)
{
    unsigned short point;
    unsigned short last_x;
    unsigned short first_line;
    short pos;
    short step;
    unsigned short *edges;
    unsigned short *dest;
    unsigned int use_right_edge;
    int sum_x;
    int taken;
    int sum_y;

    if (edge->state == EDGE_STATE_FIRST_EDGE_SAMPLES || edge->state == EDGE_STATE_BOTTOM_SAMPLES) {
        if (edge_top_corners[side].left_lower != 0)
            edges = edge->right_edges;
        else
            edges = edge->left_edges;
        step = stride;
        use_right_edge = edge_top_corners[side].left_lower;
    } else {
        if (edge_top_corners[side].left_lower == 0)
            edges = edge->right_edges;
        else
            edges = edge->left_edges;
        step = -stride;
        use_right_edge = edge_top_corners[side].left_lower == 0;
    }
    if (rising != 0)
        last_x = 6;
    else
        last_x = 0xfff9;
    if (edge_sample_pos[side] == 0) {
        util_mem_clear(edge_samples, 0x100);
        edge_side_line_sums[side].sum_x = edge_side_line_sums[side].count = edge_side_line_sums[side].sum_y = 0;
    }
    first_line = edge->top_line / edge_line_step;
    sum_x = edge_side_line_sums[side].sum_x;
    sum_y = edge_side_line_sums[side].sum_y / edge_line_step;
    taken = edge_side_line_sums[side].count;
    dest = edge_samples + taken;
    for (pos = edge_sample_pos[side]; (unsigned)pos < (unsigned)count; pos++) {
        point = step * pos + start;
        point = edge_find_point(point - first_line, window, edges, last_x, rising, use_right_edge);
        if (point != 0xffff) {
            sum_x += edges[point];
            sum_y += point + first_line;
            taken++;
            *dest = point;
            dest++;
            last_x = edges[point];
        }
    }
    edge_sample_pos[side] = pos;
    edge_side_line_sums[side].sum_x = sum_x;
    edge_side_line_sums[side].sum_y = edge_line_step * sum_y;
    edge_side_line_sums[side].count = taken;
    edge_side_line_sums[side].sample_count = taken;
}

double sqrt(double);


void edge_state_60(int side, int saved_imask, short *edge)
{
    double slope_sum;
    double value;
    int rounded;
    int u48[5];
    unsigned short margin_px;
    unsigned short angle_index;
    unsigned short shift_a;
    unsigned short shift_b;
    int u24[4];
    int product;
    int u16;
    int dx;
    int dy;
    int cos_value;
    int sin_value;

    if (edge_top_line[side].run != 0 && edge_top_line[side].rise != 0) {
        slope_sum = (double)edge_top_line[side].run / (double)edge_top_line[side].rise;
        slope_sum = slope_sum + (double)edge_top_line[side].rise / (double)edge_top_line[side].run;
        value = (double)(edge_top_line[side].top_intercept - edge_top_line[side].left_intercept) / slope_sum;
        rounded = edge_round(value);
        edge_top_corners[side].left_x_px = rounded;
        value = value * edge_top_line[side].run / edge_top_line[side].rise;
        rounded = edge_round(value);
        edge_top_corners[side].left_line = rounded + edge_top_line[side].left_intercept;
        value = (double)(edge_top_line[side].top_intercept - edge_top_line[side].right_intercept) / slope_sum;
        rounded = edge_round(value);
        edge_top_corners[side].right_x_px = rounded;
        value = value * edge_top_line[side].run / edge_top_line[side].rise;
        rounded = edge_round(value);
        edge_top_corners[side].right_line = rounded + edge_top_line[side].right_intercept;
    }
    else {
        edge_top_corners[side].left_x_px = edge_top_line[side].left_intercept;
        edge_top_corners[side].left_line = edge_top_line[side].top_intercept;
        edge_top_corners[side].right_x_px = edge_top_line[side].right_intercept;
        edge_top_corners[side].right_line = edge_top_line[side].top_intercept;
    }
    if (edge_top_corners[side].left_line == edge_top_corners[side].right_line) {
        edge_page_geometry[side].skew_angle = 0;
    }
    else {
        product = abs(edge_top_line[side].rise);
        product <<= 9;
        product += edge_top_line[side].run / 2;
        product = rt_divu32(product, edge_top_line[side].run);
        edge_page_geometry[side].skew_angle = product;
    }
    if (job_auto_size_flags.b5) {
        if (side == 0) {
            edge_top_corner_x = edge_top_corners[side].right_x_px;
        }
        edge_top_corners[side].right_x_px = img_crop_area[side].width_px + (unsigned short)edge_top_corners[side].left_x_px;
        if (img_crop_area[side].x_offset_px != 0 || img_crop_area[side].y_offset_lines != 0) {
            angle_index = (edge_page_geometry[side].skew_angle + 3) / 5;
            cos_value = edge_cos_table[angle_index];
            sin_value = edge_page_geometry[side].skew_angle * cos_value / 512;
            if (edge_top_corners[side].left_lower != 0) {
                rounded = img_crop_area[side].y_offset_lines * sin_value + img_crop_area[side].x_offset_px * cos_value;
                dx = (rounded + 555) / 1000;
                rounded = img_crop_area[side].y_offset_lines * cos_value - img_crop_area[side].x_offset_px * sin_value;
                dy = (rounded + 555) / 1000;
            }
            else {
                rounded = img_crop_area[side].x_offset_px * cos_value - img_crop_area[side].y_offset_lines * sin_value;
                dx = (rounded + 555) / 1000;
                rounded = img_crop_area[side].x_offset_px * sin_value + img_crop_area[side].y_offset_lines * cos_value;
                dy = (rounded + 555) / 1000;
            }
            edge_top_corners[side].left_x_px += dx;
            edge_top_corners[side].right_x_px += dx;
            edge_top_corner_x -= dx;
            edge_top_corners[side].left_line += dy;
            edge_top_corners[side].right_line += dy;
        }
    }
    else if (job_auto_size_flags.b4) {
        margin_px = settings_byte_0x3a[0];
        if (margin_px == 0) {
            margin_px = 50;
        }
        margin_px += 5;
        margin_px = rt_divu16(margin_px * img_side_params[side].x_res_dpi, 0xfe);
        angle_index = edge_page_geometry[side].skew_angle / 10;
        product = edge_corner_shift_table[angle_index].cos_minus_sin * margin_px;
        shift_a = rt_divu32(product + 555, 1000);
        product = edge_corner_shift_table[angle_index].cos_plus_sin * margin_px;
        shift_b = rt_divu32(product + 555, 1000);
        if (edge_top_corners[side].left_lower != 0) {
            edge_top_corners[side].left_x_px -= shift_b;
        }
        else {
            edge_top_corners[side].left_x_px -= shift_a;
        }
        if (job_auto_size_flags.b3) {
            edge_top_corners[side].right_x_px = img_crop_area[side].width_px + (unsigned short)edge_top_corners[side].left_x_px;
        }
        else if (edge_top_corners[side].left_lower != 0) {
            edge_top_corners[side].right_x_px += shift_a;
        }
        else {
            edge_top_corners[side].right_x_px += shift_b;
        }
        if (edge_top_corners[side].left_lower != 0) {
            edge_top_corners[side].left_line -= shift_a;
            edge_top_corners[side].right_line -= shift_b;
        }
        else {
            edge_top_corners[side].left_line -= shift_b;
            edge_top_corners[side].right_line -= shift_a;
        }
        edge_source_lines_extra = shift_b;
    }
    *edge = EDGE_STATE_EDGE_LENGTH;
}

void edge_state_64(int side, int saved_imask, struct edge_record *edge)
{
    edge_page_geometry[side].dx_px = edge_top_corners[side].right_x_px - edge_top_corners[side].left_x_px;
    edge_page_geometry[side].source_lines = img_side_params[side].scan_length_lines;
    edge_page_geometry[side].length_source_lines = img_crop_area[side].length_lines;
    edge_page_geometry[side].length_lines = img_side_params[side].output_length_lines;
    edge_compute_length(side, saved_imask, edge);
    edge_state_64_step_2(side, saved_imask, edge);
}

void edge_compute_length(int side, int saved_imask, struct edge_record *edge)
{
    int dx, dy;
    double length;

    dx = edge_page_geometry[side].dx_px;
    if (edge_page_geometry[side].skew_angle == 0) {
        edge_page_geometry[side].edge_length_px = dx;
    } else {
        if (edge_top_corners[side].left_lower != 0)
            dy = edge_top_corners[side].left_line - edge_top_corners[side].right_line;
        else
            dy = edge_top_corners[side].right_line - edge_top_corners[side].left_line;
        length = (double)(unsigned int)(dx * dx + dy * dy);
        length = sqrt(length);
        edge_page_geometry[side].edge_length_px = (int)(length + 0.5);
    }
}

void edge_state_64_step_2(int side, int saved_imask, struct edge_record *edge)
{
    unsigned short length_mm;
    unsigned short margin_mm;
    unsigned int zoom_permille;
    int lo;
    int hi;
    int tolerance;
    int expected_x;
    int corner_y;
    unsigned char output_buf;

    if (edge_page_geometry[side].skew_angle > 511) {
        settings_edge_fail_code[0] = 16;
        goto err;
    }
    lo = 0;
    hi = edge->bottom_line;
    if (edge_top_corners[side].left_line > hi || edge_top_corners[side].left_line < lo || edge_top_corners[side].right_line > hi || edge_top_corners[side].right_line < lo) {
        settings_edge_fail_code[0] = 20;
        goto err;
    }
    if (!job_auto_size_flags.b5) {
        if (side == 1) {
            tolerance = edge_inch1200_to_px(side, 0x1d8);
            if (img_overscan_shift_on[side]) {
                output_buf = page_side_state[side].output_buf;
                corner_y = edge_top_corners[0].right_line - img_overscan_shift_lines[0][output_buf];
                hi = corner_y + tolerance;
                lo = corner_y - tolerance;
                corner_y = edge_top_corners[side].left_line - img_overscan_shift_lines[side][output_buf];
            } else {
                hi = edge_top_corners[0].right_line + tolerance;
                lo = edge_top_corners[0].right_line - tolerance;
                corner_y = edge_top_corners[side].left_line;
            }
            if (corner_y > hi || corner_y < lo) {
                settings_edge_fail_code[0] = 32;
                goto err;
            }
            expected_x = img_side_params->capture_width_px - edge_top_corners[0].right_x_px;
            hi = expected_x + tolerance;
            lo = expected_x - tolerance;
            if (edge_top_corners[side].left_x_px > hi || edge_top_corners[side].left_x_px < lo) {
                settings_edge_fail_code[0] = 33;
                goto err;
            }
        }
        length_mm = rt_divu16(edge_page_geometry[side].edge_length_px * 254, img_side_params[side].x_res_dpi * 10);
        if (job_auto_size_flags.b4) {
            margin_mm = settings_byte_0x3a[0];
            if (margin_mm == 0) {
                margin_mm = 50;
            }
            margin_mm = margin_mm / 5;
            length_mm = length_mm - margin_mm;
            if (job_auto_size_flags.b3) {
                length_mm = 0xd2;
            }
        }
        if (length_mm > 0x134 || length_mm < 50) {
            settings_edge_fail_code[0] = 17;
            goto err;
        }
    }
    if (job_auto_size_flags.b5 && (img_crop_area[side].x_offset_px != 0 || img_crop_area[side].y_offset_lines != 0)) {
        length_mm = (edge_page_geometry[side].skew_angle + 3) / 5;
        zoom_permille = 1000000u / edge_cos_table[length_mm];
    } else {
        zoom_permille = edge_page_geometry[side].edge_length_px * 1000 + ((unsigned int)edge_page_geometry[side].dx_px >> 1);
        zoom_permille = rt_divu16(zoom_permille, edge_page_geometry[side].dx_px);
    }
    if (zoom_permille > 0x586) {
        settings_edge_fail_code[0] = 21;
        goto err;
    }
    edge_page_geometry[side].x_zoom_permille = rt_divu16(img_side_params[side].x_zoom_permille * zoom_permille + 0x22b, 1000);
    if (job_auto_size_flags.b5 || job_auto_size_flags.b3) {
        edge_page_geometry[side].width_px = img_side_params[side].width_px;
    } else {
        edge_page_geometry[side].width_px = rt_divu16(edge_page_geometry[side].dx_px * edge_page_geometry[side].x_zoom_permille + 0x22b, 1000);
        edge_page_geometry[side].width_unaligned_px = edge_page_geometry[side].width_px;
        edge_align_width(side, &edge_page_geometry[side].width_px);
        edge_page_geometry[side].paper_width_px = edge_page_geometry[side].width_px;
        img_align_width(side, &edge_page_geometry[side].width_px, 1);
        if (edge_page_geometry[side].width_px > img_side_params[side].width_px) {
            edge_page_geometry[side].width_px = img_side_params[side].width_px;
            edge_page_geometry[side].paper_width_px = img_side_params[side].width_px;
        }
        if (edge_page_geometry[side].width_unaligned_px > edge_page_geometry[side].width_px) {
            edge_page_geometry[side].width_unaligned_px = edge_page_geometry[side].width_px;
        }
    }
    edge_page_geometry[side].y_zoom_permille = rt_divu16(img_side_params[side].y_zoom_permille * zoom_permille + 0x22b, 1000);
    edge->flags &= ~EDGE_FLAG_FALLBACK;
    edge->state = EDGE_STATE_BOTTOM_SAMPLES;
    return;
err:
    edge->state = EDGE_STATE_FALLBACK;
    return;
}
