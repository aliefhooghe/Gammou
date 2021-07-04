
void node_process(float in_m, float in_p, float *out_p, float *read, float *out_m)
{
    *out_p = in_p;
    *read = in_p + in_m;
    *out_m = in_m;
}
