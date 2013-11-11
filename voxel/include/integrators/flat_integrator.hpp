#pragma once

#include "math/vector3.hpp"

#include "contact.hpp"

#include <algorithm>

/* Default flat-color integrator, which returns the object's color if
 * and only if the ray hits something, and returns black otherwise. */

template <typename GeometryTy>
math::float3 integrate_flat(const GeometryTy &geometry,
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
	    return contact.rgb;
	}
}
