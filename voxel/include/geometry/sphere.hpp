#pragma once

/* A basic geometry class which simply tests against a single sphere, for
 * testing purposes only. */

#include "math/vector3.hpp"
#include <cmath>

#include <algorithm>

struct Contact
{
    bool hit;
};

class Sphere
{
public:
	bool traverse(const math::float3 &origin, const math::float3 &direction,
		      float &distance, Contact &contact) const
	{
	    distance = ray_sphere(origin, direction);
	    return contact.hit = distance > 0;
	}

private:
	static float ray_sphere(const math::float3 &origin,
	                        const math::float3 &direction)
	{
	    const math::float3 sphere_pos(0, 0, 2);
	    const float radius2 = 1 * 1;

	    math::float3 org = sphere_pos - origin;
	    float b = dot(org, direction);
	    float det = b * b - dot(org, org) + radius2;

	    if (det >= 0.0f) {
		float l = sqrt(det);
		float p = b - l, q = b + l;
		if (p < 1e-5f) return q;
		if (q < 1e-5f) return p;
		return std::min(p, q);
	    }
	    return -1.0f;
	}
};
