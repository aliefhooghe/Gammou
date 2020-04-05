
#include "../common_libs/math_utils.h"
#include "../common_libs/math_constant.h"

void node_process(void *state, float in, float cutoff_freq, float q, float *out)
{
    float *filter_state = (float*)state; // = [Xn-1, Xn-2, Xn-3, Yn-1, Yn-2, Yn-3]

    const float dt = 1.f / 48000.f;
    const float Q = q / 128.f;
    const float omega = 2.f * PI * clamp(cutoff_freq, 1.f, 18000.f);

    const float a = omega * omega * dt * dt * dt;
	const float tmp = 4.f * omega * Q * dt;
	const float b = dt * (tmp + 4.f);
	const float c = dt * (tmp - 4.f);

    *out =
		(
			a * (filter_state[2] + 3.f * (filter_state[1] + filter_state[0]) + in) -
			(
				(a - c) * filter_state[5] +
				(3.f * a - b) * filter_state[4] +
				(c + 3.f * a) * filter_state[3]))
		/ (a + b);

    filter_state[5] = filter_state[4];
    filter_state[4] = filter_state[3];
    filter_state[3] = *out;
    filter_state[2] = filter_state[1];
    filter_state[1] = filter_state[0];
    filter_state[0] = in;
}