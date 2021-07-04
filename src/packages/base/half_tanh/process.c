
#include <math_utils.h>

void node_process(float in, float *out)
{
    *out = in > 0.f ? fast_tanh(in) : in;
}