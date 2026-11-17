#include "fw.h"

void img_spline_prepare(struct img_spline_point *points, int count, double *moments)
{
    int num;
    double step[10];
    double diag[10];
    int i;
    int j;
    int k;
    struct img_spline_point *list;
    double factor;

    list = points;
    num = count;
    moments[0] = 0.0;
    moments[num - 1] = 0.0;
    for (i = 0; i < num - 1; i++) {
        j = i + 1;
        step[i] = (double)list[j].x - list[i].x;
        diag[j] = ((double)list[j].y - list[i].y) / step[i];
    }
    factor = step[0] * moments[0];
    moments[1] = diag[2] - diag[1] - factor;
    diag[1] = (list[2].x - list[0].x) * 2.0;
    for (i = 1; i < num - 2; i++) {
        j = i + 1;
        k = i + 2;
        factor = step[i] / diag[i];
        moments[j] = diag[k] - diag[j];
        moments[j] -= moments[i] * factor;
        diag[j] = ((double)list[k].x - list[i].x) * 2.0 - step[i] * factor;
    }
    moments[num - 2] -= step[num - 2] * moments[num - 1];
    for (i = num - 2; i > 0; i--) {
        factor = moments[i] - moments[i + 1] * step[i];
        moments[i] = factor / diag[i];
    }
}
