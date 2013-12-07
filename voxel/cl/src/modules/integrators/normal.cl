/* OpenCL 1.2 --- modules/integrators/depth.cl                 IMPLEMENTATION */

#include <modules/integrator.cl>

float3 integrate(struct Ray ray, global struct Geometry *geometry,
                 struct PRNG *prng)
{
    float depth;
    Contact contact;

    if (depth_test(geometry, ray, INFINITY, &depth, &contact))
    {
        return contact.normal * 0.5f + 0.5f;
    }
    else
    {
        return (float3)(0, 0, 0);
    }
}
