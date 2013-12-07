/* OpenCL 1.2 --- modules/integrators/normal.cl                IMPLEMENTATION */

#include <modules/integrator.cl>

float3 integrate(struct Ray ray, global struct Geometry *geometry,
                 struct PRNG *prng)
{
    float depth;
    struct Hit_Info hit;

    if (intersects(geometry, ray, INFINITY, &depth, &hit))
    {
        return (hit.basis.n * 0.5f + 0.5f) * (0.5f - depth / 3);
    }

    return C_BLACK;
}
