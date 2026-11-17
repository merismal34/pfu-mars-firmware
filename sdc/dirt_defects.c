#include "fw.h"
#pragma section H

void dirt_check_defect_lists(void)
{
    unsigned short *p;
    unsigned short i;

    for (i = 1; i < 128; i++) {
        if (dirt_defects_flatbed[0][i - 1] >= dirt_defects_flatbed[0][i]) {
            if (dirt_defects_flatbed[0][i] != 0xffff) {
                dirt_defects_present = 0;
                return;
            }
        }
        if (dirt_defects_flatbed[1][i - 1] >= dirt_defects_flatbed[1][i]) {
            if (dirt_defects_flatbed[1][i] != 0xffff) {
                dirt_defects_present = 0;
                return;
            }
        }
        if (dirt_defects_flatbed[2][i - 1] >= dirt_defects_flatbed[2][i]) {
            if (dirt_defects_flatbed[2][i] != 0xffff) {
                dirt_defects_present = 0;
                return;
            }
        }
        if (flash_defect_area[i - 1] >= flash_defect_area[i]) {
            if (flash_defect_area[i] != 0xffff) {
                dirt_defects_present = 0;
                return;
            }
        }
        if (dirt_defects_adf_front_1[i - 1] >= dirt_defects_adf_front_1[i]) {
            if (dirt_defects_adf_front_1[i] != 0xffff) {
                dirt_defects_present = 0;
                return;
            }
        }
        if (dirt_defects_adf_front_2[i - 1] >= dirt_defects_adf_front_2[i]) {
            if (dirt_defects_adf_front_2[i] != 0xffff) {
                dirt_defects_present = 0;
                return;
            }
        }
        if (dirt_defects_adf_back[0][i - 1] >= dirt_defects_adf_back[0][i]) {
            if (dirt_defects_adf_back[0][i] != 0xffff) {
                dirt_defects_present = 0;
                return;
            }
        }
        if (dirt_defects_adf_back[1][i - 1] >= dirt_defects_adf_back[1][i]) {
            if (dirt_defects_adf_back[1][i] != 0xffff) {
                dirt_defects_present = 0;
                return;
            }
        }
        if (dirt_defects_adf_back[2][i - 1] >= dirt_defects_adf_back[2][i]) {
            if (dirt_defects_adf_back[2][i] != 0xffff) {
                dirt_defects_present = 0;
                return;
            }
        }
    }
    dirt_defects_present = 0;
    i = 0x480;
    p = dirt_defects_last_word;
    while (i != 0) {
        if (*p != 0xffff) {
            dirt_defects_present = 1;
            break;
        }
        i--;
        p--;
    }
}
