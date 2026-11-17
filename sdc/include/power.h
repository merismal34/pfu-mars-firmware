/* power.h: declarations of power.c. */
#ifndef POWER_H
#define POWER_H
#include "common.h"

/* functions */
extern void power_restart_sensor_path(int skip_unit_power);
extern void power_off(void);
extern void power_check_button(void);
extern void power_check_pf1_sequence(void);
extern void power_dump_state(unsigned char reason);

#endif
