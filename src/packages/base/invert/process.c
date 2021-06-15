
#include <synthesizer_def.h>
#include "../common_libs/math_constant.h"
#include "../common_libs//math_utils.h"

/**
 *  \brief out = 1/invert
 */
void node_process(float in, float *out)
{
    const float epsilon = 0.00001;
    *out = 1 / exclude_zero(in, epsilon);
}