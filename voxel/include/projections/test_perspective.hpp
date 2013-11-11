#pragma once

#include "math/vector3.hpp"

// this handles aspect ratio

void project_perspective(float u, float v, float ratio, math::float3 &origin, math::float3 &direction)
{
    origin = math::float3(0, -0.4f, 0);
    direction = normalize(math::float3(ratio * u / 0.65f, -v / 0.65f, 1));
}
