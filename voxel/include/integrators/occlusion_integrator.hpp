#pragma once

#include "math/vector3.hpp"

#include "contact.hpp"

#include <algorithm>

/* Occlusion test integrator, which returns white if the occlusion
 * test encounters an obstacle, and black otherwise. */

template <typename GeometryTy>
math::float3 integrate_occlusion(const GeometryTy &geometry,
                                 const math::float3 &origin,
                                 const math::float3 &direction)
{
	if (!geometry.occludes(origin, direction))
	{
	    return math::float3(0, 0, 0);
	}
	else
	{
	    return math::float3(1, 1, 1);
	}
}
