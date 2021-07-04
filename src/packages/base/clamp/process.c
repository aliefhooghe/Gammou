
#include <math_utils.h>

void node_process(float in, float limit_min, float limit_max, float *out)
{
    *out = clamp(in, limit_min, limit_max);
}