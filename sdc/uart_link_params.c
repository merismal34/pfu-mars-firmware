#include "fw.h"
#pragma section H

int link_param_x_check_22(int *more, unsigned char *param, unsigned char *left, int mode)
{
    unsigned short x_dpi;
    unsigned short y_dpi;
    x_dpi = (param[0] << 8) | param[1];
    y_dpi = (param[2] << 8) | param[3];
    if (mode == 1 && (x_dpi || y_dpi)) {
        return 8;
    }
    if (x_dpi == 200 || x_dpi == 240 || x_dpi == 300 || x_dpi == 400) {
    } else {
        return 8;
    }
    if (y_dpi == 50 || y_dpi == 100 || y_dpi == 150 || y_dpi == 200 || y_dpi == 240 || y_dpi == 300 || y_dpi == 400 || y_dpi == 600) {
    } else {
        return 8;
    }
    *left -= 4;
    if (*left) {
        return link_param_error();
    }
    *more = 0;
    return 0;
}

int link_param_error()
{
    return 8;
}

int link_param_reject(int *more, int param, int left, int mode)
{
    return link_param_error();
}

void link_cmd_set_params_z(void)
{
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
    if (scsi_param_buf[0] < 4) {
        err = 8;
        goto end;
    }
    if (link_no_page_started() == 0) {
        err = 0x80;
        goto end;
    }
    mode = scsi_param_byte_2[0] & 0x40;
    first_param = scsi_param_byte_2[0] & 0x3f;
    if (first_param > 8) {
        err = 8;
        goto end;
    }
    left = cnt = (unsigned char)(scsi_param_buf[0] + 253);
    src = scsi_param_byte_3;
    while (flag != 0 && !err) {
        off = cnt - left;
        idx = first_param + off;
        err = link_param_z_checks[idx](&flag, src + off, &left, mode);
    }
end:
    if (err != 0) {
        err_set_illegal_request(err);
        link_reply_4();
        return;
    }
    link_reply_2();
    return;
    if (1) ;
    data = scsi_param_byte_3;
    if ((mode & 0x40) == 0) {
        dst = ((unsigned char *)link_window_mode_ptrs[0]) + first_param;
    }
    else {
        dst = link_z_params_ptrs[0] + first_param;
    }
    util_mem_copy(dst, data, scsi_param_buf[0] - 3);
    if ((settings_byte_0x33[0] & 1) && (scsi_param_byte_2[0] & 0xc0) == 0) {
        dst = link_z_params + first_param;
        util_mem_copy(dst, data, scsi_param_buf[0] - 3);
    }
    if ((mode & 0x40) == 0) {
        if (link_apply_z_params(0) == 0) {
            err_set_illegal_request(err);
            link_reply_4();
            return;
        }
        if ((settings_byte_0x33[0] & 1) && link_apply_z_params(1) == 0) {
            err_set_illegal_request(err);
            link_reply_4();
            return;
        }
    }
    else {
        if (link_apply_z_params(1) == 0) {
            err_set_illegal_request(err);
            link_reply_4();
            return;
        }
    }
    link_reply_2();
}

extern int _builtin_get_imask(void);
extern void _builtin_set_imask(int);
struct link_mode_view {
    char pad_00;
    struct {
        unsigned char p:2,
                      reverse_mirror_given:1,
                      reverse_on:1,
                      mirror_on:1,
                      separation_given:1,
                      separation_on:1,
                      separation_mode:1;
    } image_bits;
    struct {
        unsigned char given:1,
                      outline_on:1,
                      level_given:1,
                      p:2,
                      lo:3;
    } emphasis_bits;
    char pad_03[2];
    unsigned char zoom_high, zoom_low, x_zoom_high, x_zoom_low;
};
union link_mode_word {
    unsigned short w;
    struct {
        unsigned short h:4,
                       zoom_given:1,
                       :1,
                       x_zoom_given:1,
                       lo:9;
    } f;
};

#define VIEW(type, x) (*(type *)&(x))

int link_apply_z_params(int side)
{
    int saved_imask;
    struct link_mode_view *link_mode;
    struct win_side *window;
    union link_mode_word mode_word;
    unsigned short word;
    unsigned char index;
    int x_dpi;
    int y_dpi;
    int ok;

    saved_imask = _builtin_get_imask();
    link_mode = (struct link_mode_view *)((unsigned char *)link_window_mode_ptrs[side]);
    window = &win_params.side[side];
    ok = 1;
    _builtin_set_imask(15);
    word = window->emphasis.v;
    if (link_mode->emphasis_bits.given) {
        VIEW(union win_emphasis_word, word).f.given = 1;
        window->emphasis.c.lo = 0;
        if (link_mode->emphasis_bits.level_given) {
            index = link_mode->emphasis_bits.lo;
            VIEW(union win_emphasis_word, word).c.lo = link_z_value_table[index];
        } else {
            VIEW(union win_emphasis_word, word).c.lo = 0;
        }
    } else {
        VIEW(union win_emphasis_word, word).f.given = 0;
    }
    window->emphasis.v = word;
    _builtin_set_imask(saved_imask);
    window->emphasis.c.lo = VIEW(union win_emphasis_word, word).c.lo;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    if (link_mode->emphasis_bits.given) {
        window->outline.f.given = 1;
        if (link_mode->emphasis_bits.outline_on) {
            window->outline.f.on = 1;
        } else {
            window->outline.f.on = 0;
        }
    } else {
        window->outline.f.given = 0;
    }
    _builtin_set_imask(saved_imask);
    link_set_window_mode(side);
    _builtin_set_imask(15);
    word = window->composition.v;
    VIEW(struct win_composition_bits, word).b12 = 0;
    VIEW(struct win_composition_bits, word).separation_given = 0;
    if (link_mode->image_bits.separation_given) {
        VIEW(struct win_composition_bits, word).separation_given = 1;
        VIEW(struct win_composition_bits, word).area_separation_ipc = 0;
        VIEW(struct win_composition_bits, word).area_separation = 0;
        if (link_mode->image_bits.separation_on) {
            if (window->binarize.f.given && !window->binarize.f.dtc && !window->binarize.f.sdtc && !window->outline.f.on) {
                VIEW(struct win_composition_bits, word).halftone = 1;
            }
            if (!hw_option_flags.f.b1 || link_mode->image_bits.separation_mode) {
                VIEW(struct win_composition_bits, word).area_separation = 1;
            } else {
                VIEW(struct win_composition_bits, word).area_separation_ipc = 1;
            }
            VIEW(struct win_composition_bits, word).error_diffusion = 0;
        } else {
            if (link_window_params_ptrs[side]->image_flags.f.halftone_given && !link_window_params_ptrs[side]->image_flags.f.halftone_on) {
                VIEW(struct win_composition_bits, word).halftone = 0;
            }
        }
    } else {
        if (VIEW(struct win_composition_bits, word).halftone_given) {
            VIEW(struct win_composition_bits, word).area_separation_ipc = 0;
            VIEW(struct win_composition_bits, word).area_separation = 0;
        }
    }
    if (link_mode->image_bits.separation_mode) {
        VIEW(struct win_composition_bits, word).b12 = 1;
    }
    window->composition.v = word;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    word = window->reverse_image.v;
    if (link_mode->image_bits.reverse_mirror_given) {
        VIEW(union win_reverse_image_word, word).f.given = 1;
    } else {
        window->reverse_image.f.given = 0;
    }
    if (link_mode->image_bits.reverse_on) {
        VIEW(union win_reverse_image_word, word).f.on = 1;
    } else {
        VIEW(union win_reverse_image_word, word).f.on = 0;
    }
    window->reverse_image.v = word;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    word = window->composition.v;
    window->composition.v = word;
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    if (side == 0) {
        mode_word.w = link_mode->zoom_low | (link_mode->zoom_high << 8);
        word = win_params.resolution_mode.v;
        VIEW(union win_resolution_mode_word, word).f.x_zoom_given = 0;
        if (mode_word.f.zoom_given) {
            VIEW(union win_resolution_mode_word, word).f.zoom_given = 1;
            win_params.y_zoom_percent.v = mode_word.f.lo;
            if (mode_word.f.x_zoom_given) {
                VIEW(union win_resolution_mode_word, word).f.x_zoom_given = 1;
                win_params.x_zoom_percent.v = (link_mode->x_zoom_low | (link_mode->x_zoom_high << 8)) & 0x1ff;
            } else {
                win_params.x_zoom_percent.v = win_params.y_zoom_percent.v;
            }
        } else {
            VIEW(union win_resolution_mode_word, word).f.zoom_given = 0;
            if (mode_word.f.x_zoom_given) {
                VIEW(union win_resolution_mode_word, word).f.x_zoom_given = 1;
            }
        }
        word = img_resolution_codes[(unsigned char)(((char *)&win_params)[15] & 7)];
        x_dpi = win_params.x_zoom_percent.v * word;
        x_dpi = rt_divu16(x_dpi + 99, 100);
        if (x_dpi == 50 || x_dpi == 100 || x_dpi == 150 || x_dpi == 200 || x_dpi == 240 || x_dpi == 300 || x_dpi == 400 || x_dpi == 600) {
            y_dpi = win_params.y_zoom_percent.v * word;
            y_dpi = rt_divu16(y_dpi + 99, 100);
            if (y_dpi == 50 || y_dpi == 100 || y_dpi == 150 || y_dpi == 200 || y_dpi == 240 || y_dpi == 300 || y_dpi == 400 || y_dpi == 600) {
                win_params.resolution_mode.v = word;
            } else {
                ok = 0;
            }
        } else {
            ok = 0;
        }
    }
    _builtin_set_imask(saved_imask);
    _builtin_set_imask(15);
    word = window->mirror.v;
    VIEW(union win_mirror_word, word).f.given = 0;
    if (link_mode->image_bits.reverse_mirror_given) {
        VIEW(union win_mirror_word, word).f.given = 1;
        if (link_mode->image_bits.mirror_on) {
            VIEW(union win_mirror_word, word).f.on = 1;
        } else {
            VIEW(union win_mirror_word, word).f.on = 0;
        }
    } else {
        VIEW(union win_mirror_word, word).f.given = 0;
    }
    window->mirror.v = word;
    _builtin_set_imask(saved_imask);
    return ok;
}

int link_param_z_check_0(int *more, int param, unsigned char *left, int mode)
{
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_z_check_1(int *more, unsigned char *param, unsigned char *left, int mode)
{
    struct Bits *flags;
    flags = (struct Bits *)(param + 1);
    if (*left < 3 || (*param & 0x40)) {
        return 8;
    }
    if ((param[1] == 24) & 24) {
        return 8;
    }
    if (flags->b7 && flags->b6 && flags->b5) {
        return 8;
    }
    *left -= 3;
    if (*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_z_check_4(int *more, int param, unsigned char *left, int mode)
{
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_z_check_5(int *more, unsigned char *param, unsigned char *left, int mode)
{
    union link_mode_word mode_word;
    unsigned short y_dpi;

    if (mode == 0) {
        mode_word.w = (param[0] << 8) | param[1];
        if (mode_word.f.zoom_given && mode_word.f.x_zoom_given) {
            if (*left < 4) {
                return 8;
            }
        } else {
            if (*left < 2) {
                return 8;
            }
        }
        if (mode_word.f.zoom_given) {
            if (mode_word.f.lo < 25 || mode_word.f.lo > 400) {
                return 8;
            }
        }
    }
    *left -= 2;
    if (!*left) {
        *more = 0;
        return 0;
    }
    if (mode == 0) {
        if (mode_word.f.zoom_given) {
            if (mode_word.f.x_zoom_given) {
                y_dpi = ((param[2] << 8) | param[3]) & 0x1ff;
                if (y_dpi < 25 || y_dpi > 400) {
                    return 8;
                }
            }
        }
    }
    *left -= 2;
    if (*left) {
        *more = 0;
        return 0;
    }
    *more = 0;
    return 0;
}

int link_param_reject_2(int *more, int param, int left, int mode)
{
    return link_param_error();
}

int link_param_x80_check_0(int *more, unsigned char *param, unsigned char *left, int mode)
{
    if (*param & 7) {
        return 8;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x80_check_1(int *more, unsigned char *param, unsigned char *left, int mode)
{
    if (*param & 0xf9) {
        return 8;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x80_check_2(int *more, unsigned char *param, unsigned char *left, int mode)
{
    if (*param & 0x80) {
        switch (*param & 0xf0) {
        case 0x90:
        case 0xa0:
        case 0xb0:
            break;
        default:
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

int link_param_x80_check_3(int *more, unsigned char *param, unsigned char *left, int mode)
{
    if (*param & 0x20) {
        return 8;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x80_check_4(int *more, unsigned char *param, unsigned char *left, int mode)
{
    if (*param & 0x3f) {
        return 8;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x80_check_5(int *more, char *param, unsigned char *left, int mode)
{
    if (*param) {
        return 8;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x80_check_6(int *more, int param, unsigned char *left, int mode)
{
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x80_check_7(int *more, unsigned char *param, unsigned char *left, int mode)
{
    if (*param & 0x80) {
        return 8;
    }
    if (*left < 2) {
        return link_param_error();
    }
    *left -= 2;
    if (*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x80_check_9(int *more, unsigned char *param, unsigned char *left, int mode)
{
    if (hw_hostif_flags.f.cga_test_scan) {
        if ((*param & 0x3c) || (*param & 3) == 1 || (*param & 0xc0) == 0xc0) {
            return 8;
        }
    } else {
        if ((*param & 0x3c) || (*param & 3) == 1 || (*param & 3) == 2 || (*param & 0xc0) == 0xc0) {
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

int link_param_x80_check_10(int *more, char *param, unsigned char *left, int mode)
{
    if (*param) {
        return 8;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_x80_check_11(int *more, int param, unsigned char *left, int mode)
{
    if (--*left) {
        return link_param_error();
    }
    *more = 0;
    return 0;
}

int link_param_y_check_0(int *more, char *param, unsigned char *left, int mode)
{
    if (*param) {
        return 8;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_y_check_1(int *more, char *param, unsigned char *left, int mode)
{
    if (*param) {
        return 8;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_y_check_2(int *more, unsigned char *param, unsigned char *left, int mode)
{
    if ((*param & 0x83) || (*param & 0x60) == 0x60) {
        return 8;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_y_check_3(int *more, unsigned char *param, unsigned char *left, int mode)
{
    unsigned char value;
    if (*param & 0x80) {
        value = *param & 0x70;
        value >>= 4;
        if (value > 1) {
            return 8;
        }
    }
    if (*param & 0x0f) {
        return 8;
    }
    if (--*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}


int link_param_y_check_4(int *more, unsigned char *param, unsigned char *left, int mode)
{
    if (hw_hostif_flags.f.cga_test_scan) {
        *more = 0;
        return 0;
    }
    else if (param[0] > 100 || param[1] > 100 || param[2] > 100 || param[3] > 100) {
        return 8;
    }
    if (*left < 3) {
        return link_param_error();
    }
    *left -= 3;
    if (*left) {
        *more = 1;
    } else {
        *more = 0;
    }
    return 0;
}

int link_param_y_check_7(int *more, int param, unsigned char *left, int mode)
{
    if (--*left != 0)
        return link_param_error();
    *more = 0;
    return 0;
}
