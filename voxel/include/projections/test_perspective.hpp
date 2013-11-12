#pragma once

#include "math/vector3.hpp"
#include "math/vector4.hpp"
#include "math/matrix3x3.hpp"

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

    math::float3x3 view(left, up, forward);

    math::float3 base_dir(ratio * -u, -v, 1.0f / tan(fov / 2));

    math::float3 rotated_dir = view * normalize(base_dir);

    origin = camera_pos;
    direction = rotated_dir;
}
