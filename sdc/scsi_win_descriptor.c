#include "fw.h"
#pragma section H

void scsi_win_save_descriptor(void)
{
    unsigned short *dst;
    unsigned short *src;
    short word;
    short j;
    src = (unsigned short *)scsi_param_buf;
    if (scsi_win_descriptor_id & 0x80)
        dst = scsi_win_saved_back_descriptor;
    else
        dst = scsi_win_saved_descriptors;
    word = *src;
    word |= 0x80;
    *dst = word;
    src++;
    dst++;
    *dst = *src;
    src++;
    dst++;
    *dst = *src;
    src++;
    dst++;
    word = *src;
    *dst = word;
    src++;
    dst++;
    if ((unsigned short)word > 0xf8)
        return;
    src = scsi_win_current_descriptor;
    word++;
    word = (unsigned short)word >> 1;
    for (j = 0; j < (unsigned)word; j++) {
        *dst = *src;
        src++;
        dst++;
    }
}


/* Parses one 64-byte window descriptor at address p; a bad field returns scsi_mark_invalid_field(p + offset),
   which records the offset as the sense field pointer. */
unsigned int scsi_win_parse_descriptor(unsigned int p)
{
    struct page_record *q;
    unsigned int side;
    struct win_side *window;
    unsigned int field_value;
    unsigned int length_inch1200;
    unsigned short field_word;
    unsigned short dpi;
    unsigned int field_end;
    unsigned char field_byte;
    unsigned char expected;
    unsigned char index;
    unsigned short width_px;
    unsigned short length_lines;

    q = &page_records[page_current.v];
    win_params.side[0].size_rounding.v = scsi_win_size_rounding;
    side = *(unsigned char *)p;
    scsi_win_descriptor_id = side;
    if (side == SCSI_WIN_ID_BACK || side == SCSI_WIN_ID_FRONT) {
    } else {
        { scsi_mark_invalid_field(p); return; }
    }
    if (side == 0)
        side = 0;
    else
        side = 1;
    window = &win_params.side[side];
    if (*(unsigned char *)(p + 1)) {
        p += 1;
        { scsi_mark_invalid_field(p); return; }
    }
    field_word = util_get_be16((unsigned char *)(p + 2));
    dpi = field_word;
    if (side == 1 && scsi_win_saved_descriptors[0] == SCSI_WIN_ID_BACK)
        dpi = win_params.x_res_dpi.v;
    scsi_win_check_resolution(field_word, dpi, side);
    win_params.x_res_dpi.v = scsi_win_res_dpi;
    if (scsi_win_res_valid == 0) {
        p += 2;
        { scsi_mark_invalid_field(p); return; }
    }
    field_word = util_get_be16((unsigned char *)(p + 4));
    if (side == 1 && scsi_win_saved_descriptors[0] == SCSI_WIN_ID_BACK)
        dpi = win_params.y_res_dpi.v;
    scsi_win_check_resolution(field_word, dpi, side);
    win_params.y_res_dpi.v = scsi_win_res_dpi;
    if (scsi_win_res_valid == 0) {
        p += 4;
        { scsi_mark_invalid_field(p); return; }
    }
    if (*(unsigned char *)(p + 25) == SCSI_WIN_COMPOSITION_COLOR)
        win_params.side[0].composition.f.color = 1;
    else
        win_params.side[0].composition.f.color = 0;
    field_value = util_get_be32((unsigned char *)(p + 6));
    field_word = win_params.x_res_dpi.v;
    scsi_win_units_to_inch1200(field_value, field_word);
    if (scsi_win_inch1200_valid == 0) {
        p += 6;
        { scsi_mark_invalid_field(p); return; }
    }
    window->x_offset_inch1200 = scsi_win_inch1200.c.lo;
    if ((settings_test_flags[0] & 0x80) || (settings_emulation[0] & 0x1c) == 0x14) {
        if (scsi_win_inch1200.v > 15000) {
            p += 6;
            { scsi_mark_invalid_field(p); return; }
        }
    } else {
        if (scsi_win_inch1200.v > 0x3900) {
            p += 6;
            { scsi_mark_invalid_field(p); return; }
        }
    }
    field_value = util_get_be32((unsigned char *)(p + 10));
    field_word = win_params.y_res_dpi.v;
    scsi_win_units_to_inch1200(field_value, field_word);
    if (scsi_win_inch1200_valid == 0) {
        p += 10;
        { scsi_mark_invalid_field(p); return; }
    }
    window->y_offset_inch1200 = scsi_win_inch1200.v;
    if (scsi_win_inch1200.v > 0x5fcd0) {
        p += 10;
        { scsi_mark_invalid_field(p); return; }
    }
    field_value = util_get_be32((unsigned char *)(p + 14));
    field_word = win_params.x_res_dpi.v;
    scsi_win_units_to_inch1200(field_value, field_word);
    if (scsi_win_inch1200_valid == 0) {
        p += 14;
        { scsi_mark_invalid_field(p); return; }
    }
    field_word = scsi_win_inch1200.c.lo;
    if ((settings_test_flags[0] & 0x80) || (settings_emulation[0] & 0x1c) == 0x14) {
        if (scsi_win_inch1200.v > 15000) {
            p += 6;
            { scsi_mark_invalid_field(p); return; }
        }
    } else {
        if (scsi_win_inch1200.v > 0x3900) {
            p += 6;
            { scsi_mark_invalid_field(p); return; }
        }
    }
    window->width_inch1200 = field_word;
    while (1) {
        field_word = window->width_inch1200;
        if (field_word == 0) {
            p += 14;
            { scsi_mark_invalid_field(p); return; }
        }
        field_word += window->x_offset_inch1200;
        if ((settings_test_flags[0] & 0x80) || (settings_emulation[0] & 0x1c) == 0x14) {
            if (field_word > 15000) {
                p += 14;
                { scsi_mark_invalid_field(p); return; }
            }
        } else {
            if (field_word > 0x3900) {
                p += 14;
                { scsi_mark_invalid_field(p); return; }
            }
        }
        field_word = window->width_inch1200;
        dpi = win_params.x_res_dpi.v;
        scsi_win_inch1200_to_px16(field_word, dpi);
        if (scsi_win_px16_valid == 0) {
            p += 14;
            { scsi_mark_invalid_field(p); return; }
        }
        width_px = scsi_win_px16.c.lo;
        if (settings_test_flags[0] & 0x80) {
            if (width_px > 10000) {
                p += 14;
                { scsi_mark_invalid_field(p); return; }
            }
        } else {
            if (width_px > 0x2600) {
                p += 14;
                { scsi_mark_invalid_field(p); return; }
            }
        }
        field_word = 2;
        dpi = width_px;
        if (*(unsigned char *)(p + 25) < 2) {
            field_word = 9;
            if (*(unsigned char *)(p + 32)) {
                if (width_px & 15) {
                    if ((unsigned short)(window->x_offset_inch1200 + window->width_inch1200) >= 0x3714) {
                    } else
                        width_px += 15;
                    width_px &= 0xfff0;
                    field_value = width_px * 1200;
                    window->width_inch1200 = rt_divu16(win_params.x_res_dpi.v - 1 + field_value, win_params.x_res_dpi.v);
                    continue;
                }
            } else {
                width_px += 7;
                width_px &= 0xfff8;
            }
            dpi = width_px;
            break;
        } else if (*(unsigned char *)(p + 32) == SCSI_WIN_COMPRESSION_JPEG1) {
            if (width_px & 7) {
                if (width_px < 400)
                    width_px += 8;
                width_px &= 0xfff8;
                field_value = width_px * 1200;
                window->width_inch1200 = rt_divu16(win_params.x_res_dpi.v - 1 + field_value, win_params.x_res_dpi.v);
                continue;
            }
        } else if (*(unsigned char *)(p + 32) == SCSI_WIN_COMPRESSION_JPEG2) {
            if (width_px & 15) {
                if (width_px < 400)
                    width_px += 16;
                width_px &= 0xfff0;
                field_value = width_px * 1200;
                window->width_inch1200 = rt_divu16(win_params.x_res_dpi.v - 1 + field_value, win_params.x_res_dpi.v);
                continue;
            }
        } else if (*(unsigned char *)(p + 25) == SCSI_WIN_COMPOSITION_COLOR) {
            if (width_px & 1) {
                if (width_px < 400)
                    width_px += 2;
                else
                    width_px--;
                width_px &= 0xfffe;
                field_value = width_px * 1200;
                window->width_inch1200 = rt_divu16(win_params.x_res_dpi.v - 1 + field_value, win_params.x_res_dpi.v);
                continue;
            }
        }
        break;
    }
    if (dpi < field_word) {
        p += 14;
        { scsi_mark_invalid_field(p); return; }
    }
    field_value = util_get_be32((unsigned char *)(p + 18));
    if (field_value + util_get_be32((unsigned char *)(p + 10)) > 0x5fcd0 || (unsigned int)rt_divu32(field_value * win_params.y_res_dpi.v, 1200) > 0xffff) {
        p += 18;
        { scsi_mark_invalid_field(p); return; }
    }
    field_word = win_params.y_res_dpi.v;
    if ((settings_emulation[0] & 0x1c) != 0x1c && (settings_emulation[0] & 0x1c) != 0x18) {
        if (field_word > 400 && field_value > 0x5910)
            field_value = 0x5910;
    }
    scsi_win_units_to_inch1200(field_value, field_word);
    if (scsi_win_inch1200_valid == 0) {
        p += 18;
        { scsi_mark_invalid_field(p); return; }
    }
    length_inch1200 = scsi_win_inch1200.v;
    window->length_inch1200 = length_inch1200;
    while (1) {
        length_inch1200 = window->length_inch1200;
        if (length_inch1200 == 0) {
            p += 18;
            { scsi_mark_invalid_field(p); return; }
        }
        length_inch1200 += window->y_offset_inch1200;
        if (side == 0) {
            win_long_length = 0;
            win_extra_long_length = 0;
        } else if (scsi_win_saved_descriptors[0] != SCSI_WIN_ID_BACK) {
            win_long_length = 0;
            win_extra_long_length = 0;
        }
        if (length_inch1200 > 0x60c0) { /* longer than 20.6 in */
            field_byte = *(unsigned char *)(p + 25);
            win_long_length = 1;
            dpi = 400;
            scsi_win_inch1200_to_px16(length_inch1200, dpi);
            scsi_win_long_length_lines.v = rt_divu16(scsi_win_px16.v * 120 + 99, 100);
            if (length_inch1200 > 0xac08) { /* longer than 36.7 in: 300 dpi at most */
                if (win_params.y_res_dpi.v < 201)
                    field_value = 240000;
                else if (win_params.y_res_dpi.v < 301)
                    field_value = 108000;
                else {
                    p += 18;
                    { scsi_mark_invalid_field(p); return; }
                }
                if (length_inch1200 > field_value) {
                    p += 18;
                    { scsi_mark_invalid_field(p); return; }
                }
                win_extra_long_length = 1;
            }
        }
        if (length_inch1200 > 0x5fcd0) {
            p += 18;
            { scsi_mark_invalid_field(p); return; }
        }
        length_inch1200 = window->length_inch1200;
        dpi = win_params.y_res_dpi.v;
        scsi_win_inch1200_to_px16(length_inch1200, dpi);
        if (scsi_win_px16_valid == 0) {
            p += 18;
            { scsi_mark_invalid_field(p); return; }
        }
        length_lines = scsi_win_px16.c.lo;
        if (length_lines > 0xffff || length_lines == 0) {
            p += 18;
            { scsi_mark_invalid_field(p); return; }
        }
        if (*(unsigned char *)(p + 25) >= 2) {
            if ((*(unsigned char *)(p + 32) == SCSI_WIN_COMPRESSION_JPEG1 || *(unsigned char *)(p + 32) == SCSI_WIN_COMPRESSION_JPEG2) && (length_lines & 7)) {
                if (length_lines < 400)
                    length_lines += 8;
                length_lines &= 0xfff8;
                field_value = length_lines * 1200;
                window->length_inch1200 = rt_divu32(win_params.y_res_dpi.v - 1 + field_value, win_params.y_res_dpi.v);
                continue;
            }
            break;
        }
        break;
    }
    field_word = width_px;
    field_byte = *(unsigned char *)(p + 25);
    if (field_byte < 2)
        field_word = field_word >> 3;
    dpi = length_lines;
    field_value = field_word * dpi;
    if (field_byte < 2) {
        field_word = width_px;
        field_word >>= 3;
        if (field_word & 1) {
        }
    }
    field_byte = *(unsigned char *)(p + 22);
    field_byte = ~field_byte + 1;
    if (field_byte == 0)
        field_byte = 0x80;
    window->brightness_negated = field_byte;
    field_byte = ~field_byte + 1;
    window->halftone.c.lo = field_byte;
    window->binarize.v = 0;
    field_byte = *(unsigned char *)(p + 23);
    if (field_byte == 0)
        field_byte = 0x80;
    window->binarize.c.lo = field_byte;
    window->binarize.f.given = 1;
    window->binarize.f.sdtc_given = 1;
    field_byte = *(unsigned char *)(p + 24);
    window->contrast = field_byte;
    window->composition.v = 0;
    field_byte = *(unsigned char *)(p + 25);
    if (field_byte != SCSI_WIN_COMPOSITION_LINE_ART && field_byte != SCSI_WIN_COMPOSITION_HALFTONE && field_byte != SCSI_WIN_COMPOSITION_GRAY && field_byte != SCSI_WIN_COMPOSITION_COLOR) {
        p += 25;
        { scsi_mark_invalid_field(p); return; }
    }
    if (field_byte >= SCSI_WIN_COMPOSITION_GRAY) {
        expected = 8;
        window->composition.f.multi_level = 1;
        if (field_byte == SCSI_WIN_COMPOSITION_COLOR)
            window->composition.f.color = 1;
        if (win_params.x_res_dpi.v > 600 || win_params.y_res_dpi.v > 600) {
            p += 25;
            { scsi_mark_invalid_field(p); return; }
        }
        if (scsi_win_saved_descriptors[0] == SCSI_WIN_ID_BACK)
            if (side == 1)
                if (!win_params.side[0].composition.f.multi_level) {
                    p += 25;
                    { scsi_mark_invalid_field(p); return; }
                }
    } else {
        expected = 1;
        window->composition.f.multi_level = 0;
        if (scsi_win_saved_descriptors[0] == SCSI_WIN_ID_BACK && side == 1) {
            if (win_params.side[0].composition.f.multi_level) {
                p += 25;
                { scsi_mark_invalid_field(p); return; }
            }
        }
    }
    window->white_level_follow.v = 0;
    window->white_level_follow.f.given = 1;
    if (field_byte != SCSI_WIN_COMPOSITION_LINE_ART) {
        window->white_level_follow.f.off = 1;
    } else {
        if (*(unsigned char *)(p + 62) == SCSI_WIN_IPC_DTC)
            window->white_level_follow.f.off = 0;
    }
    window->composition.f.halftone_given = 1;
    window->composition.f.separation_given = 1;
    window->composition.f.b13 = 1;
    if (field_byte == SCSI_WIN_COMPOSITION_HALFTONE)
        window->composition.f.halftone = 1;
    else
        window->composition.f.halftone = 0;
    field_byte = *(unsigned char *)(p + 26);
    if (field_byte != expected) {
        p += 26;
        { scsi_mark_invalid_field(p); return; }
    }
    field_word = 0;
    if (window->halftone.f.b14)
        ((struct Bits *)&field_word)->b6 = 1;
    window->halftone.c.hi = 0;
    field_byte = *(unsigned char *)(p + 28);
    if (field_byte & SCSI_WIN_HALFTONE_DOWNLOADED) {
        window->halftone.f.downloaded = 1;
        if ((field_byte & 0x7f) > 7) {
            p += 27;
            { scsi_mark_invalid_field(p); return; }
        }
    }
    window->halftone.f.pattern = field_byte & 7;
    field_byte = *(unsigned char *)(p + 27);
    if (field_byte > SCSI_WIN_HALFTONE_ERROR_DIFFUSION) {
        p += 27;
        { scsi_mark_invalid_field(p); return; }
    } else if (field_byte == SCSI_WIN_HALFTONE_ERROR_DIFFUSION)
        window->composition.f.error_diffusion = 1;
    else {
        window->composition.f.error_diffusion = 0;
        if (!window->halftone.f.downloaded)
            if (window->halftone.f.pattern > 3) {
                p += 27;
                { scsi_mark_invalid_field(p); return; }
            }
    }
    window->halftone.c.hi |= field_word;
    field_byte = *(unsigned char *)(p + 29);
    window->reverse_image.f.given = 1;
    if (field_byte & 0x80)
        window->reverse_image.f.on = 1;
    else
        window->reverse_image.f.on = 0;
    if (field_byte & 0x7f) {
        p += 29;
        { scsi_mark_invalid_field(p); return; }
    }
    field_word = util_get_be16((unsigned char *)(p + 30));
    if (field_word != 0) {
        p += 30;
        { scsi_mark_invalid_field(p); return; }
    }
    field_byte = *(unsigned char *)(p + 32);
    expected = *(unsigned char *)(p + 33);
    window->compression.f.type = field_byte;
    window->compression.f.argument = expected;
    if (field_byte != SCSI_WIN_COMPRESSION_NONE) {
        if (field_byte <= SCSI_WIN_COMPRESSION_MMR) {
            if (*(unsigned char *)(p + 25) >= 2) {
                p += 32;
                { scsi_mark_invalid_field(p); return; }
            }
        } else if (field_byte == SCSI_WIN_COMPRESSION_JPEG1 || field_byte == SCSI_WIN_COMPRESSION_JPEG2) {
            if (*(unsigned char *)(p + 25) < 2) {
                p += 32;
                { scsi_mark_invalid_field(p); return; }
            }
        } else {
            p += 32;
            { scsi_mark_invalid_field(p); return; }
        }
    }
    for (index = 0; index < 6; index++) {
        if (*(unsigned char *)(p + index + 34)) {
            p += index + 34;
            { scsi_mark_invalid_field(p); return; }
        }
    }
    win_params.b13.f.b0 = 1;
    if (side == 0 || (side == 1 && scsi_win_saved_descriptors[0] == SCSI_WIN_ID_FRONT)) {
        if (win_params.paper_size.f.auto_length) {
            win_params.paper_size.v = 0;
            win_params.paper_size.f.auto_length = 1;
        } else
            win_params.paper_size.v = 0;
        win_params.paper_size.f.given = 1;
        win_params.side[0].use_window_area = 1;
        win_params.side[1].use_window_area = 1;
        win_params.paper_size.f.paper_code = 3;
    }
    window->gamma_pattern.v = 0;
    win_params.flags.f.source_given = 1;
    window->dtc_options = SCSI_WIN_DTC_OPTIONS_DEFAULT; /* default of descriptor bytes 47-48 (DTC options) */
    window->outline.v = 0;
    window->outline.f.given = 1;
    window->emphasis.v = 0;
    window->emphasis.f.given = 1;
    window->mirror.v = 0;
    window->mirror.f.given = 1;
    win_scanning_order.b4 = 1;
    win_scanning_order.b3 = 0;
    if ((unsigned int)scsi_win_current_end.v == p + 40) {
        scsi_param_valid.v = 1;
        return;
    } else if ((unsigned int)scsi_win_current_end.v > p + 40) {
    } else {
        p += 40;
        { scsi_mark_invalid_field(p); return; }
    }
    if (*(unsigned char *)(p + 40) == SCSI_WIN_VENDOR_MONO || *(unsigned char *)(p + 40) == SCSI_WIN_VENDOR_COLOR) {
    } else {
        p += 40;
        { scsi_mark_invalid_field(p); return; }
    }
    if (*(unsigned char *)(p + 40) == SCSI_WIN_VENDOR_COLOR) {
        if ((unsigned int)scsi_win_current_end.v <= p + 41) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 41);
        if ((field_byte & 0x80) == 0) {
            if (field_byte != 0)
                expected = 3;
        } else
            expected = 0x81;
        window->lut_pattern.v = field_byte;
        if (field_byte > expected) {
            p += 41;
            { scsi_mark_invalid_field(p); return; }
        }
        if ((unsigned int)scsi_win_current_end.v <= p + 42) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 42); /* scanning order: 0 line, 1 dot */
        if (field_byte == 1) {
            win_scanning_order.b4 = 0;
            win_scanning_order.b3 = 1;
        } else if (field_byte == 0) {
            win_scanning_order.b4 = 1;
            win_scanning_order.b3 = 0;
        } else {
            p += 42;
            { scsi_mark_invalid_field(p); return; }
        }
        if ((unsigned int)scsi_win_current_end.v <= p + 43) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 43);
        if ((settings_emulation[0] & 0x1c) == 0x11)
            field_byte = 0;
        if (field_byte != 0) {
            p += 43;
            { scsi_mark_invalid_field(p); return; }
        }
        if ((unsigned int)scsi_win_current_end.v <= p + 44) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 44);
        if (field_byte != 0) {
            p += 44;
            { scsi_mark_invalid_field(p); return; }
        }
        if ((unsigned int)scsi_win_current_end.v <= p + 45) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 45);
        if (field_byte != 0) {
            p += 44;
            { scsi_mark_invalid_field(p); return; }
        }
        if ((unsigned int)scsi_win_current_end.v <= p + 46) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 46); /* colour emphasis */
        window->emphasis.v = field_byte;
        if ((unsigned int)scsi_win_current_end.v <= p + 47) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 47);
        field_byte = 0;
        window->mirror.v = field_byte;
        if ((unsigned int)scsi_win_current_end.v <= p + 48) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 48);
        if (field_byte != 0) {
            p += 48;
            { scsi_mark_invalid_field(p); return; }
        }
        if ((unsigned int)scsi_win_current_end.v <= p + 49) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 49);
        if (field_byte != 0) {
            p += 49;
            { scsi_mark_invalid_field(p); return; }
        }
        if ((unsigned int)scsi_win_current_end.v <= p + 50) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 50); /* white level follow: 0x80 on, 0xC0 off */
        if (field_byte & SCSI_WIN_WHITE_LEVEL_FOLLOW_ON) {
            if (field_byte & 0x40)
                window->white_level_follow.f.off = 1;
            else
                window->white_level_follow.f.off = 0;
        }
        win_white_level_follow = field_byte;
        if ((unsigned int)scsi_win_current_end.v <= p + 50) {
            scsi_param_valid.v = 1;
            return;
        }
        if ((unsigned int)scsi_win_current_end.v <= p + 52) {
            scsi_param_valid.v = 1;
            return;
        }
        field_word = util_get_be16((unsigned char *)(p + 51));
        if (field_word != 0) {
            p += 51;
            { scsi_mark_invalid_field(p); return; }
        }
        if ((unsigned int)scsi_win_current_end.v <= p + 53) {
            scsi_param_valid.v = 1;
            return;
        }
        while (1) {
            if (side == 1 && scsi_win_saved_descriptors[0] == SCSI_WIN_ID_BACK) {
                if ((unsigned int)scsi_win_current_end.v <= (field_end = p + 54)) {
                    scsi_param_valid.v = 1;
                    return;
                }
                break;
            }
            field_byte = *(unsigned char *)(p + 53);
            if (field_byte == 0) {
                if ((unsigned int)scsi_win_current_end.v <= (field_end = p + 54)) {
                    scsi_param_valid.v = 1;
                    return;
                }
                for (index = 0; index < 8; index++) {
                    if (*(unsigned char *)(p + index + 54) != 0) {
                        p = field_end;
                        { scsi_mark_invalid_field(p); return; }
                    }
                }
                break;
            }
            expected = field_byte;
            field_byte &= 0xc0; /* paper selection: 0x80 standard size, 0xC0 custom size at bytes 54/58 */
            if (field_byte == 0x80) {
                field_byte = expected;
                field_byte &= 0x1f;
                if (field_byte & 0x10)
                    win_params.paper_size.f.landscape = 1;
                else
                    win_params.paper_size.f.landscape = 0;
                win_params.paper_size.f.paper_code = field_byte & 15;
                for (index = 0; scsi_win_paper_sizes[index].paper_code != 0; index++) {
                    if (field_byte == scsi_win_paper_sizes[index].paper_code) {
                        field_value = scsi_win_paper_sizes[index].width_inch1200;
                        length_inch1200 = scsi_win_paper_sizes[index].length_inch1200;
                        break;
                    }
                }
                if (scsi_win_paper_sizes[index].paper_code == 0) {
                    p += 53;
                    { scsi_mark_invalid_field(p); return; }
                }
                if (scsi_win_check_centered_area(field_value, side) == 0) {
                    p += 53;
                    { scsi_mark_invalid_field(p); return; }
                }
                if (scsi_win_check_max_length(length_inch1200) == 0) {
                    p += 53;
                    { scsi_mark_invalid_field(p); return; }
                }
                if ((unsigned int)scsi_win_current_end.v <= (field_end = p + 54)) {
                    scsi_param_valid.v = 1;
                    return;
                }
                break;
            }
            if (field_byte == 0xc0) {
                field_byte = expected;
                field_byte &= 0x1f;
                if (field_byte & 0x10)
                    win_params.paper_size.f.landscape = 1;
                else
                    win_params.paper_size.f.landscape = 0;
                win_params.paper_size.f.paper_code = field_byte & 15;
                win_params.paper_size.f.custom_size = 1;
                field_end = p + 62;
                if ((unsigned int)scsi_win_current_end.v < field_end) {
                    p = field_end;
                    { scsi_mark_invalid_field(p); return; }
                } else {
                    field_byte = expected;
                    field_value = util_get_be32((unsigned char *)(p + 54));
                    if (field_byte & 0x20) {
                        p += 54;
                        { scsi_mark_invalid_field(p); return; }
                    }
                    win_params.paper_width_inch1200.v = field_value;
                    if (scsi_win_check_centered_area(field_value, side) == 0) {
                        p += 54;
                        { scsi_mark_invalid_field(p); return; }
                    }
                    field_value = util_get_be32((unsigned char *)(p + 58));
                    if (field_byte & 0x20) {
                        p += 58;
                        { scsi_mark_invalid_field(p); return; }
                    }
                    win_params.paper_length_inch1200.v = field_value;
                    if (scsi_win_check_max_length(field_value) == 0) {
                        p += 58;
                        { scsi_mark_invalid_field(p); return; }
                    } else
                        break;
                }
                continue;
            } else {
                p += 53;
                { scsi_mark_invalid_field(p); return; }
            }
        }
        if ((unsigned int)scsi_win_current_end.v <= p + 63) {
            scsi_param_valid.v = 1;
            return;
        }
        field_byte = *(unsigned char *)(p + 62);
        field_word = util_get_be16((unsigned char *)(p + 62));
        if (field_word != 0) {
            p += 62;
            { scsi_mark_invalid_field(p); return; }
        }
        return;
    }
    if ((unsigned int)scsi_win_current_end.v <= p + 41) {
        scsi_param_valid.v = 1;
        return;
    }
    field_byte = *(unsigned char *)(p + 41);
    if ((field_byte & 0x80) == 0) {
        if (field_byte != 0) {
            if (window->composition.f.multi_level)
                expected = 3;
            else {
                field_byte--;
                expected = 2;
            }
        }
    } else
        expected = 0x87;
    window->gamma_pattern.v = field_byte;
    if (field_byte > expected) {
        p += 41;
        { scsi_mark_invalid_field(p); return; }
    }
    if ((unsigned int)scsi_win_current_end.v <= p + 42) {
        scsi_param_valid.v = 1;
        return;
    }
    field_byte = *(unsigned char *)(p + 42); /* outline extraction: 0 or 0x80 */
    window->outline.v = field_byte;
    if (field_byte != 0 && (field_byte & 0x7f) != 0) {
        p += 42;
        { scsi_mark_invalid_field(p); return; }
    }
    if ((unsigned int)scsi_win_current_end.v <= p + 43) {
        scsi_param_valid.v = 1;
        return;
    }
    field_byte = *(unsigned char *)(p + 43);
    window->emphasis.v = field_byte;
    if ((unsigned int)scsi_win_current_end.v <= p + 44) {
        scsi_param_valid.v = 1;
        return;
    }
    field_byte = *(unsigned char *)(p + 44); /* image area separation: 0x40, or 0x80 (by the IPC option when fitted) */
    if (field_byte != 0 && field_byte != 0x40 && field_byte != 0x80) {
        p += 44;
        { scsi_mark_invalid_field(p); return; }
    }
    if (field_byte == 0x80) {
        if (!hw_option_flags.f.b1)
            window->composition.f.area_separation = 1;
        else
            window->composition.f.area_separation_ipc = 1;
    } else if (field_byte == 0x40)
        window->composition.f.area_separation = 1;
    if ((unsigned int)scsi_win_current_end.v <= p + 45) {
        scsi_param_valid.v = 1;
        return;
    }
    field_byte = *(unsigned char *)(p + 45);
    field_byte = 0;
    window->mirror.v = field_byte;
    if ((unsigned int)scsi_win_current_end.v <= p + 46) {
        scsi_param_valid.v = 1;
        return;
    }
    field_byte = *(unsigned char *)(p + 46); /* variance (SDTC), quantised to 8 steps */
    if (field_byte == 0)
        field_byte = 0;
    else if (field_byte <= 0x3f)
        field_byte = 1;
    else if (field_byte <= 0x5f)
        field_byte = 2;
    else if (field_byte <= 0x7f)
        field_byte = 3;
    else if (field_byte <= 0x9f)
        field_byte = 4;
    else if (field_byte <= 0xbf)
        field_byte = 5;
    else if (field_byte <= 0xdf)
        field_byte = 6;
    else if (field_byte <= 0xff)
        field_byte = 7;
    window->binarize.f.variance = field_byte;
    if ((unsigned int)scsi_win_current_end.v <= p + 48) {
        scsi_param_valid.v = 1;
        return;
    }
    field_word = util_get_be16((unsigned char *)(p + 47));
    window->dtc_options = field_word;
    if ((unsigned int)scsi_win_current_end.v <= p + 49) {
        scsi_param_valid.v = 1;
        return;
    }
    if ((unsigned int)scsi_win_current_end.v <= p + 50) {
        scsi_param_valid.v = 1;
        return;
    }
    field_byte = *(unsigned char *)(p + 50); /* white level follow: 0x80 on, 0xC0 off */
    if (field_byte & SCSI_WIN_WHITE_LEVEL_FOLLOW_ON) {
        if (field_byte & 0x40)
            window->white_level_follow.f.off = 1;
        else
            window->white_level_follow.f.off = 0;
    }
    win_white_level_follow = field_byte;
    if ((unsigned int)scsi_win_current_end.v <= p + 52) {
        scsi_param_valid.v = 1;
        return;
    }
    field_word = util_get_be16((unsigned char *)(p + 51));
    if ((unsigned int)scsi_win_current_end.v <= p + 53) {
        scsi_param_valid.v = 1;
        return;
    }
    while (1) {
        if (side == 1 && scsi_win_saved_descriptors[0] == SCSI_WIN_ID_BACK) {
            if ((unsigned int)scsi_win_current_end.v <= (field_end = p + 54)) {
                scsi_param_valid.v = 1;
                return;
            }
            break;
        }
        field_byte = *(unsigned char *)(p + 53);
        if (field_byte == 0) {
            if ((unsigned int)scsi_win_current_end.v <= (field_end = p + 54)) {
                scsi_param_valid.v = 1;
                return;
            }
            for (index = 0; index < 8; index++) {
                if (*(unsigned char *)(p + index + 54) != 0) {
                    p = field_end;
                    { scsi_mark_invalid_field(p); return; }
                }
            }
            break;
        }
        expected = field_byte;
        field_byte &= 0xc0; /* paper selection: 0x80 standard size, 0xC0 custom size at bytes 54/58 */
        if (field_byte == 0x80) {
            field_byte = expected;
            field_byte &= 0x1f;
            if (field_byte & 0x10)
                win_params.paper_size.f.landscape = 1;
            else
                win_params.paper_size.f.landscape = 0;
            win_params.paper_size.f.paper_code = field_byte & 15;
            for (index = 0; scsi_win_paper_sizes[index].paper_code != 0; index++) {
                if (field_byte == scsi_win_paper_sizes[index].paper_code) {
                    field_value = scsi_win_paper_sizes[index].width_inch1200;
                    length_inch1200 = scsi_win_paper_sizes[index].length_inch1200;
                    break;
                }
            }
            if (scsi_win_paper_sizes[index].paper_code == 0) {
                p += 53;
                { scsi_mark_invalid_field(p); return; }
            }
            if (scsi_win_check_centered_area(field_value, side) == 0) {
                p += 53;
                { scsi_mark_invalid_field(p); return; }
            }
            if (scsi_win_check_max_length(length_inch1200) == 0) {
                p += 53;
                { scsi_mark_invalid_field(p); return; }
            }
            if ((unsigned int)scsi_win_current_end.v <= (field_end = p + 54)) {
                scsi_param_valid.v = 1;
                return;
            }
            break;
        }
        if (field_byte == 0xc0) {
            field_byte = expected;
            field_byte &= 0x1f;
            if (field_byte & 0x10)
                win_params.paper_size.f.landscape = 1;
            else
                win_params.paper_size.f.landscape = 0;
            win_params.paper_size.f.paper_code = field_byte & 15;
            win_params.paper_size.f.custom_size = 1;
            field_end = p + 62;
            if ((unsigned int)scsi_win_current_end.v < field_end) {
                p = field_end;
                { scsi_mark_invalid_field(p); return; }
            } else {
                field_byte = expected;
                field_value = util_get_be32((unsigned char *)(p + 54));
                if (field_byte & 0x20) {
                    p += 54;
                    { scsi_mark_invalid_field(p); return; }
                }
                win_params.paper_width_inch1200.v = field_value;
                if (scsi_win_check_centered_area(field_value, side) == 0) {
                    p += 54;
                    { scsi_mark_invalid_field(p); return; }
                }
                field_value = util_get_be32((unsigned char *)(p + 58));
                if (field_byte & 0x20) {
                    p += 58;
                    { scsi_mark_invalid_field(p); return; }
                }
                win_params.paper_length_inch1200.v = field_value;
                if (scsi_win_check_max_length(field_value) == 0) {
                    p += 58;
                    { scsi_mark_invalid_field(p); return; }
                } else
                    break;
            }
            continue;
        } else {
            p += 53;
            { scsi_mark_invalid_field(p); return; }
        }
    }
    if ((unsigned int)scsi_win_current_end.v <= p + 62) {
        scsi_param_valid.v = 1;
        return;
    }
    field_byte = *(unsigned char *)(p + 62); /* IPC mode */
    if ((field_byte & 0x3f) != 0 || field_byte == (SCSI_WIN_IPC_DTC | SCSI_WIN_IPC_SDTC)) {
        p += 62;
        { scsi_mark_invalid_field(p); return; }
    }
    if (field_byte == SCSI_WIN_IPC_DTC) {
        window->binarize.f.dtc = 1;
        window->outline.v = 0;
    }
    if (field_byte == SCSI_WIN_IPC_SDTC) {
        window->binarize.f.sdtc = 1;
        window->binarize.c.lo = 0;
        window->outline.v = 0;
    }
    if (field_byte == SCSI_WIN_IPC_DTC && !hw_option_flags.f.b1 && !window->composition.f.halftone && !window->composition.f.multi_level) {
        p += 62;
        { scsi_mark_invalid_field(p); return; }
    }
    if ((unsigned int)scsi_win_current_end.v <= p + 63) {
        scsi_param_valid.v = 1;
        return;
    } else if (*(unsigned char *)(p + 63) == 0) {
        scsi_param_valid.v = 1;
        return;
    } else {
        p += 63;
        { scsi_mark_invalid_field(p); return; }
    }
}

int scsi_win_check_resolution(unsigned short dpi, unsigned short front_dpi, unsigned char side)
{
    if (dpi == 0) {
        dpi = 400;
        if (side == 1 && scsi_win_saved_descriptors[0] == SCSI_WIN_ID_BACK)
            dpi = front_dpi;
    }
    if (dpi < 50 || dpi > 800) {
        scsi_win_res_valid = 0;
        return 0;
    }
    scsi_win_res_valid = 1;
    scsi_win_res_dpi = dpi;
    return 1;
}

int scsi_win_check_centered_area(unsigned int paper_width, int side)
{
    unsigned int right_edge;
    right_edge = 0x36d2;
    if (paper_width > right_edge)
        paper_width = right_edge;
    right_edge = right_edge - paper_width;
    right_edge = right_edge >> 1;
    right_edge += win_params.side[side].x_offset_inch1200 + win_params.side[side].width_inch1200;
    if ((settings_test_flags[0] & 0x80) || (settings_emulation[0] & 0x1c) == 0x14) {
        if (right_edge > 15000)
            return 0;
    } else {
        if (right_edge > 0x3900)
            return 0;
    }
    return 1;
}

int scsi_win_check_max_length(unsigned int length)
{
    if (length > 0x60c0) {
    }
    if (length > 0x5fcd0)
        return 0;
    return 1;
}
