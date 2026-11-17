#include "mdc.h"

union job_overlap_check_flags_bits job_overlap_check_flags;
unsigned short feed_overlap_starts[4];
unsigned short feed_overlap_lengths[4];
unsigned short feed_overlap_starts_saved[4];
unsigned short feed_overlap_lengths_saved[4];
unsigned short job_overlap_pattern_count;
unsigned short feed_overlap_match_state;
unsigned short feed_overlap_search_index;
unsigned short feed_overlap_count;
unsigned short feed_overlap_max_length;
unsigned short feed_ultrasonic_starts_a;
unsigned short feed_ultrasonic_starts_b;
unsigned short job_df_on_count;
unsigned short pick_df_on_count;
struct ultrasonic_check multifeed_check;
unsigned short job_overlap_patterns_a[OVERLAP_PATTERN_COUNT][4];
unsigned short job_overlap_patterns_b[OVERLAP_PATTERN_COUNT][4];
unsigned short log_index;
union word_view log_error_word;
unsigned char log_error_armed;
