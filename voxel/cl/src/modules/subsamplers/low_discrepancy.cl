/* OpenCL 1.2 --- modules/subsamplers/low_discrepancy.cl       IMPLEMENTATION */

#include <modules/subsampler.cl>

size_t sample_count(void)
{
    return ORDER;
}

float2 sample(size_t index)
{
    const float2 samples[ORDER] = {SAMPLES};
    return samples[index]; /* precomputed */
}
