#include "mdc.h"

volatile unsigned short feed_pick_edge_count;
volatile unsigned short feed_top_edge_count;
struct feed_edge_record feed_edge_records[FEED_EDGE_RECORD_COUNT];
