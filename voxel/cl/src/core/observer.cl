/* OpenCL 1.2 --- core/observer.cl                             IMPLEMENTATION */

#include <core/observer.cl>
#include <core/math_lib.cl>

struct Observer
{
    float3 pos, dir;
    float3 plane[4];

    float yaw, pitch;
    float fov;

    // other stuff here
};

float3 get_position(constant struct Observer *observer)
{
    return observer->pos;
}

float3 get_direction(constant struct Observer *observer)
{
    return observer->dir;
}

float get_fov(constant struct Observer *observer)
{
    return observer->fov;
}

float get_yaw(constant struct Observer *observer)
{
    return observer->yaw;
}

float get_pitch(constant struct Observer *observer)
{
    return observer->pitch;
}

float3 get_focal_plane(constant struct Observer *observer, float u, float v)
{
    return lerp(lerp(observer->plane[3], observer->plane[2], u),
                lerp(observer->plane[0], observer->plane[1], u), v);
}
