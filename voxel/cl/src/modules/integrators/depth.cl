/* OpenCL 1.2 --- modules/integrators/depth.cl                 IMPLEMENTATION */

#include <modules/integrator.cl>

float3 integrate(struct Ray ray, global struct SVO_NODE *geometry,
                 struct PRNG *prng)
{
    float depth;

    if (depth_test(geometry, ray, INFINITY, &depth))
    {
        return (float3)(0.25, 0.75, 0.25) * (0.5f - depth / 3);
    }
    else
    {
        return (float3)(0, 0, 0);
    }
}
