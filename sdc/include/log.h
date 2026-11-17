/* log.h: declarations of log.c. */
#ifndef LOG_H
#define LOG_H
#include "common.h"

/* functions */
extern void log_add_event(unsigned char, unsigned char);
extern void log_clock_tick(void);
extern void log_clear_records(void);
extern void log_add_record(unsigned char, unsigned char, unsigned char, char);

#endif
