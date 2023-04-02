#include <cmath>

namespace ecas {

float celt_exp2(float x)
{
    int integer;
    float frac;
    union {
        float f;
        int i;
    } res;
    integer = (int)(floorf(x));
    if (integer < -50)
        return 0;
    frac = x - integer;
    /* K0 = 1, K1 = log(2), K2 = 3-4*log(2), K3 = 3*log(2) - 2 */
    res.f = 0.99992522f + frac * (0.69583354f
        + frac * (0.22606716f + 0.078024523f * frac));
    res.i = (res.i + (integer << 23)) & 0x7fffffff;
    return res.f;
}

float ecas_expf(float x) { 
    return celt_exp2(x*1.44269504f); // faster.
}

// Carm
float ecas_sqrtf(float x) {
    union {
        int intPart;
        float floatPart;
    } convertor;
    union {
        int intPart;
        float floatPart;
    } convertor2;
    convertor.floatPart = x;
    convertor2.floatPart = x;
    convertor.intPart = 0x1FBCF800 + (convertor.intPart >> 1);
    convertor2.intPart = 0x5f3759df - (convertor2.intPart >> 1);
    return 0.5f * (convertor.floatPart + (x * convertor2.floatPart));
}

}  // end of namespace ecas