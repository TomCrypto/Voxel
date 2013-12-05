/* OpenCL 1.2 --- modules/projections/projection.cl            IMPLEMENTATION */

#include <modules/projection.cl>

struct Ray project(constant struct Observer *observer,
                   float u, float v, float ratio)
{
    float3 origin = get_position(observer);

    u = u * ratio - (ratio - 1) * 0.5f;

    float3 dir = normalize(get_focal_plane(observer, u, v) - origin);

    return (struct Ray){origin, dir};
}
