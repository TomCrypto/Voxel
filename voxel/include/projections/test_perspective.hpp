#pragma once

#include "math/vector3.hpp"
#include "math/matrix4x4.hpp"

// this handles aspect ratio

void project_perspective(float u, float v, float ratio, math::float3 &origin, math::float3 &direction)
{
    // HARDCODED (observer settings) //

    const math::float3 camera_pos(0, -0.4f, 0);

    const math::float3 camera_dir(0.1, 0.2, 1); // or target, if it's easier

    const float fov = 100 * (M_PI / 180.0f); // Field of View (in radians)

    // END HARDCODED //

    // compute an orthonormal basis for camera space
    math::float3 up_dir(0, 1, 0); // change if we implement camera roll
    math::float3 forward = normalize(camera_dir);
    math::float3 left = normalize(cross(up_dir, forward));
    math::float3 up = normalize(cross(forward, left));

    //printf("up: (%.2f, %.2f, %.2f)\n", up.x, up.y, up.z);
    //printf("forward: (%.2f, %.2f, %.2f)\n", forward.x, forward.y, forward.z);
    //printf("left: (%.2f,  %.2f, %.2f)\n", left.x, left.y, left.z);

    math::float4x4 view; // ok, let's hope this works out

    view[0] = left.x;
    view[1] = up.x;
    view[2] = forward.x;
    view[3] = 0;

    view[4] = left.y;
    view[5] = up.y;
    view[6] = forward.y;
    view[7] = 0;

    view[8] = left.z;
    view[9] = up.z;
    view[10] = forward.z;
    view[11] = 0;

    view[12] = 0;
    view[13] = 0;
    view[14] = 0;
    view[15] = 1;

    math::float3 base_dir(ratio * u, -v, 1.0f / tan(fov / 2));
    math::float3 rotated_dir = view * normalize(base_dir);

    //printf("New direction: (%.2f, %.2f, %.2f)\n", rotated_dir.x, rotated_dir.y,
    //        rotated_dir.z);

    origin = camera_pos;
    direction = rotated_dir;

    //origin = math::float3(0, -0.4f, 0);
    //direction = normalize(math::float3(ratio * u / 0.65f, -v / 0.65f, 1));
}
