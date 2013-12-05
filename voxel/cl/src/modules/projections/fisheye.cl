/* OpenCL 1.2 --- modules/projections/fisheye.cl               IMPLEMENTATION */

#include <modules/projection.cl>

struct Ray project(constant struct Observer *observer,
                   float u, float v, float ratio)
{
    // WORK IN PROGRESS - DOES NOT WORK PROPERLY

    u = 1 - 2 * u;
    v = 1 - 2 * v;

    // u, v = cos(angle), sin(angle)
    // project the vertical point

    float z = sqrt(1 - u * u - v * v);

    // deduce the angles
    float yaw = atan2(v, u);
    float pitch = acos(z);

    // account for rotation
    yaw += get_yaw(observer);
    pitch += get_pitch(observer);

    // project camera ray
    float cosX, cosY;
    float sinX = sincos(yaw, &cosX);
    float sinY = sincos(pitch, &cosY);

    float3 dir = (float3)(sinY * cosX,
                          cosY,
                          sinY * sinX);

    return (struct Ray){get_position(observer), dir};
}
