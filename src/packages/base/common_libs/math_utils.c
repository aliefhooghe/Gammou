
#include "math_utils.h"

float fast_sin(float x)
{
    return x * (
        1.f - x * x *
        (
            (1.f / 6.f) - x * x *
            (
                (1.f / 120.f) - x * x *
                (
                    (1.f / 5040.f)
                    - (x * x / 362880.f)
                )
            )
        )
    );
}

float fast_tanh(float x)
{
    const float x2 = x*x;
    return ((((x2 + 378.f) * x2 + 17325.f) * x2 + 135135.f) * x) /
            (((28.f * x2 + 3150.f) * x2 + 62370.f) * x2 + 135135.f);
}

float clamp(float x, float min, float max)
{
    const float a = x < min ? min : x;
    return a > max ? max : a;
}

float exclude_zero(float x, float epsilon)
{
    if (x < -epsilon || x > epsilon)
        return x;
    else
        return copy_sign(x, epsilon);
}

float absolute(float x)
{
    const unsigned int bits = 0x7fffffffu & (*(unsigned int*)&x);
    return *(const float*)&bits;
}

float copy_sign(float from, float to)
{
    const unsigned int bits =
        0x80000000u & (*(unsigned int*)&from) & // select sign
        0x7fffffffu & (*(unsigned int*)&to);    // absolute(to
    return *(const float*)&bits;
}

unsigned int linear_congruential_step(unsigned int last)
{
    return 1664525u * last + 1013904223u;
}
