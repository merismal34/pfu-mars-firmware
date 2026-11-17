#include "fw.h"

extern int abs(int);

void edge_state_b0(int side, int saved_imask, struct edge_record_view *edge)
{
    int failed;

    failed = 0;
    switch (edge_b0_state) {
    case 1:
        failed = edge_state_b0_step_1(side, 0, edge);
        if (failed != 0) {
            settings_edge_fail_code[0] = 34;
            edge_b0_state = 0xff;
            edge->state = EDGE_STATE_FALLBACK;
        } else {
            if (edge_b0_runs.v == 0xffff) {
                edge_b0_state = 2;
                edge_b0_runs.v = 0;
            }
        }
        break;
    case 2:
        failed = edge_state_b0_step_1(side, 1, edge);
        if (failed != 0) {
            settings_edge_fail_code[0] = 35;
            edge->state = EDGE_STATE_FALLBACK;
            edge_b0_state = 0xff;
        } else {
            if (edge_b0_runs.v == 0xffff) {
                edge_b0_state = 0xff;
                edge->state = EDGE_STATE_OPTIONAL_PASS_2;
                edge_c0_state = 4;
                edge_c0_runs.v = 0;
                edge_c0_retry = 0;
            }
        }
        break;
    }
}


int edge_state_b0_step_1(int side, int mode, struct edge_record_view *edge)
{
    struct edge_corner_set *corners;
    unsigned short *edges;
    unsigned short big_px;
    unsigned short small_px;
    unsigned short first_index;
    unsigned short last_index;
    short stride;
    unsigned short matches;
    int corner_x;
    int corner_y;
    int line;
    int expected_x;
    int edge_x;
    int distance;
    int failed;

    corners = &edge_top_corners[side];
    switch (img_side_params[side].x_res_dpi) {
    case 600:
        big_px = 56;
        small_px = 46;
        break;
    case 400:
        big_px = 38;
        small_px = 30;
        break;
    case 300:
        big_px = 28;
        small_px = 23;
        break;
    default:
        big_px = 19;
        small_px = 15;
        break;
    }
    stride = big_px / edge_line_step;
    if (mode != 0) {
        edges = (unsigned short *)edge->right_edges;
        corner_x = corners->right_x_px;
        corner_y = corners->right_line;
    } else {
        edges = (unsigned short *)edge->left_edges;
        corner_x = corners->left_x_px;
        corner_y = corners->left_line;
    }
    if (corner_y > edge->top_line)
        first_index = (corner_y - edge->top_line) / edge_line_step;
    else
        first_index = 0;
    last_index = (edge_page_geometry[side].source_lines + corner_y - edge->top_line) / edge_line_step;
    if (edge_b0_runs.v == 0) {
        edge_b0_index = edge_b0_start_index = first_index;
        edge_b0_last_width.v = 0;
        edge_b0_falling = 0;
    }
    failed = 0;
    edge_b0_steps = 0;
    edge_b0_flag = 0;
    matches = 0;
    distance = 0;
    while (edge_b0_index <= last_index) {
        edge_x = edges[edge_b0_index];
        if (edge_x == 0 || edge_x == 0xffff) {
            edge_b0_flag = 0;
        } else {
            line = edge->top_line + edge_b0_index * edge_line_step;
            if (edge_top_line[side].run == 0 || edge_top_line[side].rise == 0)
                expected_x = corner_x;
            else
                expected_x = (edge_top_line[side].rise * (line - corner_y) + corner_x * edge_top_line[side].run) / edge_top_line[side].run;
            if (mode != 0)
                distance = edge_x - expected_x;
            else
                distance = expected_x - edge_x;
            if (distance >= big_px) {
                failed = edge_state_b0_step_1_1(side, mode, edge_b0_index, first_index, last_index, &matches, edge);
                if (failed != 0)
                    break;
            } else if (distance >= small_px) {
                failed = edge_state_b0_step_1_1(side, mode, edge_b0_index, first_index, last_index, &matches, edge);
                if (failed != 0) {
                    if (distance - edge_b0_last_width.v >= big_px) {
                        failed = edge_state_b0_step_1_1(side, mode, edge_b0_start_index, first_index, last_index, &matches, edge);
                        if (failed != 0)
                            break;
                        else
                            edge_b0_falling = 1;
                    } else {
                        edge_b0_falling = 1;
                    }
                }
            } else if (edge_b0_falling != 0 && edge_b0_last_width.v - distance >= big_px) {
                failed = edge_state_b0_step_1_1(side, mode, edge_b0_index, first_index, last_index, &matches, edge);
                if (failed != 0)
                    break;
            }
            edge_b0_flag = 1;
            edge_b0_last_width.v = distance;
        }
        edge_b0_start_index = edge_b0_index;
        edge_b0_index += stride;
        edge_b0_runs.v++;
        edge_b0_steps++;
        failed = 0;
        if (edge_b0_steps >= 40)
            break;
    }
    if (edge_b0_index > last_index)
        edge_b0_runs.v = 0xffff;
    return failed;
}

int edge_state_b0_step_1_1(int side, int mode, unsigned short start, unsigned short first_index, unsigned short last_index, unsigned short *matches, struct edge_record_view *edge)
{
    struct edge_corner_set *corners;
    int edge_x;
    int skew_x;
    int start_x;
    int tolerance;
    unsigned short *edges;
    unsigned short needed;
    unsigned short count;
    unsigned short index;
    int upward;
    int seen;
    int found;

    corners = &edge_top_corners[side];
    switch (img_side_params[side].x_res_dpi) {
    case 600:
        needed = 14;
        tolerance = 48;
        break;
    case 400:
        needed = 10;
        tolerance = 32;
        break;
    case 300:
        needed = 7;
        tolerance = 24;
        break;
    default:
        needed = 5;
        tolerance = 16;
        break;
    }
    if (mode != 0)
        edges = (unsigned short *)edge->right_edges;
    else
        edges = (unsigned short *)edge->left_edges;
    start_x = edges[start];
    count = 1;
    index = start;
    upward = 1;
    seen = 0;
    while (1) {
        if (upward != 0) {
            if (index >= last_index) {
                if (seen != 0) {
                    found = 0;
                    break;
                } else {
                    upward = 0;
                    seen = 1;
                    index = start + 0xffff;
                }
            } else {
                index++;
            }
        } else {
            if (index <= first_index) {
                if (seen != 0) {
                    found = 0;
                    break;
                } else {
                    upward = 1;
                    seen = 1;
                    index = start + 1;
                }
            } else {
                index--;
            }
        }
        edge_x = edges[index];
        if (!(edge_top_line[side].run == 0 && edge_top_line[side].rise == 0))
            skew_x = 0;
        else
            skew_x = ((index - start) * edge_line_step * edge_top_line[side].rise) / edge_top_line[side].run;
        if (abs(edge_x + skew_x - start_x) <= tolerance) {
            count++;
            if (count >= needed) {
                found = 1;
                break;
            }
        } else {
            if (seen != 0) {
                found = 0;
                break;
            } else {
                if (upward != 0)
                    upward = 0;
                else
                    upward = 1;
                seen = 1;
                index = start;
            }
        }
    }
    *matches += count;
    return found;
}


void edge_state_c0(int side, int saved_imask, struct edge_record_view *edge)
{
    int failed;

    failed = 0;
    switch (edge_c0_state) {
    case 4:
        failed = edge_state_c0_step_1(side, edge);
        if (failed != 0) {
            settings_edge_fail_code[0] = 36;
            edge->state = EDGE_STATE_FALLBACK;
            edge_c0_state = 0xff;
        } else {
            if (edge_c0_runs.v == 0xffff) {
                if ((edge_top_line[side].run == 0 || edge_top_line[side].rise == 0) && edge_c0_retry == 0) {
                    edge_c0_runs.v = 0;
                    edge_top_corners[side].left_lower = ~edge_top_corners[side].left_lower;
                    edge_c0_retry = 1;
                    return;
                }
                edge_c0_state = 0xff;
                edge->state = EDGE_STATE_APPLY;
            }
        }
        break;
    }
}

int edge_state_c0_step_1(int side, struct edge_record_view *edge)
{
    struct edge_corner_set *corners;
    unsigned short *edges;
    unsigned short start_index;
    unsigned short last_index;
    short extra_steps;
    short d52;
    int corner_x;
    int corner_y;
    int line;
    int edge_x;
    int lo;
    int hi;
    int d24;
    int start_y;
    int expected_line;
    int start_x;
    int distance;
    int tolerance;
    int no_slope;

    no_slope = 0;
    if (edge_top_line[side].run == 0 || edge_top_line[side].rise == 0)
        no_slope = 1;
    switch (img_side_params[side].x_res_dpi) {
    case 600:
        tolerance = 48;
        extra_steps = 13;
        break;
    case 400:
        tolerance = 32;
        extra_steps = 9;
        break;
    case 300:
        tolerance = 24;
        extra_steps = 7;
        break;
    default:
        tolerance = 16;
        extra_steps = 5;
        break;
    }
    corners = &edge_top_corners[side];
    if (edge_top_corners[side].left_lower != 0) {
        edges = (unsigned short *)edge->right_edges;
        corner_x = corners->right_x_px;
        corner_y = corners->right_line;
        last_index = (corners->left_line - corners->right_line) / edge_line_step;
        start_y = corners->right_line;
    } else {
        edges = (unsigned short *)edge->left_edges;
        corner_x = corners->left_x_px;
        corner_y = corners->left_line;
        last_index = (corners->right_line - corners->left_line) / edge_line_step;
        start_y = corners->left_line;
    }
    start_index = (edge_page_geometry[side].source_lines + start_y - edge->top_line) / edge_line_step;
    last_index += start_index;
    last_index += extra_steps;
    start_x = edges[start_index];
    if (edge_c0_runs.v == 0) {
        edge_c0_index = edge_c0_start_index = start_index;
    }
    edge_c0_steps.v = 0;
    while (edge_c0_index <= last_index) {
        edge_x = edges[edge_c0_index];
        if (edge_x != 0 && edge_x != 0xffff) {
            if (no_slope) {
                lo = hi = start_x;
                lo -= tolerance;
                hi += tolerance;
                if (edge_x < lo || hi < edge_x)
                    return 1;
            } else {
                line = edge->top_line + edge_c0_index * edge_line_step;
                corner_y = edge->top_line + start_index * edge_line_step;
                expected_line = (edge_top_line[side].rise * (start_x - edge_x) + corner_y * edge_top_line[side].run) / edge_top_line[side].run;
                distance = line - expected_line;
                if (distance > tolerance)
                    return 1;
            }
        }
        edge_c0_start_index = edge_c0_index;
        edge_c0_index++;
        edge_c0_runs.v++;
        edge_c0_steps.v++;
        if (edge_c0_steps.v > 40)
            break;
    }
    if (edge_c0_index > last_index)
        edge_c0_runs.v = 0xffff;
    return 0;
}
