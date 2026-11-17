#include "fw.h"

short img_spline_eval(unsigned x, struct img_spline_point *points, unsigned count, double *moments)
{
    struct img_spline_point *list;
    double dx;
    unsigned lo;
    unsigned hi;
    unsigned mid;
    double dist;
    double step;
    double result;

    list = points;
    dx = x;
    lo = 0;
    hi = count - 1;
    while (lo < hi) {
        mid = (hi + lo) >> 1;
        if (list[mid].x < dx)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (lo > 0)
        lo--;
    step = (double)list[lo + 1].x + lo - list[lo].x + 1.0;
    dist = dx - list[lo].x;
    result = ((moments[lo + 1] - moments[lo]) * dist / step + moments[lo] * 3.0) * dist;
    result += ((double)list[lo + 1].y - list[lo].y) / step;
    result -= (moments[lo] * 2.0 + moments[lo + 1]) * step;
    result *= dist;
    result += list[lo].y;
    return result;
}
