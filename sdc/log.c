#include "fw.h"


void log_add_event(unsigned char type, unsigned char code)
{
    struct log_event *event;
    char last_code;
    event = &log_events[log_event_index & 63];
    event->count = 0;
    if (type & 2) {
        if (code == 0)
            return;
        last_code = log_events[(log_event_index - 1) & 63].code;
        if (code == 40 || code == 194 || code == 18) {
            if (last_code == (char)code) {
                event = &log_events[(log_event_index - 1) & 63];
                log_event_index--;
            }
            event->count++;
        }
    }
    event->seconds = log_seconds;
    event->type = type;
    event->code = code;
    event->z = 0;
    log_event_index++;
}

/* Called once a second: advances the software calendar log_clock_sec (seconds) ..
   log_clock_year (year), with month lengths and February 29 in years divisible by 4. */
void log_clock_tick(void)
{
    log_clock_sec.v++;
    if (log_clock_sec.v >= 60) {
        log_clock_sec.v = 0;
        log_clock_min.v++;
        if (log_record_minutes != 255)
            log_record_minutes++;
        if (log_clock_min.v >= 60) {
            log_clock_min.v = 0;
            log_clock_hour.v++;
            if (log_clock_hour.v >= 24) {
                log_clock_hour.v = 0;
                log_clock_day.v++;
                if (log_clock_day.v >= 29 && log_clock_month.v == 2) {
                    if (!(log_clock_year % 4) && log_clock_day.v == 29) {
                    } else {
                        log_clock_day.v = 1;
                        log_clock_month.v++;
                    }
                }
                if (log_clock_day.v >= 31) if ((log_clock_month.v == 4 || log_clock_month.v == 6 || log_clock_month.v == 9 || log_clock_month.v == 11)) {
                    log_clock_day.v = 1;
                    log_clock_month.v++;
                }
                if (log_clock_day.v >= 32) {
                    log_clock_day.v = 1;
                    log_clock_month.v++;
                }
                if (log_clock_month.v >= 13) {
                    log_clock_month.v = 1;
                    log_clock_year++;
                }
            }
        }
    }
}

void log_clear_records(void)
{
    util_mem_clear(log_records, 1020);
    log_record_index.v = 0;
    log_record_last.v = 0xffff;
    log_record_wrap_index = 0;
    log_flags.b1 = 0;
    log_flags.b0 = 1;
}


void log_add_record(unsigned char type, unsigned char code, unsigned char detail, char mark_wrap)
{
    unsigned char *entry;
    int pad;
    if (mark_wrap) if (!log_flags.b1) {
        log_records[log_record_index.v].minutes = 0xaa;
        log_records[log_record_index.v].sec = 0xaa;
        if (++log_record_index.v >= 0xaa)
            log_record_index.v--;
        log_record_wrap_index = log_record_index.v;
        log_flags.b1 = 1;
    }
    entry = (unsigned char *)&log_records[log_record_index.v];
    entry[4] = 0;
    if ((type & 2) != 0) if ((code == 40 || code == 0xf1)) {
        if (log_record_last.v != 0xffff && code == log_records[log_record_last.v].code)
            if (((code == 40 && (detail & 0xfe) == (log_records[log_record_last.v].detail & 0xfe))
                || (code == 0xf1 && detail == log_records[log_record_last.v].detail))) {
            entry = (unsigned char *)&log_records[log_record_last.v];
            log_record_index.v = log_record_last.v;
        }
        entry[4]++;
    }
    if (log_flags.b0 != 0) {
        log_start_day = log_clock_day.v;
        log_start_hour = log_clock_hour.v;
        *entry = log_clock_min.v;
        log_record_minutes = 0;
    } else *entry = log_record_minutes;
    entry[1] = log_clock_sec.v;
    entry[2] = type;
    entry[3] = code;
    entry[5] = detail;
    log_record_last.v = log_record_index.v;
    if (++log_record_index.v >= 0xaa) {
        if (log_flags.b1)
            log_record_index.v = log_record_wrap_index;
        else
            log_record_index.v = 1;
    }
    log_flags.b0 = 0;
    log_records[log_record_index.v].minutes = 0xff;
    log_records[log_record_index.v].sec = 0xff;
}
