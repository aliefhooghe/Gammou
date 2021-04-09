
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

float absolute(float x)
{
    const unsigned int bits = 0x7fffffffu & (*(unsigned int*)&x);
    return *(float*)&bits;
}