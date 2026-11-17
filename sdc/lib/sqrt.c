/* sqrt() of the C library: the soft-float core works on an internal three-word format; NaN
 * results set errno to EDOM, infinities to ERANGE. */
typedef struct { unsigned long s_e, m_h, m_l; } IN_DOUBLE;   /* sign|exponent, mantissa */
extern IN_DOUBLE _dtoid(double);
extern IN_DOUBLE _in_sqrtd(IN_DOUBLE);
extern double    _idtod(IN_DOUBLE);
extern volatile int _errno;

#define EDOM     1101
#define ERANGE   1100
#define EXMASKD  0x7ff00000L
#define MHMASKD  0x000fffffL
#define MLMASKD  0xffffffffL
#define isEMAXd(p) (((*(long *)&p) & EXMASKD) == EXMASKD)
#define isMANTd(p) (((*(long *)&p) & MHMASKD) || ((*((long *)&p + 1)) & MLMASKD))

double sqrt(p)
double p;
{
    double ret;
    ret = _idtod(_in_sqrtd(_dtoid(p)));
    if (isEMAXd(ret)) {
        if (isMANTd(ret)) {
            _errno = EDOM;
        }
        else {
            _errno = ERANGE;
        }
    }
    return ret;
}
