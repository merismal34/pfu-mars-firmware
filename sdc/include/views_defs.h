/* views_defs.h: names for a part of an object that another module defines, written as macros by
   offset from the object, so that they follow it when the link moves it. Program 2's views are in
   boot.h. The views that only a separate symbol reproduces are defined by address at link time. */
#ifndef VIEWS_DEFS_H
#define VIEWS_DEFS_H

/* the power-save image of the MDC shared RAM inside edge_left_front (unsigned short [1400], globals.c;
   main_tasks.c power_enter_save / power_leave_save) */
#define power_image_startup_state ((unsigned short *)((unsigned char *)edge_left_front + 0x2))
#define power_image_mdc_version_0 ((union word_bytes *)((unsigned char *)edge_left_front + 0x4))
#define power_image_mdc_version_1 ((union word_bytes *)((unsigned char *)edge_left_front + 0x6))
#define power_image_mdc_version_2 ((union word_bytes *)((unsigned char *)edge_left_front + 0x8))
#define power_image_mdc_block_info ((unsigned char *)((unsigned char *)edge_left_front + 0x19))
#define power_image_settings ((unsigned char *)((unsigned char *)edge_left_front + 0x40))
#define power_image_imprinter_version_0 ((union word_bytes *)((unsigned char *)edge_left_front + 0x272))
#define power_image_imprinter_version_1 ((union word_bytes *)((unsigned char *)edge_left_front + 0x274))
#define power_image_wake_flags (*(unsigned short *)((unsigned char *)edge_left_front + 0x2e4))

/* the low bytes of the divisor words uart_divisor_table (const unsigned short [], uart_link.c) */
#define uart_divisor_low_bytes ((const unsigned char *)uart_divisor_table + 1)

#endif
