/* common.h: the types that the declarations of several modules use, the chip's structs (periph.h),
   and the declarations that no source file uses and nothing defines. */
#ifndef COMMON_H
#define COMMON_H

/* types: bitfield bytes (b7 is the most significant bit) */
struct Bits {
    unsigned char b7:1,
                  b6:1,
                  b5:1,
                  b4:1,
                  b3:1,
                  b2:1,
                  b1:1,
                  b0:1;
};

/* the chip's peripheral structs (7144s.h); the ROM pointers to them are declared below */
#include "periph.h"
/* the protocol constants shared with the boot programs */
#include "scsi_defs.h"
#include "usb_defs.h"
#include "panel_defs.h"
/* views by offset into objects of other modules (macros that follow the link) */
#include "views_defs.h"

union view_scsi_initiator_bit { char c; struct Bits b; unsigned char u; };
struct win_composition_bits {
    unsigned short halftone_given:1,
                   separation_given:1,
                   b13:1,
                   b12:1;
    unsigned short multi_level:1,
                   color:1;
    unsigned short :6;
    unsigned short area_separation:1,
                   area_separation_ipc:1,
                   halftone:1,
                   error_diffusion:1;
};
struct win_side {
    unsigned short x_offset_inch1200;
    volatile unsigned short width_inch1200;
    int y_offset_inch1200;
    volatile int length_inch1200;
    int use_window_area;
    union {
        unsigned short v;
        struct { unsigned short argument:8, type:8; } f;
    } compression;
    union {
        unsigned short v;
        struct {
            unsigned short given:1;
            unsigned short :7;
            unsigned short on:1;
            unsigned short :7;
        } f;
    } outline;
    union win_reverse_image_word {
        unsigned short v;
        struct {
            unsigned short given:1;
            unsigned short :7;
            unsigned short on:1;
            unsigned short :7;
        } f;
    } reverse_image;
    union win_mirror_word {
        unsigned short v;
        struct {
            unsigned short given:1;
            unsigned short :7;
            unsigned short on:1;
            unsigned short :7;
        } f;
    } mirror;
    union {
        volatile unsigned short v;
        struct {
            unsigned short :6;
            unsigned short round_width_down:1,
                           round_length_up:1;
            unsigned short :8;
        } f;
    } size_rounding;
    union win_binarize_word {
        volatile unsigned short v;
        struct {
            unsigned short given:1,
                           sdtc_given:1;
            unsigned short :1;
            unsigned short sdtc:1,
                           dtc:1,
                           variance:3;
            unsigned short :8;
        } f;
        struct { unsigned char hi; unsigned char lo; } c;
    } binarize;
    union {
        volatile unsigned short v;
        struct win_composition_bits f;
    } composition;
    union {
        unsigned short v;
        struct {
            unsigned short downloaded:1,
                           b14:1;
            unsigned short :3;
            unsigned short pattern:3;
            unsigned short :8;
        } f;
        struct { unsigned char hi; unsigned char lo; } c;
    } halftone;
    union win_emphasis_word {
        unsigned short v;
        struct { unsigned short given:1; unsigned short :15; } f;
        struct { unsigned char hi; unsigned char lo; } c;
    } emphasis;
    unsigned char contrast;
    union {
        unsigned char v;
        struct { unsigned char p:5, v:3; } f;
    } gamma_pattern;
    unsigned char brightness_negated;
    union win_white_level_follow_byte {
        unsigned char v;
        struct {
            unsigned char given:1;
            unsigned char :6;
            unsigned char off:1;
        } f;
    } white_level_follow;
    volatile unsigned short dtc_options;
    char pad_28[2];
    union {
        unsigned char v;
        struct { unsigned char p:5, v:3; } f;
    } lut_pattern;
    char pad_2b[17];
};
union win_byte_0d { struct Bits f; volatile unsigned char v; };
struct win_param_block {
    union {
        volatile unsigned short v;
        unsigned short w;
    } paper_width_inch1200;
    union {
        volatile unsigned short v;
        unsigned short w;
    } paper_length_inch1200;
    union {
        volatile unsigned short v;
        unsigned short w;
        struct { unsigned char pad_00; volatile unsigned char lo; } b;
    } x_res_dpi;
    union {
        unsigned short v;
        struct { unsigned char pad_00; volatile unsigned char lo; } b;
    } y_res_dpi;
    union {
        volatile unsigned short v;
        unsigned short w;
        struct { unsigned char pad_00; volatile unsigned char lo; } b;
    } x_zoom_percent;
    union {
        volatile unsigned short v;
        unsigned short w;
        struct { unsigned char pad_00; volatile unsigned char lo; } b;
    } y_zoom_percent;
    union {
        volatile unsigned char v;
        volatile struct { unsigned char source_given:1, :6, adf:1; } f;
    } flags;
    union win_byte_0d b13;
    union win_resolution_mode_word {
        volatile unsigned short v;
        struct {
            unsigned short :1;
            unsigned short zoom_given:1;
            unsigned short :1;
            unsigned short x_zoom_given:1,
                           dpi_given:1;
            unsigned short :8;
            unsigned short resolution_code:3;
        } f;
        struct { unsigned char hi; unsigned char lo; } c;
    } resolution_mode;
    union {
        unsigned short v;
        struct {
            unsigned short given:1,
                           largest_paper:1;
            unsigned short :4;
            unsigned short long_page:1,
                           auto_length:1;
            unsigned short :1;
            unsigned short custom_size:1;
            unsigned short :1;
            unsigned short landscape:1,
                           paper_code:4;
        } f;
    } paper_size;
    unsigned short imprinter_y_offset;
    volatile struct Bits b20;
    char pad_15[3];
    struct win_side side[2];
};
struct edge_record {
    unsigned short state;
    unsigned short top_lines;
    unsigned short event_20_lines;
    char pad_06[2];
    unsigned short page_end_lines;
    unsigned short flags;
    int search_failed;
    unsigned short top_line;
    unsigned short top_corner_line;
    unsigned short bottom_corner_line;
    unsigned short bottom_line;
    unsigned short start_width_px;
    unsigned short width_px;
    unsigned short last_width_px;
    unsigned short deskew_end_lines;
    unsigned short top_left_px;
    unsigned short top_right_px;
    unsigned short *left_edges;
    unsigned short *right_edges;
};
struct port_f_bits {
    unsigned char :2,
                  scsi_id_switch:4,
                  c:1,
                  :1;
};
/* USB controller register 0x13 (+39, USB_Control) */
struct usb_control_bits {
    unsigned char dis_bus_detect:1,
                  :1,
                  in_suspend:1,
                  :2,
                  restore_usb:1,
                  go_chirp:1,
                  active_usb:1;
};
/* S1R72013 register +43 (XcvrControl): bits 7, 6 and OpMode (bits 1-0) */
struct usb_reg_43_bits {
    unsigned char term_select:1,
                  xcvr_select:1,
                  p:4,
                  op_mode:2;
};
struct img_gamma_pattern_bits {
    unsigned char download:1,
                  :1,
                  white_level_follow:1,
                  :1,
                  entry_valid:1,
                  second_set:1,
                  table_index:2;
};
struct img_gamma_key {
    union { char v; struct img_gamma_pattern_bits f; } pattern;
    char contrast, brightness, pad;
};

#endif
