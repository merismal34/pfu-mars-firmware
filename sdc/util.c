#include "fw.h"

void util_mem_copy_to_odd(void *dst, void *src, int len)
{
    rt_mem_copy_to_odd(dst, src, len);
}

void util_mem_copy_from_odd(int dst, int src, int len)
{
    rt_mem_copy_from_odd(dst, src, len);
}

int util_mem_equal(void *first, void *second, int len)
{
    return rt_mem_equal(first, second, len);
}

int util_mem_compare_count(int first, int second, int len, int count)
{
    rt_mem_compare_count(first, second, len, count);
    return;
}

int util_mem_all_equal(void *area, unsigned char value, int len, void *count)
{
    return rt_mem_all_equal(area, value, len, count);
}

void util_mem_set(void *dst, unsigned char value, int len)
{
    rt_mem_set(dst, value, len);
}

char util_sign_mag8_to_int(unsigned char value)
{
    if (value >= 128) {
        value &= 127;
        value = ~value + 1;
    }
    return value;
}

char util_sign_mag8_to_int_2(unsigned char value)
{
    if (value >= 128) {
        value &= 127;
        value = ~value + 1;
    }
    return value;
}

unsigned short util_get_be16(unsigned char *p)
{
    return (p[0] << 8) | p[1];
}

unsigned long util_get_be32(unsigned char *p)
{
    return (unsigned short)((p[2] << 8) | p[3]) | ((unsigned short)((p[0] << 8) | p[1]) << 16);
}

void util_put_be16(unsigned char *p, unsigned short value)
{
    p[0] = value >> 8;
    p[1] = value;
}

int util_abs16(int value)
{
    if (value & 0x8000)
        return ~value + 1;
    return value;
}

char util_read8(char *p)
{
    return *p;
}

void util_write8(char *p, char value)
{
    *p = value;
}

short util_read16(short *p)
{
    return *p;
}

void util_write16(short *p, short value)
{
    *p = value;
}
