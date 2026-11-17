#include "fw.h"
#pragma section START
extern void _builtin_set_vbr(void **);


void start_entry(char sync_mode, char ultra)
{
    start_init(sync_mode, ultra);
}

void start_reset(void)
{
    start_init(0, 0);
}

void start_init(char sync_mode, unsigned char ultra)
{
    _builtin_set_vbr(hw_vbr_value);
    hw_init_bus();
    hw_set_vbr();
    hw_init_ram_sections();
    hw_init_ram_set3();
    if (sync_mode == 0 && ultra == 1) {
        sync_mode = 0;
        ultra = 0;
        hw_after_update = 1;
    }
    hw_restart_scsi_syncmode.v = sync_mode;
    hw_restart_scsi_ultra.v = ultra;
    if (hw_restart_scsi_syncmode.v || hw_restart_scsi_ultra.v)
        hw_after_update = 1;
    hw_init();
    hw_startup_sequence();
    main_loop();
}
