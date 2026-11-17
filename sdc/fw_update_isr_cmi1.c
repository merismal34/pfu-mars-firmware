#include "boot.h"
#pragma section 2
#pragma interrupt(fw_update_isr_cmi1)
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

void fw_update_isr_cmi1(void)
{
    fw_update_cmi1_saved_imask = _builtin_get_imask();
    if (!fw_update_p_cmt1->cmcsr_l.b7) return;
    _builtin_set_imask(15);
    fw_update_p_cmt1->cmcsr_l.b7 = 0;
    _builtin_set_imask(fw_update_cmi1_saved_imask);
    fw_update_tick_count++;
    fw_update_display_ticks.v++;
    fw_update_send_timeout++;
    if (fw_update_display_ticks.v > fw_update_display_last_tick.v) {
        if ((unsigned short)(fw_update_display_ticks.v - fw_update_display_last_tick.v) >= 125) {
            fw_update_display_last_tick.v = fw_update_display_ticks.v;
            fw_update_display_step.v++;
            if (fw_update_erased_mask) fw_update_panel_set_digit(fw_update_display_digits[fw_update_display_step.v & 15]);
        }
    } else {
        if ((unsigned short)(~fw_update_display_last_tick.v + fw_update_display_ticks.v + 1) >= 125) {
            fw_update_display_last_tick.v = fw_update_display_ticks.v;
            fw_update_display_step.v++;
            if (fw_update_erased_mask) fw_update_panel_set_digit(fw_update_display_digits[fw_update_display_step.v & 15]);
        }
    }
}

/* the panel digit patterns */
const unsigned char fw_update_display_digits[16] = {
    0x7f,
    'o', 'g', 'c', 'C', 'S', '[', '_',  /* "ogcCS[_" */
    0x7f,
    '{', 'y', 'x', '8', '<', '>', '?'  /* "{yx8<>?" */
};
