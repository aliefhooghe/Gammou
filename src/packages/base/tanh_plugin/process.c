
#include "../common_libs/math_utils.h"

void node_process(void *state, float in, float *out)
{
    *out = fast_tanh(in);
}