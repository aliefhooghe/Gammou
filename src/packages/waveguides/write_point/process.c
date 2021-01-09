
void node_process(float in_m, float write, float in_p, float *out_p, float *out_m)
{
    *out_p = in_p + write / 2.f;
    *out_m = in_m + write / 2.f;
}
