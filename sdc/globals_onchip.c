/* globals_onchip.c: uninitialised data (section BI). */
/* @section BI ffffe000 */
#include "fw.h"


#pragma section I
struct win_param_block win_params;
unsigned char win_unused_1[2];
union view_win_sides win_sides;
struct line_side_record line_side_state[2];
volatile unsigned short line_irq_count;
unsigned char line_unused_1[2];
volatile unsigned short line_motion_stop_lines[1];
volatile unsigned short line_motion_stop_lines_back[1];
unsigned short line_motion_flags;
unsigned char line_unused_2[2];
struct line_page_side line_page_state[2];
volatile struct Bits job_auto_size_flags;
struct img_crop_rect img_crop_area[1];
unsigned char img_crop_area_back[12];
struct cell_line_captured_lines line_captured_lines[3];
struct edge_record_set edge_side_records[2];
struct edge_fit_record edge_fit_params[2];
unsigned short edge_samples[128];
unsigned short edge_sample_values[128];
unsigned short edge_sample_pos[2];
unsigned short edge_samples_taken[2];
unsigned short edge_fit_state[2];
struct edge_sum_record edge_side_line_sums[2];
unsigned short edge_substate[2];
#pragma section
