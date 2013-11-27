/* OpenCL 1.2 --- modules/subsamplers/none.cl                  IMPLEMENTATION */

#include <modules/subsampler.cl>

size_t sample_count(void)
{
    return 1;
}

float2 sample(size_t index)
{
    return (float2)(0, 0);
}
