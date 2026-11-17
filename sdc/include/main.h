/* main.h: declarations of main.c, main_tasks.c.
   */
#ifndef MAIN_H
#define MAIN_H
#include "common.h"

/* the boot programs' symbols the application uses */
extern void boot_run_updater(int, char, char);

/* functions */
extern void main_loop(void);
extern void main_poll(void);
extern void main_run_page_loop(void);
extern int main_run_page_loop_step_5(void);
extern void main_run_page_loop_step_6(void);
extern void main_task_maint_mode(void);
extern void main_update_counters(void);
extern void main_task_select_adf_source(void);
extern void main_task_testmode(void);
extern void main_task_wake_requests(void);
extern void main_task_start_updater(void);
extern void main_task_download(void);
extern void main_task_clear_error_display(void);
extern void main_task_restore_display(void);
extern void main_task_retry_mdc_error(void);
extern void main_task_turn_rollers(void);
extern void main_task_multifeed_buttons(void);
extern void main_task_idle_lamp_off(void);
extern void main_task_idle_park(void);
extern void main_clear_error_display(int show_function_screen);
extern void main_task_apply_link_settings(void);
extern void main_task_save_settings(void);
extern void main_task_save_area2(void);
extern void main_restore_display_mode(void);
extern void power_task_power_save(void);
extern void power_leave_save(void);
extern void power_enter_save(int full);
extern void power_task_auto_off(void);
extern void power_task_mdc_restart(void);
extern int power_enter_save_checked(void);
extern void power_task_mdc_retry(void);
extern int power_copy_mdc_log(unsigned char after_timeout);

#endif
