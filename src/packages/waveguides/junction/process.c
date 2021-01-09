

void node_process(float in_m, float refl, float in_p, float *out_p, float *out_m)
{
    const float tmp = refl * (in_p - in_m);
    *out_p = in_p + tmp;
    *out_m = in_m + tmp;
}
