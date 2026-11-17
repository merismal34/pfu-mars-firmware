#include "fw.h"
#pragma section H

void calib_task_recalibrate(void)
{
  unsigned char error;

  if ((job_calib_valid & 0x800) || (job_calib_valid & 4)) {
    if (win_params.flags.f.adf || !(job_calib_valid & 0x8000) || !(mdc_mailbox.lamp_status & MDC_LAMP_STATUS_FLATBED)
        || (settings_test_flags[0] & 0x10) || (settings_test_flags[0] & 2) || !(settings_byte_0x7f[0] & 1)) {
      _builtin_set_imask(15);
      job_calib_valid = 0;
      _builtin_set_imask(0);
      return;
    }
  } else {
    return;
  }
  if (!mdc_mailbox.sensor_status.f.fb_home) {
    return;
  }
  if (job_calib_valid & 4) {
    do {
      if (job_aborted && scsi_resume_flags.f.resume_pending) {
        return;
      }
    } while (tick_flatbed_home_ticks < 50);
    _builtin_set_imask(15);
    job_calib_valid &= 0xfffb;
    tick_calib_age_ticks = 0;
    _builtin_set_imask(0);
  }
  afe_write_config(0);
  afe_set_test_mode(0);
  _builtin_set_imask(15);
  line_side_state[0].state_flags.v = 0;
  _builtin_set_imask(0);
  afe_set_gain(0, 0, 0, 0x80);
  afe_set_gain(0, 0, 1, 0x80);
  afe_set_gain(0, 1, 0, 0x80);
  afe_set_gain(0, 1, 1, 0x80);
  afe_set_gain(0, 2, 0, 0x80);
  afe_set_gain(0, 2, 1, 0x80);
  calib_offset_target_front.v = 15;
  calib_offset_target_2_front = 50;
  calib_offset_target_3_front = 0;
  calib_gain_target_front = 0xa0;
  calib_gain_target_2_front = 100;
  error = calib_start_offset(1);
  if (error != 0) {
    _builtin_set_imask(15);
    job_calib_valid = 0;
    _builtin_set_imask(0);
    return;
  }
  error = calib_start_gain(1);
  if (error != 0) {
    _builtin_set_imask(15);
    job_calib_valid = 0;
    _builtin_set_imask(0);
    return;
  }
  if (job_aborted && scsi_resume_flags.f.resume_pending) {
    if (page_size_wanted == 0 && page_size_ready == 0 && page_size_reply_pending == 0) {
      isr_request_command_layer(0);
    }
  }
  calib_start_apply(1);
  calib_start_black();
  dirt_run_check();
  calib_load_white_level();
  _builtin_set_imask(15);
  job_calib_valid &= 0xf7ff;
  _builtin_set_imask(0);
}
