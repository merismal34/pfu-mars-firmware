/* tick.h: declarations of tick.c. */
#ifndef TICK_H
#define TICK_H
#include "common.h"

struct cell_tick_heater_ok_init { int v[2]; };

/* functions */
extern void tick_handler(void);
extern void tick_count_mdc_sensor_clear(void);
extern void tick_run_warmup_schedule(void);
extern void tick_run_heater_monitor(void);
extern void tick_check_page_idle(void);
extern void tick_hook_4th(void);
extern void tick_handler_step_6(void);
extern void tick_hook_odd(void);
extern int tick_is_flatbed_lamp_on(void);
extern int tick_is_adf_front_lamp_on(void);
extern int tick_is_adf_back_lamp_on(void);
extern int tick_both_adf_lamps_on(void);
extern int tick_both_adf_lamps_off(void);

/* constants */
extern const struct cell_tick_heater_ok_init tick_heater_ok_init;

/* variables */
extern int tick_saved_imask;

#endif
