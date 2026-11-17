#include "fw.h"

void edge_state_38(int side, int saved_imask, struct edge_record_edges *edge)
{
    short sample;
    short end;
    short i;
    int x;
    int sum_xx;
    int sum_x;
    int sum_xy;
    int sum_y;
    unsigned short *edges;

    i = edge_sample_pos[side];
    if (edge_fit_params[side].right_side != 0)
        edges = edge->right_edges;
    else
        edges = edge->left_edges;
    sum_xx = sum_x = sum_xy = sum_y = 0;
    if (i == 0) {
        edge_fit_params[side].sum_xx = edge_fit_params[side].sum_x = edge_fit_params[side].sum_xy = edge_fit_params[side].sum_y = 0;
        end = (unsigned short)edge_fit_params[side].sample_count / 4;
    } else if ((unsigned short)i == (unsigned short)(edge_fit_params[side].sample_count / 4)) {
        end = edge_fit_params[side].sample_count / 4 + edge_fit_params[side].sample_count / 2;
    } else {
        end = edge_fit_params[side].sample_count;
    }
    for (; (unsigned)i < (unsigned)end; i++) {
        sample = edge_samples[(unsigned short)i];
        x = edges[(unsigned short)sample];
        x += x;
        x = x / edge_line_step;
        sum_xx += x * x;
        sum_x += x;
        sum_xy += (unsigned short)sample * x * 2;
        sum_y += (unsigned short)sample * 2;
    }
    edge_fit_params[side].sum_xx += sum_xx;
    edge_fit_params[side].sum_x += sum_x;
    edge_fit_params[side].sum_xy += sum_xy;
    edge_fit_params[side].sum_y += sum_y;
    if ((unsigned short)i == (unsigned short)edge_fit_params[side].sample_count) {
        if (img_side_params[side].y_res_dpi > 300)
            edge_fit_margin.v = 4;
        else
            edge_fit_margin.v = 2;
        edge_fit_limit.s.v = edge_scale_fit_span(side, 0x3d7);
        if (edge_fit_limit.s.v < edge_fit_margin.v + 2)
            edge_fit_limit.s.v = edge_fit_margin.v + 2;
        edge_fit_done = 0;
        edge_fit_state[side] = 0;
        edge->state = EDGE_STATE_FIT;
    }
    edge_sample_pos[side] = i;
}
