/* jpeg.h: declarations of jpeg.c, jpeg_buffer.c. */
#ifndef JPEG_H
#define JPEG_H
#include "common.h"

/* the long-page reserve of the JPEG capture buffers (jpeg_init_capture_buffers) */
#define JPEG_LONG_PAGE_RESERVE_4M 0x400000
#define JPEG_LONG_PAGE_RESERVE_7M 0x700000
/* jpeg_scan_flags.v: the long-page capture pause (line_start_capture, line_handle_page_events, jpeg_resume_capture) */
#define JPEG_SCAN_RESUMING     0x02
#define JPEG_SCAN_PAUSED       0x04
#define JPEG_SCAN_PAUSE_WANTED 0x08
#define JPEG_SCAN_STOPPED      0x10
#define JPEG_SCAN_LONG_PAGE    0x80

/* functions */
extern void jpeg_init_scan(void);
extern void jpeg_setup_next_page(int side);
extern void jpeg_setup(int side, unsigned short width_px, unsigned short length_lines);
extern void jpeg_write_qtable(unsigned short *dst, unsigned char *src);
extern void jpeg_init_capture_buffers(int side);
extern int jpeg_init_scan_step_2(void);
extern void jpeg_sync_buffer_pointer(int, int);
extern void jpeg_resume_capture(int saved_imask);
extern void jpeg_check_buffer_limit(int);

/* constants */
extern const unsigned char jpeg_qtables_81[64];
extern const unsigned char jpeg_chroma_qtables_81[704];
extern const unsigned char jpeg_qtable_finest[64];
extern const unsigned char jpeg_chroma_qtable_finest[64];
extern const unsigned char jpeg_qtables_82[64];
extern const unsigned char jpeg_chroma_qtables_82[832];
extern const unsigned char jpeg_huffman_dc_luma_bits[16];
extern const unsigned char jpeg_huffman_dc_luma_values[12];
extern const unsigned char jpeg_huffman_dc_chroma_bits[16];
extern const unsigned char jpeg_huffman_dc_chroma_values[12];
extern const unsigned char jpeg_huffman_ac_luma_bits[16];
extern const unsigned char jpeg_huffman_ac_luma_values[162];
extern const unsigned char jpeg_huffman_ac_chroma_bits[16];
extern const unsigned char jpeg_huffman_ac_chroma_values[162];

#endif
