/* OpenCL 1.2 --- modules/integrators/normal.cl                IMPLEMENTATION */

#include <modules/integrator.cl>

float3 integrate(struct Ray ray, global struct Geometry *geometry,
                 struct PRNG *prng)
{
    float depth;
    Hit_Info info;

    if (intersects(geometry, ray, INFINITY, &depth, &info))
    {
        return (info.basis.n * 0.5f + 0.5f) * (0.5f - depth / 3);
    }
    else
    {
        return (float3)(0, 0, 0);
    }
}
