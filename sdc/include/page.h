/* page.h: declarations of page_progress.c, page_tick.c, page_multifeed.c, page_complete.c,
   page_stall.c. */
#ifndef PAGE_H
#define PAGE_H
#include "common.h"

struct page_record_tail { int w[6]; };

/* page_record.flags.v masks (the bits: struct page_record's f view) */
#define PAGE_FLAG_SHEET_FED         0x00020000
#define PAGE_FLAGS_FED_OR_SCANNED   0x00030000
#define PAGE_FLAGS_SCAN             0x01700000
#define PAGE_FLAGS_LOAD_OR_FED      0x08030000
#define PAGE_FLAGS_REPLY_PENDING    0x3c000000
#define PAGE_FLAGS_ACTIVE           0x3d700000

/* functions */
extern void page_poll_progress(void);
extern void page_update_image_counts(void);
extern void page_tick(void);
extern void page_run_side_events(int side);
extern void page_event_80(unsigned int side);
extern void page_event_20(int side);
extern void page_event_04(unsigned int side);
extern void page_event_01(int side);
extern void page_event_40(int side);
extern void page_force_end(void);
extern void page_flush_image_size(void);
extern void page_event_08(void);
extern void page_event_02(int side);
extern void page_event_02_step_1(int side);
extern void page_set_image_size(void);
extern void page_event_40_step_1(int side);
extern void page_event_40_step_3(int side);
extern int page_next_index(int);
extern void page_copy_multifeed_flags(int);

/* functions */
extern int page_mark_complete(int);
extern void page_set_stall_limit(int);

/* variables */
extern int page_poll_imask;
extern unsigned short page_jpeg_count_retries[];
extern unsigned short page_poll_wait_count;
extern unsigned short page_read_lines[];
extern struct page_record_tail page_saved_record_tail;
extern int page_pass_held[];
extern int page_tick_imask;

#endif
