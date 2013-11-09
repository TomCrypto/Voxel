#pragma once

/* Default flat-color integrator, which returns white if and only if
 * the ray hits something (anything) and black otherwise. */

template <typename Geometry>
class FlatIntegrator
{
public:
	FlatIntegrator(const Geometry &geometry)
	  : geometry(geometry)
	{
	}

	math::float3 integrate(const math::float3 &origin, const math::float3 &direction) const
	{
		float distance;
		Contact contact;

		if (geometry.traverse(origin, direction, distance, contact))
			return math::float3(1, 1, 1);
		else
			return math::float3(0, 0, 0);
	}

private:
	const Geometry &geometry;
};
