#include "fw.h"
#pragma section H

void testmode_read_exposure(void)
{
    err_code_list[0] = calib_black_level[0][0] >> 8;
    testmode_result_1[0] = (unsigned char)calib_black_level[0][0];
    testmode_result_2[0] = calib_black_level[0][1] >> 8;
    testmode_result_3[0] = ((union word_bytes *)&calib_black_level[0][1])->b[1];
    testmode_result_4[0] = calib_black_level[0][2] >> 8;
    testmode_result_5[0] = ((union word_bytes *)&calib_black_level[0][2])->b[1];
    testmode_result_6[0] = calib_black_level[1][0] >> 8;
    testmode_result_7[0] = ((union word_bytes *)&calib_black_level[1][0])->b[1];
    testmode_result_8[0] = calib_black_level[1][1] >> 8;
    testmode_result_9[0] = ((union word_bytes *)&calib_black_level[1][1])->b[1];
    testmode_result_10[0] = calib_black_level[1][2] >> 8;
    testmode_result_11[0] = ((union word_bytes *)&calib_black_level[1][2])->b[1];
    return;
}
