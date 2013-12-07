/* OpenCL 1.2 --- modules/integrators/depth.cl                 IMPLEMENTATION */

#include <modules/integrator.cl>

float3 integrate(struct Ray ray, global struct Geometry *geometry,
                 struct PRNG *prng)
{
    float depth;

    if (intersects(geometry, ray, INFINITY, &depth, 0))
    {
        return (float3)(0.25, 0.75, 0.25) * (0.5f - depth / 3);
    }

    return C_BLACK;
}
