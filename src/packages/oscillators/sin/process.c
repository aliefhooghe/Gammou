
#define PI 3.141592653589793f

static inline float my_sin(float x)
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

void node_process(void *state, float freq, float p, float *out)
{
    float *phase = (float*)state;
    *out = my_sin(*phase + p);

    *phase += (freq / 48000.f) * PI * 2.f;
    if (*phase > PI)
        *phase -= PI * 2.f;
}