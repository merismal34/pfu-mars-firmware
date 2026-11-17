#include "boot.h"

void boot_run_updater(int scsi, char sync_mode, char ultra)
{
    boot_main(1, scsi, sync_mode, ultra);
}

void boot_reenter(char sync_mode, char ultra)
{
    boot_main(0, 1, sync_mode, ultra);
}

void boot_reset(void)
{
    boot_init_bus_ports();
    boot_main(0, 1, 0, 0);
}
