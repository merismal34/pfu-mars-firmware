/* edge_run_page_end_state dispatches on edge->state; edge_state_30/edge_state_32/edge_state_34 are its 0x30/0x32/0x34 handlers. */
#include "fw.h"


double sqrt(double);

void edge_run_page_end_state(int side, int saved_imask, struct edge_record *edge)
{
    switch (edge->state) {
    case EDGE_STATE_SKEW_DIRECTION: edge_state_30(side, saved_imask, edge); break;
    case EDGE_STATE_CORNERS: edge_state_32(side, saved_imask, edge); break;
    case EDGE_STATE_SAMPLES: edge_state_34(side, saved_imask, edge); break;
    case EDGE_STATE_FIT_SUMS: edge_state_38(side, saved_imask, edge); break;
    case EDGE_STATE_FIT: edge_state_3c(side, saved_imask, edge); break;
    case EDGE_STATE_FIRST_EDGE_SAMPLES: edge_state_3e(side, saved_imask, edge); break;
    case EDGE_STATE_FIRST_EDGE_FIT: edge_state_40(side, saved_imask, edge); break;
    case EDGE_STATE_SECOND_EDGE_SAMPLES: edge_state_44(side, saved_imask, edge); break;
    case EDGE_STATE_SECOND_EDGE_FIT: edge_state_48(side, saved_imask, edge); break;
    case EDGE_STATE_TOP_SAMPLES: edge_state_4c(side, saved_imask, edge); break;
    case EDGE_STATE_TOP_INTERSECT: edge_state_50(side, saved_imask, edge); break;
    case EDGE_STATE_TOP_CORNERS: edge_state_60(side, saved_imask, edge); break;
    case EDGE_STATE_EDGE_LENGTH: edge_state_64(side, saved_imask, edge); break;
    case EDGE_STATE_BOTTOM_SAMPLES: edge_state_80(side, saved_imask, edge); break;
    case EDGE_STATE_BOTTOM_INTERSECT: edge_state_84(side, saved_imask, edge); break;
    case EDGE_STATE_PAPER_LENGTH: edge_state_a0(side, saved_imask, edge); break;
    case EDGE_STATE_OPTIONAL_PASS_1: edge_state_b0(side, saved_imask, edge); break;
    case EDGE_STATE_OPTIONAL_PASS_2: edge_state_c0(side, saved_imask, edge); break;
    }
}

void edge_state_30(int side, int saved_imask, struct edge_record *edge)
{
    unsigned short first_span, second_span;
    short end_line;
    unsigned short min_span;
    int left_slope, right_slope, failed, use_corners;
    unsigned char output_buf;
    unsigned short limit_side;

    failed = 0;
    use_corners = 0;
    util_mem_clear(&edge_fit_params[side], 44);
    util_mem_clear(&edge_side_line_sums[side], 16);
    util_mem_clear(&edge_top_line[side], 20);
    util_mem_clear(&edge_top_corners[side], 20);
    util_mem_clear(&edge_page_geometry[side], 32);
    util_mem_clear(&edge_bottom_point[side], 4);
    if (side == 0) {
        first_span = edge->top_corner_line - edge->top_line;
        end_line = edge->bottom_corner_line;
        second_span = end_line - edge->top_corner_line;
        min_span = edge_line_step * 76;
        if (second_span < first_span || (first_span > min_span && second_span < (first_span << 1))) {
            failed = edge_measure_slope(&left_slope, &right_slope, edge->top_line, edge->top_corner_line, side, edge);
            if (right_slope < 0) {
                right_slope = abs(right_slope);
                if (left_slope < right_slope)
                    edge_top_corners[side].left_lower = 0;
                else
                    edge_top_corners[side].left_lower = 1;
            } else {
                failed = 1;
            }
            use_corners = 1;
        } else {
            failed = edge_vote_skew_direction(&left_slope, &right_slope, edge->top_corner_line, end_line, side, edge, &use_corners);
            if (left_slope > right_slope)
                edge_top_corners[side].left_lower = 0;
            else
                edge_top_corners[side].left_lower = 1;
        }
    } else {
        output_buf = page_side_state[1].output_buf;
        if (((struct edge_record *)edge_side_records)[output_buf].flags & EDGE_FLAG_FALLBACK) {
            failed = 1;
        } else {
            if (edge_top_corners[0].left_lower)
                edge_top_corners[1].left_lower = 0;
            else
                edge_top_corners[1].left_lower = 1;
            edge_top_line[1].run = edge_top_line[0].run;
            edge_top_line[1].rise = -edge_top_line[0].rise;
            if (edge_page_geometry[0].skew_angle > 6)
                use_corners = 1;
            else
                use_corners = 0;
        }
    }
    if ((settings_byte_0x35[0] & 0x10) || (settings_non_rectangular[0] & 1)) {
        edge->state = EDGE_STATE_FALLBACK;
        return;
    }
    if (failed) {
        edge->state = EDGE_STATE_FALLBACK;
        if (side == 0)
            settings_edge_fail_code[0] = 2;
    } else {
        if (win_params.flags.f.adf == 0) {
            if (edge_search_hits > edge_flatbed_limit_lines.v)
                limit_side = 0x100;
            else if (edge_right_limit_hits > edge_flatbed_limit_lines.v)
                limit_side = 1;
            else
                limit_side = 0;
            if (edge_top_corners[side].left_lower != 0 && limit_side == 0x100)
                limit_side = 0;
            else if (edge_top_corners[side].left_lower == 0 && limit_side == 1)
                limit_side = 0;
            if (limit_side != 0)
                edge_fit_params[side].right_side = 1;
            else
                edge_fit_params[side].right_side = 0;
            edge_sample_pos[side] = 0;
            edge->state = EDGE_STATE_SAMPLES;
            return;
        }
        if (use_corners) {
            edge->state = EDGE_STATE_CORNERS;
        } else {
            if (settings_byte_0xdf[0] & 0x80)
                edge_fit_params[side].right_side = 1;
            else
                edge_fit_params[side].right_side = 0;
            edge_sample_pos[side] = 0;
            if (side == 0)
                edge->state = EDGE_STATE_SAMPLES;
            else
                edge->state = EDGE_STATE_FIRST_EDGE_SAMPLES;
        }
    }
}

int edge_measure_slope(int *left_slope, int *right_slope, unsigned short from_line, unsigned short to_line, int side, struct edge_record *edge)
{
    unsigned short quarter, i, samples, upper, lower;
    int valid, right_upper, right_lower;
    unsigned short *left_edges, *right_edges;

    valid = 0;
    from_line = (from_line - edge->top_line) / edge_line_step;
    to_line = (to_line - edge->top_line) / edge_line_step;
    quarter = (to_line - from_line) / 4;
    if (quarter > 65)
        samples = 64;
    else if (quarter > 33)
        samples = 32;
    else if (quarter > 17)
        samples = 16;
    else if (quarter > 9)
        samples = 8;
    else if (quarter > 5)
        samples = 4;
    else
        return 1;
    *left_slope = *right_slope = 0;
    upper = from_line + quarter;
    lower = to_line - quarter;
    left_edges = edge->left_edges;
    right_edges = edge->right_edges;
    for (i = 0; i < samples; i++) {
        right_upper = right_edges[upper];
        right_lower = right_edges[lower];
        if (right_upper != 0xffff && right_lower != 0xffff) {
            *right_slope += right_upper - right_lower;
            *left_slope += left_edges[upper] - left_edges[lower];
            valid++;
        }
        upper++;
        lower++;
    }
    if (valid > 0) {
        *left_slope /= valid;
        *right_slope /= valid;
    } else {
        return 1;
    }
    return 0;
}

int edge_vote_skew_direction(int *votes_a, int *votes_b, unsigned short from_line, unsigned short to_line, int side, struct edge_record *edge, int *steep)
{
    unsigned short stride, i, samples, mean, upper, lower, count, pass, right_upper, right_lower, left_upper, left_lower, lower_skip, upper_skip, max_skip;
    unsigned short *left_edges, *right_edges, *walk_a, *walk_b;
    int sums[2];
    unsigned int height, left_spread, right_spread;
    unsigned short left_upper_list[16], right_upper_list[16], left_lower_list[16], right_lower_list[16];
    int steep_once;

    steep_once = 0;
    *steep = 0;
    from_line = (from_line - edge->top_line) / edge_line_step;
    to_line = (to_line - edge->top_line) / edge_line_step;
    stride = to_line - from_line;
    if (stride > 64) {
        samples = 32;
        stride = stride >> 6;
    } else {
        samples = stride >> 1;
        stride = 1;
    }
    if (stride > 8)
        max_skip = 8;
    else
        max_skip = stride;
    sums[0] = sums[1] = 0;
    count = 0;
    left_edges = edge->left_edges;
    right_edges = edge->right_edges;
    upper = from_line;
    lower = to_line;
    for (i = 0; i < samples; i++) {
        right_upper = right_edges[upper];
        lower_skip = upper_skip = 0;
        while (upper_skip < max_skip) {
            if (right_upper != 0xffff)
                break;
            upper_skip++;
            right_upper = *(right_edges + upper + upper_skip);
        }
        right_lower = right_edges[lower];
        while (lower_skip < max_skip) {
            if (right_lower != 0xffff)
                break;
            lower_skip++;
            right_lower = *(right_edges + lower - lower_skip);
        }
        if (right_upper != 0xffff && right_lower != 0xffff) {
            left_upper = *(left_edges + upper + upper_skip);
            left_lower = *(left_edges + lower - lower_skip);
            sums[0] += left_upper + left_lower;
            sums[1] += right_upper + right_lower;
            right_upper_list[count] = right_upper;
            right_lower_list[count] = right_lower;
            left_upper_list[count] = left_upper;
            left_lower_list[count] = left_lower;
            count++;
            if (*steep == 0) {
                height = (lower - lower_skip - upper - upper_skip) * edge_line_step;
                right_spread = abs(right_upper - right_lower);
                right_spread *= 57;
                left_spread = abs(left_upper - left_lower);
                left_spread *= 57;
                if (right_spread > height && left_spread > height) {
                    if (steep_once)
                        *steep = 1;
                    else
                        steep_once = 1;
                }
            }
        }
        if (count == 16)
            break;
        upper += stride;
        lower -= stride;
    }
    if (count > 0) {
        *votes_a = *votes_b = 0;
        samples = count;
        if (samples > 16)
            samples = 16;
        walk_a = left_upper_list;
        walk_b = left_lower_list;
        for (pass = 0; pass < 2; pass++) {
            mean = rt_divu32(sums[pass], count * 2);
            for (i = 0; i < samples; i++) {
                if (*walk_a > mean)
                    (*votes_a)++;
                else
                    (*votes_b)++;
                if (*walk_b > mean)
                    (*votes_b)++;
                else
                    (*votes_a)++;
                walk_a++;
                walk_b++;
            }
            walk_a = right_upper_list;
            walk_b = right_lower_list;
        }
    } else {
        return 1;
    }
    return 0;
}

void edge_state_32(int side, int saved_imask, struct edge_record *edge)
{
    unsigned short *left_edges, *right_edges;
    unsigned short i, count, left, right, min_left, max_right, left_corner, right_corner, saved_w18, saved_w20;
    int buf[1];

    count = (edge->bottom_line - edge->top_line) / edge_line_step;
    left_edges = edge->left_edges;
    right_edges = edge->right_edges;
    max_right = 0;
    min_left = 0xffff;
    if (edge_top_corners[side].left_lower) {
        left_corner = 0;
        right_corner = count;
    } else {
        left_corner = count;
        right_corner = 0;
    }
    for (i = 0; i < count; i++) {
        right = right_edges[i];
        if (right != 0xffff) {
            left = left_edges[i];
            if (left != 0) {
                if (left < min_left) {
                    min_left = left;
                    left_corner = i;
                }
                if (right > max_right) {
                    max_right = right;
                    right_corner = i;
                }
            }
        }
    }
    left_corner = edge_line_step * left_corner + edge->top_line;
    right_corner = edge_line_step * right_corner + edge->top_line;
    saved_w18 = edge->top_corner_line;
    saved_w20 = edge->bottom_corner_line;
    if (edge_top_corners[side].left_lower) {
        edge->top_corner_line = left_corner;
        edge->bottom_corner_line = right_corner;
    } else {
        edge->top_corner_line = right_corner;
        edge->bottom_corner_line = left_corner;
    }
    if (side == 0) {
        edge_corners_result = edge_check_corners(side, saved_imask, edge, &edge_fit_params[side].right_side);
    } else if (edge_corners_result == 0) {
        edge_corners_result = edge_check_corners(side, saved_imask, edge, buf);
    }
    if (edge_corners_result) {
        edge->top_corner_line = saved_w18;
        edge->bottom_corner_line = saved_w20;
    }
    edge_sample_pos[side] = 0;
    if (side == 0)
        edge->state = EDGE_STATE_SAMPLES;
    else
        edge->state = EDGE_STATE_FIRST_EDGE_SAMPLES;
}

int edge_check_corners(int side, int saved_imask, struct edge_record *edge, int *use_right_edge)
{
    unsigned short corners[8], index, j, angle_a, angle_b, angles[6], bias;
    unsigned short *first_edges, *second_edges;
    unsigned int ratio, run, length, rise;
    double square;

    if (edge_top_corners[side].left_lower) {
        first_edges = edge->left_edges;
        second_edges = edge->right_edges;
    } else {
        first_edges = edge->right_edges;
        second_edges = edge->left_edges;
    }
    index = 0;
    j = 0;
    while (1) {
        corners[0] = *(second_edges + index + j);
        if (corners[0] != 0xffff && corners[0] != 0)
            break;
        j++;
    }
    corners[1] = index;
    index = (edge->top_corner_line - edge->top_line) / edge_line_step;
    corners[2] = first_edges[index];
    corners[3] = index * edge_line_step;
    index = (edge->bottom_corner_line - edge->top_line) / edge_line_step;
    corners[4] = second_edges[index];
    corners[5] = index * edge_line_step;
    index = (edge->bottom_line - edge->top_line) / edge_line_step;
    j = 0;
    while (1) {
        corners[6] = *(first_edges + index - j);
        if (corners[6] != 0xffff && corners[6] != 0)
            break;
        j++;
    }
    corners[7] = index * edge_line_step;
    /* edge lengths in mm (px * 254 / (dpi * 10)); an edge under 50 mm fails */
    length = abs(corners[0] - corners[4]);
    rise = abs(corners[1] - corners[5]);
    square = (double)(length * length + rise * rise);
    length = sqrt(square);
    length = rt_divu16(length * 254, img_side_params[side].y_res_dpi * 10);
    if (length < 50)
        goto fail;
    length = abs(corners[2] - corners[6]);
    rise = abs(corners[3] - corners[7]);
    square = (double)(length * length + rise * rise);
    length = sqrt(square);
    length = rt_divu16(length * 254, img_side_params[side].y_res_dpi * 10);
    if (length < 50)
        goto fail;
    ratio = 204 * corners[3];
    run = abs(corners[0] - corners[2]);
    ratio = rt_divu32(ratio + run / 2, run);
    if (ratio > 204)
        goto fail;
    angle_a = edge_atan_table[ratio]; /* atan(i / 204) in 0.1 degree; i > 204 is beyond 45 degrees */
    ratio = abs(corners[4] - corners[0]);
    ratio *= 204;
    run = corners[5];
    ratio = rt_divu32(ratio + run / 2, run);
    if (ratio > 204)
        goto fail;
    angles[0] = angle_b = edge_atan_table[ratio];
    angles[2] = abs(angle_b - angle_a);
    ratio = abs(corners[6] - corners[2]);
    ratio *= 204;
    run = abs(corners[7] - corners[3]);
    ratio = rt_divu32(ratio + run / 2, run);
    if (ratio > 204)
        goto fail;
    angles[1] = angle_b = edge_atan_table[ratio];
    angles[3] = abs(angle_b - angle_a);
    ratio = abs(corners[7] - corners[5]);
    ratio *= 204;
    run = abs(corners[6] - corners[4]);
    ratio = rt_divu32(ratio + run / 2, run);
    if (ratio > 204)
        goto fail;
    if (side == 1)
        return 0;
    angle_a = edge_atan_table[ratio];
    angle_b = angles[0];
    angles[4] = abs(angle_b - angle_a);
    angle_b = angles[1];
    angles[5] = abs(angle_b - angle_a);
    angle_a = angles[2] + angles[4];
    angle_b = angles[3] + angles[5];
    bias = settings_byte_0xdf[0];
    if (edge_top_corners[side].left_lower) {
        if (bias & 0x80)
            angle_b += bias & 127;
        else
            angle_a += bias;
        if (angle_b > angle_a)
            *use_right_edge = 1;
        else
            *use_right_edge = 0;
    } else {
        if (bias & 0x80)
            angle_a += bias & 127;
        else
            angle_b += bias;
        if (angle_b > angle_a)
            *use_right_edge = 0;
        else
            *use_right_edge = 1;
    }
    return 0;
fail:
    if (settings_byte_0xdf[0] & 0x80)
        *use_right_edge = 1;
    else
        *use_right_edge = 0;
    return 1;
}

void edge_state_34(int side, int saved_imask, struct edge_record *edge)
{
    unsigned short stride, point, last_x, window, pos, target_pos, first, last, min_point, max_point;
    int x, taken, min_x, max_x;
    unsigned short *edges;
    int rising, use_right_edge, from_top;

    rising = edge_top_corners[side].left_lower;
    use_right_edge = edge_fit_params[side].right_side;
    if (edge_top_corners[side].left_lower == 0)
        from_top = 1;
    else
        from_top = 0;
    if (use_right_edge == 0) {
        if (from_top)
            from_top = 0;
        else
            from_top = 1;
    }
    if (from_top) {
        first = edge->top_corner_line - edge->top_line;
        first /= edge_line_step;
        last = edge->bottom_line;
    } else {
        first = 0;
        if (edge->bottom_corner_line == 0)
            last = edge->event_20_lines + line_page_state[side].top_offset_lines;
        else
            last = edge->bottom_corner_line;
    }
    last -= edge->top_line;
    last /= edge_line_step;
    stride = last - first;
    if (stride < 41) {
        edge->state = EDGE_STATE_FALLBACK;
        settings_edge_fail_code[0] = 3;
        return;
    }
    stride /= 129;
    stride += 1;
    edge_fit_params[side].sample_limit = (last - first) / stride;
    edge_fit_params[side].sample_limit--;
    if (stride > 32)
        window = 32;
    else
        window = stride;
    if (use_right_edge)
        edges = edge->right_edges;
    else
        edges = edge->left_edges;
    if (rising)
        last_x = 6;
    else
        last_x = 0xfff9;
    pos = edge_sample_pos[side];
    if (pos == 0) {
        edge_samples_taken[side] = 0;
        util_mem_clear(edge_samples, 0x100);
        edge_fit_params[side].min_x = 0x3900 / edge_line_step;
        edge_fit_params[side].max_x = 0;
        edge_fit_params[side].min_y = last * 2;
        edge_fit_params[side].max_y = 0;
        edge_fit_params[side].count = edge_fit_params[side].sample_count = 0;
    }
    taken = edge_fit_params[side].count;
    min_x = (edge_fit_params[side].min_x * edge_line_step) / 2;
    max_x = (edge_fit_params[side].max_x * edge_line_step) / 2;
    min_point = edge_fit_params[side].min_y / 2;
    max_point = edge_fit_params[side].max_y / 2;
    edge_samples_taken[side] += 32;
    if (edge_samples_taken[side] > edge_fit_params[side].sample_limit)
        edge_samples_taken[side] = edge_fit_params[side].sample_limit;
    target_pos = edge_samples_taken[side];
    while (pos < target_pos) {
        pos++;
        point = stride * pos + first;
        point = edge_find_point(point, window, edges, last_x, rising, use_right_edge);
        if (point != 0xffff) {
            x = edges[point];
            last_x = x;
            edge_samples[taken] = point;
            taken++;
            if (x < min_x)
                min_x = x;
            if (x > max_x)
                max_x = x;
            if (point < min_point)
                min_point = point;
            if (point > max_point)
                max_point = point;
        }
    }
    edge_fit_params[side].min_x = (min_x * 2) / edge_line_step;
    edge_fit_params[side].max_x = (max_x * 2) / edge_line_step;
    edge_fit_params[side].min_y = min_point * 2;
    edge_fit_params[side].max_y = max_point * 2;
    edge_fit_params[side].count = taken;
    edge_fit_params[side].sample_count = taken;
    edge_sample_pos[side] = pos;
    if (edge_sample_pos[side] >= edge_fit_params[side].sample_limit) {
        edge_sample_pos[side] = 0;
        if (edge_fit_params[side].count < edge_fit_params[side].sample_limit / 2) {
            edge->state = EDGE_STATE_FALLBACK;
            settings_edge_fail_code[0] = 4;
        } else {
            edge->state = EDGE_STATE_FIT_SUMS;
        }
    }
}

unsigned short edge_find_point(unsigned short start, unsigned short window, unsigned short *edges, unsigned short last_x, int rising, int use_right_edge)
{
    unsigned short count;
    unsigned short *ptr;

    count = 0;
    if (rising)
        last_x -= 6;
    else
        last_x += 6;
    ptr = edges + start;
    if (use_right_edge) {
        if (rising) {
            while (count < window) {
                if (*ptr != 0xffff && *ptr > last_x) {
                    start += count;
                    return start;
                }
                ptr++;
                count++;
            }
        } else {
            while (count < window) {
                if (*ptr != 0xffff && *ptr < last_x) {
                    start += count;
                    return start;
                }
                ptr++;
                count++;
            }
        }
    } else {
        if (rising) {
            while (count < window) {
                if (*ptr != 0 && *ptr > last_x) {
                    start += count;
                    return start;
                }
                ptr++;
                count++;
            }
        } else {
            while (count < window) {
                if (*ptr != 0 && *ptr < last_x) {
                    start += count;
                    return start;
                }
                ptr++;
                count++;
            }
        }
    }
    return 0xffff;
}
