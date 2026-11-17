#include "mdc.h"

unsigned short feed_wait_ticks;
unsigned char feed_state;
unsigned char feed_ramp_steps;
unsigned char feed_ramp_steps_next;
union feed_mode_bits feed_mode;
unsigned char feed_sheets_in_path;
unsigned char feed_pick_edge_state;
unsigned char feed_top_edge_state;
