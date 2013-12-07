/* OpenCL 1.2 --- modules/integrators/ao.cl                    IMPLEMENTATION */

#include <modules/integrator.cl>

float3 integrate(struct Ray ray, global struct Geometry *geometry,
                 struct PRNG *prng)
{
    float distance;
    struct Hit_Info hit;

    if (intersects(geometry, ray, INFINITY, &distance, &hit))
    {
        advance(&ray, distance, transform(cosine(prng), hit.basis));
        if (!occludes(geometry, ray, INFINITY)) return C_WHITE;
    }

    return C_BLACK;
}
