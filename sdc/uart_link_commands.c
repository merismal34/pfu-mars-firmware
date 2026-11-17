#include "fw.h"
#pragma section H
extern int _builtin_get_imask(void);
extern void _builtin_set_imask(int);

void link_cmd_get_status(void)
{
  struct page_record *current_page;
  int saved_imask;

  current_page = (struct page_record *)(page_current.v * 136 + (unsigned long)page_records);
  if (((unsigned short *)scsi_param_buf)[0] != 0x242) {
    link_reply_bad_frame();
    return;
  }
  else if ((hw_cga_board_present.c[0] & 64) && current_page->error_flags.v) {
    link_reply_busy();
    return;
  }
  saved_imask = _builtin_get_imask();
  _builtin_set_imask(15);
  _builtin_set_imask(saved_imask);
  link_reply_2();
}

void link_cmd_get_page_info(void)
{
  struct page_record *current_page;
  int saved_imask;

  current_page = (struct page_record *)(page_current.v * 136 + (unsigned long)page_records);
  if (((unsigned short *)scsi_param_buf)[0] != 0x343) {
    link_reply_bad_frame();
    return;
  }
  else if ((hw_cga_board_present.c[0] & 64) && current_page->error_flags.v) {
    link_reply_busy();
    return;
  }
  saved_imask = _builtin_get_imask();
  _builtin_set_imask(15);
  if (link_no_page_started() == 0) {
    _builtin_set_imask(saved_imask);
    link_reply_not_ready();
    return;
  }
  _builtin_set_imask(saved_imask);
  link_reply_7();
}

void link_cmd_get_capabilities(void)
{
  if (((unsigned short *)scsi_param_buf)[0] != 0x240) {
    link_reply_bad_frame();
    return;
  }
  link_reply_13();
}

/* link_cmd_get_identity */
void link_cmd_get_identity(void)
{
  if (((unsigned short *)scsi_param_buf)[0] != 0x24a) {
    link_reply_bad_frame();
    return;
  }
  link_reply_14();
}

void link_cmd_set_halftone(void)
{
  int saved_imask;
  unsigned char *src;
  unsigned char *table;
  unsigned char *loaded_flags;
  unsigned char bank;

  if (uart_frame_len.v != 0x43) {
    link_reply_bad_frame();
    return;
  }
  saved_imask = _builtin_get_imask();
  _builtin_set_imask(15);
  if (link_no_page_started() == 0) {
    _builtin_set_imask(saved_imask);
    link_reply_not_ready();
    return;
  }
  else {
    link_reply_2();
    return;
  }
  _builtin_set_imask(saved_imask);
  src = scsi_param_byte_3;
  bank = scsi_param_byte_2[0] & 7;
  table = img_download_halftone[bank];
  loaded_flags = img_halftone_downloads;
  link_store_table(src, table, loaded_flags, 64, 8, scsi_param_byte_2[0] & 7);
}

struct cell_u8_param { unsigned char v; };

void link_cmd_set_table_data(void)
{
  int saved_imask;
  int i;
  unsigned char *src;
  unsigned char *table;
  unsigned char *loaded_flags;
  unsigned short len;
  unsigned char mode;
  unsigned char bank;

  if (((unsigned short *)scsi_param_buf)[0] != 0x547) {
    link_reply_bad_frame();
    return;
  }
  saved_imask = _builtin_get_imask();
  _builtin_set_imask(15);
  if (link_no_page_started() == 0) {
    _builtin_set_imask(saved_imask);
    link_reply_not_ready();
    return;
  }
  _builtin_set_imask(saved_imask);
  len = uart_frame_len.v - 5;
  if (--len == (unsigned short)(((struct cell_u8_param *)scsi_param_byte_4)[0].v | ((struct cell_u8_param *)scsi_param_byte_3)[0].v << 8)) {
    link_reply_bad_frame();
    return;
  }
  for (i = 0; i < 34; i++) {
    if (len == link_table_data_lengths[i])
      break;
  }
  mode = scsi_param_byte_5[0] & 0xc0;
  if (i >= 32 && mode != 0xc0) {
    link_reply_bad_frame();
    return;
  }
  i++;
  src = scsi_param_byte_6;
  bank = scsi_param_byte_5[0] & 7;
  if (mode == 0 && i <= 8) {
    table = img_download_halftone[bank];
    loaded_flags = img_halftone_downloads;
  }
  else if (mode == 0x80 && i <= 8) {
    table = img_download_halftone[bank];
    loaded_flags = img_halftone_downloads;
  }
  else if (mode == 0x40 && (i == 16 || i == 32)) {
    table = img_download_gamma[bank];
    loaded_flags = img_gamma_loaded;
  }
  else if (mode == 0xc0 && (i == 33 || i == 34) && (bank == 0 || bank == 1)) {
    table = img_download_lut[bank];
    loaded_flags = img_lut_loaded + bank;
  }
  else {
    link_reply_bad_frame();
    return;
  }
  link_store_table(src, table, loaded_flags, len, i, bank);
}

void link_store_table(unsigned char *src, unsigned char *table, unsigned char *loaded_flags, int len, unsigned char size_index, unsigned char bank)
{
  int i;

  if (len == 0x100 || len == 0x300) {
    for (i = len; i > 0; i--, table += 4, src++) {
      table[0] = src[0];
      table[1] = src[0];
      table[2] = src[0];
      table[3] = src[0];
    }
  }
  else {
    for (i = len; i > 0; i--) {
      *table++ = *src++;
    }
  }
  if (loaded_flags == img_gamma_loaded || loaded_flags == img_lut_loaded) {
    loaded_flags = bank + loaded_flags;
    *loaded_flags = 0xff;
    util_mem_clear(&img_loaded_tables[0].gamma_built_mono, 4);
    util_mem_clear(img_loaded_tables[0].gamma_loaded_mono, 4);
    util_mem_clear(&img_loaded_tables[0].gamma_built_color, 4);
    util_mem_clear(img_loaded_tables[0].gamma_loaded_color, 4);
    util_mem_clear(&img_loaded_tables[1].gamma_built_mono, 4);
    util_mem_clear(img_loaded_tables[1].gamma_loaded_mono, 4);
    util_mem_clear(&img_loaded_tables[1].gamma_built_color, 4);
    util_mem_clear(img_loaded_tables[1].gamma_loaded_color, 4);
  }
  else {
    loaded_flags = bank * 8 + loaded_flags;
    *(int *)loaded_flags = 1;
    loaded_flags[6] = size_index;
    loaded_flags[7] = size_index;
    img_loaded_tables[0].halftone_built = 0;
    img_loaded_tables[0].halftone_loaded = 0;
    img_loaded_tables[1].halftone_built = 0;
    img_loaded_tables[1].halftone_loaded = 0;
  }
  link_reply_2();
}

void link_cmd_get_detected_size(void)
{
  int saved_imask;

  if (((unsigned short *)scsi_param_buf)[0] != 0x245) {
    link_reply_bad_frame();
    return;
  }
  saved_imask = _builtin_get_imask();
  _builtin_set_imask(15);
  if (link_no_page_started() == 0) {
    _builtin_set_imask(saved_imask);
    link_reply_not_ready();
    return;
  }
  _builtin_set_imask(saved_imask);
  link_reply_11();
}

void link_cmd_get_device_status(void)
{
  if (((unsigned short *)scsi_param_buf)[0] != 0x24c) {
    link_reply_bad_frame();
    return;
  }
  link_reply_16();
}

struct link_params_frame {
    union UBF print_flags;
    union UBF style_flags;
    unsigned short reserved_2;
    unsigned short y_offset;
    union UBF counter_flags;
    unsigned char counter_mid;
    unsigned char counter_low;
    unsigned char counter_high;
};


void link_cmd_set_imprinter(void)
{
  struct link_params_frame *params;
  unsigned int length;
  unsigned char counter_step;

  if (!hw_option_flags.f.b0) {
    link_reply_bad_frame();
    return;
  }
  if (scsi_param_buf[0] != 11 && scsi_param_buf[0] != 12) {
    link_reply_bad_frame();
    return;
  }
  params = (struct link_params_frame *)scsi_param_byte_2;
  if (params->reserved_2) {
    link_reply_bad_frame();
    return;
  }
  if (params->style_flags.v & 2) {
    link_reply_bad_frame();
    return;
  }
  counter_step = params->counter_flags.v & 3;
  if (counter_step == 3) {
    link_reply_bad_frame();
    return;
  }
  scsi_imprinter_counter_step.v = counter_step;
  scsi_imprinter_style.v = 0;
  scsi_imprinter_flags.v &= 131;
  if (params->style_flags.f.b5) {
    link_imprinter_flags.b5 = 0;
    if (params->style_flags.f.b4) {
      link_imprinter_flags.b5 = 1;
    }
    link_imprinter_flags.b4 = 1;
  }
  else {
    link_imprinter_flags.b4 = 0;
  }
  if (params->print_flags.f.b7) {
    link_imprinter_flags.b6 = 1;
    if (params->print_flags.f.b6) {
      link_imprinter_flags.b7 = 1;
      scsi_imprinter_print.f.b7 = 1;
    }
    else {
      link_imprinter_flags.b7 = 0;
      scsi_imprinter_print.f.b7 = 0;
    }
  }
  else {
    link_imprinter_flags.b6 = 0;
  }
  if (params->style_flags.v & 1)
    scsi_imprinter_style.v |= 2;
  if (params->style_flags.f.b7) {
    link_imprinter_flags.b2 = 1;
    if (params->style_flags.f.b6)
      scsi_imprinter_flags.f.b1 = 1;
    else
      scsi_imprinter_flags.f.b1 = 0;
  }
  else {
    link_imprinter_flags.b2 = 0;
    scsi_imprinter_flags.f.b1 = 1;
  }
  length = params->y_offset;
  length += 0x294;
  if (length > 0xffff)
    length = 0xffff;
  win_params.imprinter_y_offset = length;
  if (scsi_imprinter_counter_step.v) {
    if (params->counter_flags.f.b7)
      scsi_imprinter_flags.f.b5 = 1;
    if (params->counter_flags.f.b6)
      scsi_imprinter_flags.f.b4 = 1;
  }
  if (scsi_param_buf[0] == 11) {
    scsi_imprinter_counter.v &= 0xff0000;
    scsi_imprinter_counter.v |= (params->counter_mid << 8) + params->counter_low;
  }
  else {
    scsi_imprinter_counter.v = (params->counter_mid << 8) + params->counter_low + (params->counter_high << 16);
  }
  scsi_imprinter_counter_initial = scsi_imprinter_counter.v;
  link_reply_2();
}

void link_cmd_set_imprint_text(void)
{
  int i;
  unsigned char *p;
  int has_counter;
  int counter_8_digits;
  unsigned char text_len;
  unsigned char buf[87];

  has_counter = 0;
  if (!hw_option_flags.f.b0) {
    link_reply_bad_frame();
    return;
  }
  if (scsi_param_buf[0] > 0x53 || scsi_param_byte_2[0]) {
    link_reply_bad_frame();
    return;
  }
  if (scsi_param_buf[0] - 3 == 0) {
    scsi_imprinter_flags.f.b7 = 0;
    scsi_imprinter_text_len.v = 0;
    text_len = 0;
  }
  else {
    i = 0;
    text_len = 0;
    for (; i < scsi_param_buf[0] - 3; i++, text_len++) {
      p = (unsigned char *)scsi_param_buf + i + 3;
      if (*p == 0)
        break;
      if (*p < 32 || *p > 126) {
        link_reply_bad_frame();
        return;
      }
      if (*p == 37) {
        if (p[1] == 37) {
          buf[text_len] = *p;
          i++;
          continue;
        }
        if (util_mem_equal(&p[1], link_counter_format_5, 4)) {
          counter_8_digits = 0;
        }
        else if (util_mem_equal(&p[1], link_counter_format_8, 4)) {
          counter_8_digits = 1;
        }
        else {
          link_reply_bad_frame();
          return;
        }
        has_counter = 1;
        break;
      }
      buf[text_len] = *p;
    }
  }
  buf[text_len] = 0;
  if (!link_imprinter_flags.b6 && has_counter == 0) {
    has_counter = 1;
    counter_8_digits = 1;
  }
  i = text_len;
  if (has_counter) {
    if (counter_8_digits) {
      i += 8;
      if (i > 43) {
        link_reply_bad_frame();
        return;
      }
    }
    else {
      i += 5;
      if (i > 40) {
        link_reply_bad_frame();
        return;
      }
    }
  }
  else {
    if (i > 40) {
      link_reply_bad_frame();
      return;
    }
  }
  scsi_imprinter_print_len = i;
  scsi_imprinter_text_len.v = text_len + 1;
  if (has_counter) {
    scsi_imprinter_flags.f.b7 = 1;
    if (counter_8_digits) {
      scsi_imprinter_flags.f.b0 = 1;
      link_imprinter_flags.b0 = 1;
    }
    else {
      scsi_imprinter_flags.f.b0 = 0;
      link_imprinter_flags.b0 = 0;
    }
  }
  else {
    scsi_imprinter_flags.f.b7 = 0;
  }
  util_mem_copy(scsi_imprinter_text, buf, text_len + 1);
  link_reply_2();
}

void link_cmd_wake(void)
{
  struct page_record *current_page;
  int saved_imask;

  current_page = (struct page_record *)(page_current.v * 136 + (unsigned long)page_records);
  if (((unsigned short *)scsi_param_buf)[0] != 0x370 || scsi_param_byte_2[0] != 0) {
    link_reply_bad_frame();
    return;
  }
  if (hw_cga_board_present.f.link_scan_started && current_page->error_flags.v) {
    link_reply_busy();
    return;
  }
  saved_imask = _builtin_get_imask();
  _builtin_set_imask(15);
  tick_idle_seconds.v = 0;
  if (power_state.f.saving) {
    current_page->flags.f.power_pending = 1;
    power_state.f.leave_request = 1;
    _builtin_set_imask(saved_imask);
    link_clear_reply();
  }
  else {
    _builtin_set_imask(saved_imask);
    link_reply_2();
  }
}

void link_cmd_set_params_x(void)
{
  int saved_imask;
  unsigned char *data;
  unsigned char *dst;
  unsigned char *src;
  int flag;
  short err;
  unsigned char mode;
  unsigned char first_param;
  unsigned char idx;
  unsigned char off;
  unsigned char left;
  unsigned char cnt;

  flag = 1;
  err = 0;
  saved_imask = _builtin_get_imask();
  if (scsi_param_buf[0] < 4) {
    err = 0x40;
    goto end;
  }
  _builtin_set_imask(15);
  if (link_no_page_started() == 0) {
    _builtin_set_imask(saved_imask);
    err = 0x80;
    goto end;
  }
  _builtin_set_imask(saved_imask);
  mode = scsi_param_byte_2[0] & 0xc0;
  if (scsi_param_byte_1[0] == 88) {
    if (mode == 0xc0) {
      err = 8;
      goto end;
    }
  }
  else if (mode & 0x80) {
    err = 8;
    goto end;
  }
  left = cnt = (unsigned char)(scsi_param_buf[0] + 253);
  src = scsi_param_byte_3;
  first_param = scsi_param_byte_2[0] & 63;
  if (scsi_param_byte_1[0] == 88) {
    if (mode == 0x80) {
      if (first_param > 11) {
        err = 8;
        goto end;
      }
      while (flag != 0 && !err) {
        off = (unsigned char)(cnt - left);
        idx = off + first_param;
        err = link_param_x80_checks[idx](&flag, src + off, &left, mode);
      }
    }
    else {
      if (first_param > 25) {
        err = 8;
        goto end;
      }
      if (mode)
        mode = 1;
      while (flag != 0 && !err) {
        off = (unsigned char)(cnt - left);
        idx = off + first_param;
        err = link_param_x_checks[idx](&flag, src + off, &left, mode);
      }
    }
  }
  else {
    if (mode)
      mode = 1;
    if (first_param > 7) {
      err = 8;
      goto end;
    }
    while (flag != 0 && !err) {
      off = (unsigned char)(cnt - left);
      idx = off + first_param;
      err = link_param_y_checks[idx](&flag, src + off, &left, mode);
    }
  }
end:
  _builtin_set_imask(saved_imask);
  if (err != 0) {
    err_set_illegal_request(err);
    link_reply_4();
    return;
  }
  data = scsi_param_byte_3;
  mode = scsi_param_byte_2[0] & 0xc0;
  mode >>= 6;
  if (scsi_param_byte_1[0] == 89)
    mode += 3;
  dst = link_param_dest[mode] + first_param;
  util_mem_copy(dst, data, scsi_param_buf[0] - 3);
  if ((settings_byte_0x33[0] & 1) && (scsi_param_byte_2[0] & 0xc0) == 0) {
    if (scsi_param_byte_1[0] == 88) {
      dst = link_window_back_params + first_param;
      mode = 0;
    }
    else {
      dst = link_side_back_params + first_param;
      mode--;
    }
    util_mem_copy(dst, data, scsi_param_buf[0] - 3);
  }
  link_param_apply_table[mode]();
  if ((settings_byte_0x33[0] & 1) && (scsi_param_byte_2[0] & 0xc0) == 0) {
    if (scsi_param_byte_1[0] == 88)
      link_set_window_back();
    else
      link_apply_side_params_back();
  }
  link_reply_2();
}

void link_set_window_front(void)
{
  link_set_window(0);
}


struct link_tc_bits {
    unsigned char p:1,
                  variance:3,
                  sdtc_given:1,
                  sdtc:1,
                  :2;
};
#define VU(x) (*(union win_binarize_word *)&(x))
#define TC(x) (*(struct link_tc_bits *)&(x))
#define VW(x) (*(union win_white_level_follow_byte *)&(x))

#define VB(x) (*(struct win_composition_bits *)&(x))
#define TB(x) (*(struct Bits *)&(x))

void link_set_window(int side)
{
    struct link_window_params_record *link_window;
    struct win_side *window;
    int saved_imask;
    char flag;
    unsigned char byte;
    unsigned short word;
    unsigned short base;

    link_window = link_window_params_ptrs[side];
    window = &win_params.side[side];
    saved_imask = _builtin_get_imask();
    if (side == 0) {
        if (win_params.paper_size.f.largest_paper && !(settings_window_mode[0] & 0x80) && win_params.flags.f.adf) {
            flag = 0;
        } else {
            flag = 1;
        }
        _builtin_set_imask(15);
        byte = win_params.flags.v & 0x7f;
        if (link_window_params_ptrs[0]->scan_flags.f.source_given) {
            byte |= 0x80;
            byte &= 0xfe;
            if (link_window_params_ptrs[0]->scan_flags.f.adf) {
                byte |= 1;
            }
        }
        win_params.flags.v = byte;
        _builtin_set_imask(saved_imask);
        _builtin_set_imask(15);
        byte = link_window_params_ptrs[0]->scan_flags.c;
        word = win_params.resolution_mode.v & 0x7eff;
        if (link_window_params_ptrs[0]->scan_flags.f.resolution_given) {
            word |= 0x8000;
            byte = link_window_params_ptrs[0]->scan_flags.c & 7;
            word &= 0xfff8;
            win_params.resolution_mode.v = word |= byte;
        } else {
            if (link_window_params_ptrs[0]->scan_flags.f.resolution_code == 3) {
                word |= 0x100;
            }
            base = word & 0xfff8;
            word &= 7;
            if (word <= 1) {
                word = 0;
            } else if (base & 0x100) {
                word = 3;
            } else {
                word = 2;
            }
            win_params.resolution_mode.v = word += base;
        }
        _builtin_set_imask(saved_imask);
        win_params.resolution_mode.f.dpi_given = 0;
        win_params.side[0].size_rounding.f.round_length_up = 0;
        if (link_window_params_ptrs[0]->gamma_flags.f.dpi_given) {
            win_params.resolution_mode.f.dpi_given = 1;
            win_params.side[0].size_rounding.f.round_length_up = 1;
            word = link_window_params_ptrs[0]->x_res_dpi;
            if (word == 0) {
                word = 400;
            }
            win_params.x_res_dpi.v = word;
            word = link_window_params_ptrs[0]->y_res_dpi;
            if (word == 0) {
                word = 400;
            }
            win_params.y_res_dpi.v = word;
        }
    }
    _builtin_set_imask(15);
    byte = window->white_level_follow.v;
    VW(byte).f.given = 0;
    if (link_window->follow_level.f.white_level_follow_given) {
        VW(byte).f.given = 1;
        if (link_window->follow_level.f.white_level_follow_off) {
            VW(byte).f.off = 1;
        } else {
            VW(byte).f.off = 0;
        }
    }
    window->white_level_follow.v = byte;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    word = window->composition.v;
    VB(word).halftone_given = 0;
    if (link_window->image_flags.f.halftone_given) {
        VB(word).halftone_given = 1;
        if (link_window->image_flags.f.halftone_on) {
            VB(word).halftone = 1;
        } else {
            VB(word).halftone = 0;
        }
        VB(word).error_diffusion = 0;
    } else {
    }
    if (link_window->image_flags.f.multi_level) {
        VB(word).multi_level = 1;
        byte = link_side_params_ptrs[0]->color_bits.color_mode;
        if (byte == 2) {
            VB(word).color = 1;
        }
    } else {
        VB(word).multi_level = 0;
        VB(word).color = 0;
    }
    window->composition.v = word;
    byte = (link_window->image_flags.c >> 4 & 7) | (link_window->image_flags.c & 0x80);
    if (!link_window->image_flags.f.halftone_downloaded) {
        byte &= 0x83;
    }
    window->halftone.v &= 0x78ff;
    window->halftone.v |= byte << 8;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    if (side == 0) {
        win_params.paper_size.f.given = 0;
        win_params.paper_size.f.long_page = 0;
        if (link_window_params_ptrs[0]->paper.f.paper_mode & 2) {
            win_params.paper_size.f.given = 1;
            win_params.paper_size.f.custom_size = 0;
            win_params.paper_size.f.largest_paper = 0;
            win_params.paper_size.f.landscape = 0;
            if (link_window_params_ptrs[0]->paper.f.paper_mode & 1) {
                win_params.paper_size.f.custom_size = 1;
                win_params.paper_width_inch1200.v = link_window_params_ptrs[0]->paper_width_inch400 * 3;
                win_params.paper_length_inch1200.v = link_window_params_ptrs[0]->paper_length_inch400 * 3;
            } else {
                win_params.paper_size.f.paper_code = 0;
                win_params.paper_size.v |= link_window_params_ptrs[0]->paper.c;
            }
        } else {
            if (link_window_params_ptrs[0]->paper.f.paper_mode & 1) {
                win_params.paper_size.f.custom_size = 0;
                win_params.paper_size.f.largest_paper = 1;
                if (flag && !win_params.flags.f.adf) {
                    win_params.paper_size.f.paper_code = 0;
                    win_params.paper_size.v |= hw_window_mode_default();
                    win_params.paper_size.f.custom_size = 0;
                    win_params.paper_size.f.largest_paper = 0;
                }
            } else {
                if (win_params.paper_size.f.custom_size || flag) {
                    win_params.paper_size.f.paper_code = 0;
                    win_params.paper_size.v |= hw_window_mode_default();
                }
                win_params.paper_size.f.custom_size = 0;
                win_params.paper_size.f.largest_paper = 0;
            }
        }
        win_params.side[0].use_window_area = 0;
        win_params.paper_size.f.auto_length = 0;
        win_params.paper_size.f.largest_paper = 0;
        if (link_window_params_ptrs[0]->paper.f.use_window_area) {
            win_params.side[0].use_window_area = 1;
        }
        if (!link_window_params_ptrs[0]->scan_flags.f.fixed_length) {
            win_params.paper_size.f.auto_length = 1;
        }
        if (link_window_params_ptrs[0]->paper.f.paper_mode & 1) {
            win_params.paper_size.f.largest_paper = 1;
        }
        if (link_window_params_ptrs[0]->image_flags.f.long_page) {
            win_params.paper_size.f.long_page = 1;
        }
    } else {
        if (link_window_params_ptrs[1]->paper.f.use_window_area) {
            win_params.side[1].use_window_area = 1;
        } else {
            win_params.side[1].use_window_area = 0;
        }
    }
    _builtin_set_imask(saved_imask);
    window->x_offset_inch1200 = link_window->x_offset_inch400 * 3; /* link units x 3: the window's 1/1200 inch */
    window->width_inch1200 = link_window->width_inch400 * 3;
    window->y_offset_inch1200 = link_window->y_offset_inch400 * 3;
    window->length_inch1200 = link_window->length_inch400 * 3;
    if (side == 0) {
        _builtin_set_imask(15);
        byte = win_params.b13.v;
        if (link_window_params_ptrs[0]->scan_flags.f.b7) {
            TB(byte).b0 = 1;
        } else {
            TB(byte).b0 = 0;
        }
        win_params.b13.v = byte;
        _builtin_set_imask(saved_imask);
    }
    byte = (link_window->gamma_flags.c >> 4 & 0x87) | (link_window->gamma_flags.c & 0x80);
    if (!link_window->gamma_flags.f.gamma_downloaded) {
        byte &= 0x83;
    }
    window->gamma_pattern.v &= 0x78;
    window->gamma_pattern.v |= byte;
    if (win_params.side[0].composition.f.color) {
        window->contrast = link_side_params_ptrs[side]->color_contrast;
        window->brightness_negated = link_side_params_ptrs[side]->color_brightness;
    } else {
        window->contrast = link_window->contrast;
        window->brightness_negated = 0;
    }
    link_set_window_mode(side);
}

void link_set_window_mode(int side)
{
    struct link_window_params_record *link_window;
    struct win_side *window;
    int saved_imask;
    char mode_byte;
    char level;
    unsigned short word;
    int w;

    link_window = link_window_params_ptrs[side];
    window = &win_params.side[side];
    saved_imask = _builtin_get_imask();
    mode_byte = link_window->mode_byte;
    if (mode_byte == 0) {
        mode_byte = link_window_mode_ptrs[side]->mode_byte;
    }
    _builtin_set_imask(15);
    word = window->binarize.v;
    if (link_window->follow_level.g.binarize_mode) {
        VU(word).f.given = 1;
        level = 0;
        if (link_window->follow_level.g.binarize_mode & 2) {
            VU(word).f.dtc = 1;
            VU(word).f.sdtc = 0;
            if (window->composition.f.halftone) {
                level = link_get_level(side);
            }
        } else {
            VU(word).f.dtc = 0;
            VU(word).f.sdtc_given = 0;
            if (TC(mode_byte).sdtc_given) {
                VU(word).f.sdtc_given = 1;
                if (TC(mode_byte).sdtc) {
                    VU(word).f.sdtc = 1;
                    if (window->composition.f.halftone) {
                        level = link_get_level(side);
                    } else {
                        VU(word).c.lo = 0;
                    }
                    VU(word).f.variance = TC(mode_byte).variance;
                } else {
                    VU(word).f.sdtc = 0;
                    level = link_get_level(side);
                }
            } else {
                level = link_get_level(side);
            }
        }
        VU(word).c.lo = level;
    } else {
        VU(word).f.given = 0;
        if (TC(mode_byte).sdtc_given) {
            VU(word).f.sdtc_given = 1;
            level = 0;
            if (TC(mode_byte).sdtc) {
                VU(word).f.dtc = 0;
                VU(word).f.sdtc = 1;
                if (!window->composition.f.halftone) {
                    VU(word).c.lo = 0;
                }
                VU(word).v |= TC(mode_byte).variance << 8;
            } else {
                if (!VU(word).f.sdtc && !VU(word).c.lo) {
                    VU(word).c.lo = 0x80;
                }
                VU(word).f.sdtc = 0;
            }
        } else {
            VU(word).f.sdtc_given = 0;
        }
    }
    window->binarize.v = word;
    window->dtc_options = link_window->dtc_options_lo | (link_window->dtc_options_hi << 8);
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    word = window->composition.v;
    if (VB(word).separation_given && VB(word).area_separation_ipc) {
        VB(word).error_diffusion = 0;
        window->composition.v = word;
    }
    _builtin_set_imask(saved_imask);
}

unsigned char link_get_level(int side)
{
    unsigned char level;

    if (link_window_params_ptrs[side]->gamma_flags.f.level_given) {
        level = link_window_params_ptrs[side]->level;
        if (level == 0) {
            level = 0x80;
        }
    } else {
        level = link_level_table[link_window_params_ptrs[side]->follow_level.g.level_code];
    }
    return level;
}

void link_set_window_back(void)
{
    link_set_window(1);
}


void link_apply_global_params(void)
{
    int saved_imask;
    unsigned char value;
    int backing;

    saved_imask = _builtin_get_imask();
    _builtin_set_imask(15);
    if (link_global_params_ptr->side_flags.sides_given) {
        value = link_global_params_ptr->side_flags.side_selection;
        win_sides.v = value;
        win_sides.f.link_given = 1;
    } else {
        win_sides.f.link_given = 0;
    }
    value = link_global_params_ptr->prepick & 6;
    value <<= 5;
    job_set_prepick(value, 0);
    if (link_global_params_ptr->scan_flags.scan_mode == 1 || link_global_params_ptr->scan_flags.scan_mode == 0) {
        link_global_params_ptr->scan_flags.scan_mode = 3;
    } else {
        img_link_scan_mode = link_global_params_ptr->scan_flags.scan_mode;
    }
    if (settings_byte_0x33[0] & 8) {
        link_global_params_ptr->scan_flags.scan_mode = 3;
        img_link_scan_mode = 3;
    }
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    link_global_param_low[0] = link_global_params_ptr->byte_2.param_low;
    link_global_param_high[0] = link_global_params_ptr->byte_2.param_high;
    _builtin_set_imask(saved_imask);
    if (link_global_params_ptr->double_feed_flags.double_feed_given) {
        job_double_feed_flags.b.length_check = 0;
        job_double_feed_flags.b.ultrasonic_check = 0;
        value = link_global_params_ptr->double_feed_flags.double_feed_length;
        if (value & 8) {
            job_double_feed_flags.b.length_check = 1;
            if (value & 3) {
                job_double_feed_flags.c |= (unsigned char)((value & 3) - 1);
            } else {
                job_double_feed_flags.c |= settings_multifeed_check.f.lo;
            }
        } else {
            job_double_feed_flags.b.length_check = 0;
        }
        if (link_global_params_ptr->double_feed_flags.double_feed_thickness) {
            job_double_feed_flags.b.ultrasonic_check = 1;
        } else {
            job_double_feed_flags.b.ultrasonic_check = 0;
        }
        if (link_global_params_ptr->double_feed_flags.double_feed_continue) {
            job_double_feed_flags.b.length_report_only = 1;
            job_double_feed_flags.b.ultrasonic_report_only = 1;
        } else {
            job_double_feed_flags.b.length_report_only = 0;
            job_double_feed_flags.b.ultrasonic_report_only = 0;
        }
    } else {
        if ((*(unsigned char *)&settings_multifeed_check & 3) > 2) {
            job_double_feed_flags.c = (*(unsigned char *)&settings_multifeed_check & 0x78) | 2;
        } else {
        }
    }
    backing = job_black_backing;
    if (link_global_params_ptr->paper_flags.backing_given) {
        value = link_global_params_ptr->paper_flags.backing;
        if (value & 2) {
            backing = 1;
        } else {
            backing = 0;
        }
    }
    if (link_global_params_ptr->paper_flags.page_end_mode == 1) {
        job_page_end_at_pick_sensor = 1;
    } else {
        job_page_end_at_pick_sensor = 0;
    }
    value = link_global_params_ptr->paper_flags.auto_size_mode;
    _builtin_set_imask(15);
    if (value == 3) {
        job_auto_size_flags.b6 = 1;
        backing = 1;
    } else if (value == 2) {
        job_auto_size_flags.b6 = 0;
    }
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    job_auto_size_flags.b7 = 0;
    _builtin_set_imask(saved_imask);
    if (backing != job_black_backing) {
        job_adf_sheet_count = 0;
        job_black_backing = backing;
    }
    win_params.side[0].compression.f.type = 0;
    win_params.side[0].compression.f.type = 0;
    win_params.side[1].compression.f.argument = 0;
    win_params.side[1].compression.f.argument = 0;
    value = link_global_params_ptr->reserved_10;
}

int link_param_x_check_0(int *more, struct Bits *param, unsigned char *left, int mode)
{
    struct page_record *current_page;
    short error;
    current_page = &page_records[page_current.v];
    error = 0;
    if (param->b4) {
        settings_byte_0x9a[0] &= 0xfe;
        win_params.flags.f.adf = 1;
    } else {
        settings_byte_0x9a[0] |= 1;
        win_params.flags.f.adf = 0;
    }
    if (mode == 0) {
        if (current_page->flags.f.sheet_fed && param->b5) {
            if (win_params.flags.f.adf) {
                if (!param->b4) error |= 0x80;
            } else {
                if (param->b4) error |= 0x80;
            }
        }
        if (param->b3 && (*(unsigned char *)param & 15) == 14) error |= 8;
    }
    if (error) {
        return error;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_1(int *more, struct link_param_bits_3_2_3 *param, unsigned char *left, int mode)
{
    short error;
    error = 0;
    if (!(((char *)&hw_option_flags)[0] & 2) && (param->a & 2)) {
        error |= 0x10;
    }
    if (error) {
        return error;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_2(int *more, struct link_param_bits_1_3_4 *param, unsigned char *left, int mode)
{
    unsigned char value;
    if (param->t) {
        value = param->a;
        if (value > 1) {
            return 8;
        }
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_3(int *more, int param, unsigned char *left, int mode)
{
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_4(int *more, unsigned char *param, unsigned char *left, int mode)
{
    unsigned char bits;
    if (((char *)&hw_option_flags)[0] & 2) {
        bits = *param & 0x1e;
        if (bits & 0x10) {
            *param |= 0x1e;
        }
        if (bits & 8) {
            *param |= 0xe;
        }
        if (bits & 4) {
            *param |= 6;
        }
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_5(int *more, struct link_param_bits_2_6 *param, unsigned char *left, int mode)
{
    if (mode == 0 && (param->a & 2) && !(param->a & 1) && link_param_x_check_5_table[*(unsigned char *)param & 0x1f]) {
        return 0x20;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_6(int *more, unsigned char *param, unsigned char *left, int mode)
{
    unsigned short value;
    if (mode == 0) {
        if (*left < 2) {
            return 8;
        }
        if ((value = (param[0] << 8) | param[1]) == 0 || value > 0x1300) {
            return 0x20;
        }
    }
    *left -= 2;
    if (*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_8(int *more, unsigned char *param, unsigned char *left, int mode)
{
    unsigned short value;
    if (mode == 0) {
        if (*left < 2) {
            return 8;
        }
        if ((value = (param[0] << 8) | param[1]) == 0 || value > 0x1b00) {
            return 0x20;
        }
    }
    *left -= 2;
    if (*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_10(int *more, unsigned char *param, unsigned char *left, int mode)
{
    unsigned short offset;
    unsigned short size;
    if (*left < 4) {
        return 8;
    }
    size = (param[2] << 8) | param[3];
    if (size == 0 || (size & 0x8000)) {
        return 0x20;
    }
    size += 7;
    if (size & 0x8000) {
        return 0x20;
    }
    offset = (param[0] << 8) | param[1];
    if (offset & 0x8000) {
        return 0x20;
    }
    size &= 0xfff8;
    if (offset + size > 0x1300) {
        return 0x20;
    }
    *left -= 4;
    if (*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_14(int *more, unsigned char *param, unsigned char *left, int mode)
{
    unsigned short offset;
    unsigned short size;
    if (*left < 4) {
        return 8;
    }
    size = (param[2] << 8) | param[3];
    if (size == 0 || (size & 0x8000)) {
        return 0x20;
    }
    offset = (param[0] << 8) | param[1];
    if (offset & 0x8000) {
        return 0x20;
    }
    if (offset + size > 0x1b00) {
        return 0x20;
    }
    *left -= 4;
    if (*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_18(int *more, int param, unsigned char *left, int mode)
{
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_19(int *more, struct link_param_bits_1_3_4 *param, unsigned char *left, int mode)
{
    unsigned char value;
    value = param->a;
    if (param->t) {
        if (value > 1) {
            return 8;
        }
    } else {
        if (value) {
            return 8;
        }
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_20(int *more, int param, unsigned char *left, int mode)
{
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x_check_21(int *more, int param, unsigned char *left, int mode)
{
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}
