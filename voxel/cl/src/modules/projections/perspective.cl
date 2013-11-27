/* OpenCL 1.2 --- modules/projections/projection.cl            IMPLEMENTATION */

#include <modules/projection.cl>

struct Ray project(constant struct OBSERVER *observer,
                   float u, float v, float ratio)
{
    float3 origin = observer->pos;
    
    float3 dir = normalize(lerp(lerp(observer->plane[0], observer->plane[1], u),
                                lerp(observer->plane[3], observer->plane[2], u), v));
                                
    return (struct Ray){origin, dir};
}
