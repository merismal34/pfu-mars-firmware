/* edge.h: declarations of edge_search.c, edge_page_end.c, edge_fit_sums.c, edge_fit.c,
   edge_fit_solve.c, edge_bottom.c and 2 more files. */
#ifndef EDGE_H
#define EDGE_H
#include "common.h"

/* edge_record.state: the edge detection state of a side (edge_run_*_state) */
#define EDGE_FLAG_FALLBACK   0x0004
#define EDGE_FLAG_SIZE_FINAL 0x0080
#define EDGE_STATE_START               0x00
#define EDGE_STATE_THRESHOLDS          0x04
#define EDGE_STATE_LEVEL_WATCH         0x08
#define EDGE_STATE_LEVEL_ADJUST        0x0C
#define EDGE_STATE_COLLECT             0x10
#define EDGE_STATE_WIDTH_TRACK         0x14
#define EDGE_STATE_SKEW_DIRECTION      0x30
#define EDGE_STATE_CORNERS             0x32
#define EDGE_STATE_SAMPLES             0x34
#define EDGE_STATE_FIT_SUMS            0x38
#define EDGE_STATE_FIT                 0x3C
#define EDGE_STATE_FIRST_EDGE_SAMPLES  0x3E
#define EDGE_STATE_FIRST_EDGE_FIT      0x40
#define EDGE_STATE_SECOND_EDGE_SAMPLES 0x44
#define EDGE_STATE_SECOND_EDGE_FIT     0x48
#define EDGE_STATE_TOP_SAMPLES         0x4C
#define EDGE_STATE_TOP_INTERSECT       0x50
#define EDGE_STATE_TOP_CORNERS         0x60
#define EDGE_STATE_EDGE_LENGTH         0x64
#define EDGE_STATE_BOTTOM_SAMPLES      0x80
#define EDGE_STATE_BOTTOM_INTERSECT    0x84
#define EDGE_STATE_PAPER_LENGTH        0xA0
#define EDGE_STATE_OPTIONAL_PASS_1     0xB0
#define EDGE_STATE_OPTIONAL_PASS_2     0xC0
#define EDGE_STATE_FALLBACK            0x100
#define EDGE_STATE_APPLY               0x1000
#define EDGE_STATE_WAIT_BOTTOM         0x8000
#define EDGE_STATE_DONE                0xFFFF

struct edge_record_words {
    unsigned short state;
    unsigned short pad_02;
    unsigned short event_20_lines;
    unsigned short pad_06;
    short page_end_lines;
    unsigned short flags;
};
struct edge_record_tables {
    char pad_00[36];
    unsigned short *left_edges;
    unsigned short *right_edges;
};
struct edge_record_state { unsigned short state; unsigned short top_lines; };
struct edge_record_view {
    unsigned short state;
    char pad_02[14];
    unsigned short top_line;
    char pad_12[18];
    int left_edges;
    int right_edges;
};
struct edge_record_edges {
    unsigned short state;
    char pad_02[34];
    unsigned short *left_edges;
    unsigned short *right_edges;
};

/* functions */
extern void edge_run_line_step(int side, int saved_imask, struct edge_record *edge);
extern void edge_run_page_step(int, int, unsigned short *);
extern void edge_end_long_page(int side, int saved_imask);
extern void edge_run_setup_state(int side, int saved_imask, unsigned short *edge);
extern void edge_state_00(int side, int saved_imask, unsigned short *edge);
extern void edge_state_04(int side, int saved_imask, unsigned short *edge);
extern void edge_state_08(int side, int saved_imask, unsigned short *edge);
extern void edge_state_0c(int side, int saved_imask, unsigned short *edge);
extern void edge_run_search_state(int side, int saved_imask, short line, struct edge_record *edge);
extern void edge_state_10(int side, int saved_imask, short line, struct edge_record *edge);
extern void edge_state_14(int side, int saved_imask, short line, struct edge_record *edge);
extern void edge_state_14_step_3(int side, short width, short line, struct edge_record *edge);
extern void edge_search_line(int side, short line);
extern void edge_read_line(int side, short line, unsigned short *edge);
extern void edge_run_page_end_state(int side, int saved_imask, struct edge_record *edge);
extern void edge_state_30(int side, int saved_imask, struct edge_record *edge);
extern int edge_measure_slope(int *left_slope, int *right_slope, unsigned short from_line, unsigned short to_line, int side, struct edge_record *edge);
extern int edge_vote_skew_direction(int *votes_a, int *votes_b, unsigned short from_line, unsigned short to_line, int side, struct edge_record *edge, int *steep);
extern void edge_state_32(int side, int saved_imask, struct edge_record *edge);
extern int edge_check_corners(int side, int saved_imask, struct edge_record *edge, int *use_right_edge);
extern void edge_state_34(int side, int saved_imask, struct edge_record *edge);
extern unsigned short edge_find_point(unsigned short start, unsigned short window, unsigned short *edges, unsigned short last_x, int rising, int use_right_edge);
extern void edge_state_38(int side, int saved_imask, struct edge_record_edges *edge);
extern void edge_state_3c(int side, int saved_imask, short *edge);
extern void edge_state_3e(int side, int saved_imask, struct edge_record *edge);
extern int edge_sample_side(int side, short from, short to, struct edge_record *edge);
extern void edge_state_40(int side, int unused, unsigned short *edge);
extern void edge_state_44(int side, int unused, struct edge_record *edge);
extern void edge_state_48(int side, int unused, unsigned short *edge);
extern void edge_state_4c(int side, int saved_imask, struct edge_record *edge);
extern void edge_state_50(int side, int saved_imask, struct edge_record *edge);
extern void edge_copy_samples(int side, unsigned short start, short stride, short count, short window, unsigned int rising, struct edge_record *edge);
extern void edge_state_60(int side, int saved_imask, short *edge);
extern void edge_state_64(int side, int saved_imask, struct edge_record *edge);
extern void edge_compute_length(int side, int saved_imask, struct edge_record *edge);
extern void edge_state_64_step_2(int side, int saved_imask, struct edge_record *edge);
extern void edge_fit_state_0(int, int);
extern int edge_scale_fit_span(int, int);
extern int edge_fit_state_100_step_1(int side);
extern void edge_fit_state_10(int side, int unused, struct edge_record_tables *edge);
extern short edge_fit_state_100(int, int);
extern void edge_fit_state_1000(int side, int unused, struct edge_record_tables *edge);
extern void edge_run_substate(int, int, int, void *);
extern void edge_substate_0(int side, int rising);
extern void edge_substate_10(int side, int mode, int rising, struct edge_record *edge);
extern void edge_substate_100(int side, struct edge_record_state *edge);
extern void edge_substate_400(int side, unsigned short *edge);
extern void edge_substate_1000(int side, int mode, int rising, struct edge_record *edge);
extern void edge_substate_10_step_2(unsigned short *dest, int count, unsigned short *samples, int side, short rising, int mode, unsigned short *edges, struct edge_record *edge);
extern void edge_substate_10_step_3(unsigned short *dest, int count, unsigned short *samples, int side, short rising, unsigned short *edges);
extern unsigned short edge_state_4c_step_1(int side, struct edge_record *edge);
extern unsigned short edge_state_50_step_2(int side, struct edge_record *edge);
extern int edge_intersect_top_line(int side, int saved_imask, int flip);
extern void edge_state_8000(int side, int saved_imask, struct edge_record *edge);
extern void edge_state_8000_step_1(int side, int saved_imask, struct edge_record *edge);
extern void edge_state_80(int side, int saved_imask, struct edge_record *edge);
extern void edge_state_84(int side, int saved_imask, struct edge_record *edge);
extern void edge_state_a0(int side, int saved_imask, struct edge_record *edge);
extern int edge_scale_lines(int side, int lines);
extern void edge_update_output_lines(int side, int saved_imask);
extern unsigned short edge_inch1200_to_px(int, int);
extern unsigned short edge_inch1200_to_lines(int side, int inch1200);
extern void edge_align_width(int side, unsigned short *width);
extern int edge_round(double value);
extern unsigned short edge_align_lines(unsigned short lines, int compressed);
extern void edge_state_b0(int side, int saved_imask, struct edge_record_view *edge);
extern int edge_state_b0_step_1(int side, int mode, struct edge_record_view *edge);
extern int edge_state_b0_step_1_1(int side, int mode, unsigned short start, unsigned short first_index, unsigned short last_index, unsigned short *matches, struct edge_record_view *edge);
extern void edge_state_c0(int side, int saved_imask, struct edge_record_view *edge);
extern int edge_state_c0_step_1(int side, struct edge_record_view *edge);

/* functions */
extern void edge_run_result_state(int side, int saved_imask, struct edge_record_words *edge);
extern void edge_state_1000_a(int saved_imask, struct edge_record_words *edge);
extern void edge_state_100(int side, int saved_imask, struct edge_record_words *edge);
extern void edge_state_1000_b(int side, unsigned saved_imask, struct edge_record_words *edge);
extern void edge_state_1000_c(int side, unsigned saved_imask, struct edge_record_words *edge);
extern void edge_state_1000_d(int side, unsigned saved_imask, struct edge_record_words *edge);
extern void edge_state_1000_e(int side, unsigned saved_imask, struct edge_record_words *edge);
extern void edge_state_1000_e_step_1(int side, short width_px);
extern void edge_state_1000_e_step_3(int side, short width_px, struct edge_record_words *edge);
extern unsigned short edge_compute_paper_length(int side, unsigned short source_lines);

/* constants */
struct edge_corner_shift {
    unsigned short cos_minus_sin;
    unsigned short cos_plus_sin;
};
extern const struct edge_corner_shift edge_corner_shift_table[52];
extern const unsigned short edge_atan_table[205];
extern const unsigned short edge_cos_table[105];

#endif
