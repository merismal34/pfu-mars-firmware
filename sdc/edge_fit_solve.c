#include "fw.h"

void edge_fit_state_0(int side, int saved_imask)
{
    double sxx;
    double sxy;
    double slope;
    double denominator;
    double value;
    int failed;
    int spread;
    int min_spread;
    int minimum;

    failed = 1;
    spread = edge_fit_params[side].max_x - edge_fit_params[side].min_x;
    min_spread = edge_scale_fit_span(side, 228);
    if (img_side_params[side].y_res_dpi > 300)
        minimum = 3;
    else
        minimum = 2;
    if (min_spread < minimum)
        min_spread = minimum;
    if (spread > min_spread) {
        sxx = edge_fit_params[side].count - (double)edge_fit_params[side].sum_x * edge_fit_params[side].sum_x / edge_fit_params[side].sum_xx;
        sxy = edge_fit_params[side].sum_y - (double)edge_fit_params[side].sum_x * edge_fit_params[side].sum_xy / edge_fit_params[side].sum_xx;
        if (sxy != 0.0 || sxx != 0.0) {
            slope = sxy / sxx;
            if (slope < 8388600.0 && slope > -8388600.0) {
                denominator = edge_fit_params[side].sum_xy - edge_fit_params[side].sum_x * slope;
                value = edge_fit_params[side].sum_xx;
                edge_top_line[side].run = 0x10000;
                value = value * 65536.0 / denominator;
                edge_top_line[side].rise = edge_round(value);
                failed = 0;
            }
        }
    }
    if (failed != 0) {
        edge_top_line[side].run = 0;
        edge_top_line[side].rise = 0;
        edge_fit_state[side] = 0x8000;
    } else {
        edge_sample_pos[side] = 0;
        if (edge_fit_done != 0)
            edge_fit_state[side] = 0x8000;
        else
            edge_fit_state[side] = 16;
    }
}

int edge_scale_fit_span(int side, int factor)
{
    int scaled;
    int span;

    span = edge_fit_params[side].max_y - edge_fit_params[side].min_y;
    scaled = span * factor / 65536;
    return scaled;
}


extern int abs(int);

int edge_fit_state_100_step_1(int side)
{
    int too_steep;
    int spread;
    int span;
    int limit;
    int slope;
    double value;
    too_steep = 0;
    spread = edge_fit_params[side].max_x - edge_fit_params[side].min_x;
    spread += edge_fit_margin.v * 2;
    span = edge_fit_params[side].max_y - edge_fit_params[side].min_y;
    value = (double)spread * 65536.0 / (double)span;
    limit = edge_round(value);
    slope = abs(edge_top_line[side].rise);
    if (slope > limit) {
        too_steep = 1;
    }
    return too_steep;
}


void edge_fit_state_10(int side, int unused, struct edge_record_tables *edge)
{
    unsigned short *edges;
    unsigned short *sample;
    unsigned short *dest;
    unsigned short pos;
    unsigned short next;
    unsigned short current;
    unsigned short end;
    int predicted;
    int current_x;
    int divisor;
    int slope_step;
    int first;

    if (edge_fit_params[side].right_side != 0) {
        edges = edge->right_edges;
    } else {
        edges = edge->left_edges;
    }
    divisor = edge_top_line[side].run;
    slope_step = edge_top_line[side].rise * edge_line_step;
    pos = edge_sample_pos[side];
    sample = edge_samples + pos;
    dest = edge_sample_values + pos;
    if (pos == 0) {
        end = edge_fit_params[side].sample_count / 4;
        util_mem_clear(edge_sample_values, 0x100);
        current = 0;
        next = 0;
        first = 1;
    } else {
        if (pos == ((edge_fit_params[side].sample_count / 4 - 1) & 0xffff)) {
            end = edge_fit_params[side].sample_count / 2;
        } else if (pos == (unsigned short)edge_fit_params[side].sample_count / 2 - 1) {
            end = edge_fit_params[side].sample_count / 2 + edge_fit_params[side].sample_count / 4;
        } else {
            end = edge_fit_params[side].sample_count;
        }
        current = edge_fit_saved_a;
        next = edge_fit_saved_b;
        first = edge_fit_first;
        current_x = edges[current];
    }
    end--;
    for (; pos < end; pos++) {
        if (current == 0) {
            current = *sample;
            current_x = edges[*sample];
        }
        next = sample[1];
        if (current != 0 && next != 0) {
            if (first != 0) {
                predicted = (current - next) * slope_step / divisor;
                predicted += edges[next];
                *dest = abs(current_x - predicted);
                first = 0;
            }
            predicted = (next - current) * slope_step / divisor + current_x;
            dest[1] = abs(edges[next] - predicted);
            current = 0;
            next = 0;
        } else {
            dest[1] = 0;
        }
        sample++;
        dest++;
    }
    edge_sample_pos[side] = pos;
    if (edge_sample_pos[side] > ((edge_fit_params[side].sample_count - 2) & 0xffff)) {
        edge_sample_pos[side] = 0;
        edge_fit_state[side] = 0x100;
    } else {
        edge_fit_saved_a = current;
        edge_fit_saved_b = next;
        edge_fit_first = first;
    }
}


short edge_fit_state_100(int side, int unused)
{
    int too_steep;
    unsigned short worst;
    unsigned short neighbour;
    unsigned short max_error;
    unsigned short pass;
    unsigned short *values;
    unsigned short *samples;
    int count;
    int i;
    int best;

    count = edge_fit_params[side].sample_count;
    samples = edge_samples;
    values = edge_sample_values;
    too_steep = edge_fit_state_100_step_1(side);
    edge_fit_rows[0] = edge_fit_row0_b[0] = edge_fit_row1_a[0] = edge_fit_row1_b[0] = 0;
    for (pass = 0; pass < 2; pass++) {
        max_error = 0;
        for (i = 0; i < count; i++) {
            if (values[i] > max_error && samples[i] != 0) {
                max_error = values[i];
                worst = samples[i];
                best = i;
            }
        }
        if (max_error > edge_fit_margin.v || too_steep == 1) {
            samples[best] = 0;
            values[best] = 0;
            neighbour = 0;
            if (best != 0) {
                for (i = 1; i < best + 1; i++) {
                    neighbour = *(samples + best - i);
                    if (neighbour != 0) {
                        *(samples + best - i) = 0;
                        *(values + best - i) = 0;
                        break;
                    }
                }
            } else {
                for (i = 1; i < count; i++) {
                    neighbour = samples[i];
                    if (neighbour != 0) {
                        samples[i] = 0;
                        values[i] = 0;
                        break;
                    }
                }
            }
            edge_fit_rows[pass * 2] = worst;
            edge_fit_row0_b[pass * 2] = neighbour;
        } else {
            if (pass == 0) {
                edge_fit_state[side] = 0x8000;
                return max_error;
            } else {
                edge_fit_done = 1;
            }
        }
    }
    edge_fit_state[side] = 0x1000;
    return max_error;
}


void edge_fit_state_1000(int side, int unused, struct edge_record_tables *edge)
{
    unsigned short sample;
    unsigned short row;
    int x;
    int removed;
    unsigned short *edges;
    unsigned short *walk;
    unsigned short min_sample;
    unsigned short max_sample;
    unsigned short end;
    unsigned short pos;
    int min_x;
    int max_x;

    if (edge_fit_params[side].right_side != 0) {
        edges = edge->right_edges;
    } else {
        edges = edge->left_edges;
    }
    if (edge_sample_pos[side] == 0) {
        for (row = 0; row < 4; row++) {
            removed = edge_fit_rows[row];
            if (removed != 0) {
                x = edges[removed];
                x += x;
                x /= edge_line_step;
                edge_fit_params[side].sum_xx -= x * x;
                edge_fit_params[side].sum_x -= x;
                edge_fit_params[side].sum_xy -= x * removed * 2;
                edge_fit_params[side].sum_y -= removed * 2;
                edge_fit_params[side].count -= 1;
            }
        }
        edge_fit_params[side].min_x = 0x3900 / edge_line_step;
        edge_fit_params[side].max_x = 0;
        edge_fit_params[side].min_y = 0xffff;
        edge_fit_params[side].max_y = 0;
        end = 0x40;
    } else {
        end = edge_fit_params[side].sample_limit;
    }
    min_x = edge_fit_params[side].min_x * edge_line_step / 2;
    max_x = edge_fit_params[side].max_x * edge_line_step / 2;
    min_sample = edge_fit_params[side].min_y / 2;
    max_sample = edge_fit_params[side].max_y / 2;
    pos = edge_sample_pos[side];
    walk = edge_samples + pos;
    for (; pos < end; pos++, walk++) {
        if (*walk != 0) {
            sample = *walk;
            x = edges[sample];
            if (x < min_x) {
                min_x = x;
            }
            if (x > max_x) {
                max_x = x;
            }
            if (sample < min_sample) {
                min_sample = sample;
            }
            if (sample > max_sample) {
                max_sample = sample;
            }
        }
    }
    edge_fit_params[side].min_x = min_x * 2 / edge_line_step;
    edge_fit_params[side].max_x = max_x * 2 / edge_line_step;
    edge_fit_params[side].min_y = min_sample * 2;
    edge_fit_params[side].max_y = max_sample * 2;
    edge_sample_pos[side] = pos;
    if (edge_sample_pos[side] >= edge_fit_params[side].sample_limit) {
        edge_sample_pos[side] = 0;
        edge_fit_state[side] = 0;
    }
}
