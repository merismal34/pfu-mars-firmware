#include "boot.h"
#pragma section 2
struct cell_u16 { int v; };
extern void _builtin_set_imask(int);
extern int _builtin_get_imask(void);

int fw_update_queue_record(struct fw_update_command_link *record)
{
    int i;
    int queued;
    int saved_imask;

    queued = 0;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    for (i = 0; i < 2; i++) {
        if (fw_update_record_queue[i].v != 0) {
            continue;
        }
        fw_update_record_queue[i].v = (int)record;
        queued = 1;
        break;
    }
    _builtin_set_imask(saved_imask);
    return queued;
}

void fw_update_unqueue_record(struct fw_update_command_link *record)
{
    int i;
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    for (i = 0; i < 2; i++) {
        if (fw_update_record_queue[i].v != (int)record) {
            continue;
        }
        for (; i < 1; i++) {
            fw_update_record_queue[i].v = fw_update_queue_next[i].v;
        }
        fw_update_record_queue[i].v = 0;
        break;
    }
    _builtin_set_imask(saved_imask);
}

struct fw_update_command_record *fw_update_alloc_record(void)
{
    int i;
    struct fw_update_command_record *record;
    int saved_imask;

    record = 0;
    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    for (i = 0; i < 2; i++) {
        if (fw_update_record_in_use[i].c != 0) {
            continue;
        }
        record = &fw_update_records[i];
        record->in_use = 0x80;
        break;
    }
    _builtin_set_imask(saved_imask);
    return record;
}

void fw_update_free_record(struct fw_update_command_record *record)
{
    int x;
    int saved_imask;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    fw_update_unqueue_record((struct fw_update_command_link *)record);
    record->in_use = 0;
    fw_update_mem_clear_fast(record, 68);
    _builtin_set_imask(saved_imask);
}

void fw_update_set_current_record(struct fw_update_command_link *record)
{
    fw_update_record_active.b7 = 1;
    fw_update_current_record.p = record;
}

void fw_update_release_record(struct fw_update_command_record *record)
{
    fw_update_free_record(record);
    fw_update_record_active.b7 = 0;
    fw_update_current_record.p = 0;
}

void fw_update_clear_current_record(struct fw_update_command_link *record)
{
    fw_update_record_active.b7 = 0;
    fw_update_current_record.p = 0;
}
