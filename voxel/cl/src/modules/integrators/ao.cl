/* OpenCL 1.2 --- modules/integrators/ao.cl                    IMPLEMENTATION */

#include <modules/integrator.cl>

float3 integrate(struct Ray ray, global struct Geometry *geometry,
                 struct PRNG *prng)
{
    float distance;
    Hit_Info hit_info;

    if (intersects(geometry, ray, INFINITY, &distance, &hit_info))
    {
        advance(&ray, distance, transform(cosine(prng), hit_info.basis));
        if (!occludes(geometry, ray, INFINITY)) return (float3)(1, 1, 1);
    }

    return (float3)(0, 0, 0);
}
