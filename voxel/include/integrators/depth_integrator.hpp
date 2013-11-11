#pragma once

#include "math/vector3.hpp"

#include "contact.hpp"

#include <algorithm>

/* Depth integrator, which returns a grayscale color corresponding to
 * the distance of the objects from the camera (a depth map). The
 * depth map is not normalized. */

template <typename GeometryTy>
math::float3 integrate_depth(const GeometryTy &geometry,
                             const math::float3 &origin,
                             const math::float3 &direction)
{
	float distance;
	Contact contact;

	if (!geometry.traverse(origin, direction, distance, contact))
	{
	    return math::float3(0, 0, 0);
	}
	else
	{
	    return math::float3(1, 1, 1) * distance / 2;
	}
}
