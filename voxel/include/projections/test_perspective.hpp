#pragma once

#include "math/vector3.hpp"

void project_perspective(float u, float v, math::float3 *origin, math::float3 *direction)
{
    *origin = math::float3(0, -0.4f, 0);
    *direction = normalize(math::float3(u / 0.65f, -v / 0.65f, 1));
}
