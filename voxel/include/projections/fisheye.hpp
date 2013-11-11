#pragma once

/* Test fisheye perspective. Does not handle aspect ratio (obviously). */

#include "math/vector3.hpp"

#include <cmath>

void project_fisheye(float u, float v, float /*ratio*/, math::float3 &origin, math::float3 &direction)
{
    float phi = M_PI * (0.5f - u);
    float theta = M_PI_2 * (1 + v); // voodoo magic
    
    direction = math::float3(sin(theta) * cos(phi),
                             cos(theta),
                             sin(theta) * sin(phi));

    origin = math::float3(0, -0.4f, 0);
}
