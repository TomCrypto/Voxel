/* OpenCL 1.2 --- modules/integrators/ao.cl                    IMPLEMENTATION */

#include <modules/integrator.cl>

float3 integrate(struct Ray ray, global struct SVO_NODE *geometry,
                 struct PRNG *prng)
{
    float distance;

    if (traverse(geometry, ray, INFINITY, &distance))
    {
        advance(&ray, distance - 1e-3f, ray.d);

        float2 pt = rand2(prng) * (float2)(M_PI * 2, M_PI);

        float cosX, cosY;
        float sinX = sincos(pt.x, &cosX);
        float sinY = sincos(pt.y, &cosY);

        ray.d = (float3)(sinY * cosX,
                         cosY,
                         sinY * sinX);

        if (!occludes(geometry, ray, INFINITY)) return (float3)(1, 1, 1);
    }

    return (float3)(0, 0, 0);
}
