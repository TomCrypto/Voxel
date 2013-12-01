/* OpenCL 1.2 --- modules/integrators/ao.cl                    IMPLEMENTATION */

#include <modules/integrator.cl>

float3 integrate(struct Ray ray, global struct SVO_NODE *geometry,
                 struct PRNG *prng)
{
    float distance;

    if (traverse(geometry, ray, INFINITY, &distance))
    {
        advance(&ray, distance - 1e-3f, ray.d);

        size_t total = 1, hits = 0;

        for (size_t t = 0; t < total; ++t)
        {
            float2 pt = rand2(prng) * (float2)(M_PI * 2, M_PI);

            float cosX, cosY;
            float sinX = sincos(pt.x, &cosX);
            float sinY = sincos(pt.y, &cosY);

            float3 new_dir = (float3)(sinY * cosX,
                                      cosY,
                                      sinY * sinX);

            ray.d = new_dir;

            if (!occludes(geometry, ray, INFINITY))
                hits++;
        }

        return (float3)(1, 1, 1) * ((float)hits / total * 1.8f);
    }

    return (float3)(0, 0, 0);
}
