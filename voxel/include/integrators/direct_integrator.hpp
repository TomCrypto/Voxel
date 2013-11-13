#pragma once

#include "math/vector3.hpp"

#include "contact.hpp"

#include <algorithm>

/* Direct lighting integrator, which can handle one-bounce lighting
 * and direct, point-light shadowing. Only one light source supported. */

template <typename GeometryTy>
math::float3 integrate_direct(const GeometryTy &geometry,
                              const math::float3 &origin,
                              const math::float3 &direction)
{
	float distance;
	Contact contact, dummy;

	if (geometry.traverse(origin, direction, distance, contact))
	{
	    // compute hit point (push back outside the object a little)
	    math::float3 hit = origin + direction * distance + 1e-5f * contact.normal;
	    float ambient = 0.25f;
	    bool light_hit = false;
	    
	    // can we hit the light from here? if not, shadow
	    math::float3 dir_to_light = geometry.light() - hit;
	    float distance_to_light = length(dir_to_light);
	    dir_to_light = normalize(dir_to_light);
	    
	    light_hit = !geometry.occludes(hit, dir_to_light, distance_to_light);
	    if (!light_hit) return ambient * contact.rgb;
	    else
	    {
	        float diffuse = std::max(0.0f, dot(contact.normal, dir_to_light)) * 0.85f / pow(distance_to_light, 2) * 0.10f;
	        
	        return (ambient + diffuse) * contact.rgb;
	    }
		
    }
	else return math::float3(0, 0, 0); // no intersection
}
